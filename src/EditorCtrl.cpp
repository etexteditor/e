/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#include "EditorCtrl.h"
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <algorithm>
#include "EditorFrame.h"
#include "StyleRun.h"
#include "eApp.h"
#include <wx/tipwin.h>
#include <wx/file.h>
#include "FindCmdDlg.h"
#include "RunCmdDlg.h"
#include "doc_byte_iter.h"
#include "BundleMenu.h"
#include "GutterCtrl.h"
#include "PreviewDlg.h"
#include "RedoDlg.h"
#include "CompletionPopup.h"
#include "MultilineDataObject.h"
#include "EditorBundlePanel.h"
#include <wx/tokenzr.h>
#include "jsonreader.h"
#include "eDocumentPath.h"

#ifdef __WXMSW__
    #include "CygwinDlg.h"
	#include <wx/msw/registry.h>
#endif

// id's
enum {
	TIMER_FOLDTOOLTIP = 100
};

BEGIN_EVENT_TABLE(EditorCtrl, wxControl)
	EVT_PAINT(EditorCtrl::OnPaint)
	EVT_CHAR(EditorCtrl::OnChar)
	EVT_KEY_DOWN(EditorCtrl::OnKeyDown)
	EVT_KEY_UP(EditorCtrl::OnKeyUp)
	EVT_SIZE(EditorCtrl::OnSize)
	EVT_ERASE_BACKGROUND(EditorCtrl::OnEraseBackground)
	EVT_LEFT_DOWN(EditorCtrl::OnMouseLeftDown)
	EVT_RIGHT_DOWN(EditorCtrl::OnMouseRightDown)
	EVT_LEFT_UP(EditorCtrl::OnMouseLeftUp)
	EVT_LEFT_DCLICK(EditorCtrl::OnMouseDClick)
	EVT_MOTION(EditorCtrl::OnMouseMotion)
	EVT_MOUSE_CAPTURE_LOST(EditorCtrl::OnMouseCaptureLost)
	EVT_ENTER_WINDOW(EditorCtrl::OnMouseMotion)
	EVT_LEAVE_WINDOW(EditorCtrl::OnLeaveWindow)
	EVT_MOUSEWHEEL(EditorCtrl::OnMouseWheel)
	EVT_SCROLLWIN(EditorCtrl::OnScroll)
	EVT_IDLE(EditorCtrl::OnIdle)
	EVT_CLOSE(EditorCtrl::OnClose)
	EVT_MENU_RANGE(1000, 1999, EditorCtrl::OnPopupListMenu)
	EVT_TIMER(TIMER_FOLDTOOLTIP, EditorCtrl::OnFoldTooltipTimer)
	EVT_SET_FOCUS(EditorCtrl::OnFocus)
END_EVENT_TABLE()

// Initialize statics
const unsigned int EditorCtrl::m_caretWidth = 2;
unsigned long EditorCtrl::s_ctrlDownTime = 0;
bool EditorCtrl::s_altGrDown = false;
wxString EditorCtrl::s_bashCmd;
wxString EditorCtrl::s_bashEnv;

/// Open a page saved from a previous session
EditorCtrl::EditorCtrl(const int page_id, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size)
	: m_catalyst(cw), m_doc(cw), dispatcher(cw.GetDispatcher()), m_syntaxHandler(((eApp*)wxTheApp)->GetSyntaxHandler()), m_enableDrawing(false), m_isResizing(true),
	  m_lines(mdc, m_doc, *this), bitmap(bitmap), scrollPos(0), m_scrollPosX(0), topline(-1), commandMode(false), m_theme(((eApp*)wxTheApp)->GetSyntaxHandler().GetTheme()),
      m_parentFrame(parentFrame), m_changeToken(0), m_savedForPreview(false), m_bundlePanel(NULL),
      m_modCallback(NULL), m_scrollCallback(NULL), m_foldTooltipTimer(this, TIMER_FOLDTOOLTIP), m_activeTooltip(NULL),
	  lastpos(0), m_doubleClickedLine(-1), m_currentSel(-1), m_search_hl_styler(m_doc, m_lines, m_searchRanges), m_syntaxstyler(m_doc, m_lines),
 	  do_freeze(true), m_options_cache(0), m_re(NULL), m_symbolCacheToken(0)
{
	Create(parent, wxID_ANY, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	Hide(); // start hidden to avoid flicker
	Init();
	wxString mirrorPath;
	doc_id di;
	int newpos;
	int topline;
	wxString syntax;
	vector<unsigned int> folds;
	vector<unsigned int> bookmarks;
	cxLOCK_READ(m_catalyst)
		// Retrieve the page info
		wxASSERT(page_id >= 0 && page_id < catalyst.GetPageCount());
		catalyst.GetPageSettings(page_id, mirrorPath, di, newpos, topline, syntax, folds, bookmarks);
	cxENDLOCK
	const bool isBundleItem = m_parentFrame.IsBundlePath(mirrorPath);

	if (m_parentFrame.IsRemotePath(mirrorPath)) {
		// If the mirror points to a remote file, we have to download it first.
		SetDocument(di, mirrorPath);
	}
	else {
		if (isBundleItem) {
			const PListHandler& plistHandler = m_syntaxHandler.GetPListHandler();
			m_bundleType = plistHandler.GetBundleTypeFromUri(mirrorPath);
			m_remotePath = mirrorPath;
		}
		else m_path = mirrorPath;

		cxLOCKDOC_WRITE(m_doc)
			// Set the document & settings
			doc.SetDocument(di);
			doc.SetDocRead();
		cxENDLOCK
		m_lines.ReLoadText();
	}

	// Set the syntax to match the new path
	if (syntax.empty()) {
		m_syntaxstyler.UpdateSyntax();
		FoldingInvalidate(); // Init Folding
	}
	else SetSyntax(syntax);

	// Set lines & positions
	m_lines.SetPos(newpos);
	scrollPos = m_lines.GetYPosFromLine(topline);

	// Fold lines that was folded in previous session
	if (!folds.empty()) {
		// We have to make sure all text is syntaxed
		// and all fold markers found
		m_syntaxstyler.ParseAll();
		UpdateFolds();

		for (vector<unsigned int>::const_iterator p = folds.begin(); p != folds.end(); ++p) {
			const unsigned int line_id = *p;
			const cxFold target(line_id);
			vector<cxFold>::iterator f = lower_bound(m_folds.begin(), m_folds.end(), target);
			if (f != m_folds.end() && f->line_id == line_id && f->type == cxFOLD_START) {
				Fold(*p);
			}
		}
	}

	// Set bookmarks
	for (vector<unsigned int>::const_iterator p = bookmarks.begin(); p != bookmarks.end(); ++p) {
		AddBookmark(*p);
	}
}

/// Open a document
EditorCtrl::EditorCtrl(const doc_id di, const wxString& mirrorPath, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size)
	: m_catalyst(cw), m_doc(cw), dispatcher(cw.GetDispatcher()), m_syntaxHandler(((eApp*)wxTheApp)->GetSyntaxHandler()), m_enableDrawing(false), m_isResizing(true),
	  m_lines(mdc, m_doc, *this), bitmap(bitmap), scrollPos(0), m_scrollPosX(0), topline(-1), commandMode(false), m_theme(((eApp*)wxTheApp)->GetSyntaxHandler().GetTheme()),
      m_parentFrame(parentFrame), m_changeToken(0), m_savedForPreview(false), m_bundlePanel(NULL),
      m_modCallback(NULL), m_scrollCallback(NULL), m_foldTooltipTimer(this, TIMER_FOLDTOOLTIP), m_activeTooltip(NULL),
	  lastpos(0), m_doubleClickedLine(-1), m_currentSel(-1), m_search_hl_styler(m_doc, m_lines, m_searchRanges), m_syntaxstyler(m_doc, m_lines),
 	  do_freeze(true), m_options_cache(0), m_re(NULL), m_symbolCacheToken(0)
{
	Create(parent, wxID_ANY, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	Init();

	cxLOCKDOC_WRITE(m_doc)
		// Set the document & settings
		doc.SetDocument(di);
		doc.SetDocRead();
	cxENDLOCK
	m_path = mirrorPath;

	// Set lines & positions
	m_lines.ReLoadText();

	// Set the syntax to match the new document
	m_syntaxstyler.UpdateSyntax();

	// Init Folding
	FoldingInvalidate();
}

/// Create a new empty document
EditorCtrl::EditorCtrl(CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size)
	: m_catalyst(cw), m_doc(cw, true), dispatcher(cw.GetDispatcher()), m_syntaxHandler(((eApp*)wxTheApp)->GetSyntaxHandler()), m_enableDrawing(false), m_isResizing(true),
	  m_lines(mdc, m_doc, *this), bitmap(bitmap), scrollPos(0), m_scrollPosX(0), topline(-1), commandMode(false), m_theme(((eApp*)wxTheApp)->GetSyntaxHandler().GetTheme()),
      m_parentFrame(parentFrame), m_changeToken(0), m_savedForPreview(false), m_bundlePanel(NULL),
      m_modCallback(NULL), m_scrollCallback(NULL), m_foldTooltipTimer(this, TIMER_FOLDTOOLTIP), m_activeTooltip(NULL),
	  lastpos(0), m_doubleClickedLine(-1), m_currentSel(-1), m_search_hl_styler(m_doc, m_lines, m_searchRanges), m_syntaxstyler(m_doc, m_lines),
 	  do_freeze(true), m_options_cache(0), m_re(NULL), m_symbolCacheToken(0)
{
	Create(parent, wxID_ANY, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	Hide(); // start hidden to avoid flicker

	cxLOCKDOC_WRITE(m_doc)
		// Make sure we start out with a single empty revision
		doc.Freeze();
	cxENDLOCK

	Init();

	// Init Folding
	FoldingInvalidate();
}

void EditorCtrl::Init() {
	// Initialize the memoryDC for dubblebuffering
	mdc.SetFont(m_theme.font);
	// We do not yet call mdc.SelectObject(bitmap) since the bitmap
	// is shared with the other EditCtrl's. We call it in Show() & OnSize().

	m_remoteProfile = NULL;
	m_search_hl_styler.Init(*this);

	// Column selection state
	m_blockKeyState = BLOCKKEY_NONE;
	m_selMode = SEL_NORMAL;

	// Settings
	m_doAutoPair = true;
	m_doAutoWrap = true;
	m_wrapAtMargin = false;
	bool doShowMargin = false;
	int marginChars = 80;
	cxLOCK_READ(m_catalyst)
		catalyst.GetSettingBool(wxT("autoPair"), m_doAutoPair);
		catalyst.GetSettingBool(wxT("autoWrap"), m_doAutoWrap);
		catalyst.GetSettingBool(wxT("showMargin"), doShowMargin);
		catalyst.GetSettingBool(wxT("wrapMargin"), m_wrapAtMargin);
		catalyst.GetSettingInt(wxT("marginChars"), marginChars);
	cxENDLOCK
	m_lastScopePos = -1; // scope selection
	if (!doShowMargin) m_wrapAtMargin = false;

	// Initialize gutter (line numbers)
	m_gutterCtrl = new GutterCtrl(*this, wxID_ANY);
	m_gutterCtrl->Hide();
	m_showGutter = false;
	m_gutterLeft = true; // left side is default
	m_gutterWidth = 0;
	SetShowGutter(m_parentFrame.IsGutterShown());

	// To keep track of when we should freeze versions
	lastpos = 0;
	lastaction = ACTION_INSERT;

	// Initialize tooltips
	//m_revTooltip.Create(this);

	// resize the bitmap used for doublebuffering
	wxSize size = GetClientSize();
	if (bitmap.GetWidth() < size.x || bitmap.GetHeight() < size.y) {
		bitmap = wxBitmap(size.x, size.y);
	}

	// Init the lines
	m_lines.SetWordWrap(m_parentFrame.GetWrapMode());
	m_lines.ShowIndent(m_parentFrame.IsIndentShown());
	m_lines.Init();
	//m_lines.AddStyler(m_usersStyler);
	m_lines.AddStyler(m_syntaxstyler);
	m_lines.AddStyler(m_search_hl_styler);

	// Set initial tabsize
	SetTabWidth(m_parentFrame.GetTabWidth());

	// Should we show margin line?
	if (!doShowMargin) marginChars = 0;
	m_lines.ShowMargin(marginChars);

	// Create a Carret to indicate edit position
	m_caretHeight = m_lines.GetLineHeight();
	caret = new LiveCaret(this, m_caretWidth, m_caretHeight); // will be deleted by window on destroy
	caret->Move(0, 0);
	SetCaret(caret);
	caret->Show();

	SetCursor(wxCursor(wxCURSOR_IBEAM));

	// Set drop target so files and text can be dragged from explorer to trigger drag commands
	DragDropTarget* dropTarget = new DragDropTarget(*this);
	SetDropTarget(dropTarget);

	// Make sure we get notified if we should display another document
	// (Unsubscription is done in OnClose())
	dispatcher.SubscribeC(wxT("WIN_SETDOCUMENT"), (CALL_BACK)OnSetDocument, this);
	dispatcher.SubscribeC(wxT("DOC_COMMITED"), (CALL_BACK)OnDocCommited, this);
	dispatcher.SubscribeC(wxT("THEME_CHANGED"), (CALL_BACK)OnThemeChanged, this);
	dispatcher.SubscribeC(wxT("BUNDLES_RELOADED"), (CALL_BACK)OnBundlesReloaded, this);
	dispatcher.SubscribeC(wxT("SETTINGS_CHANGED"), (CALL_BACK)OnSettingsChanged, this);
}

EditorCtrl::~EditorCtrl() {
	// Make sure we no longer recieve any notifiers
	dispatcher.UnSubscribe(wxT("WIN_SETDOCUMENT"), (CALL_BACK)OnSetDocument, this);
	dispatcher.UnSubscribe(wxT("DOC_COMMITED"), (CALL_BACK)OnDocCommited, this);
	dispatcher.UnSubscribe(wxT("THEME_CHANGED"), (CALL_BACK)OnThemeChanged, this);
	dispatcher.UnSubscribe(wxT("BUNDLES_RELOADED"), (CALL_BACK)OnBundlesReloaded, this);
	dispatcher.UnSubscribe(wxT("SETTINGS_CHANGED"), (CALL_BACK)OnSettingsChanged, this);

	// Delete the document
	//catalyst.DeleteDocument(revision.GetDocumentID());
	cxLOCKDOC_WRITE(m_doc)
		doc.Close();
	cxENDLOCK

	// Notify mate that we have finished editing document
	if (!m_mate.empty()) {
#ifdef __WXMSW__
		HWND hWndRecv = ::FindWindow(wxT("wxWindowClassNR"), m_mate);
		if (hWndRecv) {
			const wxString cmd = wxT("DONE");
			const wxCharBuffer msg = cmd.mb_str(wxConvUTF8);

			// Send WM_COPYDATA message
			COPYDATASTRUCT cds;
			cds.dwData = 0;
			cds.cbData = (DWORD)(strlen(msg.data()) + 1);
			cds.lpData = (void*)msg.data();
			::SendMessage(hWndRecv, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds );
		}
#endif
	}

	ClearRemoteInfo();
}

void EditorCtrl::ClearRemoteInfo() {
	if (m_remotePath.empty()) return;

	m_remotePath.clear();
	m_remoteProfile = NULL;
	if (m_path.IsOk()) wxRemoveFile(m_path.GetFullPath()); // Clean up temp buffer file
}

unsigned int EditorCtrl::GetLength() const {
	return m_lines.GetLength();
}

unsigned int EditorCtrl::GetPos() const {
	return m_lines.GetPos();
}

unsigned int EditorCtrl::GetCurrentLineNumber() {
	// Line index start from 1
	return m_lines.GetCurrentLine() +1 ;
}

unsigned int EditorCtrl::GetCurrentColumnNumber() {
	const unsigned int pos = m_lines.GetPos();
	const unsigned int line = m_lines.GetCurrentLine();
	const unsigned int linestart = m_lines.GetLineStartpos(line);

	// Column index start from 1
	//return (pos - linestart) + 1;

	cxLOCKDOC_READ(m_doc)
		return doc.GetLengthInChars(linestart, pos) + 1;
	cxENDLOCK;
}

int EditorCtrl::GetTopLine() {
	wxASSERT(scrollPos >= 0);

	//wxLogDebug(wxT("GetTopLine() - %d %d - %s"), scrollPos, m_lines.GetLineFromYPos(scrollPos), GetPath());

	return m_lines.GetLineFromYPos(scrollPos);
}

void EditorCtrl::Freeze() {
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
	cxENDLOCK
}

doc_id EditorCtrl::GetDocID() const {
	cxLOCKDOC_READ(m_doc)
		return doc.GetDocument();
	cxENDLOCK
}

wxString EditorCtrl::GetName() const {
	cxLOCKDOC_READ(m_doc)
		return doc.GetPropertyName();
	cxENDLOCK
}

wxFontEncoding EditorCtrl::GetEncoding() const {
	cxLOCKDOC_READ(m_doc)
		return doc.GetPropertyEncoding();
	cxENDLOCK
}

void EditorCtrl::SetEncoding(wxFontEncoding enc) {
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
		doc.SetPropertyEncoding(enc);
		doc.Freeze();
	cxENDLOCK
	MarkAsModified();
}

bool EditorCtrl::GetBOM() const {
	cxLOCKDOC_READ(m_doc)
		return doc.GetPropertyBOM();
	cxENDLOCK
}

void EditorCtrl::SetBOM(bool saveBOM) {
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
		doc.SetPropertyBOM(saveBOM);
		doc.Freeze();
	cxENDLOCK
	MarkAsModified();
}

wxTextFileType EditorCtrl::GetEOL() const {
	cxLOCKDOC_READ(m_doc)
		return doc.GetPropertyEOL();
	cxENDLOCK
}

void EditorCtrl::SetEOL(wxTextFileType eol) {
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
		doc.SetPropertyEOL(eol);
		doc.Freeze();
	cxENDLOCK
	MarkAsModified();
}
/*
void EditorCtrl::ActivateHighlight() {
	m_usersStyler.Enable();
	//DrawLayout();
}

void EditorCtrl::DisableHighlight() {
	m_usersStyler.Disable();
	//DrawLayout();
}
*/
void EditorCtrl::ClearSearchHighlight() {
	m_search_hl_styler.Clear();
	DrawLayout();
}

bool EditorCtrl::IsOk() const {
	wxSize size = GetClientSize();

	// It is ok to have a size 0 window with invalid bitmap
	if (size.x == 0 && size.y == 0) return true;
	else return bitmap.Ok();
}

// WARNING: Be aware that during a change grouping, lines is not valid
// Only call functions like lines.RemoveAllSelections() that does not use line info
void EditorCtrl::StartChange() {
	do_freeze = false;
	change_pos = m_lines.GetPos();
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
		change_doc_id = doc.GetDocument();
	cxENDLOCK
	change_toppos = m_lines.GetPosFromXY(0, scrollPos+1);
}

void EditorCtrl::EndChange() {
	wxASSERT(do_freeze == false);

	doc_id di;
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
		di = doc.GetDocument();
	cxENDLOCK

	// Invalidate all stylers
	StylersInvalidate();

	if (change_doc_id.type == di.type && change_doc_id.document_id == di.document_id) {
		// If we are still in the same document, we want to find
		// matching positions and selections

		if (change_doc_id.version_id == di.version_id) return;

		vector<interval> oldsel = m_lines.GetSelections();

		m_lines.ReLoadText();

		// re-set the width
		UpdateEditorWidth();

		RemapPos(change_doc_id, change_pos, oldsel, change_toppos);
	}
	else {
		scrollPos = 0;

		m_lines.ReLoadText();

		// re-set the width
		UpdateEditorWidth();
	}

	do_freeze = true;
}

void EditorCtrl::ReDraw() {
	DrawLayout();
}

bool EditorCtrl::Show(bool show) {
	bool result = wxControl::Show(show);

	// All EditorCtrl's share the same bitmap.
	// So we have to redraw it when the control is first shown
	if (show) {
		mdc.SelectObject(bitmap);
		DrawLayout();
	}
	else mdc.SelectObject(wxNullBitmap);

	return result;
}

void EditorCtrl::SetShowGutter(bool showGutter) {
	if (m_showGutter == showGutter) return;

	if (showGutter) {
		m_gutterCtrl->Show();
	}
	else {
		m_gutterCtrl->Hide();
		m_gutterWidth = 0;
	}

	m_showGutter = showGutter;
	DrawLayout();
}

void EditorCtrl::SetShowIndent(bool showIndent) {
	m_lines.ShowIndent(showIndent);
	DrawLayout();
}

void EditorCtrl::SetWordWrap(cxWrapMode wrapMode) {
	if (m_lines.GetWrapMode() == wrapMode) return;

	// Make sure we keep the same topline
	topline = m_lines.GetLineFromYPos(scrollPos);

	m_lines.SetWordWrap(wrapMode);

	if (m_wrapAtMargin && wrapMode != cxWRAP_NONE) {
		m_lines.SetWidth(m_lines.GetMarginPos(), topline);
	}

	scrollPos = m_lines.GetYPosFromLine(topline);
	DrawLayout();
}

void EditorCtrl::SetTabWidth(unsigned int width) {
	if (m_parentFrame.IsSoftTabs()) m_indent = wxString(wxT(' '), m_parentFrame.GetTabWidth());
	else m_indent = wxString(wxT("\t"));

	m_lines.SetTabWidth(width);
	FoldingReIndent();

	MarkAsModified();
}

const wxFont& EditorCtrl::GetEditorFont() const {
	return mdc.GetFont();
}

void EditorCtrl::SetGutterRight(bool doMove) {
	m_gutterLeft = !doMove;
	m_gutterCtrl->SetGutterRight(doMove);
	DrawLayout();
}

//
// Returns true if a scrollbar was added or removed, else false.
//
bool EditorCtrl::UpdateScrollbars(unsigned int x, unsigned int y) {
	// Check if we need a vertical scrollbar
	const int scroll_thumb = GetScrollThumb(wxVERTICAL);
	const unsigned int height = m_lines.GetHeight();
    if (height > y) {
		const int scroll_range = GetScrollRange(wxVERTICAL);
		if (scroll_thumb != (int)y || scroll_range != (int)height) {
			SetScrollbar(wxVERTICAL, scrollPos, y, height);
			if (scroll_thumb == 0) return true; // Creation of scrollbar have sent a size event
		}

		// Avoid empty space at bottom
		if (scrollPos + y > height) {
			scrollPos = wxMax(0, height - y);
		}

		SetScrollPos(wxVERTICAL, scrollPos);
	}
	else if (scroll_thumb > 0) {
		SetScrollbar(wxVERTICAL, 0, 0, 0);
		return true; // Removal of scrollbar have sent a size event
	}

	// Check if we need a horizontal scrollbar
	const int scrollThumbX = GetScrollThumb(wxHORIZONTAL);
	const unsigned int width = m_lines.GetWidth() + m_caretWidth;
	if ((m_lines.GetWrapMode() == cxWRAP_NONE || m_wrapAtMargin) && width > x) {
		m_scrollPosX = wxMin(m_scrollPosX, (int)(width-x));
		m_scrollPosX = wxMax(0, m_scrollPosX);

		SetScrollbar(wxHORIZONTAL, m_scrollPosX, x, width);
	}
	else if (scrollThumbX > 0) {
		m_scrollPosX = 0;
		SetScrollbar(wxHORIZONTAL, 0, 0, 0);
		return true; // Removal of scrollbar have sent a size event
	}

	return false; // no scrollbar was added or removed
}

void EditorCtrl::DrawLayout(wxDC& dc, bool WXUNUSED(isScrolling)) {
	if (!IsShown() || !m_enableDrawing) return; // No need to draw
	wxLogDebug(wxT("DrawLayout() : %d (%d,%d)"), GetId(), m_enableDrawing, IsShown());
	//wxLogDebug(wxT("DrawLayout() : %s"), GetName());

	wxASSERT(m_scrollPosX >= 0);

	// Check if we should cancel a snippet
	if (m_snippetHandler.IsActive()) m_snippetHandler.Validate();

	// Get view dimensions
	const wxSize size = GetClientSize();
	if (size.x == 0 || size.y == 0) return; // Nothing to draw

	// resize the bitmap used for doublebuffering
	if (bitmap.GetWidth() < size.x || bitmap.GetHeight() < size.y) {
		// disassociate and release mem for old bitmap
		mdc.SelectObjectAsSource(wxNullBitmap);
		bitmap = wxNullBitmap;

		// Resize bitmap
		bitmap = wxBitmap(size.x, size.y);

		// Select the new bitmap
		mdc.SelectObject(bitmap);
	}

	if (m_showGutter) {
		m_gutterWidth = m_gutterCtrl->CalcLayout(size.y);
	}
	const unsigned int editorSizeX = ClientWidthToEditor(size.x);

	// There may only be room for the gutter
	if (editorSizeX == 0) {
		if (m_showGutter) m_gutterCtrl->DrawGutter();
		return;
	}

	// Verify scrollPos (might come from an un-updated scrollbar)
	if (scrollPos < 0) {
		// Scroll value of -1 indicates that we should move to the
		// bottom of the document.
		wxASSERT(scrollPos == -1); // only valid neg. value
		//wxASSERT(size.x == m_lines.GetWidth()); // should never come with a size change
		m_lines.PrepareAll(); // make sure all line positions are valid
		scrollPos = wxMax(0, m_lines.GetHeight()-size.y);
	}
	else {
		scrollPos = wxMin(scrollPos, wxMax(0, m_lines.GetHeight()-size.y));

		if (m_isResizing && editorSizeX != m_lines.GetDisplayWidth()) {
			const int newtopline = m_lines.GetLineFromYPos(scrollPos);
			const unsigned int lineWidth = (m_wrapAtMargin && m_lines.GetWrapMode() != cxWRAP_NONE) ? m_lines.GetMarginPos() : editorSizeX;
			m_lines.SetWidth(lineWidth, newtopline);

			// If there are folds, we have to make sure all line positions are valid
			if (HasFoldedFolds()) m_lines.PrepareAll();

			// Lock topline on resize
			if (topline == -1) topline = newtopline;
			scrollPos = m_lines.GetYPosFromLine(newtopline);
		}
		else {
			topline = -1;
			scrollPos += m_lines.PrepareYPos(scrollPos);
			scrollPos = wxMin(scrollPos, m_lines.GetHeight()-size.y);
			scrollPos = wxMax(0, scrollPos);
		}
	}
	wxASSERT(scrollPos >= 0);

	wxASSERT(scrollPos <= m_lines.GetHeight());

	// Check if we need to adjust scrollbar
	if (UpdateScrollbars(editorSizeX, size.y)) return; // adding/removing scrollbars send size event

	// Make sure we remove un-needed stylers
	if (!m_parentFrame.IsSearching()) {
		m_search_hl_styler.Clear(); // Only highlight search terms during search
	}

	// When scrolling, we can just draw the new parts
	wxRect rect(0, scrollPos, editorSizeX, size.y);
	/*if (isScrolling && scrollPos != old_scrollPos && this == FindFocus()) {
		// If there is overlap, then move the image
		if (scrollPos + size.y > old_scrollPos && scrollPos < old_scrollPos + size.y) {
			const int top = wxMax(scrollPos, old_scrollPos);
			const int bottom = wxMin(scrollPos, old_scrollPos) + size.y;
			const int overlap_height = bottom - top;
#ifdef __WXMSW__
			::BitBlt(GetHdcOf(mdc), 0, (top - old_scrollPos) + (old_scrollPos - scrollPos),  editorSizeX, overlap_height, GetHdcOf(mdc), 0, top - old_scrollPos, SRCCOPY);
#else
			mdc.Blit(0, (top - old_scrollPos) + (old_scrollPos - scrollPos),  editorSizeX, overlap_height, &mdc, 0, top - old_scrollPos);
#endif
			// Calculate rect of newly revealed part
			const int new_height = size.y - overlap_height;
			const int newtop = (top == scrollPos ? bottom : scrollPos);
			rect = wxRect(0,newtop,editorSizeX,new_height);
			wxASSERT(newtop <= m_lines.GetHeight());
		}
	}*/

	// Highlight matching brackets
	MatchBrackets();

	// Set the theme colors
	mdc.SetTextForeground(m_theme.foregroundColor);
	mdc.SetBrush(wxBrush(m_theme.backgroundColor));
	mdc.SetPen(wxPen(m_theme.backgroundColor));

	// Clear the background
	mdc.DrawRectangle(rect.x, rect.y - scrollPos, rect.width, rect.height);

	// Draw the layout to MemoryDC
	m_lines.Draw(-m_scrollPosX, -scrollPos, rect);
	
	// During the draw we may have corrected some approximated
	// line dimensions causing the dimesions of the entire document
	// to have changed. So we have to check if the scrollbars should
	// be updated again.
	if (UpdateScrollbars(editorSizeX, size.y)) return; // adding/removing scrollbars send size event

	// avoid leaving a caret trace
	if (caret->IsVisible()) caret->Hide();

	// Copy MemoryDC to Display
	const unsigned int xpos = m_gutterLeft ? m_gutterWidth : 0;
#ifdef __WXMSW__
	::BitBlt(GetHdcOf(dc), xpos, 0,(int)editorSizeX, (int)size.y, GetHdcOf(mdc), 0, 0, SRCCOPY);
#else
	dc.Blit(xpos, 0,  editorSizeX, size.y, &mdc, 0, 0);
#endif

	if (m_lines.IsCaretInPreparedPos()) {
		// Move the caret to the new position
		m_lines.UpdateCaretPos();
		if (IsCaretVisible()) {
			const wxPoint cpos = GetCaretPoint();
			caret->Move(cpos);
			caret->Show();
		}
	}

	// Draw the gutter
	if (m_showGutter) m_gutterCtrl->DrawGutter();

	// Check if we should send notification about scrolling
	if (scrollPos != old_scrollPos) {
		if (m_scrollCallback) m_scrollCallback(m_scrollCallbackData);
		old_scrollPos = scrollPos;
	}

	m_isResizing = false;
}

unsigned int EditorCtrl::ClientWidthToEditor(unsigned int width) const {
	return (width > m_gutterWidth) ? (width - m_gutterWidth) : 0;
}

wxPoint EditorCtrl::ClientPosToEditor(unsigned int xpos, unsigned int ypos) const {
	if (m_gutterLeft) return wxPoint((xpos - m_gutterWidth) + m_scrollPosX, ypos + scrollPos);
	else return wxPoint(xpos + m_scrollPosX, ypos + scrollPos);
}

wxPoint EditorCtrl::EditorPosToClient(unsigned int xpos, unsigned int ypos) const {
	if (m_gutterLeft) return wxPoint(m_gutterWidth + (xpos - m_scrollPosX), ypos - scrollPos);
	else return wxPoint(xpos - m_scrollPosX, ypos - scrollPos);
}

wxPoint EditorCtrl::GetCaretPoint() const {
	const wxPoint cpos = m_lines.GetCaretPos();
	return EditorPosToClient(cpos.x, cpos.y);
}

unsigned int EditorCtrl::UpdateEditorWidth() {
	const wxSize size = GetClientSize();
	const unsigned int width = ClientWidthToEditor(size.x);
	m_lines.SetWidth(width);

	return width;
}

wxString EditorCtrl::GetNewIndentAfterNewline(unsigned int lineid) {
	wxASSERT(lineid < m_lines.GetLineCount());

	int i = lineid;
	for (; i >= 0; --i) {
		const unsigned int linestart = m_lines.GetLineStartpos(i);
		const unsigned int lineend = m_lines.GetLineEndpos(i, true);

		if (linestart < lineend) {
			const deque<const wxString*> scope = m_syntaxstyler.GetScope(linestart);

			// Check if we should ignore the indentation on current line
			const wxString& ignorePattern = m_syntaxHandler.GetIndentNonePattern(scope);
			if (!ignorePattern.empty()) {
				const search_result res = RegExFind(ignorePattern, linestart, false, NULL, lineend);
				if (res.error_code > 0) continue;
			}

			// Check if indentation should increase
			const wxString& increasePattern = m_syntaxHandler.GetIndentIncreasePattern(scope);
			if (!increasePattern.empty()) {
				const search_result res = RegExFind(increasePattern, linestart, false, NULL, lineend);
				if (res.error_code > 0) return GetLineIndent(i) + m_indent;
			}

			/*// Check if only next line should be indented
			const wxString& nextPattern = m_syntaxHandler.GetIndentNextPattern(scope);
			if (!nextPattern.empty()) {
				const search_result res = RegExFind(nextPattern, linestart, false, NULL, lineend);
				if (res.error_code > 0) {
					//if (i) {
					//	// If the line above is already nextLineIndented, we should keep same indent
					//	// (we assume same pattern is valid)
					//	const unsigned int linestart2 = m_lines.GetLineStartpos(i-1);
					//	const unsigned int lineend2 = m_lines.GetLineEndpos(i-1, true);
					//	if (linestart2 < lineend2) {
					//		const search_result res2 = RegExFind(nextPattern, linestart2, false, NULL, lineend2);
					//		if (res.error_code > 0) return GetLineIndent(i+1);
					//	}
					//}
					return GetRealIndent(i+1); //+ m_indent;
				}
			}*/
		}

		// If we get to here we should just use the same indent as the line above
		break;
	}

	if (i == -1) return GetLineIndent(0);
	else return GetLineIndent(i);
}

wxString EditorCtrl::GetRealIndent(unsigned int lineid, bool newline) {
	if (lineid == 0) return GetLineIndent(0);
	wxASSERT(lineid < m_lines.GetLineCount());

	unsigned int linestart = m_lines.GetLineStartpos(lineid);
	unsigned int lineend;

	// The real indentation level of a line is defined by the lines above it
	unsigned int validLine = lineid;
	wxString indent;

	// We assume all the lines closely above have same indent rules
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(linestart);
	const wxString& unIndentedLinePattern = m_syntaxHandler.GetIndentNonePattern(scope);
	const wxString& indentNextLinePattern = m_syntaxHandler.GetIndentNextPattern(scope);
	const wxString& increasePattern = m_syntaxHandler.GetIndentIncreasePattern(scope);

	int i = lineid-1;
	for (;;) {
		// Move up through the lines until we find one with valid indentation
		if (!unIndentedLinePattern.empty()) {
			for (; i >= 0; --i) {
				linestart = m_lines.GetLineStartpos(i);
				lineend = m_lines.GetLineEndpos(i, true);

				// Ignore unindented lines
				const search_result res = RegExFind(unIndentedLinePattern, linestart, false, NULL, lineend);
				if (res.error_code <= 0) {validLine = i; break;}
			}
		}
		if (validLine == lineid) break;

		linestart = m_lines.GetLineStartpos(validLine);
		lineend = m_lines.GetLineEndpos(validLine, true);

		// Increasing line is ok (but we have to use it's indentation)
		if (!increasePattern.empty()) {
			const search_result res = RegExFind(increasePattern, linestart, false, NULL, lineend);
			if (res.error_code > 0) {
				indent = m_indent; // increase with one
				break;
			}
		}

		// Nextline indenter is ok (but don't indent if target is nextlineindent as well)
		if (!indentNextLinePattern.empty()) {
			const search_result res = RegExFind(indentNextLinePattern, linestart, false, NULL, lineend);
			if (res.error_code > 0) {
				// Multiple nextLineIndented lines in sequence should have same indent
				const unsigned int linestart2 = m_lines.GetLineStartpos(lineid);
				const unsigned int lineend2 = m_lines.GetLineEndpos(lineid, true);
				if (linestart2 != lineend2) { // never increase indent on new line
					const search_result res2 = RegExFind(indentNextLinePattern, linestart2, false, NULL, lineend2);
					if (res2.error_code <= 0) indent = m_indent; // increase with one
				}
				break;
			}

			// If valid line does not have any specific indent rule
			// we have to check whether it is nextlineindented by line above
			for (--i; i >= 0; --i) {
				linestart = m_lines.GetLineStartpos(i);
				lineend = m_lines.GetLineEndpos(i, true);

				// Ignore unindented lines
				if (!unIndentedLinePattern.empty()) {
					const search_result res = RegExFind(unIndentedLinePattern, linestart, false, NULL, lineend);
					if (res.error_code > 0) continue;
				}

				const search_result res = RegExFind(indentNextLinePattern, linestart, false, NULL, lineend);
				if (res.error_code > 0) validLine = i;
				break;
			}
		}
		break;
	}

	indent += GetLineIndent(validLine);

	// Check if the current line should be de-dented
	if (!newline && validLine < lineid) {
		linestart = m_lines.GetLineStartpos(lineid);
		lineend = m_lines.GetLineEndpos(lineid, true);

		const wxString& decreasePattern = m_syntaxHandler.GetIndentDecreasePattern(scope);
		if (!decreasePattern.empty()) {
			const search_result res = RegExFind(decreasePattern, linestart, false, NULL, lineend);
			if (res.error_code > 0) {
				// Decrease tab level with one
				if (!indent.empty()) {
					const unsigned int tabWidth = m_parentFrame.GetTabWidth();
					if (indent[0] == wxT('\t')) indent.Remove(0, 1);
					else if (indent.size() >= tabWidth) indent.Remove(0, tabWidth);
				}
				return indent;
			}
		}
	}

	// Return indent of line
	return indent;
}

wxString EditorCtrl::GetLineIndentFromPos(unsigned int pos) {
	wxASSERT(pos <= GetLength());

	const unsigned int lineid = m_lines.GetLineFromCharPos(pos);
	return GetLineIndent(lineid);
}

wxString EditorCtrl::GetLineIndent(unsigned int lineid) {
	if (lineid == 0 && GetLength() == 0) return wxEmptyString;
	wxASSERT(lineid < m_lines.GetLineCount());

	const unsigned int linestart = m_lines.GetLineStartpos(lineid);
	const unsigned int lineend = m_lines.GetLineEndpos(lineid, false);
	if (linestart == lineend) return wxEmptyString;

	wxString indent;
	cxLOCKDOC_READ(m_doc)
		doc_byte_iter dbi(doc, linestart);
		while ((unsigned int)dbi.GetIndex() < lineend) {
			if (!wxIsspace(*dbi) || *dbi == '\n') break;
			++dbi;
		}

		if (linestart < (unsigned int)dbi.GetIndex()) {
			indent = doc.GetTextPart(linestart, dbi.GetIndex());
		}
	cxENDLOCK

	return indent;
}

unsigned int EditorCtrl::GetLineIndentLevel(unsigned int lineid) {
	if (lineid == 0 && GetLength() == 0) return 0;
	wxASSERT(lineid < m_lines.GetLineCount());

	const unsigned int linestart = m_lines.GetLineStartpos(lineid);
	const unsigned int lineend = m_lines.GetLineEndpos(lineid, false);
	if (linestart == lineend) return 0;

	unsigned int indent = 0;
	const unsigned int tabWidth = m_parentFrame.GetTabWidth();

	// The level is counted in spaces
	cxLOCKDOC_READ(m_doc)
		doc_byte_iter dbi(doc, linestart);
		while ((unsigned int)dbi.GetIndex() < lineend) {
			if (*dbi == '\t') {
				// it is ok to have a few spaces before tab (making one mixed tab)
				const unsigned int spaces = tabWidth - (indent % tabWidth);
				indent += spaces;
			}
			else if (*dbi == ' ') {
				++indent;
			}
			else break;
			++dbi;
		}
	cxENDLOCK

	return indent;
}

unsigned int EditorCtrl::CountIndent(const wxString& text) const {
	const unsigned int tabWidth = m_parentFrame.GetTabWidth();
	unsigned int indent = 0;

	// The level is counted in spaces
	for (unsigned int i = 0; i < text.size(); ++i) {
		const wxChar c = text[i];

		if (c == '\t') {
			// it is ok to have a few spaces before tab (making one mixed tab)
			const unsigned int spaces = tabWidth - (indent % tabWidth);
			indent += spaces;
		}
		else if (c == ' ') {
			++indent;
		}
		else break;
	}

	return indent;
}

unsigned int EditorCtrl::GetLineIndentPos(unsigned int lineid) {
	if (lineid == 0 && GetLength() == 0) return 0;
	wxASSERT(lineid < m_lines.GetLineCount());

	const unsigned int linestart = m_lines.GetLineStartpos(lineid);
	const unsigned int lineend = m_lines.GetLineEndpos(lineid, false);
	if (linestart == lineend) return 0;

	cxLOCKDOC_READ(m_doc)
		doc_byte_iter dbi(doc, linestart);
		while ((unsigned int)dbi.GetIndex() < lineend) {
			if (!wxIsspace(*dbi) || *dbi == '\n') break;
			++dbi;
		}

		return dbi.GetIndex();
	cxENDLOCK
}

bool EditorCtrl::IsSpaces(unsigned int start, unsigned int end) const {
	wxASSERT(start <= end && end <= m_lines.GetLength());
	if (start == end) return false;

	cxLOCKDOC_READ(m_doc)
		doc_byte_iter dbi(doc, start);

		while ((unsigned int)dbi.GetIndex() < end) {
			if (*dbi != ' ') return false;
			++dbi;
		}
	cxENDLOCK

	return true;
}

unsigned int EditorCtrl::CountMatchingChars(wxChar match, unsigned int start, unsigned int end) const {
	wxASSERT(start <= end && end <= m_lines.GetLength());
	if (start == end) return 0;

	const wxString text = GetText(start, end);
	unsigned int count = 0;

	for (unsigned int i = 0; i < text.size(); ++i) {
		if (text[i] == match) ++count;
	}

	return count;
}

wxString EditorCtrl::GetText() const {
	return GetText(0, GetLength());
}

void EditorCtrl::WriteText(wxOutputStream& stream) const {
	cxLOCKDOC_READ(m_doc)
		doc.WriteText(stream);
	cxENDLOCK
}

wxString EditorCtrl::GetText(unsigned int start, unsigned int end) const {
	wxASSERT(start <= end && end <= m_lines.GetLength());
	cxLOCKDOC_READ(m_doc)
		return doc.GetTextPart(start, end);
	cxENDLOCK
}

void EditorCtrl::GetText(vector<char>& text) const {
	GetTextPart(0, GetLength(), text);
}

void EditorCtrl::GetTextPart(unsigned int start, unsigned int end, vector<char>& text) const {
	const unsigned int len = end - start;
	if (len) {
		text.resize(len);

		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(start, end, (unsigned char*)&*text.begin());
		cxENDLOCK
	}
}

void EditorCtrl::Tab() {
	const bool shiftDown = wxGetKeyState(WXK_SHIFT);
	const bool isMultiSel = m_lines.IsSelectionMultiline() && !m_lines.IsSelectionShadow();

	// If there are multible lines selected tab triggers indentation
	if (m_snippetHandler.IsActive()) {
		if (shiftDown) m_snippetHandler.PrevTab();
		else m_snippetHandler.NextTab();
	}
	else if (shiftDown || isMultiSel) {
		if (lastaction != ACTION_NONE || lastpos != GetPos()) Freeze();

		IndentSelectedLines(!shiftDown);

		lastpos = GetPos();
		lastaction = ACTION_NONE;
	}
	else {
		// If the tab is preceded by a word it can trigger a snippet
		const unsigned int pos = GetPos();
		unsigned int wordstart = pos;

		// First check if we can get a trigger before first non-alnum char
		cxLOCKDOC_READ(m_doc)
			while (wordstart) {
				const unsigned int prevchar = doc.GetPrevCharPos(wordstart);
				const wxChar c = doc.GetChar(prevchar);
				if (!Isalnum(c)) break;
				wordstart = prevchar;
			}
		cxENDLOCK
		if (wordstart < pos && DoTabTrigger(wordstart, pos)) return;

		// If that did not give a trigger, try to first whitespace
		cxLOCKDOC_READ(m_doc)
			while (wordstart) {
				const unsigned int prevchar = doc.GetPrevCharPos(wordstart);
				const wxChar c = doc.GetChar(prevchar);
				if (wxIsspace(c)) break;
				wordstart = prevchar;
			}
		cxENDLOCK
		if (wordstart < pos) {
			if (DoTabTrigger(wordstart, pos)) return;

			// If we still don't have a trigger, check if we are in quotes or parans
			// (needed for the rare case when trigger is non-alnum)
			unsigned int qstart = pos;
			cxLOCKDOC_READ(m_doc)
				while (qstart > wordstart) {
					const unsigned int prevchar = doc.GetPrevCharPos(qstart);
					const wxChar c = doc.GetChar(prevchar);
					if (c == wxT('"') || c == wxT('\'') || c == wxT('(') || c == wxT('{') || c == wxT('[')) break;
					qstart = prevchar;
				}
			cxENDLOCK
			if (qstart < pos && qstart != wordstart && DoTabTrigger(qstart, pos)) return;
		}

		// If we get to here we have to insert a real tab
		if (!m_parentFrame.IsSoftTabs()) InsertChar(wxChar('\t'));
		else {
			const unsigned int linestart = m_lines.GetLineStartpos(m_lines.GetCurrentLine());
			const unsigned int tabWidth = m_parentFrame.GetTabWidth();

			// Calculate pos with tabs expanded
			unsigned int tabpos = 0;
			cxLOCKDOC_READ(m_doc)
				for (doc_byte_iter dbi(doc, linestart); (unsigned int)dbi.GetIndex() < pos; ++dbi) {
					// Only count first byte of UTF-8 multibyte chars
					if ((*dbi & 0xC0) == 0x80) continue;
					if (*dbi == '\t') tabpos += tabWidth;
					else tabpos += 1;
				}
			cxENDLOCK;

			// Insert as many spaces as it takes to get to next tabstop
			const unsigned int spaces = tabpos % tabWidth;
			const wxString indent(wxT(' '), (spaces == 0 || spaces == tabWidth) ? tabWidth : tabWidth - spaces);

			Insert(indent);
		}
	}
}

bool EditorCtrl::DoTabTrigger(unsigned int wordstart, unsigned int wordend) {
	wxASSERT(wordstart < wordend && wordend <= m_lines.GetLength());

	wxString trigger;
	cxLOCKDOC_READ(m_doc)
		trigger = doc.GetTextPart(wordstart, wordend);
	cxENDLOCK

	const deque<const wxString*> scope = m_syntaxstyler.GetScope(wordend);
	const vector<const tmAction*> actions = m_syntaxHandler.GetActions(trigger, scope);

	//wxLogDebug(wxT("%s (%u)"), trigger, snippets.size());
	if (!actions.empty()) {
		//wxLogDebug(wxT("%s"), actions[0]->content);

		// Present user with a list of actions
		int actionIndex = 0;
		if (actions.size() > 1) {
			actionIndex = ShowPopupList(actions);
			if (actionIndex == -1) return true;
		}

		// Clean up first
		if (!m_lines.IsSelectionShadow()) RemoveAllSelections();
		m_currentSel = -1;
		m_snippetHandler.Clear(); // stop any active snippets

		// Remove the trigger
		Freeze();
		RawDelete(wordstart, wordend);

		// Do the Action
		DoAction(*actions[actionIndex], NULL, true);
		return true;
	}

	return false; // no action found for trigger
}

void EditorCtrl::FilterThroughCommand() {
	RunCmdDlg dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
		const tmCommand cmd = dlg.GetCommand();
		DoAction(cmd, NULL, false);
	}
}

void EditorCtrl::DoActionFromDlg() {
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(GetPos());

	FindCmdDlg dlg(this, scope);
	if (dlg.ShowModal() == wxID_OK) {
		const tmAction* action = dlg.GetSelection();
		if (action) DoAction(*action, NULL, false);
	}
}

void EditorCtrl::DoAction(const tmAction& action, const map<wxString, wxString>* envVars, bool isRaw) {
	wxLogDebug(wxT("Doing action '%s' from bundle '%s'"), action.name.c_str(), action.bundle ? action.bundle->name.c_str() : wxT("none"));
	if (action.IsSyntax()) {
		SetSyntax(action.name);
		return;
	}

	if (m_activeTooltip) m_activeTooltip->Close();
	wxWindowDisabler wd;

	// Get Action contents
	const vector<char>& cmdContent = m_syntaxHandler.GetActionContent(action);
	if (cmdContent.empty()) return; // nothing to do

	// Set up the process environment
	const bool isUnix = action.isUnix;
	cxEnv env;
	SetEnv(env, isUnix, action.bundle);

	if (envVars) {
		env.SetEnv(*envVars);
	}

	if (!isRaw) Freeze();

	// Set shell variables from bundle prefs
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(GetPos());
	map<wxString, wxString> shellVars = m_syntaxHandler.GetShellVariables(scope);
	env.SetEnv(shellVars);

	// Set current working dir to document location
	wxString cwd;
	if (m_path.IsOk()) cwd = m_path.GetPath();

	if (action.IsSnippet()) {
		DeleteSelections();
		m_snippetHandler.StartSnippet(this, cmdContent, env, action.bundle);
	}
	else if (action.IsCommand()) {
		#ifdef __WXMSW__
		if (isUnix && !((eApp*)wxTheApp)->InitCygwin()) return;
		#endif // __WXMSW__

		const tmCommand* cmd = (tmCommand*)&action;

		// beforeRunningCommand
		if (cmd->save == tmCommand::csDOC && IsModified()) {
			if (!SaveText()) return;
			m_parentFrame.SetPath(); // update tabs
		}
		else if (cmd->save == tmCommand::csALL) {
			if (m_parentFrame.HasProject()) {
				// Ask parent to save all modified docs
				m_parentFrame.SaveAllFilesInProject();
			}
			else {
				if (!SaveText()) return;
				m_parentFrame.SetPath(); // update tabs
			}
		}

		// Establish input source
		tmCommand::CmdInput src = cmd->input;
		bool isSelectionInput = false;
		if (src == tmCommand::ciSEL) {
			if (!IsSelected()) {
				//if (cmd->fallbackInput == tmCommand::ciNONE) {
				//	src = tmCommand::ciDOC;
				//}
				//else src = cmd->fallbackInput;
				src = cmd->fallbackInput;
			}
			isSelectionInput = true;
		}

		// Get the input
		unsigned int selStart = GetPos();
		unsigned int selEnd = selStart;
		switch (src) {
		case tmCommand::ciSEL:
			if(IsSelected()) {
				const interval& sel = m_lines.GetSelections()[0];
				selStart = sel.start;
				selEnd = sel.end;
			}
			break;

		case tmCommand::ciDOC:
			selStart = 0;
			selEnd = GetLength();
			break;

		case tmCommand::ciLINE:
			{
				const unsigned int cl = m_lines.GetCurrentLine();
				selStart = m_lines.GetLineStartpos(cl);
				selEnd = m_lines.GetLineEndpos(cl);
			}
			break;

		case tmCommand::ciWORD:
			{
				const interval iv = GetWordIv(selStart);
				selStart = iv.start;
				selEnd = iv.end;
			}
			break;

		case tmCommand::ciCHAR:
			if (selStart == GetLength()) break;

			cxLOCKDOC_READ(m_doc)
				selEnd = doc.GetNextCharPos(selStart);
			cxENDLOCK
			break;

        case tmCommand::ciNONE:
            break;

		case tmCommand::ciSCOPE:
			const deque<interval> scopes = m_syntaxstyler.GetScopeIntervals(GetPos());
			if (!scopes.empty()) {
				const interval& iv = scopes.back();
				selStart = iv.start;
				selEnd = iv.end;
			}
			break;
		}

		// Get the input
		vector<char> input;
		if (selStart < selEnd) {
			const unsigned int inputLen = selEnd - selStart;
			if (cmd->inputXml) {
				m_syntaxstyler.GetTextWithScopes(selStart, selEnd, input);
			}
			else {
				input.resize(inputLen);
				cxLOCKDOC_READ(m_doc)
					doc.GetTextPart(selStart, selEnd, (unsigned char*)&*input.begin());
				cxENDLOCK
			}
		}

		// If there is input we have to set the TM_INPUT_START_* env vars
		// which contains the input start relative to the line start
		if (!input.empty() && src != tmCommand::ciDOC) {
			const unsigned int lineNum = m_lines.GetLineFromCharPos(selStart);
			const unsigned int lineStart = m_lines.GetLineStartpos(lineNum);
			const unsigned int lineIndex = selStart - lineStart;
			env.SetEnv(wxT("TM_INPUT_START_LINE"), wxString::Format(wxT("%u"), lineNum+1));
			env.SetEnv(wxT("TM_INPUT_START_LINE_INDEX"), wxString::Format(wxT("%u"), lineIndex));
			env.SetEnv(wxT("TM_INPUT_START_COLUMN"), wxString::Format(wxT("%u"), lineIndex+1));
		}

		// Run command
		vector<char> output;
		vector<char> errout;
		int pid;
		{
			// Set a busy cursor
			// will be reset when leaving scope
			wxBusyCursor wait;

			pid = RawShell(cmdContent, input, &output, &errout, env, isUnix, cwd);
		}
		if (pid != 0) wxLogDebug(wxT("shell returned pid = %d"), pid);

		tmCommand::CmdOutput outputMode = cmd->output;
		if (pid == 200) outputMode = tmCommand::coNONE;
		else if (pid == 201) outputMode = tmCommand::coSEL;
		else if (pid == 202) outputMode = tmCommand::coDOC;
		else if (pid == 203) outputMode = tmCommand::coINSERT;
		else if (pid == 204) outputMode = tmCommand::coSNIPPET;
		else if (pid == 205) outputMode = tmCommand::coHTML;
		else if (pid == 206) outputMode = tmCommand::coTOOLTIP;
		else if (pid == 207) outputMode = tmCommand::coNEWDOC;

		// Handle output
		if (outputMode != tmCommand::coNONE) {
			wxString shellout;
			wxString shellerr;
			wxString out;
			if (!output.empty()) shellout = wxString(&*output.begin(), wxConvUTF8, output.size());
			if (!errout.empty()) shellerr = wxString(&*errout.begin(), wxConvUTF8, errout.size());
			out = shellout + shellerr;
#ifdef __WXMSW__
			// WINDOWS ONLY!! newline conversion
			out.Replace(wxT("\r\n"), wxT("\n"));
#endif // __WXMSW__

			// End if we could not run command and there is no output
			if (pid != -1 || !out.empty()) {

				if (!isSelectionInput) {
					if(IsSelected()) {
						{
							const interval& sel = m_lines.GetSelections()[0];
							selStart = sel.start;
							selEnd = sel.end;
						}
					}
					else selStart = selEnd = GetPos();
				}
				RemoveAllSelections();

				switch (outputMode) {
				case tmCommand::coDOC:
					selStart = 0;
					selEnd = GetLength();
				case tmCommand::coSEL:
					{
						unsigned int pos = GetPos();

						RawDelete(selStart, selEnd);
						const unsigned int bytelen = RawInsert(selStart, out);

						if (src == tmCommand::ciSEL) {
							const unsigned int endPos = selStart + bytelen;
							m_lines.AddSelection(selStart, endPos);
							SetPos(endPos);
						}
						else {
							// Try to set a position as close a possible to previous
							if (pos > GetLength()) SetPos(GetLength());
							else {
								cxLOCKDOC_READ(m_doc)
									pos = doc.GetValidCharPos(pos);
								cxENDLOCK
								SetPos(pos);
							}
						}
					}
					break;

				case tmCommand::coINSERT:
					{
						const unsigned int bytelen = RawInsert(selEnd, out);
						SetPos(selEnd + bytelen);
					}
					break;

				case tmCommand::coTOOLTIP:
					out.Trim(); // strip trailing spaces & newlines
					ShowTipAtCaret(out);
					break;

				case tmCommand::coSNIPPET:
					{
						RawDelete(selStart, selEnd);
						SetPos(selStart);

						if (!output.empty()) {
							m_snippetHandler.StartSnippet(this, output, env, action.bundle);
						}
					}
					break;

				case tmCommand::coHTML:
					if (!shellout.empty()) {
						m_parentFrame.ShowOutput(cmd->name, shellout);
					}
					else {
						// Only show stderr in HTML window if there is
						// no other output
						m_parentFrame.ShowOutput(cmd->name, shellerr);
					}
					break;

				case tmCommand::coNEWDOC:
					{
						// Create new document
						doc_id di;
						cxLOCK_WRITE(m_catalyst)
							Document newdoc(catalyst.NewDocument(), m_catalyst);
							newdoc.Insert(0, out);
							newdoc.Freeze();
							di = newdoc.GetDocument();
						cxENDLOCK

						m_parentFrame.OpenDocument(di);
					}
					break;

				case tmCommand::coREPLACEDOC:
					{
						RawDelete(0, GetLength());
						RawInsert(0, out);
						SetPos(0);
					}
					break;

                case tmCommand::coNONE:
                    break;
				}
			}
		}

		// If the command has modified the file on disk, and we have not changed doc
		// since last save, the we automatically reload
		if (!IsEmpty() && !IsModified() && !IsRemote() && !IsBundleItem()) {
			// Get mirror info
			doc_id di;
			wxDateTime mDate;
			cxLOCK_READ(m_catalyst)
				catalyst.GetFileMirror(m_path.GetFullPath(), di, mDate);
			cxENDLOCK

			// Check if file exists
			if (mDate.IsValid() && m_path.FileExists()) {
				wxDateTime modDate = m_path.GetModificationTime();
				if (modDate.IsValid()) { // file could be locked by another app
					// Windows does not really handle the minor parts of file dates
					mDate.SetMillisecond(0);
					modDate.SetMillisecond(0);

					if (mDate != modDate) {
						LoadText(m_path.GetFullPath());
					}
				}
			}
		}
	}

	if (!isRaw) {
		MakeCaretVisible();
		DrawLayout();

		// command may have shown a dialog
		//Raise();
		SetFocus();
	}
}

unsigned int EditorCtrl::RawInsert(unsigned int pos, const wxString& text, bool doSmartType) {
	if (text.empty()) return 0;
	wxASSERT(pos <= m_lines.GetLength());

	// Reset autoPair state if inserting outside inner pair
	if (!m_pairStack.empty() && pos != m_pairStack.back().end) {
		m_pairStack.clear();
	}

	wxString autoPair;
	if (doSmartType) {
		// Check if we are inserting at end of inner pair
		if (!m_pairStack.empty()) { // we must be at end
			wxChar c;
			cxLOCKDOC_READ(m_doc)
				c = doc.GetChar(pos);
			cxENDLOCK

			// Jump over auto-paired char
			if (text == c) {
				cxLOCKDOC_READ(m_doc)
					pos = doc.GetNextCharPos(pos);
				cxENDLOCK
				m_lines.SetPos(pos);
				m_pairStack.pop_back();
				return 0;
			}
		}

		if (text.length() == 1) {
			autoPair = AutoPair(pos, text);
		}
	}

	// Insert the text
	unsigned int byte_len = 0;
	cxLOCKDOC_WRITE(m_doc)
		byte_len = doc.Insert(pos, text);
	cxENDLOCK
	m_lines.Insert(pos, byte_len); // Update the view
	StylersInsert(pos, byte_len);  // Update stylers

	// Smart Typing Pairs
	if (doSmartType) {
		// Caret is moved to end of insertion (but before smart pair)
		unsigned int pairpos = pos + byte_len;
		m_lines.SetPos(pairpos);

		if (!autoPair.empty()) {
			// insert paired char
			unsigned int pair_len = 0;
			cxLOCKDOC_WRITE(m_doc)
				pair_len = doc.Insert(pairpos, autoPair);
			cxENDLOCK
			m_lines.Insert(pairpos, pair_len); // Update the view
			StylersInsert(pairpos, pair_len);  // Update stylers
			byte_len += pair_len;
		}
		else {
			// Adjust containing pairs
			for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
				t->end += byte_len;
			}
		}
	}
	else {
		// Ensure carret stays at same position
		unsigned int caretPos = m_lines.GetPos();
		if (caretPos > pos) m_lines.SetPos(caretPos + byte_len);
	}


	MarkAsModified();
	return byte_len;
}

wxString EditorCtrl::GetAutoPair(unsigned int pos, const wxString& text) {
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(pos);
	const map<wxString, wxString> smartPairs = m_syntaxHandler.GetSmartTypingPairs(scope);

	const map<wxString, wxString>::const_iterator p = smartPairs.find(text);

#ifdef __WXDEBUG__
	unsigned int debug = 0;
	if (debug == 1) {
		for (map<wxString, wxString>::const_iterator i = smartPairs.begin(); i != smartPairs.end(); ++i) {
			wxLogDebug(wxT("%s -> %s"), i->first.c_str(), i->second.c_str());
		}
	}
#endif

	if (p == smartPairs.end()) return wxEmptyString;
	else return p->second;
}

wxString EditorCtrl::AutoPair(unsigned int pos, const wxString& text, bool addToStack) {
	wxASSERT(!text.empty());

	if (!m_doAutoPair) return wxEmptyString;

	// Are we just before a pair end?
	bool inPair = false;
	for (vector<interval>::const_iterator k = m_pairStack.begin(); k != m_pairStack.end(); ++k) {
		if (k->end == pos) {
			inPair = true;
			break;
		}
	}

	const unsigned int lineid = m_lines.GetLineFromCharPos(pos);
	const unsigned int lineend = m_lines.GetLineEndpos(lineid);

	// Only try to autopair if we are before pair-end, ws or eol
	if (!inPair && pos != lineend) {
		cxLOCKDOC_READ(m_doc)
			if (!wxIsspace(doc.GetChar(pos))) return wxEmptyString;
		cxENDLOCK
	}

	const wxString pairEnd = GetAutoPair(pos, text);
	if (pairEnd.empty()) return wxEmptyString;


	// Only pair equivalent 'brackets' if the total linecount is even
	if (text == pairEnd) {
		const unsigned int linestart = m_lines.GetLineStartpos(lineid);
		if (CountMatchingChars(text[0], linestart, lineend) % 2 != 0) return wxEmptyString;
	}

	if (addToStack) {
		const size_t starter_len = wxConvUTF8.FromWChar(NULL, 0, text) - 1; // also counts trailing null-byte
		const size_t ender_len = wxConvUTF8.FromWChar(NULL, 0, pairEnd) - 1; // also counts trailing null-byte
		const size_t byte_len = starter_len + ender_len;

		// Adjust containing pairs
		for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
			t->end += (unsigned int)byte_len;
		}

		const unsigned int pairPos = pos + starter_len;
		m_pairStack.push_back(interval(pairPos, pairPos));
	}

	return pairEnd;
}

void EditorCtrl::GotoMatchingBracket() {
	if (m_hlBracket.start == m_hlBracket.end) return;

	const unsigned int pos = m_lines.GetPos();

	if (pos == m_hlBracket.start) m_lines.SetPos(m_hlBracket.end);
	else if (pos == m_hlBracket.end) m_lines.SetPos(m_hlBracket.start);
	else return; // not on bracket

	MakeCaretVisible();
}

void EditorCtrl::MatchBrackets() {
	const unsigned int pos = m_lines.GetPos();

	if (m_bracketToken == m_changeToken && m_bracketPos == pos) return;
	m_bracketToken = m_changeToken;
	m_bracketPos = pos;

	// Clear old highlighted bracket
	m_hlBracket.start = m_hlBracket.end = 0;

	const unsigned int len = m_lines.GetLength();
	if (pos == len) return;

	// Get list of brackets
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(pos);
	const map<wxString, wxString> smartPairs = m_syntaxHandler.GetSmartTypingPairs(scope);

	// Get current char
	wxChar c;
	cxLOCKDOC_READ(m_doc)
		c = doc.GetChar(pos);
	cxENDLOCK

	bool searchForward = true;
	int limit = len;
	char start_bracket = '\0';
	char end_bracket = '\0';

	// Check if current char is a start bracket
	const map<wxString, wxString>::const_iterator p = smartPairs.find(c);
	if (p != smartPairs.end()) {
		start_bracket = p->first.mb_str(wxConvUTF8).data()[0];
		end_bracket = p->second.mb_str(wxConvUTF8).data()[0];

		// Indentical start and end brackets (just search on current line)
		if (start_bracket == end_bracket) {
			const unsigned int lineid = m_lines.GetCurrentLine();
			const unsigned int linestart = m_lines.GetLineStartpos(lineid);

			// count brackets from start of line
			unsigned int count = 0;
			unsigned int bracketpos = 0;
			cxLOCKDOC_READ(m_doc)
				bool escaped = false;
				for (doc_byte_iter dbi(doc, linestart); dbi.GetIndex() < (int)pos; ++dbi) {
					if (escaped) escaped = false;
					else {
						if (*dbi == '\\') escaped = true;
						else if (*dbi == start_bracket) {
							++count;
							bracketpos = dbi.GetIndex();
						}
					}
				}
				if (escaped) return; // current char is escaped
			cxENDLOCK

			if (count & 1) {
				m_hlBracket.start = bracketpos;
				m_hlBracket.end = pos;
			}
			else {
				const unsigned int lineend = m_lines.GetLineEndpos(lineid);
				cxLOCKDOC_READ(m_doc)
					bool escaped = false;
					for (doc_byte_iter dbi(doc, pos+1); dbi.GetIndex() < (int)lineend; ++dbi) {
						if (escaped) escaped = false;
						else {
							if (*dbi == '\\') escaped = true;
							else if (*dbi == start_bracket) {
								m_hlBracket.start = pos;
								m_hlBracket.end = dbi.GetIndex();
								break;
							}
						}
					}
				cxENDLOCK
			}

			return;
		}
	}
	else {
		// Search through end brackets
		bool bracketFound = false;
		for (map<wxString, wxString>::const_iterator p2 = smartPairs.begin(); p2 != smartPairs.end(); ++p2) {
			if (p2->second == c) {
				start_bracket = p2->second.mb_str(wxConvUTF8).data()[0];
				end_bracket = p2->first.mb_str(wxConvUTF8).data()[0];
				searchForward = false;
				bracketFound = true;
				limit = 0;
				break;
			}
		}
		if (!bracketFound) return; // no bracket at pos
	};

	if (searchForward) {
		cxLOCKDOC_READ(m_doc)
			doc_byte_iter dbi(doc, pos+1);
			bool escaped = false;
			unsigned int count = 1;
			while (dbi.GetIndex() < limit) {
				if (escaped) escaped = false;
				else {
					if (*dbi == '\\') escaped = true;
					else if (*dbi == start_bracket) ++count;
					else if (*dbi == end_bracket) {
						--count;
						if (count == 0) {
							m_hlBracket.start = pos;
							m_hlBracket.end = dbi.GetIndex();
							return;
						}
					}
				}
				++dbi;
			}
		cxENDLOCK
	}
	else {
		cxLOCKDOC_READ(m_doc)
			doc_byte_iter dbi(doc, pos-1);
			unsigned int count = 1;
			while (dbi.GetIndex() >= limit) {
				if (*dbi == start_bracket) {
					unsigned int esc_count = 0;
					for (doc_byte_iter dbi2 = dbi-1; dbi2.GetIndex() >= limit && *dbi2 == '\\'; --dbi2) ++esc_count;
					if (!(esc_count & 1)) ++count;
				}
				else if (*dbi == end_bracket) {
					unsigned int esc_count = 0;
					for (doc_byte_iter dbi2 = dbi-1; dbi2.GetIndex() >= limit && *dbi2 == '\\'; --dbi2) ++esc_count;
					if (!(esc_count & 1)) {
						--count;
						if (count == 0) {
							m_hlBracket.start = dbi.GetIndex();
							m_hlBracket.end = pos;
							return;
						}
					}
				}

				--dbi;
			}
		cxENDLOCK
	}
}

unsigned int EditorCtrl::RawDelete(unsigned int start, unsigned int end) {
	if (start == end) return 0;
	wxASSERT(end > start && end <= m_lines.GetLength());

	const unsigned int pos = m_lines.GetPos();

	if (!m_pairStack.empty()) {
		const interval& iv = m_pairStack.back();

		// Detect backspacing in active auto-pair
		if (end == iv.start && end == iv.end) {
			unsigned int nextpos;
			cxLOCKDOC_READ(m_doc)
				nextpos = doc.GetNextCharPos(iv.end);
			cxENDLOCK

			// Also delete pair ender
			end = nextpos;
			m_pairStack.pop_back();
		}
		else if (iv.start > start || iv.end < end) {
			// Reset autoPair state if deleting outside inner pair
			m_pairStack.clear();
		}
	}

	cxLOCKDOC_WRITE(m_doc)
		doc.Delete(start, end);
	cxENDLOCK
	m_lines.Delete(start, end);

	// Update stylers
	StylersDelete(start, end);

	// Update caret pos
	unsigned int del_len = end - start;
	if (pos > start) {
		if (pos <= end) m_lines.SetPos(start);
		else m_lines.SetPos(pos - del_len);
	}

	// Adjust containing pairs
	for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
		t->end -= del_len;
	}

	MarkAsModified();
	return del_len;
}

void EditorCtrl::RawMove(unsigned int start, unsigned int end, unsigned int dest) {
	wxASSERT(start <= end && end <= m_lines.GetLength());
	wxASSERT(dest <= start || dest >= end);
	wxASSERT(dest <= m_lines.GetLength());

	const unsigned int pos = m_lines.GetPos();

	if (start < end) {
		// Get the text and caret pos
		wxString text;
		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(start, end, text);
		cxENDLOCK

		// Delete source
		RawDelete(start, end);

		// Adjust destination
		const unsigned int len = end - start;
		if (dest >= end) {
			dest -= len;
		}

		// Insert at destination
		RawInsert(dest, text);
	}

	// If caret was in source, it moves with the text
	if (pos >= start && pos <= end) {
		m_lines.SetPos(dest + (pos - start));
	}

	// no need for MarkAsModified(), already called by subfunctions
}

unsigned int EditorCtrl::InsertNewline() {
	unsigned int pos = m_lines.GetPos();
	const unsigned int lineid = m_lines.GetCurrentLine();
	const bool atLineEnd = (pos == m_lines.GetLineEndpos(lineid, true));

	// Insert the newline
	unsigned int byte_len = RawInsert(pos, wxT("\n"), true);
	pos += byte_len;

	const wxString indent = GetNewIndentAfterNewline(lineid);

	// Check if indentation level should be increased
	if (!indent.empty()) {
		const unsigned int indent_len = RawInsert(pos, indent, false);
		pos += indent_len;
		byte_len += indent_len;

		// Check there is a line rest that should be moved to new line
		if (!atLineEnd) {
			// Get the correct indentation for new line
			const wxString newindent = GetRealIndent(lineid+1);
			const unsigned int indentlevel = CountIndent(indent);
			const unsigned int newindentlevel = CountIndent(newindent);

			// Only double the newlines if the new line will be de-dented
			if (newindentlevel < indentlevel) {
				const unsigned int newlinelen = RawInsert(pos, wxT('\n'), false);
				byte_len += newlinelen;
				const unsigned int newlinestart = pos + newlinelen;

				// Set correct indentation for new line
				if (!newindent.empty()) {
					byte_len += RawInsert(newlinestart, newindent, false);
				}
			}
		}
	}

	m_lines.SetPos(pos);
	lastpos = pos;
	lastaction = ACTION_INSERT;

	MarkAsModified();

	return byte_len;
}

void EditorCtrl::InsertChar(const wxChar& text) {
	if (m_snippetHandler.IsActive()) {
		m_snippetHandler.Insert(wxString(text));
		return;
	}

	// Check if we need to freeze last change
	unsigned int pos = m_lines.GetPos();
	if (pos != lastpos || lastaction != ACTION_INSERT) {
		cxLOCKDOC_WRITE(m_doc)
			doc.Freeze();
		cxENDLOCK
	}

	if (m_lines.IsSelected()) {
		// Don't freeze if we are inserting a single char over a
		// single selection
		if (m_lines.GetSelections().size() == 1) do_freeze = false;
		InsertOverSelections(wxString(text));
		do_freeze = true;
	}
	else {
		// Newline needs special handling for indentation
		if (text == wxT('\n')) {
			InsertNewline();
			return;
		}

		const unsigned int lineid = m_lines.GetCurrentLine();
		unsigned int lineend = m_lines.GetLineEndpos(lineid, true);
		const bool atLineEnd = (pos == lineend);

		// Insert the char
		RawInsert(pos, text, true);
		pos = m_lines.GetPos();

		if (atLineEnd) {
			// We only check for indentation change when inserting at end of line
			// this makes it possible to manually adjust indentation.
			const unsigned int linestart = m_lines.GetLineStartpos(lineid);
			lineend = pos;

			// Check if indentation level should be changed
			if (linestart < lineend) {
				int indentChange = 0;

				// Check if we should decrease indent
				const deque<const wxString*> scope = m_syntaxstyler.GetScope(linestart);
				const wxString& decreasePattern = m_syntaxHandler.GetIndentDecreasePattern(scope);
				if (!decreasePattern.empty()) {
					const search_result res = RegExFind(decreasePattern, linestart, false, NULL, lineend);
					if (res.error_code > 0) indentChange = -1;
				}

				// Check if we should increase indent
				// (if line above is a nextlineindenter and this one is not)
				if (indentChange == 0 && lineid > 0) {
					const wxString& nextPattern = m_syntaxHandler.GetIndentNextPattern(scope);
					if (!nextPattern.empty()) {
						const unsigned int linestart2 = m_lines.GetLineStartpos(lineid-1);
						const unsigned int lineend2 = m_lines.GetLineEndpos(lineid-1, true);
						const search_result res = RegExFind(nextPattern, linestart2, false, NULL, lineend2);
						if (res.error_code > 0) {
							const search_result res2 = RegExFind(nextPattern, linestart, false, NULL, lineend);
							if (res2.error_code <= 0) indentChange = 1;
						}
					}
				}

				if (indentChange != 0) {
					const unsigned int tabWidth = m_parentFrame.GetTabWidth();

					// Get the indentation len based on the line above
					const wxString currentIndent = lineid == 0 ? *wxEmptyString : GetNewIndentAfterNewline(lineid-1);
					int indentLen = 0;
					unsigned int spaces = 0;
					for (unsigned int i = 0; i < currentIndent.size(); ++i) {
						if (currentIndent[i] == wxT('\t')) {
							if (spaces == 0) ++indentLen;
							else break; // spaces smaller than tabWidth ends indent
						}
						else if (currentIndent[i] == wxT(' ')) {
							++spaces;
							if (spaces == tabWidth) {
								++indentLen;
								spaces = 0;
							}
						}
						else break;
					}

					// Adjust indent level
					indentLen = wxMax(indentLen + indentChange, 0);

					// Count len of current indentation
					unsigned int curLen = 0;
					spaces = 0;
					cxLOCKDOC_READ(m_doc)
						for (doc_byte_iter dbi(doc, linestart); (unsigned int)dbi.GetIndex() < lineend; ++dbi) {
							if (*dbi == '\t') {
							if (spaces == 0) ++curLen;
								else break; // spaces smaller than tabWidth ends indent
							}
							else if (*dbi == ' ') {
								++spaces;
								if (spaces == tabWidth) {
									++curLen;
									spaces = 0;
								}
							}
							else break;
						}
					cxENDLOCK

					// Adjust indentation
					const int indent_adj = indentLen - curLen;
					if (indent_adj > 0) {
						wxString indent;
						for (unsigned int ia = 0; (int)ia < indent_adj; ++ia) indent += m_indent;
						unsigned int indent_len;
						cxLOCKDOC_WRITE(m_doc)
							indent_len = doc.Insert(linestart, indent);
						cxENDLOCK
						m_lines.Insert(linestart, indent_len);
						StylersInsert(linestart, indent_len);

						pos += indent_len;
					}
					else if (indent_adj < 0) {
						unsigned int end = 0;
						// Calc how much should be deleted
						spaces = 0;
						cxLOCKDOC_READ(m_doc)
							doc_byte_iter dbi(doc, linestart);
							for (; (unsigned int)dbi.GetIndex() < lineend; ++dbi) {
								if (*dbi == '\t') {
								if (spaces == 0) ++end;
									else break; // spaces smaller than tabWidth ends indent
								}
								else if (*dbi == ' ') {
									++spaces;
									if (spaces == tabWidth) {
										++end;
										spaces = 0;
									}
								}
								else break;

								if ((int)end == -indent_adj) {
									++dbi;
									break;
								}
							}
							end = dbi.GetIndex();
						cxENDLOCK

						if (linestart < end) {
							cxLOCKDOC_WRITE(m_doc)
								doc.Delete(linestart, end);
							cxENDLOCK
							m_lines.Delete(linestart, end);
							StylersDelete(linestart, end);

							pos -= (end - linestart);
						}
					}
				}
			}
		}

		m_lines.SetPos(pos);
	}

	lastpos = m_lines.GetPos();
	lastaction = ACTION_INSERT;

	MarkAsModified();
}

void EditorCtrl::Insert(const wxString& text) {
	if (text.empty()) return;

	if (m_snippetHandler.IsActive()) {
		m_snippetHandler.Insert(text);
		return;
	}

	m_lines.Verify();

	if (m_lines.IsSelected()) InsertOverSelections(text);
	else {
		unsigned int pos = m_lines.GetPos();

		// Update the database
		unsigned int byte_len;
		cxLOCKDOC_WRITE(m_doc)
			byte_len = doc.Insert(pos, text);
		cxENDLOCK

		// Update the view
		m_lines.Insert(pos, byte_len);

		// Update stylers
		StylersInsert(pos, byte_len);

		// Make sure the caret is at the right position
		pos += byte_len;
		m_lines.SetPos(pos);

		// Draw the updated view
		DrawLayout();
	}

	MarkAsModified();
}

void EditorCtrl::Delete(unsigned int start, unsigned int end) {
	wxASSERT(end >= start && end <= m_lines.GetLength());
	if (start == end) return;
	m_lines.Verify(true);

	if (m_snippetHandler.IsActive()) {
		m_snippetHandler.Delete(start, end);
		return;
	}
	m_pairStack.clear(); // invalidate auto-pair state

	const unsigned int pos = m_lines.GetPos();

	cxLOCKDOC_WRITE(m_doc)
		doc.Delete(start, end);
	cxENDLOCK
	m_lines.Delete(start, end);

	// Update stylers
	StylersDelete(start, end);

	// Update caret pos
	unsigned int del_len = end - start;
	if (pos > start) {
		if (pos <= end) m_lines.SetPos(start);
		else m_lines.SetPos(pos - del_len);
	}

	MarkAsModified();
}

void EditorCtrl::Commit(const wxString& label, const wxString& desc) {
	cxLOCKDOC_WRITE(m_doc)
		doc.Commit(label, desc);
	cxENDLOCK
}

void EditorCtrl::Clear() {
	cxLOCKDOC_WRITE(m_doc)
		doc.Clear();
	cxENDLOCK
	m_lines.Clear();
	scrollPos = 0;
	topline = -1;
	m_currentSel = -1;

	// Draw the updated view
	DrawLayout();

	MarkAsModified();
}

bool EditorCtrl::IsEmpty() const {
	if (!m_path.IsOk()) {
		cxLOCKDOC_READ(m_doc)
			if (doc.IsEmpty()) return true;
		cxENDLOCK
	}

	return false;
}

cxFileResult EditorCtrl::LoadText(const wxFileName& newpath) {
	// Check if we already have set an encoding
	wxFontEncoding enc;
	cxLOCKDOC_READ(m_doc)
		enc = doc.GetPropertyEncoding();
	cxENDLOCK

	return LoadText(newpath.GetFullPath(), enc);
}

cxFileResult EditorCtrl::LoadText(const wxString& newpath, const RemoteProfile* rp) {
	// Check if we already have set an encoding
	wxFontEncoding enc;
	cxLOCKDOC_READ(m_doc)
		enc = doc.GetPropertyEncoding();
	cxENDLOCK

	return LoadText(newpath, enc, rp);
}

cxFileResult EditorCtrl::LoadText(const wxString& newpath, wxFontEncoding enc, const RemoteProfile* rp) {
	cxFileResult result = cxFILE_OK;
	wxFileName filepath;

	if (newpath.StartsWith(wxT("bundle://"))) {
		if (!LoadBundleItem(newpath)) return cxFILE_OPEN_ERROR;
	}
	else {
		// First clean up old remote info (and delete evt. buffer file);
		ClearRemoteInfo();

		// If the path points to a remote file, we have to download it first.
		if (m_parentFrame.IsRemotePath(newpath)) {
			m_remoteProfile = rp ? rp : m_parentFrame.GetRemoteProfile(newpath, false);
			const wxString buffPath = m_parentFrame.DownloadFile(newpath, m_remoteProfile);
			if (buffPath.empty()) return cxFILE_DOWNLOAD_ERROR; // download failed

			filepath = buffPath;
			m_remotePath = newpath;
		}
		else {
			filepath = newpath;
		}

		// Invalidate all stylers
		StylersInvalidate();

		result = m_lines.LoadText(filepath, enc, m_remotePath);
	}
	if (result != cxFILE_OK) return result;

	scrollPos = 0;
	topline = -1;
	m_currentSel = -1;

	m_modSkipDate = wxInvalidDateTime;
	MarkAsModified();

	// re-set the width
	// Has to be bigger than zero to avoid problems during redraw.
	UpdateEditorWidth();

	SetPath(filepath.GetFullPath()); // also updates syntax

	return result;
}

bool EditorCtrl::CheckBundleItemModified() const {
	wxASSERT(IsBundleItem());

	// Get information about bundle item
	BundleItemType bundleType;
	unsigned int bundleId;
	unsigned int itemId;
	const PListHandler& plistHandler = m_syntaxHandler.GetPListHandler();
	if (!plistHandler.GetBundleItemFromUri(m_remotePath, bundleType, bundleId, itemId)) return false;
	const PListDict itemDict = plistHandler.Get(bundleType, bundleId, itemId);

	// Get mirror
	doc_id di;
	wxDateTime modDate;
	cxLOCK_READ(m_catalyst)
		if (!catalyst.GetFileMirror(m_remotePath, di, modDate)) return false;
	cxENDLOCK

	if (modDate != itemDict.GetModDate()) {
		// Ask user if we should reload
		return true;
	}

	return false;
}

bool EditorCtrl::LoadBundleItem(const wxString& bundleUri) {
	// Get information about bundle item
	unsigned int bundleId;
	unsigned int itemId;
	const PListHandler& plistHandler = m_syntaxHandler.GetPListHandler();
	if (!plistHandler.GetBundleItemFromUri(bundleUri, m_bundleType, bundleId, itemId)) return false;
	const PListDict itemDict = plistHandler.Get(m_bundleType, bundleId, itemId);

	// Invalidate all stylers
	StylersInvalidate();

	// Check if we already have a mirror
	bool doReload = true;
	doc_id di;
	wxDateTime modDate;
	bool hasMirror;
	cxLOCK_READ(m_catalyst)
		hasMirror = catalyst.GetFileMirror(bundleUri, di, modDate);
	cxENDLOCK
	if (hasMirror) {
		if (modDate == itemDict.GetModDate()) { // item unchanged since mirror?
			if (di == GetDocID()) return true; // already loaded
			doReload = false;
		}
		SetDocument(di, bundleUri);
	}

	if (doReload) {
		// Set item name and encoding
		cxLOCKDOC_WRITE(m_doc)
			if (doc.IsEmpty()) doc.Clear(true); // make this initial rev
			doc.DeleteAll();
			doc.SetPropertyName(itemDict.wxGetString("name"));
			doc.SetPropertyEncoding(wxFONTENCODING_UTF8); // bundle contents is always utf8
			doc.SetPropertyEOL(wxTextFileType_Unix);
			doc.SetProperty(wxT("bundle:uuid"), itemDict.wxGetString("uuid"));
		cxENDLOCK

		// Load the item contents
		switch (m_bundleType) {
			case BUNDLE_SNIPPET:
				{
					const char* co = itemDict.GetString("content");
					cxLOCKDOC_WRITE(m_doc)
						if (co) doc.Insert(0, co);

						// Set properties
						doc.SetProperty(wxT("bundle:keyEquivalent"), itemDict.wxGetString("keyEquivalent"));
						doc.SetProperty(wxT("bundle:tabTrigger"), itemDict.wxGetString("tabTrigger"));
						doc.SetProperty(wxT("bundle:scope"), itemDict.wxGetString("scope"));
					cxENDLOCK
					m_lines.ReLoadText();
				}
				break;

			case BUNDLE_COMMAND:
			case BUNDLE_DRAGCMD:
				{
					bool winCommand = true;
					const char* co = itemDict.GetString("winCommand");
					if (!co) {
						co = itemDict.GetString("command");
						winCommand = false;
					}
					if (co) {
						cxLOCKDOC_WRITE(m_doc)
							doc.Insert(0, co);

							// Set properties (for historical reasons runEnvironment is a bit cumbersome)
							const wxString runEnvironment = itemDict.wxGetString("runEnvironment");
							if (!runEnvironment.empty()) doc.SetProperty(wxT("bundle:runEnvironment"), runEnvironment);
							else if (winCommand) doc.SetProperty(wxT("bundle:runEnvironment"), wxT("cygwin"));

							if (m_bundleType == BUNDLE_COMMAND) {
								doc.SetProperty(wxT("bundle:beforeRunningCommand"), itemDict.wxGetString("beforeRunningCommand"));
								doc.SetProperty(wxT("bundle:input"), itemDict.wxGetString("input"));
								doc.SetProperty(wxT("bundle:fallbackInput"), itemDict.wxGetString("fallbackInput"));
								doc.SetProperty(wxT("bundle:output"), itemDict.wxGetString("output"));

								doc.SetProperty(wxT("bundle:keyEquivalent"), itemDict.wxGetString("keyEquivalent"));
								doc.SetProperty(wxT("bundle:tabTrigger"), itemDict.wxGetString("tabTrigger"));
							}
							else { // m_bundleType == BUNDLE_DRAGCMD
								// Get file extensions
								PListArray extArray;
								if (itemDict.GetArray("draggedFileExtensions", extArray)) {
									wxString fileTypes;
									for (unsigned int i = 0; i < extArray.GetSize(); ++i) {
										if (i) fileTypes += wxT(", ");
										fileTypes += extArray.wxGetString(i);
									}
									doc.SetProperty(wxT("bundle:draggedFileExtensions"), fileTypes);
								}
								else doc.DeleteProperty(wxT("bundle:draggedFileExtensions"));
							}

							doc.SetProperty(wxT("bundle:scope"), itemDict.wxGetString("scope"));
						cxENDLOCK
						m_lines.ReLoadText();
					}
				}
				break;

			case BUNDLE_LANGUAGE:
				{
					const wxString jsonSettings = itemDict.GetJSON(true /*strip*/);
					cxLOCKDOC_WRITE(m_doc)
						doc.Insert(0, jsonSettings);

						// Set properties
						doc.SetProperty(wxT("bundle:keyEquivalent"), itemDict.wxGetString("keyEquivalent"));
					cxENDLOCK
					m_lines.ReLoadText();

					m_syntaxstyler.SetSyntax(wxT("JSON"));
				}
				break;

			case BUNDLE_PREF:
				{
					wxString jsonSettings = wxT("{ }"); // default
					PListDict settingsDict;
					if (itemDict.GetDict("settings", settingsDict)) {
						jsonSettings = settingsDict.GetJSON();
					}
					
					cxLOCKDOC_WRITE(m_doc)
						doc.Insert(0, jsonSettings);

						// Set properties
						doc.SetProperty(wxT("bundle:scope"), itemDict.wxGetString("scope"));
					cxENDLOCK
					m_lines.ReLoadText();

					m_syntaxstyler.SetSyntax(wxT("JSON"));
				}
				break;

			default:
				wxASSERT(false);
		}
		Freeze();

		// Set mirror
		modDate = itemDict.GetModDate();
		di = GetDocID();
		cxLOCK_WRITE(m_catalyst)
			catalyst.SetFileMirror(bundleUri, di, modDate);
		cxENDLOCK
	}

	m_remotePath = bundleUri;

	// Set initial syntax
	switch (m_bundleType) {
		case BUNDLE_SNIPPET:
			m_syntaxstyler.SetSyntax(wxT("Snippet"));
			break;
		case BUNDLE_COMMAND:
		case BUNDLE_DRAGCMD:
			if (!m_syntaxstyler.UpdateSyntax()) {
				if (itemDict.HasKey("winCommand")) m_syntaxstyler.SetSyntax(wxT("MSDOS batch file")); // default syntax for windows native
				else m_syntaxstyler.SetSyntax(wxT("Shell Script (Bash)")); // default syntax
			}
			break;
		case BUNDLE_LANGUAGE:
		case BUNDLE_PREF:
			m_syntaxstyler.SetSyntax(wxT("JSON"));
			break;
		default: wxASSERT(false);
	}

	// Update Bundle panels (shows properties)
	if (m_bundlePanel) m_bundlePanel->UpdatePanel();
	else wxASSERT(false);

	return true;
}

bool EditorCtrl::SaveBundleItem(bool WXUNUSED(duplicate)) {
	wxASSERT(IsBundleItem());

	unsigned int bundleId;
	unsigned int itemId;

	// Check if bundle item still exists (may have been deleted by a bundle update)
	PListHandler& plistHandler = m_syntaxHandler.GetPListHandler();
	if (!plistHandler.GetBundleItemFromUri(m_remotePath, m_bundleType, bundleId, itemId)) {
		wxMessageBox(_("Bundle item has been deleted"), _("Could not save bundle item!"), wxICON_ERROR|wxOK, this);
		return false;
	}

	// Only save if there are changes
	if (!IsModified()) return true;

	PListDict itemDict = plistHandler.GetEditable(m_bundleType, bundleId, itemId);
	if (!itemDict.IsOk()) return false;

	cxLOCKDOC_READ(m_doc)
		// Set new bundle contents
		switch (m_bundleType) {
			case BUNDLE_COMMAND:
			case BUNDLE_DRAGCMD:
				{
					// runEnvironment and contents
					wxString runEnv;
					doc.GetProperty(wxT("bundle:runEnvironment"), runEnv);
					
					if (runEnv == wxT("windows")) itemDict.SetString("runEnvironment", "windows");
					else itemDict.DeleteItem("runEnvironment");

					if (runEnv.empty()) itemDict.DeleteItem("winCommand");
					const char* co = runEnv.empty() ? "command" : "winCommand";
					if (doc.GetLength()) {
						vector<char> buffer;
						doc.GetTextPart(0, doc.GetLength(), buffer);
						buffer.push_back('\0');
						itemDict.SetString(co, &*buffer.begin());
					}
					else itemDict.SetString(co, "");

					// Get Properties
					if (m_bundleType == BUNDLE_COMMAND) {
						wxString beforeRunningCommand;
						wxString input;
						wxString fallbackInput;
						wxString output;
						wxString keyEquivalent;
						wxString tabTrigger;
						doc.GetProperty(wxT("bundle:beforeRunningCommand"), beforeRunningCommand);
						doc.GetProperty(wxT("bundle:input"), input);
						doc.GetProperty(wxT("bundle:fallbackInput"), fallbackInput);
						doc.GetProperty(wxT("bundle:output"), output);
						doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
						doc.GetProperty(wxT("bundle:tabTrigger"), tabTrigger);

						if (!beforeRunningCommand.empty()) itemDict.wxSetString("beforeRunningCommand", beforeRunningCommand);
						if (!output.empty()) itemDict.wxSetString("input", input);
						if (!output.empty()) itemDict.wxSetString("output", output);
						if (!keyEquivalent.empty()) itemDict.wxSetString("keyEquivalent", keyEquivalent); else itemDict.DeleteItem("keyEquivalent");
						if (!tabTrigger.empty()) itemDict.wxSetString("tabTrigger", tabTrigger); else itemDict.DeleteItem("tabTrigger");
						if (input == wxT("selection") && !fallbackInput.empty()) itemDict.wxSetString("fallbackInput", fallbackInput); else itemDict.DeleteItem("fallbackInput");
					}
					else { // BUNDLE_DRAGCMD
						PListArray extArray = itemDict.NewArray("draggedFileExtensions");
						extArray.Clear();

						wxString exts;
						doc.GetProperty(wxT("bundle:draggedFileExtensions"), exts);
						wxStringTokenizer tokens(exts, wxT(" ,"), wxTOKEN_STRTOK);

						for (wxString tok = tokens.GetNextToken(); !tok.empty(); tok = tokens.GetNextToken()) {
							extArray.AddString(tok.mb_str(wxConvUTF8));
						}
					}
					
					wxString scope;
					doc.GetProperty(wxT("bundle:scope"), scope);
					itemDict.wxSetString("scope", scope);
				}
				break;

			case BUNDLE_SNIPPET:
				{
					// contents
					if (doc.GetLength()) {
						vector<char> buffer;
						doc.GetTextPart(0, doc.GetLength(), buffer);
						buffer.push_back('\0');
						itemDict.SetString("content", &*buffer.begin());
					}
					else itemDict.SetString("content", "");

					wxString keyEquivalent;
					wxString tabTrigger;
					wxString scope;
					doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
					doc.GetProperty(wxT("bundle:tabTrigger"), tabTrigger);
					doc.GetProperty(wxT("bundle:scope"), scope);

					if (!keyEquivalent.empty()) itemDict.wxSetString("keyEquivalent", keyEquivalent); else itemDict.DeleteItem("keyEquivalent");
					if (!tabTrigger.empty()) itemDict.wxSetString("tabTrigger", tabTrigger); else itemDict.DeleteItem("tabTrigger");
					itemDict.wxSetString("scope", scope);
				}
				break;

			case BUNDLE_PREF:
			case BUNDLE_LANGUAGE:
				{
					const wxString jsonStr = doc.GetText();

					// Parse the JSON string
					wxJSONReader reader;
					wxJSONValue root;
					const int numErrors = reader.Parse(jsonStr, &root);
					if ( numErrors > 0 )  {
						// if there are errors in the JSON document, print the errors 
						wxString msg = _("<h2>Invalid JSON syntax:</h2>\n");
						const wxArrayString& errors = reader.GetErrors();
						wxRegEx reLineCol(wxT("line ([[:digit:]]+), col ([[:digit:]]+)"));
						for ( int i = 0; i < numErrors; i++ )  {
							if (i) msg += wxT("<br>\n");

							const wxString& error = errors[i];
							if (reLineCol.Matches(error)) {
								size_t matchStart;
								size_t matchLen;
								reLineCol.GetMatch(&matchStart, &matchLen);
								const wxString line = reLineCol.GetMatch(error, 1);
								const wxString col = reLineCol.GetMatch(error, 2);
								
								msg += error.substr(0, matchStart);
								msg += wxT("<a href=\"txmt://open?") + m_remotePath + wxT("&line=") + line + wxT("&column=") + col;
								msg += wxT("\">") + error.substr(matchStart, matchLen) + wxT("</a>") + error.substr(matchStart+matchLen);
							}
							else msg += error;
						}
						m_parentFrame.ShowOutput(_("Syntax error"), msg);
						return false;
					}

					if (m_bundleType == BUNDLE_PREF) {
						PListDict settingsDict;
						if (!itemDict.GetDict("settings", settingsDict)) settingsDict = itemDict.NewDict("settings");

						settingsDict.SetJSON(root);

						wxString scope;
						doc.GetProperty(wxT("bundle:scope"), scope);
						itemDict.wxSetString("scope", scope);
					}
					else { // BUNDLE_LANGUAGE
						itemDict.SetJSON(root, true /*strip*/);

						wxString keyEquivalent;
						doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
						if (!keyEquivalent.empty()) itemDict.wxSetString("keyEquivalent", keyEquivalent); else itemDict.DeleteItem("keyEquivalent");
					}
				}
				break;

			default: wxASSERT(false);
		}
	cxENDLOCK

	// Save to plist
	if (!plistHandler.Save(m_bundleType, bundleId, itemId)) return false;

	// Update mirror
	const wxDateTime modDate = itemDict.GetModDate();
	const doc_id di = GetDocID();
	cxLOCK_WRITE(m_catalyst)
		catalyst.SetFileMirror(m_remotePath, di, modDate);
	cxENDLOCK

	// Reload bundles
	wxBusyCursor wait; // Show user that we are reloading
	if (m_bundleType == BUNDLE_PREF || m_bundleType == BUNDLE_LANGUAGE) {
		// we have to call LoadBundles since all syntaxes will have to be reloaded
		m_syntaxHandler.LoadBundles(TmSyntaxHandler::cxRELOAD);
	}
	else m_syntaxHandler.ReParseBundles();

	return true;
}

bool EditorCtrl::SaveText(bool askforpath) {
	if (IsBundleItem()) return SaveBundleItem(askforpath);

	// We always have to ask for the path if we don't have it
	if (!m_path.IsOk()) askforpath = true;

	wxFileName filepath;
	bool savedLocal = true;

	wxString docName;
	cxLOCKDOC_WRITE(m_doc)
		docName = doc.GetPropertyName();
	cxENDLOCK

	wxString newpath;
	if (m_remotePath.empty()) {
		if (!m_path.IsOk()) {
			const wxFileName path(m_parentFrame.GetSaveDir(), docName);
			newpath = path.GetFullPath();
		}
		else {
			if (m_path.GetFullName() != docName && !docName.empty()) {
				// The filename has been changed.
				// Probably from reversing to a previous revision
				m_path.SetFullName(docName);
			}
			newpath = m_path.GetFullPath();
		}
	}
	else {
		savedLocal = false;
		newpath = docName;
	}

	if (askforpath || newpath.empty()) {
		wxString filters = wxT("All files (*.*)|*.*|Text files (*.txt)|*.txt|") \
						wxT("Batch Files (*.bat)|*.bat|INI Files (*.ini)|*.ini|") \
						wxT("C/C++ Files (*.c, *.cpp, *.cxx)|*.c;*.cpp;*.cxx|") \
						wxT("Header Files (*.h, *.hpp, *.hxx)|*.h;*.hpp;*.hxx|") \
						wxT("HTML Files (*.html, *.htm, *.css)|*.html;*.htm;*.css|") \
						wxT("Perl Files (*.pl, *.pm, *.pod)|*.pl;*.pm;*.pod|") \
						wxT("Python Files (*.py, *.pyw)|*.py;*.pyw");
						// Also defined in EditorFrame::OnMenuOpen()

		wxFileDialog dlg(this, _T("Save as..."),
						_T(""), _T(""), filters,
						wxSAVE|wxCHANGE_DIR);

		if (!newpath.empty()) dlg.SetPath(newpath);
		else dlg.SetPath(_("Untitled"));

		dlg.Centre();
		if (dlg.ShowModal() != wxID_OK) return false;
		newpath = dlg.GetPath();

		if (wxFileExists(newpath)) {
			const int answer = wxMessageBox(newpath + _T(" already exists. Do you want to replace it?"), _T("e Warning"), wxICON_QUESTION|wxOK|wxCANCEL);
			if (answer != wxOK) return false;
		}

		// Verify that the path is valid
		filepath = newpath;
		filepath.MakeAbsolute();
		if (!filepath.IsOk()) {
			wxMessageBox(filepath.GetFullPath() + _T(" is not a file!"), _T("e Error"), wxICON_ERROR);
			return false;
		}
		if (filepath.IsDir()) {
			wxMessageBox(filepath.GetFullPath() + _T(" is a directory, not a file!"), _T("e Error"), wxICON_ERROR);
			return false;
		}

		savedLocal = true;
	}
	else filepath = m_path; // path to temp buffer file

	// Check if file is write protected
	if (filepath.FileExists() && !filepath.IsFileWritable()) {
		const int answer = wxMessageBox(filepath.GetFullPath() + _T(" is write protected.\nDo you want to save it anyway?"), _T("Save"), wxOK|wxCANCEL|wxICON_QUESTION);
		if (answer == wxOK) {
#ifdef __WXMSW__
			DWORD dwAttrs = ::GetFileAttributes(newpath);
			if (dwAttrs & FILE_ATTRIBUTE_READONLY) {
				dwAttrs &= ~FILE_ATTRIBUTE_READONLY; // remove read-only bit
				if (!::SetFileAttributes(newpath, dwAttrs)) {
					wxMessageBox(_T("Unable to remove write protection!"), _T("e Error"), wxICON_ERROR);
					return false;
				}
			}
#else
            // Get protection
			struct stat s;
			int res = stat(newpath.mb_str(wxConvUTF8), &s);
			if (res == 0) res = chmod(newpath.mb_str(wxConvUTF8), s.st_mode |= S_IWUSR);
			if (res != 0) {
			    wxMessageBox(_T("Unable to remove write protection!"), _T("e Error"), wxICON_ERROR);
                return false;
			}
#endif
		}
		else return false;
	}

	// Check if we need to force the native end-of-line
	bool forceNativeEOL = false; // default value
	((eApp*)wxTheApp)->GetSettingBool(wxT("force_native_eol"), forceNativeEOL);

	// Save the text
	cxFileResult savedResult;
	const wxString realname = m_remotePath.empty() ? wxT("") : docName;
	cxLOCKDOC_WRITE(m_doc)
		savedResult = doc.SaveText(filepath, forceNativeEOL, realname);
	cxENDLOCK

	const wxString pathStr = filepath.GetFullPath();

	switch (savedResult) {
	case cxFILE_OK:
		// Clean up remote info if file was saved locally
		if (!m_remotePath.empty() && savedLocal) {
			ClearRemoteInfo();
		}

		SetPath(pathStr);

		MarkAsModified();
		m_savedForPreview = true;
		break;

	case cxFILE_CONV_ERROR:
		wxMessageBox(_T("Could not convert file: ") + pathStr + _T("\nIt may contain chars that cannot be represented in current encoding. Try saving in another encoding."), _T("e Error"), wxICON_ERROR);
		return false;

	case cxFILE_WRITABLE_ERROR:
		wxMessageBox(pathStr + _T(" is write protected."), _T("e Error"), wxICON_ERROR);
		return false;

	default:
		wxMessageBox(_T("Could not save file: ") + pathStr, _T("e Error"), wxICON_ERROR);
		return false;
	}

	if (!m_remotePath.empty()) {
		while(1) {
			// Upload file
			wxBeginBusyCursor();
			const CURLcode res =  m_parentFrame.GetRemoteThread().UploadAndDate(m_remotePath, pathStr, *m_remoteProfile);
			wxEndBusyCursor();

			if (res == CURLE_LOGIN_DENIED) {
				if (!m_parentFrame.AskRemoteLogin(m_remoteProfile)) return false;
			}
			else if (res != CURLE_OK) {
				wxString msg = _T("Could not upload file: ") + m_remotePath;
				msg += wxT("\n") + m_parentFrame.GetRemoteThread().GetLastError();
				wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
				return false;
			}
			else break; // download succeded
		}

		// Set mirror to new modDate so it matches file on server
		const wxDateTime modDate = filepath.GetModificationTime();
		const doc_id di = GetDocID();
		cxLOCK_WRITE(m_catalyst)
			catalyst.SetFileMirror(m_remotePath, di, modDate);
		cxENDLOCK
	}

	return true;
}

void EditorCtrl::SetPath(const wxString& newpath) {
	wxASSERT(!m_parentFrame.IsRemotePath(newpath)); // just to catch a bug
	
	if (IsBundleItem()) return; // bundleItem only has remotePath

	if (m_path.GetFullPath() != newpath) {
		m_path = newpath;
		wxASSERT(m_path.IsAbsolute());

		// Clear the env var cache
		m_tmFilePath.clear();
		m_tmDirectory.clear();

		// Set the syntax to match the new path
		if (m_syntaxstyler.UpdateSyntax()) {
			// Redraw (since syntax have changed)
			DrawLayout();
		}
	}
}

bool EditorCtrl::IsModified() const {
	// When is a document in modified state:
	//   1. If it is mirrored but doc_id has changed
	if (m_path.IsOk() || IsBundleItem()) {
		doc_id di;
		wxDateTime modifiedDate;
		const wxString mirror = m_remotePath.empty() ? m_path.GetFullPath() : m_remotePath;
		bool hasMirror;
		cxLOCK_READ(m_catalyst)
			hasMirror = catalyst.GetFileMirror(mirror, di, modifiedDate);
		cxENDLOCK

		if(hasMirror) {
			if (!modifiedDate.IsValid()) return true;
			else if (m_doc == di) return false;
			else return true;
		}
		else {
			wxASSERT(false); // cannot have a path and not be mirrored
			return true;
		}
	}

	//   2. If it is an un-mirrored but unattached and non-empty draft
	cxLOCKDOC_READ(m_doc)
		if (doc.IsDraft() && !doc.IsDraftAttached() && !doc.IsEmpty() ) return true;
	cxENDLOCK

	// If we reach here the doc is un-modified
	return false;
}

bool EditorCtrl::SetDocument(const doc_id& di, const wxString& path, const RemoteProfile* rp) {
	// No reason to set doc if we are already there
	doc_id oldDoc;
	cxLOCKDOC_READ(m_doc)
		oldDoc = doc.GetDocument();
	cxENDLOCK
	if (di == oldDoc) return true;

	// If the current doc is an clean draft (no revs & no parent)
	// we have to remember to delete it after setting the new doc
	bool deleteOld = oldDoc.IsOk() && IsEmpty();

	topline = -1;
	m_currentSel = -1;
	m_pairStack.empty(); // reset autoPair state

	bool inSameHistory = false;
	if (oldDoc.IsOk()) {
		cxLOCK_READ(m_catalyst)
			inSameHistory = catalyst.InSameHistory(oldDoc, di);
		cxENDLOCK
	}

	if (inSameHistory) {
		// Reload entire document
		cxLOCKDOC_WRITE(m_doc)
			doc.SetDocument(di);
		cxENDLOCK
		
		ApplyDiff(oldDoc, true /*moveToFirstChange*/);

		// If the syntax has been manually set, we want to preserve it
		if (!m_syntaxstyler.IsOk()) m_syntaxstyler.UpdateSyntax();
	}
	else {
		// This is a new document, clear up old state
		ClearRemoteInfo();

		// Invalidate all stylers
		// (has to be done before reloading to avoid stale refs)
		StylersInvalidate();

		cxLOCKDOC_WRITE(m_doc)
			doc.SetDocument(di);
		cxENDLOCK

		// If the path points to a remote file, we have to save it to a temp bufferfile first.
		if (m_parentFrame.IsRemotePath(path)) {
			m_remoteProfile = rp ? rp : m_parentFrame.GetRemoteProfile(path, false);
			m_path = m_parentFrame.GetTempPath();
			m_remotePath = path;

			cxLOCKDOC_WRITE(m_doc)
				const cxFileResult res = doc.SaveText(m_path, false, m_remotePath, true); // keep date
				if (res != cxFILE_OK) return false;
			cxENDLOCK
		}
		else if (!path.empty()) m_path = path;

		scrollPos = 0;
		m_lines.ReLoadText();

		// Set the syntax to match the new filename
		m_syntaxstyler.UpdateSyntax();

		// re-set the width
		UpdateEditorWidth();
	}
	cxLOCKDOC_WRITE(m_doc)
		doc.SetDocRead();
		doc.MakeHead(); // Set the current document to be head
	cxENDLOCK

	if (deleteOld) {
		cxLOCK_WRITE(m_catalyst)
			catalyst.DeleteDraft(oldDoc);
		cxENDLOCK
	}

	m_lines.Verify(true); // DEBUG

	// We mark as modified before redrawing to avoid stale refs i symbol cache
	MarkAsModified();

	// Draw the updated view
	DrawLayout();
	SetFocus();

	// Update the path (title)
	m_parentFrame.SetPath();

	// If this is a bundle item we also have to update the panel
	if (m_bundlePanel) m_bundlePanel->UpdatePanel();

	// Notify that we have changed document
	const doc_id docId = di;
	dispatcher.Notify(wxT("WIN_CHANGEDOC"), this, GetId());

	return true;
}

void EditorCtrl::ApplyDiff(const doc_id& oldDoc, bool moveToFirstChange) {
	vector<cxChange> changes;
	cxLOCKDOC_READ(m_doc)
		const doc_id di = doc.GetDocument();
		changes = doc.GetChanges(oldDoc, di);
	cxENDLOCK
	
	// Lines has to be made valid first
	m_lines.ApplyDiff(changes);

	// When applying changes to the syntax, we have to be very carefull
	// not to do any reads of text from stale refs. To avoid this we only
	// apply changes as full lines.
	const vector<cxLineChange> linechanges = m_lines.ChangesToFullLines(changes);


	m_syntaxstyler.ApplyDiff(linechanges);
	FoldingApplyDiff(linechanges);
	m_search_hl_styler.ApplyDiff(changes);
	BookmarksApplyDiff(changes);

	// Move caret to position of first change
	if (moveToFirstChange) {
		m_lines.RemoveAllSelections();
		if (!changes.empty()) {
			const unsigned int changePos = changes[0].start;
			SetPos(changePos);
			wxLogDebug(wxT("changepos: %d"), changePos);
		}
		if (!IsCaretVisible()) {
			MakeCaretVisibleCenter();
		}
	}
}

interval EditorCtrl::UndoSelection(const cxDiffEntry& de) {
	wxASSERT(m_lines.IsSelected());

	const vector<interval>& selections = m_lines.GetSelections();
	if (selections.empty()) return interval(0,0);

	if (lastaction != ACTION_UNDOSEL) {
		Freeze();
		lastaction = ACTION_UNDOSEL;
	}

	// Delete current selection
	interval sel = selections[0];
	const bool caretfront = (m_lines.GetPos() == sel.start);
	RawDelete(sel.start, sel.end);

	unsigned int byte_len = 0;
	if (de.range.start < de.range.end) {
		// Get the text from target version
		wxString text;
		cxLOCKDOC_READ(m_doc)
			doc_id di = doc.GetDocument();
			di.version_id = de.version;
			text = doc.GetTextPart(di, de.range.start, de.range.end);
		cxENDLOCK

		// Insert new text
		byte_len = RawInsert(sel.start, text);
	}

	// Update selection
	sel.end = sel.start + byte_len;
	m_lines.RemoveAllSelections();
	m_lines.AddSelection(sel.start, sel.end);

	// Make sure caret follows selection
	if (!caretfront) m_lines.SetPos(sel.end);

	return sel;
}

void EditorCtrl::StylersClear() {
	m_search_hl_styler.Clear();
	m_syntaxstyler.Clear();
	FoldingClear();
}

void EditorCtrl::StylersInvalidate() {
	m_search_hl_styler.Invalidate();
	m_syntaxstyler.Invalidate();
	FoldingInvalidate();
}

void EditorCtrl::StylersInsert(unsigned int pos, unsigned int length) {
	m_search_hl_styler.Insert(pos, length);
	m_syntaxstyler.Insert(pos, length);
	FoldingInsert(pos, length);
	BookmarksInsert(pos, length);
}

void EditorCtrl::StylersDelete(unsigned int start, unsigned int end) {
	m_search_hl_styler.Delete(start, end);
	m_syntaxstyler.Delete(start, end);
	FoldingDelete(start, end);
	BookmarksDelete(start, end);
}

unsigned int EditorCtrl::GetChangePos(const doc_id& old_version_id) const {
	wxASSERT(old_version_id.IsOk());

	// Get the diff
	doc_id new_version_id;
	vector<match> matchlist;
	cxLOCKDOC_READ(m_doc)
		new_version_id = doc.GetDocument();

		wxASSERT(old_version_id != new_version_id);
#ifdef __WXDEBUG__
		const Catalyst& catalyst = m_catalyst.GetCatalyst();
		wxASSERT(catalyst.InSameHistory(new_version_id, old_version_id));
#endif  //__WXDEBUG__

		matchlist = doc.Diff(new_version_id, old_version_id);
	cxENDLOCK

	// Return the position of the first change
	if (matchlist.empty()) return 0; // everything changed
	else if (matchlist[0].iv1_start_pos > 0) return 0; // insertion at top
	else if (matchlist[0].iv2_start_pos > 0) return 0; // deletion at top
	else {
		return matchlist[0].iv1_end_pos;
	}
}

void EditorCtrl::RemapPos(const doc_id& old_version_id, unsigned int old_pos, const vector<interval>& old_sel, unsigned int toppos) {
	wxASSERT(old_version_id.IsOk());

	doc_id new_version_id;
	vector<match> matchlist;
	cxLOCKDOC_READ(m_doc)
		new_version_id = doc.GetDocument();

		wxASSERT(old_version_id != new_version_id);
#ifdef __WXDEBUG__
		const Catalyst& catalyst = m_catalyst.GetCatalyst();
		wxASSERT(catalyst.InSameHistory(new_version_id, old_version_id));
#endif  //__WXDEBUG__

		matchlist = doc.Diff(new_version_id, old_version_id);
	cxENDLOCK

	int newpos = -1;
	unsigned int current_sel = 0;
	int selstart = -1;
	int newtoppos = -1;
	int prev_end = 0;
	unsigned int i = 0;
	while (i < matchlist.size()) {
		match m = matchlist[i];

		// Remap cursor position
		if (newpos == -1) {
			if (old_pos < m.iv2_start_pos) newpos = prev_end;
			else if (old_pos == m.iv2_start_pos) newpos = m.iv1_start_pos;
			else if (m.iv2_start_pos < old_pos && old_pos <= m.iv2_end_pos) {
				// Translate pos to new pos
				const int offset = old_pos - m.iv2_start_pos;
				newpos = m.iv1_start_pos + offset;
			}
		}

		// Remap selections
		while (current_sel < old_sel.size()) {
			interval iv = old_sel[current_sel];

			if (selstart == -1) {
				if (iv.start <= m.iv2_start_pos) selstart = prev_end;
				else if (m.iv2_start_pos < iv.start && iv.start <= m.iv2_end_pos) {
					// Translate pos to new pos
					const int offset = iv.start - m.iv2_start_pos;
					selstart = m.iv1_start_pos + offset;
				}
				else break;
			}
			if (selstart != -1) {
				int selend = -1;
				if (iv.end <= m.iv2_start_pos) selend = prev_end;
				else if (m.iv2_start_pos < iv.end && iv.end <= m.iv2_end_pos) {
					// Translate pos to new pos
					const int offset = iv.end - m.iv2_start_pos;
					selend = m.iv1_start_pos + offset;
				}

				if (selend != -1) {
					if (selstart < selend) m_lines.AddSelection(selstart, selend);
					selstart = -1;
					++current_sel;
				}
				else break;
			}
		}

		// Remap top pos
		if (newtoppos == -1) {
			if (toppos <= m.iv2_start_pos) newtoppos = prev_end;
			else if (m.iv2_start_pos < toppos && toppos <= m.iv2_end_pos) {
				// Translate pos to new pos
				const int offset = toppos - m.iv2_start_pos;
				newtoppos = m.iv1_start_pos + offset;
			}
		}

		prev_end = m.iv1_end_pos;
		++i;
	}
	wxASSERT(newpos != -1);
	wxASSERT(newtoppos != -1);

	m_lines.SetPos(newpos);

	const wxPoint cp = m_lines.GetCharPos(newtoppos);
	scrollPos = cp.y;
}

void EditorCtrl::DeleteSelections() {
	if(!m_lines.IsSelected()) return;

	if (m_snippetHandler.IsActive()) {
		const interval& iv = m_lines.GetSelections()[0];
		m_snippetHandler.Delete(iv.start, iv.end);
		return;
	}
	m_pairStack.clear(); // invalidate auto-pair state

	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze(); // always freeze before modifying sel contents
	cxENDLOCK

	unsigned int pos = m_lines.GetPos();

	const vector<interval>& selections = m_lines.GetSelections();

	int dl = 0;
	for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
		if (iv->start < iv->end) {
			cxLOCKDOC_WRITE(m_doc)
				doc.Delete((*iv).start-dl, (*iv).end-dl);
			cxENDLOCK
			m_lines.Delete((*iv).start-dl, (*iv).end-dl);
			StylersDelete((*iv).start-dl, (*iv).end-dl);
		}

		// Update caret position
		if (pos >= (*iv).start-dl && pos <= (*iv).end-dl) pos = (*iv).start-dl;
		else if (pos > (*iv).end-dl) pos -= (*iv).end - (*iv).start;

		dl += (*iv).end - (*iv).start;
	}

	m_lines.RemoveAllSelections();
	m_lines.SetPos(pos);

	// We might have been called while a selection is being made
	m_currentSel = -1;

	MarkAsModified();
}

bool EditorCtrl::DeleteInShadow(unsigned int pos, bool nextchar) {
	wxASSERT(m_lines.IsSelectionShadow() && m_lines.IsSelected());
	wxASSERT(pos <= m_lines.GetLength());

	bool inAutoPair = false;
	if (!m_pairStack.empty()) {
		const interval& iv = m_pairStack.back();

		// Detect backspacing in active auto-pair
		if (!nextchar && pos == iv.start && pos == iv.end) {
			// Also delete pair ender
			inAutoPair = true;
			m_pairStack.pop_back();
		}
		else if ((nextchar && pos >= iv.end) || (!nextchar && pos <= iv.start)) {
			// Reset autoPair state if deleting outside inner pair
			m_pairStack.clear();
		}
	}

	const vector<interval> selections = m_lines.GetSelections();
	unsigned int offset = 0;

	// Calculate delete positions
	for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
		// Check for overlap
		if (pos >= iv->start && pos <= iv->end) {
			// Check if range is outside shadow
			if (pos == iv->start && nextchar == false) return false;
			if (pos == iv->end && nextchar == true) return false;

			cxLOCKDOC_READ(m_doc)
				offset = doc.GetLengthInChars(iv->start, pos);
			cxENDLOCK
			break;
		}
	}

	m_lines.RemoveAllSelections();
	m_lines.ShadowSelections(); // We have to restore shadow mode after removing old selections

	// Do the deletions
	int dl = 0;
	for (vector<interval>::const_iterator i = selections.begin(); i != selections.end(); ++i) {
		unsigned int dp;
		unsigned int byte_len;
		unsigned int del_end;
		cxLOCKDOC_WRITE(m_doc)
			dp = doc.GetCharPos(i->start - dl, offset);
			byte_len = doc.DeleteChar(dp, nextchar);
		cxENDLOCK
		const bool atCaret = (dp == pos);

		if (nextchar) {
			del_end = dp + byte_len;
			m_lines.Delete(dp, del_end);
			StylersDelete(dp, del_end);
			if (pos > dp) pos -= byte_len; // Update caret position
		}
		else { // delete previous char
			del_end = dp;
			unsigned int delPos = dp-byte_len;

			if (inAutoPair) {
				if (pos == dp) pos -= byte_len; // if current only move pos one back

				cxLOCKDOC_WRITE(m_doc)
					byte_len += doc.DeleteChar(delPos, true);
				cxENDLOCK

				m_lines.Delete(delPos, delPos+byte_len);
				StylersDelete(delPos, delPos+byte_len);

				if (pos > dp) pos -= byte_len; // adjust pos to full deletion
			}
			else {
				m_lines.Delete(delPos, dp);
				StylersDelete(delPos, dp);
				if (pos >= dp) pos -= byte_len; // Update caret position
			}
		}

		// pairStack may be moved by insertions above it
		if (!m_pairStack.empty() && m_pairStack[0].start > del_end) {
			for (vector<interval>::iterator p = m_pairStack.begin(); p != m_pairStack.end(); ++p) {
				p->start -= byte_len;
				p->end -= byte_len;
			}
		}

		if (atCaret) {
			// Adjust containing pairs
			for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
				t->end -= byte_len;
			}
		}

		// Restore shadow selection
		m_lines.AddSelection(i->start - dl, (i->end - dl) - byte_len);

		dl += byte_len;
	}

	m_lines.SetPos(pos);
	lastpos = pos;
	lastaction = ACTION_DELETE;

	MarkAsModified();
	return true;
}

void EditorCtrl::InsertOverSelections(const wxString& text) {
	if(!m_lines.IsSelected()) return;
	const vector<interval>& selections = m_lines.GetSelections();
	unsigned int pos = m_lines.GetPos();

	// Reset autoPair state if inserting outside inner pair
	if (!m_pairStack.empty() && pos != m_pairStack.back().end) {
		m_pairStack.clear();
	}

	wxString autoPair;
	if (text.length() == 1) {
		if (m_lines.IsSelectionShadow()) {
			if (!m_pairStack.empty()) {
				if (pos != m_pairStack.back().end) {
					// Reset autoPair state if inserting outside inner pair
					m_pairStack.clear();
				}
				else {
					wxChar c;
					cxLOCKDOC_READ(m_doc)
						c = doc.GetChar(pos);
					cxENDLOCK

					// Jump over auto-paired char
					if (text == c) {
						cxLOCKDOC_READ(m_doc)
							pos = doc.GetNextCharPos(pos);
						cxENDLOCK
						m_lines.SetPos(pos);
						m_pairStack.pop_back();
						return;
					}
				}
			}
		}
		else if (m_doAutoWrap) {
			// if all selections are zero-length, we want to enter multi-edit mode instead
			bool doWrap = false;
			for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
				if (p->start != p->end) {
					doWrap = true;
					break;
				}
			}

			if (doWrap) {
				const deque<const wxString*> scope = m_syntaxstyler.GetScope(pos);
				const map<wxString, wxString> smartPairs = m_syntaxHandler.GetSmartTypingPairs(scope);
				map<wxString, wxString>::const_iterator p = smartPairs.find(text);
				if (p != smartPairs.end()) {
					WrapSelections(p->first, p->second);
					return;
				}
			}
		}

		// else just do standard autopairing
		autoPair = AutoPair(pos, text);
	}

	unsigned int offset = 0;
	unsigned int shadowlength = 0;
	vector<unsigned int> inpos;

	// Calculate insert positions
	if (m_lines.IsSelectionShadow()) {
		const unsigned int shadowpos = m_lines.GetPos();
		for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
			inpos.push_back(iv->start);

			if (shadowpos >= iv->start && shadowpos <= iv->end) {
				offset = shadowpos - iv->start;
			}
		}

		// Calculate length of shadow selections (they should all be the same size)
		shadowlength = selections[0].end - selections[0].start;

		m_lines.RemoveAllSelections();
		m_lines.ShadowSelections(); // We have to restore shadow mode
	}
	else {
		unsigned int dl = 0;
		for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
			inpos.push_back(iv->start-dl);
			dl += iv->end - iv->start;
		}

		DeleteSelections();
		pos = m_lines.GetPos(); // deletions can have changed pos

		if (inpos.size() > 1) m_lines.ShadowSelections();
	}

	// Insert copied text
	if (!text.empty()) {
		unsigned int il = 0;
		for (vector<unsigned int>::const_iterator i = inpos.begin(); i != inpos.end(); ++i) {
			const unsigned int ins_pos = *i+il+offset;
			unsigned int byte_len;
			cxLOCKDOC_WRITE(m_doc)
				byte_len = doc.Insert(ins_pos, text);
			cxENDLOCK
			m_lines.Insert(ins_pos, byte_len);
			StylersInsert(ins_pos, byte_len);
			const unsigned int pair_pos = ins_pos+byte_len;

			unsigned int pair_len = 0;
			if (text == wxT("\n")) {
				// Check if we also need to insert indentation
				const unsigned int lineid = m_lines.GetLineFromCharPos(ins_pos);
				const wxString indent = GetNewIndentAfterNewline(lineid);

				if (!indent.empty()) {
					const unsigned int indent_pos = ins_pos+1;
					unsigned int indent_len;
					cxLOCKDOC_WRITE(m_doc)
						indent_len = doc.Insert(indent_pos, indent);
					cxENDLOCK
					m_lines.Insert(indent_pos, indent_len);
					StylersInsert(indent_pos, indent_len);
					byte_len += indent_len;
				}
			}
			else if (!autoPair.empty()) {
				cxLOCKDOC_WRITE(m_doc)
					pair_len = doc.Insert(pair_pos, autoPair);
				cxENDLOCK
				m_lines.Insert(pair_pos, pair_len);
				StylersInsert(pair_pos, pair_len);
			}

			// Extend shadow to cover new text
			const unsigned int full_len = byte_len + pair_len;
			if (m_lines.IsSelectionShadow()) m_lines.AddSelection(*i+il, *i+il+shadowlength+full_len);

			// pairStack may be moved by insertions above it
			if (!m_pairStack.empty() && m_pairStack[0].start > pair_pos) {
				for (vector<interval>::iterator p = m_pairStack.begin(); p != m_pairStack.end(); ++p) {
					p->start += full_len;
					p->end += full_len;
				}
			}

			// Adjust caret pos
			if (pos > ins_pos) pos += full_len;
			else if (pos == ins_pos) {
				pos += byte_len;

				// If we are just inserting text, adjust containing pairs
				if (autoPair.empty()) {
					for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
						t->end += byte_len;
					}
				}
			}

			il += full_len;
		}
	}

	m_lines.SetPos(pos);
	if (do_freeze && !m_lines.IsSelectionShadow()) {
		cxLOCKDOC_WRITE(m_doc)
			doc.Freeze();
		cxENDLOCK
	}

	MarkAsModified();
}

void EditorCtrl::WrapSelections(const wxString& front, const wxString& back) {
	const vector<interval>& selections = m_lines.GetSelections();
	unsigned int pos = m_lines.GetPos();
	unsigned int offset = 0;

	for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
		//offset += RawInsert(offset + p->start, front);
		//offset += RawInsert(offset + p->end, back);

		unsigned int insPos = offset + p->start;
		unsigned int byte_len = RawInsert(insPos, front);
		if (pos > insPos) pos += byte_len;
		offset += byte_len;

		insPos = offset + p->end;
		byte_len = RawInsert(insPos, back);
		if (pos >= insPos) pos += byte_len;
		offset += byte_len;
	}

	m_lines.SetPos(pos);

	// Selection state
	m_lines.RemoveAllSelections();
	m_currentSel = -1;
	m_sel_start = pos;
}

vector<unsigned int> EditorCtrl::GetSelectedLineNumbers() {
	vector<unsigned int> sel_lines;

	const vector<interval>& selections = m_lines.GetSelections();
	unsigned int prevline = m_lines.GetLineFromCharPos(selections.begin()->start);
	sel_lines.push_back(prevline);

	// Get a list of lines with selections
	for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
		unsigned int startline = m_lines.GetLineFromCharPos(iv->start);
		unsigned int endline = m_lines.GetLineFromCharPos(iv->end);
		if (iv->end == m_lines.GetLineStartpos(endline)) --endline;

		for (; startline <= endline; ++startline) {
			if (startline > prevline) { // avoid duplicates
				sel_lines.push_back(startline);
				prevline = startline;
			}
		}
	}

	return sel_lines;
}

void EditorCtrl::SelectLines(const vector<unsigned int>& sel_lines) {
	unsigned int lastline = 0;
	unsigned int firststart = 0;
	int sel = -1;

	// select the lines (consequtive lines as one selection)
	for (vector<unsigned int>::const_iterator i = sel_lines.begin(); i != sel_lines.end(); ++i) {
		unsigned int linestart = m_lines.GetLineStartpos(*i);
		unsigned int lineend = m_lines.GetLineEndpos(*i, false);

		if (i > sel_lines.begin() && *i == lastline+1 && sel != -1) {
			m_lines.UpdateSelection(sel, firststart, lineend);
		}
		else {
			sel = m_lines.AddSelection(linestart, lineend);
			firststart = linestart;
		}

		lastline = *i;
	}
}

void EditorCtrl::TabsToSpaces() {
	if (GetLength() == 0) return; // Cannot do anything in empty doc

	wxBusyCursor busy;

	const unsigned int tabWidth = m_parentFrame.GetTabWidth();
	const wxString indentString(wxT(' '), tabWidth);
	unsigned int pos = m_lines.GetPos();

	if (m_lines.IsSelected()) {
		// Get list of lines to be converted
		vector<unsigned int> sel_lines = GetSelectedLineNumbers();
		m_lines.RemoveAllSelections();
		bool reSelect = true;

		Freeze();

		cxLOCKDOC_WRITE(m_doc)
			doc_byte_iter dbi(doc, 0);

			for (vector<unsigned int>::const_iterator p = sel_lines.begin(); p != sel_lines.end(); ++p) {
				const unsigned int linestart = m_lines.GetLineStartpos(*p);
				unsigned int lineend = m_lines.GetLineEndpos(*p, true);
				if (linestart == lineend) continue;

				unsigned int indent = 0;

				dbi.SetIndex(linestart);
				while ((unsigned int)dbi.GetIndex() < lineend) {
					if (*dbi == '\t') {
						// it is ok to have a few spaces before tab (making one mixed tab)
						const unsigned int spaces = tabWidth - (indent % tabWidth);
						indent += spaces;

						// Replace the tab char
						const unsigned int tabPos = dbi.GetIndex();
						RawDelete(tabPos, tabPos+1);
						const unsigned int byte_len = RawInsert(tabPos, ((spaces == tabWidth) ? indentString : wxString(wxT(' '), spaces)) );

						dbi.RefreshIndex(tabPos + byte_len);
						if (byte_len > 1) {
							const unsigned int diff = byte_len - 1;
							lineend += diff; // line length may have changed
							if (pos > tabPos) pos += diff;
						}
					}
					else if (*dbi == ' ') {
						++indent;
						++dbi;
					}
					else break;
				}
			}
		cxENDLOCK

		Freeze();

		// Re-select the lines (consequtive lines as one selection)
		if (reSelect && !sel_lines.empty()) {
			SelectLines(sel_lines);
			m_lines.SetPos(m_lines.GetLineEndpos(sel_lines.back(), false));
		}
	}
	else {
		// Convert entire document
		Freeze();

		cxLOCKDOC_WRITE(m_doc)
			doc_byte_iter dbi(doc, 0);
			doc.StartChange();
			int len = doc.GetLength();

			do {
				unsigned int indent = 0;

				while (dbi.GetIndex() < len) {
					if (*dbi == '\t') {
						// it is ok to have a few spaces before tab (making one mixed tab)
						const unsigned int spaces = tabWidth - (indent % tabWidth);
						indent += spaces;

						// Replace the tab char
						const unsigned int tabPos = dbi.GetIndex();
						const unsigned int byte_len = doc.Replace(tabPos, tabPos+1, ((spaces == tabWidth) ? indentString : wxString(wxT(' '), spaces)));

						dbi.RefreshIndex(tabPos + byte_len);
						if (pos > tabPos) pos += (byte_len-1);
						len = doc.GetLength(); // update after change
					}
					else if (*dbi == ' ') {
						++indent;
						++dbi;
					}
					else break;
				}

				// Advance to next line
				while (dbi.GetIndex() < len) {
					const char c = *dbi;
					++dbi;
					if (c == '\n') break;
				}
			} while (dbi.GetIndex() < len);

			doc.EndChange();
		cxENDLOCK

		StylersInvalidate(); // invalidate before reload to avoid stale refs
		m_lines.ReLoadText();
		UpdateEditorWidth(); // Re-set the width (updates lines and stylers)

		m_lines.SetPos(pos);
	}

	MarkAsModified();
}

void EditorCtrl::SpacesToTabs() {
	if (GetLength() == 0) return; // Cannot do anything in empty doc

	wxBusyCursor busy;

	const unsigned int tabWidth = m_parentFrame.GetTabWidth();

	// Get list of lines to be converted
	if (m_lines.IsSelected()) {
		vector<unsigned int> sel_lines = GetSelectedLineNumbers();
		m_lines.RemoveAllSelections();
		bool reSelect = true;

		Freeze();

		cxLOCKDOC_WRITE(m_doc)
			doc_byte_iter dbi(doc, 0);

			for (vector<unsigned int>::const_iterator p = sel_lines.begin(); p != sel_lines.end(); ++p) {
				const unsigned int linestart = m_lines.GetLineStartpos(*p);
				unsigned int lineend = m_lines.GetLineEndpos(*p, true);
				if (linestart == lineend) continue;

				unsigned int spacesStart = linestart;
				dbi.SetIndex(linestart);

				while ((unsigned int)dbi.GetIndex() < lineend) {
					if (*dbi == ' ') {
						++dbi;
					}
					else if (*dbi == '\t') {
							const unsigned int pos = dbi.GetIndex();
							const unsigned int spaces = pos - spacesStart;

							// number of spaces before tab
							if (spaces == 0) {
								++dbi;
								spacesStart = dbi.GetIndex();
								continue;
							}

							// Replace the tab char
							RawDelete(spacesStart, pos);
							const unsigned int byte_len = RawInsert(spacesStart, wxString(wxT('\t'), spaces / tabWidth) );

							// line length may have changed
							if (byte_len < spaces) {
								lineend -= (spaces - byte_len);
							}

							const unsigned int nextPos = spacesStart + byte_len;
							dbi.RefreshIndex(nextPos);
							spacesStart = nextPos;
					}
					else break;
				}

				if ((int)spacesStart < dbi.GetIndex()) {
					const unsigned int spaces = dbi.GetIndex() - spacesStart;
					const unsigned int spacesToTabs = spaces - (spaces % tabWidth);

					if (spacesToTabs > 0) {
						RawDelete(spacesStart, spacesStart + spacesToTabs);
						RawInsert(spacesStart, wxString(wxT('\t'), spacesToTabs / tabWidth) );
						dbi.RefreshIndex(spacesStart);
					}
				}
			}
		cxENDLOCK

		Freeze();

		// Re-select the lines (consequtive lines as one selection)
		if (reSelect && !sel_lines.empty()) {
			SelectLines(sel_lines);
			m_lines.SetPos(m_lines.GetLineEndpos(sel_lines.back(), false));
		}
	}
	else {
		// Convert entire document
		unsigned int caretpos = m_lines.GetPos();
		Freeze();

		cxLOCKDOC_WRITE(m_doc)
			doc_byte_iter dbi(doc, 0);
			doc.StartChange();
			int len = doc.GetLength();
			unsigned int spacesStart = 0;

			do {
				while (dbi.GetIndex() < len) {
					if (*dbi == ' ') ++dbi;
					else if (*dbi == '\t') {
							const unsigned int pos = dbi.GetIndex();
							const unsigned int spaces = pos - spacesStart;

							// number of spaces before tab
							if (spaces == 0) {
								++dbi;
								spacesStart = dbi.GetIndex();
								continue;
							}

							// Replace the tab char
							const unsigned int byte_len = doc.Replace(spacesStart, pos, wxString(wxT('\t'), spaces / tabWidth) );

							// length may have changed
							len = doc.GetLength();

							const unsigned int nextPos = spacesStart + byte_len;
							if (caretpos > spacesStart) caretpos = wxMax(spacesStart, caretpos + (spaces - byte_len));
							dbi.RefreshIndex(nextPos);
							spacesStart = nextPos;
					}
					else break;
				}

				if ((int)spacesStart < dbi.GetIndex()) {
					const unsigned int spaces = dbi.GetIndex() - spacesStart;
					const unsigned int spacesToTabs = spaces - (spaces % tabWidth);

					if (spacesToTabs > 0) {
						doc.Replace(spacesStart, spacesStart + spacesToTabs, wxString(wxT('\t'), spacesToTabs / tabWidth) );
						dbi.RefreshIndex(spacesStart);
					}
				}

				// Advance to next line
				while (dbi.GetIndex() < len) {
					const char c = *dbi;
					++dbi;
					if (c == '\n') {
						spacesStart = dbi.GetIndex();
						break;
					}
				}
			} while (dbi.GetIndex() < len);

			doc.EndChange();
		cxENDLOCK

		// Invalidate all stylers
		StylersInvalidate();
		m_lines.ReLoadText();
		UpdateEditorWidth(); // Re-set the width (updates lines and stylers)

		m_lines.SetPos(caretpos);
	}

	MarkAsModified();
}

void EditorCtrl::IndentSelectedLines(bool add_indent) {
	if (!add_indent && GetLength() == 0) return; // Cannot dedent in empty doc

	// We start by adding the first line (to avoid duplicates)
	vector<unsigned int> sel_lines;
	bool reSelect = false;

	if (m_lines.IsSelected()) {
		sel_lines = GetSelectedLineNumbers();
		m_lines.RemoveAllSelections();
		reSelect = true;
	}
	else sel_lines.push_back(m_lines.GetCurrentLine());

	unsigned int pos = m_lines.GetPos();
	unsigned int tabWidth = m_parentFrame.GetTabWidth();

	// Insert the indentations
	for (vector<unsigned int>::const_iterator i = sel_lines.begin(); i != sel_lines.end(); ++i) {
		unsigned int ins_pos = m_lines.GetLineStartpos(*i);

		if (add_indent) { // Indent
			unsigned int indent_len;
			cxLOCKDOC_WRITE(m_doc)
				indent_len = doc.Insert(ins_pos, m_indent);
			cxENDLOCK
			m_lines.Insert(ins_pos, indent_len);
			StylersInsert(ins_pos, indent_len);
			if (pos >= ins_pos) pos += indent_len;
		}
		else {
			wxChar c;
			cxLOCKDOC_READ(m_doc)
				c = doc.GetChar(ins_pos);
			cxENDLOCK

			unsigned int indent_len = 0;
			if (c == wxT('\t')) indent_len = 1;
			else if (c == wxT(' ')) {
				const unsigned int len = m_lines.GetLength();

				if (ins_pos + tabWidth <= len && IsSpaces(ins_pos, ins_pos + tabWidth)) {
					indent_len = tabWidth;
				}
				else {
					// remove leading whitespace
					indent_len = 1;
					unsigned int char_pos = ins_pos+1;
					cxLOCKDOC_READ(m_doc)
						while (char_pos < len && doc.GetChar(char_pos++) == wxT(' ')) {
							++indent_len;
						}
					cxENDLOCK
				}
			}

			if (indent_len) {
				// Un-indent
				cxLOCKDOC_WRITE(m_doc)
					doc.Delete(ins_pos, ins_pos+indent_len);
				cxENDLOCK
				m_lines.Delete(ins_pos, ins_pos+indent_len);
				StylersDelete(ins_pos, ins_pos+indent_len);
				if (pos > ins_pos) pos -= indent_len;
			}
		}
	}

	// Re-select the lines (consequtive lines as one selection)
	if (reSelect) {
		SelectLines(sel_lines);
	}

	m_lines.SetPos(pos);

	MarkAsModified();
}

void EditorCtrl::SwapLines(unsigned int line1, unsigned int line2) {
	wxASSERT(line1 < line2 && line2 < m_lines.GetLineCount());

	const unsigned int linestart = m_lines.GetLineStartpos(line1);
	const unsigned int lineend = m_lines.GetLineEndpos(line1, true);
	const unsigned int linestart2 = m_lines.GetLineStartpos(line2);
	const unsigned int lineend2 = m_lines.GetLineEndpos(line2, true);

	// Move top line to end of bottom line
	if (linestart != lineend) {
		RawMove(linestart, lineend, lineend2);
	}

	if (!m_lines.isLineVirtual(line2) && linestart2 != lineend2) {
		const unsigned int offset = lineend - linestart;

		// Move bottom line to top lines position
		RawMove(linestart2 - offset, lineend2 - offset, linestart);
	}
}

void EditorCtrl::Transpose() {
	Freeze();

	vector<interval> selections = m_lines.GetSelections();
	const unsigned int pos = m_lines.GetPos();

	if (selections.empty()) {
		const unsigned int lineid = m_lines.GetCurrentLine();
		const unsigned int linestart = m_lines.GetLineStartpos(lineid);
		const unsigned int lineend = m_lines.GetLineEndpos(lineid, true);

		if (pos == lineend) {
			// if we are at end of line, we swap it with the one below it
			if (lineid < m_lines.GetLastLine()) {
				SwapLines(lineid, lineid+1);
			}
		}
		else if (pos == linestart) {
			// if we are at start of line, we swap it with the one above it
			if (lineid > 0) {
				SwapLines(lineid-1, lineid);
			}
		}
		else {
			bool swapWords = false;
			unsigned int nextPos;
			unsigned int prevPos;
			interval prevWord(0,0);
			interval nextWord(0,0);

			cxLOCKDOC_READ(m_doc)
				wxChar nextChar;
				wxChar prevChar;
				nextPos = doc.GetNextCharPos(pos);
				prevPos = doc.GetPrevCharPos(pos);
				nextChar = doc.GetChar(pos);
				prevChar = doc.GetChar(prevPos);

				// Check if we are between two words
				if (wxIsspace(nextChar) || wxIsspace(prevChar)) {
					// Get prevword
					bool start = true;
					for (unsigned int prevWordPos = prevPos; prevWordPos >= linestart; prevWordPos = doc.GetPrevCharPos(prevWordPos)) {
						prevChar = doc.GetChar(prevWordPos);
						if (start) {
							if (!wxIsspace(prevChar)) {
								prevWord.start = linestart;
								prevWord.end = doc.GetNextCharPos(prevWordPos);
								start = false;
							}
						}
						else {
							if (wxIsspace(prevChar)) {
								prevWord.start = doc.GetNextCharPos(prevWordPos);
								break;
							}
						}
						if (prevWordPos == linestart) break;
					}

					// Get nextword
					start = true;
					for (unsigned int nextWordPos = pos; nextWordPos < lineend; nextWordPos = doc.GetNextCharPos(nextWordPos)) {
						nextChar = doc.GetChar(nextWordPos);
						if (start) {
							if (!wxIsspace(nextChar)) {
								nextWord.start = nextWordPos;
								nextWord.end = lineend;
								start = false;
							}
						}
						else {
							if (wxIsspace(nextChar)) {
								nextWord.end = nextWordPos;
								break;
							}
						}
						if (nextWordPos == lineend) break;

					}

					if (prevWord.start != prevWord.end && nextWord.start != nextWord.end) {
						swapWords = true; // both words found
					}

				}
			cxENDLOCK

			if (swapWords) {
				// Keep relative caret pos
				const unsigned int nextWordLen = nextWord.end - nextWord.start;
				const unsigned int newPos = prevWord.start + nextWordLen + (pos - prevWord.end);

				// Swap words
				RawMove(nextWord.start, nextWord.end, prevWord.end);
				RawMove(prevWord.start, prevWord.end, nextWord.start + nextWordLen);

				SetPos(newPos);
			}
			else {
				// swap with previous char
				RawMove(prevPos, pos, nextPos);
			}
		}

	}
	else if (selections.size() == 1) {
		const unsigned int start = selections[0].start;
		const unsigned int end = selections[0].end;

		if (start < end) {
			const unsigned int lineid1 = m_lines.GetLineFromCharPos(start);
			const unsigned int lineid2 = m_lines.GetLineFromCharPos(end);

			// if multiline, it should swap the lines
			if (lineid1 < lineid2) {
				unsigned int l1 = lineid1;
				unsigned int l2 = lineid2;

				m_lines.RemoveAllSelections();

				do {
					SwapLines(l1, l2);
					++l1;
					--l2;
				}
				while ((int)l2 - (int)l1 > 1);

				m_lines.AddSelection(m_lines.GetLineStartpos(lineid1), m_lines.GetLineEndpos(lineid2, true));
			}
			else {
				// Reverse text
				wxString text;
				cxLOCKDOC_READ(m_doc)
					doc.GetTextPart(start, end, text);
				cxENDLOCK

				// Append text in reverse order
				wxString rev;
				size_t i = text.size();
				while (i) {
					--i;
					rev.append(text[i]);
				}

				RawDelete(start, end);
				RawInsert(start, rev);
				m_lines.SetPos(pos); // keep same position
			}
		}

	}
	else {
		m_lines.RemoveAllSelections();
		vector<interval> newsel;

		int offset = 0;
		while (selections.size() > 1) {
			// Swap first and last selection (part one)
			const unsigned int insertPos1 = offset+selections.back().end;
			RawMove(offset+selections[0].start, offset+selections[0].end, insertPos1);
			const int firstLen = selections[0].end - selections[0].start;
			int newoffset = offset - firstLen;

			// Swap first and last selection (part two)
			const unsigned int insertPos2 = offset+selections[0].start;
			const unsigned int secondStart = newoffset+selections.back().start;
			RawMove(secondStart, newoffset+selections.back().end, insertPos2);
			const int secondLen = selections.back().end - selections.back().start;
			offset = newoffset += secondLen;

			// Update new selections
			const interval iv1(insertPos2, insertPos2 + secondLen);
			const interval iv2(insertPos1 - firstLen, (insertPos1 - firstLen) + firstLen);
			vector<interval>::iterator p = newsel.insert(newsel.begin()+(newsel.size()/2), iv1); ++p;
			p = newsel.insert(p, iv2);

			// Remove the swapped selections
			selections.erase(selections.begin());
			selections.erase(selections.end()-1);
		}

		// Update middle selection
		if (selections.size() == 1) {
			vector<interval>::iterator p = newsel.insert(newsel.begin()+(newsel.size()/2), selections[0]);
			p->start += offset;
			p->end += offset;
		}

		// Re-select
		for (vector<interval>::iterator p = newsel.begin(); p != newsel.end(); ++p) {
			m_lines.AddSelection(p->start, p->end);
		}
	}

	Freeze();
}

void EditorCtrl::DelCurrentLine(bool fromPos) {
	Freeze();

	const unsigned int pos = m_lines.GetPos();
	const unsigned int lineid = m_lines.GetCurrentLine();
	const unsigned int linestart = m_lines.GetLineStartpos(lineid);

	if (fromPos) {
		const unsigned int lineend = m_lines.GetLineEndpos(lineid, true);
		if (pos == lineend) {
			// Delete last newline (if any)
			const unsigned int realend = m_lines.GetLineEndpos(lineid, false);
			if (pos < realend) Delete(pos, realend);
		}
		else {
			// Delete from pos to end-of-line
			Delete(pos, lineend);
		}
	}
	else {
		// Delete entire line
		const unsigned int lineend = m_lines.GetLineEndpos(lineid, false);
		Delete(linestart, lineend);
	}

	Freeze();
}

void EditorCtrl::ConvertCase(cxCase conversion) {
	Freeze();

	vector<interval> selections = m_lines.GetSelections();
	const unsigned int pos = m_lines.GetPos();

	// If no selection, we convert the current word
	if (selections.empty()) {
		const interval word = GetWordIv(pos);
		if (word.start < word.end) selections.push_back(word);
	}

	for (unsigned int i = 0; i < selections.size(); ++i) {
		const unsigned int start = selections[i].start;
		const unsigned int end = selections[i].end;

		wxString text;
		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(start, end, text);
		cxENDLOCK

		switch (conversion) {
		case cxUPPERCASE:
#ifdef __WXMSW__
			::CharUpperBuff(text.begin(), (DWORD)text.length());
#else
            text.MakeUpper();
#endif
			break;
		case cxLOWERCASE:
#ifdef __WXMSW__
			::CharLowerBuff(text.begin(), (DWORD)text.length());
#else
            text.MakeLower();
#endif
			break;
		case cxTITLECASE:
			{
				bool wordStart = true;
				for (unsigned int n = 0; n < text.size(); ++n) {
					const wxChar c = text[n];

					if (wxIsspace(c)) wordStart = true;
					else {
#ifdef __WXMSW__
						if (wordStart) text[n] = (wxChar)::CharUpper((LPTSTR)c);
						else text[n] = (wxChar)::CharLower((LPTSTR)c);
#else
						if (wordStart) text[n] = wxToupper(c);
						else text[n] = wxTolower(c);
#endif
						wordStart = false;
					}
				}
			}
			break;
		case cxREVERSECASE:
			{
				for (unsigned int n = 0; n < text.size(); ++n) {
					const wxChar c = text[n];
#ifdef __WXMSW__
					if (::IsCharUpper(c)) text[n] = (wxChar)::CharLower((LPTSTR)c);
					else if (::IsCharLower(c)) text[n] = (wxChar)::CharUpper((LPTSTR)c);
#else
					if (wxIsupper(c)) text[n] = wxTolower(c);
					else if (wxIslower(c)) text[n] = wxToupper(c);
#endif
				}
			}
			break;
		default:
			wxASSERT(false);
		}

		RawDelete(start, end);
		RawInsert(start, text);
	}

	// keep same position
	m_lines.SetPos(pos);

	Freeze();
}

void EditorCtrl::SetSyntax(const wxString& syntaxName, bool isManual) {
	// If this is a manual override we want to associate this ext to syntax
	wxString ext;
	if (isManual) ext = m_path.GetFullPath().AfterFirst(wxT('.'));

	// First time we set a new syntax there can be a delay while parsing
	wxBeginBusyCursor();
	m_syntaxstyler.SetSyntax(syntaxName, ext);
	wxEndBusyCursor();

	// We also have to reparse the foldings
	FoldingInvalidate();

	MarkAsModified(); // flush symbol cache

	DrawLayout();
	MarkAsModified();
};

void EditorCtrl::OnCopy() {
	wxString copytext;
	if (m_lines.IsSelected()) copytext = GetSelText();
	else if (!m_lines.IsEmpty()) {
		// Copy line
		const unsigned int line_id = m_lines.GetCurrentLine();
		if (!m_lines.isLineVirtual(line_id)) {
			const unsigned int startpos = m_lines.GetLineStartpos(line_id);
			const unsigned int endpos = m_lines.GetLineEndpos(line_id, false); // include newline
			copytext = GetText(startpos, endpos);
		}
	}

	if (!copytext.empty()) {
#ifdef __WXMSW__
		// WINDOWS ONLY!! newline conversion
		// BUG in wxWidgets: If copytext contains nullbytes string gets truncated
		// The build-in replace is too slow so we do
		// it manually (at the price of some memory)
		//copytext.Replace(wxT("\n"), wxT("\r\n"));
		wxString newtext;
		newtext.reserve(copytext.size() + (copytext.size() / 20));
		unsigned int lastPos = 0;
		for (unsigned int i = 0; i < copytext.size(); ++i) {
			if (copytext[i] == wxT('\n')) {
				newtext += copytext.substr(lastPos, i - lastPos);
				newtext += wxT("\r\n");
				lastPos = i + 1;
			}
		}
		if (lastPos < copytext.size()) newtext += copytext.substr(lastPos, copytext.size() - lastPos);
		copytext.swap(newtext);
#endif // __WXMSW__

		// Create the data object
		wxDataObjectSimple* dataObject = new wxTextDataObject(copytext);

		// If we have a multiselection, we want ot create a composite object,
		// so that we can get the individual parts on paste
		if (m_lines.IsMultiSelected()) {
			MultilineDataObject* mdo = new MultilineDataObject;
			const vector<interval>& selections = m_lines.GetSelections();
			cxLOCKDOC_READ(m_doc)
				// Get the selected text
				for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
					mdo->AddText(doc.GetTextPart((*iv).start, (*iv).end));
				}
			cxENDLOCK

			wxDataObjectComposite* compObject = new wxDataObjectComposite();
			compObject->Add(mdo, true);
			compObject->Add(dataObject);

			dataObject = (wxDataObjectSimple*)compObject;
		}

		// Copy the text to the clipboard
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(dataObject);
			wxTheClipboard->Close();
		}
		else {
			delete dataObject;
			wxFAIL_MSG(wxT("Could not open clipboard"));
		}
	}
}

void EditorCtrl::OnCut() {
	bool doCopy = true;
	if (!m_lines.IsSelected()) {
		// Select current line 
		const unsigned int cl = m_lines.GetCurrentLine();
		const unsigned int start = m_lines.GetLineStartpos(cl);
		const unsigned int end = m_lines.GetLineEndpos(cl, false);
		m_lines.AddSelection(start, end);

		if (m_lines.IsLineEmpty(cl)) doCopy = false; // don't copy empty line
	}

	if (doCopy) OnCopy();
	DeleteSelections();
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
	cxENDLOCK
}


void EditorCtrl::InsertColumn(const wxArrayString& text, bool select) {
	if (text.IsEmpty()) return;

	Freeze();

	if (m_lines.IsSelected()) {
		vector<unsigned int> inpos;
		unsigned int offset = 0;
		unsigned int pos = m_lines.GetPos();
		const vector<interval>& selections = m_lines.GetSelections();
		const bool isShadow = m_lines.IsSelectionShadow(); 

		// Calculate insert positions
		unsigned int dl = 0;
		for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
			inpos.push_back(iv->start-dl);
			
			if (!isShadow) dl += iv->end - iv->start;
			else if (pos >= iv->start && pos <= iv->end) {
				offset = pos - iv->start;
			}
		}

		// Remove selections
		if (isShadow) m_lines.RemoveAllSelections();
		else {
			DeleteSelections();
			pos = m_lines.GetPos(); // deletions can have changed pos
		}

		// Insert copied text
		unsigned int il = 0;
		for (unsigned int i = 0; i < inpos.size(); ++i) {
			if (i >= text.GetCount()) break;

			const unsigned int ins_pos = inpos[i]+il+offset;
			unsigned int byte_len;
			cxLOCKDOC_WRITE(m_doc)
				byte_len = doc.Insert(ins_pos, text[i]);
			cxENDLOCK
			m_lines.Insert(ins_pos, byte_len);
			StylersInsert(ins_pos, byte_len);

			const unsigned int end_pos = pos + byte_len;
			if (select) m_lines.AddSelection(ins_pos, end_pos);

			// Adjust caret pos
			if (pos >= ins_pos) pos = end_pos;

			il += byte_len;
		}

		m_lines.SetPos(pos);
	}
	else {
		// Column paste
		const wxPoint cpos = m_lines.GetCaretPos();
		unsigned int pos = m_lines.GetPos();
		for (size_t i = 0; i < text.GetCount(); ++i) {
			if (i) {
				const full_pos fp = m_lines.MovePosDown(cpos.x);
				pos = fp.pos;
				
				// Insert newlines if pasting beyond end of doc
				if (fp.xy_outbound && m_lines.GetCurrentLine() == (int)m_lines.GetLastLine()) {
					pos = m_lines.GetLength();
					pos += RawInsert(pos, wxT("\n"));
					m_lines.SetPos(pos);
				}
			}
			const unsigned int inspos = pos;
			pos += RawInsert(inspos, text[i]);

			if (select) m_lines.AddSelection(inspos, pos);
		}
		m_lines.SetPos(pos);
	}

	Freeze();
	MarkAsModified();
	DrawLayout();
}

void EditorCtrl::OnPaste() {
	if (!wxTheClipboard->Open()) return;
	
	if (wxTheClipboard->IsSupported( MultilineDataObject::s_formatId )) {
		MultilineDataObject data;
		if (wxTheClipboard->GetData(data)) {
			// Get the text from clipboard
			wxArrayString textParts;
			data.GetText(textParts);
			wxTheClipboard->Close();
			
			InsertColumn(textParts);
		}
	}
	else if (wxTheClipboard->IsSupported( wxDF_TEXT )) {
		// Get text from clipboard
		wxTextDataObject data;
		if (wxTheClipboard->GetData(data)) {
			// Get the text from clipboard
			wxString copytext = data.GetText();
			wxTheClipboard->Close();
			if (copytext.empty()) return;

#ifdef __WXMSW__
			ConvertCRLFtoLF(copytext);
#endif // __WXMSW__

			Freeze();

			Insert(copytext);
			m_lines.PrepareAll(); // make sure all lines are valid
			
			Freeze(); // No extension possible after paste
		}
	}
}

#ifdef __WXMSW__
void EditorCtrl::ConvertCRLFtoLF(wxString& text) { // static
	// WINDOWS ONLY!! newline conversion
	// The build-in replace is too slow so we do
	// it manually (at the price of some memory)
	//copytext.Replace(wxT("\r\n"), wxT("\n"));
	wxString newtext;
	newtext.reserve(text.size());
	unsigned int lastPos = 0;
	for (unsigned int i = 0; i < text.size(); ++i) {
		if (text[i] == wxT('\r')) {
			newtext += text.substr(lastPos, i - lastPos);
			lastPos = i + 1;
		}
	}
	if (lastPos < text.size()) newtext += text.substr(lastPos, text.size() - lastPos);
	text.swap(newtext);
}
#endif // __WXMSW__

bool EditorCtrl::IsSelected() const {
	return m_lines.IsSelected();
}

void EditorCtrl::RemoveAllSelections() {
	m_lines.RemoveAllSelections();
	m_currentSel = -1;
}

void EditorCtrl::Select(unsigned int start, unsigned int end) {
	wxASSERT(start <= end && end <= m_lines.GetLength());

	m_lines.RemoveAllSelections();
	m_lines.AddSelection(start, end);
}

void EditorCtrl::SelectAll() {
	m_lastScopePos = -1; // invalidate scope selections
	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();

	if (m_lines.GetLength()) {
		m_lines.RemoveAllSelections();
		m_lines.AddSelection(0, m_lines.GetLength());
	}
}

interval EditorCtrl::GetWordIv(unsigned int pos) const {
	unsigned int wordstart = pos;

	cxLOCKDOC_READ(m_doc)
		while (wordstart) {
			const unsigned int prevpos = doc.GetPrevCharPos(wordstart);
			const wxChar c = doc.GetChar(prevpos);
			if (!Isalnum(c) && c != '_') break;
			wordstart = prevpos;
		}

		unsigned int wordend = pos;
		while (wordend < doc.GetLength()) {
			const unsigned int nextpos = doc.GetNextCharPos(wordend);
			const wxChar c = doc.GetChar(wordend);
			if (!Isalnum(c) && c != '_') break;
			wordend = nextpos;
		}

		return interval(wordstart, wordend);
	cxENDLOCK
}

wxString EditorCtrl::GetCurrentWord() const {
	const interval iv = GetWordIv(GetPos());

	if (iv.start < iv.end) {
		cxLOCKDOC_READ(m_doc)
			return doc.GetTextPart(iv.start, iv.end);
		cxENDLOCK
	}
	else return wxEmptyString;
}

wxString EditorCtrl::GetCurrentLine() {
	const unsigned int cl = m_lines.GetCurrentLine();
	const unsigned int start = m_lines.GetLineStartpos(cl);
	const unsigned int end = m_lines.GetLineEndpos(cl);
	if (start < end) {
		cxLOCKDOC_READ(m_doc)
			return doc.GetTextPart(start, end);
		cxENDLOCK
	}

	// We end here if the line is empty
	return wxEmptyString;
}

void EditorCtrl::SelectWord(unsigned int pos, bool multiselect) {
	wxASSERT(pos <= m_doc.GetLength());
	m_lastScopePos = -1; // invalidate scope selections

	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
	if (!multiselect || m_lines.IsSelectionShadow()) m_lines.RemoveAllSelections();

	// Check if we should select brackets or word
	interval iv;
	if (m_hlBracket.start < m_hlBracket.end && (pos == m_hlBracket.start || pos == m_hlBracket.end)) {
		iv = m_hlBracket;
		iv.end += 1; // include end bracket
	}
	else iv = GetWordIv(pos);

	if (iv.start < iv.end) {
		m_selMode = SEL_WORD;
		m_sel_start = iv.start;
		m_sel_end = iv.end;
		m_currentSel = m_lines.AddSelection(iv.start, iv.end);
		m_lines.SetPos(iv.end);
		MakeCaretVisible();
	}
}

void EditorCtrl::SelectLine(unsigned int line_id, bool multiselect) {
	wxASSERT(line_id < m_lines.GetLineCount());
	m_lastScopePos = -1; // invalidate scope selections

	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
	if (!multiselect || m_lines.IsSelectionShadow()) m_lines.RemoveAllSelections();

	// Select the line
	if (!m_lines.isLineVirtual(line_id)) {
		const unsigned int startpos = m_lines.GetLineStartpos(line_id);
		const unsigned int endpos = m_lines.GetLineEndpos(line_id, false); // include newline

		m_selMode = SEL_LINE;
		m_sel_start = startpos;
		m_sel_end = endpos;
		m_currentSel = m_lines.AddSelection(startpos, endpos);
		m_lines.SetPos(endpos);
	}
}

void EditorCtrl::SelectCurrentLine() {
	SelectLine(m_lines.GetCurrentLine());
}

void EditorCtrl::SelectScope() {
	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
	unsigned int pos = GetPos();

	// Start over if all is selected
	const vector<interval>& selections = m_lines.GetSelections();
	if (selections.size() == 1 && selections[0].start == 0 && selections[0].end == m_lines.GetLength()) {
		m_lines.RemoveAllSelections();
		if (m_lastScopePos != -1 && m_lastScopePos <= (int)m_lines.GetLength()) {
			pos = m_lastScopePos;
		}
	}

	const deque<interval> scopes = m_syntaxstyler.GetScopeIntervals(pos);
	if (scopes.empty()) return;

	if (selections.empty()) {
		const interval& iv = scopes.back();
		m_currentSel = m_lines.AddSelection(iv.start, iv.end);
		m_lines.SetPos(iv.end);
	}
	else {
		// If there are selections, we want to select the scope that
		// encompases entire selection(s)
		const unsigned int sel_start = selections[0].start;
		const unsigned int sel_end = selections.back().end;
		m_lines.RemoveAllSelections();

		// Advance up though scopes until we find one that encompases
		for (deque<interval>::const_reverse_iterator p = scopes.rbegin(); p != scopes.rend(); ++p) {
			if ((p->start <= sel_start && p->end >= sel_end) && (p->start < sel_start || p->end > sel_end)) {
				m_currentSel = m_lines.AddSelection(p->start, p->end);
				m_lines.SetPos(p->end);
				break;
			}
		}
	}

	// Keep state (only used if we have hit top)
	if (m_lastScopePos == -1) {
		m_lastScopePos = pos;
	}
}

void EditorCtrl::GetCompletionMatches(interval wordIv, wxArrayString& result, bool precharbase) const {
	wxASSERT(wordIv.start <= wordIv.end && wordIv.end <= GetLength());
	wxASSERT(!precharbase || wordIv.end - wordIv.start == 1);

	map<wxString,unsigned int> completions;

	// Get target word
	cxLOCKDOC_READ(m_doc)
		const unsigned int doclen = doc.GetLength();
		const wxString target = doc.GetTextPart(wordIv.start, wordIv.end);

		unsigned int pos = 0;
		while (pos < doclen) {
			const search_result sr = doc.Find(target, pos, true);
			if (sr.error_code != 0) break;

			// Ignore current word
			if (sr.start == wordIv.start && sr.end == wordIv.end) {
				pos = sr.end;
				continue;
			}

			// Check if it is the start of a word
			if (sr.start) {
				const unsigned int prevpos = doc.GetPrevCharPos(sr.start);
				const wxChar c = doc.GetChar(prevpos);
				if (Isalnum(c) || c == '_') {
					pos = doc.GetNextCharPos(sr.start);
					continue;
				}
			}

			// Get the rest of the word
			unsigned int wordend = sr.end;
			while (wordend < doclen) {
				const unsigned int nextpos = doc.GetNextCharPos(wordend);
				const wxChar c = doc.GetChar(wordend);
				if (!Isalnum(c) && c != '_') break;
				wordend = nextpos;
			}

			if (wordend > sr.end) {
				// Add to completion list
				const unsigned int wordstart = precharbase ? sr.start+1 : sr.start; // remove base char
				const wxString word = doc.GetTextPart(wordstart, wordend);
				const unsigned int dist = sr.start < wordIv.start ? wordIv.start - sr.start : sr.start - wordIv.start;

				map<wxString,unsigned int>::const_iterator p = completions.find(word);
				if (p == completions.end() || p->second < dist) {
					completions[word] = dist;
				}
			}

			pos = wordend;
		}
	cxENDLOCK

	// remove entries that already are in list
	for (unsigned int i = 0; i < result.GetCount(); ++i) {
		map<wxString,unsigned int>::iterator p = completions.find(result[i]);
		if (p != completions.end()) completions.erase(p);
	}

	// reverse map to be indexed by distance
	map<unsigned int, wxString> sorted;
	for (map<wxString,unsigned int>::const_iterator p = completions.begin(); p != completions.end(); ++p) {
		sorted[p->second] = p->first;
	}

	// return list of completions, sorted by distance
	result.Alloc(result.GetCount() + sorted.size());
	for (map<unsigned int,wxString>::const_iterator p2 = sorted.begin(); p2 != sorted.end(); ++p2) {
		result.Add(p2->second);
	}
}

wxArrayString EditorCtrl::GetCompletionList() {
	// Get current word
	interval iv = GetWordIv(GetPos());

	// If there is no word, and the preceding char is not a whitespace then
	// we use that as base for finding completions.
	bool precharbase = false;
	if (iv.start == iv.end && iv.start != 0) {
		cxLOCKDOC_READ(m_doc)
			const unsigned int prevcharpos = doc.GetPrevCharPos(iv.start);
			const wxChar c = doc.GetChar(prevcharpos);
			if (!wxIsspace(c)) {
				iv.start = prevcharpos;
				precharbase = true;
			}
		cxENDLOCK
	}

	// If we are completing based on the preceding char, then we want it's scope
	// rather than the current pos
	const unsigned int scopePos = precharbase ? iv.start : GetPos();
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(scopePos);

	wxArrayString completions;

	// Check if we have a completion list
	const vector<wxString>* completionList = m_syntaxHandler.GetCompletionList(scope);
	if (completionList) {
		const wxString target = GetText(iv.start, iv.end);
		for (vector<wxString>::const_iterator p = completionList->begin(); p != completionList->end(); ++p) {
			if (precharbase || p->StartsWith(target)) completions.Add(*p);
		}
	}
	else if (iv.start < iv.end) { // Don't try to run completionCmd if we have no current word
		// Check for completion command
		const tmCompletionCmd* completionCmd = m_syntaxHandler.GetCompletionCmd(scope);
		if (completionCmd) {
			vector<char> input;
			vector<char> output;
			cxEnv env;
			SetEnv(env, true, completionCmd->bundle);

			// We cannot run command while escape is down
			while (wxGetKeyState(WXK_ESCAPE)) {
				wxLogDebug(wxT("waiting for escape up"));
				wxMilliSleep(50);
			}

			const long result = RawShell(completionCmd->cmd, input, &output, NULL, env);
			if (result == -1) return wxArrayString();

			// break output into lines
			vector<char>::const_iterator linestart = output.begin();
			for (vector<char>::const_iterator p = output.begin(); p != output.end(); ++p) {
				if (*p == '\n') {
					const unsigned int len = p - linestart;
					if (len) completions.Add(wxString(&*linestart, wxConvUTF8, len));
					linestart = p+1;
				}
			}
			if (linestart < output.end()) completions.Add(wxString(&*linestart, wxConvUTF8, output.end()-linestart));
		}
	}

	// Get default completions (matches in doc)
	if (!m_syntaxHandler.DisableDefaultCompletion(scope)) {
		GetCompletionMatches(iv, completions, precharbase);
	}

	return completions;
}

void EditorCtrl::DoCompletion() {
	const wxArrayString completions = GetCompletionList();
	if (completions.empty()) return;

	// Get current word
	const interval iv = GetWordIv(GetPos());

	// Calc popup position
	const wxPoint firstCharPos = m_lines.GetCharPos(iv.start);
	const wxPoint lastCharPos = m_lines.GetCharPos(iv.end);
	const wxPoint popupTopPos = ClientToScreen(EditorPosToClient(firstCharPos.x, lastCharPos.y));
	const wxPoint popupPos(popupTopPos.x, popupTopPos.y + m_caretHeight);

	// The popup will delete itself when done
	const wxString target = GetText(iv.start, iv.end);
	new CompletionPopup(*this, popupPos, popupTopPos, target, completions);
}

void EditorCtrl::ReplaceCurrentWord(const wxString& word) {
	const interval iv = GetWordIv(GetPos());

	Freeze();

	if (m_snippetHandler.IsActive()) {
		m_snippetHandler.Delete(iv.start, iv.end);
		m_snippetHandler.Insert(word);
		return;
	}
	else {
		RawDelete(iv.start, iv.end);
		unsigned int byte_len = RawInsert(iv.start, word, true);
		SetPos(iv.start + byte_len);
	}

	Freeze();
}

void EditorCtrl::SetSearchRange() {
	m_searchRanges = m_lines.GetSelections();
	m_lines.RemoveAllSelections();
	//if (!m_searchRanges.empty()) SetPos(m_searchRanges[0].start);
	DrawLayout();
}

void EditorCtrl::ClearSearchRange(bool reset) {
	if (!m_searchRanges.empty()) {
		if (reset && !m_lines.IsSelected()) {
			// Reset selection
			for (vector<interval>::const_iterator p = m_searchRanges.begin(); p != m_searchRanges.end(); ++p) {
				m_lines.AddSelection(p->start, p->end);
			}
		}
		m_searchRanges.clear();
		DrawLayout();
	}
}

bool EditorCtrl::DoFind(const wxString& text, unsigned int start_pos, int options, bool dir_forward) {
	wxASSERT(start_pos >= 0 && start_pos <= m_lines.GetLength());
	bool matchcase = options & FIND_MATCHCASE;

	m_lines.RemoveAllSelections();
	if (text.empty()) {
		DrawLayout();
		return false; // can't find empty text
	}

	// Use higlight styler
	if (options & FIND_HIGHLIGHT) m_search_hl_styler.SetSearch(text, options);
	else m_search_hl_styler.Clear();

	// Do the search
	search_result sr = {0,0,0};
	if (m_searchRanges.empty()) {
		cxLOCKDOC_READ(m_doc)
			if (options & FIND_USE_REGEX) {
				if (dir_forward) sr = doc.RegExFind(text, start_pos, matchcase);
				else sr = doc.RegExFindBackwards(text, start_pos, matchcase);
			}
			else {
				if (dir_forward) sr = doc.Find(text, start_pos, matchcase);
				else sr = doc.FindBackwards(text, start_pos, matchcase);
			}
		cxENDLOCK
	}
	else { // Search in selection(s)
		if (dir_forward) {
			// Find first range containing start_pos
			vector<interval>::const_iterator p = m_searchRanges.begin();
			for (; p != m_searchRanges.end(); ++p) {
				if (start_pos >= p->start && start_pos < p->end) break;
			}

			if (p != m_searchRanges.end()) {
				unsigned int rangestart = start_pos;
				for (; p != m_searchRanges.end(); ++p) {
					if (rangestart < p->start) rangestart = p->start;

					cxLOCKDOC_READ(m_doc)
						if (options & FIND_USE_REGEX) sr = doc.RegExFind(text, rangestart, matchcase, NULL, p->end);
						else sr = doc.Find(text, rangestart, matchcase, p->end);
					cxENDLOCK
					if (sr.error_code >= 0) break; // match found or error
				}
			}
			else sr.error_code = -1; // invalid start_pos;
		}
		else {
			// Find first range containing start_pos
			vector<interval>::reverse_iterator p = m_searchRanges.rbegin();
			for (; p != m_searchRanges.rend(); ++p) {
				if (start_pos > p->start && start_pos <= p->end) break;
			}

			if (p != m_searchRanges.rend()) {
				unsigned int rangestart = start_pos;
				for (; p != m_searchRanges.rend(); ++p) {
					if (rangestart > p->end) rangestart = p->end;

					cxLOCKDOC_READ(m_doc)
						if (options & FIND_USE_REGEX) sr = doc.RegExFindBackwards(text, rangestart, matchcase);
						else sr = doc.FindBackwards(text, rangestart, matchcase);
					cxENDLOCK

					if (sr.error_code >= 0) {
						if (sr.start < p->start) { // match outside sel
							sr.error_code = -1;
							continue;
						}
						else break; // match found or error
					}
				}
			}
			else sr.error_code = -1; // invalid start_pos;
		}
	}

	// Update the display
	if (sr.error_code >= 0) {
		// Select the found text
		m_lines.AddSelection(sr.start, sr.end);

		// Move caret to end of selection
		m_lines.SetPos(sr.end, false);

		// Make sure the new selection is shown
		if (!IsCaretVisible()) {
			MakeCaretVisibleCenter(); // first center vertically
			MakeSelectionVisible();
		}

		DrawLayout();
		return true;
	}
	else {
		DrawLayout();
		if (sr.error_code == -4) return true; // invalid (incomplete?) search pattern
		else return false; // text not found
	}
}

bool EditorCtrl::FindNextChar(wxChar c, unsigned int start_pos, unsigned int end_pos, interval& iv) const {
	search_result sr;
	cxLOCKDOC_READ(m_doc)
		sr = doc.Find(c, start_pos, true, end_pos);
	cxENDLOCK

	if (sr.error_code >= 0) {
		iv.start = sr.start;
		iv.end = sr.end;
		return true;
	}
	else return false;
}

cxFindResult EditorCtrl::Find(const wxString& text, int options) {
	// We have to be aware of incremental searches, so to find out if we are continuing
	// a previous search we check if lastpos/pos are in sync with start_pos/found_pos
	if (m_search_found_pos != m_lines.GetPos() || m_search_start_pos != m_lines.GetLastpos()) {
		m_search_start_pos = m_lines.GetPos();
		m_lines.SetLastpos(m_search_start_pos);
	}

	cxFindResult result = cxNOT_FOUND;
	if (DoFind(text, m_search_start_pos, options)) result = cxFOUND;

	if (result == cxNOT_FOUND && m_search_start_pos > 0) {
		// Restart search from top
		const unsigned int start_pos = m_searchRanges.empty() ? 0 : m_searchRanges[0].start;
		if (DoFind(text, start_pos, options)) result = cxFOUND_AFTER_RESTART;
	}

	if (result == cxNOT_FOUND) {
		m_lines.SetPos(m_search_start_pos, false); // Move back to where the search started
		m_search_found_pos = m_search_start_pos;
	}
	else m_search_found_pos = m_lines.GetPos();

	return result;
}

cxFindResult EditorCtrl::FindNext(const wxString& text, int options) {
	int start_pos;
	if (options & FIND_RESTART) start_pos = m_searchRanges.empty() ? 0 : m_searchRanges[0].start;
	else {
		if (m_lines.IsSelected()) {
			const interval& iv = m_lines.GetSelections().back();
			start_pos = iv.end;

			// Avoid hitting same zero-length selection
			if (iv.start == iv.end) {
				++start_pos;
				if (start_pos > (int)m_lines.GetLength()) return cxNOT_FOUND;
			}
		}
		else start_pos = m_lines.GetPos();
	}

	cxFindResult result = cxNOT_FOUND;
	if (DoFind(text, start_pos, options)) result = cxFOUND;

	if (result == cxNOT_FOUND && start_pos > 0) {
		// Restart search from top
		const unsigned int start_pos = m_searchRanges.empty() ? 0 : m_searchRanges[0].start;
		if (DoFind(text, start_pos, options)) result = cxFOUND_AFTER_RESTART;
	}

	// Make sure that next search has the new starting point
	// (no selection if invalid pattern)
	if (result != cxNOT_FOUND && IsSelected()) {
		m_lines.SetLastpos(m_lines.GetSelections()[0].start);
	}

	return result;
}

bool EditorCtrl::FindPrevious(const wxString& text, int options) {
	int start_pos;
	if (options & FIND_RESTART) start_pos = m_searchRanges.empty() ? 0 : m_searchRanges.back().end;
	else {
		if (m_lines.IsSelected()) {
			const interval& iv = m_lines.GetSelections()[0];
			start_pos  = iv.start;

			// Avoid hitting same zero-length selection
			if (iv.start == iv.end) {
				if (start_pos == 0) return false;
				--start_pos;
			}
		}
		else start_pos = m_lines.GetPos();
	}

	const bool result = DoFind(text, start_pos, options, false);

	// Make sure that next search has the new starting point
	// (no selection if invalid pattern)
	if (result && IsSelected()) m_lines.SetLastpos(m_lines.GetSelections()[0].start);
	return result;
}

wxString EditorCtrl::ParseReplaceString(const wxString& replacetext, const map<unsigned int,interval>& captures, const vector<char>* source) const {
	RepParseState state(replacetext, captures, source);

	const wxChar *start = replacetext.c_str();
	DoRepParse(state, start, start + replacetext.size());

	return state.newtext;
}

void EditorCtrl::DoRepParse(RepParseState& state, const wxChar* start, const wxChar* end) const {
	wxASSERT(start <= end);

	for ( const wxChar *p = start; p < end; ++p ) {
 		size_t index = (size_t)-1;

        if (*p == wxT('$')) {
            if (wxIsdigit(*++p)) {
                // back reference
                wxChar *end;
                index = (size_t)wxStrtoul(p, &end, 10);
                p = end - 1; // -1 to compensate for p++ in the loop
            }

			// do we have a back reference?
			if (index != (size_t)-1) {
				// yes, get its text
				map<unsigned int,interval>::const_iterator iv = state.captures.find(index);
				if ( iv != state.captures.end() ) {
					wxString reftext;
					if (state.source) {
						reftext = wxString(&*state.source->begin() + iv->second.start, wxConvUTF8, iv->second.end - iv->second.start);
					}
					else {
						cxLOCKDOC_READ(m_doc)
							reftext = doc.GetTextPart(iv->second.start, iv->second.end);
						cxENDLOCK
					}

					// Case foldings
					if (state.caseChar && !reftext.empty()) {
						if (state.lowcase) reftext[0] = wxTolower(reftext[0]);
						else if (state.upcase) reftext[0] = wxToupper(reftext[0]);
						else wxASSERT(false);

						state.caseChar = false;
						state.lowcase = false;
						state.upcase = false;
					}
					else if (state.caseText) {
						if (state.lowcase) reftext.MakeLower();
						else if (state.upcase) reftext.MakeUpper();
						else wxASSERT(false);
					}

					state.newtext += reftext;
				}
			}
        }
		else if (*p == wxT('\\')) {
			++p;
			if (p < end) {
				if (*p == wxT('n')) state.newtext += wxT('\n');
				else if (*p == wxT('t')) state.newtext += m_indent;
				else if (*p == wxT('l')) {
					state.caseChar = true;
					state.lowcase = true;
					state.upcase = false;
				}
				else if (*p == wxT('u')) {
					state.caseChar = true;
					state.lowcase = false;
					state.upcase = true;
				}
				else if (*p == wxT('L')) {
					state.caseText = true;
					state.lowcase = true;
					state.upcase = false;
				}
				else if (*p == wxT('U')) {
					state.caseText = true;
					state.lowcase = false;
					state.upcase = true;
				}
				else if (*p == wxT('E')) {
					state.caseText = false;
					state.caseChar = false;
					state.lowcase = false;
					state.upcase = false;
				}
				else state.newtext += *p;
			}
			else {
				state.newtext += '\\';
				break;
			}
		}
		else if (*p == wxT('(') && *(p+1) == wxT('?')) {
			// Parse condition
			const wxChar* p2 = p+2;
			const wxChar* inStart = NULL;
			const wxChar* inEnd = NULL;
			const wxChar* otStart = NULL;
			const wxChar* otEnd = NULL;
			bool success = false;

			if (wxIsdigit(*p2)) {
				// Get capture index
				wxChar *iend;
				unsigned int index = wxStrtoul(p2, &iend, 10);
				p2 = iend;

				if (*p2 != wxT(':')) goto error;
				++p2;

				inStart = p2;
				inEnd = p2;

				// find insertion end
				unsigned int pcount = 0;
				while(1) {
					if (!inEnd) goto error;
					else if (*inEnd == wxT('\\')) ++inEnd; // ignore escaped chars
					else if (*inEnd == wxT('(')) ++pcount; // count parens
					else if (*inEnd == wxT(')')) if (pcount) --pcount; else break;
					else if (*inEnd == wxT(':')) {
						// find 'otherwise' end
						otStart = otEnd = inEnd + 1;
						pcount = 0;
						while(1) {
							if (!otEnd) goto error;
							else if (*otEnd == wxT('\\')) ++otEnd; // ignore escaped chars
							else if (*otEnd == wxT('(')) ++pcount; // count parens
							else if (*otEnd == wxT(')')) {if (pcount) --pcount; else break;}
							++otEnd;
						}
						break;
					}
					++inEnd;
				}

				// parse the part that meet condition
				map<unsigned int,interval>::const_iterator iv = state.captures.find(index);
				if (iv != state.captures.end()) {
					DoRepParse(state, inStart, inEnd);
				}
				else if (otStart) {
					DoRepParse(state, otStart, otEnd);
				}
				success = true;
			}

error:
			if (success) p = otEnd ? otEnd : inEnd;
			else state.newtext += *p; // ordinary character

		}
        else {
			if (state.caseChar || state.caseText) {
				if (state.lowcase) state.newtext += wxTolower(*p);
				else if (state.upcase) state.newtext += wxToupper(*p);
				else wxASSERT(false);

				state.caseChar = false;
			}
			else state.newtext += *p; // ordinary character
		}
	}
}

search_result EditorCtrl::RegExFind(const pcre* re, const pcre_extra* study, unsigned int start_pos, map<unsigned int,interval> *captures, unsigned int end_pos, int search_options) const {
	wxASSERT(start_pos <= GetLength());
	wxASSERT(end_pos == 0 || (end_pos >= start_pos && end_pos <= GetLength()));
	if (end_pos == 0) end_pos = GetLength();

	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	search_result sr;
	search_options |= PCRE_NO_UTF8_CHECK;

	int rc = -1;
	unsigned int pos = start_pos;
	unsigned int lineStart;
	unsigned int lineEnd;
	vector<char> line;
	cxLOCKDOC_READ(m_doc)
		lineStart = doc.GetLineStart(start_pos);
		lineEnd = doc.GetLine(lineStart, line);
	cxENDLOCK
	unsigned int lineLen = lineEnd - lineStart;

	// Do the search (one line at a time)
	for (;;) {
		if (pos >= end_pos) break;
		if (pos == lineEnd) {
			lineStart = lineEnd;
			cxLOCKDOC_READ(m_doc)
				lineEnd = doc.GetLine(lineStart, line);
			cxENDLOCK
			lineLen = lineEnd - lineStart;
		}

		rc = pcre_exec(
			re,                   // the compiled pattern
			study,                // extra data - if we study the pattern
			&*line.begin(),         // the subject string
			lineLen,              // the length of the subject
			pos - lineStart,      // start at offset in the subject
			search_options,       // options
			ovector,              // output vector for substring information
			OVECCOUNT);           // number of elements in the output vector

		if (rc < 0) {
			// Go to next line
			pos = lineEnd;
		}
		else break; // match found!
	}

	// Copy match info from ovector to result struct
	sr.error_code = rc;
	if (rc >= 0) {
		sr.start = lineStart + ovector[0];
		sr.end = lineStart + ovector[1];

		wxASSERT(sr.start >= 0 && sr.start <= GetLength());
		wxASSERT(sr.end >= sr.start && sr.end <= GetLength());
	}

	if (captures != NULL && rc > 0) {
		// copy intervals to the supplied vector
		for (int i = 0; i < rc; ++i) {
			if (ovector[2*i] != -1) {
				const interval iv(lineStart + ovector[2*i], lineStart + ovector[2*i+1]);
				(*captures)[i] = iv;
			}
		}
	}

	return sr;
}

search_result EditorCtrl::RegExFind(const wxString& searchtext, unsigned int start_pos, bool matchcase, map<unsigned int,interval> *captures, unsigned int end_pos) const {
	wxASSERT(end_pos == 0 || (end_pos >= start_pos && end_pos <= GetLength()));
	if (end_pos == 0) end_pos = GetLength();

	int options = PCRE_UTF8;
	if (!matchcase) options |= PCRE_CASELESS;

	// Check if we have the regex cached
	if (m_re) {
		if (searchtext == m_regex_cache && options == m_options_cache) {
			// We might want to study the regex the first time we end here
		}
		else {
			free(m_re);
			m_re = NULL;
		}
	}

	// Compile the pattern
	if (!m_re) {
		const char *error;
		int erroffset;

		m_re = pcre_compile(
			searchtext.mb_str(wxConvUTF8),   // the pattern
			options,              // options
			&error,               // for error message
			&erroffset,           // for error offset
			NULL);                // use default character tables
		if (m_re == 0) {
			search_result sr;
			sr.error_code = -4; // invalid pattern
			return sr;
		}
		else {
			// The compiled regex (m_re) is cached for possible reuse in next search
			// it is finally free'd in destructor
			m_options_cache = options;
			m_regex_cache = searchtext;
		}
	}

	// Do the search
	return RegExFind(m_re, NULL, start_pos, captures, end_pos);
}

search_result EditorCtrl::RawRegexSearch(const char* regex, unsigned int subjectStart, unsigned int subjectEnd, unsigned int pos, map<unsigned int,interval> *captures) const {
	wxASSERT(regex);
	wxASSERT(subjectStart <= subjectEnd);
	wxASSERT(subjectEnd <= GetLength());
	wxASSERT(pos >= subjectStart && pos <= subjectEnd);

	search_result sr;

	// Compile the pattern
	const char *error;
	int erroffset;
	pcre* re = pcre_compile(
		regex,      // the pattern
		PCRE_UTF8,  // options
		&error,     // for error message
		&erroffset, // for error offset
		NULL);      // use default character tables
	if (!re) {
		sr.error_code = -4; // invalid pattern
		return sr;
	}

	// Get the subject
	const char* subject = "";
	const unsigned int subjectLen = subjectEnd - subjectStart;
	vector<char> subjectBuf(subjectLen);
	if (subjectLen) {
		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(subjectStart, subjectEnd, (unsigned char*)&*subjectBuf.begin());
		cxENDLOCK
		subject = &*subjectBuf.begin();
	}

	// Do the search
	int rc = -1;
	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	rc = pcre_exec(
		re,                   // the compiled pattern
		NULL,                 // extra data - if we study the pattern
		subject,              // the subject string
		subjectLen,           // the length of the subject
		pos - subjectStart,   // start at offset in the subject
		PCRE_NO_UTF8_CHECK,   // options
		ovector,              // output vector for substring information
		OVECCOUNT);           // number of elements in the output vector

	free(re); // clean up

	// Copy match info from ovector to result struct
	sr.error_code = rc;
	if (rc >= 0) {
		sr.start = subjectStart + ovector[0];
		sr.end = subjectStart + ovector[1];
	}

	if (captures && rc > 0) {
		// copy intervals to the supplied vector
		for (int i = 0; i < rc; ++i) {
			if (ovector[2*i] != -1) {
				const interval iv(subjectStart + ovector[2*i], subjectStart + ovector[2*i+1]);
				(*captures)[i] = iv;
			}
		}
	}

	return sr;
}

search_result EditorCtrl::RawRegexSearch(const char* regex, const vector<char>& subject, unsigned int pos, map<unsigned int,interval> *captures) const {
	wxASSERT(regex);
	wxASSERT(pos < subject.size() || pos == 0);

	search_result sr;
	if (subject.empty()) {
		sr.error_code = -1;
		return sr;
	}

	// Compile the pattern
	const char *error;
	int erroffset;
	const pcre* const re = pcre_compile(
		regex,      // the pattern
		PCRE_UTF8,  // options
		&error,     // for error message
		&erroffset, // for error offset
		NULL);      // use default character tables
	if (!re) {
		sr.error_code = -4; // invalid pattern
		return sr;
	}

	// Do the search
	int rc = -1;
	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	rc = pcre_exec(
		re,                   // the compiled pattern
		NULL,                 // extra data - if we study the pattern
		&*subject.begin(),      // the subject string
		(int)subject.size(),           // the length of the subject
		pos,   // start at offset in the subject
		PCRE_NO_UTF8_CHECK,   // options
		ovector,              // output vector for substring information
		OVECCOUNT);           // number of elements in the output vector


	// Copy match info from ovector to result struct
	sr.error_code = rc;
	if (rc >= 0) {
		sr.start = ovector[0];
		sr.end = ovector[1];
	}

	if (captures && rc > 0) {
		// copy intervals to the supplied vector
		for (int i = 0; i < rc; ++i) {
			if (ovector[2*i] != -1) {
				const interval iv(ovector[2*i], ovector[2*i+1]);
				(*captures)[i] = iv;
			}
		}
	}

	return sr;
}



search_result EditorCtrl::RegExFindBackwards(const wxString& searchtext, unsigned int start_pos, unsigned int end_pos, bool matchcase) const {
	wxASSERT(end_pos <= start_pos && start_pos <= GetLength());
	if (end_pos == 0) end_pos = GetLength();

	int options = PCRE_UTF8;
	if (!matchcase) options |= PCRE_CASELESS;

	// Check if we have the regex cached
	if (m_re) {
		if (searchtext == m_regex_cache && options == m_options_cache) {
			// We might want to study the regex the first time we end here
		}
		else {
			free(m_re);
			m_re = NULL;
		}
	}

	// Compile the pattern
	if (!m_re) {
		const char *error;
		int erroffset;

		m_re = pcre_compile(
			searchtext.mb_str(wxConvUTF8),   // the pattern
			options,              // options
			&error,               // for error message
			&erroffset,           // for error offset
			NULL);                // use default character tables

		if (m_re == 0) {
			search_result sr;
			sr.error_code = -4; // invalid pattern
			return sr;
		}
		else {
			// The compiled regex (m_re) is cached for possible reuse in next search
			// it is finally free'd in destructor
			m_options_cache = options;
			m_regex_cache = searchtext;
		}
	}

	// Do the search
	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	search_result sr = {-1, 0, 0};

	int search_options = PCRE_NO_UTF8_CHECK;

	// Get current line (but only use up till start_pos)
	int rc = -1;
	unsigned int lineStart;
	unsigned int lineEnd;
	vector<char> line;
	cxLOCKDOC_READ(m_doc)
		lineStart = doc.GetLineStart(start_pos);
		lineEnd = doc.GetLine(lineStart, line);
	cxENDLOCK
	unsigned int lineLen = start_pos - lineStart;
	if (lineLen < lineEnd - lineStart) search_options |= PCRE_NOTEOL;
	unsigned int pos = lineStart;

	// Do the search (one line at a time)
	for (;;) {
		rc = pcre_exec(
			m_re,                   // the compiled pattern
			NULL,                // extra data - if we study the pattern
			&*line.begin(),         // the subject string
			lineLen,              // the length of the subject
			pos - lineStart,      // start at offset in the subject
			search_options,       // options
			ovector,              // output vector for substring information
			OVECCOUNT);           // number of elements in the output vector

		if (rc < 0) {
			if (sr.error_code >= 0) break; // we have passed last match

			// Go to previous line
			pos = lineStart;
		}
		else {
			// Copy match info from ovector to result struct
			sr.error_code = rc;
			if (rc >= 0) {
				sr.start = lineStart + ovector[0];
				sr.end = lineStart + ovector[1];

				wxASSERT(sr.start >= 0 && sr.start <= GetLength());
				wxASSERT(sr.end >= sr.start && sr.end <= GetLength());
			}
		}

		if (pos == 0) break;
		if (pos == lineStart) {
			if (sr.error_code >= 0) break; // we have passed last match

			--pos; // we have to remove one from pos to ensure we are in previous line
			cxLOCKDOC_READ(m_doc)
				lineStart = doc.GetLineStart(pos);
				lineEnd = doc.GetLine(lineStart, line);
			cxENDLOCK
			lineLen = lineEnd - lineStart;
			pos = lineStart;
		}
	}

	// This will return the last found match
	return sr;
}

bool EditorCtrl::Replace(const wxString& searchtext, const wxString& replacetext, int options) {
	if (m_lines.IsSelected()) {
		const interval iv = m_lines.GetSelections()[0];
		m_lines.RemoveAllSelections();
		unsigned int byte_len = 0;

		if (options & FIND_USE_REGEX) {
			// We need to search again to get captures
			map<unsigned int,interval> captures;
			const search_result result = RegExFind(searchtext, iv.start, options & FIND_MATCHCASE, &captures);

			if (result.error_code >= 0) {
				const wxString new_replacetext = ParseReplaceString(replacetext, captures);
				RawDelete(iv.start, iv.end);
				byte_len = RawInsert(iv.start, new_replacetext, false);
				m_lines.SetPos(iv.start + byte_len); // move to end of insertion
			}
			else wxASSERT(false);
		}
		else {
			RawDelete(iv.start, iv.end);
			byte_len = RawInsert(iv.start, replacetext, false);
			m_lines.SetPos(iv.start + byte_len); // move to end of insertion
		}

		// Adjust searchranges
		if (!m_searchRanges.empty()) {
			const int diff = byte_len - (iv.end - iv.start);
			for (vector<interval>::iterator p = m_searchRanges.begin(); p != m_searchRanges.end(); ++p) {
				if (p->start > iv.start) p->start += diff;
				if (p->end > iv.start) p->end += diff;
			}
		}

		return DoFind(searchtext, m_lines.GetPos(), options);
	}
	else {
		int startpos = (options & FIND_RESTART) ? 0 : m_lines.GetPos();
		return DoFind(searchtext, startpos, options);
	}
}

bool EditorCtrl::ReplaceAll(const wxString& searchtext, const wxString& replacetext, int options) {
	bool matchcase = options & FIND_MATCHCASE;

	if (m_lines.GetLength() == 0 || searchtext.empty()) return false;
	const wxBusyCursor busy; // show busy cursor

	// Prepare before searching
	doc_id oldDoc;
	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze();
		oldDoc = doc.GetDocument();
	cxENDLOCK
	m_lines.RemoveAllSelections();

	// Set if we should highlight afterwards
	if (options & FIND_HIGHLIGHT) m_search_hl_styler.SetSearch(searchtext, options);
	else m_search_hl_styler.Clear();

	search_result lastresult = {-1, 0, 0};
	search_result result = {-1, 0, 0};
	unsigned int start_pos = m_searchRanges.empty() ? 0 : m_searchRanges[0].start;
	vector<interval>::iterator p = m_searchRanges.begin();
	map<unsigned int,interval> captures;
	unsigned int byte_len = 0;

	// Replace and continue search
	cxLOCKDOC_WRITE(m_doc)
		doc.StartChange();
	cxENDLOCK
	while(1) {
		captures.clear();

		// Find match
		if (m_searchRanges.empty()) {
			cxLOCKDOC_READ(m_doc)
				if (options & FIND_USE_REGEX) result = doc.RegExFind(searchtext, start_pos, matchcase, &captures);
				else result = doc.Find(searchtext, start_pos, matchcase);
			cxENDLOCK
		}
		else {
			for (; p != m_searchRanges.end(); ++p) {
				if (start_pos < p->start) start_pos = p->start;

				cxLOCKDOC_READ(m_doc)
					if (options & FIND_USE_REGEX) result = doc.RegExFind(searchtext, start_pos, matchcase, &captures, p->end);
					else result = doc.Find(searchtext, start_pos, matchcase, p->end);
				cxENDLOCK
				if (result.error_code >= 0) break; // match found or error
			}
			if (p == m_searchRanges.end()) break; // outside ranges
		}

		// Handle result
		if (result.error_code < 0) break; // no match found
		lastresult = result;

		// Get the replacement string
		wxString textNew;
		if (!replacetext.empty()) {
			if (options & FIND_USE_REGEX) textNew = ParseReplaceString(replacetext, captures);
			else textNew = replacetext;
		}

		// Delete original
		if (result.start != result.end) {
			cxLOCKDOC_WRITE(m_doc)
				doc.Delete(result.start, result.end);
			cxENDLOCK
			//m_lines.Delete(result.start, result.end);
		}

		// Insert replacement
		if (!textNew.empty()) {
			cxLOCKDOC_WRITE(m_doc)
				byte_len = doc.Insert(result.start, textNew);
			cxENDLOCK
			//m_lines.Insert(result.start, byte_len);
		}

		// Adjust searchranges
		if (!m_searchRanges.empty()) {
			const int diff = byte_len - (result.end - result.start);
			for (vector<interval>::iterator p2 = p; p2 != m_searchRanges.end(); ++p2) {
				if (p2->start > result.start) p2->start += diff;
				if (p2->end > result.start) p2->end += diff;
			}
		}

		// If we have replaced upto end-of-line, move to next
		// line to avoid infinite replace of ($).
		start_pos = result.start+byte_len;
		cxLOCKDOC_READ(m_doc)
			if (start_pos == doc.GetLength()) break;
			if (doc.GetChar(start_pos) == wxT('\n')) {
				++start_pos;
				if (start_pos == doc.GetLength()) break;
			}
		cxENDLOCK
	}
	cxLOCKDOC_WRITE(m_doc)
		doc.EndChange();
	cxENDLOCK

	// Update lines and stylers
	ApplyDiff(oldDoc);

	// Update the caret position
	if (lastresult.error_code >= 0) m_lines.SetPos(lastresult.start + byte_len);
	// else no change

	MarkAsModified();

	// Update the display
	MakeCaretVisible();
	DrawLayout();

	return true;
}
/*
bool EditorCtrl::ReplaceAllRegex(const wxString& regex, const wxString& replacetext, int options) {
	if (m_lines.GetLength() == 0 || searchtext.empty()) return false;

	bool matchcase = options & FIND_MATCHCASE;
	const wxBusyCursor busy;

	// Prepare before searching
	Freeze();
	m_lines.RemoveAllSelections();
	map<unsigned int,interval> captures;

	// Set if we should highlight afterwards
	if (options & FIND_HIGHLIGHT) m_search_hl_styler.SetSearch(searchtext, options);
	else m_search_hl_styler.Clear();

	int options = PCRE_UTF8;
	if (!matchcase) options |= PCRE_CASELESS;

	// Compile the pattern
	const char *error;
	int erroffset;
	const pcre* const re = pcre_compile(
		regex.mb_str(wxConvUTF8), // the pattern
		options,                  // options
		&error,                   // for error message
		&erroffset,               // for error offset
		NULL);                    // use default character tables
	if (!re) {
		return false; // invalid pattern
	}


}
*/

void EditorCtrl::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	const wxSize size = GetClientSize();
	const int editorSizeX = ClientWidthToEditor(size.x);

	// Re-Blit MemoryDC to Display
	const unsigned int xpos = m_gutterLeft ? m_gutterWidth : 0;
#ifdef __WXMSW__
	::BitBlt(GetHdcOf(dc), xpos, 0,(int)editorSizeX, (int)size.y, GetHdcOf(mdc), 0, 0, SRCCOPY);
#else
	dc.Blit(xpos, 0,  editorSizeX, size.y, &mdc, 0, 0);
#endif
}

void EditorCtrl::OnSize(wxSizeEvent& WXUNUSED(event)) {
	// Draw the new layout
	m_isResizing = true;
	DrawLayout();
}

void EditorCtrl::OnKeyDown(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ALT) {
		m_blockKeyState = BLOCKKEY_INIT;
		m_blocksel_ids.clear();
	}
	event.Skip();
}

void EditorCtrl::OnKeyUp(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ALT) {
		m_blockKeyState = BLOCKKEY_NONE;
		m_blocksel_ids.clear();
	}
	event.Skip();
}

void EditorCtrl::OnChar(wxKeyEvent& event) {
	// Remove tooltips
	/*if (m_revTooltip.IsShown()) {
		m_revTooltip.Hide();
	}*/

	wxString modifiers;
	if (event.ControlDown()) modifiers += wxT("CTRL-");
	if (event.AltDown()) modifiers += wxT("ALT-");
	if (event.MetaDown()) modifiers += wxT("META-");
	if (event.ShiftDown()) modifiers += wxT("SHIFT-");

	wxLogDebug(wxT("KeyEvent: %s%d %c(%d)"), modifiers.c_str(), event.GetKeyCode(), event.GetUnicodeKey(), event.GetUnicodeKey());

	// Key Diagnostics
	if (m_parentFrame.IsKeyDiagMode()) {
		Insert(wxString::Format(wxT("\tKeyEvent: %s%c (%d)\n"), modifiers.c_str(), event.GetUnicodeKey(), event.GetKeyCode()));
		Insert(wxString::Format(wxT("\tunicode: %x\n"), event.GetUnicodeKey()));
	}

	int key = event.GetKeyCode();
	unsigned int pos;
	const unsigned int oldpos = m_lines.GetPos();

	// Invalidate state
	if (!m_pairStack.empty() && (oldpos < m_pairStack.back().start || oldpos > m_pairStack.back().end)) {
		m_pairStack.clear();
	}
	m_lastScopePos = -1; // scope selections

	// Menu shortcuts might have set commandMode
	// without resetting it.
	commandMode = event.ControlDown();

	// Check if unicode value is safe to use instead of keycode
	const wxChar c = event.GetUnicodeKey();
// FIXME - is this needed (or it's just for speed?)
#ifdef __WXMSW__
	if ((unsigned int)c > 127) {
		InsertChar(c);
	}
	else 
#endif
	{

		if (commandMode) {
			DoCommand(key);
			switch (key) {
			case WXK_INSERT:
			case 3: // Ctrl-C
				OnCopy();
				break;

			case 11: // Ctrl-K
				if (event.ShiftDown()) DelCurrentLine(false);
				else DelCurrentLine(true);
				break;

			case 16: // Ctrl-P
				ShowScopeTip();
				break;

			case 22: // Ctrl-V
				OnPaste();
				break;

			case 24: // Ctrl-X
				OnCut();
				break;

			case WXK_SPACE: // Ctrl-space
				/*if (m_lines.GetLastpos() != m_lines.GetPos()) {
					m_lines.RemoveAllSelections();
					m_lines.AddSelection(m_lines.GetLastpos(), m_lines.GetPos());
				}*/
				DoCompletion();
				break;

			case WXK_LEFT: // Ctrl <-
				CursorWordLeft(event.ShiftDown() || event.AltDown());
				break;

			case WXK_RIGHT: // Ctrl ->
				CursorWordRight(event.ShiftDown() || event.AltDown());
				break;

			case WXK_UP: // Ctrl arrow up
				if (event.ShiftDown() || event.AltDown()) CursorUp(true);
				else {
					scrollPos = scrollPos - (scrollPos % m_lines.GetLineHeight()) - m_lines.GetLineHeight();
					if (scrollPos < 0) scrollPos = 0;

					// Make sure caret stays visible
					const wxPoint cpos = m_lines.GetCaretPos();
					const wxSize clientsize = GetClientSize();
					const wxSize caretsize = caret->GetSize();
					if (cpos.y + caretsize.y >= scrollPos + clientsize.y) {
						if (lastaction == ACTION_UP) m_lines.ClickOnLine(lastxpos, scrollPos + clientsize.y - 1);
						else {
							lastxpos = cpos.x;
							m_lines.ClickOnLine(cpos.x, scrollPos + clientsize.y - 1);
						}
						lastaction = ACTION_UP;
					}

					// Draw the updated view (without first calling MakeCaretVisible())
					DrawLayout();
					return;
				}
				break;

			case WXK_DOWN: // Ctrl arrow down
				if (event.ShiftDown() || event.AltDown()) CursorDown(true);
				else {
					const wxSize size = GetClientSize();
					scrollPos = scrollPos - (scrollPos % m_lines.GetLineHeight()) + m_lines.GetLineHeight();
					const int lastVisibleTopPos = wxMax(0, m_lines.GetHeight() - size.y);
					scrollPos = wxMin(lastVisibleTopPos, scrollPos);

					// Make sure caret stays visible
					const wxPoint cpos = m_lines.GetCaretPos();
					if (cpos.y < scrollPos) {
						if (lastaction == ACTION_DOWN) m_lines.ClickOnLine(lastxpos, scrollPos + 1);
						else {
							lastxpos = cpos.x;
							m_lines.ClickOnLine(cpos.x, scrollPos + 1);
						}
						lastaction = ACTION_DOWN;
					}

					// Draw the updated view (without first calling MakeCaretVisible())
					DrawLayout();
					return;
				}
				break;

			case WXK_HOME:
				SetPos(0);

				// Handle selection
				if (!event.ShiftDown()) m_lines.RemoveAllSelections();
				else if (oldpos != 0) SelectFromMovement(0, oldpos);

				lastaction = ACTION_NONE;
				break;

			case WXK_END:
				pos = GetLength();

				// Check if end is in a fold
				unsigned int fold_start;
				if (IsPosInFold(pos, &fold_start)) {
					pos = fold_start;
				}

				SetPos(pos);

				// Handle selection
				if (!event.ShiftDown()) m_lines.RemoveAllSelections();
				else if (oldpos != m_lines.GetPos()) SelectFromMovement(oldpos, m_lines.GetPos());

				lastaction = ACTION_NONE;
				break;

			case WXK_F4:
				// Close tab
				break;

			default:
				commandMode = false; // do nothing if key is unmapped
			}
			wxLogDebug(wxT("ctrl %d"), key);
		}

		if (!commandMode) {

			switch (key) {

	#ifdef __WXDEBUG__
			/*case WXK_F1:
				// Simulate crash
				pos = 0;
				pos = 5 / pos;
				break;*/

			case WXK_F2:
				// Simulate crash
				wxASSERT(false);
				break;

			case WXK_F4:
				m_lines.Print();
				break;

			case WXK_F5:
				cxLOCKDOC_READ(m_doc)
					doc.PrintAll();
				cxENDLOCK
				break;

			case WXK_F7:
				wxLogDebug(wxT("Charpos: %u"), m_lines.GetPos());
				wxLogDebug(wxT("ScrollPos: %d"), scrollPos);
				if (m_syntaxstyler.IsOk()) {
					wxLogDebug(wxT("Scope:"));
					const deque<const wxString*> scopes = m_syntaxstyler.GetScope(m_lines.GetPos());
					for (unsigned int i = 0; i < scopes.size(); ++i) {
						wxLogDebug(wxT("%s"), scopes[i]->c_str());
					}
				}

				//doc.Encode();
				break;

			case WXK_F8:
				{
					const wxSize size = GetClientSize();
					const int last_scroll = m_lines.GetHeight() - size.y;
					while(scrollPos < last_scroll) {
						old_scrollPos = scrollPos;
						++scrollPos;
						DrawLayout(true);
					}
				}
				return;

			case WXK_F9:
				{
					tmCommand action;
					action.input = tmCommand::ciSEL;
					action.inputXml = true;
					const char* test = "cat";
					vector<char> content(test, test + strlen(test));
					action.SwapContent(content);
					action.output = tmCommand::coNEWDOC;
					DoAction(action, NULL, false);
				}
				break;

			case WXK_F10:
				{
					vector<Styler_Syntax::SymbolRef> symbols;
					m_syntaxstyler.GetSymbols(symbols);
					for (vector<Styler_Syntax::SymbolRef>::const_iterator p = symbols.begin(); p != symbols.end(); ++p) {
						wxLogDebug(wxT("%d-%d -> \"%s\" -> \"%s\""), p->start, p->end, GetText(p->start, p->end).c_str(), p->transform->c_str());
					}
				}
				break;

			case WXK_F11:
				TestMilestones();
				break;
	#endif //__WXDEBUG__


			case WXK_LEFT:
			case WXK_NUMPAD_LEFT:
				CursorLeft(event.ShiftDown() || event.AltDown());
				break;

			case WXK_RIGHT:
			case WXK_NUMPAD_RIGHT:
				CursorRight(event.ShiftDown() || event.AltDown());
				break;

			case WXK_UP:
			case WXK_NUMPAD_UP:
				CursorUp(event.ShiftDown() || event.AltDown());
				break;

			case WXK_DOWN:
			case WXK_NUMPAD_DOWN:
				CursorDown(event.ShiftDown() || event.AltDown());
				break;

			case WXK_HOME:
			case WXK_NUMPAD_HOME:
			case WXK_NUMPAD_BEGIN:
				{
					const unsigned int currentLine = m_lines.GetCurrentLine();
					const unsigned int indentPos = GetLineIndentPos(currentLine);

					if (oldpos <= indentPos) {
						// Move to start-of-line
						m_lines.SetPos(m_lines.GetLineStartpos(currentLine));
					}
					else {
						// Move to first text after indentation
						m_lines.SetPos(indentPos);
					}

					// Handle selection
					if (!event.ShiftDown()) m_lines.RemoveAllSelections();
					else if (oldpos != m_lines.GetPos()) SelectFromMovement(oldpos, m_lines.GetPos());

					lastaction = ACTION_NONE;
				}
				break;

			case WXK_END:
			case WXK_NUMPAD_END:
				m_lines.SetPos(m_lines.GetLineEndpos(m_lines.GetCurrentLine()));

				// Handle selection
				if (!event.ShiftDown()) m_lines.RemoveAllSelections();
				else if (oldpos != m_lines.GetPos()) SelectFromMovement(oldpos, m_lines.GetPos());

				lastaction = ACTION_NONE;
				break;


			case WXK_PAGEUP:
			case WXK_NUMPAD_PAGEUP:
				PageUp(event.ShiftDown());
				lastaction = ACTION_NONE;
				break;

			case WXK_PAGEDOWN:
			case WXK_NUMPAD_PAGEDOWN:
				PageDown(event.ShiftDown());
				lastaction = ACTION_NONE;
				break;

			case WXK_INSERT:
			case WXK_NUMPAD_INSERT:
				if (event.ShiftDown()) OnPaste();
				break;

			case WXK_DELETE:
			case WXK_NUMPAD_DELETE:
				if (event.ShiftDown()) OnCut();
				else {
					pos = m_lines.GetPos();

					// Reset autoPair state if deleting outside inner pair
					if (!m_pairStack.empty() && (m_pairStack.back().start > pos || m_pairStack.back().end <= pos)) {
						m_pairStack.clear();
					}

					// Check if we should delete entire word
					if (event.ControlDown() && !m_lines.IsSelected()) {
						const interval iv = GetWordIv(pos);

						if (pos >= iv.start && pos < iv.end) {
							Delete(pos, iv.end);
							break;
						}
					}

					// Check if we delete outside zero-width selection
					if (m_lines.IsSelected()) {
						const vector<interval>& sels = m_lines.GetSelections();
						if (sels.size() == 1 && sels[0].start == sels[0].end) {
							m_lines.RemoveAllSelections();
						}
					}

					// Handle deletions in snippet
					if (m_snippetHandler.IsActive()) {
						if (m_lines.IsSelected()) DeleteSelections();
						else if (pos < m_lines.GetLength()) {
							unsigned int nextcharpos;
							cxLOCKDOC_READ(m_doc)
								nextcharpos = doc.GetNextCharPos(pos);
							cxENDLOCK
							m_snippetHandler.Delete(pos, nextcharpos);
						}
						break;
					}

					if (pos != lastpos || lastaction != ACTION_DELETE) {
						cxLOCKDOC_WRITE(m_doc)
							doc.Freeze();
						cxENDLOCK
					}

					if (m_lines.IsSelected()) {
						if (m_lines.IsSelectionShadow()) {
							if (DeleteInShadow(pos, true)) break; // if false we have to del outside shadow
						}
						else {
							DeleteSelections();
							cxLOCKDOC_WRITE(m_doc)
								doc.Freeze();
							cxENDLOCK
							break;
						}

						m_lines.RemoveAllSelections();
					}

					if (pos >= m_lines.GetLength()) return; // Can't delete at end of text

					unsigned int nextpos;
					wxChar nextchar;
					cxLOCKDOC_READ(m_doc)
						nextpos = doc.GetNextCharPos(pos);
						nextchar = doc.GetChar(pos);
					cxENDLOCK

					// Check if we are deleting over a fold
					unsigned int fold_end;
					if (IsPosInFold(nextpos, NULL, &fold_end)) {
						Delete(pos, fold_end); // delete fold
						break;
					}

					// Check if we are at a soft tabpoint
					const unsigned int tabWidth = m_parentFrame.GetTabWidth();
					if (nextchar == wxT(' ') && m_lines.IsAtTabPoint()
						&& pos + tabWidth <= m_lines.GetLength() && IsSpaces(pos, pos + tabWidth)) {

						Delete(pos, pos + tabWidth);
					}
					else {
						// Just delete the char
						unsigned int byte_len;
						cxLOCKDOC_WRITE(m_doc)
							byte_len = doc.DeleteChar(pos);
						cxENDLOCK
						unsigned int del_end = pos + byte_len;
						m_lines.Delete(pos, del_end);
						StylersDelete(pos, del_end);
						MarkAsModified();
					}

					lastpos = pos;
					lastaction = ACTION_DELETE;
				}
				break;

			case WXK_BACK:
				{
					pos = m_lines.GetPos();

					// Check if we should delete entire word
					if (event.ControlDown() && !m_lines.IsSelected()) {
						const interval iv = GetWordIv(pos);

						if (pos <= iv.end && pos > iv.start) {
							Delete(iv.start, pos);
							break;
						}
					}

					// Check if we delete outside zero-width selection
					if (m_lines.IsSelected()) {
						const vector<interval>& sels = m_lines.GetSelections();
						if (sels.size() == 1 && sels[0].start == sels[0].end) {
							m_lines.RemoveAllSelections();
						}
					}

					// Handle deletions in snippet
					if (m_snippetHandler.IsActive()) {
						if (m_lines.IsSelected()) DeleteSelections();
						else if (pos > 0) {
							unsigned int prevcharpos;
							cxLOCKDOC_READ(m_doc)
								prevcharpos = doc.GetPrevCharPos(pos);
							cxENDLOCK

							m_snippetHandler.Delete(prevcharpos, pos);
						}
						break;
					}

					if (pos != lastpos || lastaction != ACTION_DELETE) {
						cxLOCKDOC_WRITE(m_doc)
							doc.Freeze();
						cxENDLOCK
					}

					if (m_lines.IsSelected()) {
						if (m_lines.IsSelectionShadow()) {
							if (DeleteInShadow(pos, false)) break; // if false we have to del outside shadow
						}
						else {
							DeleteSelections();
							cxLOCKDOC_WRITE(m_doc)
								doc.Freeze();
							cxENDLOCK
							break;
						}

						m_lines.RemoveAllSelections();
					}

					if (pos == 0) return; // Can't delete at start of text


					unsigned int prevpos;
					wxChar prevchar;
					cxLOCKDOC_READ(m_doc)
						prevpos = doc.GetPrevCharPos(pos);
						prevchar = doc.GetChar(prevpos);
					cxENDLOCK
					const unsigned int tabWidth = m_parentFrame.GetTabWidth();
					unsigned int newpos;

					// Check if we are at a soft tabpoint
					if (prevchar == wxT(' ') && m_lines.IsAtTabPoint()
						&& pos >= tabWidth && IsSpaces(pos - tabWidth, pos)) {

						newpos = pos - tabWidth;
						RawDelete(newpos, pos);
					}
					else {
						// Else just delete the char
						RawDelete(prevpos, pos);
						newpos = prevpos;
					}

					m_lines.SetPos(newpos);

					lastpos = newpos;
					lastaction = ACTION_DELETE;
					MarkAsModified();
				}
				break;

			case WXK_RETURN:
			case WXK_NUMPAD_ENTER:
				if (event.ShiftDown()) {
					// Dismiss selections/snippets
					if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
					RemoveAllSelections();

					// Move caret to end-of-line
					m_lines.SetPos(m_lines.GetLineEndpos(m_lines.GetCurrentLine(), true));
				}

				InsertChar(wxChar('\n'));
				break;

			case WXK_TAB:
			case WXK_NUMPAD_TAB:
				Tab();
				break;

			case WXK_ESCAPE:
				if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
				else if (m_lines.IsSelectionShadow()) m_lines.RemoveAllSelections();
				else {
					DoCompletion();
				}
				break;

			default:
				// If we process alt then menu shortcuts don't work
				if (event.AltDown()) {
					event.Skip();
					return; // do nothing if we don't know the char
				}

				// Ignore unhandled keycodes
				if (key >= WXK_START && key <= WXK_COMMAND) break;
				//if (key >= WXK_SPECIAL1 && key <= WXK_SPECIAL20) break;

				//if (wxIsprint(c)) { // Normal chars (does not work with זרו??)
				if ((unsigned int)c > 31) { // Normal chars
					InsertChar(c);
				}
				else {
					event.Skip();
					return; // do nothing if we don't know the char
				}
			}
		}
	}

	MakeCaretVisible();

	// Draw the updated view
	DrawLayout();

	// The act of drawing can change the view dimensions by
	// adding/removing scrollbars. So we just check if the
	// caret has been pushed out of the view.
	if (MakeCaretVisible()) DrawLayout();
}

void EditorCtrl::DoCommand(int c) {
	wxLogDebug(wxT("DoCommand %d"), c);
	bool command_active = false;
	if (commandStack.empty()) {
		if (c == 26) { // ctrl-z
			commandStack.push_back(c);
			command_active = cmd_Undo(0, commandStack);
		}
	}
	else {
		if (commandStack[0] == 26) {
			commandStack.push_back(c);
			command_active = cmd_Undo(0, commandStack);
		}
	}

	if (!command_active) commandStack.clear();
}

void EditorCtrl::EndCommand() {
	if (!commandStack.empty()) {
		if (commandStack[0] == 26) cmd_Undo(0, commandStack, true);

		commandStack.clear();
	}
	commandMode = false;
}

void EditorCtrl::DoUndo() {
	cxLOCKDOC_READ(m_doc)
		if (!doc.IsDraft()) return; // no undo history

		const doc_id di = doc.GetDraftParent();
		if (!di.IsOk()) return; // We are in top document (has no parent)

		SetDocument(di);
	cxENDLOCK
}

void EditorCtrl::Redo() {
	const doc_id currentDoc = GetDocID();

	vector<int> childlist;
	cxLOCKDOC_READ(m_doc)
		doc.GetVersionChildren(currentDoc.version_id, childlist);
	cxENDLOCK

	if (childlist.size() == 1) {
		const doc_id di(DRAFT, currentDoc.document_id, childlist[0]);
		SetDocument(di);
	}
	else if (childlist.size() > 1) {
		// Show dialog with undo history so user can choose branch
		RedoDlg dlg(this, m_catalyst, GetId(), currentDoc);
	}
}

bool EditorCtrl::cmd_Undo(int WXUNUSED(count), vector<int>& cStack, bool end) {
	wxASSERT(!cStack.empty());

	cxLOCKDOC_WRITE(m_doc)
		if (!doc.IsDraft()) return false; // no undo history
		if (end) return false;

		int action = cStack.back();

		int currentVersion = doc.GetVersionID();

		if(action == WXK_UP || action == 26) {
			if (currentVersion) {
				const doc_id di = doc.GetDraftParent();
				if (!di.IsOk()) return true; // We are in top document (has no parent)

				SetDocument(di);
			}
		}
		else if (action == WXK_DOWN) {
			vector<int> childlist;
			doc.GetVersionChildren(currentVersion, childlist);
			if (!childlist.empty()) {
				const doc_id di(DRAFT, doc.GetDocumentID(), childlist[0]);
				SetDocument(di);
			}
		}
		else if (action == WXK_LEFT) {
			if (currentVersion) {
				vector<int> childlist;
				doc.GetVersionChildren(doc.GetDraftParent(currentVersion).version_id, childlist);
				vector<int>::const_iterator idx = find(childlist.begin(), childlist.end(), currentVersion);
				if (idx > childlist.begin()) {
					const doc_id di(DRAFT, doc.GetDocumentID(), *(idx-1));
					SetDocument(di);
				}
			}
		}
		else if (action == WXK_RIGHT) {
			if (currentVersion) {
				vector<int> childlist;
				doc.GetVersionChildren(doc.GetDraftParent(currentVersion).version_id, childlist);
				vector<int>::const_iterator idx = find(childlist.begin(), childlist.end(), currentVersion);
				if (idx < childlist.end()-1) {
					const doc_id di(DRAFT, doc.GetDocumentID(), *(idx+1));
					SetDocument(di);
				}
			}
		}
	cxENDLOCK

	if (end) return false;
	else return true;
}

void EditorCtrl::SetPos(unsigned int pos) {
	wxASSERT(pos >= 0 && pos <= m_lines.GetLength());
	m_lines.SetPos(pos);
}

void EditorCtrl::SetPos(int line, int column) {
	// line & colums indexes starts from 1
	if (line > 0) {
		--line;
		if (line < (int)m_lines.GetLineCount()) {
			m_lines.SetPos(m_lines.GetLineStartpos(line));
		}
	}
	if (column > 0) {
		--column;
		const unsigned int pos = m_lines.GetPos();
		const unsigned int curLine = m_lines.GetLineFromCharPos(pos);
		const unsigned int lineEnd = m_lines.GetLineEndpos(curLine);
		unsigned int newpos;
		cxLOCKDOC_READ(m_doc)
			newpos = doc.GetValidCharPos(pos + column);
		cxENDLOCK
		m_lines.SetPos(wxMin(newpos, lineEnd));
	}
}

void EditorCtrl::PageUp(bool select, int WXUNUSED(count)) {
	const wxSize size = GetClientSize();
	const int oldpos = m_lines.GetPos();
	const wxPoint cpos = m_lines.GetCaretPos();

	m_lines.ClickOnLine(cpos.x, wxMax(0, cpos.y - size.y));

	// Scroll the view up a page as well. If this is not enough to
	// show the caret (might have been outside the view when PageDown was
	// pressed), then it will be brought into view by MakeSelectionVisible()
	scrollPos -= size.y;
	if (scrollPos < 0) scrollPos = 0;

	// Handle selection
	if (!select) m_lines.RemoveAllSelections();
	else SelectFromMovement(oldpos, m_lines.GetPos());
}

void EditorCtrl::PageDown(bool select, int WXUNUSED(count)) {
	const wxSize size = GetClientSize();
	const int oldpos = m_lines.GetPos();
	const wxPoint cpos = m_lines.GetCaretPos();

	m_lines.ClickOnLine(cpos.x, cpos.y + size.y + 1);

	// Scroll the view down a page as well. If this is not enough to
	// show the caret (might have been outside the view when PageDown was
	// pressed), then it will be brought into view by MakeSelectionVisible()
	scrollPos += size.y;
	if (scrollPos > m_lines.GetHeight() - size.y) scrollPos = wxMax(0, m_lines.GetHeight() - size.y);

	// Handle selection
	if (!select) m_lines.RemoveAllSelections();
	else SelectFromMovement(oldpos, m_lines.GetPos());
}

void EditorCtrl::CursorUp(bool select) {
	const int oldpos = m_lines.GetPos();

	if (lastaction == ACTION_UP) m_lines.MovePosUp(lastxpos);
	else {
		const wxPoint cpos = m_lines.GetCaretPos();
		lastxpos = cpos.x;
		m_lines.MovePosUp();
	}

	// Handle selection
	if (!select) m_lines.RemoveAllSelections(true, m_lines.GetPos());
	else SelectFromMovement(oldpos, m_lines.GetPos());

	lastaction = ACTION_UP;
}

void EditorCtrl::CursorDown(bool select) {
	const int oldpos = m_lines.GetPos();

	if (lastaction == ACTION_DOWN) m_lines.MovePosDown(lastxpos);
	else {
		const wxPoint cpos = m_lines.GetCaretPos();
		lastxpos = cpos.x;
		m_lines.MovePosDown();
	}

	// Handle selection
	if (!select) m_lines.RemoveAllSelections(true, m_lines.GetPos());
	else SelectFromMovement(oldpos, m_lines.GetPos());

	lastaction = ACTION_DOWN;
}

void EditorCtrl::CursorLeft(bool select) {
	const unsigned int pos = m_lines.GetPos();
	if (pos > 0) {
		unsigned int prevpos;
		wxChar prevchar;
		cxLOCKDOC_READ(m_doc)
			prevpos = doc.GetPrevCharPos(pos);
			prevchar = doc.GetChar(prevpos);
		cxENDLOCK

		// Check if we are at a soft tabpoint
		if (prevchar == wxT(' ') && m_lines.IsAtTabPoint()) {
			const unsigned int tabWidth = m_parentFrame.GetTabWidth();
			if (pos >= tabWidth && IsSpaces(pos - tabWidth, pos)) {
				prevpos = pos - tabWidth;
			}
		}

		// Check if we are moving over a fold
		unsigned int fold_start;
		if (IsPosInFold(prevpos, &fold_start)) {
			prevpos = fold_start;
		}

		m_lines.SetPos(prevpos); // Caret will be moved in DrawLayout()

		// Handle selection
		if (select) SelectFromMovement(pos, prevpos);
		else m_lines.RemoveAllSelections(true, prevpos);
	}
	lastaction = ACTION_NONE;
}

void EditorCtrl::CursorRight(bool select) {
	const unsigned int pos = m_lines.GetPos();
	if (pos < m_lines.GetLength()) {
		unsigned int nextpos;
		wxChar nextchar;
		cxLOCKDOC_READ(m_doc)
			nextpos = doc.GetNextCharPos(pos);
			nextchar = doc.GetChar(pos);
		cxENDLOCK

		// Check if we are at a soft tabpoint
		if (nextchar == wxT(' ') && m_lines.IsAtTabPoint()) {
			const unsigned int tabWidth = m_parentFrame.GetTabWidth();
			if (pos + tabWidth <= m_lines.GetLength() && IsSpaces(pos, pos + tabWidth)) {
				nextpos = pos + tabWidth;
			}
		}

		// Check if we are moving over a fold
		unsigned int fold_end;
		if (IsPosInFold(nextpos, NULL, &fold_end)) {
			if (fold_end == GetLength()) return;
			nextpos = fold_end;
		}

		m_lines.SetPos(nextpos); // Caret will be moved in DrawLayout()

		// Handle selection
		if (!select) m_lines.RemoveAllSelections(true, nextpos);
		else SelectFromMovement(pos, nextpos);
	}
	lastaction = ACTION_NONE;
}

void EditorCtrl::CursorWordLeft(bool select) {
	unsigned int pos = m_lines.GetPos();
	const unsigned int oldpos = pos;

	/* State machine:
		   | a s p  (* marks SetPos)
		---+-------
		 a | a * *
		 s | a s p
		 p | * * p
	*/
	if (pos > 0) {
		cxLOCKDOC_READ(m_doc)
			pos = doc.GetPrevCharPos(pos);
			if (pos) {
				wxChar prev_char = doc.GetChar(pos);
				for (pos = doc.GetPrevCharPos(pos); pos > 0; pos = doc.GetPrevCharPos(pos)) {
					wxChar c = doc.GetChar(pos);
					if (c == wxT('\n')) {
						pos = doc.GetNextCharPos(pos);
						break; // don't extend beyond start-of-line
					}
					else if (Isalnum(prev_char) && c != wxT('_') && (wxIsspace(c) || wxIspunct(c))) {
						pos = doc.GetNextCharPos(pos);
						break;
					}
					else if (wxIspunct(prev_char) && prev_char != wxT('_') && (Isalnum(c) || wxIsspace(c))) {
						pos = doc.GetNextCharPos(pos);
						break;
					}
					prev_char = c;
				}
			}
		cxENDLOCK

		// Check if we are moving over a fold
		unsigned int fold_start;
		if (IsPosInFold(pos, &fold_start)) {
			pos = fold_start;
		}

		m_lines.SetPos(pos);

		// Handle selection
		if (!select) m_lines.RemoveAllSelections(true, pos);
		else if (oldpos != pos) SelectFromMovement(oldpos, pos);
	}
}

void EditorCtrl::CursorWordRight(bool select) {
	unsigned int pos = m_lines.GetPos();
	const unsigned int oldpos = pos;

	/* State machine:
		   | a s p  (* marks SetPos)
		---+-------
		 a | a s *
		 s | * s *
		 p | * s p
	*/
	cxLOCKDOC_READ(m_doc)
		if (pos < doc.GetLength()) {
			wxChar prev_char = doc.GetChar(pos);
			for (pos = doc.GetNextCharPos(pos); pos < doc.GetLength(); pos = doc.GetNextCharPos(pos)) {
				const wxChar c = doc.GetChar(pos);
				if (c == wxT('\n') && pos > oldpos) break; // don't extend beyond eol
				if (Isalnum(c) && prev_char != wxT('_') && (wxIsspace(prev_char) || wxIspunct(prev_char))) break;
				else if (wxIspunct(c) && c != wxT('_') && (Isalnum(prev_char) || wxIsspace(prev_char))) break;
				prev_char = c;
			}
		}
	cxENDLOCK

	// Check if we are moving over a fold
	unsigned int fold_end;
	if (IsPosInFold(pos, NULL, &fold_end)) {
		pos = fold_end;
	}

	m_lines.SetPos(pos);

	// Handle selection
	if (!select) m_lines.RemoveAllSelections(true, pos);
	else if (oldpos != pos) {
		SelectFromMovement(oldpos, pos);
	}
}

void EditorCtrl::SelectFromMovement(unsigned int oldpos, unsigned int newpos, bool makeVisible) {
	wxASSERT(oldpos >= 0 && oldpos <= m_lines.GetLength());
	wxASSERT(newpos >= 0 && newpos <= m_lines.GetLength());
	if (oldpos == newpos) return; // Empty selection

	// If alt key is down do a block selection
	if (!wxGetKeyState(WXK_ALT)) m_blockKeyState = BLOCKKEY_NONE; // may not have catched keyup
	if (m_blockKeyState != BLOCKKEY_NONE) {
		if (m_blockKeyState == BLOCKKEY_INIT) {
			// Get start location
			const wxPoint spos = m_lines.GetCharPos(oldpos);
			m_sel_startpoint.x = spos.x;
			m_sel_startpoint.y = spos.y;
			m_blockKeyState = BLOCKKEY_DOWN;
			m_lines.RemoveAllSelections();
		}
		// Block select to new caret location
		const wxPoint epos = m_lines.GetCharPos(newpos);
		SelectBlock(epos, false);

		return;
	}

	int sel = -1;

	if (m_lines.IsSelected()) {
		// Get the selections
		const vector<interval>& selections = m_lines.GetSelections();

		// Check if we are at the start or end of a selection
		for (unsigned int i = 0; i < selections.size(); ++i) {
			if (selections[i].end == oldpos) {
				sel = m_lines.UpdateSelection(i, selections[i].start, newpos);
				if (sel != -1) MakeSelectionVisible();
				return;
			}
			else if (selections[i].start == oldpos) {
				sel = m_lines.UpdateSelection(i, newpos, selections[i].end);
				if (sel != -1) MakeSelectionVisible();
				return;
			}
		}

		// We were not in connection with any current selections
		m_lines.RemoveAllSelections();
		sel = m_lines.AddSelection(oldpos, newpos);
	}
	else sel = m_lines.AddSelection(oldpos, newpos);

	if (makeVisible && sel != -1) MakeSelectionVisible();
}

bool EditorCtrl::IsCaretVisible() {
	const wxSize caretsize = caret->GetSize();
	const wxPoint cpos = m_lines.GetCaretPos();
	const wxSize clientsize = GetClientSize();

	// Check vertically
	if (cpos.y + caretsize.y < scrollPos) return false;
	else {
		if (cpos.y >= scrollPos + clientsize.y) return false;
	}

	// Check horizontally
	if (cpos.x + caretsize.x < m_scrollPosX) return false;
	else {
		const int sizeX = ClientWidthToEditor(clientsize.x);

		if (cpos.x >= m_scrollPosX + sizeX) return false;
	}

	return true;
}

bool EditorCtrl::MakeCaretVisible() {
	const wxPoint cpos = m_lines.GetCaretPos();
	const wxSize clientsize = GetClientSize();

	// Check if the caret have moved outside visible display vertically
	if (cpos.y < scrollPos) {
		scrollPos = cpos.y;
		return true;
	}
	else {
		const int lineheight = m_lines.GetLineHeight();
		if (cpos.y + lineheight > scrollPos + clientsize.y) {
			scrollPos = (cpos.y + lineheight) - clientsize.y;
			return true;
		}
	}

	// Check if the caret have moved outside visible display horizontally
	if (cpos.x < m_scrollPosX) {
		m_scrollPosX = wxMax(cpos.x - 50, 0);
		return true;
	}
	else {
		const int sizeX = ClientWidthToEditor(clientsize.x);

		if (cpos.x >= m_scrollPosX + sizeX) {
			const int textWidth = m_lines.GetWidth();
			const int maxScrollPos = (textWidth < sizeX) ? 0 : textWidth + m_caretWidth - sizeX; // room for caret at end
			m_scrollPosX = (cpos.x + 50) - sizeX;
			if (m_scrollPosX > maxScrollPos) m_scrollPosX = maxScrollPos;
			return true;
		}
	}

	return false; // no movement
	// NOTE: Will first be visible on next redraw
}

void EditorCtrl::MakeCaretVisibleCenter() {
	const wxPoint cpos = m_lines.GetCaretPos();
	const wxSize clientsize = GetClientSize();

	scrollPos = cpos.y - (clientsize.y / 2);
	if (scrollPos < 0) scrollPos = 0;

	// Go to start of line
	m_scrollPosX = 0;
}

void EditorCtrl::MakeSelectionVisible(unsigned int sel_id) {
	// Get the selection
	const vector<interval>& selections = m_lines.GetSelections();
	wxASSERT(sel_id >= 0 && sel_id < selections.size());
	const interval iv = selections[sel_id];

	const wxPoint sel_start = m_lines.GetCharPos(iv.start);
	const wxPoint sel_end = m_lines.GetCharPos(iv.end);
	const wxSize clientsize = GetClientSize();

	// Vertically
	if (sel_start.y < scrollPos) {
		scrollPos = sel_start.y;
	}
	else {
		const int lineheight = m_lines.GetLineHeight();
		if (sel_end.y + lineheight >= scrollPos + clientsize.y) {
			// Make sure that we can see as much of the selection as possible
			scrollPos = wxMin(sel_start.y, (sel_end.y + lineheight) - clientsize.y);
		}
	}

	const int sizeX = ClientWidthToEditor(clientsize.x);

	// Horizontally for End
	if (sel_end.x < m_scrollPosX) {
		m_scrollPosX = wxMax(sel_end.x - 50, 0);
	}
	else if (sel_end.x >= m_scrollPosX + sizeX) {
		m_scrollPosX = (sel_end.x + 50) - sizeX;
	}

	// Horizontally for Start
	if (sel_start.x < m_scrollPosX) {
		m_scrollPosX = wxMax(sel_start.x - 50, 0);
	}
	else if (sel_start.x >= m_scrollPosX + sizeX) {
		m_scrollPosX = (sel_start.x + 50) - sizeX;
	}

	// NOTE: Will first be visible on next redraw
}

wxString EditorCtrl::GetSelFirstLine() {
	// returns the first line of the selection (if there is *one* selection)

	if (m_lines.IsSelected()) {
		const vector<interval>& selections = m_lines.GetSelections();
		if (selections.size() != 1) return wxT("");

		// Get text
		const unsigned int start_pos = selections[0].start;
		const unsigned int line_id = m_lines.GetLineFromCharPos(start_pos);
		const unsigned int line_end = m_lines.GetLineEndpos(line_id);
		cxLOCKDOC_READ(m_doc)
			return doc.GetTextPart(start_pos, wxMin(selections[0].end, line_end));
		cxENDLOCK
	}
	else return wxT("");
}

wxString EditorCtrl::GetFirstSelection() const {
	if (m_lines.IsSelected()) {
		const interval& sel = m_lines.GetSelections()[0];

		// Get text
		cxLOCKDOC_READ(m_doc)
			return doc.GetTextPart(sel.start, sel.end);
		cxENDLOCK
	}
	else return wxT("");
}

wxString EditorCtrl::GetSelText() const {
	if (m_lines.IsSelected()) {
		wxString text;
		const vector<interval>& selections = m_lines.GetSelections();

		cxLOCKDOC_READ(m_doc)
			// Get the selected text
			for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
				if (iv > selections.begin()) text += wxT('\n'); // Add newline between multiple selections

				text += doc.GetTextPart((*iv).start, (*iv).end);
			}
		cxENDLOCK

		return text;
	}
	else return wxT("");
}


void editor_ctrl_configure_system_env(cxEnv& env, bool isUnix) {
	const wxString baseAppPath = ((eApp*)wxTheApp)->GetAppPath();

	// TM_SUPPORT_PATH
	wxFileName supportPath = baseAppPath;
	supportPath.AppendDir(wxT("Support"));
	const bool supportPathExists = supportPath.DirExists();
	if (supportPathExists) {
		const wxString tmSupportPath = isUnix ? eDocumentPath::WinPathToCygwin(supportPath) : supportPath.GetPath();
		env.SetEnv(wxT("TM_SUPPORT_PATH"), tmSupportPath);

		// TM_BASH_INIT
		if (isUnix) {
			wxFileName bashInit = supportPath;
			bashInit.AppendDir(wxT("lib"));
			bashInit.SetFullName(wxT("cygwin_bash_init.sh"));
			if (bashInit.FileExists()) {
				wxString s_tmBashInit = eDocumentPath::WinPathToCygwin(bashInit);
				env.SetEnv(wxT("TM_BASH_INIT"), s_tmBashInit);
			}
		}
	}

	// PATH
	wxString envPath;
	if (wxGetEnv(wxT("PATH"), &envPath)) {
#ifdef __WXMSW__
		// Check if cygwin is on the path
		if (!eDocumentPath::s_cygPath.empty()) {
			if (!envPath.Contains(eDocumentPath::s_cygPath)) {
				const wxString binPath = eDocumentPath::s_cygPath + wxT("\\bin");
				const wxString x11Path = eDocumentPath::s_cygPath + wxT("\\usr\\X11R6\\bin");

				if (!envPath.empty()) {
					envPath = binPath + wxT(";") + x11Path + wxT(";") + envPath;
				}
			}
		}
#endif // __WXMSW__

		// Add TM_SUPPORT_PATH/bin to the PATH
		if (supportPathExists) {
			wxFileName supportBinPath = supportPath;
			supportBinPath.AppendDir(wxT("bin"));
			if (supportBinPath.DirExists()) {
				envPath = supportBinPath.GetPath() + wxT(";") + envPath;
			}
		}

		env.SetEnv(wxT("PATH"), envPath);
	}

	// TM_APPPATH
	wxString appPath = baseAppPath;
	if (isUnix) appPath = eDocumentPath::WinPathToCygwin(appPath);
	env.SetEnv(wxT("TM_APPPATH"), appPath);

	// TM_FULLNAME
	env.SetEnv(wxT("TM_FULLNAME"), wxGetUserName());
}

void EditorCtrl::SetEnv(cxEnv& env, bool isUnix, const tmBundle* bundle) {
#ifdef __WXMSW__
	if (isUnix) ((eApp*)wxTheApp)->InitCygwin(true);
#endif // __WXMSW__

	// Load current env (app)
	env.SetToCurrent();

	editor_ctrl_configure_system_env(env, isUnix);

	// TM_FILENAME
	// note: in case of remote files, this is of the buffer file
	const wxString name = m_path.GetFullName();
	if (!name.empty()) env.SetEnv(wxT("TM_FILENAME"), name);

	// TM_FILEPATH & TM_DIRECTORY
	// note: in case of remote files, this is of the buffer file
	if (m_path.IsOk()) {
		if (m_tmFilePath.empty()) {
			if (isUnix) {
				m_tmFilePath = eDocumentPath::WinPathToCygwin(m_path);
				wxFileName dir(m_path.GetPath(), wxEmptyString);
				m_tmDirectory = eDocumentPath::WinPathToCygwin(dir);
			}
			else {
				m_tmFilePath = m_path.GetFullPath();
				m_tmDirectory = m_path.GetPath();
			}
		}

		env.SetEnv(wxT("TM_FILEPATH"), m_tmFilePath);
		env.SetEnv(wxT("TM_DIRECTORY"), m_tmDirectory);
	}

	// TM_SELECTED_TEXT
	if (IsSelected()) {
		env.SetEnv(wxT("TM_SELECTED_TEXT"), GetFirstSelection());
	}

	// TM_CURRENT_WORD
	const wxString word = GetCurrentWord();
	if (!word.empty()) env.SetEnv(wxT("TM_CURRENT_WORD"), word);

	// TM_CURRENT_LINE
	const wxString line = GetCurrentLine();
	if (!line.empty()) env.SetEnv(wxT("TM_CURRENT_LINE"), line);

	const unsigned int lineNum = GetCurrentLineNumber();
	const unsigned int columnIndex = GetCurrentColumnNumber();

	// TM_LINE_NUMBER
	env.SetEnv(wxT("TM_LINE_NUMBER"), wxString::Format(wxT("%u"), lineNum));

	// TM_LINE_INDEX
	// (has to be counted in bytes as scripts get text in utf8)
	const unsigned int linestart = m_lines.GetLineStartpos(m_lines.GetCurrentLine());
	const unsigned int line_index = m_lines.GetPos() - linestart;
	env.SetEnv(wxT("TM_LINE_INDEX"), wxString::Format(wxT("%u"), line_index));

	// TM_COLUMN_NUMBER
	env.SetEnv(wxT("TM_COLUMN_NUMBER"), wxString::Format(wxT("%u"), columnIndex));

	// TM_TAB_SIZE
	const wxString tabsize = wxString::Format(wxT("%u"), m_parentFrame.GetTabWidth());
	env.SetEnv(wxT("TM_TAB_SIZE"), tabsize);

	// TM_SOFT_TABS
	if (m_parentFrame.IsSoftTabs()) env.SetEnv(wxT("TM_SOFT_TABS"), wxT("YES"));
	else env.SetEnv(wxT("TM_SOFT_TABS"), wxT("NO"));

	// TM_SCOPE
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(GetPos());
	wxString scopeStr;
	for (unsigned int i = 0; i < scope.size(); ++i) {
		if (i) scopeStr += wxT('\n');
		scopeStr += *scope[i];
	}
	env.SetEnv(wxT("TM_SCOPE"), scopeStr);

	// TM_MODE
	const wxString& syntaxName = m_syntaxstyler.GetName();
	if (!syntaxName.empty()) env.SetEnv(wxT("TM_MODE"), syntaxName);

	// TM_PROJECT_DIRECTORY
	if (m_parentFrame.HasProject() && !m_parentFrame.IsProjectRemote()) {
		const wxFileName& prjPath = m_parentFrame.GetProject();
		if (isUnix) env.SetEnv(wxT("TM_PROJECT_DIRECTORY"), eDocumentPath::WinPathToCygwin(prjPath));
		else env.SetEnv(wxT("TM_PROJECT_DIRECTORY"), prjPath.GetPath());

		// Set project specific env vars
		env.SetEnv(m_parentFrame.GetProjectEnv());
	}

	// TM_SELECTED_FILE & TM_SELECTED_FILES
	const wxArrayString selections = m_parentFrame.GetSelectionsInProject();
	if (!selections.IsEmpty()) {
		if (isUnix) env.SetEnv(wxT("TM_SELECTED_FILE"), eDocumentPath::WinPathToCygwin(selections[0]));
		else env.SetEnv(wxT("TM_SELECTED_FILE"), selections[0]);

		wxString sels;
		for (unsigned int i = 0; i < selections.GetCount(); ++i) {
			if (i) sels += wxT(" '");
			else sels += wxT('\'');
			if (isUnix) sels += eDocumentPath::WinPathToCygwin(selections[i]);
			else sels += selections[0];
			sels += wxT('\'');
		}
		env.SetEnv(wxT("TM_SELECTED_FILES"), sels);
	}

	// TM_CARET_XPOS & TM_CARET_YPOS
	if (IsCaretVisible()) {
		const wxPoint caretPos = GetCaretPoint();
		const wxPoint screenPos = ClientToScreen(caretPos);
		env.SetEnv(wxT("TM_CARET_XPOS"), wxString::Format(wxT("%d"), screenPos.x));
		env.SetEnv(wxT("TM_CARET_YPOS"), wxString::Format(wxT("%d"), screenPos.y + m_lines.GetLineHeight()));
	}

	// Set bundle specific env
	if (bundle) {
		if (isUnix) env.SetEnv(wxT("TM_BUNDLE_PATH"), eDocumentPath::WinPathToCygwin(bundle->path));
		else env.SetEnv(wxT("TM_BUNDLE_PATH"), bundle->path.GetPath());

		const wxFileName bsupportPath = m_syntaxHandler.GetBundleSupportPath(bundle->bundleRef);
		if (bsupportPath.IsOk()) {
			if (isUnix) env.SetEnv(wxT("TM_BUNDLE_SUPPORT"), eDocumentPath::WinPathToCygwin(bsupportPath));
			else env.SetEnv(wxT("TM_BUNDLE_SUPPORT"), bsupportPath.GetPath());
		}
	}
}

//
// Runs the given command in an appropriate shell, returning stdout, stderr and the result code.
// If an internal error occurs, such as invalid inputs to this fuction, -1 is returned.
//
long EditorCtrl::RawShell(const vector<char>& command, const vector<char>& input, vector<char>* output, vector<char>* errorOut, cxEnv& env, bool isUnix, const wxString& cwd) {
	if (command.empty()) return -1;

#ifdef __WXMSW__
	if (isUnix && !((eApp*)wxTheApp)->InitCygwin()) return -1;
#endif // __WXMSW__

	// Create temp file with command
	wxFileName tmpfilePath = ((eApp*)wxTheApp)->GetAppDataPath();
	tmpfilePath.SetFullName(isUnix ? wxT("tmcmd") : wxT("tmcmd.bat"));
	wxFile tmpfile(tmpfilePath.GetFullPath(), wxFile::write);
	if (!tmpfile.IsOpened()) return -1;

	tmpfile.Write(&command[0], command.size());
	tmpfile.Close();

	wxString execCmd;

	// Look for shebang
	if (command.size() > 2 && command[0] == '#' && command[1] == '!') {
		// Ignore leading whitespace
		unsigned int i = 2;
		for (; i < command.size() && isspace(command[i]); ++i);

		// Get the interpreter path
		unsigned int start = i;
		for (; i < command.size() && !isspace(command[i]); ++i);
		wxString cmd(&command[start], wxConvUTF8, i - start);

		// Rest of line is argument for interpreter
		for (start = i; i < command.size() && command[i] != '\n'; ++i);
		wxString args(&command[start], wxConvUTF8, i - start);

#ifdef __WXMSW__
		// Convert possible unix path to windows
		const wxString newpath = eDocumentPath::CygwinPathToWin(cmd);
		if (newpath.empty()) return -1;
		execCmd = newpath + args;
#else
		execCmd = cmd + args;
#endif // __WXMSW__
		execCmd += wxT(" \"") + tmpfilePath.GetFullPath() + wxT("\"");
	}
	else if (isUnix) {
		if (s_bashCmd.empty()) {
#ifdef __WXMSW__
			s_bashCmd = eDocumentPath::s_cygPath + wxT("\\bin\\bash.exe \"") + tmpfilePath.GetFullPath() + wxT("\"");
#else
            s_bashCmd = wxT("bash \"") + tmpfilePath.GetFullPath() + wxT("\"");
#endif

			wxFileName initPath = ((eApp*)wxTheApp)->GetAppPath();
			initPath.AppendDir(wxT("Support"));
			initPath.AppendDir(wxT("lib"));
			initPath.SetFullName(wxT("bash_init.sh"));
			if (initPath.FileExists()) {
				s_bashEnv = initPath.GetFullPath();
			}
		}

		env.SetEnv(wxT("BASH_ENV"), s_bashEnv);
		execCmd = s_bashCmd;
	}
#ifdef __WXMSW__
	else {
		// Windows native runs as bat files
		// (needs double quotes for path)
		execCmd = wxT("cmd /C \"\"") + tmpfilePath.GetFullPath() + wxT("\"\"");
	}
#endif // __WXMSW__

	// Get ready for execution
	cxExecute exec(env, cwd);
	bool debugOutput = false; // default setting
	((eApp*)wxTheApp)->GetSettingBool(wxT("bundleDebug"), debugOutput);
	exec.SetDebugLogging(debugOutput);

	// Exec the command
	wxLogDebug(wxT("Running command: %s"), execCmd.c_str());
	int resultCode = exec.Execute(execCmd, input);

	// Get the output
	if (resultCode != -1) {
		if (output) output->swap(exec.GetOutput());
		if (errorOut) errorOut->swap(exec.GetErrorOut());
	}

	return resultCode;
}

wxString EditorCtrl::GetBashCommand(const wxString& cmd, cxEnv& env) {
#ifdef __WXMSW__
	if (!((eApp*)wxTheApp)->InitCygwin()) return wxEmptyString;
#endif

	if (s_bashEnv.empty()) {
		wxFileName initPath = ((eApp*)wxTheApp)->GetAppPath();
		initPath.AppendDir(wxT("Support"));
		initPath.AppendDir(wxT("lib"));
		initPath.SetFullName(wxT("bash_init.sh"));
		if (initPath.FileExists()) {
			s_bashEnv = initPath.GetFullPath();
		}
	}
	env.SetEnv(wxT("BASH_ENV"), s_bashEnv);

#ifdef __WXMSW__
	return eDocumentPath::s_cygPath + wxT("\\bin\\bash.exe -c \"") + cmd + wxT("\"");
#else
    return wxT("bash -c \"") + cmd + wxT("\"");
#endif
}

wxString EditorCtrl::RunShellCommand(const vector<char>& command, bool doSetEnv) {
	if (command.empty()) return wxEmptyString;

	cxEnv env;
	if (doSetEnv) SetEnv(env);

	// Set a busy cursor
	// will be reset when leaving scope
	wxBusyCursor wait;

	// Run the command
	vector<char> input;
	vector<char> output;
	const int resultCode = RawShell(command, input, &output, NULL, env);
    if ( resultCode == -1) return wxEmptyString; // exec failed

	wxString outputStr;
	if (!output.empty()) outputStr += wxString(&*output.begin(), wxConvUTF8, output.size());
#ifdef __WXMSW__
	// Do newline conversion for Windows only.
	outputStr.Replace(wxT("\r\n"), wxT("\n"));
#endif // __WXMSW__

	// Insert output into text
	return outputStr;
}

void EditorCtrl::RunCurrent(bool doReplace) {
	vector<char> command;
	unsigned int start;
	unsigned int end;

	if (m_lines.IsSelected()) {
		// Get first selection
		const interval& sel = m_lines.GetSelections()[0];
		start = sel.start;
		end = sel.end;
	}
	else {
		// Get current line
		const unsigned int cl = m_lines.GetCurrentLine();
		start = m_lines.GetLineStartpos(cl);
		end = m_lines.GetLineEndpos(cl, false);
	}

	if (start < end) {
		const unsigned int cmdLen = end - start;
		command.resize(cmdLen);
		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(start, end, (unsigned char*)&*command.begin());
		cxENDLOCK
	}

	if (command.empty()) return;

	RemoveAllSelections();

	Freeze();
	if (doReplace) {
		RawDelete(start, end);
		end = start;
	}
	SetPos(end);

	const wxString output = RunShellCommand(command);
	if (!output.empty()) {
		// If inserting at last (virtual) line we have to first add a newline
		if (!doReplace && end == GetLength() && end) {
			wxChar prevchar;
			cxLOCKDOC_READ(m_doc)
				const unsigned int prepos = doc.GetPrevCharPos(end);
				prevchar = doc.GetChar(prepos);
			cxENDLOCK
			if (prevchar != wxT('\n')) end += RawInsert(end, wxT("\n"));
		}
		const unsigned int bytelen = RawInsert(end, output);
		SetPos(end + bytelen);
	}
	Freeze();

	MakeCaretVisible();
	DrawLayout();
}

void EditorCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
	// # no evt.skip() as we don't want the control to erase the background
}

void EditorCtrl::OnFocus(wxFocusEvent& WXUNUSED(event)) {
	ClearSearchRange(true);
}

void EditorCtrl::OnMouseLeftDown(wxMouseEvent& event) {
	// Remove tooltips
	/*if (m_revTooltip.IsShown()) {
		m_revTooltip.Hide();
	}*/
	m_lastScopePos = -1; // invalidate scope selections

	// Fix: For some reason we did not always recieve focus?
	// might be related to being on wxPanel in sizer.
	SetFocus();

	lastpos = m_lines.GetPos();
	m_dragStartPos = wxDefaultPosition; // reset drag state

	wxLogDebug(wxT("Clicked: %d,%d"), event.GetX(), event.GetY());

	// Get Mouse location
	const wxPoint mpos = ClientPosToEditor(event.GetX(), event.GetY());
	m_sel_startpoint.x = mpos.x;
	m_sel_startpoint.y = mpos.y;

	// Find out what was clicked on
	const full_pos fp = m_lines.ClickOnLine(mpos.x, mpos.y);
	const unsigned int line_id = m_lines.GetLineFromCharPos(fp.pos);

	// Set selection state
	m_selMode = SEL_NORMAL;
	m_sel_start = m_sel_end = fp.pos;
	lastaction = ACTION_NONE;

	// Check if it is really a triple click
	wxLogDebug(wxT("m_doubleClickTimer = %d"), m_doubleClickTimer.Time());
	if (m_doubleClickedLine == (int)line_id && m_doubleClickTimer.Time() < 250) {
		SelectLine(line_id, event.ControlDown());
	}
	else {
		MakeCaretVisible();

		// Check if we are starting a drag
		bool isDragging = false;
		if (!event.ShiftDown() && !fp.xy_outbound && m_lines.IsSelected() && !m_lines.IsSelectionShadow()) {
			const vector<interval>& sels = m_lines.GetSelections();
			for (vector<interval>::const_iterator p = sels.begin(); p != sels.end(); ++p) {
				if ((int)p->start <= fp.pos && (int)p->end >= fp.pos) {
					isDragging = true;
					if (event.ControlDown() && p->start == p->end) {
						// ctrl-clicking a zero length selection removes it
						m_lines.RemoveSelection(distance(sels.begin(), p));
					}
					else {
						// clicked on selection
						wxLogDebug(wxT("Preparing for text drag"));
						m_dragStartPos = mpos; // start pos in editor coords
					}
					break;
				}
			}
		}

		if (!isDragging) {
			// If not multiselecting remove previous selections
			// Shadow selections are removed if the click is outside
			if (m_lines.IsSelectionShadow() || (!event.ControlDown() && !event.ShiftDown())) {
				if (fp.xy_outbound) m_lines.RemoveAllSelections();
				else m_lines.RemoveAllSelections(true, fp.pos);
				m_currentSel = -1;
				m_blocksel_ids.clear();
			}

			// Check if we should make new selection
			if (event.ShiftDown()) {
				SelectFromMovement(lastpos, fp.pos, false);
			}
			else if (event.ControlDown()) {
				// Make zero length selection
				m_currentSel = m_lines.AddSelection(fp.pos, fp.pos);
			}
		}
	}

	// Reset triple-click detection state
	m_doubleClickedLine = -1;
	m_doubleClickTimer.Pause();

	DrawLayout();

	// Make sure we capure all mouse events
	// this is released in OnMouseLeftUp()
	//wxLogDebug(wxT("EditorCtrl::CaptureMouse() %d"), event.LeftDown());
	CaptureMouse();
}

void EditorCtrl::OnMouseRightDown(wxMouseEvent& event) {
	// Get Mouse location
	const wxPoint mpos = ClientPosToEditor(event.GetX(), event.GetY());

	// If we get it from context key on keyboard the coords are invalid
	const bool fromKey = (mpos.x < 0 || mpos.y < 0);
	const unsigned int pos = fromKey ? GetPos() : m_lines.GetPosFromXY(mpos.x, mpos.y);

	// Check if we are clicking inside a selection
	bool inSelection = false;
	if (m_lines.IsSelected()) {
		const vector<interval>& sels = m_lines.GetSelections();
		for (vector<interval>::const_iterator p = sels.begin(); p != sels.end(); ++p) {
			if (pos >= p->start && pos <= p->end) {
				inSelection = true;
				break;
			}
		}

		if (!inSelection) m_lines.RemoveAllSelections();
	}

	if (fromKey) MakeCaretVisible();
	else if (!inSelection) SetPos(pos);
	DrawLayout();

	wxMenu contextMenu;
	contextMenu.Append(wxID_CUT, _("&Cut\tCtrl+X"), _("Cut"));
	contextMenu.Append(wxID_COPY, _("&Copy\tCtrl+C"), _("Copy"));
	contextMenu.Append(wxID_PASTE, _("&Paste\tCtrl+V"), _("Paste"));
	contextMenu.AppendSeparator();
	wxMenu* selectMenu = new wxMenu;
		selectMenu->Append(MENU_SELECTWORD, _("&Word\tCtrl+Shift+W"), _("Select Word"));
		selectMenu->Append(MENU_SELECTLINE, _("&Line\tCtrl+Shift+L"), _("Select Line"));
		selectMenu->Append(MENU_SELECTSCOPE, _("&Current Scope\tCtrl+Shift+Space"), _("Select Current Scope"));
		selectMenu->Append(MENU_SELECTFOLD, _("Current &Fold\tShift-F1"), _("Select Current Fold"));
		selectMenu->Append(wxID_SELECTALL, _("&All\tCtrl+A"), _("Select All"));
		contextMenu.Append(MENU_SELECT, _("&Select"), selectMenu,  _("Select"));

	if (!inSelection) {
		contextMenu.Enable(wxID_CUT, false);
		contextMenu.Enable(wxID_COPY, false);
	}

	if (fromKey) PopupMenu(&contextMenu, GetCaretPoint());
	else PopupMenu(&contextMenu);
}

void EditorCtrl::OnMouseMotion(wxMouseEvent& event) {
	// Get Mouse location
	const wxPoint mpos = ClientPosToEditor(event.GetX(), event.GetY());

	// Close tooltip on motion
	/*if (m_revTooltip.IsShown() && m_revTooltipMousePos != wxGetMousePosition()) {
		m_revTooltip.Hide();
	}*/

	if (event.LeftIsDown() && HasCapture()) {
		wxASSERT(m_sel_start >= 0 && m_sel_start <= (int)m_lines.GetLength());
		wxASSERT(m_sel_end >= 0 && m_sel_end <= (int)m_lines.GetLength());

		// Find out what is under mouse
		const full_pos fp = m_lines.ClickOnLine(mpos.x, mpos.y);
		MakeCaretVisible();

		// Check if we should start dragging
		if (m_dragStartPos != wxDefaultPosition) {
			// We will start dragging if we have moved beyond a couple of pixels
			const int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X);
			const int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y);

			if (abs(mpos.x - m_dragStartPos.x) > drag_x_threshold ||
				abs(mpos.y - m_dragStartPos.y) > drag_y_threshold)
			{
				wxLogDebug(wxT("Starting text drag"));

				// Start drag
				wxDropSource dragSource(this);
				if (m_lines.IsMultiSelected()) {
					MultilineDataObject* mdo = new MultilineDataObject;
					const vector<interval>& selections = m_lines.GetSelections();
					cxLOCKDOC_READ(m_doc)
						// Get the selected text
						for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
							mdo->AddText(doc.GetTextPart((*iv).start, (*iv).end));
						}
					cxENDLOCK

					wxDataObjectComposite compObject;
					compObject.Add(mdo, true);
					compObject.Add(new wxTextDataObject(GetSelText()));
					
					dragSource.SetData(compObject);
					dragSource.DoDragDrop(wxDrag_DefaultMove);
				}
				else {
					wxTextDataObject textObject(GetSelText());				
					dragSource.SetData(textObject);
					dragSource.DoDragDrop(wxDrag_DefaultMove);
				}

				m_dragStartPos = wxDefaultPosition; // reset drag state
			}

			return;
		}

		// Making new selections removes selection shadow
		if (m_lines.IsSelectionShadow()) {
			m_lines.RemoveAllSelections();
			m_currentSel = -1;
		}

		if (event.AltDown()) { // Block selection
			SelectBlock(mpos, event.ControlDown());
			DrawLayout();
		}
		else {
			if (m_selMode == SEL_WORD && m_currentSel != -1) {
				// Extend selection one word at a time
				const interval iv = GetWordIv(fp.pos);
				const unsigned int sel_start = wxMin(m_sel_start, (int)iv.start);
				const unsigned int sel_end = wxMax(m_sel_end, (int)iv.end);
				m_currentSel = m_lines.UpdateSelection(m_currentSel, sel_start, sel_end);

				// move caret to closest end
				const int middle = sel_start + ((sel_end - sel_start)/2);
				if (fp.pos < middle) SetPos(sel_start);
				else SetPos(sel_end);

				DrawLayout();
			}
			else if (m_selMode == SEL_LINE && m_currentSel != -1) {
				// Extend selection one line at a time
				const unsigned int line_id = m_lines.GetCurrentLine();
				if (!m_lines.isLineVirtual(line_id)) {
					const unsigned int linestart = m_lines.GetLineStartpos(line_id);
					const unsigned int lineend = m_lines.GetLineEndpos(line_id, false); // include newline

					const unsigned int sel_start = wxMin(m_sel_start, (int)linestart);
					const unsigned int sel_end = wxMax(m_sel_end, (int)lineend);
					m_currentSel = m_lines.UpdateSelection(m_currentSel, sel_start, sel_end);
					DrawLayout();
				}
			}
			else if (fp.pos != m_sel_end) {
				// Normal selection
				m_sel_end = fp.pos;

				// Update the lines selection info
				if (!m_lines.IsSelected() || m_currentSel == -1) {
					m_currentSel = m_lines.AddSelection(m_sel_start, m_sel_end);
				}
				else {
					m_currentSel = m_lines.UpdateSelection(m_currentSel, m_sel_start, m_sel_end);
				}
				DrawLayout();
			}
		}
	}
	else {
		if (!m_foldTooltipTimer.IsRunning() && mpos.y >= 0 && mpos.y < m_lines.GetHeight()) {
			// Find out what is under mouse
			const unsigned int line_id = m_lines.GetLineFromYPos(mpos.y);

			// Check if we are hovering over a fold indicator
			if (IsLineFolded(line_id)) {
				const wxRect indicatorRect = m_lines.GetFoldIndicatorRect(line_id);

				if (indicatorRect.Contains(mpos)) {
					wxLogDebug(wxT("Starting Fold Indicator timer"));
					m_foldTooltipLine = line_id;
					m_foldTooltipTimer.Start(500, wxTIMER_ONE_SHOT);
				}
			}
		}
	}
}

void EditorCtrl::OnFoldTooltipTimer(wxTimerEvent& WXUNUSED(event)) {
	// Get mouse position
	const wxPoint m = ScreenToClient(wxGetMousePosition());
	const wxPoint mpos = ClientPosToEditor(m.x, m.y);

	if (mpos.y >= 0 && mpos.y < m_lines.GetHeight()) {
		// Find out what is under mouse
		const unsigned int line_id = m_lines.GetLineFromYPos(mpos.y);

		// Check if we are still hovering over a fold indicator
		if (line_id == m_foldTooltipLine) {
			wxRect bRect = m_lines.GetFoldIndicatorRect(line_id);
			if (bRect.Contains(mpos)) {
				const vector<cxFold*> foldStack = GetFoldStack(line_id);
				if (!foldStack.empty()) {
					// Find start of fold
					const cxFold* f = foldStack.back();
					const unsigned int fold_start = m_lines.GetLineStartpos(f->line_id);

					// Find the end of fold
					const unsigned int lastline = GetLastLineInFold(foldStack);
					unsigned int lastposinfold = m_lines.GetLineEndpos(lastline, false);

					wxString text = GetText(fold_start, lastposinfold);
					text.Replace(wxT("\t"), wxT("  ")); // replace tabs with spaces

					// Calc bounding rect
					wxPoint point = ClientToScreen(EditorPosToClient(bRect.x, bRect.y));
					bRect.x = point.x;
					bRect.y = point.y;

					// Show tooltip
					new wxTipWindow(this, text, 400, NULL, &bRect);
					wxLogDebug(wxT("Show fold tooltip"));
				}
			}
		}
	}
}


void EditorCtrl::OnLeaveWindow(wxMouseEvent& WXUNUSED(event)) {
	/*if (m_revTooltip.IsShown() && m_revTooltipMousePos != wxGetMousePosition()) {
		m_revTooltip.Hide();
	}*/
}

int EditorCtrl::ShowPopupList(const vector<const tmAction*>& actionList) {
	// Create list of refs to real list
	vector<size_t> sortedList;
	sortedList.resize(actionList.size());
	for (size_t i = 0; i < sortedList.size(); ++i) {
		sortedList[i] = i;
	}

	sort(sortedList.begin(), sortedList.end(), CompareActionBundle(actionList));

	// Create the menu
	wxMenu listMenu;
	const tmBundle* bundle = NULL;
	unsigned int shortcut = 1;
	for (vector<size_t>::const_iterator p = sortedList.begin(); p != sortedList.end(); ++p) {
		const tmAction& a = *actionList[*p];
		
		// Add bundle title
		if (a.bundle && a.bundle != bundle) {
			wxMenuItem* item = listMenu.Append(-1, a.bundle->name);
			item->Enable(false);
			bundle = a.bundle;
		}

		wxString itemText;
		if (a.bundle) itemText = wxT("  "); // slight indentation for bundle members
		itemText += a.name;
		
		if (shortcut < 10) itemText += wxString::Format(wxT("\t&%u"), shortcut);
		else if (shortcut == 10) itemText += wxT("\t&0");
		++shortcut;

		// Add item 
		listMenu.Append(new PopupMenuItem(&listMenu, 1000+(*p), itemText));
	}

	return ShowPopupList(listMenu);
}

int EditorCtrl::ShowPopupList(const wxArrayString& list) {
	// Create the menu
	wxMenu listMenu;
	int menuId = 1000;
	bool shortcuts = true;
	for (size_t i = 0; i < list.Count(); ++i) {
		if (list[i].empty()) {
			listMenu.AppendSeparator();
			shortcuts = false; // no shortcuts after separator
		}
		else {
			wxString itemText = list[i];

			// Add shortcuts
			if (shortcuts) {
				if (i < 9) itemText += wxString::Format(wxT("\t&%u"), i+1);
				else if (i == 9) itemText += wxT("\t&0");
			}

			listMenu.Append(new PopupMenuItem(&listMenu, menuId, itemText));
		}
		++menuId;
	}

	return ShowPopupList(listMenu);
}

int EditorCtrl::ShowPopupList(wxMenu& menu) {
	MakeCaretVisible();
	DrawLayout();

	// Get the caret coords
	const wxPoint caretPoint = GetCaretPoint();
	const wxPoint tipPos = ClientToScreen(caretPoint);

	m_popupMenuChoice = -1;
	PopupMenu(&menu, caretPoint.x, caretPoint.y + m_caretHeight);

	// It returns the menu ids - 1000
	return m_popupMenuChoice;
}

void EditorCtrl::OnPopupListMenu(wxCommandEvent& event) {
	m_popupMenuChoice = event.GetId() - 1000; // event range starts from 1000
}

void EditorCtrl::ShowTipAtCaret(const wxString& text) {
	MakeCaretVisible();
	DrawLayout();

	if (m_activeTooltip) m_activeTooltip->Close();

	m_activeTooltip = new TextTip(this, text, ClientToScreen(GetCaretPoint()), wxSize(0, m_caretHeight), &m_activeTooltip);
}

void EditorCtrl::ShowScopeTip() {
	if (m_syntaxstyler.IsOk()) {
		// Get scopes as string
		const deque<const wxString*> scopesArray = m_syntaxstyler.GetScope(GetPos());
		wxString scopes;
		for (unsigned int i = 0; i < scopesArray.size(); ++i) {
			if (!scopes.empty()) scopes += wxT("\n");
			scopes += *scopesArray[i];
		}

		// Show tip window
		ShowTipAtCaret(scopes);
	}
}
/*
void EditorCtrl::ShowRevTooltip() {
	const unsigned int pos = m_lines.GetPos();
	cxNodeInfo ni;
	cxLOCKDOC_READ(m_doc)
		ni = doc.GetNodeInfo(pos);
	cxENDLOCK

	if (ni.source.IsOk()) {
		MakeCaretVisible();

		const wxPoint tipPos = ClientToScreen(GetCaretPoint());
		m_revTooltip.SetDocument(ni.source, tipPos);

		// Remember mouse position so we know if mouse have been moved
		m_revTooltipMousePos = wxGetMousePosition();
	}
}
*/
void EditorCtrl::SelectBlock(wxPoint sel_endpoint, bool multi) {
	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();

	// Remove previous selection
	if (multi) {
		for (vector<unsigned int>::reverse_iterator p = m_blocksel_ids.rbegin(); p != m_blocksel_ids.rend(); ++p) {
			m_lines.RemoveSelection(*p);
		}
		m_blocksel_ids.clear();
	}
	else {
		m_lines.RemoveAllSelections();
		m_currentSel = -1;
	}

	// Order points
	int x_left = m_sel_startpoint.x;
	int x_right = sel_endpoint.x;
	if (x_left > x_right) {
		x_left = sel_endpoint.x;
		x_right = m_sel_startpoint.x;
	}
	int y_top = m_sel_startpoint.y;
	int y_bottom = sel_endpoint.y;
	if (y_top > y_bottom) {
		y_top = sel_endpoint.y;
		y_bottom = m_sel_startpoint.y;
	}

	// Extend y_bottom to bottom of line
	const unsigned int lineheight = m_lines.GetLineHeight();
	y_bottom += lineheight - (y_bottom % lineheight);

	// Make sure rect does not extend beyond text
	y_top = wxMax(y_top, 0);
	y_bottom = wxMin(y_bottom, m_lines.GetHeight());
	if (y_top % lineheight == 0) ++y_top; // offset the top a bit

	// If the entire block is behind the lines, we only select
	// the line endings.
	bool outbound = true;

	// Calculate how wide the block should be (rect extends with tabs)
	int x = x_left;
	int x2 = x_right;
	for (int y = y_top; y <= y_bottom; y += lineheight) {
		const full_pos selstart = m_lines.ClickOnLine(x_left, y, false);
		if (selstart.xy_outbound) continue;
		if (selstart.caret_xpos < (int)x) x = selstart.caret_xpos;

		const full_pos selend = m_lines.ClickOnLine(x_right, y, false);
		if (selend.caret_xpos > (int)x2) x2 = selend.caret_xpos;

		outbound = false; // At least one point is within line
	}

	// Select the lines
	for (int y2 = y_top; y2 <= y_bottom; y2 += lineheight) {
		const full_pos start = m_lines.ClickOnLine(x, y2, false);
		if (x && !outbound && start.xy_outbound) continue;

		const unsigned int end = m_lines.GetPosFromXY(x2, y2);
		const unsigned int sel_id = m_lines.AddSelection(start.pos, end);
		m_blocksel_ids.push_back(sel_id);
	}

	// Check if selection below text has moved caret to end (outside selection)
	const unsigned int pos = m_lines.GetPos();
	if (m_lines.IsSelected() && pos == m_lines.GetLength()) {
		const unsigned int lastselpos = m_lines.GetSelections().back().end;
		if (pos != lastselpos) m_lines.SetPos(lastselpos);
	}
}

void EditorCtrl::OnMouseLeftUp(wxMouseEvent& WXUNUSED(event)) {
	wxLogDebug(wxT("EditorCtrl::OnMouseLeftUp (pos=%d)"), GetPos());

	if (HasCapture()) {
		// Release the capture made in OnMouseLeftDown()
		wxLogDebug(wxT("EditorCtrl::ReleaseMouse()"));
		ReleaseMouse();

		//full_pos fp = lines.ClickOnLine(event.m_x, event.m_y + scrollPos);
		MakeCaretVisible();

		// Move caret to follow mouse
		//caret->Move(fp.caret_xpos, fp.caret_ypos - scrollPos);

		// Reset state variables
		m_selMode = SEL_NORMAL;
		m_currentSel = -1;
		m_blocksel_ids.clear();

		// If we had clicked on a selection, we can only remove selections
		// when we know a drag has not been initiated
		if (m_lines.IsSelected() && m_dragStartPos != wxDefaultPosition) {
			RemoveAllSelections();
			DrawLayout();
		}
	}

	//wxASSERT(currentSel == -1 || (unsigned int)currentSel < lines.GetSelections().size());
}

void EditorCtrl::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event)) {
	wxLogDebug(wxT("Mouse Capture Lost"));

	// Reset state variables
	m_selMode = SEL_NORMAL;
	m_currentSel = -1;
	m_blocksel_ids.clear();
	//m_dragStartPos = wxDefaultPosition; // reset drag state
}

void EditorCtrl::OnMouseDClick(wxMouseEvent& event) {
	// Get Mouse location
	const wxPoint mpos = ClientPosToEditor(event.GetX(), event.GetY());

	// Find out what was clicked on
	const full_pos fp = m_lines.ClickOnLine(mpos.x, mpos.y);

	// Set trible-click detection state
	m_doubleClickedLine = m_lines.GetLineFromCharPos(fp.pos);
	m_doubleClickTimer.Start();

	if (event.AltDown()) {
		SelectScope();
	}
	else {
		// Check if we should multiselect
		bool multiselect = event.ControlDown();

		// Select the word clicked on
		SelectWord(fp.pos, multiselect);
		wxLogDebug(wxT("DClick done %d"), GetPos());
	}

	DrawLayout();

	// Make sure we capure all mouse events
	// this is released in OnMouseLeftUp()
	//wxLogDebug(wxT("EditorCtrl::CaptureMouse() %d"), event.LeftDown());
	CaptureMouse();
}

void EditorCtrl::OnMouseWheel(wxMouseEvent& event) {
	// Remove tooltips
	/*if (m_revTooltip.IsShown()) {
		m_revTooltip.Hide();
	}*/

	if (GetScrollThumb(wxVERTICAL)) { // Only handle scrollwheel if we have a scrollbar
		const wxSize size = GetClientSize();
		int pos = scrollPos;
		const int rotation = event.GetWheelRotation();

		if (event.GetLinesPerAction() == (int)UINT_MAX) { // signifies to scroll a page
			wxScrollWinEvent newEvent;
			newEvent.SetOrientation(wxVERTICAL);
			newEvent.SetEventObject(this);

			if (rotation > 0) newEvent.SetEventType(wxEVT_SCROLLWIN_PAGEUP);
            else newEvent.SetEventType(wxEVT_SCROLLWIN_PAGEDOWN);

            ProcessEvent(newEvent);
		}
		else {
			const int linescount = (abs(rotation) / event.GetWheelDelta()) * event.GetLinesPerAction();

			if (rotation > 0) { // up
				pos = pos - (pos % m_lines.GetLineHeight()) - m_lines.GetLineHeight()*linescount;
				if (pos < 0) pos = 0;
			}
			else if (rotation < 0) { // down
				pos = pos - (pos % m_lines.GetLineHeight()) + m_lines.GetLineHeight()*linescount;
				if (pos > m_lines.GetHeight() - size.y) pos = m_lines.GetHeight() - size.y;
			}
			else return; // no rotation

			if (pos != scrollPos) {
				scrollPos = pos;
				DrawLayout();
			}
		}
	}
}

void EditorCtrl::SetScroll(unsigned int ypos) {
	if (scrollPos != (int)ypos) {
		old_scrollPos = scrollPos;
		scrollPos = ypos;
		DrawLayout(true);
	}
}

void EditorCtrl::OnScroll(wxScrollWinEvent& event) {
	// Remove tooltips
	/*if (m_revTooltip.IsShown()) {
		m_revTooltip.Hide();
	}*/

	const wxSize size = GetClientSize();
	int pos = (event.GetOrientation() == wxVERTICAL) ? scrollPos : m_scrollPosX;
	const int page_size = (event.GetOrientation() == wxVERTICAL) ? size.y : m_lines.GetDisplayWidth();
	const int doc_size = (event.GetOrientation() == wxVERTICAL) ? m_lines.GetHeight() : m_lines.GetWidth();
	const int line_size = (event.GetOrientation() == wxVERTICAL) ? m_lines.GetLineHeight() : 10;

	if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK ||
		event.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE)
	{
		pos = event.GetPosition();
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP) {
		pos -= page_size;
		if (pos < 0) pos = 0;
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN) {
		pos += page_size;
		if (pos > doc_size - page_size) pos = doc_size - page_size;
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP) {
		pos = pos - (pos % line_size) - line_size;
		if (pos < 0) pos = 0;
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN) {
		pos = pos - (pos % line_size) + line_size;
		if (pos > doc_size - size.y) pos = doc_size - page_size;
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_TOP) {
		pos = 0;
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM) {
		pos = -1; // end
	}

	if (event.GetOrientation() == wxVERTICAL) {
		if (pos != scrollPos) {
			wxASSERT(pos >= -1);

			old_scrollPos = scrollPos;
			scrollPos = pos;
			DrawLayout(true);
		}
	}
	else {
		// Horizontal scroll
		if (pos != m_scrollPosX) {
			m_scrollPosX = pos;
			DrawLayout(false);
		}
	}
}

void EditorCtrl::OnIdle(wxIdleEvent& event) {
	if (!m_doc.IsOk()) return; // The doc may have been closed

	const int topline = m_lines.GetLineFromYPos(scrollPos);
	const int lineoffset = scrollPos - m_lines.GetYPosFromLine(topline);

	// Update lines
	if (m_lines.NeedIdle()) {
		m_lines.OnIdle();
	}

	// Update syntax
	const bool syntaxNeedIdle = m_syntaxstyler.OnIdle();

	// Update foldings
	ParseFoldMarkers();

	// ScrollPos may no longer be valid, calc new scrollpos
	if (GetScrollThumb(wxVERTICAL)) {
		scrollPos = m_lines.GetYPosFromLine(topline) + lineoffset;
		SetScrollbar(wxVERTICAL, scrollPos, GetClientSize().y, m_lines.GetHeight());
	}

	// Check if we should request more idle events
	if (syntaxNeedIdle || m_lines.NeedIdle()) {
		//wxLogDebug(wxT("EditorCtrl::OnIdle - RequestMore %d %d"), syntaxNeedIdle, lines.NeedIdle());
		event.RequestMore();
	}
}

void EditorCtrl::OnClose(wxCloseEvent& WXUNUSED(event)) {
	//Destroy();
}

void EditorCtrl::OnSetDocument(EditorCtrl* self, void* data, int filter) {
	wxASSERT(self->IsOk());

	if (filter != self->GetId()) return;

	const doc_id* const di = (doc_id*)data;

	if (self->m_doc != *di) {
		self->SetDocument(*di);
	}
}

void EditorCtrl::OnDocCommited(EditorCtrl* self, void* data, int WXUNUSED(filter)) {
	wxASSERT(self->IsOk());

	const docid_pair* const dp = (docid_pair*)data;
	wxASSERT(dp->doc1.IsDraft());
	wxASSERT(dp->doc2.IsDocument());

	const doc_id di = self->GetDocID();
	if (di.SameDoc(dp->doc1)) {
		self->SetDocument(dp->doc2);
	}
}

void EditorCtrl::OnThemeChanged(EditorCtrl* self, void* WXUNUSED(data), int WXUNUSED(filter)) {
	wxASSERT(self->IsOk());

	const wxFont& mdcFont = self->mdc.GetFont();
	const wxFont& themeFont = self->m_theme.font;

	// Restyle the syntax highlighting
	// (we have to do this before updating in lines to
	// avoid refs to invalid styles)
	self->m_syntaxstyler.ReStyle();

	// Update theme settings
	if (mdcFont != themeFont) {
		self->mdc.SetFont(themeFont);
		self->m_lines.UpdateFont(); // also invalidates line positions
		self->m_caretHeight = self->m_lines.GetLineHeight();
		self->caret->SetSize(2, self->m_caretHeight);
	}
	else self->m_lines.Invalidate();

	self->m_gutterCtrl->UpdateTheme();

	self->DrawLayout();
}

void EditorCtrl::OnSettingsChanged(EditorCtrl* self, void* WXUNUSED(data), int WXUNUSED(filter)) {
	wxASSERT(self->IsOk());

	bool doShowMargin = false;
	int marginChars = 80;

	// Update settings
	cxLOCK_READ(self->m_catalyst)
		catalyst.GetSettingBool(wxT("autoPair"), self->m_doAutoPair);
		catalyst.GetSettingBool(wxT("autoWrap"), self->m_doAutoWrap);
		catalyst.GetSettingBool(wxT("showMargin"), doShowMargin);
		catalyst.GetSettingBool(wxT("wrapMargin"), self->m_wrapAtMargin);
		catalyst.GetSettingInt(wxT("marginChars"), marginChars);
	cxENDLOCK

	if (!doShowMargin) {
		marginChars = 0;
		self->m_wrapAtMargin = false;
	}

	// Make sure we keep the same topline
	const unsigned int topline = self->m_lines.GetLineFromYPos(self->scrollPos);

	self->m_isResizing = true;
	self->m_lines.ShowMargin(marginChars);
	if (self->m_wrapAtMargin && self->m_lines.GetWrapMode() != cxWRAP_NONE) {
		self->m_lines.SetWidth(self->m_lines.GetMarginPos());
	}
	
	self->scrollPos = self->m_lines.GetYPosFromLine(topline);
	self->DrawLayout();
}


void EditorCtrl::OnBundlesReloaded(EditorCtrl* self, void* WXUNUSED(data), int WXUNUSED(filter)) {
	wxASSERT(self->IsOk());

	// Check if we are editing an item that has been modified
	if (self->IsBundleItem()) {
		//TODO: If modified by externally, ask user if he wish to reload
	}

	// Re-set the syntax
	// (we have to do this before updating in lines to avoid refs to invalid styles)
	const wxString syntaxName = self->m_syntaxstyler.GetName();
	self->m_syntaxstyler.SetSyntax(syntaxName);

	// Update theme settings
	if (self->mdc.GetFont() != self->m_theme.font) {
		self->mdc.SetFont(self->m_theme.font);
		self->m_lines.UpdateFont();
		self->m_caretHeight = self->m_lines.GetLineHeight();
		self->caret->SetSize(2, self->m_caretHeight);
	}
	else self->m_lines.Invalidate();

	self->MarkAsModified();

	self->DrawLayout();
}

bool EditorCtrl::DoShortcut(int keyCode, int modifiers) {
	// Get list of all actions available from current scope
	vector<const tmAction*> actions;
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(GetPos());
	m_syntaxHandler.GetActions(scope, actions, TmSyntaxHandler::ShortcutMatch(keyCode, modifiers));

	// Key Diagnostics
	if (m_parentFrame.IsKeyDiagMode()) {
		Insert(wxString::Format(wxT("\tactions=%d\n"), actions.size()));
		if (actions.size() == 1) {
			if (actions[0]->bundle) Insert(wxString::Format(wxT("\tbundle=%s\n"), actions[0]->bundle->name.c_str()));
			Insert(wxString::Format(wxT("\taction=%s\n"), actions[0]->name.c_str()));
		}
	}

	if (actions.empty()) return false; // no matching shortcut
	else if (actions.size() == 1) {
		DoAction(*actions[0], NULL, false);
	}
	else {
		// Show popup menu
		const int result = ShowPopupList(actions);
		if (result != -1) {
			DoAction(*actions[result], NULL, false);
		}
	}

	return true;
}

void EditorCtrl::DoDragCommand(const tmDragCommand &cmd, const wxString& path) {
	map<wxString, wxString> env;
	wxLogDebug(wxT("DoDragCommand pos:%d len:%d"), GetPos(), GetLength());
	// TODO: Handle native mode

	// Make path relative to document dir
	const wxFileName& docPath = GetFilePath();
	if (docPath.IsOk()) {
		wxFileName unixDocPath(eDocumentPath::WinPathToCygwin(docPath), wxPATH_UNIX);
		wxFileName filePath(eDocumentPath::WinPathToCygwin(path), wxPATH_UNIX);
		filePath.MakeRelativeTo(unixDocPath.GetPath(0, wxPATH_UNIX), wxPATH_UNIX);

		env[wxT("TM_DROPPED_FILE")] = filePath.GetFullPath(wxPATH_UNIX);
	}
	else env[wxT("TM_DROPPED_FILE")] = eDocumentPath::WinPathToCygwin(path);

	// Full path
	env[wxT("TM_DROPPED_FILEPATH")] = eDocumentPath::WinPathToCygwin(path);

	// Modifiers
	wxString modifiers;
	if (wxGetKeyState(WXK_SHIFT)) modifiers += wxT("SHIFT");
	if (wxGetKeyState(WXK_ALT)) {
		if (!modifiers.empty()) modifiers += wxT('|');
		modifiers += wxT("CONTROL");
	}
	if (wxGetKeyState(WXK_WINDOWS_LEFT) || wxGetKeyState(WXK_WINDOWS_RIGHT)) {
		if (!modifiers.empty()) modifiers += wxT('|');
		modifiers += wxT("OPTION");
	}
	if (wxGetKeyState(WXK_CONTROL)) {
		if (!modifiers.empty()) modifiers += wxT('|');
		modifiers += wxT("COMMAND");
	}
	env[wxT("TM_MODIFIER_FLAGS")] = modifiers;

	// Do the command
	DoAction(cmd, &env, false);

	wxLogDebug(wxT("After DoDragCommand pos:%d len:%d"), GetPos(), GetLength());
}

void EditorCtrl::OnDragOver(wxCoord x, wxCoord y) {
	const unsigned int lineHeight = m_lines.GetLineHeight();
	const int scrollBorder = lineHeight / 2;
	const wxSize size = GetClientSize();

	// Check if we should scroll vertical
	if (y < scrollBorder) {
		scrollPos = scrollPos - (scrollPos % lineHeight) - lineHeight;
		if (scrollPos < 0) scrollPos = 0;
	}
	else if (y > size.y - scrollBorder) {
		scrollPos = scrollPos - (scrollPos % lineHeight) + lineHeight;
		if (scrollPos > m_lines.GetHeight() - size.y) scrollPos = m_lines.GetHeight() - size.y;
	}

	// Check if we should scroll horizontal
	const int editorX = ClientWidthToEditor(x);
	const int editorWidth = ClientWidthToEditor(size.x);
	if (editorX < scrollBorder) {
		m_scrollPosX -= 50;
		if (m_scrollPosX < 0) m_scrollPosX = 0;
	}
	else if (editorX > editorWidth - 50) {
		m_scrollPosX += 50;

		if (m_scrollPosX + editorWidth > m_lines.GetWidth()) {
			m_scrollPosX = m_lines.GetWidth() - editorWidth;
			m_scrollPosX = wxMax(m_scrollPosX, 0);
		}
	}

	// Move caret to insertion point
	const wxPoint mpos = ClientPosToEditor(x, y);
	m_lines.ClickOnLine(mpos.x, mpos.y);

	DrawLayout();
}

void EditorCtrl::OnDragDropText(const wxString& text, wxDragResult dragType) {
	wxLogDebug(wxT("OnDragDropText (pos=%d)"), GetPos());

	// If we are moving inside same doc we should delete source
	if (dragType == wxDragMove && m_dragStartPos != wxDefaultPosition) {
		DeleteSelections(); // Will adjust caret pos
	}
	RemoveAllSelections();

	const unsigned int pos = GetPos();

#ifdef __WXMSW__
	wxString copytext = text;
	ConvertCRLFtoLF(copytext);
	const unsigned int byte_len = RawInsert(pos, copytext);
#else
	const unsigned int byte_len = RawInsert(pos, text);
#endif

	// Select the new text
	const unsigned int newpos = pos + byte_len;
	m_lines.AddSelection(pos, newpos);
	SetPos(newpos);

	MakeCaretVisible();
	DrawLayout();

	m_parentFrame.BringToFront();
}

void EditorCtrl::OnDragDropColumn(const wxArrayString& text, wxDragResult dragType) {
	wxLogDebug(wxT("OnDragDropColumn (pos=%d)"), GetPos());

	// If we are moving inside same doc we should delete source
	if (dragType == wxDragMove && m_dragStartPos != wxDefaultPosition) {
		DeleteSelections(); // Will adjust caret pos
	}
	RemoveAllSelections();

	InsertColumn(text, true /*select*/);
	
	m_parentFrame.BringToFront();
}

void EditorCtrl::OnDragDrop(const wxArrayString& filenames) {
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(GetPos());

	// Remove selections before showing popup menu
	RemoveAllSelections();
	DrawLayout();

	bool newTabs = false;
	m_parentFrame.Raise();

	for (unsigned int i = 0; i < filenames.GetCount(); ++i) {
		// Go to end of active snippet (we might be inserting multible);
		if (i && m_snippetHandler.IsActive()) {
			RemoveAllSelections();
			m_snippetHandler.GotoEndAndClear();
		}

		// Get extension
		const wxString ext = filenames[i].AfterLast(wxT('.'));

		// Get matching DragCommands
		vector<const tmDragCommand*> actions;
		m_syntaxHandler.GetDragActions(scope, actions, TmSyntaxHandler::ExtMatch(ext));

		if (actions.empty()) {
			// No matches. Open new doc
			m_parentFrame.Open(filenames[i]);
			newTabs = true;
			continue;
		}

		// Create the menu
		wxMenu listMenu;
		int menuId = 1010; // first 10 are reserved for commands

		if (filenames.GetCount() > 1) {
			const wxFileName name(filenames[i]);
			listMenu.Append(1000, name.GetFullName());
			listMenu.Enable(1000, false);
			listMenu.AppendSeparator();
		}

		// Add drag actions
		wxArrayString actionList;
		for (vector<const tmDragCommand*>::const_iterator p = actions.begin(); p != actions.end(); ++p) {
			listMenu.Append(menuId++, (*p)->name);
		}

		// Add commands
		listMenu.AppendSeparator();
		listMenu.Append(1001, _("Open"));
		if (i < filenames.GetCount()-1) listMenu.Append(1002, _("Open All"));

		// Show menu
		const int result = ShowPopupList(listMenu);
		if (result == 1001-1000) {
			// Open
			m_parentFrame.Open(filenames[i]);
			newTabs = true;
		}
		else if (result == 1002-1000) {
			// Open all
			for (; i < filenames.GetCount(); ++i) {
				m_parentFrame.Open(filenames[i]);
			}
			newTabs = true;
			break;
		}
		else if (result >= 1010-1000) {
			DoDragCommand(*actions[result-10], filenames[i]);
		}
	}

	if (!newTabs) SetFocus();
}

int EditorCtrl::GetSymbols(vector<Styler_Syntax::SymbolRef>& symbols) const {
	// Only return symbols if the entire syntax is parsed
	if (!m_syntaxstyler.IsParsed() || !m_syntaxHandler.AllBundlesLoaded()) return 0;

	// DEBUG: We want to be able to see in crash dumps if symbols was reloaded
	int res = 1;

	// Check if we have symbols cached (so we avoid building them
	// twice for statusbar & symbollist).
	if (m_symbolCacheToken != m_changeToken) {
		m_symbolCache.clear();
		m_syntaxstyler.GetSymbols(m_symbolCache);
		m_symbolCacheToken = m_changeToken;
		res = 2; // DEBUG: reloaded
	}

	symbols = m_symbolCache;
	return res;
}

wxString EditorCtrl::GetSymbolString(const Styler_Syntax::SymbolRef& sr) const {
	const Styler_Syntax::SymbolRef sr_debug = sr; // copy so we can see contents in call stack

	const wxString& transform = *(sr_debug.transform);

	// If there is no transformation, just return the text
	if (transform.empty()) return GetText(sr.start, sr.end);

	// Get the full symbol
	vector<char> source;
	GetTextPart(sr.start, sr.end, source);

	// Strip newlines
	vector<char>::iterator p = remove(source.begin(), source.end(), '\n');
	source.erase(p, source.end());

	wxString regex;
	wxString replace;
	const size_t len = transform.size();

	// Parse transformation
	for (size_t i = 0; i < len; ++i) {
		wxChar c = transform[i];

		if (c == 's' && i+1 < len && transform[i+1] == '/') {
			i += 2; // advance over "s/"
			const size_t regexstart = i;

			// Get regex part
			for (; i < len; ++i) {
				c = transform[i];
				if (c == '\\') ++i; // ignore escaped
				else if (c == '/') {
					regex = transform.substr(regexstart, i-regexstart);

					// Get replacement part
					const size_t repstart = ++i;
					for (;i < len; ++i) {
						c = transform[i];
						if (c == '\\') ++i; // ignore escaped
						else if (c == '/') {
							replace = transform.substr(repstart, i-repstart);

#ifdef __WXMSW__
							// em space (unicode 0x2003) is used in some transforms
							// but since windows draws them as a box we replace them
							// with real space
							replace.Replace(wxT("\x2003"), wxT(" "));
#endif

							// Do the regex search
							map<unsigned int,interval> captures;
							const search_result res = RawRegexSearch(regex.mb_str(wxConvUTF8), source, 0, &captures);

							if (res.error_code >= 0) {
								// Get the replacement
								const wxString rep = ParseReplaceString(replace, captures, &source);

								// Insert instead of match
								source.erase(source.begin()+res.start, source.begin()+res.end);
								const wxCharBuffer buf = rep.mb_str(wxConvUTF8);
								source.insert(source.begin()+res.start, buf.data(), buf.data()+strlen(buf.data()));
							}

							break;
						}
					}

					break;
				}
			}
		}
		else if (c == '#') {
			// ignore comments
			while (i < len && transform[i] != '\n') ++i;
		}
	}

	if (source.empty()) return wxEmptyString;
	else return wxString(&*source.begin(), wxConvUTF8, source.size());
}

bool EditorCtrl::OnPreKeyDown(wxKeyEvent& event) {
	// Don't allow input if we are in busy state
	// (otherwise we might get recursive Yield in cxExecute)
	if (wxIsBusy()) return true;

	int id = event.GetKeyCode();

#ifdef __WXMSW__
	// Detect AltGr. We don't want to use combinations with AltGr in shortcuts,
	// as it would make it impossible to enter national chars in certain locales.
	if (event.m_rawCode == 18 &&                        // alt is down
		(HIWORD(event.m_rawFlags) & KF_EXTENDED) && // it is the right alt key
		wxIsCtrlDown())                             // ctrl is still down
	{
		wxLogDebug(wxT("AltGr Down"));
		s_altGrDown = true;
	}
#endif

	// Ignore initial presses on modifier key
	if (id != WXK_SHIFT &&
		id != WXK_ALT &&
		id != WXK_CONTROL &&
		id != WXK_WINDOWS_LEFT &&
		id != WXK_WINDOWS_RIGHT)
	{
		if (m_parentFrame.IsKeyDiagMode()) {
			wxString msg = wxT("\nKey Diagnostics:\n");
			msg += wxString::Format(wxT("\tid=%d\n\tmodifiers=0x%x\n\tag=%d\n"), id, event.GetModifiers(), (int)s_altGrDown);
			Insert(msg);
		}

#ifdef __WXMSW__
		// We only want to ignore AltGr if it actually produce output
		// (otherwise right-alt+ctrl would be unusable on us keyboards)
		if (s_altGrDown) {
			unsigned char keystate[256];
			memset (keystate, 0, sizeof (keystate));
			::GetKeyboardState(keystate);

			const int BUFFER_SIZE = 2;
			unsigned char buf[BUFFER_SIZE] = { 0 };
			const int ret = ::ToAscii(event.m_rawCode, 0, keystate, (WORD *)buf, 0);

			if (m_parentFrame.IsKeyDiagMode()) {
				Insert(wxString::Format(wxT("\tret=%d\n\tbuf=%d\n"), ret, *buf));
			}

			if ((ret >= 1) && (((*buf >= 32) && (*buf <= 126)) || ((*buf >= 160) && (*buf <= 255)))) {
				return false;
			}
		}
#endif

		int modifiers = event.GetModifiers();

		// Bundle shortcuts have highest priority
		if (DoShortcut(id, modifiers)) return true;

#ifdef __WXMSW__
		// Ctrl-Back gets translated by the system to Ctrl-Delete so
		// we have to catch it here and send our own event
		if (id == WXK_BACK && modifiers == 0x0002) {
			wxKeyEvent event(CreateKeyEvent(wxEVT_CHAR, id, event.m_rawFlags, event.m_rawCode));
			ProcessEvent(event);
			return true;
		}
#endif
	}
	return false;
}
bool EditorCtrl::OnPreKeyUp(wxKeyEvent& event) {
#ifdef __WXMSW__
		if (event.m_rawCode == 17 || event.m_rawCode == 18) {
			s_altGrDown = false;
		}
#endif
	return false;
}

#ifdef __WXDEBUG__

void EditorCtrl::Print() {
	cxLOCKDOC_WRITE(m_doc)
		doc.Print();
	cxENDLOCK
}

#endif  //__WXDEBUG__

EditorCtrl::cxFold::cxFold(unsigned int line, cxFoldType type, unsigned int indent)
: line_id(line), type(type), count(0), indent(indent) {
}

vector<unsigned int> EditorCtrl::GetFoldedLines() const {
	vector<unsigned int> folds;
	for (vector<cxFold>::const_iterator p = m_folds.begin(); p != m_folds.end(); ++p) {
		if (p->type == cxFOLD_START_FOLDED) folds.push_back(p->line_id);
	}
	return folds;
}

bool EditorCtrl::HasFoldedFolds() const {
	for (vector<cxFold>::const_iterator p = m_folds.begin(); p != m_folds.end(); ++p) {
		if (p->type == cxFOLD_START_FOLDED) return true;
	}
	return false;
}

void EditorCtrl::FoldingClear() {
	m_folds.clear();
	m_foldedLines = 0;
	m_foldLineCount = 0;
}

void EditorCtrl::FoldingInvalidate() {
	FoldingClear();
}

void EditorCtrl::ParseFoldMarkers() {
	if (!m_syntaxstyler.IsOk()) return; // no syntax

	// Get fold status for each line in doc
	const unsigned int lineCount = m_lines.GetLineCount(false/*includeVirtual*/);
	if (m_foldLineCount == 0) m_foldLineCount = lineCount; // first run after invalidate
	wxASSERT(m_foldLineCount == lineCount);
	wxASSERT(m_foldedLines <= lineCount);
	if (m_foldedLines == lineCount) return;

	unsigned int i = m_foldedLines;
	unsigned int lineStart = m_lines.GetLineStartpos(i);
	const unsigned int lastSyntaxedLine = m_lines.GetLineFromCharPos(m_syntaxstyler.GetLastParsedPos());

	while (i < lineCount && i < lastSyntaxedLine) {
		const unsigned int lineEnd = m_lines.GetLineEndpos(i, false);

		// Check if we have a fold rule
		const deque<const wxString*> scope = m_syntaxstyler.GetScope(lineStart);
		if (const TmSyntaxHandler::cxFoldRule* rule = m_syntaxHandler.GetFoldRule(scope)) {
			// Do we match fold starter
			const search_result sr = RawRegexSearch(&*rule->foldingStartMarker.begin(), lineStart, lineEnd, lineStart);
			const bool matchStartMarker = (sr.error_code > 0);

			// Do we match fold ender
			const search_result sr2 = RawRegexSearch(&*rule->foldingEndMarker.begin(), lineStart, lineEnd, lineStart);
			const bool matchEndMarker = (sr2.error_code > 0);

			if (matchStartMarker) {
				if (!matchEndMarker) { // starter and ender on same line cancels eachother out
					m_folds.push_back(cxFold(i, cxFOLD_START, GetLineIndentLevel(i)));
				}
			}
			else if (matchEndMarker) {
				m_folds.push_back(cxFold(i, cxFOLD_END, GetLineIndentLevel(i)));
			}
		}

		lineStart = lineEnd;
		++i;
	}

	m_foldedLines = i;
}

vector<EditorCtrl::cxFold>::iterator EditorCtrl::ParseFoldLine(unsigned int line_id, vector<cxFold>::iterator insertPos, bool doFold) {
	wxASSERT(line_id < m_lines.GetLineCount());

	const unsigned int lineStart = m_lines.GetLineStartpos(line_id);

	// Check if we have a fold rule
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(lineStart);
	if (const TmSyntaxHandler::cxFoldRule* rule = m_syntaxHandler.GetFoldRule(scope)) {
		const unsigned int lineEnd = m_lines.GetLineEndpos(line_id, false);

		// Do we match fold starter
		const char* const startMarker = &*rule->foldingStartMarker.begin();
		const search_result sr = RawRegexSearch(startMarker, lineStart, lineEnd, lineStart);
		const bool matchStartMarker = (sr.error_code > 0);

		// Do we match fold ender
		const char* const endMarker = &*rule->foldingEndMarker.begin();
		const search_result sr2 = RawRegexSearch(endMarker, lineStart, lineEnd, lineStart);
		const bool matchEndMarker = (sr2.error_code > 0);

		if (matchStartMarker) {
			if (!matchEndMarker) { // starter and ender on same line cancels eachother out
				return m_folds.insert(insertPos, cxFold(line_id, (doFold ? cxFOLD_START_FOLDED : cxFOLD_START), GetLineIndentLevel(line_id)))+1;
			}
		}
		else if (matchEndMarker) {
			return m_folds.insert(insertPos, cxFold(line_id, cxFOLD_END, GetLineIndentLevel(line_id)))+1;
		}
	}

	return insertPos; // nothing inserted
}

void EditorCtrl::FoldingInsert(unsigned int pos, unsigned int len) {
	// Find out which lines was affected
	const unsigned int lineCount = m_lines.GetLineCount(false/*includeVirtual*/);
	const unsigned int firstline = m_lines.GetLineFromCharPos(pos);
	unsigned int lastline = m_lines.GetLineFromCharPos(pos + len);
	if (lastline && m_lines.isLineVirtual(lastline)) --lastline; // Don't try to parse last virtual line

	// How many new lines was inserted?
	unsigned int newLines = lastline - firstline;
	if (firstline == m_foldLineCount) ++newLines; // adjust for first insertion in last line (creating it)
	wxASSERT(newLines == lineCount - m_foldLineCount);

	// Find (and erase for re-parsing) the first modified line
	bool doRefold = false;
	const cxFold target(firstline);
	vector<cxFold>::iterator p = lower_bound(m_folds.begin(), m_folds.end(), target);
	if (p != m_folds.end() && p->line_id == firstline) {
		if (p->type == cxFOLD_START_FOLDED && newLines == 0) doRefold = true;
		p = m_folds.erase(p);
	}

	// Parse the new lines
	for (unsigned int i = firstline; i <= lastline; ++i) {
		p = ParseFoldLine(i, p, doRefold);
	}

	// Adjust line id's in following
	if (newLines) {
		while (p != m_folds.end()) {
			p->line_id += newLines;
			++p;
		}
	}

	m_foldedLines += newLines;
	wxASSERT(m_foldedLines <= lineCount);
	m_foldLineCount = lineCount;
}

void EditorCtrl::FoldingReIndent() {
	for (vector<cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p) {
		p->indent = GetLineIndentLevel(p->line_id);
	}
}

void EditorCtrl::FoldingDelete(unsigned int start, unsigned int WXUNUSED(end)) {
	// Check if all has been deleted
	if (GetLength() == 0) {
		FoldingClear();
		return;
	}

	// Find the affected line
	const unsigned int line_id = m_lines.GetLineFromCharPos(start);

	// How many lines was deleted
	const unsigned int newLines = m_foldLineCount - m_lines.GetLineCount(false/*includeVirtual*/);

	// Find (and re-parse) the modified line
	bool doRefold = false;
	const cxFold target(line_id);
	vector<cxFold>::iterator p = lower_bound(m_folds.begin(), m_folds.end(), target);
	if (p != m_folds.end() && p->line_id == line_id) {
		if (p->type == cxFOLD_START_FOLDED) doRefold = true;
		p = m_folds.erase(p);
	}
	if (!m_lines.isLineVirtual(line_id)) {
		p = ParseFoldLine(line_id, p, doRefold);
	}

	// Remove deleted lines
	const unsigned int lastline = line_id + newLines;
	while (p != m_folds.end() && p->line_id <= lastline) {
		p = m_folds.erase(p);
	}

	// Adjust line id's in following
	if (newLines) {
		while (p != m_folds.end()) {
			p->line_id -= newLines;
			++p;
		}
	}

	m_foldedLines -= newLines;
	const unsigned int lineCount = m_lines.GetLineCount(false/*includeVirtual*/);
	wxASSERT(m_foldedLines <= lineCount);
	m_foldLineCount = lineCount;
}

void EditorCtrl::FoldingApplyDiff(const vector<cxLineChange>& linechanges) {
	// Check if all has been deleted
	if (m_lines.IsEmpty()) {
		FoldingClear();
		return;
	}

	for (vector<cxLineChange>::const_iterator l = linechanges.begin(); l != linechanges.end(); ++l) {
		const unsigned int line_id = m_lines.GetLineFromCharPos(l->start);

		if (line_id > m_foldedLines) return;
		
		// Find the first line of modification
		const cxFold target(line_id);
		vector<cxFold>::iterator f = lower_bound(m_folds.begin(), m_folds.end(), target);

		// Erase for re-parsing the first modified line
		bool doRefold = false;
		if (f != m_folds.end() && f->line_id == line_id) {
			if (f->type == cxFOLD_START_FOLDED) doRefold = true;
			f = m_folds.erase(f);
		}

		if (l->lines >= 0) { // INSERTION or edit on single line
			const unsigned int lastline = line_id + l->lines;

			// Parse the new lines
			for (unsigned int i = line_id; i <= lastline; ++i) {
				f = ParseFoldLine(i, f, doRefold);
				if (doRefold) doRefold = false; // only refold first line
			}
		}
		else { // DELETION
			// Reparse line with partial deletion
			if (!m_lines.isLineVirtual(line_id)) {
				f = ParseFoldLine(line_id, f, doRefold);
			}

			// Remove deleted lines
			const unsigned int lastline = line_id - l->lines;
			while (f != m_folds.end() && f->line_id <= lastline) {
				f = m_folds.erase(f);
			}
		}

		// Adjust line id's in following
		if (l->lines != 0) {
			for ( ; f != m_folds.end(); ++f) {
				f->line_id += l->lines;
			}
			m_foldedLines += l->lines;
		}
	}

	m_foldLineCount = m_lines.GetLineCount(false/*includeVirtual*/);
	wxASSERT(m_foldedLines <= m_foldLineCount);
}

unsigned int EditorCtrl::GetLastLineInFold(const vector<cxFold*>& fStack) const {
	wxASSERT(!fStack.empty());

	// We need a modifiable (but const) stack
	vector<const cxFold*> foldStack;
	foldStack.reserve(fStack.size());
	for (vector<cxFold*>::const_iterator s = fStack.begin(); s != fStack.end(); ++s) foldStack.push_back(*s);

    // Convert pointer to iterator
	vector<cxFold>::const_iterator p = m_folds.begin() + (foldStack.back() - &*m_folds.begin());
	const unsigned int foldLine = p->line_id;

	for (vector<cxFold>::const_iterator f = p+1; f != m_folds.end(); ++f) {
		if (f->type != cxFOLD_END){
			foldStack.push_back(&*f);
			continue;
		}

		// Check if end marker matches any starter on the stack (ignore unmatched)
		for (vector<const cxFold*>::reverse_iterator fr = foldStack.rbegin(); fr != foldStack.rend(); ++fr) {
			if (f->indent == (*fr)->indent) {
				if ((*fr)->line_id == foldLine) {
					return f->line_id; // end matches current
				}
				else if ((*fr)->line_id < foldLine) {
					return f->line_id-1; // end matches previous (ending fold prematurely)
				}
				else {
					// skip subfolds
					vector<const cxFold*>::iterator fb = (++fr).base();
					foldStack.erase(fb, foldStack.end()); // pop
					break;
				}
			}
		}
	}

	return m_foldLineCount-1; // default is to end-of-doc
}

void EditorCtrl::Fold(unsigned int line_id) {
	wxASSERT(line_id < m_foldLineCount);

	// Find the foldmarker
	vector<cxFold*> foldStack = GetFoldStack(line_id);
	wxASSERT(!foldStack.empty());
	vector<cxFold>::iterator p = m_folds.begin() + (foldStack.back() - &*m_folds.begin()); // convert pointer to iterator
	wxASSERT(p != m_folds.end() && p->type == cxFOLD_START);
	wxASSERT(p->line_id == line_id);

	// Do the fold
	p->type = cxFOLD_START_FOLDED;

	// Find start of fold (end-of-line in the fold starting line)
	const unsigned int endofline = m_lines.GetLineEndpos(line_id);

	// Find the end of fold
	const unsigned int lastline = GetLastLineInFold(foldStack);
	unsigned int lastposinfold = m_lines.GetLineEndpos(lastline, false);

	// Keep track of number of lines folded
	p->count = lastline - p->line_id;

	// if caret is inside fold, move it to top of fold (end-of-line)
	const unsigned int pos = GetPos();
	if (pos > endofline && (pos < lastposinfold || lastposinfold == GetLength())) {
		m_lines.RemoveAllSelections();
		SetPos(endofline);
	}
}

void EditorCtrl::FoldAll() {
	for (vector<cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p) {
		if (p->type == cxFOLD_START) Fold(p->line_id);
	}
}

void EditorCtrl::FoldOthers() {
	const unsigned int line_id = m_lines.GetCurrentLine();
	const unsigned int pos = m_lines.GetPos();

	FoldAll();
	UnFoldParents(line_id);
	m_lines.SetPos(pos); // pos could be moved by FoldAll()
}

void EditorCtrl::UnFold(unsigned int line_id) {
	wxASSERT(line_id < m_lines.GetLineCount());

	// Find the foldmarker
	const cxFold target(line_id);
	vector<cxFold>::iterator p = lower_bound(m_folds.begin(), m_folds.end(), target);
	wxASSERT(p != m_folds.end() && p->type == cxFOLD_START_FOLDED);

	p->type = cxFOLD_START;
	p->count = 0;
}

void EditorCtrl::UnFoldParents(unsigned int line_id) {
	wxASSERT(line_id < m_lines.GetLineCount());

	// Make sure the line is visible
	vector<cxFold*> foldStack = GetFoldStack(line_id);
	for (vector<cxFold*>::iterator p = foldStack.begin(); p != foldStack.end(); ++p) {
		if ((*p)->type == cxFOLD_START_FOLDED) {
			(*p)->type = cxFOLD_START;
			(*p)->count = 0;
		}
	}
}

void EditorCtrl::UnFoldAll() {
	for (vector<cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p) {
		if (p->type == cxFOLD_START_FOLDED) {
			p->type = cxFOLD_START;
			p->count = 0;
		}
	}
}

void EditorCtrl::ToggleFold() {
	const unsigned int line_id = m_lines.GetCurrentLine();
	vector<cxFold*> foldStack = GetFoldStack(line_id);

	if (!foldStack.empty()) {
		const cxFold* f = foldStack.back();
		if (f->type == cxFOLD_START_FOLDED) UnFold(f->line_id);
		else Fold(f->line_id);
	}
}

void EditorCtrl::SelectFold() {
	const unsigned int line_id = m_lines.GetCurrentLine();
	SelectFold(line_id);
}

void EditorCtrl::SelectFold(unsigned int line_id) {
	wxASSERT(line_id < m_lines.GetLineCount());

	vector<cxFold*> foldStack = GetFoldStack(line_id);
	if (!foldStack.empty()) {
		// If current fold is folded, we want parent if possible
		while (foldStack.size() > 1 && foldStack.back()->type == cxFOLD_START_FOLDED) foldStack.pop_back();

		// Find start of fold
		vector<cxFold>::iterator p = m_folds.begin() + (foldStack.back() - &*m_folds.begin()); // convert pointer to iterator
		const unsigned int fold_start = m_lines.GetLineStartpos(p->line_id);

		// Find the end of fold
		const unsigned int lastline = GetLastLineInFold(foldStack);
		unsigned int lastposinfold = m_lines.GetLineEndpos(lastline, false);

		// Select the entire fold
		m_lines.RemoveAllSelections();
		m_currentSel = m_lines.AddSelection(fold_start, lastposinfold);
		m_lines.SetPos(lastposinfold);
	}
}

bool EditorCtrl::IsLineFolded(unsigned int line_id) const {
	wxASSERT(line_id < m_lines.GetLineCount());

	const cxFold target(line_id);
	vector<cxFold>::const_iterator p = lower_bound(m_folds.begin(), m_folds.end(), target);
	return p != m_folds.end() && p->line_id == line_id && p->type == cxFOLD_START_FOLDED;
}

bool EditorCtrl::IsPosInFold(unsigned int pos, unsigned int* fold_start, unsigned int* fold_end) {
	wxASSERT(pos <= GetLength());
	const unsigned int line_id = m_lines.GetLineFromCharPos(pos);

	vector<EditorCtrl::cxFold>::const_iterator p = m_folds.begin();
	while (p != m_folds.end()) {
		if (p->type == EditorCtrl::cxFOLD_START_FOLDED) {
			// Check if we have passed pos
			const unsigned int line_end = m_lines.GetLineEndpos(p->line_id, true);
			if (line_end >= pos) return false;

			// Check if we are in fold
			const unsigned int lastline = p->line_id + p->count;
			if (line_id <= lastline) {
				if (fold_start) *fold_start = line_end;
				if (fold_end) *fold_end = m_lines.GetLineEndpos(lastline, false);
				return true;
			}

			// Advance to end of fold
			while (p != m_folds.end() && p->line_id <= lastline) ++p;
		}
		else ++p;
	}

	return false;
}

vector<EditorCtrl::cxFold*> EditorCtrl::GetFoldStack(unsigned int line_id) {
	vector<cxFold*> foldStack;
	if (m_lines.isLineVirtual(line_id)) return foldStack;

	wxASSERT(line_id < m_foldLineCount);

	for (vector<EditorCtrl::cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p) {

		if (p->type == EditorCtrl::cxFOLD_END) {
			// Check if end marker matches any starter on the stack (ignore unmatched)
			for (vector<cxFold*>::reverse_iterator f = foldStack.rbegin(); f != foldStack.rend(); ++f) {
				if (p->indent == (*f)->indent) {
					// Check if pos is in this fold
					if (line_id < p->line_id) return foldStack;
					else if (line_id == p->line_id) {
						foldStack.erase(f.base(), foldStack.end());
						return foldStack;
					}
					else foldStack.erase((++f).base(), foldStack.end()); // pop
					break;
				}
			}
		}
		else {
			if (line_id < p->line_id) break; // we have passed line

			// Entering fold
			foldStack.push_back(&*p);

			if (p->line_id == line_id) break; // in fold starter
		}
	}

	return foldStack;
}

void EditorCtrl::ToogleBookmarkOnCurrentLine() {
	const unsigned line_id = m_lines.GetCurrentLine();
	AddBookmark(line_id, true /*toggle*/);
}

void EditorCtrl::AddBookmark(unsigned int line_id, bool toggle) {
	wxASSERT(line_id < m_lines.GetLineCount());

	// Find insert position
	vector<cxBookmark>::iterator p = m_bookmarks.begin();
	while (p != m_bookmarks.end()) {
		if (p->line_id == line_id) {
			if (toggle) m_bookmarks.erase(p);
			return; // already added
		}
		else if (p->line_id > line_id) break;
		++p;
	}

	// Insert the new bookmark
	const cxBookmark bm = {line_id, m_lines.GetLineStartpos(line_id), m_lines.GetLineEndpos(line_id)};
	m_bookmarks.insert(p, bm);
}

void EditorCtrl::DeleteBookmark(unsigned int line_id) {
	for (vector<cxBookmark>::iterator p = m_bookmarks.begin(); p != m_bookmarks.end(); ++p) {
		if (p->line_id == line_id) {
			m_bookmarks.erase(p);
			return;
		}
		else if (p->line_id > line_id) {
			wxASSERT(false); // no bookmark on line
			return;
		}
	}
}

void EditorCtrl::BookmarksInsert(unsigned int pos, unsigned int length) {
	for (vector<cxBookmark>::iterator p = m_bookmarks.begin(); p != m_bookmarks.end(); ++p) {
		if (p->start > pos) {
			p->start += length;
			p->line_id = m_lines.GetLineFromStartPos(p->start);
			p->end = m_lines.GetLineEndpos(p->line_id);
		}
		else if (p->end > pos) {
			p->end = m_lines.GetLineEndpos(p->line_id);
		}
	}
}

void EditorCtrl::BookmarksDelete(unsigned int start, unsigned int end) {
	const unsigned int len = end - start;

	vector<cxBookmark>::iterator p = m_bookmarks.begin();
	while (p != m_bookmarks.end()) {
		if (p->end > start) { // ignore bookmarks before deletion
			if ((start == p->start && end >= p->end) || (start < p->start && end >= p->start)) {
				p = m_bookmarks.erase(p);
				continue;
			}
			else if (start >= p->start) {
				p->end = m_lines.GetLineEndpos(p->line_id);
			}
			else {
				p->start -= len;
				p->line_id = m_lines.GetLineFromStartPos(p->start);
				p->end = m_lines.GetLineEndpos(p->line_id);
			}
		}

		++p;
	}
}

void EditorCtrl::BookmarksApplyDiff(const vector<cxChange>& changes) {
	if (changes.empty()) return;

	// Check if all has been deleted
	if (m_lines.IsEmpty()) {
		m_bookmarks.clear();
		return;
	}

	// Adjust all start positions and remove deleted lines
	int offset = 0;
	for (vector<cxChange>::const_iterator ch = changes.begin(); ch != changes.end(); ++ch) {
		const unsigned int len = ch->end - ch->start;
		if (ch->type == cxINSERTION) {
			for (vector<cxBookmark>::iterator p = m_bookmarks.begin(); p != m_bookmarks.end(); ++p) {
				if (p->start >= ch->start) p->start += len;
			}
			offset += len;
		}
		else { // ch->type == cxDELETION
			const unsigned int start = ch->start + offset;
			const unsigned int end = ch->end + offset;

			vector<cxBookmark>::iterator p = m_bookmarks.begin();
			while (p != m_bookmarks.end()) {
				if ((p->start > start && p->start <= end) || (p->start == start && ch->lines > 0)) {
					p = m_bookmarks.erase(p);
					continue;
				}
				else if (p->start > end) p->start -= len;
				++p;
			}
			offset -= len;
		}
	}

	// Update line numbers and ends
	for (vector<cxBookmark>::iterator p = m_bookmarks.begin(); p != m_bookmarks.end(); ++p) {
		p->line_id = m_lines.GetLineFromStartPos(p->start);
		p->end = m_lines.GetLineEndpos(p->line_id);
	}
}

void EditorCtrl::GotoNextBookmark() {
	if (m_bookmarks.empty()) return;

	// Find first bookmark after current line
	const unsigned int line_id = m_lines.GetCurrentLine();
	vector<cxBookmark>::iterator p = m_bookmarks.begin();
	for (; p != m_bookmarks.end(); ++p) {
		if (p->line_id > line_id) break;
	}
	if (p == m_bookmarks.end()) p = m_bookmarks.begin(); // restart from top

	// Goto bookmark
	m_lines.SetPos(m_lines.GetLineStartpos(p->line_id));
	MakeCaretVisible();
	DrawLayout();
}

void EditorCtrl::GotoPrevBookmark() {
	if (m_bookmarks.empty()) return;

	// Find first bookmark before current line
	const unsigned int line_id = m_lines.GetCurrentLine();
	vector<cxBookmark>::reverse_iterator p = m_bookmarks.rbegin();
	for (; p != m_bookmarks.rend(); ++p) {
		if (p->line_id < line_id) break;
	}
	if (p == m_bookmarks.rend()) p = m_bookmarks.rbegin(); // restart from bottom

	// Goto bookmark
	m_lines.SetPos(m_lines.GetLineStartpos(p->line_id));
	MakeCaretVisible();
	DrawLayout();
}

// ------ TextTip -----------------------------------------

BEGIN_EVENT_TABLE(EditorCtrl::TextTip, wxPopupTransientWindow)
	EVT_KEY_DOWN(EditorCtrl::TextTip::OnKeyDown)
	EVT_KEY_UP(EditorCtrl::TextTip::OnKeyDown)
	EVT_CHAR(EditorCtrl::TextTip::OnKeyDown)
	EVT_LEFT_DOWN(EditorCtrl::TextTip::OnMouseClick)
    EVT_RIGHT_DOWN(EditorCtrl::TextTip::OnMouseClick)
    EVT_MIDDLE_DOWN(EditorCtrl::TextTip::OnMouseClick)
	EVT_MOUSE_CAPTURE_LOST(EditorCtrl::TextTip::OnCaptureLost)
END_EVENT_TABLE()

EditorCtrl::TextTip::TextTip(wxWindow *parent, const wxString& text, const wxPoint& pos, const wxSize& size, TextTip** windowPtr) : m_windowPtr(windowPtr) {
	Create(parent, wxSIMPLE_BORDER);

	wxLogDebug(wxT("TextTip::TextTip %x"), this);

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

	wxStaticText* t = new wxStaticText(this, -1, text);
	t->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	t->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(t, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

	SetSizerAndFit(sizer);
	Layout();
	Position(pos, size);
	Popup();
	SetFocus();
}

EditorCtrl::TextTip::~TextTip() {
	if ( m_windowPtr )
    {
        *m_windowPtr = NULL;
    }
}

void EditorCtrl::TextTip::OnKeyDown(wxKeyEvent& WXUNUSED(event)) {
	Close();
	//if ( !wxPendingDelete.Member(this) ) DismissAndNotify();

	// Pass event on to parent so we don't miss a key entry
	//GetParent()->ProcessEvent(event);
}

void EditorCtrl::TextTip::OnMouseClick(wxMouseEvent& WXUNUSED(event)) {
    Close();
}

void EditorCtrl::TextTip::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event)) {
	// WORKAROUND: This event is not caught in parent class
	wxLogDebug(wxT("OnCaptureLost"));
	//if ( !wxPendingDelete.Member(this) ) DismissAndNotify();
	Close();
}

void EditorCtrl::TextTip::OnDismiss() {
    Close();
	// We can't destroy the window immediately as we might
	// be in the middle of handling some other event.
    //if ( !wxPendingDelete.Member(this) )
		// delayed destruction: the window will be deleted
		// during the next idle loop iteration
        //wxPendingDelete.Append(this);
}

void EditorCtrl::TextTip::Close() {
	wxLogDebug(wxT("TextTip::Close() %x"), this);
	if (HasCapture()) ReleaseMouse();
	Show(false);
    Destroy();
}

bool EditorCtrl::TextTip::OnPreKeyDown(wxKeyEvent& WXUNUSED(event)) {
	Close();
	return true;
}


#ifdef __WXDEBUG__

void EditorCtrl::TestMilestones() {
	//Clear();

	// Initialize random seed
	srand( time(NULL) );

	// We only insert ascii chars so we don't have to worry about
	// utf-8
	wxString text;
	vector<wxChar> numchars;
	for (unsigned int c = 0; c < 10; ++c) numchars.push_back(wxT('0')+c);

	for (unsigned int i = 0; i < 10000; ++i) {
		const unsigned int randtest = rand() % 10;

		if (randtest < 5) { // insert
			const unsigned int insnum = rand() % 10;
			const wxString instext(numchars[insnum], insnum);

			const unsigned int inspos = rand() % (GetLength()+1);
			SetPos(inspos);
			Insert(instext);
		}
		else if (randtest < 7) { // delete
			const unsigned int len = GetLength();
			if (len) {
				const unsigned int delpos = rand() % len;
				const unsigned int del_len = rand() % 10;

				if (delpos + del_len <= len) {
					Delete(delpos, delpos + del_len);
				}
			}
		}
		else if (randtest < 8) { // freeze
			Freeze();
		}
		else if (randtest < 9) { // jump draft revision
			doc_id di;
			cxLOCKDOC_READ(m_doc)
				const unsigned int count = doc.GetVersionCount();
				di = doc.GetDocument();
				di.version_id = rand() % count;
			cxENDLOCK

			SetDocument(di);
		}
		else if (randtest < 10) { // milestone
			text = GetText();

			wxString msg = wxString::Format(wxT("Commit: %d"), i);
			Commit(wxT(""), msg);

			const wxString newtext = GetText();
			wxASSERT(text == newtext);
		}

		DrawLayout();
	}
}

#endif //__WXDEBUG__

// -- FrameDropTarget -----------------------------------------------------------------

EditorCtrl::DragDropTarget::DragDropTarget(EditorCtrl& parent) : m_parent(parent) {
	m_fileObject = new wxFileDataObject;
	m_textObject = new wxTextDataObject;
	m_columnObject = new MultilineDataObject;
	m_dataObject = new wxDataObjectComposite;

	// Specify the type of data we will accept
	m_dataObject->Add(m_columnObject, true /*preferred*/); // WORKAROUND: has to be first as wxDropTarget ignores preferred
	m_dataObject->Add(m_fileObject);
	m_dataObject->Add(m_textObject);
	SetDataObject(m_dataObject);
}

wxDragResult EditorCtrl::DragDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
	m_parent.OnDragOver(x, y);
	return def;
}

wxDragResult EditorCtrl::DragDropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def) {
	// Copy the data from the drag source to our data object
	GetData();

	const wxDataFormat df = m_dataObject->GetReceivedFormat();
	if (df == wxDF_TEXT || df == wxDF_UNICODETEXT) {
		m_parent.OnDragDropText(m_textObject->GetText(), def);
	}
    else if (df == wxDF_FILENAME) {
		m_parent.OnDragDrop(m_fileObject->GetFilenames());
	}
	else if (df == wxDataFormat(MultilineDataObject::s_formatId)) {
		wxArrayString text;
		m_columnObject->GetText(text);
		m_parent.OnDragDropColumn(text, def);
	}

	return def;
}
