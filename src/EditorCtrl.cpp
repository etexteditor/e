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

#include <wx/clipbrd.h>
#include <wx/filename.h>
#include <wx/tipwin.h>
#include <wx/file.h>

#include <set>
#include <algorithm>

#include "pcre.h"

#include "doc_byte_iter.h"
#include "tm_syntaxhandler.h"
#include "EditorFrame.h"
#include "StyleRun.h"
#include "FindCmdDlg.h"
#include "RunCmdDlg.h"
#include "BundleMenu.h"
#include "GutterCtrl.h"
#include "PreviewDlg.h"
#include "RedoDlg.h"
#include "CompletionPopup.h"
#include "MultilineDataObject.h"
#include "eDocumentPath.h"
#include "ShellRunner.h"
#include "Env.h"
#include "Fold.h"
#include "TextTip.h"
#include "RemoteThread.h"
#include "LiveCaret.h"
#include "eSettings.h"
#include "IAppPaths.h"
#include "Strings.h"
#include "ReplaceStringParser.h"

// Document Icons
#include "document.xpm"

// Generates a string appropriate for a Drag Command's TM_MODIFIER_FLAGS
// Note that we remap Windows modifier keys to the equivalents expected by TextMate commands.
static wxString get_modifier_key_env() {
	wxString modifiers;
	if (wxGetKeyState(WXK_SHIFT))
		modifiers += wxT("SHIFT");

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

	return modifiers;
}

inline bool Isalnum(wxChar c) {
#ifdef __WXMSW__
	return ::IsCharAlphaNumeric(c) != 0;
#else
	return wxIsalnum(c);
#endif
}

// Embedded class: Sort list based on bundle
class CompareActionBundle : public binary_function<size_t, size_t, bool> {
public:
	CompareActionBundle(const vector<const tmAction*>& actionList) : m_list(actionList) {};
	bool operator() (const size_t a1, const size_t a2) const {
		return m_list[a1]->bundle > m_list[a2]->bundle;
	}
private:
	 const vector<const tmAction*>& m_list;
};

class DragDropTarget : public wxDropTarget {
public:
	DragDropTarget(EditorCtrl& parent);
	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
	wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
		m_parent.OnDragOver(x, y);
		return def;
	}

private:
	EditorCtrl& m_parent;
	wxFileDataObject* m_fileObject;
	wxTextDataObject* m_textObject;
	MultilineDataObject* m_columnObject;
	wxDataObjectComposite* m_dataObject;
};


enum ShellOutput {soDISCARD, soREPLACESEL, soREPLACEDOC, soINSERT, soSNIPPET, soHTML, soTOOLTIP, soNEWDOC};

enum EditorCtrl_IDs {
	TIMER_FOLDTOOLTIP = 100,
	ID_LEFTSCROLL
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
	EVT_COMMAND_SCROLL(ID_LEFTSCROLL, EditorCtrl::OnScrollBar)
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

/// Open a page saved from a previous session
EditorCtrl::EditorCtrl(const int page_id, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame) : 
	m_catalyst(cw),
	m_doc(cw),
	dispatcher(cw.GetDispatcher()),

	bitmap(bitmap), 
	m_parentFrame(parentFrame), 

	m_syntaxHandler(m_parentFrame.GetSyntaxHandler()),
	m_theme(m_syntaxHandler.GetTheme()),
	m_lines(mdc, m_doc, *this, m_theme),

	m_search_hl_styler(m_doc, m_lines, m_searchRanges, m_cursors, m_theme),
	m_syntaxstyler(m_doc, m_lines, &m_syntaxHandler),

	m_foldTooltipTimer(this, TIMER_FOLDTOOLTIP),
	m_activeTooltip(NULL),

	m_beforeRedrawCallback(NULL),
	m_afterRedrawCallback(NULL), 
	m_scrollCallback(NULL), 

	m_enableDrawing(false), 
	m_isResizing(true),
	scrollPos(0), 
	m_scrollPosX(0), 
	topline(-1), 
	commandMode(false),
	m_changeToken(0), 
	m_savedForPreview(false), 
	lastpos(0), 
	m_currentSel(-1), 
	do_freeze(true), 
	m_options_cache(0), 
	m_re(NULL), 
	m_symbolCacheToken(0),

	bookmarks(m_lines),
	m_commandHandler(parentFrame, *this),
	m_macro(parentFrame.GetMacro())
{
	Create(parent, wxID_ANY, wxPoint(-100,-100), wxDefaultSize, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	Hide(); // start hidden to avoid flicker
	Init();

	eFrameSettings& settings = parentFrame.GetFrameSettings();
	RestoreSettings(page_id, settings);
}

/// Open a document
EditorCtrl::EditorCtrl(const doc_id di, const wxString& mirrorPath, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size):
	m_catalyst(cw),
	m_doc(cw),
	dispatcher(cw.GetDispatcher()),

	bitmap(bitmap), 
	m_parentFrame(parentFrame),

	m_syntaxHandler(m_parentFrame.GetSyntaxHandler()),
	m_theme(m_syntaxHandler.GetTheme()),
	m_lines(mdc, m_doc, *this, m_theme),
	
	m_search_hl_styler(m_doc, m_lines, m_searchRanges, m_cursors, m_theme),
	m_syntaxstyler(m_doc, m_lines, &m_syntaxHandler),

	m_foldTooltipTimer(this, TIMER_FOLDTOOLTIP),
	m_activeTooltip(NULL),

	m_beforeRedrawCallback(NULL),
	m_afterRedrawCallback(NULL),
	m_scrollCallback(NULL),

	m_enableDrawing(false), 
	m_isResizing(true),
	scrollPos(0),
	m_scrollPosX(0),
	topline(-1),
	commandMode(false), 
	m_changeToken(0),
	m_savedForPreview(false),
	lastpos(0),
	m_currentSel(-1), 
	do_freeze(true),
	m_options_cache(0),
	m_re(NULL),
	m_symbolCacheToken(0),

	bookmarks(m_lines),
	m_commandHandler(parentFrame, *this),
	m_macro(parentFrame.GetMacro())

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
	FoldingClear();
}


/// Create a new empty document
EditorCtrl::EditorCtrl(CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size):
	m_catalyst(cw),
	m_doc(cw, true), 
	dispatcher(cw.GetDispatcher()), 

	bitmap(bitmap), 
	m_parentFrame(parentFrame), 

	m_syntaxHandler(m_parentFrame.GetSyntaxHandler()), 
	m_theme(m_syntaxHandler.GetTheme()),
	m_lines(mdc, m_doc, *this, m_theme), 

	m_search_hl_styler(m_doc, m_lines, m_searchRanges, m_cursors, m_theme),
	m_syntaxstyler(m_doc, m_lines, &m_syntaxHandler),

	m_foldTooltipTimer(this, TIMER_FOLDTOOLTIP), 
	m_activeTooltip(NULL),

	m_beforeRedrawCallback(NULL),
	m_afterRedrawCallback(NULL), 
	m_scrollCallback(NULL), 

	m_enableDrawing(false), 
	m_isResizing(true),
	scrollPos(0), 
	m_scrollPosX(0), 
	topline(-1), 
	commandMode(false), 
	m_changeToken(0), 
	m_savedForPreview(false), 
	lastpos(0), 
	m_currentSel(-1), 
	do_freeze(true), 
	m_options_cache(0), 
	m_re(NULL), 
	m_symbolCacheToken(0),

	bookmarks(m_lines),
	m_commandHandler(parentFrame, *this),
	m_macro(parentFrame.GetMacro())
{
	Create(parent, wxID_ANY, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	Hide(); // start hidden to avoid flicker

	cxLOCKDOC_WRITE(m_doc)
		// Make sure we start out with a single empty revision
		doc.Freeze();
	cxENDLOCK

	Init();

	// Init Folding
	FoldingClear();
}

void EditorCtrl::RestoreSettings(unsigned int page_id, eFrameSettings& settings, unsigned int subid) {
	wxString mirrorPath;
	doc_id di;
	int newpos;
	int topline;
	wxString syntax;
	vector<unsigned int> folds;
	vector<unsigned int> bookmarks;

	// Retrieve the page info
	wxASSERT(0 <= page_id && page_id < (int)settings.GetPageCount());
	settings.GetPageSettings(page_id, mirrorPath, di, newpos, topline, syntax, folds, bookmarks, (SubPage)subid);

	if (eDocumentPath::IsRemotePath(mirrorPath)) {
		// If the mirror points to a remote file, we have to download it first.
		SetDocument(di, mirrorPath);
	}
	else {
		const bool isBundleItem = eDocumentPath::IsBundlePath(mirrorPath);
		if (isBundleItem) m_remotePath = mirrorPath;
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
		FoldingClear(); // Init Folding
	}
	else SetSyntax(syntax);

	SetDocumentAndScrollPosition(newpos, topline);

	// Fold lines that were folded in previous session
	if (!folds.empty()) {
		// We have to make sure all text is syntaxed and all fold markers found
		m_syntaxstyler.ParseAll();
		UpdateFolds();

		for (vector<unsigned int>::const_iterator p = folds.begin(); p != folds.end(); ++p) {
			const unsigned int line_id = *p;
			const cxFold target(line_id);
			vector<cxFold>::iterator f = lower_bound(m_folds.begin(), m_folds.end(), target);
			if (f != m_folds.end() && f->line_id == line_id && f->type == cxFOLD_START)
				Fold(*p);
		}
	}

	// Set bookmarks
	for (vector<unsigned int>::const_iterator p = bookmarks.begin(); p != bookmarks.end(); ++p)
		AddBookmark(*p);
}

void EditorCtrl::Init() {
	// Initialize the memoryDC for dubblebuffering
	mdc.SetFont(m_theme.font);
	// We do not yet call mdc.SelectObject(bitmap) since the bitmap
	// is shared with the other EditCtrl's. We call it in Show() & OnSize().

	m_remoteProfile = NULL;

	// Column selection state
	m_blockKeyState = BLOCKKEY_NONE;
	m_selMode = SEL_NORMAL;

	// Settings
	bool autopair = true;
	m_doAutoWrap = true;
	m_wrapAtMargin = false;
	bool doShowMargin = false;
	int marginChars = 80;

	eSettings& settings = eGetSettings();
	settings.GetSettingBool(wxT("autoPair"), autopair);
	settings.GetSettingBool(wxT("autoWrap"), m_doAutoWrap);
	settings.GetSettingBool(wxT("showMargin"), doShowMargin);
	settings.GetSettingBool(wxT("wrapMargin"), m_wrapAtMargin);
	settings.GetSettingInt(wxT("marginChars"), marginChars);

	m_autopair.Enable(autopair);

	m_lastScopePos = -1; // scope selection
	if (!doShowMargin) m_wrapAtMargin = false;

	// Initialize gutter (line numbers)
	m_gutterCtrl = new GutterCtrl(*this, wxID_ANY);
	m_gutterCtrl->Hide();
	m_showGutter = false;
	m_gutterLeft = true; // left side is default
	m_gutterWidth = 0;
	SetShowGutter(m_parentFrame.IsGutterShown());

	m_leftScrollbar = NULL; // default is using internal (right) scrollbar
	m_leftScrollWidth = 0;

	// To keep track of when we should freeze versions
	lastpos = 0;
	lastaction = ACTION_INSERT;

	// Initialize tooltips
	//m_revTooltip.Create(this);

	// resize the bitmap used for doublebuffering
	wxSize size = GetClientSize();
	if (bitmap.GetWidth() < size.x || bitmap.GetHeight() < size.y)
		bitmap = wxBitmap(size.x, size.y);

	// Init the lines
	m_lines.SetWordWrap(m_parentFrame.GetWrapMode());
	m_lines.ShowIndent(m_parentFrame.IsIndentShown());
	m_lines.Init();
	//m_lines.AddStyler(m_usersStyler);
	m_lines.AddStyler(m_syntaxstyler);
	m_lines.AddStyler(m_search_hl_styler);

	// Set initial tabsize
	SetTabWidth(m_parentFrame.GetTabWidth(), m_parentFrame.IsSoftTabs());

	// Should we show margin line?
	if (!doShowMargin) marginChars = 0;
	m_lines.ShowMargin(marginChars);

	// Create a caret to indicate edit position
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

	NotifyParentMate();
	ClearRemoteInfo();
}

// Notify mate that we have finished editing document
void EditorCtrl::NotifyParentMate() {
#ifdef __WXMSW__
	if (!m_mate.empty()) {
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
	}
#endif
}

void EditorCtrl::SaveSettings(unsigned int i, eFrameSettings& settings) {
	SaveSettings(i, settings, 0); 
}

void EditorCtrl::SaveSettings(unsigned int i, eFrameSettings& settings, unsigned int subid) {
	const wxString& path = GetPath();
	const doc_id di = GetDocID();
	const int pos = GetPos();
	const int topline = GetTopLine();
	const wxString& syntax = GetSyntaxName();
	const vector<unsigned int> folds = GetFoldedLines();

	settings.SetPageSettings(i, path, di, pos, topline, syntax, folds, bookmarks.GetBookmarks(), (SubPage)subid);
	//wxLogDebug(wxT("  %d (%d,%d,%d) pos:%d topline:%d"), i, di.type, di.document_id, di.version_id, pos, topline);
}

EditorCtrl* EditorCtrl::GetActiveEditor() { return this; }

const char** EditorCtrl::RecommendedIcon() const { return document_xpm; }

void EditorCtrl::CommandModeEnded() {
	ClearSearchRange();
	m_commandHandler.Clear();
}

void EditorCtrl::ClearRemoteInfo() {
	if (m_remotePath.empty()) return;

	m_remotePath.clear();
	m_remoteProfile = NULL;
	if (m_path.IsOk()) // Clean up temp buffer file
		wxRemoveFile(m_path.GetFullPath());
}

unsigned int EditorCtrl::GetLength() const { return m_lines.GetLength(); }
unsigned int EditorCtrl::GetPos() const { return m_lines.GetPos(); }

void EditorCtrl::SetDocumentAndScrollPosition(int pos, int topline) {
	m_lines.SetPos(pos);
	scrollPos = m_lines.GetYPosFromLine(topline);
}

unsigned int EditorCtrl::GetCurrentLineNumber() {
	// Rows count from 1; internally they count from 0.
	return m_lines.GetCurrentLine() +1 ;
}

// Gets the current tab-expanded cursor position, counting from 1.
unsigned int EditorCtrl::GetCurrentColumnNumber() {
	// Start of current line
	const unsigned int line = m_lines.GetCurrentLine();
	const unsigned int linestart = m_lines.GetLineStartpos(line);

	// Cursor position within line
	const unsigned int pos = m_lines.GetPos();

	// Calculate pos with tabs expanded.
	const unsigned int tab_size = this->m_parentFrame.GetTabWidth();
	unsigned int tabpos = 0;
	cxLOCKDOC_READ(m_doc)
		for (doc_byte_iter dbi(doc, linestart); (unsigned int)dbi.GetIndex() < pos; ++dbi) {
			// Only count first byte of UTF-8 multibyte chars
			if ((*dbi & 0xC0) == 0x80) continue;
			if (*dbi == '\t') {
				// Figure out how far to advance to next tab stop
				const int over = tabpos % tab_size;
				tabpos += (tab_size - over);
			}
			else tabpos++;
		}
	cxENDLOCK;

	// Columns are 1-indexed.
	return tabpos + 1;
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

doc_id EditorCtrl::GetLastStableDocID() const {
	cxLOCKDOC_READ(m_doc)
		doc_id di = doc.GetDocument();
		if (doc.IsFrozen()) return di;
		else return doc.GetDraftParent(di.version_id);
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

void EditorCtrl::SetSearchHighlight(const wxString& pattern, int options) {
	m_search_hl_styler.SetSearch(pattern, options);
}

void EditorCtrl::ClearSearchHighlight() {
	m_search_hl_styler.Clear();
	DrawLayout();
}

bool EditorCtrl::IsOk() const {
	wxSize size = GetClientSize();

	// It is ok to have a size 0 window with invalid bitmap
	return (size.x == 0 && size.y == 0) || bitmap.Ok();
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

	if (change_doc_id.SameDoc(di)) {
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

	if (showGutter) m_gutterCtrl->Show();
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

	if (m_wrapAtMargin && wrapMode != cxWRAP_NONE)
		m_lines.SetWidth(m_lines.GetMarginPos(), topline);

	scrollPos = m_lines.GetYPosFromLine(topline);
	DrawLayout();
}

void EditorCtrl::SetTabWidth(unsigned int width, bool soft_tabs) {
	// m_indent is the string used for indentation, either a real tab character
	// or an appropriate number of spaces
	if (soft_tabs) 
		m_indent = wxString(wxT(' '), width);
	else 
		m_indent = wxString(wxT("\t"));

	m_lines.SetTabWidth(width);
	FoldingReIndent();

	MarkAsModified();
}

const wxFont& EditorCtrl::GetEditorFont() const { return mdc.GetFont(); }

void EditorCtrl::SetGutterRight(bool doMove) {
	m_gutterLeft = !doMove;
	m_gutterCtrl->SetGutterRight(doMove);
	DrawLayout();
}

bool EditorCtrl::HasScrollbar() const {
	if (m_leftScrollbar) return m_leftScrollbar->IsShown();
	else return (GetScrollThumb(wxVERTICAL) > 0);
}

void EditorCtrl::SetScrollbarLeft(bool doMove) {
	if (doMove) {
		m_leftScrollbar = new wxScrollBar(this, ID_LEFTSCROLL, wxPoint(0,0), wxDefaultSize, wxSB_VERTICAL);
		m_leftScrollbar->SetCursor(*wxSTANDARD_CURSOR); // Set to standard cursor (otherwise it will inherit from editorCtrl)

		const bool isShown = (GetScrollThumb(wxVERTICAL) > 0);
		if (isShown) {
			m_leftScrollWidth = m_leftScrollbar->GetSize().x;

			SetScrollbar(wxVERTICAL, 0, 0, 0); // hide windows own scrollbar
			return; // removing scrollbar will have sent draw event
		}
	}
	else {
		delete m_leftScrollbar;
		m_leftScrollbar = NULL;
		m_leftScrollWidth = 0;
	}

	m_isResizing = true;
	DrawLayout();
}

//
// Returns true if a scrollbar was added or removed, else false.
//
bool EditorCtrl::UpdateScrollbars(unsigned int x, unsigned int y) {
	const unsigned int height = m_lines.GetHeight();

	// Check if we need a vertical scrollbar
	if (m_leftScrollbar) {
		const int scroll_thumb = m_leftScrollbar->GetThumbSize();
		if (height > y) {
			m_leftScrollbar->Show();
			const int scroll_range = m_leftScrollbar->GetRange();
			if (scroll_thumb != (int)y || scroll_range != (int)height) {
				m_leftScrollbar->SetScrollbar(scrollPos, y, height, y);
				if (scroll_thumb == 0) {
					ReDraw();
					return true; // cancel old redraw
				}
			}

			// Avoid empty space at bottom
			if (scrollPos + y > height) scrollPos = wxMax(0, height - y);

			m_leftScrollbar->SetSize(-1, GetSize().y);
			m_leftScrollbar->SetThumbPosition(scrollPos);
			const unsigned int width = m_leftScrollbar->GetSize().x;
			
			if (m_leftScrollWidth != width) {
				m_leftScrollWidth = width;
				ReDraw();
				return true; // cancel old redraw
			}
		}
		else if (scroll_thumb > 0) {
			m_leftScrollbar->Hide();
			m_leftScrollWidth = 0;
			ReDraw();
			return true; // cancel old redraw
		}
	}
	else {
		const int scroll_thumb = GetScrollThumb(wxVERTICAL);
		if (height > y) {
			const int scroll_range = GetScrollRange(wxVERTICAL);
			if (scroll_thumb != (int)y || scroll_range != (int)height) {
				SetScrollbar(wxVERTICAL, scrollPos, y, height);
				if (scroll_thumb == 0) return true; // Creation of scrollbar have sent a size event
			}

			// Avoid empty space at bottom
			if (scrollPos + y > height) scrollPos = wxMax(0, height - y);

			SetScrollPos(wxVERTICAL, scrollPos);
		}
		else if (scroll_thumb > 0) {
			SetScrollbar(wxVERTICAL, 0, 0, 0);
			return true; // Removal of scrollbar have sent a size event
		}
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

void EditorCtrl::DrawLayout(bool isScrolling) {
	wxClientDC dc(this);
	DrawLayout(dc, isScrolling);
}

void EditorCtrl::DrawLayout(wxDC& dc, bool WXUNUSED(isScrolling)) {
	if (!IsShown() || !m_enableDrawing) return; // No need to draw
	wxLogDebug(wxT("DrawLayout() : %d (%d,%d)"), GetId(), m_enableDrawing, IsShown());
	//wxLogDebug(wxT("DrawLayout() : %s"), GetName());

	if (m_beforeRedrawCallback) m_beforeRedrawCallback(m_callbackData);

	wxASSERT(m_scrollPosX >= 0);

	// Check if we should cancel a snippet
	if (m_snippetHandler.IsActive()) m_snippetHandler.Validate();

	// Get view dimensions
	const wxSize size = GetClientSize();
	if (size.x == 0 || size.y == 0) return; // Nothing to draw

	if (m_showGutter) {
		m_gutterWidth = m_gutterCtrl->CalcLayout(size.y);

		// Move gutter to correct position
		const unsigned int gutterxpos = m_gutterLeft ? m_leftScrollWidth : size.x - m_gutterWidth;
		if (m_gutterCtrl->GetPosition().x != (int)gutterxpos) {
			m_gutterCtrl->SetPosition(wxPoint(gutterxpos, 0));
		}
	}
	const unsigned int editorSizeX = ClientWidthToEditor(size.x);

	// There may only be room for the gutter
	if (editorSizeX == 0) {
		if (m_showGutter) m_gutterCtrl->DrawGutter();
		return;
	}
	
	// Resize the bitmap used for doublebuffering
	if (bitmap.GetWidth() < (int)editorSizeX || bitmap.GetHeight() < size.y) {
		// disassociate and release mem for old bitmap
		mdc.SelectObjectAsSource(wxNullBitmap);
		bitmap = wxNullBitmap;

		// Resize bitmap
		bitmap = wxBitmap(size.x, size.y);
		mdc.SelectObject(bitmap);
	}

	// We always have to reselect the bitmap if it has been resized
	// by another editorCtrl. Otherwise the memorydc will get confused.
	if (mdc.GetSize() != wxSize(bitmap.GetWidth(), bitmap.GetHeight())) {
		mdc.SelectObject(bitmap);
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
	if (!(m_parentFrame.IsSearching() || m_commandHandler.IsSearching()))
		m_search_hl_styler.Clear(); // Only highlight search terms during search

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
	const unsigned int xpos = m_leftScrollWidth + (m_gutterLeft ? m_gutterWidth : 0);
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
	if (m_isResizing || scrollPos != old_scrollPos) {
		if (m_scrollCallback) m_scrollCallback(m_callbackData);
		old_scrollPos = scrollPos;
	}
	if (m_afterRedrawCallback) m_afterRedrawCallback(m_callbackData);

	m_isResizing = false;
}

unsigned int EditorCtrl::ClientWidthToEditor(unsigned int width) const {
	return (width > m_gutterWidth) ? (width - (m_gutterWidth + m_leftScrollWidth)) : 0;
}

wxPoint EditorCtrl::ClientPosToEditor(unsigned int xpos, unsigned int ypos) const {
	unsigned int adjXpos = (xpos - m_leftScrollWidth) + m_scrollPosX;
	if (m_gutterLeft) adjXpos -= m_gutterWidth;

	return wxPoint(adjXpos, ypos + scrollPos);
}

wxPoint EditorCtrl::EditorPosToClient(unsigned int xpos, unsigned int ypos) const {
	unsigned int adjXpos = (xpos - m_scrollPosX) + m_leftScrollWidth;
	if (m_gutterLeft) adjXpos += m_gutterWidth;

	return wxPoint(adjXpos, ypos - scrollPos);
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
				if (res.error_code > 0)
					continue;
			}

			// Check if indentation should increase
			const wxString& increasePattern = m_syntaxHandler.GetIndentIncreasePattern(scope);
			if (!increasePattern.empty()) {
				const search_result res = RegExFind(increasePattern, linestart, false, NULL, lineend);
				if (res.error_code > 0) 
					return m_lines.GetLineIndent(i) + m_indent;
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
					//		if (res.error_code > 0) return m_lines.GetLineIndent(i+1);
					//	}
					//}
					return GetRealIndent(i+1); //+ m_indent;
				}
			}*/
		}

		// If we get to here we should just use the same indent as the line above
		break;
	}

	return m_lines.GetLineIndent( (i == -1) ? 0 : i );
}

wxString EditorCtrl::GetRealIndent(unsigned int lineid, bool newline) {
	if (lineid == 0) return m_lines.GetLineIndent(0);
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

	indent += m_lines.GetLineIndent(validLine);

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

// Used by SnippetHandler
wxString EditorCtrl::GetLineIndentFromPos(unsigned int pos) {
	wxASSERT(pos <= GetLength());

	const unsigned int lineid = m_lines.GetLineFromCharPos(pos);
	return m_lines.GetLineIndent(lineid);
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
		return true;
	cxENDLOCK
}

unsigned int EditorCtrl::CountMatchingChars(wxChar match, unsigned int start, unsigned int end) const {
	wxASSERT(start <= end && end <= m_lines.GetLength());
	if (start == end) return 0;

	const wxString text = GetText(start, end);
	unsigned int count = 0;

	for (unsigned int i = 0; i < text.size(); ++i)
		if (text[i] == match) ++count;

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
	if (start == end) return;
	const unsigned int len = end - start;
	text.resize(len);

	cxLOCKDOC_READ(m_doc)
		doc.GetTextPart(start, end, (unsigned char*)&*text.begin());
	cxENDLOCK
}

void EditorCtrl::GetLine(unsigned int lineid, vector<char>& text) const {
	unsigned int start;
	unsigned int end;
	m_lines.GetLineExtent(lineid, start, end);

	GetTextPart(start, end, text);
}

void EditorCtrl::Tab() {
	const bool shiftDown = wxGetKeyState(WXK_SHIFT);
	const bool isMultiSel = m_lines.IsSelectionMultiline() && !m_lines.IsSelectionShadow();

	// If there are multiple lines selected then tab triggers indentation
	if (m_snippetHandler.IsActive()) {
		if (shiftDown) {
			if (m_macro.IsRecording()) m_macro.Add(wxT("PrevSnippetField"));
			m_snippetHandler.PrevTab();
		}
		else {
			if (m_macro.IsRecording()) m_macro.Add(wxT("NextSnippetField"));
			m_snippetHandler.NextTab();
		}
		return;
	}

	if (shiftDown || isMultiSel) {
		if (lastaction != ACTION_NONE || lastpos != GetPos()) Freeze();
		const bool addIndent = !shiftDown;
		if (m_macro.IsRecording()) {
			if (addIndent) m_macro.Add(wxT("IndentSelectedLines"));
			else m_macro.Add(wxT("DedentSelectedLines"));
		}

		IndentSelectedLines(addIndent);

		lastpos = GetPos();
		lastaction = ACTION_NONE;

		return;
	}

	if (m_macro.IsRecording()) m_macro.Add(wxT("Tab"));

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
	if (!m_parentFrame.IsSoftTabs()) {	// Hard Tab
		InsertChar(wxChar('\t'));
		return;
	}

	// Soft Tab (incremental number of spaces)
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

bool EditorCtrl::DoTabTrigger(unsigned int wordstart, unsigned int wordend) {
	wxASSERT(wordstart < wordend && wordend <= m_lines.GetLength());

	wxString trigger;
	cxLOCKDOC_READ(m_doc)
		trigger = doc.GetTextPart(wordstart, wordend);
	cxENDLOCK

	const deque<const wxString*> scope = m_syntaxstyler.GetScope(wordend);
	const vector<const tmAction*> actions = m_syntaxHandler.GetActions(trigger, scope);

	if (actions.empty()) return false; // no action found for trigger

	//wxLogDebug(wxT("%s (%u)"), trigger, snippets.size());
	//wxLogDebug(wxT("%s"), actions[0]->content);

	// Present user with a list of actions
	int actionIndex = 0;
	if (actions.size() > 1) {
		actionIndex = ShowPopupList(actions);
		if (actionIndex == -1)
			return true;
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

void EditorCtrl::FilterThroughCommand() {
	RunCmdDlg dlg(this, eGetSettings());
	if (dlg.ShowModal() == wxID_OK) {
		const tmCommand cmd = dlg.GetCommand();

		if (m_macro.IsRecording()) {
			eMacroCmd& mc = m_macro.Add(wxT("FilterThroughCommand"));
			mc.AddArg(wxT("command"), cmd.name);
			switch (cmd.input) {
				case tmCommand::ciNONE: mc.AddArg(wxT("input"), wxT("none")); break;
				case tmCommand::ciSEL:  mc.AddArg(wxT("input"), wxT("selection")); break;
				case tmCommand::ciDOC:  mc.AddArg(wxT("input"), wxT("document")); break;
			};

			switch (cmd.output) {
				case tmCommand::coNONE:    mc.AddArg(wxT("output"), wxT("discard")); break;
				case tmCommand::coSEL:     mc.AddArg(wxT("output"), wxT("replaceSelectedText")); break;
				case tmCommand::coDOC:     mc.AddArg(wxT("output"), wxT("replaceDocument")); break;
				case tmCommand::coINSERT:  mc.AddArg(wxT("output"), wxT("afterSelectedText")); break;
				case tmCommand::coSNIPPET: mc.AddArg(wxT("output"), wxT("insertAsSnippet")); break;
				case tmCommand::coHTML:    mc.AddArg(wxT("output"), wxT("showAsHTML")); break;
				case tmCommand::coTOOLTIP: mc.AddArg(wxT("output"), wxT("showAsTooltip")); break;
				case tmCommand::coNEWDOC:  mc.AddArg(wxT("output"), wxT("openAsNewDocument")); break;
			}
		}

		DoAction(cmd, NULL, false);
	}
}

void EditorCtrl::DoActionFromDlg() {
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(GetPos());
	vector<const tmAction*> actions;
	m_syntaxHandler.GetAllActions(scope, actions);

	FindCmdDlg dlg(this, actions);
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
	cxEnv env;
	SetEnv(env, action.isUnix, action.bundle);
	if (envVars) env.SetEnv(*envVars);

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
		if (action.isUnix && !eDocumentPath::InitCygwin()) return;
		#endif // __WXMSW__

		const tmCommand* cmd = (tmCommand*)&action;

		// beforeRunningCommand
		if (cmd->save == tmCommand::csDOC && IsModified()) {
			if (!SaveText()) return;
			m_parentFrame.UpdateWindowTitle(); // update tabs
		}
		else if (cmd->save == tmCommand::csALL) {
			if (m_parentFrame.HasProject()) {
				// Ask parent to save all modified docs
				m_parentFrame.SaveAllFilesInProject();
			}
			else {
				if (!SaveText()) return;
				m_parentFrame.UpdateWindowTitle(); // update tabs
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
			{
				const interval* const sel = m_lines.FirstSelection();
				if (sel) sel->Get(selStart, selEnd);
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
			wxBusyCursor wait; // Show busy cursor while running command.
			pid = ShellRunner::RawShell(cmdContent, input, &output, &errout, env, action.isUnix, cwd);
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
					const interval* const sel = m_lines.FirstSelection();
					if (sel) sel->Get(selStart, selEnd);
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

						if (!output.empty())
							m_snippetHandler.StartSnippet(this, output, env, action.bundle);
					}
					break;

				case tmCommand::coHTML:
					// Only show stderr in HTML window if there is no other output
					m_parentFrame.ShowOutput(cmd->name, !shellout.empty() ? shellout : shellout);
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

					if (mDate != modDate)
						LoadText(m_path.GetFullPath());
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

	m_autopair.ClearIfInsertingOutsideInnerPair(pos);

	wxString autoPair;
	if (doSmartType) {
		// Check if we are inserting at end of inner pair
		if (m_autopair.HasPairs()) { // we must be at end
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
				m_autopair.DropInnerPair();
				return 0;
			}
		}

		if (text.length() == 1)
			autoPair = AutoPair(pos, text);
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

		if (autoPair.empty()) m_autopair.AdjustEndsUp(byte_len); // Adjust containing pairs
		else {
			// insert paired char
			unsigned int pair_len = 0;
			cxLOCKDOC_WRITE(m_doc)
				pair_len = doc.Insert(pairpos, autoPair);
			cxENDLOCK
			m_lines.Insert(pairpos, pair_len); // Update the view
			StylersInsert(pairpos, pair_len);  // Update stylers
			byte_len += pair_len;
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
		for (map<wxString, wxString>::const_iterator i = smartPairs.begin(); i != smartPairs.end(); ++i)
			wxLogDebug(wxT("%s -> %s"), i->first.c_str(), i->second.c_str());
	}
#endif

	if (p == smartPairs.end()) return wxEmptyString;
	return p->second;
}

wxString EditorCtrl::AutoPair(unsigned int pos, const wxString& text, bool addToStack) {
	wxASSERT(!text.empty());

	if (!m_autopair.Enabled()) return wxEmptyString;

	// Are we just before a pair end?
	bool inPair = m_autopair.AtEndOfPair(pos);

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
		m_autopair.AdjustEndsUp(byte_len);

		const unsigned int pairPos = pos + starter_len;
		m_autopair.AddInnerPair(pairPos);
	}

	return pairEnd;
}

void EditorCtrl::GotoMatchingBracket() {
	unsigned int pos = m_lines.GetPos();
	const unsigned int len = m_lines.GetLength();
	if (pos == len) return;

	// Get list of brackets
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(pos);
	const map<wxString, wxString> smartPairs = m_syntaxHandler.GetSmartTypingPairs(scope);

	// Build a set of end brackets
	set<wxString> endBrackets;
	for (map<wxString, wxString>::const_iterator p2 = smartPairs.begin(); p2 != smartPairs.end(); ++p2) {
		endBrackets.insert(p2->second);
	}

	// Advance until we hit first start or end bracket
	bool bracketFound = false;
	cxLOCKDOC_READ(m_doc)
		for (; pos < len; pos = doc.GetNextCharPos(pos)) {
			const wxChar c = doc.GetChar(pos);

			// Check if current char is a start bracket
			const map<wxString, wxString>::const_iterator p = smartPairs.find(c);
			if (p != smartPairs.end()) {
				bracketFound = true;
				break;
			}

			// Check if current char is an end bracket
			const set<wxString>::const_iterator p2 = endBrackets.find(c);
			if (p2 != endBrackets.end()) {
				bracketFound = true;
				break;
			}
		}
	cxENDLOCK
	if (!bracketFound) return;

	// Move cursor to the matching bracket
	unsigned int pos2;
	if (!FindMatchingBracket(pos, pos2)) return;
	m_lines.SetPos(pos2);
}

bool EditorCtrl::FindMatchingBracket(unsigned int pos, unsigned int& pos2) {
	// Get list of brackets
	const deque<const wxString*> scope = m_syntaxstyler.GetScope(pos);
	const map<wxString, wxString> smartPairs = m_syntaxHandler.GetSmartTypingPairs(scope);

	// Get current char
	wxChar c;
	cxLOCKDOC_READ(m_doc)
		c = doc.GetChar(pos);
	cxENDLOCK

	bool searchForward = true;
	int limit = m_lines.GetLength();
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
				if (escaped) return false; // current char is escaped
			cxENDLOCK

			if (count & 1) {
				pos2 = bracketpos;
				return true;
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
								pos2 = dbi.GetIndex();
								return true;
							}
						}
					}
				cxENDLOCK
			}

			return false;
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
		if (!bracketFound) return false; // no bracket at pos
	}

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
							pos2 = dbi.GetIndex();
							return true;
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
							pos2 = dbi.GetIndex();
							return true;
						}
					}
				}

				--dbi;
			}
		cxENDLOCK
	}
	return false;
}

void EditorCtrl::MatchBrackets() {
	const unsigned int pos = m_lines.GetPos();

	// If no change to document or position, then stop.
	if (!m_bracketHighlight.UpdateIfChanged(m_changeToken, pos)) return;

	m_bracketHighlight.Clear();

	if (pos == m_lines.GetLength()) return;

	unsigned int pos2;
	if (!FindMatchingBracket(pos, pos2)) return;

	if (pos < pos2) m_bracketHighlight.Set(pos, pos2);
	else  m_bracketHighlight.Set(pos2, pos);
}

unsigned int EditorCtrl::RawDelete(unsigned int start, unsigned int end) {
	if (start == end) return 0;
	wxASSERT(end > start && end <= m_lines.GetLength());

	const unsigned int pos = m_lines.GetPos();

	if (m_autopair.HasPairs()) {
		const interval& iv = m_autopair.InnerPair();

		// Detect backspacing in active auto-pair
		if (end == iv.start && end == iv.end) {
			unsigned int nextpos;
			cxLOCKDOC_READ(m_doc)
				nextpos = doc.GetNextCharPos(iv.end);
			cxENDLOCK

			// Also delete pair ender
			end = nextpos;
			m_autopair.DropInnerPair();
		}
		else if (iv.start > start || iv.end < end) {
			// Reset autoPair state if deleting outside inner pair
			m_autopair.Clear();
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
	m_autopair.AdjustEndsDown(del_len);

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
		if (dest >= end)
			dest -= len;

		// Insert at destination
		RawInsert(dest, text);
	}

	// If caret was in source, it moves with the text
	if (start <= pos && pos <= end)
		m_lines.SetPos(dest + (pos - start));

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
			const unsigned int indentlevel = CountTextIndent(indent, m_parentFrame.GetTabWidth());
			const unsigned int newindentlevel = CountTextIndent(newindent, m_parentFrame.GetTabWidth());

			// Only double the newlines if the new line will be de-dented
			if (newindentlevel < indentlevel) {
				const unsigned int newlinelen = RawInsert(pos, wxT('\n'), false);
				byte_len += newlinelen;
				const unsigned int newlinestart = pos + newlinelen;

				// Set correct indentation for new line
				if (!newindent.empty())
					byte_len += RawInsert(newlinestart, newindent, false);
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
	if (m_macro.IsRecording()) {
		eMacroCmd& cmd = lastaction != ACTION_INSERT || m_macro.IsEmpty() || m_macro.Last().GetName() != wxT("InsertChars")
			             ? m_macro.AddWithStrArg(wxT("InsertChars"), wxT("text"), wxT("")) : m_macro.Last();
		cmd.ExtendString(0, text);
	}

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

		// Adjust containing pairs
		m_autopair.AdjustEndsUp(byte_len);

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
	m_autopair.Clear(); // invalidate auto-pair state

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

void EditorCtrl::Delete(bool delWord) {
	if (m_macro.IsRecording()) {
		if (delWord) m_macro.Add(wxT("DeleteWord"));
		else m_macro.Add(wxT("Delete"));
	}

	const unsigned int pos = m_lines.GetPos();

	// Reset autoPair state if deleting outside inner pair
	if (m_autopair.HasPairs())
	{
		const interval& inner_pair = m_autopair.InnerPair();
		if (pos < inner_pair.start || inner_pair.end <= pos)
			m_autopair.Clear();
	}

	// Check if we should delete entire word
	if (delWord && !m_lines.IsSelected()) {
		const interval iv = GetWordIv(pos);

		if (pos >= iv.start && pos < iv.end) {
			Delete(pos, iv.end);
			return;
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
		return;
	}

	if (pos != lastpos || lastaction != ACTION_DELETE) {
		cxLOCKDOC_WRITE(m_doc)
			doc.Freeze();
		cxENDLOCK
	}

	if (m_lines.IsSelected()) {
		if (m_lines.IsSelectionShadow()) {
			if (DeleteInShadow(pos, true)) return; // if false we have to del outside shadow
		}
		else {
			DeleteSelections();
			cxLOCKDOC_WRITE(m_doc)
				doc.Freeze();
			cxENDLOCK
			return;
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
		return;
	}

	// Check if we are at a soft tabpoint
	const unsigned int tabWidth = m_parentFrame.GetTabWidth();
	if (nextchar == wxT(' ') && m_lines.IsAtTabPoint()
		&& pos + tabWidth <= m_lines.GetLength() && IsSpaces(pos, pos + tabWidth))
	{
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

void EditorCtrl::Backspace(bool delWord) {
	if (m_macro.IsRecording()) {
		if (delWord) m_macro.Add(wxT("DeleteWord"));
		else m_macro.Add(wxT("Backspace"));
	}

	const unsigned int pos = m_lines.GetPos();

	// Check if we should delete entire word
	if (delWord && !m_lines.IsSelected()) {
		const interval iv = GetWordIv(pos);

		if (pos <= iv.end && pos > iv.start) {
			Delete(iv.start, pos);
			return;
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
		return;
	}

	if (pos != lastpos || lastaction != ACTION_DELETE) {
		cxLOCKDOC_WRITE(m_doc)
			doc.Freeze();
		cxENDLOCK
	}

	if (m_lines.IsSelected()) {
		if (m_lines.IsSelectionShadow()) {
			if (DeleteInShadow(pos, false)) return; // if false we have to del outside shadow
		}
		else {
			DeleteSelections();
			cxLOCKDOC_WRITE(m_doc)
				doc.Freeze();
			cxENDLOCK
			return;
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
	wxFileName filepath;
	cxFileResult result = LoadLinesIntoDocument(newpath, enc, rp, filepath);
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

	return cxFILE_OK;
}

cxFileResult EditorCtrl::LoadLinesIntoDocument(const wxString& whence_to_load, wxFontEncoding enc, const RemoteProfile* rp, wxFileName& localPath) {
	// First clean up old remote info (and delete evt. buffer file);
	ClearRemoteInfo();

	if (!eDocumentPath::IsRemotePath(whence_to_load)) localPath = whence_to_load;
	else	{
		// If the path points to a remote file, we have to download it first.
		m_remoteProfile = rp ? rp : m_parentFrame.GetRemoteProfile(whence_to_load, false);
		const wxString buffPath = m_parentFrame.DownloadFile(whence_to_load, m_remoteProfile);
		if (buffPath.empty()) return cxFILE_DOWNLOAD_ERROR; // download failed

		localPath = buffPath;
		m_remotePath = whence_to_load;
	}

	// Invalidate all stylers
	StylersInvalidate();

	return m_lines.LoadText(localPath, enc, m_remotePath);
}

bool EditorCtrl::SaveText(bool askforpath) {
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
		wxFileDialog dlg(this, _T("Save as..."), _T(""), _T(""), EditorFrame::DefaultFileFilters, wxSAVE|wxCHANGE_DIR);
		dlg.SetPath( newpath.empty() ? _("Untitled") : newpath );

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
		if (answer != wxOK) return false;

		if (!eDocumentPath::MakeWritable(newpath)) {
			wxMessageBox(_T("Unable to remove write protection!"), _T("e Error"), wxICON_ERROR);
			return false;
		}
	}

	// Check if we need to force the native end-of-line
	bool forceNativeEOL = false;
	bool noAtomic = false;
	eSettings& settings = eGetSettings();
	settings.GetSettingBool(wxT("force_native_eol"), forceNativeEOL);
	settings.GetSettingBool(wxT("disable_atomic_save"), noAtomic);

	// Save the text
	cxFileResult savedResult;
	const wxString realname = m_remotePath.empty() ? wxT("") : docName;
	cxLOCKDOC_WRITE(m_doc)
		savedResult = doc.SaveText(filepath, forceNativeEOL, realname, false, noAtomic);
	cxENDLOCK

	const wxString pathStr = filepath.GetFullPath();

	switch (savedResult) {
	case cxFILE_OK:
		// Clean up remote info if file was saved locally
		if (!m_remotePath.empty() && savedLocal)
			ClearRemoteInfo();

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
		if (!m_parentFrame.UploadFile(m_remotePath, pathStr, m_remoteProfile))
			return false;

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
	wxASSERT(!eDocumentPath::IsRemotePath(newpath)); // just to catch a bug

	if (m_path.GetFullPath() == newpath) return;

	m_path = newpath;
	wxASSERT(m_path.IsAbsolute());

	// Clear the env var cache
	m_tmFilePath.clear();
	m_tmDirectory.clear();

	// Set the syntax to match the new path
	if (m_syntaxstyler.UpdateSyntax())
		DrawLayout(); // Redraw (since syntax for this file has changed)
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

		if (hasMirror) {
			if (modifiedDate.IsValid() && (m_doc == di)) return false;
			return true;
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

cxFileResult EditorCtrl::OpenFile(const wxString& filepath, wxFontEncoding enc, const RemoteProfile* rp, const wxString& mate) {
	// Bundle items do their own mirror handling during loading
	if (eDocumentPath::IsBundlePath(filepath))
		return LoadText(filepath, enc, rp);

	if (!mate.empty())
		SetMate(mate);

	// Check if there is a mirror that matches the file
	doc_id di;
	wxDateTime modifiedDate;
	bool isMirror;
	cxLOCK_READ(m_catalyst)
		isMirror = catalyst.GetFileMirror(filepath, di, modifiedDate);
	cxENDLOCK

	if(isMirror) {
		const bool isRemoteItem = eDocumentPath::IsRemotePath(filepath);
		bool doReload = true;

		// Check if the file on disk has been changed since last save
		if (modifiedDate.IsValid()) {
			if (isRemoteItem) {
				if (!rp) rp = m_parentFrame.GetRemoteProfile(filepath, false);
				const wxDateTime fileDate = m_parentFrame.GetRemoteThread().GetModDate(filepath, *rp);
				if (modifiedDate == fileDate)
					doReload = false; // No need to reload unchanged file
				else wxLogDebug(wxT("file %s needs to be reloaded"), filepath.c_str());
			}
			else {
				const wxFileName path(filepath);
				if (path.FileExists() && modifiedDate == path.GetModificationTime())
					doReload = false; // No need to reload unchanged file
				else wxLogDebug(wxT("file %s needs to be reloaded"), filepath.c_str());
			}
		}

		// Set doc before reload to update
		if (filepath != m_path.GetFullPath()) {
			if (doReload) EnableRedraw(false); // avoid drawing twice
			const bool res = SetDocument(di, filepath, rp);
			EnableRedraw(true);
			if (!res) return cxFILE_OPEN_ERROR;
		}

#ifdef __WXMSW__
		// Filename may have changed case on disk
		// It gets corrected during reload, but otherwise we have to correct it manually
		if (!isRemoteItem && !doReload) {
			const wxString longpath = wxFileName(filepath).GetLongPath(); // gets path with correct case
			const wxString newName = wxFileName(longpath).GetFullName();
			const wxString oldName = GetName();
			if (newName != oldName) {
				cxLOCKDOC_WRITE(GetDocument())
					doc.SetPropertyName(newName);
				cxENDLOCK
			}
		}
#endif

		if (!doReload)
			return cxFILE_OK;
	}

	return LoadText(filepath, enc, rp);
}

bool EditorCtrl::SetDocument(const doc_id& di, const wxString& path, const RemoteProfile* rp) {
	doc_id oldDoc;
	cxLOCKDOC_READ(m_doc)
		oldDoc = doc.GetDocument();
	cxENDLOCK

	if (di == oldDoc) // No reason to set doc if we are already there
		return true;

	// If the current doc is an clean draft (no revs & no parent)
	// we have to remember to delete it after setting the new doc
	bool deleteOld = oldDoc.IsOk() && IsEmpty();

	topline = -1;
	m_currentSel = -1;
	m_autopair.Clear(); // reset autoPair state - adamv - changed from 'empty' to 'clear'.

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
		m_path.Clear();

		// Invalidate all stylers
		// (has to be done before reloading to avoid stale refs)
		StylersInvalidate();

		cxLOCKDOC_WRITE(m_doc)
			doc.SetDocument(di);
		cxENDLOCK

		// If the path points to a remote file, we have to save it to a temp bufferfile first.
		if (eDocumentPath::IsRemotePath(path)) {
			m_remoteProfile = rp ? rp : m_parentFrame.GetRemoteProfile(path, false);
			m_path = GetAppPaths().CreateTempAppDataFile();
			m_remotePath = path;

			cxLOCKDOC_WRITE(m_doc)
				const cxFileResult res = doc.SaveText(m_path, false, m_remotePath, true); // keep date
				if (res != cxFILE_OK) return false;
			cxENDLOCK
		}
		else if (eDocumentPath::IsBundlePath(path)) m_remotePath = path;
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
	m_parentFrame.UpdateWindowTitle();

	// If this is a bundle item we also have to update the panel
	UpdateParentPanels();

	// Notify that we have changed document
	const doc_id docId = di; // just to be on the stack in bug reports
	dispatcher.Notify(wxT("WIN_CHANGEDOC"), this, m_parentFrame.GetId());

	return true;
}

// Only derived controls are embedded in a parent panel; see BundleItemEditorCtrl.
void EditorCtrl::UpdateParentPanels() {}

void EditorCtrl::GetLinesChangedSince(const doc_id& di, vector<size_t>& lines) {
	vector<cxChange> changes;
	cxLOCKDOC_READ(m_doc)
		const doc_id current = doc.GetDocument();
		doc_id prev = di;
		if (prev == current) {
			if (prev.IsDraft()) prev = doc.GetDraftParent(di.version_id);
			else return;
		}
		changes = doc.GetChanges(prev, current);
	cxENDLOCK

	const vector<cxLineChange> linechanges = m_lines.ChangesToFullLines(changes);
	const size_t end = m_lines.GetLength();

	for (vector<cxLineChange>::const_iterator p = linechanges.begin(); p != linechanges.end(); ++p) {
		const size_t firstline = m_lines.GetLineFromStartPos(p->start);
		const size_t lastline = p->end == end ? m_lines.GetLineCount() : m_lines.GetLineFromStartPos(p->end);
		lines.push_back(firstline);

		for (size_t i = firstline+1; i < lastline; ++i) {
			lines.push_back(i);
		}
	}
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
	if (!changes.empty()) bookmarks.ApplyChanges(changes);

	// Move caret to position of first change
	if (moveToFirstChange) {
		m_lines.RemoveAllSelections();
		if (!changes.empty()) {
			const unsigned int changePos = changes[0].start;
			SetPos(changePos);
			wxLogDebug(wxT("changepos: %d"), changePos);
		}
		if (!IsCaretVisible())
			MakeCaretVisibleCenter();
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
	FoldingClear();
}

void EditorCtrl::StylersInsert(unsigned int pos, unsigned int length) {
	m_search_hl_styler.Insert(pos, length);
	m_syntaxstyler.Insert(pos, length);
	FoldingInsert(pos, length);
	bookmarks.InsertChars(pos, length);
}

void EditorCtrl::StylersDelete(unsigned int start, unsigned int end) {
	m_search_hl_styler.Delete(start, end);
	m_syntaxstyler.Delete(start, end);
	FoldingDelete(start, end);
	bookmarks.DeleteChars(start, end);
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
	if (matchlist[0].iv1_start_pos > 0) return 0; // insertion at top
	if (matchlist[0].iv2_start_pos > 0) return 0; // deletion at top

	return matchlist[0].iv1_end_pos;
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
	const interval* const selection = m_lines.FirstSelection();
	if (selection == NULL) return;

	if (m_snippetHandler.IsActive()) {
		m_snippetHandler.Delete(selection->start, selection->end);
		return;
	}

	m_autopair.Clear(); // invalidate auto-pair state

	cxLOCKDOC_WRITE(m_doc)
		doc.Freeze(); // always freeze before modifying sel contents
	cxENDLOCK

	unsigned int pos = m_lines.GetPos();

	const vector<interval>& selections = m_lines.GetSelections();
	const bool hasRanges = HasSearchRange();

	size_t dl = 0;
	for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
		if (iv->start == iv->end) continue;

		const size_t start = iv->start - dl;
		const size_t end = iv->end - dl;
		const size_t len = iv->end - iv->start;

		cxLOCKDOC_WRITE(m_doc)
			doc.Delete(start, end);
		cxENDLOCK
		m_lines.Delete(start, end);
		StylersDelete(start, end);

		// Update caret position
		if (pos >= start && pos <= end) pos = start;
		else if (pos > end) pos -= len;

		if (hasRanges) {
			// Find range containing deletion
			for (size_t i = 0; i < m_searchRanges.size(); ++i) {
				interval& r = m_searchRanges[i];
				if (end <= r.end) {
					// Adjust cursor
					size_t& c = m_cursors[i];
					if (c == end) c = start;
					else if (c > end) c -= len;

					// Adjust range len
					r.end -= len;

					// Adjust all following ranges
					++i;
					for (; i < m_searchRanges.size(); ++i) {
						m_searchRanges[i].start -= len;
						m_searchRanges[i].end -= len;
						m_cursors[i] -= len;
					}
					break;
				}
			}

		}

		dl += len;
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
	if (m_autopair.HasPairs()) {
		const interval& iv = m_autopair.InnerPair();

		// Detect backspacing in active auto-pair
		if (!nextchar && iv.IsPoint(pos)) {
			// Also delete pair ender
			inAutoPair = true;
			m_autopair.DropInnerPair();
		}
		else if ((nextchar && iv.end <= pos) || (!nextchar && pos <= iv.start)) {
			// Reset autoPair state if deleting outside inner pair
			m_autopair.Clear();
		}
	}

	const vector<interval> selections = m_lines.GetSelections();
	unsigned int offset = 0;

	// Calculate delete positions
	for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
		// Check for overlap
		if ( iv->start <= pos && pos <= iv->end) {
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
		if (m_autopair.BeforeOuterPair(del_end))
			m_autopair.AdjustIntervalsDown(byte_len);

		if (atCaret) // Adjust containing pairs
			m_autopair.AdjustEndsDown(byte_len);

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

	m_autopair.ClearIfInsertingOutsideInnerPair(pos);

	wxString autoPair;
	if (text.length() == 1) {
		if (m_lines.IsSelectionShadow()) {
			if (m_autopair.HasPairs()) {
				if (pos != m_autopair.InnerPair().end) {
					// Reset autoPair state if inserting outside inner pair
					m_autopair.Clear();
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
						m_autopair.DropInnerPair();
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

			if (shadowpos >= iv->start && shadowpos <= iv->end)
				offset = shadowpos - iv->start;
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
			if (m_autopair.BeforeOuterPair(pair_pos)) {
				m_autopair.AdjustIntervalsUp(full_len);
			}

			// Adjust caret pos
			if (pos > ins_pos) pos += full_len;
			else if (pos == ins_pos) {
				pos += byte_len;

				// If we are just inserting text, adjust containing pairs
				if (autoPair.empty())
					m_autopair.AdjustEndsUp(byte_len);
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
		unsigned int linestart, lineend;
		m_lines.GetLineExtent(*i, linestart, lineend);

		if (i > sel_lines.begin() && *i == lastline+1 && sel != -1)
			m_lines.UpdateSelection(sel, firststart, lineend);
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

// Adam V - TODO - reduce code duplication here by introducing a
// class that iterates either the current selections or the entire document
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
	if (reSelect)
		SelectLines(sel_lines);

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
	if (linestart != lineend)
		RawMove(linestart, lineend, lineend2);

	if (!m_lines.IsLineVirtual(line2) && linestart2 != lineend2) {
		const unsigned int offset = lineend - linestart;

		// Move bottom line to top lines position
		RawMove(linestart2 - offset, lineend2 - offset, linestart);
	}
}

void EditorCtrl::Transpose() {
	if (m_macro.IsRecording()) m_macro.Add(wxT("Transpose"));

	Freeze();

	vector<interval> selections = m_lines.GetSelections();
	const unsigned int pos = m_lines.GetPos();

	if (selections.empty()) {
		const unsigned int lineid = m_lines.GetCurrentLine();
		const unsigned int linestart = m_lines.GetLineStartpos(lineid);
		const unsigned int lineend = m_lines.GetLineEndpos(lineid, true);

		if (pos == lineend) {
			// if we are at end of line, we swap it with the one below it
			if (lineid < m_lines.GetLastLine())
				SwapLines(lineid, lineid+1);
		}
		else if (pos == linestart) {
			// if we are at start of line, we swap it with the one above it
			if (lineid > 0)
				SwapLines(lineid-1, lineid);
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

					if (prevWord.start != prevWord.end && nextWord.start != nextWord.end)
						swapWords = true; // both words found
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
		for (vector<interval>::iterator p = newsel.begin(); p != newsel.end(); ++p)
			m_lines.AddSelection(p->start, p->end);
	}

	Freeze();
}

void EditorCtrl::DelCurrentLine(bool fromPos) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("DeleteCurrentLine"), wxT("fromPos"), fromPos);
	}

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
		else Delete(pos, lineend); // Delete from pos to end-of-line
	}
	else {
		// Delete entire line
		const unsigned int lineend = m_lines.GetLineEndpos(lineid, false);
		Delete(linestart, lineend);
	}

	Freeze();
}

void EditorCtrl::ConvertCase(cxCase conversion) {
	if (m_macro.IsRecording()) {
		eMacroCmd& cmd = m_macro.Add(wxT("ConvertCase"));
		if (conversion == cxUPPERCASE) cmd.AddArg(wxT("conversion"), wxT("upper"));
		else if (conversion == cxLOWERCASE) cmd.AddArg(wxT("conversion"), wxT("lower"));
		else if (conversion == cxTITLECASE) cmd.AddArg(wxT("conversion"), wxT("title"));
		else if (conversion == cxREVERSECASE) cmd.AddArg(wxT("conversion"), wxT("reverse"));
	}

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
	FoldingClear();

	MarkAsModified(); // flush symbol cache

	DrawLayout();
	MarkAsModified();
};

void EditorCtrl::OnCopy() {
	if (m_macro.IsRecording()) m_macro.Add(wxT("Copy"));

	wxString copytext;
	if (m_lines.IsSelected()) copytext = GetSelText();
	else if (!m_lines.IsEmpty()) {
		// Copy line
		const unsigned int line_id = m_lines.GetCurrentLine();
		if (!m_lines.IsLineVirtual(line_id)) {
			unsigned int startpos, endpos;
			m_lines.GetLineExtent(line_id, startpos, endpos);
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
				for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv)
					mdo->AddText(doc.GetTextPart((*iv).start, (*iv).end));
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
	if (m_macro.IsRecording()) m_macro.Add(wxT("Cut"));

	bool doCopy = true;
	if (!m_lines.IsSelected()) {
		// Select current line 
		const unsigned int cl = m_lines.GetCurrentLine();
		unsigned int start, end;
		m_lines.GetLineExtent(cl, start, end);
		m_lines.AddSelection(start, end);

		if (m_lines.IsLineEmpty(cl)) doCopy = false; // don't copy empty line
	}

	if (doCopy) {
		MacroDisabler m(m_macro);
		OnCopy();
	}
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
	if (m_macro.IsRecording()) m_macro.Add(wxT("Paste"));
	if (!wxTheClipboard->Open()) return;
	
	if (wxTheClipboard->IsSupported( MultilineDataObject::FormatId )) {
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
			InplaceConvertCRLFtoLF(copytext);
#endif // __WXMSW__

			Freeze();

			Insert(copytext);
			m_lines.PrepareAll(); // make sure all lines are valid
			
			Freeze(); // No extension possible after paste
		}
	}
}

bool EditorCtrl::IsSelected() const {
	return m_lines.IsSelected();
}

void EditorCtrl::RemoveAllSelections() {
	m_lines.RemoveAllSelections();
	m_currentSel = -1;
}

void EditorCtrl::ReverseSelections() {
	if (!m_lines.IsSelected()) return;
	
	const vector<interval> oldsel = m_lines.GetSelections();
	m_lines.RemoveAllSelections();

	if (m_searchRanges.empty()) {
		unsigned int pos = 0;
		for (vector<interval>::const_iterator p = oldsel.begin(); p != oldsel.end(); ++p) {
			if (pos < p->start) m_lines.AddSelection(pos, p->start);
			pos = p->end;
		}
		const unsigned int len = m_lines.GetLength();
		if (pos < len) {
			m_lines.AddSelection(pos, len);
			pos = len;
		}

		m_lines.SetPos(pos);
	}
	else {
		vector<interval>::const_iterator s = oldsel.begin();
		for (size_t i = 0; i < m_searchRanges.size(); ++i) {
			const interval& r = m_searchRanges[i];
			unsigned int pos = r.start;

			while (s != oldsel.end() && s->end <= r.end) {
				if (pos < s->start)  {
					m_lines.AddSelection(pos, s->start);
					m_cursors[i] = s->start;
				}
				pos = s->end;
				++s;
			}
			if (pos < r.end) {
				m_lines.AddSelection(pos, r.end);
				m_cursors[i] = r.end;
			}
		}

		m_lines.SetPos(m_cursors.back());
	}
}

void EditorCtrl::Select(unsigned int start, unsigned int end) {
	// The positions may come from unverified input
	// So we have to make sure they are valid
	cxLOCKDOC_READ(m_doc)
		start = doc.GetValidCharPos(start);
		if (end > doc.GetLength()) end = doc.GetLength();
		else if (end != doc.GetLength()) end = doc.GetValidCharPos(end);
	cxENDLOCK
	if (start >= end) return;

	m_lines.RemoveAllSelections();
	m_lines.AddSelection(start, end);
}

void EditorCtrl::AddSelection(unsigned int start, unsigned int end, bool allowEmpty) {
	// The positions may come from unverified input
	// So we have to make sure they are valid
	cxLOCKDOC_READ(m_doc)
		start = doc.GetValidCharPos(start);
		if (end > doc.GetLength()) end = doc.GetLength();
		else if (end != doc.GetLength()) end = doc.GetValidCharPos(end);
	cxENDLOCK
	if (start > end) return;
	if (start == end && !allowEmpty) return;

	m_lines.AddSelection(start, end);
}

void EditorCtrl::SelectAll() {
	if (m_macro.IsRecording()) m_macro.Add(wxT("SelectAll"));

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
	if (iv.empty()) return wxEmptyString;

	cxLOCKDOC_READ(m_doc)
		return doc.GetTextPart(iv.start, iv.end);
	cxENDLOCK
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

void EditorCtrl::SelectWord(bool multiselect) {
	if (m_macro.IsRecording()) m_macro.Add(wxT("SelectWord"));

	SelectWordAt(GetPos(), multiselect);
}

void EditorCtrl::SelectWordAt(unsigned int pos, bool multiselect) {
	wxASSERT(pos <= m_doc.GetLength());
	m_lastScopePos = -1; // invalidate scope selections

	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
	if (!multiselect || m_lines.IsSelectionShadow()) m_lines.RemoveAllSelections();

	// Check if we should select brackets or word
	interval iv;
	if (m_bracketHighlight.HasOrderedInterval() && m_bracketHighlight.IsEndPoint(pos)) {
		iv = m_bracketHighlight.GetInterval();
		iv.end += 1; // include end bracket
	}
	else iv = GetWordIv(pos);

	if (!iv.empty()) {
		m_selMode = SEL_WORD;
		m_sel_start = iv.start;
		m_sel_end = iv.end;
		m_currentSel = m_lines.AddSelection(iv.start, iv.end);
		m_lines.SetPos(iv.end);
		MakeCaretVisible();
	}
}

void EditorCtrl::SelectLine(unsigned int line_id, bool multiselect) {
	// If we have a new, empty document, and the user triple-clicks anyway,
	// don't do anything.
	if (m_lines.GetLineCount() == 0) return;

	wxASSERT(line_id < m_lines.GetLineCount());
	m_lastScopePos = -1; // invalidate scope selections

	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
	if (!multiselect || m_lines.IsSelectionShadow()) m_lines.RemoveAllSelections();

	// Select the line
	if (!m_lines.IsLineVirtual(line_id)) {
		unsigned int start, end;
		m_lines.GetLineExtent(line_id, start, end);

		m_selMode = SEL_LINE;
		m_sel_start = start;
		m_sel_end = end;
		m_currentSel = m_lines.AddSelection(start, end);
		m_lines.SetPos(end);
	}
}

void EditorCtrl::SelectCurrentLine() {
	if (m_macro.IsRecording()) m_macro.Add(wxT("SelectCurrentLine"));

	SelectLine(m_lines.GetCurrentLine());
}

void EditorCtrl::ExtendSelectionToLine(unsigned int sel_id) {
	wxASSERT(sel_id < m_lines.GetSelections().size());

	const unsigned int pos = m_lines.GetPos();
	unsigned int line_id = m_lines.GetLineFromCharPos(pos);
	const bool isStart = m_lines.IsLineStart(line_id, pos);
	const interval& iv = m_lines.GetSelections()[sel_id];

	if (pos > iv.start) {
		if (isStart) --line_id;
		const unsigned int end = m_lines.GetLineEndpos(line_id, false);
		m_lines.UpdateSelection(sel_id, iv.start, end);
		m_lines.SetPos(end);
	}
	else {
		const unsigned int start = m_lines.GetLineStartpos(line_id);
		m_lines.UpdateSelection(sel_id, start, iv.end);
		m_lines.SetPos(start);
	}
}

void EditorCtrl::SelectScope() {
	if (m_macro.IsRecording()) m_macro.Add(wxT("SelectScope"));

	if (m_snippetHandler.IsActive()) m_snippetHandler.Clear();
	unsigned int pos = GetPos();

	// Start over if all is selected
	const vector<interval>& selections = m_lines.GetSelections();
	if (selections.size() == 1 && selections[0].start == 0 && selections[0].end == m_lines.GetLength()) {
		m_lines.RemoveAllSelections();
		if (m_lastScopePos != -1 && m_lastScopePos <= (int)m_lines.GetLength())
			pos = m_lastScopePos;
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
	if (m_lastScopePos == -1)
		m_lastScopePos = pos;
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
				if (p == completions.end() || p->second < dist)
					completions[word] = dist;
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
	for (map<wxString,unsigned int>::const_iterator p = completions.begin(); p != completions.end(); ++p)
		sorted[p->second] = p->first;

	// return list of completions, sorted by distance
	result.Alloc(result.GetCount() + sorted.size());
	for (map<unsigned int,wxString>::const_iterator p2 = sorted.begin(); p2 != sorted.end(); ++p2)
		result.Add(p2->second);
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
		for (vector<wxString>::const_iterator p = completionList->begin(); p != completionList->end(); ++p)
			if (precharbase || p->StartsWith(target)) completions.Add(*p);
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

			const long result = ShellRunner::RawShell(completionCmd->cmd, input, &output, NULL, env);
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
	if (!m_syntaxHandler.DisableDefaultCompletion(scope))
		GetCompletionMatches(iv, completions, precharbase);

	return completions;
}

void EditorCtrl::DoCompletion() {
	const wxArrayString completions = GetCompletionList();
	if (completions.empty()) return;

	// If only one item, then auto-complete
	if (completions.GetCount() == 1) {
		this->ReplaceCurrentWord(completions[0]);
		return;
	}

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
	if (m_macro.IsRecording()) {
		m_macro.AddWithStrArg(wxT("ReplaceCurrentWord"), wxT("text"), word);
	}

	const interval iv = GetWordIv(GetPos());

	Freeze();

	if (m_snippetHandler.IsActive()) {
		m_snippetHandler.Delete(iv.start, iv.end);
		m_snippetHandler.Insert(word);
		return;
	}

	RawDelete(iv.start, iv.end);
	unsigned int byte_len = RawInsert(iv.start, word, true);
	SetPos(iv.start + byte_len);

	Freeze();
}

bool EditorCtrl::HasSearchRange() const {
	return !m_searchRanges.empty();
}

void EditorCtrl::SetSearchRange() {
	m_searchRanges = m_lines.GetSelections();
	m_lines.RemoveAllSelections();
	
	// Position cursors at beginning of ranges
	const unsigned int pos = m_lines.GetPos();
	m_cursors.resize(m_searchRanges.size());
	for (size_t i = 0; i < m_searchRanges.size(); ++i) {
		const interval& r = m_searchRanges[i];
		m_cursors[i] = r.start;
		if (pos == r.end) SetPos(r.start); // make sure real cursor follows
	}

	DrawLayout();
}

void EditorCtrl::AdjustSearchRangeInsert(size_t range_id, int len) {
	wxASSERT(range_id < m_searchRanges.size());

	m_searchRanges[range_id].end += len;

	for (size_t i = range_id+1; i < m_searchRanges.size(); ++i) {
		interval& iv = m_searchRanges[i];
		iv.start += len;
		iv.end += len;
	}
}

void EditorCtrl::ClearSearchRange(bool reset) {
	if (m_searchRanges.empty()) return;

	if (reset && !m_lines.IsSelected()) {
		// Reset selection to cursors
		for (vector<unsigned int>::const_iterator p = m_cursors.begin(); p != m_cursors.end(); ++p)
			m_lines.AddSelection(*p, *p);
	}
	m_searchRanges.clear();
	m_cursors.clear();
	DrawLayout();
}

const vector<interval>& EditorCtrl::GetSearchRange() const {
	return m_searchRanges;
}

const vector<unsigned int>& EditorCtrl::GetSearchRangeCursors() const {
	return m_cursors;
}
	
void EditorCtrl::SetSearchRangeCursor(size_t cursor, unsigned int pos) {
	wxASSERT(cursor < m_cursors.size());
	m_cursors[cursor] = pos;
}

search_result EditorCtrl::SearchDirect(const wxString& pattern, int options, size_t startpos, size_t endpos) const {
	const bool regex = (options & FIND_USE_REGEX) != 0;
	const bool matchcase = (options & FIND_MATCHCASE) != 0;;
	const bool forward = (options & FIND_REVERSE) == 0;

	// Direct interface to document search
	cxLOCKDOC_READ(m_doc)
		if (regex) {
			if (forward) return doc.RegExFind(pattern, startpos, matchcase, NULL, endpos);
			else {
				search_result sr = doc.RegExFindBackwards(pattern, startpos, matchcase);
				if (sr.start < endpos) sr.error_code = -1;
				return sr;
			}
		}
		else {
			if (forward) return doc.Find(pattern, startpos, matchcase, endpos);
			else {
				search_result sr = doc.FindBackwards(pattern, startpos, matchcase);
				if (sr.start < endpos) sr.error_code = -1;
				return sr;
			}
		}
	cxENDLOCK
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
			for (; p != m_searchRanges.end(); ++p)
				if (start_pos >= p->start && start_pos < p->end) break;

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
			for (; p != m_searchRanges.rend(); ++p)
				if (start_pos > p->start && start_pos <= p->end) break;

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

	if (sr.error_code < 0) return false;

	iv.start = sr.start;
	iv.end = sr.end;
	return true;
}

cxFindResult EditorCtrl::Find(const wxString& text, int options) {
	// We have to be aware of incremental searches, so to find out if we are continuing
	// a previous search we check if lastpos/pos are in sync with start_pos/found_pos
	bool isIncremental = true;
	if (m_search_found_pos != m_lines.GetPos() || m_search_start_pos != m_lines.GetLastpos()) {
		m_search_start_pos = m_lines.GetPos();
		m_lines.SetLastpos(m_search_start_pos);
		isIncremental = false;
	}

	if (m_macro.IsRecording()) {
		eMacroCmd& cmd = isIncremental && !m_macro.IsEmpty() && m_macro.Last().GetName() == wxT("Find")
			             ? m_macro.Last() : m_macro.Add(wxT("Find"));
		cmd.SetArg(0, wxT("findString"), text);
		cmd.SetArg(1, wxT("ignoreCase"), !(options & FIND_MATCHCASE));
		cmd.SetArg(2, wxT("regularExpression"), (options & FIND_USE_REGEX) != 0);
		cmd.SetArg(3, wxT("wrapAround"), (options & FIND_RESTART) != 0);
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
	if (m_macro.IsRecording()) {
		eMacroCmd& cmd = m_macro.Add(wxT("FindNext"));
		cmd.SetArg(0, wxT("findString"), text);
		cmd.SetArg(1, wxT("ignoreCase"), !(options & FIND_MATCHCASE));
		cmd.SetArg(2, wxT("regularExpression"), (options & FIND_USE_REGEX) != 0);
		cmd.SetArg(3, wxT("wrapAround"), (options & FIND_RESTART) != 0);
	}

	unsigned int start_pos;
	if (options & FIND_RESTART) start_pos = m_searchRanges.empty() ? 0 : m_searchRanges[0].start;
	else if (m_lines.IsSelected()) {
		const interval& iv = m_lines.GetSelections().back();
		start_pos = iv.end;

		// Avoid hitting same zero-length selection
		if (iv.start == iv.end) {
			++start_pos;
			if (start_pos > (int)m_lines.GetLength()) 
				return cxNOT_FOUND;
		}
	}
	else start_pos = m_lines.GetPos();

	cxFindResult result = cxNOT_FOUND;
	if (DoFind(text, start_pos, options)) result = cxFOUND;

	if (result == cxNOT_FOUND && start_pos > 0) {
		// Restart search from top
		start_pos = m_searchRanges.empty() ? 0 : m_searchRanges[0].start;
		if (DoFind(text, start_pos, options)) 
			result = cxFOUND_AFTER_RESTART;
	}

	// Make sure that next search has the new starting point
	// (no selection if invalid pattern)
	if (result != cxNOT_FOUND && IsSelected())
		m_lines.SetLastpos(m_lines.FirstSelection()->start);

	return result;
}

cxFindResult EditorCtrl::FindPrevious(const wxString& text, int options) {
	if (m_macro.IsRecording()) {
		eMacroCmd& cmd = m_macro.Add(wxT("FindPrevious"));
		cmd.SetArg(0, wxT("findString"), text);
		cmd.SetArg(1, wxT("ignoreCase"), !(options & FIND_MATCHCASE));
		cmd.SetArg(2, wxT("regularExpression"), (options & FIND_USE_REGEX) != 0);
		cmd.SetArg(3, wxT("wrapAround"), (options & FIND_RESTART) != 0);
	}

	unsigned int start_pos;
	if (options & FIND_RESTART) start_pos = m_searchRanges.empty() ? m_lines.GetLength() : m_searchRanges.back().end;
	else if (m_lines.IsSelected()) {
		const interval* const selection = m_lines.FirstSelection();
		start_pos  = selection->start;

		// Avoid hitting same zero-length selection
		if (selection->empty()) {
			if (start_pos == 0) return cxNOT_FOUND;
			else --start_pos;
		}
	}
	else start_pos = m_lines.GetPos();

	cxFindResult result = cxNOT_FOUND;
	if (DoFind(text, start_pos, options, false)) result = cxFOUND;

	if (result == cxNOT_FOUND && start_pos == 0) {
		// Restart search from bottom
		start_pos = m_searchRanges.empty() ? m_lines.GetLength() : m_searchRanges.back().end;
		if (DoFind(text, start_pos, options, false)) result = cxFOUND_AFTER_RESTART;
	}

	// Make sure that next search has the new starting point
	// (no selection if invalid pattern)
	if (result != cxNOT_FOUND && IsSelected())
		m_lines.SetLastpos(m_lines.FirstSelection()->end);

	return result;
}

wxString EditorCtrl::ParseReplaceString(const wxString& replacetext, const map<unsigned int,interval>& captures, const vector<char>* source) const {
	ReplaceStringParser parser(m_doc, m_indent, replacetext, captures, source);
	return parser.Parse();
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
		// We might want to study the regex the first time we end here
		if (searchtext == m_regex_cache && options == m_options_cache) {}
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
	wxASSERT(subjectStart <= pos && pos <= subjectEnd);

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

	pcre_free(re); // clean up

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

	pcre_free(re); // clean up

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
		// We might want to study the regex the first time we end here
		if (searchtext == m_regex_cache && options == m_options_cache) {}
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
	if (!m_lines.IsSelected()) {
		int startpos = (options & FIND_RESTART) ? 0 : m_lines.GetPos();
		return DoFind(searchtext, startpos, options);
	}

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

int EditorCtrl::ReplaceAll(const wxString& searchtext, const wxString& replacetext, int options) {
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

	unsigned int replacements = 0;

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
			textNew = (options & FIND_USE_REGEX) ? ParseReplaceString(replacetext, captures) : replacetext;
		}

		// Delete original
		if (result.start != result.end) {
			cxLOCKDOC_WRITE(m_doc)
				doc.Delete(result.start, result.end);
			cxENDLOCK
		}

		// Insert replacement
		if (!textNew.empty()) {
			cxLOCKDOC_WRITE(m_doc)
				byte_len = doc.Insert(result.start, textNew);
			cxENDLOCK
		}
		else byte_len = 0;

		// Adjust searchranges
		if (!m_searchRanges.empty()) {
			const int diff = byte_len - (result.end - result.start);
			for (vector<interval>::iterator p2 = p; p2 != m_searchRanges.end(); ++p2) {
				if (p2->start > result.start) p2->start += diff;
				if (p2->end > result.start) p2->end += diff;
			}
		}

		replacements++;

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

		// We also want to avoid infinite loop when replacing a possible
		// zero-len match with nothing
		if (result.start == result.end && byte_len == 0) ++start_pos;

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

	return replacements;
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
	
	// We have to do a full redraw, because there may be other editorCtrls
	// shown at the save time which have written to the same backing bitmap
	DrawLayout(dc);

/*	const wxSize size = GetClientSize();
	const int editorSizeX = ClientWidthToEditor(size.x);

	// Re-Blit MemoryDC to Display
	const unsigned int xpos = m_gutterLeft ? m_gutterWidth : 0;
#ifdef __WXMSW__
	::BitBlt(GetHdcOf(dc), xpos, 0,(int)editorSizeX, (int)size.y, GetHdcOf(mdc), 0, 0, SRCCOPY);
#else
	dc.Blit(xpos, 0,  editorSizeX, size.y, &mdc, 0, 0);
#endif*/
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

bool EditorCtrl::ProcessCommandModeKey(wxKeyEvent& event) {
	MacroDisabler m(m_macro); // Avoid dublicate recording
	return m_commandHandler.ProcessCommand(event, isRecording);
}

void EditorCtrl::OnChar(wxKeyEvent& event) {
	if (m_parentFrame.IsCommandMode()) {
		if (ProcessCommandModeKey(event)) return;
		ClearSearchRange(); // Most non-commandmode commands are not range aware
	}

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

	const int key = event.GetKeyCode();
	const unsigned int oldpos = m_lines.GetPos();
	const SelAction doSelect = (event.ShiftDown() || event.AltDown()) ? SEL_SELECT : SEL_REMOVE;

	// If the cursor is positioned outside of the innermost bracket pair, then
	// we toss our nested bracket pair information.
	if (m_autopair.HasPairs() && !m_autopair.ContainedInInnerPair(oldpos))
		m_autopair.Clear();

	m_lastScopePos = -1; // scope selections

	// Menu shortcuts might have set commandMode
	// without resetting it.
	commandMode = event.ControlDown();

	// Check if unicode value is safe to use instead of keycode
	const wxChar c = event.GetUnicodeKey();
// FIXME - is this needed (or it's just for speed?)
#ifdef __WXMSW__
	if ((unsigned int)c > 127) InsertChar(c);
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
				DelCurrentLine(!event.ShiftDown());
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
				DoCompletion();
				break;

			case WXK_LEFT: // Ctrl <-
				CursorWordLeft(doSelect);
				break;

			case WXK_RIGHT: // Ctrl ->
				CursorWordRight(doSelect);
				break;

			case WXK_UP: // Ctrl arrow up
				if (event.ShiftDown() || event.AltDown()) {
					CursorUp(SEL_SELECT);
				}
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
				if (event.ShiftDown() || event.AltDown()) {
					CursorDown(SEL_SELECT);
				}
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
				CursorToHome(doSelect);
				lastaction = ACTION_NONE;
				break;

			case WXK_END:
				CursorToEnd(doSelect);
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
					for (unsigned int i = 0; i < scopes.size(); ++i)
						wxLogDebug(wxT("%s"), scopes[i]->c_str());
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

			/*case WXK_F10:
				{
					vector<SymbolRef> symbols;
					m_syntaxstyler.GetSymbols(symbols);
					for (vector<SymbolRef>::const_iterator p = symbols.begin(); p != symbols.end(); ++p)
						wxLogDebug(wxT("%d-%d -> \"%s\" -> \"%s\""), p->start, p->end, GetText(p->start, p->end).c_str(), p->transform->c_str());
				}
				break;*/

			case WXK_F10:
				if (m_macro.IsRecording()) {
					m_macro.EndRecording();
					wxLogDebug(wxT("Macro Recording Stopped"));
				}
				else {
					m_macro.Clear();
					m_macro.StartRecording();
					wxLogDebug(wxT("Macro Recording Started"));
				}
				break;

			case WXK_F11:
				PlayMacro();
				break;
	#endif //__WXDEBUG__

			case WXK_LEFT:
			case WXK_NUMPAD_LEFT:
				CursorLeft(doSelect);
				break;

			case WXK_RIGHT:
			case WXK_NUMPAD_RIGHT:
				CursorRight(doSelect);
				break;

			case WXK_UP:
			case WXK_NUMPAD_UP:
				CursorUp(doSelect);
				break;

			case WXK_DOWN:
			case WXK_NUMPAD_DOWN:
				CursorDown(doSelect);
				break;

			case WXK_HOME:
			case WXK_NUMPAD_HOME:
			case WXK_NUMPAD_BEGIN:
				CursorToLineStart(true);
				if (event.ShiftDown()) SelectFromMovement(oldpos, GetPos());
				else RemoveAllSelections();
				lastaction = ACTION_NONE;
				break;

			case WXK_END:
			case WXK_NUMPAD_END:
				CursorToLineEnd();
				if (event.ShiftDown()) SelectFromMovement(oldpos, GetPos());
				else RemoveAllSelections();
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
					const bool delWord = event.ControlDown();
					Delete(delWord);
				}
				break;

			case WXK_BACK:
				{
					const bool delWord = event.ControlDown();
					Backspace(delWord);
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
				else m_parentFrame.ShowCommandMode();
				break;

			default:
				// If we process alt then menu shortcuts don't work
				if (event.AltDown()) {
					event.Skip();
					return;
				}

				// Ignore unhandled keycodes
				if (key >= WXK_START && key <= WXK_COMMAND) break;
				//if (key >= WXK_SPECIAL1 && key <= WXK_SPECIAL20) break;

				//if (wxIsprint(c)) { // Normal chars (does not work with 'ae', 'oslash', 'a-circle', etc.??)
				if ((unsigned int)c > 31) // Normal chars
					InsertChar(c);
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
		if (commandStack[0] == 26) 
			cmd_Undo(0, commandStack, true);
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
		RedoDlg dlg(this, &m_parentFrame, m_catalyst, GetId(), currentDoc);
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
				if (idx < childlist.end() - 1) {
					const doc_id di(DRAFT, doc.GetDocumentID(), *(idx+1));
					SetDocument(di);
				}
			}
		}
	cxENDLOCK

	return !end;
}

void EditorCtrl::SetPos(unsigned int pos) {
	wxASSERT(0 <= pos && pos <= m_lines.GetLength());
	m_lines.SetPos(pos);
}

void EditorCtrl::SetPos(int line, int column) {
	// line & colums indexes starts from 1
	if (line > 0) CursorToLine(line);
	if (column > 0) CursorToColumn(column);
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

void EditorCtrl::CursorUp(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorUp"), wxT("select"), (select == SEL_SELECT));
	}

	const int oldpos = m_lines.GetPos();

	if (lastaction == ACTION_UP) m_lines.MovePosUp(lastxpos);
	else {
		const wxPoint cpos = m_lines.GetCaretPos();
		lastxpos = cpos.x;
		m_lines.MovePosUp();
	}

	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(oldpos, m_lines.GetPos());

	lastaction = ACTION_UP;
}

void EditorCtrl::CursorDown(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorDown"), wxT("select"), (select == SEL_SELECT));
	}

	const int oldpos = m_lines.GetPos();

	if (lastaction == ACTION_DOWN) m_lines.MovePosDown(lastxpos);
	else {
		const wxPoint cpos = m_lines.GetCaretPos();
		lastxpos = cpos.x;
		m_lines.MovePosDown();
	}

	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(oldpos, m_lines.GetPos());

	lastaction = ACTION_DOWN;
}

void EditorCtrl::CursorLeft(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorLeft"), wxT("select"), (select == SEL_SELECT));
	}

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
			if (pos >= tabWidth && IsSpaces(pos - tabWidth, pos))
				prevpos = pos - tabWidth;
		}

		// Check if we are moving over a fold
		unsigned int fold_start;
		if (IsPosInFold(prevpos, &fold_start))
			prevpos = fold_start;

		m_lines.SetPos(prevpos); // Caret will be moved in DrawLayout()
	}
	
	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(pos, m_lines.GetPos());

	lastaction = ACTION_NONE;
}

void EditorCtrl::CursorRight(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorRight"), wxT("select"), (select == SEL_SELECT));
	}

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
			if (pos + tabWidth <= m_lines.GetLength() && IsSpaces(pos, pos + tabWidth))
				nextpos = pos + tabWidth;
		}

		// Check if we are moving over a fold
		unsigned int fold_end;
		if (IsPosInFold(nextpos, NULL, &fold_end)) {
			if (fold_end == GetLength()) return;
			nextpos = fold_end;
		}

		m_lines.SetPos(nextpos); // Caret will be moved in DrawLayout()
	}
	
	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(pos, m_lines.GetPos());

	lastaction = ACTION_NONE;
}

void EditorCtrl::CursorWordLeft(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorWordLeft"), wxT("select"), (select == SEL_SELECT));
	}

	const unsigned int oldpos = m_lines.GetPos();
	unsigned int pos = oldpos;

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
		if (IsPosInFold(pos, &fold_start))
			pos = fold_start;

		m_lines.SetPos(pos);
	}

	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(oldpos, pos);
}

void EditorCtrl::CursorWordRight(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorWordRight"), wxT("select"), (select == SEL_SELECT));
	}

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
	if (IsPosInFold(pos, NULL, &fold_end))
		pos = fold_end;

	m_lines.SetPos(pos);

	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(oldpos, pos);
}

void EditorCtrl::CursorToHome(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorToHome"), wxT("select"), (select == SEL_SELECT));
	}
	const unsigned int oldpos = m_lines.GetPos();

	SetPos(0);

	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(oldpos, 0);
}

void EditorCtrl::CursorToEnd(SelAction select) {
	if (m_macro.IsRecording()) {
		m_macro.Add(wxT("CursorToEnd"), wxT("select"), (select == SEL_SELECT));
	}

	const unsigned int oldpos = m_lines.GetPos();
	unsigned int pos = GetLength();

	// Check if end is in a fold
	unsigned int fold_start;
	if (IsPosInFold(pos, &fold_start))
		pos = fold_start;

	SetPos(pos);

	// Handle selection
	if (select == SEL_REMOVE) m_lines.RemoveAllSelections();
	else if (select == SEL_SELECT) SelectFromMovement(oldpos, pos);
}

void EditorCtrl::CursorToLine(unsigned int line) {
	if (line == 0) return; // line id's start from 1
	if (--line >= m_lines.GetLineCount()) return CursorToEnd(SEL_IGNORE);

	m_lines.SetPos(m_lines.GetLineStartpos(line));
}

void EditorCtrl::CursorToColumn(unsigned int column) {
	if (column == 0) return; // column id's start from 1
	--column;

	const unsigned int curLine = m_lines.GetCurrentLine();
	const unsigned int lineStart = m_lines.GetLineStartpos(curLine);
	const unsigned int lineEnd = m_lines.GetLineEndpos(curLine);

	unsigned int newpos;
	cxLOCKDOC_READ(m_doc)
		newpos = doc.GetValidCharPos(lineStart + column);
	cxENDLOCK
	m_lines.SetPos(wxMin(newpos, lineEnd));
}

void EditorCtrl::CursorToLineStart(bool soft) {
	const unsigned int oldpos = GetPos();
	const unsigned int currentLine = m_lines.GetCurrentLine();
	const unsigned int startOfLine = m_lines.GetLineStartpos(currentLine);

	if (!soft) m_lines.SetPos(startOfLine);
	else {
		const unsigned int indentPos = m_lines.GetLineIndentPos(currentLine);

		if (indentPos < oldpos) {
			// Move to first text after indentation
			m_lines.SetPos(indentPos);
		}
		else {
			if (oldpos == startOfLine) {
				// Move back to text after indentation
				m_lines.SetPos(indentPos);
			}
			else {
				// Move to start-of-line
				m_lines.SetPos(startOfLine);
			}
		}
	}
}

void EditorCtrl::CursorToLineEnd() {
	m_lines.SetPos(m_lines.GetLineEndpos(m_lines.GetCurrentLine()));
}

void EditorCtrl::CursorToNextChar(wxChar c) {
	const unsigned int oldpos = GetPos();

	search_result sr;
	cxLOCKDOC_READ(m_doc)
		const unsigned int startpos = doc.GetNextCharPos(oldpos); // don't find current
		sr = doc.Find(c, startpos, true, GetLength());
	cxENDLOCK
	if (sr.error_code < 0) return; // no match

	m_lines.SetPos(sr.start);
}

void EditorCtrl::CursorToPrevChar(wxChar c) {
	const unsigned int oldpos = GetPos();

	search_result sr;
	cxLOCKDOC_READ(m_doc)
		sr = doc.FindBackwards(c, oldpos, true);
	cxENDLOCK
	if (sr.error_code < 0) return; // no match

	m_lines.SetPos(sr.start);
}

void EditorCtrl::CursorToWordStart(bool bigword) {
	unsigned int pos = m_lines.GetPos();

	cxLOCKDOC_READ(m_doc)
		if (pos < doc.GetLength()) {
			wxChar prev_char = doc.GetChar(pos);
			if (bigword) {
				for (pos = doc.GetNextCharPos(pos); pos < doc.GetLength(); pos = doc.GetNextCharPos(pos)) {
					const wxChar c = doc.GetChar(pos);
					if (wxIsspace(prev_char) && (Isalnum(c) || c == wxT('_') || wxIspunct(c))) break;
					prev_char = c;
				}
			}
			else {
				for (pos = doc.GetNextCharPos(pos); pos < doc.GetLength(); pos = doc.GetNextCharPos(pos)) {
					const wxChar c = doc.GetChar(pos);
					if ((wxIsspace(prev_char) || wxIspunct(prev_char)) && (Isalnum(c) || c == wxT('_'))) break;
					if (!wxIspunct(prev_char) && wxIspunct(c)) break;
					prev_char = c;
				}
			}
		}
	cxENDLOCK

	// Check if we are moving over a fold
	unsigned int fold_end;
	if (IsPosInFold(pos, NULL, &fold_end))
		pos = fold_end;

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToWordEnd(bool bigword) {
	unsigned int pos = m_lines.GetPos();

	cxLOCKDOC_READ(m_doc)
		if (pos < doc.GetLength()) {
			wxChar prev_char = doc.GetChar(pos);
			if (bigword) {
				for (pos = doc.GetNextCharPos(pos); pos < doc.GetLength(); pos = doc.GetNextCharPos(pos)) {
					const wxChar c = doc.GetChar(pos);
					if ((Isalnum(prev_char) || prev_char == wxT('_') || wxIspunct(prev_char)) && wxIsspace(c)) break;
					prev_char = c;
				}
			}
			else {
				for (pos = doc.GetNextCharPos(pos); pos < doc.GetLength(); pos = doc.GetNextCharPos(pos)) {
					const wxChar c = doc.GetChar(pos);
					if ((Isalnum(prev_char) || prev_char == wxT('_')) && (wxIsspace(c) || wxIspunct(c))) break;
					if (wxIspunct(prev_char) && !wxIspunct(c)) break;
					prev_char = c;
				}
			}
		}
	cxENDLOCK

	// Check if we are moving over a fold
	unsigned int fold_end;
	if (IsPosInFold(pos, NULL, &fold_end))
		pos = fold_end;

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToPrevWordStart(bool bigword) {
	unsigned int pos = m_lines.GetPos();
	if (pos == 0) return;

	cxLOCKDOC_READ(m_doc)
		pos = doc.GetPrevCharPos(pos);
		if (pos) {
			wxChar c = doc.GetChar(pos);
			unsigned int prev_pos = doc.GetPrevCharPos(pos);
			if (bigword) {
				for (;;) {
					wxChar prev_char = doc.GetChar(prev_pos);
					if (wxIsspace(prev_char) && (Isalnum(c) || c == wxT('_') || wxIspunct(c))) break;

					c = prev_char;
					pos = prev_pos;
					if (pos == 0) break;
					prev_pos = doc.GetPrevCharPos(pos);
				}
			}
			else {
				for (;;) {
					wxChar prev_char = doc.GetChar(prev_pos);
					if ((Isalnum(c) || c == wxT('_')) && (wxIsspace(prev_char) || wxIspunct(prev_char))) break;
					if (wxIspunct(c) && !wxIspunct(prev_char)) break;

					c = prev_char;
					pos = prev_pos;
					if (pos == 0) break;
					prev_pos = doc.GetPrevCharPos(pos);
				}
			}
		}
	cxENDLOCK

	// Check if we are moving over a fold
	unsigned int fold_start;
	if (IsPosInFold(pos, &fold_start))
		pos = fold_start;

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToNextLine() {
	const unsigned int nextLine = m_lines.GetCurrentLine()+1;
	if (nextLine >= m_lines.GetLineCount()) return;
	
	const unsigned int indentPos = m_lines.GetLineIndentPos(nextLine);
	m_lines.SetPos(indentPos);
}

void EditorCtrl::CursorToPrevLine() {
	const unsigned int currentLine = m_lines.GetCurrentLine();
	if (currentLine == 0) return;
	const unsigned int prevLine = currentLine-1;
	
	const unsigned int indentPos = m_lines.GetLineIndentPos(prevLine);
	m_lines.SetPos(indentPos);
}

void EditorCtrl::CursorToNextSentence() {
	unsigned int pos = m_lines.GetPos();

	cxLOCKDOC_READ(m_doc)
		if (pos < doc.GetLength()) {
			wxChar prev_char = doc.GetChar(pos);
			bool after = false;
			for (pos = doc.GetNextCharPos(pos); pos < doc.GetLength(); pos = doc.GetNextCharPos(pos)) {
				const wxChar c = doc.GetChar(pos);
				if (prev_char == '.' && wxIsspace(c)) after = true;
				if (after && !wxIsspace(c)) break;
				prev_char = c;
			}
		}
	cxENDLOCK

	// Check if we are moving over a fold
	unsigned int fold_end;
	if (IsPosInFold(pos, NULL, &fold_end))
		pos = fold_end;

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToPrevSentence() {
	unsigned int pos = m_lines.GetPos();
	if (pos == 0) return;

	cxLOCKDOC_READ(m_doc)
		pos = doc.GetPrevCharPos(pos);
		if (pos) {
			wxChar c = doc.GetChar(pos);
			unsigned int prev_pos = doc.GetPrevCharPos(pos);
			unsigned int lastpos = 0;
			bool after = false;
			
			for (;;) {
				if (!wxIsspace(c)) lastpos = pos;

				wxChar prev_char = doc.GetChar(prev_pos);
				if (prev_char == '.' && wxIsspace(c)) {
					if (after && lastpos != 0) {
						pos = lastpos;
						break;
					}
					else after = true;
				}

				c = prev_char;
				pos = prev_pos;
				if (pos == 0) break;
				prev_pos = doc.GetPrevCharPos(pos);
			}
		}
	cxENDLOCK

	// Check if we are moving over a fold
	unsigned int fold_start;
	if (IsPosInFold(pos, &fold_start))
		pos = fold_start;

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToNextParagraph() {
	unsigned int pos = m_lines.GetPos();

	search_result sr;
	cxLOCKDOC_READ(m_doc)
		sr = doc.RegExFind(wxT("\\n\\s*\\n\\s*(?=\\S)"), pos, true, NULL, GetLength());
	cxENDLOCK
	if (sr.error_code < 0) return; // no match

	m_lines.SetPos(sr.end);
}

void EditorCtrl::CursorToParagraphStart() {
	unsigned int pos = m_lines.GetPos();
	const unsigned int oldpos = pos;

	search_result sr;
	cxLOCKDOC_READ(m_doc)
		const unsigned int startpos = doc.GetPrevCharPos(oldpos); // don't find current
		sr = doc.RegExFindBackwards(wxT("(\\A|\\n\\s*\\n\\s*)(?=\\S)"), startpos, true);
	cxENDLOCK
	if (sr.error_code < 0) return; // no match

	m_lines.SetPos(sr.end);
}

void EditorCtrl::CursorToNextSymbol() {
	vector<SymbolRef> symbols;
	GetSymbols(symbols);
	if (symbols.empty()) return;

	unsigned int pos = m_lines.GetPos();
	for (std::vector<SymbolRef>::const_iterator p = symbols.begin(); p != symbols.end(); ++p) {
		if (p->start > pos) {
			pos = p->start;
			break;
		}
	}

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToPrevSymbol() {
	vector<SymbolRef> symbols;
	GetSymbols(symbols);
	if (symbols.empty()) return;

	unsigned int pos = m_lines.GetPos();
	for (std::vector<SymbolRef>::reverse_iterator p = symbols.rbegin(); p != symbols.rend(); ++p) {
		if (pos > p->start) {
			pos = p->start;
			break;
		}
	}

	m_lines.SetPos(pos);
}

void EditorCtrl::CursorToNextCurrent() {
	const unsigned int oldpos = GetPos();
	const interval iv = IsSelected() ? GetSelections()[0] : GetWordIv(oldpos);
	if (iv.empty()) return;

	const wxString current = GetText(iv.start, iv.end);

	search_result sr;
	cxLOCKDOC_READ(m_doc)
		sr = doc.Find(current, iv.end, true, GetLength());
	cxENDLOCK
	if (sr.error_code < 0) return; // no match

	m_lines.SetPos(sr.start);
}

void EditorCtrl::CursorToPrevCurrent() {
	const unsigned int oldpos = GetPos();
	const interval iv = IsSelected() ? GetSelections()[0] : GetWordIv(oldpos);
	if (iv.empty()) return;

	const wxString current = GetText(iv.start, iv.end);

	search_result sr;
	cxLOCKDOC_READ(m_doc)
		const unsigned int startpos = doc.GetPrevCharPos(iv.start); // don't find current
		sr = doc.FindBackwards(current, startpos, true);
	cxENDLOCK
	if (sr.error_code < 0) return; // no match

	m_lines.SetPos(sr.start);
}

void EditorCtrl::SelectFromMovement(unsigned int oldpos, unsigned int newpos, bool makeVisible, bool multiSelect) {
	wxASSERT(0 <= oldpos && oldpos <= m_lines.GetLength());
	wxASSERT(0 <= newpos && newpos <= m_lines.GetLength());
	if (oldpos == newpos) return; // Empty selection

	// If alt key is down do a block selection
	if (!wxGetKeyState(WXK_ALT))
		m_blockKeyState = BLOCKKEY_NONE; // may not have caught keyup

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

	if (!m_lines.IsSelected())
		sel = m_lines.AddSelection(oldpos, newpos);
	else {
		// Get the selections
		const vector<interval>& selections = m_lines.GetSelections();

		// Check if we are at the start or end of a selection, and extend that one.
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

		// Otherwise, we were not in connection with any current selections so start a new one.
		if (!multiSelect) m_lines.RemoveAllSelections();
		sel = m_lines.AddSelection(oldpos, newpos);
	}

	if (makeVisible && sel != -1)
		MakeSelectionVisible();
}

bool EditorCtrl::GetNextObjectScope(const wxString& scope, size_t pos, interval& iv, interval& iv_inner) const {
	return m_syntaxstyler.GetNextMatch(scope, pos, iv, iv_inner);
}

bool EditorCtrl::GetNextObjectWords(size_t count, size_t pos, interval& iv, interval& iv_inner) const {
	const unsigned int len = m_lines.GetLength();
	if (pos == len) return false;

	cxLOCKDOC_READ(m_doc)
		// Move to start of first word
		wxChar c = doc.GetChar(pos);
		if (IsWordChar(c)) {
			while (pos) {
				const unsigned int prevpos = doc.GetPrevCharPos(pos);
				c = doc.GetChar(prevpos);
				if (!IsWordChar(c)) break;
				pos = prevpos;
			}
		}
		else {
			while (pos < len && !IsWordChar(c)) {
				pos = doc.GetNextCharPos(pos);
				c = doc.GetChar(pos);
			}
			if (pos == len) return false; // no words left
		}

		iv.start = iv_inner.start = pos;
		while (pos < len && count > 0) {
			// Advance to word end
			c = doc.GetChar(pos);
			while (pos < len && IsWordChar(c)) {
				pos = doc.GetNextCharPos(pos);
				c = doc.GetChar(pos);
			}

			iv_inner.end = pos;

			// Advance to next word
			c = doc.GetChar(pos);
			while (pos < len && !IsWordChar(c)) {
				pos = doc.GetNextCharPos(pos);
				c = doc.GetChar(pos);
			}

			--count;
		}

		iv.end = pos;
	cxENDLOCK

	return true;
}

bool EditorCtrl::GetNextObjectBlock(wxChar brace, size_t pos, interval& iv, interval& iv_inner) {
	const unsigned int len = m_lines.GetLength();
	if (pos >= len) return false;

	char startBrace;
	char endBrace;
	switch (brace) {
	case '{': startBrace = '{'; endBrace = '}'; break;
	case '(': startBrace = '('; endBrace = ')'; break;
	case '[': startBrace = '['; endBrace = ']'; break;
	case '<': startBrace = '<'; endBrace = '>'; break;
	case '"': startBrace = '"'; endBrace = '"'; break;
	case '\'': startBrace = '\''; endBrace = '\''; break;
	default: return false;
	}

	cxLOCKDOC_READ(m_doc)
		// Advance to first startbrace
		doc_byte_iter dbi(doc, pos);
		while (dbi.GetIndex() < (int)len) {
			if (*dbi == startBrace) {
				iv.start = dbi.GetIndex();
				break;
			}
			else if (*dbi == '\\') ++dbi; // jump escaped chars
			
			++dbi;
		}

		// Find endbrace
		++dbi; // ignore startbrace
		while (dbi.GetIndex() < (int)len) {
			// TODO: nesting
			if (*dbi == endBrace) {
				iv.end = dbi.GetIndex()+1;
				iv_inner.Set(iv.start+1, iv.end-1);
				return true;
			}
			else if (*dbi == '\\') ++dbi; // jump escaped chars
			
			++dbi;
		}
	cxENDLOCK

	return false;
}

bool EditorCtrl::GetNextObjectSentence(size_t pos, interval& iv, interval& iv_inner) {
	const unsigned int len = m_lines.GetLength();
	if (pos >= len) return false;

	cxLOCKDOC_READ(m_doc)
		// Find sentence start
		doc_byte_iter dbi(doc, pos);
		iv.start = iv_inner.start = 0;
		if (pos > 0) {
			--dbi;
			for (;;) {
				if (*dbi == '.') {
					++dbi;
					// Advance past whitespace
					while (dbi.GetIndex() < (int)len && isspace(*dbi)) ++dbi;
					
					iv.start = iv_inner.start = dbi.GetIndex();
					break;
				}

				if (dbi.GetIndex() == 0) break;
				--dbi;
			}
		}

		// Find sentence end
		dbi.SetIndex(pos);
		iv.end = iv_inner.end = len;
		while (dbi.GetIndex() < (int)len) {
			if (*dbi == '.') {
				++dbi;
				iv.end = iv_inner.end = dbi.GetIndex();
				
				// Trailing whitespace
				while (dbi.GetIndex() < (int)len && isspace(*dbi)) {
					++dbi;
					iv.end = dbi.GetIndex();
				}

				// Period may also be used for abbr.
				if (dbi.GetIndex() < (int)len && islower(*dbi)) continue;
				else return true;
			}
			++dbi;
		}
	cxENDLOCK

	return true;
}

bool EditorCtrl::GetNextObjectParagraph(size_t pos, interval& iv, interval& iv_inner) {
	const unsigned int len = m_lines.GetLength();
	if (pos >= len) return false;

	cxLOCKDOC_READ(m_doc)
		// Find paragraph start
		doc_byte_iter dbi(doc, pos);
		iv.start = iv_inner.start = 0;
		if (pos > 0) {
			--dbi;
			for (;;) {
				if (*dbi == '\n') {
					const unsigned int start = dbi.GetIndex()+1;
					
					// Advance past any whitespace
					bool isPara = true;
					while (dbi.GetIndex() > 0) {
						if (*dbi == '\n') { // double newline marks paragraph start
							iv.start = iv_inner.start = start;
							break;
						}
						else if (isspace(*dbi)) --dbi;
						else {
							isPara = false;
							break;
						}
					}
					
					if (isPara) break;
				}

				if (dbi.GetIndex() == 0) break;
				--dbi;
			}
		}

		// Find paragraph end
		dbi.SetIndex(pos);
		iv.end = iv_inner.end = len;
		while (dbi.GetIndex() < (int)len) {
			if (*dbi == '\n') {
				const unsigned int end = dbi.GetIndex();
				
				// Trailing whitespace
				++dbi;
				bool isPara = false;
				while (dbi.GetIndex() < (int)len) {
					if (*dbi == '\n') isPara = true;
					else if (!isspace(*dbi)) break;
					++dbi;
				}

				if (isPara) {
					iv_inner.end = end;
					iv.end = dbi.GetIndex();
					return true;
				}
			}
			++dbi;
		}
	cxENDLOCK

	return true;
}

bool EditorCtrl::GetContainingObjectString(wxChar brace, size_t pos, interval& iv, interval& iv_inner) {
	const unsigned int len = m_lines.GetLength();
	if (pos >= len) return false;

	char t;
	switch (brace) {
	case '"': t = '"'; break;
	case '\'': t = '\''; break;
	default: return false;
	}

	const unsigned int lineid = m_lines.GetCurrentLine();
	const unsigned int linestart = m_lines.GetLineStartpos(lineid);
	const unsigned int lineend = m_lines.GetLineEndpos(lineid);

	// count brackets from start of line
	bool startfound = false;
	cxLOCKDOC_READ(m_doc)
		doc_byte_iter dbi(doc, linestart);
		while (dbi.GetIndex() < (int)lineend) {
			if (*dbi == t) {
				const unsigned int p = dbi.GetIndex();
				if (p > pos) {
					if (!startfound) return false;
					else {
						iv.end = p+1;
						iv_inner.Set(iv.start+1, p);
						return true;
					}
				}
				else {
					iv.start = p;
					startfound = true;
				}
			}
			else if (*dbi == '\\') ++dbi; // jump escaped

			++dbi;
		}
	cxENDLOCK

	return false;
}


bool EditorCtrl::GetContainingObjectBlock(wxChar brace, size_t pos, interval& iv, interval& iv_inner) {
	const unsigned int len = m_lines.GetLength();
	if (pos >= len) return false;

	wxChar endBrace;
	switch (brace) {
	case '{': endBrace = '}'; break;
	case '(': endBrace = ')'; break;
	case '[': endBrace = ']'; break;
	case '<': endBrace = '>'; break;
	default: return false;
	}

	// Is first char start brace?
	bool onBrace = false;
	cxLOCKDOC_READ(m_doc)
		const wxChar c = doc.GetChar(pos);
		if (c == brace) onBrace = true;
	cxENDLOCK
	if (onBrace) {
		unsigned int endpos;
		if (!FindMatchingBracket(pos, endpos)) return false;
		iv.Set(pos, endpos+1);
		iv_inner.Set(pos+1, endpos);
		return true;
	}

	cxLOCKDOC_READ(m_doc)
		unsigned int p = pos;
		unsigned int endpos = 0;
		unsigned int count = 0;

		// Advance to first unmatched brace
		while (p < len) {
			const wxChar c = doc.GetChar(p);
			if (c == brace) ++count;
			else if (c == endBrace) {
				if (count == 0) { // unmatched endbrace found
					endpos = p;
					break;
				}
				else --count;
			}
			else if (c == '\\') p = doc.GetNextCharPos(p); // jump escaped
			
			p = doc.GetNextCharPos(p);
		}
		if (endpos == 0) return false;
		
		// Backtrace to starter
		unsigned int startpos;
		if (!FindMatchingBracket(endpos, startpos)) return false;
		iv.Set(startpos, endpos+1);
		iv_inner.Set(startpos+1, endpos);
		return true;
	cxENDLOCK
}


bool EditorCtrl::IsCaretVisible() {
	const wxSize caretsize = caret->GetSize();
	const wxPoint cpos = m_lines.GetCaretPos();
	const wxSize clientsize = GetClientSize();

	// Check vertically
	if (cpos.y + caretsize.y < scrollPos) return false;
	if (cpos.y >= scrollPos + clientsize.y) return false;

	// Check horizontally
	if (cpos.x + caretsize.x < m_scrollPosX) return false;

	const int sizeX = ClientWidthToEditor(clientsize.x);
	if (cpos.x >= m_scrollPosX + sizeX) return false;

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

	const int lineheight = m_lines.GetLineHeight();
	if (cpos.y + lineheight > scrollPos + clientsize.y) {
		scrollPos = (cpos.y + lineheight) - clientsize.y;
		return true;
	}

	// Check if the caret have moved outside visible display horizontally
	if (cpos.x < m_scrollPosX) {
		m_scrollPosX = wxMax(cpos.x - 50, 0);
		return true;
	}

	const int sizeX = ClientWidthToEditor(clientsize.x);
	if (cpos.x >= m_scrollPosX + sizeX) {
		const int textWidth = m_lines.GetWidth();
		const int maxScrollPos = (textWidth < sizeX) ? 0 : textWidth + m_caretWidth - sizeX; // room for caret at end
		m_scrollPosX = (cpos.x + 50) - sizeX;
		if (m_scrollPosX > maxScrollPos) m_scrollPosX = maxScrollPos;
		return true;
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
	if (sel_start.y < scrollPos) scrollPos = sel_start.y;
	else {
		const int lineheight = m_lines.GetLineHeight();
		if (sel_end.y + lineheight >= scrollPos + clientsize.y) {
			// Make sure that we can see as much of the selection as possible
			scrollPos = wxMin(sel_start.y, (sel_end.y + lineheight) - clientsize.y);
		}
	}

	const int sizeX = ClientWidthToEditor(clientsize.x);

	// Horizontally for End
	if (sel_end.x < m_scrollPosX)
		m_scrollPosX = wxMax(sel_end.x - 50, 0);
	else if (sel_end.x >= m_scrollPosX + sizeX)
		m_scrollPosX = (sel_end.x + 50) - sizeX;

	// Horizontally for Start
	if (sel_start.x < m_scrollPosX)
		m_scrollPosX = wxMax(sel_start.x - 50, 0);
	else if (sel_start.x >= m_scrollPosX + sizeX)
		m_scrollPosX = (sel_start.x + 50) - sizeX;

	// NOTE: Will first be visible on next redraw
}

void EditorCtrl::KeepCaretAlive(bool keepAlive) {
	caret->KeepAlive(keepAlive);
}

wxString EditorCtrl::GetSelFirstLine() {
	// returns the first line of the selection (if there is *one* selection)
	if (!m_lines.IsSelected()) return wxT("");

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

wxString EditorCtrl::GetFirstSelection() const {
	const interval* const sel = m_lines.FirstSelection();
	if (!sel) return wxEmptyString;

	// Get text
	cxLOCKDOC_READ(m_doc)
		return doc.GetTextPart(sel->start, sel->end);
	cxENDLOCK
}

// Get the selected text
wxString EditorCtrl::GetSelText() const {
	if (!m_lines.IsSelected()) return wxT("");

	wxString text;
	const vector<interval>& selections = m_lines.GetSelections();

	cxLOCKDOC_READ(m_doc)
		for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
			if (iv > selections.begin()) // Add newline between multiple selections
				text += wxT('\n');
			text += doc.GetTextPart((*iv).start, (*iv).end);
		}
	cxENDLOCK

	return text;
}

void EditorCtrl::SetEnv(cxEnv& env, bool isUnix, const tmBundle* bundle) {
#ifdef __WXMSW__
	if (isUnix) eDocumentPath::InitCygwin(true);
#endif // __WXMSW__

	// Load existing enviroment
	env.SetToCurrent();

	// Add any keys configured in app settings
	env.SetEnv(eGetSettings().env);

	// Add app keys
	env.AddSystemVars(isUnix, GetAppPaths().AppPath());

	// Add document/editor keys

	// TM_EDITOR_ID (used for scripting via ipc)
	const int id = -GetId(); // internally id is negative, but pass to user as positive
	env.SetEnv(wxT("TM_EDITOR_ID"), wxString::Format(wxT("%u"), id));

	// TM_FILENAME
	// note: in case of remote files, this is of the buffer file
	const wxString name = m_path.GetFullName();
	env.SetIfValue(wxT("TM_FILENAME"), name);

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
	if (IsSelected())
		env.SetEnv(wxT("TM_SELECTED_TEXT"), GetFirstSelection());

	// TM_CURRENT_WORD
	const wxString word = GetCurrentWord();
	env.SetIfValue(wxT("TM_CURRENT_WORD"), word);

	// TM_CURRENT_LINE
	const wxString line = GetCurrentLine();
	env.SetIfValue(wxT("TM_CURRENT_LINE"), line);

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
	env.SetEnv(wxT("TM_SOFT_TABS"), m_parentFrame.IsSoftTabs() ? wxT("YES") : wxT("NO"));

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
	env.SetIfValue(wxT("TM_MODE"), syntaxName);

	// TM_PROJECT_DIRECTORY
	if (m_parentFrame.HasProject() && !m_parentFrame.IsProjectRemote()) {
		const wxFileName& prjPath = m_parentFrame.GetRootPath();
		env.SetEnv(wxT("TM_PROJECT_DIRECTORY"), isUnix ? eDocumentPath::WinPathToCygwin(prjPath) : prjPath.GetPath());

		// Set project specific env vars
		env.SetEnv(m_parentFrame.GetProjectEnv());
	}

	// TM_SELECTED_FILE & TM_SELECTED_FILES
	const wxArrayString selections = m_parentFrame.GetSelectionsInProject();
	if (!selections.IsEmpty()) {
		env.SetEnv(wxT("TM_SELECTED_FILE"), isUnix ? eDocumentPath::WinPathToCygwin(selections[0]) : selections[0]);

		wxString sels;
		for (unsigned int i = 0; i < selections.GetCount(); ++i) {
			sels += (i ? wxT(" '") : wxT("'"));
			sels += (isUnix ? eDocumentPath::WinPathToCygwin(selections[i]) : selections[i]);
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

void EditorCtrl::RunCurrentSelectionAsCommand(bool doReplace) {
	vector<char> command;
	unsigned int start;
	unsigned int end;

	const interval* const selection = m_lines.FirstSelection();
	if (selection) selection->Get(start, end);
	else {
		// Get current line
		const unsigned int cl = m_lines.GetCurrentLine();
		m_lines.GetLineExtent(cl, start, end);
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

	{
		// Set a busy cursor
		// will be reset when leaving scope
		wxBusyCursor wait;

		cxEnv env;
		SetEnv(env);
		const wxString output = ShellRunner::RunShellCommand(command, env);
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

	} // internal scope for busy cursor

	Freeze();

	MakeCaretVisible();
	DrawLayout();
}

// # no evt.skip() as we don't want the control to erase the background
void EditorCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

void EditorCtrl::OnFocus(wxFocusEvent& WXUNUSED(event)) {
	if (!m_parentFrame.IsCommandMode()) ClearSearchRange(true);
}

void EditorCtrl::OnMouseLeftDown(wxMouseEvent& event) {
	ClearSearchRange();
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
	if (m_tripleClicks.TripleClickedLine((int)line_id)) {
		SelectLine(line_id, event.ControlDown());
	}
	else {
		MakeCaretVisible();

		// Check if we are starting a drag
		bool isDragging = false;
		if (!event.ShiftDown() && !fp.xy_outbound && m_lines.IsSelected() && !m_lines.IsSelectionShadow()) {
			const vector<interval>& sels = m_lines.GetSelections();
			for (vector<interval>::const_iterator p = sels.begin(); p != sels.end(); ++p) {
				if ((int)p->start <= fp.pos && fp.pos <= (int)p->end) {
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
			if (m_lines.IsSelectionShadow() || !(event.ControlDown() || event.ShiftDown())) {
				if (fp.xy_outbound) m_lines.RemoveAllSelections();
				else m_lines.RemoveAllSelections(true, fp.pos);
				m_currentSel = -1;
				m_blocksel_ids.clear();
			}

			// Check if we should make new selection
			if (event.ShiftDown()) // SHIFT selects from the last point
				SelectFromMovement(lastpos, fp.pos, false);
			else if (event.ControlDown()) // CTRL starts a new multi selection at this point
				m_currentSel = m_lines.AddSelection(fp.pos, fp.pos);
		}
	}

	m_tripleClicks.Reset();

	DrawLayout();

	// Make sure we capure all mouse events; this is released in OnMouseLeftUp()
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
		selectMenu->Append(wxID_SELECTALL, _("&All\tCtrl+A"), _("Select All"));
		selectMenu->Append(EditorFrame::MENU_SELECTWORD, _("&Word\tCtrl+Shift+W"), _("Select Word"));
		selectMenu->Append(EditorFrame::MENU_SELECTLINE, _("&Line\tCtrl+Shift+L"), _("Select Line"));
		selectMenu->Append(EditorFrame::MENU_SELECTSCOPE, _("&Current Scope\tCtrl+Shift+Space"), _("Select Current Scope"));
		selectMenu->Append(EditorFrame::MENU_SELECTFOLD, _("Current &Fold\tShift-F1"), _("Select Current Fold"));
		contextMenu.Append(EditorFrame::MENU_SELECT, _("&Select"), selectMenu,  _("Select"));
	contextMenu.AppendSeparator();
	contextMenu.Append(wxID_UNDO, _("&Undo\tCtrl+Z"), _("Undo"));
	contextMenu.Append(wxID_REDO, _("&Redo\tCtrl+Y"), _("Redo"));

	if (!inSelection) {
		contextMenu.Enable(wxID_CUT, false);
		contextMenu.Enable(wxID_COPY, false);
	}

	if (fromKey) PopupMenu(&contextMenu, GetCaretPoint());
	else PopupMenu(&contextMenu);
}

static bool should_start_drag(const wxPoint& start, const wxPoint& end) {
	// If no start position was given, we're not dragging.
	if (start == wxDefaultPosition) return false;

	// Drag metric can be changed by user at any time, so always get it.
	const int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X);
	const int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y);

	return (abs(end.x - start.x) > drag_x_threshold) ||
			  (abs(end.y - start.y) > drag_y_threshold);
}

// Start drag-and-drop of selected text.
void EditorCtrl::StartDragSelectedText(void) {
	wxDropSource dragSource(this);
	if (m_lines.IsMultiSelected()) {
		MultilineDataObject* mdo = new MultilineDataObject;
		const vector<interval>& selections = m_lines.GetSelections();
		cxLOCKDOC_READ(m_doc)
			// Get the selected text
			for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv)
				mdo->AddText(doc.GetTextPart((*iv).start, (*iv).end));
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
}

void EditorCtrl::OnMouseMotion(wxMouseEvent& event) {
	// Editor-relative mouse position of this event
	const wxPoint mpos = ClientPosToEditor(event.GetX(), event.GetY());

	// Close tooltip on motion
	/*if (m_revTooltip.IsShown() && m_revTooltipMousePos != wxGetMousePosition()) {
		m_revTooltip.Hide();
	}*/

	const bool click_and_drag = event.LeftIsDown() && HasCapture();
	if (click_and_drag) {
		wxASSERT(0 <= m_sel_start && m_sel_start <= (int)m_lines.GetLength());
		wxASSERT(0 <= m_sel_end && m_sel_end <= (int)m_lines.GetLength());

		MakeCaretVisible();

		// Check if we should start dragging
		if (should_start_drag(m_dragStartPos, mpos)) {
			wxLogDebug(wxT("Starting text drag"));
			StartDragSelectedText();
			m_dragStartPos = wxDefaultPosition; // reset drag state
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
			// Find out what is under mouse
			const full_pos fp = m_lines.ClickOnLine(mpos.x, mpos.y);

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
				if (!m_lines.IsLineVirtual(line_id)) {
					unsigned int linestart, lineend;
					m_lines.GetLineExtent(line_id, linestart, lineend);

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
				if (!m_lines.IsSelected() || m_currentSel == -1)
					m_currentSel = m_lines.AddSelection(m_sel_start, m_sel_end);
				else
					m_currentSel = m_lines.UpdateSelection(m_currentSel, m_sel_start, m_sel_end);

				DrawLayout();
			}
		}
	}
	else {
		if (!m_foldTooltipTimer.IsRunning() && 0 <= mpos.y && mpos.y < m_lines.GetHeight()) {
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

	if (mpos.y < 0 || m_lines.GetHeight() <= mpos.y) return;

	// Find out what is under mouse
	const unsigned int line_id = m_lines.GetLineFromYPos(mpos.y);

	// Check if we are still hovering over a fold indicator
	if (line_id != m_foldTooltipLine) return;

	wxRect bRect = m_lines.GetFoldIndicatorRect(line_id);
	if (!bRect.Contains(mpos)) return;

	const vector<cxFold*> foldStack = GetFoldStack(line_id);
	if (foldStack.empty()) return;

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


void EditorCtrl::OnLeaveWindow(wxMouseEvent& WXUNUSED(event)) {
	/*if (m_revTooltip.IsShown() && m_revTooltipMousePos != wxGetMousePosition()) {
		m_revTooltip.Hide();
	}*/
}

int EditorCtrl::ShowPopupList(const vector<const tmAction*>& actionList) {
	// Create list of refs to real list
	vector<size_t> sortedList;
	sortedList.resize(actionList.size());
	for (size_t i = 0; i < sortedList.size(); ++i)
		sortedList[i] = i;

	sort(sortedList.begin(), sortedList.end(), CompareActionBundle(actionList));

	// Create the menu
	wxMenu listMenu;
	const tmBundle* bundle = NULL;
	unsigned int shortcut = 1;
	for (vector<size_t>::const_iterator p = sortedList.begin(); p != sortedList.end(); ++p) {
		const tmAction& a = *actionList[*p];
		
		// Add bundle title
		if (a.bundle && a.bundle != bundle) {
			wxString bundleName = a.bundle->name;
			bundleName.Replace(wxT("&"), wxT("&&"));
			wxMenuItem* item = listMenu.Append(-1, bundleName);
			item->Enable(false);
			bundle = a.bundle;
		}

		wxString itemText;
		if (a.bundle) itemText = wxT("  "); // slight indentation for bundle members
		itemText += a.name;
		itemText.Replace(wxT("&"), wxT("&&"));
		
		if (shortcut < 10) itemText += wxString::Format(wxT("\t&%u"), shortcut);
		else if (shortcut == 10) itemText += wxT("\t&0");
		++shortcut;

		// Add item 
		listMenu.Append(new PopupMenuItem(&listMenu, 1000+(*p), itemText));
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
		for (vector<unsigned int>::reverse_iterator p = m_blocksel_ids.rbegin(); p != m_blocksel_ids.rend(); ++p)
			m_lines.RemoveSelection(*p);
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

	// Start timing for a triple click.
	m_tripleClicks.Start(m_lines.GetLineFromCharPos(fp.pos));

	if (event.AltDown()) SelectScope();
	else {
		// Check if we should multiselect
		bool multiselect = event.ControlDown();

		// Select the word clicked on
		SelectWordAt(fp.pos, multiselect);
		wxLogDebug(wxT("DClick done %d"), GetPos());
	}

	DrawLayout();

	// Make sure we capure all mouse events; released in OnMouseLeftUp()
	//wxLogDebug(wxT("EditorCtrl::CaptureMouse() %d"), event.LeftDown());
	CaptureMouse();
}

void EditorCtrl::DoVerticalWheelScroll(wxMouseEvent& event) {
	const wxSize size = GetClientSize();
	int pos = scrollPos;
	const double rotation = event.GetWheelRotation();

	if (event.GetLinesPerAction() == (int)UINT_MAX) { // signifies to scroll a page
		wxScrollWinEvent newEvent;
		newEvent.SetOrientation(wxVERTICAL);
		newEvent.SetEventObject(this);
		newEvent.SetEventType(rotation>0 ? wxEVT_SCROLLWIN_PAGEUP : wxEVT_SCROLLWIN_PAGEDOWN);
        ProcessEvent(newEvent);
		return;
	}
	
	if (rotation == 0) return;

	const double linescount = (rotation / ((double)event.GetWheelDelta())) * ((double)event.GetLinesPerAction());
	pos = pos - (m_lines.GetLineHeight() * linescount);

	if (rotation > 0) pos = max(pos, 0); // up
	else if (rotation < 0) pos = min(pos, m_lines.GetHeight() - size.y); // down

	if (pos != scrollPos) {
		scrollPos = pos;
		DrawLayout();
	}
}

void EditorCtrl::DoHorizontalWheelScroll(wxMouseEvent& event) {
	const wxSize size = GetClientSize();
	int pos = m_scrollPosX;
	const int rotation = event.GetWheelRotation();

	if (event.GetLinesPerAction() == (int)UINT_MAX) { // signifies to scroll a page
		wxScrollWinEvent newEvent;
		newEvent.SetOrientation(wxHORIZONTAL);
		newEvent.SetEventObject(this);
		newEvent.SetEventType(rotation>0 ? wxEVT_SCROLLWIN_PAGEUP : wxEVT_SCROLLWIN_PAGEDOWN);
        ProcessEvent(newEvent);
		return;
	}

	if (rotation == 0) return;

	const int scroll_amount= (rotation / event.GetWheelDelta()) * event.GetLinesPerAction();
	pos -= 10 * scroll_amount;

	if (rotation > 0) pos = max(pos, 0); // left
	else if (rotation < 0) pos = min(pos, m_lines.GetWidth() - (int)m_lines.GetDisplayWidth()); // right

	if (pos != m_scrollPosX) {
		m_scrollPosX = pos;
		DrawLayout();
	}
}

void EditorCtrl::ProcessMouseWheel(wxMouseEvent& event) {
	if (event.ShiftDown()) {
		// Only handle scrollwheel if we have a scrollbar
		if (GetScrollThumb(wxHORIZONTAL))
			DoHorizontalWheelScroll(event);
	}
	else {
		// Only handle scrollwheel if we have a scrollbar
		if (HasScrollbar())
			DoVerticalWheelScroll(event);
	}
}

void EditorCtrl::OnMouseWheel(wxMouseEvent& event) {
	// If the EditorCtrl is focused, only handle the event directly if happens within the bounds
	// of the editor control. Otherwise, let if float up to the EditorFrame.

	// The EditorFrame may float the event back down, but it will do so by
	// calling EditorCtrl::ProcessMouseWheel directly.

	const wxSize& my_size = this->GetSize();
	const wxPoint& where = event.GetPosition();

	if ((where.x < 0) && (where.y < 0) && 
		(where.x > my_size.GetWidth()) && (where.y > my_size.GetHeight()))
	{
		ProcessMouseWheel(event);
		return;
	}

	event.Skip(true);
}

void EditorCtrl::SetScroll(unsigned int ypos) {
	if (scrollPos != (int)ypos) {
		old_scrollPos = scrollPos;
		scrollPos = ypos;
		DrawLayout(true);
	}
}

void EditorCtrl::OnScroll(wxScrollWinEvent& event) {
	HandleScroll(event.GetOrientation(), event.GetPosition(), event.GetEventType());
}

void EditorCtrl::OnScrollBar(wxScrollEvent& event) {
	HandleScroll(event.GetOrientation(), event.GetPosition(), event.GetEventType());
}

void EditorCtrl::HandleScroll(int orientation, int position, wxEventType eventType) {
	const wxSize size = GetClientSize();
	int pos = (orientation == wxVERTICAL) ? scrollPos : m_scrollPosX;
	const int page_size = (orientation == wxVERTICAL) ? size.y : m_lines.GetDisplayWidth();
	const int doc_size = (orientation == wxVERTICAL) ? m_lines.GetHeight() : m_lines.GetWidth();
	const int line_size = (orientation == wxVERTICAL) ? m_lines.GetLineHeight() : 10;

	if (eventType == wxEVT_SCROLLWIN_THUMBTRACK ||
		eventType == wxEVT_SCROLLWIN_THUMBRELEASE ||
		eventType == wxEVT_SCROLL_THUMBTRACK ||
		eventType == wxEVT_SCROLL_THUMBRELEASE)
	{
		pos = position;
	}
	else if (eventType == wxEVT_SCROLLWIN_PAGEUP ||
		     eventType == wxEVT_SCROLL_PAGEUP ) {
		pos -= page_size;
		if (pos < 0) pos = 0;
	}
	else if (eventType == wxEVT_SCROLLWIN_PAGEDOWN ||
		     eventType == wxEVT_SCROLL_PAGEDOWN) {
		pos += page_size;
		if (pos > doc_size - page_size) pos = doc_size - page_size;
	}
	else if (eventType == wxEVT_SCROLLWIN_LINEUP ||
		     eventType == wxEVT_SCROLL_LINEUP) {
		pos = pos - (pos % line_size) - line_size;
		if (pos < 0) pos = 0;
	}
	else if (eventType == wxEVT_SCROLLWIN_LINEDOWN ||
		     eventType == wxEVT_SCROLL_LINEDOWN) {
		pos = pos - (pos % line_size) + line_size;
		if (pos > doc_size - size.y) pos = doc_size - page_size;
	}
	else if (eventType == wxEVT_SCROLLWIN_TOP ||
		     eventType == wxEVT_SCROLL_TOP) {
		pos = 0;
	}
	else if (eventType == wxEVT_SCROLLWIN_BOTTOM ||
		     eventType == wxEVT_SCROLL_BOTTOM) {
		pos = -1; // end
	}

	if (orientation == wxVERTICAL) {
		if (pos != scrollPos) {
			wxASSERT(pos >= -1);

			old_scrollPos = scrollPos;
			scrollPos = pos;
			DrawLayout(true);
		}
	}
	else { // Horizontal scroll
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
	if (m_lines.NeedIdle())
		m_lines.OnIdle();

	// Update syntax
	const bool syntaxNeedIdle = m_syntaxstyler.OnIdle();

	// Update foldings
	ParseFoldMarkers();

	// ScrollPos may no longer be valid, calc new scrollpos
	if (HasScrollbar()) {
		scrollPos = m_lines.GetYPosFromLine(topline) + lineoffset;
		if (m_leftScrollbar) m_leftScrollbar->SetScrollbar(scrollPos, GetClientSize().y, m_lines.GetHeight(), GetClientSize().y);
		else SetScrollbar(wxVERTICAL, scrollPos, GetClientSize().y, m_lines.GetHeight());
	}

	// Check if we should request more idle events
	if (syntaxNeedIdle || m_lines.NeedIdle()) {
		//wxLogDebug(wxT("EditorCtrl::OnIdle - RequestMore %d %d"), syntaxNeedIdle, lines.NeedIdle());
		event.RequestMore();
	}
}

void EditorCtrl::OnClose(wxCloseEvent& WXUNUSED(event)) {}

void EditorCtrl::OnSetDocument(EditorCtrl* self, void* data, int filter) {
	wxASSERT(self->IsOk());

	if (filter != self->GetId()) return;

	const doc_id* const di = (doc_id*)data;

	if (self->m_doc != *di)
		self->SetDocument(*di);
}

void EditorCtrl::OnDocCommited(EditorCtrl* self, void* data, int WXUNUSED(filter)) {
	wxASSERT(self->IsOk());

	const docid_pair* const dp = (docid_pair*)data;
	wxASSERT(dp->doc1.IsDraft());
	wxASSERT(dp->doc2.IsDocument());

	const doc_id di = self->GetDocID();
	if (di.SameDoc(dp->doc1))
		self->SetDocument(dp->doc2);
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
	eSettings& settings = eGetSettings();
	bool autoPair = false;
	settings.GetSettingBool(wxT("autoPair"), autoPair);
	self->m_autopair.Enable(autoPair);

	settings.GetSettingBool(wxT("autoWrap"), self->m_doAutoWrap);
	settings.GetSettingBool(wxT("showMargin"), doShowMargin);
	settings.GetSettingBool(wxT("wrapMargin"), self->m_wrapAtMargin);
	settings.GetSettingInt(wxT("marginChars"), marginChars);

	if (!doShowMargin) {
		marginChars = 0;
		self->m_wrapAtMargin = false;
	}

	// Make sure we keep the same topline
	const unsigned int topline = self->m_lines.GetLineFromYPos(self->scrollPos);

	self->m_isResizing = true;
	self->m_lines.ShowMargin(marginChars);
	if (self->m_wrapAtMargin && self->m_lines.GetWrapMode() != cxWRAP_NONE)
		self->m_lines.SetWidth(self->m_lines.GetMarginPos());
	
	self->scrollPos = self->m_lines.GetYPosFromLine(topline);
	self->DrawLayout();
}


void EditorCtrl::OnBundlesReloaded(EditorCtrl* self, void* WXUNUSED(data), int WXUNUSED(filter)) {
	wxASSERT(self->IsOk());

	// Check if we are editing an item that has been modified
	if (self->IsBundleItem()) {} //TODO: If modified externally, prompt user to reload

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
	
	if (actions.size() == 1)
		DoAction(*actions[0], NULL, false);
	else {
		// Show popup menu
		const int result = ShowPopupList(actions);
		if (result != -1)
			DoAction(*actions[result], NULL, false);
	}

	return true;
}

void EditorCtrl::DoDragCommand(const tmDragCommand &cmd, const wxString& path) {
	// Note: the 'path' parameter is the path of the dropped file; might want to rename parameter

	map<wxString, wxString> env;
	wxLogDebug(wxT("DoDragCommand pos:%d len:%d"), GetPos(), GetLength());

	// Full path
	const wxString fullPath = cmd.isUnix ? eDocumentPath::WinPathToCygwin(path) : path;
	env[wxT("TM_DROPPED_FILEPATH")] = fullPath;

	// Make path relative to document dir
	const wxFileName& docPath = GetFilePath();
	if (docPath.IsOk()) {
		if (cmd.isUnix) {
			wxFileName unixDocPath(eDocumentPath::WinPathToCygwin(docPath), wxPATH_UNIX);
			wxFileName filePath(eDocumentPath::WinPathToCygwin(path), wxPATH_UNIX);
			filePath.MakeRelativeTo(unixDocPath.GetPath(0, wxPATH_UNIX), wxPATH_UNIX);

			env[wxT("TM_DROPPED_FILE")] = filePath.GetFullPath(wxPATH_UNIX);
		}
		else {
			wxFileName filePath(path);
			filePath.MakeRelativeTo(docPath.GetFullPath());
			env[wxT("TM_DROPPED_FILE")] = filePath.GetFullPath();
		}
	}
	else env[wxT("TM_DROPPED_FILE")] = fullPath;


	wxString modifiers = get_modifier_key_env();
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
	if (dragType == wxDragMove && m_dragStartPos != wxDefaultPosition)
		DeleteSelections(); // Will adjust caret pos

	RemoveAllSelections();

	const unsigned int pos = GetPos();

#ifdef __WXMSW__
	wxString copytext = text;
	InplaceConvertCRLFtoLF(copytext);
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
	if (dragType == wxDragMove && m_dragStartPos != wxDefaultPosition)
		DeleteSelections(); // Will adjust caret pos

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
		for (vector<const tmDragCommand*>::const_iterator p = actions.begin(); p != actions.end(); ++p)
			listMenu.Append(menuId++, (*p)->name);

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
			for (; i < filenames.GetCount(); ++i)
				m_parentFrame.Open(filenames[i]);

			newTabs = true;
			break;
		}
		else if (result >= 1010-1000)
			DoDragCommand(*actions[result-10], filenames[i]);
	}

	if (!newTabs) SetFocus();
}

void EditorCtrl::GotoSymbolPos(unsigned int pos) {
	SetPos(pos);
	MakeCaretVisibleCenter();
	ReDraw();
	SetFocus();
}

int EditorCtrl::GetSymbols(vector<SymbolRef>& symbols) const {
	// Only return symbols if the entire syntax is parsed
	if (!m_syntaxstyler.IsParsed() || !m_syntaxHandler.AllBundlesLoaded()) return 0;

	// DEBUG: We want to be able to see in crash dumps if symbols was reloaded
	int res = 1;

	// Check if we have symbols cached, to avoid building them twice for statusbar & symbollist
	if (m_symbolCacheToken != m_changeToken) {
		m_symbolCache.clear();
		m_syntaxstyler.GetSymbols(m_symbolCache);
		m_symbolCacheToken = m_changeToken;
		res = 2; // DEBUG: reloaded
	}

	symbols = m_symbolCache;
	return res;
}

wxString EditorCtrl::GetSymbolString(const SymbolRef& sr) const {
	const SymbolRef sr_debug = sr; // copy so we can see contents in call stack

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

		if (c == '#') { // ignore comments
			while (i < len && transform[i] != '\n') ++i;
			continue;
		}

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
	}

	if (source.empty()) return wxEmptyString;
	return wxString(&*source.begin(), wxConvUTF8, source.size());
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
			// Don't call ToAscii with dead keys as it clears the keyboard buffer
			UINT k = ::MapVirtualKey(event.m_rawCode, MAPVK_VK_TO_CHAR);
			if (k & 0x80000000) return false; // dead keys have top bit set

			unsigned char keystate[256];
			memset (keystate, 0, sizeof (keystate));
			::GetKeyboardState(keystate);

			const int BUFFER_SIZE = 2;
			unsigned char buf[BUFFER_SIZE] = { 0 };
			const int ret = ::ToAscii(event.m_rawCode, 0, keystate, (WORD *)buf, 0);

			if (m_parentFrame.IsKeyDiagMode())
				Insert(wxString::Format(wxT("\tret=%d\n\tbuf=%d\n"), ret, *buf));

			if ((ret >= 1) && (((32 <= *buf) && (*buf <= 126)) || ((160 <= *buf) && (*buf <= 255))))
				return false;
		}
#endif

		int modifiers = event.GetModifiers();

#ifdef __WXMSW__
		// GetModifiers does not report the windows keys
		if (::GetKeyState(VK_LWIN) < 0) modifiers |= 0x0008; // wxMOD_META (Left Windows key)
		if (::GetKeyState(VK_RWIN) < 0) modifiers |= 0x0008; // wxMOD_META (Right Windows key)
#endif

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
		if (event.m_rawCode == 17 || event.m_rawCode == 18)
			s_altGrDown = false;
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

vector<unsigned int> EditorCtrl::GetFoldedLines() const {
	vector<unsigned int> folds;
	for (vector<cxFold>::const_iterator p = m_folds.begin(); p != m_folds.end(); ++p)
		if (p->type == cxFOLD_START_FOLDED) folds.push_back(p->line_id);
	return folds;
}

bool EditorCtrl::HasFoldedFolds() const {
	for (vector<cxFold>::const_iterator p = m_folds.begin(); p != m_folds.end(); ++p)
		if (p->type == cxFOLD_START_FOLDED) return true;
	return false;
}

void EditorCtrl::FoldingClear() {
	m_folds.clear();
	m_foldedLines = 0;
	m_foldLineCount = 0;
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
				if (!matchEndMarker) // starter and ender on same line cancels out
					m_folds.push_back(cxFold(i, cxFOLD_START, m_lines.GetLineIndentLevel(i)));
			}
			else if (matchEndMarker)
				m_folds.push_back(cxFold(i, cxFOLD_END, m_lines.GetLineIndentLevel(i)));
		}

		lineStart = lineEnd;
		++i;
	}

	m_foldedLines = i;
}

vector<cxFold>::iterator EditorCtrl::ParseFoldLine(unsigned int line_id, vector<cxFold>::iterator insertPos, bool doFold) {
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
			if (!matchEndMarker) { // starter and ender on same line cancels out
				return m_folds.insert(
						insertPos, 
						cxFold(line_id, (doFold ? cxFOLD_START_FOLDED : cxFOLD_START), m_lines.GetLineIndentLevel(line_id))
					) + 1;
			}
		}
		else if (matchEndMarker) {
			return m_folds.insert(
					insertPos, 
					cxFold(line_id, cxFOLD_END, m_lines.GetLineIndentLevel(line_id))
				) + 1;
		}
	}

	return insertPos; // nothing inserted
}

void EditorCtrl::FoldingInsert(unsigned int pos, unsigned int len) {
	// Find out which lines were affected
	const unsigned int lineCount = m_lines.GetLineCount(false/*includeVirtual*/);
	const unsigned int firstline = m_lines.GetLineFromCharPos(pos);
	unsigned int lastline = m_lines.GetLineFromCharPos(pos + len);
	if (lastline && m_lines.IsLineVirtual(lastline)) --lastline; // Don't try to parse last virtual line

	// How many new lines were inserted?
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
	for (unsigned int i = firstline; i <= lastline; ++i)
		p = ParseFoldLine(i, p, doRefold);

	// Adjust line ids in following
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
	for (vector<cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p)
		p->indent = m_lines.GetLineIndentLevel(p->line_id);
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

	if (!m_lines.IsLineVirtual(line_id))
		p = ParseFoldLine(line_id, p, doRefold);

	// Remove deleted lines
	const unsigned int lastline = line_id + newLines;
	while (p != m_folds.end() && p->line_id <= lastline)
		p = m_folds.erase(p);

	// Adjust line ids in following
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
			if (!m_lines.IsLineVirtual(line_id))
				f = ParseFoldLine(line_id, f, doRefold);

			// Remove deleted lines
			const unsigned int lastline = line_id - l->lines;
			while (f != m_folds.end() && f->line_id <= lastline)
				f = m_folds.erase(f);
		}

		// Adjust line id's in following
		if (l->lines != 0) {
			for (; f != m_folds.end(); ++f)
				f->line_id += l->lines;

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
				if ((*fr)->line_id == foldLine) // end matches current
					return f->line_id; 
				else if ((*fr)->line_id < foldLine) // end matches previous (ending fold prematurely)
					return f->line_id-1; 
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
	for (vector<cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p)
		if (p->type == cxFOLD_START) Fold(p->line_id);
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
	if (m_macro.IsRecording()) m_macro.Add(wxT("SelectFold"));

	const unsigned int line_id = m_lines.GetCurrentLine();
	SelectFold(line_id);
}

void EditorCtrl::SelectFold(unsigned int line_id) {
	wxASSERT(line_id < m_lines.GetLineCount());

	vector<cxFold*> foldStack = GetFoldStack(line_id);
	if (foldStack.empty()) return;

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

bool EditorCtrl::IsLineFolded(unsigned int line_id) const {
	wxASSERT(line_id < m_lines.GetLineCount());

	const cxFold target(line_id);
	vector<cxFold>::const_iterator p = lower_bound(m_folds.begin(), m_folds.end(), target);
	return p != m_folds.end() && p->line_id == line_id && p->type == cxFOLD_START_FOLDED;
}

bool EditorCtrl::IsPosInFold(unsigned int pos, unsigned int* fold_start, unsigned int* fold_end) {
	wxASSERT(pos <= GetLength());
	const unsigned int line_id = m_lines.GetLineFromCharPos(pos);

	vector<cxFold>::const_iterator p = m_folds.begin();
	while (p != m_folds.end()) {
		if (p->type == cxFOLD_START_FOLDED) {
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

vector<cxFold*> EditorCtrl::GetFoldStack(unsigned int line_id) {
	vector<cxFold*> foldStack;
	if (m_lines.IsLineVirtual(line_id)) return foldStack;
	if (m_foldLineCount == 0) return foldStack;

	wxASSERT(line_id < m_foldLineCount);

	for (vector<cxFold>::iterator p = m_folds.begin(); p != m_folds.end(); ++p) {

		if (p->type == cxFOLD_END) {
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

void EditorCtrl::ClearBookmarks() {bookmarks.Clear();}

const vector<cxBookmark>& EditorCtrl::GetBookmarks() const {return bookmarks.GetBookmarks();}

void EditorCtrl::ToogleBookmarkOnCurrentLine() {
	const unsigned line_id = m_lines.GetCurrentLine();
	AddBookmark(line_id, true /*toggle*/);
}

void EditorCtrl::AddBookmark(unsigned int line_id, bool toggle) {
	wxASSERT(line_id < m_lines.GetLineCount());
	bookmarks.AddBookmark(line_id, toggle);
}

void EditorCtrl::DeleteBookmark(unsigned int line_id) {
	bookmarks.DeleteBookmark(line_id);
}

void EditorCtrl::GotoNextBookmark() {
	const unsigned int new_line = bookmarks.NextBookmark(m_lines.GetCurrentLine());
	if (new_line != NO_BOOKMARK) {
		m_lines.SetPos(m_lines.GetLineStartpos(new_line));
		MakeCaretVisible();
		DrawLayout();
	}
}

void EditorCtrl::GotoPrevBookmark() {
	const unsigned int new_line = bookmarks.PrevBookmark(m_lines.GetCurrentLine());
	if (new_line != NO_BOOKMARK) {
		m_lines.SetPos(m_lines.GetLineStartpos(new_line));
		MakeCaretVisible();
		DrawLayout();
	}
}

EditorChangeState EditorCtrl::GetChangeState() const {
	return EditorChangeState(this->GetId(), this->GetChangeToken());
}

void EditorCtrl::PlayMacro() {
	PlayMacro(m_macro);
}

void EditorCtrl::PlayMacro(const eMacro& macro) {
	if (m_macro.IsRecording()) m_macro.EndRecording(); // avoid endless loop

	for (size_t i = 0; i < macro.GetCount(); ++i) {
		const eMacroCmd& cmd = macro.GetCommand(i);
		PlayCommand(cmd);
	}
}

wxVariant EditorCtrl::PlayCommand(const eMacroCmd& cmd) {
	const wxString& name = cmd.GetName();

	if (name == wxT("CursorUp")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorUp(select);
	}
	else if (name == wxT("CursorDown")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorDown(select);
	}
	else if (name == wxT("CursorLeft")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorLeft(select);
	}
	else if (name == wxT("CursorRight")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorRight(select);
	}
	else if (name == wxT("CursorWordLeft")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorWordLeft(select);
	}
	else if (name == wxT("CursorWordRight")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorWordRight(select);
	}
	else if (name == wxT("CursorToHome")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorToHome(select);
	}
	else if (name == wxT("CursorToEnd")) {
		const SelAction select = cmd.GetArgBool(0) ? SEL_SELECT : SEL_REMOVE;
		CursorToEnd(select);
	}
	else if (name == wxT("SelectAll")) SelectAll();
	else if (name == wxT("SelectWord")) SelectWord();
	else if (name == wxT("SelectCurrentLine")) SelectCurrentLine();
	else if (name == wxT("SelectScope")) SelectScope();
	else if (name == wxT("SelectFold")) SelectFold();
	else if (name == wxT("InsertChars")) {
		const wxString text = cmd.GetArgString(0);
		for (wxString::const_iterator p = text.begin(); p != text.end(); ++p) {
			InsertChar(*p);
		}
	}
	else if (name == wxT("Tab")) Tab();
	else if (name == wxT("Copy")) OnCopy();
	else if (name == wxT("Cut")) OnCut();
	else if (name == wxT("Paste")) OnPaste();
	else if (name == wxT("Backspace")) Backspace();
	else if (name == wxT("Delete")) Delete();
	else if (name == wxT("DeleteWord")) Delete(true); 
	else if (name == wxT("DeleteCurrentLine")) {
		const bool fromPos = cmd.GetArgBool(0, false);
		DelCurrentLine(fromPos);
	}
	else if (name == wxT("ReplaceCurrentWord")) {
		const wxString text = cmd.GetArgString(0);
		ReplaceCurrentWord(text);
	}
	else if (name == wxT("Transpose")) Transpose();
	else if (name == wxT("ConvertCase")) {
		const wxString conversion = cmd.GetArgString(0);
		if (conversion == wxT("upper")) ConvertCase(cxUPPERCASE);
		else if (conversion == wxT("lower")) ConvertCase(cxLOWERCASE);
		else if (conversion == wxT("title")) ConvertCase(cxTITLECASE);
		else if (conversion == wxT("reverse")) ConvertCase(cxREVERSECASE);
	}
	else if (name == wxT("NextSnippetField")) m_snippetHandler.NextTab();
	else if (name == wxT("PrevSnippetField")) m_snippetHandler.PrevTab();
	else if (name == wxT("IndentSelectedLines")) IndentSelectedLines(true);
	else if (name == wxT("DedentSelectedLines")) IndentSelectedLines(false);
	else if (name == wxT("FilterThroughCommand")) {
		const wxString command = cmd.GetArgString(0);
		const wxString input = cmd.GetArgString(1);
		const wxString output = cmd.GetArgString(2);

		tmCommand tc;
		tc.SetContent(command);
		
		tc.input = tmCommand::ciNONE;
		if (input == wxT("selection"))      tc.input = tmCommand::ciSEL;
		else if (input == wxT("document"))  tc.input = tmCommand::ciDOC;
		else if (input == wxT("line"))      tc.input = tmCommand::ciLINE;
		else if (input == wxT("character")) tc.input = tmCommand::ciCHAR;
		else if (input == wxT("word"))      tc.input = tmCommand::ciWORD;
		else if (input == wxT("scope"))     tc.input = tmCommand::ciSCOPE;

		tc.output = tmCommand::coNONE;
		if (input == wxT("discard"))                  tc.output = tmCommand::coNONE;
		else if (input == wxT("replaceSelectedText")) tc.output = tmCommand::coSEL;
		else if (input == wxT("replaceDocument"))     tc.output = tmCommand::coDOC;
		else if (input == wxT("afterSelectedText"))   tc.output = tmCommand::coINSERT;
		else if (input == wxT("insertAsSnippet"))     tc.output = tmCommand::coSNIPPET;
		else if (input == wxT("showAsHTML"))          tc.output = tmCommand::coHTML;
		else if (input == wxT("showAsTooltip"))       tc.output = tmCommand::coTOOLTIP;
		else if (input == wxT("openAsNewDocument"))   tc.output = tmCommand::coNEWDOC;

		DoAction(tc, NULL, false);
	}
	else if (name == wxT("Find")) {
		const wxString pattern = cmd.GetArgString(0);
		int options = 0;
		if (!cmd.GetArgBool(1)) options |= FIND_MATCHCASE;
		if (cmd.GetArgBool(2)) options |= FIND_USE_REGEX;
		if (cmd.GetArgBool(3)) options |= FIND_RESTART;

		Find(pattern, options);
	}
	else if (name == wxT("FindNext")) {
		const wxString pattern = cmd.GetArgString(0);
		int options = 0;
		if (!cmd.GetArgBool(1)) options |= FIND_MATCHCASE;
		if (cmd.GetArgBool(2)) options |= FIND_USE_REGEX;
		if (cmd.GetArgBool(3)) options |= FIND_RESTART;

		FindNext(pattern, options);
	}
	else if (name == wxT("FindPrevious")) {
		const wxString pattern = cmd.GetArgString(0);
		int options = 0;
		if (!cmd.GetArgBool(1)) options |= FIND_MATCHCASE;
		if (cmd.GetArgBool(2)) options |= FIND_USE_REGEX;
		if (cmd.GetArgBool(3)) options |= FIND_RESTART;

		FindPrevious(pattern, options);
	}
	else if (name == wxT("RunCommandMode")) {
		const wxString command = cmd.GetArgString(0);
		m_commandHandler.PlayCommand(command);
	}
	else return wxVariant(NULL, wxT("Unknown method"));

	return wxVariant();
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

// -- Editor DragDropTarget -----------------------------------------------------------------

DragDropTarget::DragDropTarget(EditorCtrl& parent) : m_parent(parent) {
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

wxDragResult DragDropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def) {
	// Copy the data from the drag source to our data object
	GetData();

	const wxDataFormat df = m_dataObject->GetReceivedFormat();
	if (df == wxDF_TEXT || df == wxDF_UNICODETEXT) {
		m_parent.OnDragDropText(m_textObject->GetText(), def);
	}
    else if (df == wxDF_FILENAME) {
		m_parent.OnDragDrop(m_fileObject->GetFilenames());
	}
	else if (df == wxDataFormat(MultilineDataObject::FormatId)) {
		wxArrayString text;
		m_columnObject->GetText(text);
		m_parent.OnDragDropColumn(text, def);
	}

	return def;
}
