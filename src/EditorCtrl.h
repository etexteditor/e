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

#ifndef __EDITORCTRL_H__
#define __EDITORCTRL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/dnd.h>
#include <wx/caret.h>

#include "Catalyst.h"
#include "Lines.h"
#include "styler_searchhl.h"
#include "styler_syntax.h"
#include "SnippetHandler.h"
#include "key_hook.h"
#include "FindFlags.h"
#include "BundleItemType.h"
#include "BracketHighlight.h"
#include "DetectTripleClicks.h"

#include "IFoldingEditor.h"
#include "IEditorDoAction.h"
#include "IPrintableDocument.h"
#include "IEditorSymbols.h"
#include "IEditorSearch.h"

// Pre-definitions
class wxFileName;

class cxEnv;
class GutterCtrl;
class EditorFrame;
class PreviewDlg;
class cxRemoteAction;
class MultilineDataObject;
class TextTip;

struct thTheme;
class tmAction;
class tmDragCommand;
class TmSyntaxHandler;

class EditorCtrl : public KeyHookable<wxControl>, 
	public IFoldingEditor,
	public IEditorDoAction,
	public IPrintableDocument,
	public IEditorSymbols,
	public IEditorSearch
{
public:
	EditorCtrl(const doc_id di, const wxString& mirrorPath, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos = wxPoint(-100,-100), const wxSize& size = wxDefaultSize);

	EditorCtrl(const int page_id, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos = wxPoint(-100,-100), const wxSize& size = wxDefaultSize);

	EditorCtrl(CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos = wxPoint(-100,-100), const wxSize& size = wxDefaultSize);

	virtual ~EditorCtrl();

	void Init();
	bool IsOk() const;

	bool IsRemote() const {return m_remoteProfile != NULL;};
	const RemoteProfile* GetRemoteProfile() const {return m_remoteProfile;};

	void SetMate(const wxString& mate) {m_mate = mate;};
	const wxString& GetMate() const {return m_mate;};

	// Change state
	void StartChange();
	void EndChange();

	// Document info
	unsigned int GetLength() const;
	unsigned int GetLineCount() const {return m_lines.GetLineCount();};
	int GetTopLine();

	// Text Retrieval
	wxString GetText() const;
	wxString GetText(unsigned int start, unsigned int end) const;
	void GetText(vector<char>& text) const;
	void GetTextPart(unsigned int start, unsigned int end, vector<char>& text) const;
	wxString GetCurrentWord() const;
	void WriteText(wxOutputStream& stream) const;

	// Position
	unsigned int GetPos() const;
	unsigned int GetScrollPos() const {return scrollPos;};
	void SetScroll(unsigned int ypos);
	unsigned int GetCurrentLineNumber();
	unsigned int GetCurrentColumnNumber();
	unsigned int GetLineFromPos(unsigned int pos) {return m_lines.GetLineFromCharPos(pos);};
	bool IsLineEnd(unsigned int pos) {return m_lines.IsLineEnd(pos);};

	void SetDocumentAndScrollPosition(int pos, int topline);

	// Editing
	void InsertChar(const wxChar& text);
	void Insert(const wxString& text);
	unsigned int InsertNewline();
	void Delete(unsigned int start, unsigned int end);
	void Freeze();
	void Commit(const wxString& label, const wxString& desc);

	// Text manipulation
	void Transpose();
	void SwapLines(unsigned int line1, unsigned int line2);
	void ConvertCase(cxCase conversion);
	void DelCurrentLine(bool fromPos);
	void ReplaceCurrentWord(const wxString& word);

	// Raw editing commands
	unsigned int RawInsert(unsigned int pos, const wxString& text, bool doSmarType=false);
	unsigned int RawDelete(unsigned int start, unsigned int end);
	void RawMove(unsigned int start, unsigned int end, unsigned int dest);

	// Drawing & Layout
	void EnableRedraw(bool enable) {m_enableDrawing = enable;};
	void ReDraw();
	bool Show(bool show);
	void SetWordWrap(cxWrapMode wrapMode);
	void SetShowGutter(bool showGutter);
	void SetShowIndent(bool showIndent);
	void SetTabWidth(unsigned int width);
	void SetGutterRight(bool doMove=true);
	const wxFont& GetEditorFont() const;

	// Document handling
	void Clear();
	bool IsEmpty() const;
	cxFileResult OpenFile(const wxString& filepath, wxFontEncoding enc, const RemoteProfile* rp=NULL, const wxString& mate=wxEmptyString);
	cxFileResult LoadText(const wxFileName& newpath);
	cxFileResult LoadText(const wxString& newpath, const RemoteProfile* rp=NULL);
	cxFileResult LoadText(const wxString& newpath, wxFontEncoding enc, const RemoteProfile* rp=NULL);
	virtual bool SaveText(bool askforpath=false);
	bool IsModified() const;
	DocumentWrapper& GetDocument() {return m_doc;};
	virtual const DocumentWrapper& GetDocument() const {return m_doc;};
	bool SetDocument(const doc_id& di, const wxString& path=wxEmptyString, const RemoteProfile* rp=NULL);
	doc_id GetDocID() const;
	virtual wxString GetName() const;
	virtual const vector<unsigned int>& GetOffsets() const {return m_lines.GetOffsets();};

	virtual const char** RecommendedIcon();

	// Bundle Editing
	bool IsBundleItem() const {return m_remotePath.StartsWith(wxT("bundle://"));};

	// Undo / Redo
	void DoUndo();
	void Redo();
	interval UndoSelection(const cxDiffEntry& de);

	// Skip reloading of modified file?
	const wxDateTime& GetModSkipDate() const {return m_modSkipDate;};
	void SetModSkipDate(const wxDateTime& skipDate) {m_modSkipDate = skipDate;};

	// Path
	const wxFileName& GetFilePath() const {return m_path;};
	const wxString GetPath() const {return (m_remotePath.empty() ? m_path.GetFullPath() : m_remotePath);};
	virtual void SetPath(const wxString& newpath);

	// Copy/Paste
	void OnCopy();
	void OnCut();
	void OnPaste();

	// Selection
	bool IsSelected() const;
	void SelectAll();
	void Select(unsigned int start, unsigned int end);
	const vector<interval>& GetSelections() const {return m_lines.GetSelections();};
	void RemoveAllSelections();
	wxString GetFirstSelection() const;
	wxString GetSelFirstLine();
	wxString GetSelText() const;
	void SelectWord(unsigned int pos, bool multiselect=false);
	void SelectLine(unsigned int lineId, bool multiselect=false);
	void SelectCurrentLine();
	void SelectScope();

	// Movement commands
	void SetPos(unsigned int pos);
	void SetPos(int line, int column);
	void PageUp(bool select=false, int count=1);
	void PageDown(bool select=false, int count=1);
	void CursorUp(bool select=false);
	void CursorDown(bool select=false);
	void CursorLeft(bool select=false);
	void CursorRight(bool select=false);
	void CursorWordLeft(bool select=false);
	void CursorWordRight(bool select=false);
	void GotoMatchingBracket();

	// Search & Replace
	virtual cxFindResult Find(const wxString& text, int options=0);
	virtual cxFindResult FindNext(const wxString& text, int options=0);
	virtual bool FindPrevious(const wxString& text, int options=0);
	virtual bool Replace(const wxString& searchtext, const wxString& replacetext, int options=0);
	virtual bool ReplaceAll(const wxString& searchtext, const wxString& replacetext, int options=0);
	virtual void ClearSearchHighlight();

	// SnippetHandler and EditorFrame use 3 of the following methods; may need some more refactoring here to capture that
	wxString ParseReplaceString(const wxString& replacetext, const map<unsigned int,interval>& captures, const vector<char>* source=NULL) const;
	search_result RegExFind(const pcre* re, const pcre_extra* study, unsigned int start_pos, map<unsigned int,interval> *captures, unsigned int end_pos, int search_options=0) const;
	search_result RegExFind(const wxString& searchtext, unsigned int start_pos, bool matchcase, map<unsigned int,interval> *captures=NULL, unsigned int end_pos=0) const;
	search_result RegExFindBackwards(const wxString& searchtext, unsigned int start_pos, unsigned int end_pos, bool matchcase) const;
	search_result RawRegexSearch(const char* regex, unsigned int subjectStart, unsigned int subjectEnd, unsigned int pos, map<unsigned int,interval> *captures=NULL) const;
	search_result RawRegexSearch(const char* regex, const vector<char>& subject, unsigned int pos, map<unsigned int,interval> *captures=NULL) const;
	bool FindNextChar(wxChar c, unsigned int start_pos, unsigned int end_pos, interval& iv) const;
	void SetSearchRange();
	void ClearSearchRange(bool reset=false);

	// Settings
	void SaveSettings(unsigned int i, eSettings& m_settings);

	// Needed by IEditorSearch interface
	wxEvtHandler* GetEventHandlerI() const {return GetEventHandler();};

	// Caret and Selection Visibility
private:
	bool IsCaretVisible();
public:
	bool MakeCaretVisible();
	void MakeCaretVisibleCenter();
	void MakeSelectionVisible(unsigned int sel_id = 0);
	void KeepCaretAlive(bool keepAlive=true) {caret->KeepAlive(keepAlive);};

	// Syntax Highlighting
	const wxString& GetSyntaxName() const {return m_syntaxstyler.GetName();};
	void SetSyntax(const wxString& syntaxName, bool isManual=false);
	void AddStyler(Styler& styler) {m_lines.AddStyler(styler);};

	// Indentation
	const wxString& GetIndentUnit() const {return m_indent;};
	void IndentSelectedLines(bool add_indent=true);
	void DedentSelectedLines() {IndentSelectedLines(false);};
	wxString GetLineIndent(unsigned int lineid);
	unsigned int GetLineIndentLevel(unsigned int lineid);
	unsigned int CountIndent(const wxString& text) const;
	wxString GetLineIndentFromPos(unsigned int pos);
	void TabsToSpaces();
	void SpacesToTabs();

	// Encoding
	wxFontEncoding GetEncoding() const;
	void SetEncoding(wxFontEncoding enc);
	bool GetBOM() const;
	void SetBOM(bool saveBOM);
	wxTextFileType GetEOL() const;
	void SetEOL(wxTextFileType eol);

	// Author Highlighing
	/*bool IsHighlightActive() const {return m_usersStyler.IsActive();};
	void ActivateHighlight();
	void DisableHighlight();
	void ShowRevTooltip();*/

	// Commands & Shell
	void DoActionFromDlg();
	void ShowScopeTip();
	virtual void DoAction(const tmAction& action, const map<wxString, wxString>* envVars, bool isRaw);
	void DoDragCommand(const tmDragCommand &cmd, const wxString& path);
	void FilterThroughCommand();

	// Drag-and-drop operations
	void OnDragOver(wxCoord x, wxCoord y);
	void OnDragDrop(const wxArrayString& filenames);
	void OnDragDropText(const wxString& text, wxDragResult dragType);
	void OnDragDropColumn(const wxArrayString& text, wxDragResult dragType);

	// Shell
	void SetEnv(cxEnv& env, bool isUnix=true, const tmBundle* bundle=NULL);
	void RunCurrentSelectionAsCommand(bool doReplace);

	// Track if doc has been modified
	void MarkAsModified() {++m_changeToken; if(m_modCallback) m_modCallback(m_modCallbackData);};
	unsigned int GetChangeToken() const {return m_changeToken;};
	virtual EditorChangeState GetChangeState() const;

	// Callbacks
	void SetModifiedCallback(void(*callback)(void*), void* data) {m_modCallback = callback; m_modCallbackData = data;};
	void SetScrollCallback(void(*callback)(void*), void* data) {m_scrollCallback = callback; m_scrollCallbackData = data;};


	// Preview (in html)
	bool IsSavedForPreview() const {return m_savedForPreview;};
	void MarkPreviewDone() {m_savedForPreview = false;};

	// Completion
	void DoCompletion();
	wxArrayString GetCompletionList();

	// Symbols
	virtual int GetSymbols(vector<SymbolRef>& symbols) const;
	virtual wxString GetSymbolString(const SymbolRef& sr) const;
	virtual void GotoSymbolPos(unsigned int pos);

	// Bracket Highlighting
	virtual const interval& GetHlBracket() const {return m_bracketHighlight.GetInterval();};

	// Folding
	vector<unsigned int> GetFoldedLines() const;
	virtual const vector<cxFold>& GetFolds() const {return m_folds;};
	void UpdateFolds() {ParseFoldMarkers();};
	void Fold(unsigned int line_id);
	void FoldAll();
	void FoldOthers();
	void UnFold(unsigned int line_id);
	virtual void UnFoldParents(unsigned int line_id);
	void UnFoldAll();
	void ToggleFold();
	void SelectFold();
	void SelectFold(unsigned int line_id);
	bool IsLineFolded(unsigned int line_id) const;
	virtual bool IsPosInFold(unsigned int pos, unsigned int* fold_start=NULL, unsigned int* fold_end=NULL);
	bool HasFoldedFolds() const;
	vector<cxFold*> GetFoldStack(unsigned int line_id);

	// Bookmarks
	void ToogleBookmarkOnCurrentLine();
	void AddBookmark(unsigned int line_id, bool toggle=false);
	void DeleteBookmark(unsigned int line_id);
	void ClearBookmarks() {m_bookmarks.clear();};
	void GotoNextBookmark();
	void GotoPrevBookmark();
	const vector<cxBookmark>& GetBookmarks() const {return m_bookmarks;};

#ifdef __WXDEBUG__
	void Print();
#endif  //__WXDEBUG__

protected:
	virtual bool OnPreKeyDown(wxKeyEvent& event);
	virtual bool OnPreKeyUp(wxKeyEvent& event);

	// Let the editor class load lines into the document, however it needs to 
	// for the kind of thing being loaded.
	virtual cxFileResult LoadLinesIntoDocument(const wxString& whence_to_load, wxFontEncoding enc, const RemoteProfile* rp, wxFileName& localPath);

	// Allow derived classes to notify their parent panels to update.
	virtual void UpdateParentPanels();

private:
	// Classes
	class RepParseState {
	public:
		RepParseState(const wxString& reptext, const map<unsigned int,interval>& caps, const vector<char>* source=NULL)
		: replacetext(reptext), captures(caps), source(source), upcase(false), lowcase(false), caseChar(false), caseText(false) {
			newtext.reserve(replacetext.size());
		};
		// member variables
		const wxString& replacetext;
		const map<unsigned int,interval>& captures;
		const vector<char>* source;
		wxString newtext;
		bool upcase;
		bool lowcase;
		bool caseChar;
		bool caseText;
	};

	// Embedded class: DragDropTarget
	class DragDropTarget : public wxDropTarget {
	public:
		DragDropTarget(EditorCtrl& parent);
		wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
		wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

	private:
		EditorCtrl& m_parent;
		wxFileDataObject* m_fileObject;
		wxTextDataObject* m_textObject;
		MultilineDataObject* m_columnObject;
		wxDataObjectComposite* m_dataObject;
	};

	class LiveCaret : public wxCaret {
	public:
		LiveCaret(wxWindow *window, int width, int height)
			: wxCaret(window, width, height), m_keepAlive(false) {};
		void KeepAlive(bool keepAlive) {m_keepAlive = keepAlive;};

		virtual void OnKillFocus() {
#ifdef __WXDEBUG__
			// WORKAROUND: avoid assert in wxCaret::DoMove()
			wxCaret::OnKillFocus();
#else
			if (!m_keepAlive) wxCaret::OnKillFocus();
#endif // __WXDEBUG__
		};

	private:
		bool m_keepAlive;
	};

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseRightDown(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	void OnMouseDClick(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnLeaveWindow(wxMouseEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnPopupListMenu(wxCommandEvent& event);
	void OnFoldTooltipTimer(wxTimerEvent& event);
	void OnFocus(wxFocusEvent& event);
	DECLARE_EVENT_TABLE();

	// Called by event handlers to do the bulk of some operation.
	void DoVerticalWheelScroll(wxMouseEvent& event);
	void DoHorizontalWheelScroll(wxMouseEvent& event);


	// Notification handlers
	static void OnSetDocument(EditorCtrl* self, void* data, int filter);
	static void OnDocCommited(EditorCtrl* self, void* data, int filter);
	static void OnThemeChanged(EditorCtrl* self, void* data, int filter);
	static void OnBundlesReloaded(EditorCtrl* self, void* data, int filter);
	static void OnSettingsChanged(EditorCtrl* self, void* data, int filter);

	void ClearRemoteInfo();

	void DrawLayout(bool isScrolling=false) {wxClientDC dc(this);DrawLayout(dc, isScrolling);};
	void DrawLayout(wxDC& dc, bool isScrolling=false);
	bool UpdateScrollbars(unsigned int x, unsigned int y);

	// Coord conversion
	unsigned int ClientWidthToEditor(unsigned int width) const;
	wxPoint ClientPosToEditor(unsigned int xpos, unsigned int ypos) const;
	wxPoint EditorPosToClient(unsigned int xpos, unsigned int ypos) const;
	wxPoint GetCaretPoint() const;
	unsigned int UpdateEditorWidth();

	void ShowTipAtCaret(const wxString& text);
	bool DoShortcut(int keyCode, int modifiers);

	// Show lists as a popup menu
	int ShowPopupList(const vector<const tmAction*>& actionList);
	int ShowPopupList(wxMenu& menu);

	void Tab();
	bool DoTabTrigger(unsigned int wordstart, unsigned int wordend);

	void DeleteSelections();
	void InsertOverSelections(const wxString& text);
	void InsertColumn(const wxArrayString& text, bool select=false);
	void WrapSelections(const wxString& front, const wxString& back);
	bool DeleteInShadow(unsigned int pos, bool nextchar=true);
	void SelectFromMovement(unsigned int oldpos, unsigned int newpos, bool makeVisible=true);
	wxString GetNewIndentAfterNewline(unsigned int lineid);
	interval GetWordIv(unsigned int pos) const;

	// Line selections
	vector<unsigned int> GetSelectedLineNumbers();
	void SelectLines(const vector<unsigned int>& sel_lines);

	// Block selection
	enum BlockKeyState {
		BLOCKKEY_NONE,
		BLOCKKEY_DOWN,
		BLOCKKEY_INIT
	};
	void SelectBlock(wxPoint endpoint, bool multi=false);

	wxString GetCurrentLine();

	bool DoFind(const wxString& text, unsigned int start_pos, int options=0, bool dir_forward = true);
	void DoRepParse(RepParseState& state, const wxChar* start, const wxChar* end) const;

	void GetCompletionMatches(interval wordIv, wxArrayString& result, bool precharbase) const;

	void DoCommand(int c);
	void EndCommand();

	unsigned int GetChangePos(const doc_id& old_version_id) const;
	void RemapPos(const doc_id& old_version_id, unsigned int old_pos, const vector<interval>& old_sel, unsigned int toppos);

	// Auto Pairing
	wxString GetAutoPair(unsigned int pos, const wxString& text);
	wxString AutoPair(unsigned int pos, const wxString& text, bool addToStack=true);
	void MatchBrackets();

	// Indentation
	wxString GetRealIndent(unsigned int lineid, bool newline=false);
	unsigned int GetLineIndentPos(unsigned int lineid);

	bool IsSpaces(unsigned int start, unsigned int end) const;
	unsigned int CountMatchingChars(wxChar match, unsigned int start, unsigned int end) const;

	void ApplyDiff(const doc_id& oldDoc, bool moveToFirstChange=false);

	// Styler Managment
	void StylersClear();
	void StylersInvalidate();
	void StylersInsert(unsigned int pos, unsigned int length);
	void StylersDelete(unsigned int start, unsigned int end);

	// Folding
	void FoldingClear();
	void FoldingInvalidate();
	void FoldingInsert(unsigned int pos, unsigned int length);
	void FoldingDelete(unsigned int start, unsigned int end);
	void FoldingApplyDiff(const vector<cxLineChange>& linechanges);
	void FoldingReIndent();
	void ParseFoldMarkers();
	vector<cxFold>::iterator ParseFoldLine(unsigned int line_id, vector<cxFold>::iterator insertPos, bool doFold);
	unsigned int GetLastLineInFold(const vector<cxFold*>& foldStack) const;

	// Bookmarks
	void BookmarksInsert(unsigned int pos, unsigned int length);
	void BookmarksDelete(unsigned int start, unsigned int end);
	void BookmarksApplyDiff(const vector<cxChange>& changes);

	// Commands
	bool cmd_Undo(int count, vector<int>& cStack, bool end=false);

	// Utility functions
	static void ConvertCRLFtoLF(wxString& text);
	static bool Isalnum(wxChar c) {
#ifdef __WXMSW__
			return ::IsCharAlphaNumeric(c) != 0;
#else
			return wxIsalnum(c);
#endif // __WXMSW__
	};

	// Tests
#ifdef __WXDEBUG__
	void TestMilestones();
#endif //__WXDEBUG__

	enum action {ACTION_NONE, ACTION_INSERT, ACTION_DELETE, ACTION_UP, ACTION_DOWN, ACTION_UNDOSEL};

	// Member variables
	CatalystWrapper& m_catalyst;
	DocumentWrapper m_doc;
	Dispatcher& dispatcher;

	wxBitmap& bitmap;
	EditorFrame& m_parentFrame;

	TmSyntaxHandler& m_syntaxHandler;
	const tmTheme& m_theme;
	Lines m_lines;

	Styler_SearchHL m_search_hl_styler;
	Styler_Syntax m_syntaxstyler;

	wxTimer m_foldTooltipTimer;
	TextTip* m_activeTooltip;

	void (*m_modCallback)(void*);
	void (*m_scrollCallback)(void*);

	bool m_enableDrawing;
	bool m_isResizing;
	int scrollPos;
	int m_scrollPosX;
	int topline;
	bool commandMode;
	unsigned int m_changeToken;
	bool m_savedForPreview;
	unsigned int lastpos;
	int m_currentSel;
	bool do_freeze;
	mutable int m_options_cache; // for compiled regex
	mutable pcre *m_re; // for last compiled regex
	mutable unsigned int m_symbolCacheToken;

	// Above: set in constructors' intializer list
	// ----
	// Below: not set in initializer list

	int lastxpos; // Used to keep Up/Down in line

	GutterCtrl* m_gutterCtrl;
	bool m_showGutter;
	bool m_gutterLeft;
	unsigned int m_gutterWidth;
	LiveCaret* caret;
	wxMemoryDC mdc;
	int old_scrollPos; // set by OnScroll when scrolling
	wxFileName m_path;
	wxString m_remotePath;
	const RemoteProfile* m_remoteProfile;
	vector<int> commandStack;
	static const unsigned int m_caretWidth;
	unsigned int m_caretHeight;
	wxDateTime m_modSkipDate;
	wxString m_mate;

	// Callback data
	void* m_modCallbackData;
	void* m_scrollCallbackData;

	// Folding vars
	vector<cxFold> m_folds;
	unsigned int m_foldedLines;
	unsigned int m_foldLineCount;
	unsigned int m_foldTooltipLine;

	// Bookmarks
	vector<cxBookmark> m_bookmarks;

	action lastaction;
	wxPoint lastMousePos; // Used to check if mouse have really moved

	// Drag'n'Drop
	wxPoint m_dragStartPos;

	DetectTripleClicks m_tripleClicks;

	enum SelMode {
		SEL_NORMAL,
		SEL_WORD,
		SEL_LINE
	};

	SelMode m_selMode;
	int m_sel_start;
	int m_sel_end;
	wxPoint m_sel_startpoint;
	vector<unsigned int> m_blocksel_ids;
	BlockKeyState m_blockKeyState;
	int m_popupMenuChoice; // used by ShowPopupList()

	// Snippets
	SnippetHandler m_snippetHandler;

	// Change state vars
	int change_pos;
	doc_id change_doc_id;
	int change_toppos;

	// incremental search trackers
	vector<interval> m_searchRanges;
	// start/found are used to track state between Find calls
	unsigned int m_search_start_pos;
	unsigned int m_search_found_pos;

	wxString m_indent;

	class xAutoPair {
	public:
		void AdjustEndsUp(unsigned int bytes_inserted){
			for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
				t->end += bytes_inserted;
			}
		};

		void AdjustEndsDown(unsigned int bytes_removed) {
			for (vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
				t->end -= bytes_removed;
			}
		};

		void Clear() { m_pairStack.clear(); };

		void ClearIfInsertingOutsideInnerPair(unsigned int pos) {
			// Reset autoPair state if inserting outside inner pair
			if (!m_pairStack.empty() && pos != m_pairStack.back().end) {
				m_pairStack.clear();
			}
		};

		bool HasPairs() const { return !m_pairStack.empty(); };

		bool AtEndOfPair(unsigned int pos) const {
			for (vector<interval>::const_iterator p = m_pairStack.begin(); p != m_pairStack.end(); ++p) {
				if (p->end == pos) return true;
			}
			return false;
		};

		const interval& InnerPair() const { return m_pairStack.back(); };
		const interval& OuterPair() const { return m_pairStack[0]; };

		bool Enabled() const { return m_doAutoPair; };
		void Enable(bool enable) { m_doAutoPair = enable; };

		vector<interval> m_pairStack;

	private:
		bool m_doAutoPair;
	};

	xAutoPair m_autopair;

	// Wrapping
	bool m_doAutoWrap;
	bool m_wrapAtMargin;

	BracketHighlight m_bracketHighlight;

	int m_lastScopePos;

	// Cache of last compiled regex
	mutable wxString m_regex_cache;

	// Cached env variables
	wxString m_tmFilePath;
	wxString m_tmDirectory;

	// Symbol cache
	mutable vector<SymbolRef> m_symbolCache;

	// Key state
	static unsigned long s_ctrlDownTime;
	static bool s_altGrDown;

	friend class GutterCtrl;
};

#endif // __EDITORCTRL_H__
