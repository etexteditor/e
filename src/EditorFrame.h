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

#ifndef __EDITORFRAME_H__
#define __EDITORFRAME_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/aui/aui.h>
#include <wx/imaglist.h>

#include "Catalyst.h"
#include "key_hook.h"
#include "eSettings.h"

#include "WrapMode.h"

#include "IFrameEditorService.h"
#include "IFrameSymbolService.h"
#include "IFrameRemoteThread.h"
#include "IFrameProjectService.h"
#include "IFrameUndoPane.h"
#include "IFrameSearchService.h"
#include "IOpenTextmateURL.h"
#include "ITabPage.h"


class EditorCtrl;
struct EditorChangeState;
class ProjectPane;
class PreviewDlg;
class SymbolList;
class wxFilesChangedEvent;
class ChangeCheckerThread;
class BundlePane;
class DocHistory;
class UndoHistory;
class SearchPanel;
class TmSyntaxHandler;
class StatusBar;
class DirWatcher;
class FindInProjectDlg;
class HtmlOutputPane;
class eAuiNotebook;
class DiffDirPane;
class DiffPanel;
class IEditorSearch;
class RemoteThread;
class SnippetList;


class EditorFrame : public KeyHookable<wxFrame>,
	public IFrameSymbolService,
	public IFrameProjectService,
	public IFrameUndoPane,
	public IFrameSearchService,
	public IOpenTextmateURL
{
public:
	enum FrameMenu {
		MENU_NEWWINDOW,
		MENU_CLOSEWINDOW,
		MENU_DIFF,
		MENU_OPENPROJECT,
		MENU_OPENREMOTE,
		MENU_CLOSEPROJECT,
		MENU_SAVEALL,
		MENU_SAVEFORMAT,
		MENU_LINEEND,
		MENU_ENCODING,
		MENU_BOM,
		MENU_IMPORT,
		MENU_CLOSE,
		MENU_REVSEL,
		MENU_FIND_AND_REPLACE,
		MENU_FIND_IN_PROJECT,
		MENU_FIND_IN_SEL,
		MENU_FIND_NEXT,
		MENU_FIND_PREVIOUS,
		MENU_FIND_CURRENT,
		MENU_SELECT,
		MENU_SELECTWORD,
		MENU_SELECTLINE,
		MENU_SELECTSCOPE,
		MENU_SELECTFOLD,
		MENU_SELECTTAG,
		MENU_SYNTAX,
		MENU_EDIT_THEME,
		MENU_SETTINGS,
		MENU_UNDOHIS,
		MENU_REVHIS,
		MENU_COMMANDOUTPUT,
		MENU_LINENUM,
		MENU_INDENTGUIDE,
		MENU_WORDWRAP,
		MENU_WRAP_NONE,
		MENU_WRAP_NORMAL,
		MENU_WRAP_SMART,
		MENU_HL_USERS,
		MENU_INCOMMING,
		MENU_INCOMMING_TOOLBAR,
		MENU_SHOWPROJECT,
		MENU_SHOWSYMBOLS,
		MENU_SHOWSNIPPETS,
		MENU_SHIFT_PROJECT_FOCUS,
		MENU_PREVIEW,
		MENU_STATUSBAR,

		MENU_GO_NEXT_TAB,
		MENU_GO_LAST_TAB,
		MENU_GO_PREV_TAB,

		MENU_OPEN_EXT,
		MENU_TABS,
		MENU_TABS_SHOWDROPDOWN,
		MENU_GOTO_BRACKET,
		MENU_GOTO_LINE,
		MENU_GOTO_FILE,
		MENU_GOTO_SYMBOLS,
		MENU_FOLDTOGGLE,
		MENU_FOLDALL,
		MENU_FOLDOTHERS,
		MENU_UNFOLDALL,
		MENU_BUY,
		MENU_REGISTER,
		MENU_WEBSITE,
		MENU_ABOUT,
		MENU_EOL_DOS,
		MENU_EOL_UNIX,
		MENU_EOL_MAC,
		MENU_EOL_NATIVE,
		MENU_DOC_OPEN,
		MENU_DOC_SHARE,
		MENU_COMMIT,
		MENU_REVTOOLTIP,
		MENU_FINDCMD,
		MENU_FILTER,
		MENU_RUN,
		MENU_HELP_FORUM,
		MENU_RECENT_FILES,
		MENU_RECENT_PROJECTS,
		MENU_TEXTCONV,
		MENU_UPPERCASE,
		MENU_LOWERCASE,
		MENU_TITLECASE,
		MENU_REVERSECASE,
		MENU_COMPLETE,
		MENU_INDENTLEFT,
		MENU_INDENTRIGHT,
		MENU_TABSTOSPACES,
		MENU_SPACESTOTABS,
		MENU_BUNDLE_FUNCTIONS,
		MENU_RELOAD_BUNDLES,
		MENU_DEBUG_BUNDLES,
		MENU_EDIT_BUNDLES,
		MENU_MANAGE_BUNDLES,
		MENU_TABS_NEW,
		MENU_TABS_CLOSE,
		MENU_TABS_CLOSE_OTHER,
		MENU_TABS_CLOSE_ALL,
		MENU_TABS_COPY_PATH,
		MENU_KEYDIAG,
		MENU_BOOKMARK_NEXT,
		MENU_BOOKMARK_PREVIOUS,
		MENU_BOOKMARK_TOGGLE,
		MENU_BOOKMARK_CLEAR
	};

	EditorFrame(CatalystWrapper cat, unsigned int frameId, const wxString& title, const wxRect& rect, TmSyntaxHandler& syntax_handler);
	~EditorFrame();

	void RestoreState();

	// Tabs
	void AddTab(wxWindow* page=NULL);
	ITabPage* GetPage(size_t idx);
	void UpdateNotebook();

	// Editor
	void OpenDocument(const doc_id& di);
	void UpdateWindowTitle();
	void UpdateTabs();
	void GotoPos(int line, int column);
	bool CloseTab(unsigned int tab_id, bool removetab=true);
	EditorCtrl* GetEditorCtrl();
	virtual IEditorSearch* GetSearch();

	// Editor Service methods.
	virtual wxControl* GetEditorAndChangeType(const EditorChangeState& lastChangeState, EditorChangeType& newStatus);
	virtual void FocusEditor();

	TmSyntaxHandler& GetSyntaxHandler() const { return this->m_syntax_handler; }

	// Files
	bool Open(const wxString& path, const wxString& mate=wxEmptyString); // file or project
	virtual bool OpenFile(const wxFileName& path, wxFontEncoding enc=wxFONTENCODING_SYSTEM, const wxString& mate=wxEmptyString);
	virtual bool OpenRemoteFile(const wxString& url, const RemoteProfile* rp=NULL);
	virtual void UpdateRenamedFile(const wxFileName& path, const wxFileName& newPath);
	void Compare(const wxString& leftPath, const wxString& rightPath);

	virtual bool OpenTxmtUrl(const wxString& url);
	bool AskToSaveMulti(int keep_tab=-1);
	void SaveAllFilesInProject();
	//void CheckForModifiedFiles();
	void CheckForModifiedFilesAsync();
	wxString GetSaveDir() const;
	

	// Reopen files on startup from previous session.
	void ReopenFiles(wxArrayString& files, unsigned long firstLine, unsigned long firstColumn, wxString& mate);

	// RemoteFile support functions
	const RemoteProfile* GetRemoteProfile(const wxString& url, bool withDir);
	wxString DownloadFile(const wxString& url, const RemoteProfile* rp);
	bool UploadFile(const wxString& url, const wxString& buffPath, const RemoteProfile* rp);
	wxDateTime GetRemoteDate(const wxString& url, const RemoteProfile* rp);
	virtual bool AskRemoteLogin(const RemoteProfile* rp);

	// Bundle Editor support functions
	void ShowBundleManager();

	// Search Bar
	virtual void ShowSearch(bool show=true, bool replace=false);
	bool IsSearching() const;

	// Settings
	bool GetSetting(const wxString& name) const;
	void SetSetting(const wxString& name, bool value);
	bool IsSoftTabs() const {return m_softTabs;};
	void SetSoftTab(bool isSoft);
	unsigned int GetTabWidth() const {return m_tabWidth;};
	void SetTabWidth(unsigned int width);
	cxWrapMode GetWrapMode() const {return m_wrapMode;};
	bool IsGutterShown() const {return m_showGutter;};
	bool IsIndentShown() const {return m_showIndent;};
	eFrameSettings& GetFrameSettings() {return m_settings;};

	// Registration
	void RemoveRegMenus();

	// Project Pane
	bool OpenProject(const wxString& prj);
	bool OpenDirProject(const wxFileName& path);
	bool OpenRemoteProjectFromUrl(const wxString& url);
	bool OpenRemoteProject(const RemoteProfile* rp);
	bool HasProject() const;
	bool CloseProject();
	bool IsProjectRemote() const;
	const wxFileName& GetRootPath() const;
	wxArrayString GetSelectionsInProject() const;
	const map<wxString,wxString>& GetProjectEnv() const;

	// Output Pane
	void ShowOutput(const wxString& title, const wxString& output);

	// Symbol List (pane)
	void ShowSymbolList(bool keepOpen=true);
	virtual void CloseSymbolList();

	// Snippet List (pane)
	void ShowSnippetList();
	virtual void CloseSnippetList();
	void RefreshSnippetList();

	// DirWatcher & RemoteThread
	virtual DirWatcher& GetDirWatcher() {wxASSERT(m_dirWatcher); return *m_dirWatcher;};

	virtual void SetUndoPaneCaption(const wxString& caption);

	virtual RemoteThread& GetRemoteThread() {return *m_remoteThread;};

	// Preview
	void SetWebPreviewTitle(const wxString& title);

	bool IsKeyDiagMode() {return m_keyDiags;};

	void BringToFront();

	void CreateEncodingMenu(wxMenu& menu) const;
	void UpdateEncodingMenu(wxMenu& menu) const;

	static wxString DefaultFileFilters;

protected:
	virtual bool OnPreKeyUp(wxKeyEvent& event);

private:
	// Init functions
	void InitAccelerators();
	void InitMenus();
	void InitStatusbar();
	void InitMemberSettings();

	EditorCtrl* GetEditorCtrlFromPage(size_t page_idx);
	EditorCtrl* GetEditorCtrlFromFile(const wxString& filepath, unsigned int& page_idx);
	DiffPanel* GetDiffPaneFromFiles(const wxString& path1, const wxString& path2, unsigned int& page_idx) const;

	// Menu & statusdbar handling
	wxMenu* GetBundleMenu();
	void ResetBundleMenu();
	void ResetSyntaxMenu();
	void CreateAndSetStatusbar();
	void UpdateTabMenu();
	void UpdateRecentFiles();

	// Loading files
	bool DoOpenFile(const wxString& filepath, wxFontEncoding enc, const RemoteProfile* rp=NULL, const wxString& mate=wxEmptyString);

	// Changed files
	void AskToReloadMulti(const vector<unsigned int>& pathToPages, const vector<wxDateTime>& modDates);

	// State
	void SaveState();
	void LoadSize();
	void SaveSize();

	bool DeletePage(unsigned int page_id, bool removetab=true);

	// Web Preview (pane)
	void ShowWebPreview();
	void CloseWebPreview();

	// Projects
	void ShowProjectPane(const wxString& project);
	void ShowBundlePane();
	void ShowDiffPane(const wxString& path1, const wxString& path2);

	// Event handlers
	void OnShiftProjectFocus(wxCommandEvent& event);

	void OnOpeningMenu(wxMenuEvent& event);

	void OnMenuNew(wxCommandEvent& event);
	void OnMenuNewWindow(wxCommandEvent& event);
	void OnMenuOpen(wxCommandEvent& event);
	void OnMenuCompareFiles(wxCommandEvent& event);
	void OnMenuOpenProject(wxCommandEvent& event);
	void OnMenuOpenRemote(wxCommandEvent& event);
	void OnMenuCloseProject(wxCommandEvent& event);
	void OnMenuOpenRecentFile(wxCommandEvent& event);
	void OnMenuOpenRecentProject(wxCommandEvent& event);
	void OnMenuSave(wxCommandEvent& event);
	void OnMenuSaveAs(wxCommandEvent& event);
	void OnMenuSaveAll(wxCommandEvent& event);
	void OnMenuPageSetup(wxCommandEvent& event);
	//void OnMenuPrintPreview(wxCommandEvent& event);
	void OnMenuPrint(wxCommandEvent& event);
	void OnMenuClose(wxCommandEvent& event);
	void OnMenuCloseWindow(wxCommandEvent& event);
	void OnMenuExit(wxCommandEvent& event);
	void OnMenuUndo(wxCommandEvent& event);
	void OnMenuRedo(wxCommandEvent& event);
	void OnMenuCut(wxCommandEvent& event);
	void OnMenuCopy(wxCommandEvent& event);
	void OnMenuPaste(wxCommandEvent& event);
	void OnMenuFind(wxCommandEvent& event);
	void OnMenuFindInProject(wxCommandEvent& event);
	void OnMenuFindInSel(wxCommandEvent& event);
	void OnMenuFindNext(wxCommandEvent& event);
	void OnMenuFindPrevious(wxCommandEvent& event);
	void OnMenuFindCurrent(wxCommandEvent& event);
	void OnMenuReplace(wxCommandEvent& event);
	void OnMenuConvUpper(wxCommandEvent& event);
	void OnMenuConvLower(wxCommandEvent& event);
	void OnMenuConvTitle(wxCommandEvent& event);
	void OnMenuConvReverse(wxCommandEvent& event);
	void OnMenuRevSel(wxCommandEvent& event);
	void OnMenuIndentLeft(wxCommandEvent& event);
	void OnMenuIndentRight(wxCommandEvent& event);
	void OnMenuTabsToSpaces(wxCommandEvent& event);
	void OnMenuSpacesToTabs(wxCommandEvent& event);
	void OnMenuCompleteWord(wxCommandEvent& event);
	void OnMenuSelectAll(wxCommandEvent& event);
	void OnMenuSelectWord(wxCommandEvent& event);
	void OnMenuSelectLine(wxCommandEvent& event);
	void OnMenuSelectScope(wxCommandEvent& event);
	void OnMenuSelectFold(wxCommandEvent& event);
	void OnMenuSelectTag(wxCommandEvent& event);
	void OnMenuEditTheme(wxCommandEvent& event);
	void OnMenuSettings(wxCommandEvent& event);
	void OnMenuFilter(wxCommandEvent& event);
	void OnMenuRunCurrent(wxCommandEvent& event);

	void OnMenuNextTab(wxCommandEvent& event);
	void OnMenuLastTab(wxCommandEvent& event);
	void OnMenuPrevTab(wxCommandEvent& event);
	void OnMenuGotoTab(wxCommandEvent& event);
	void OnMenuGotoLastTab(wxCommandEvent& event);

	void OnMenuOpenExt(wxCommandEvent& event);
	void OnMenuGotoFile(wxCommandEvent& event);
	void OnMenuGotoLine(wxCommandEvent& event);
	void OnMenuGotoBracket(wxCommandEvent& event);
	void OnMenuFoldToggle(wxCommandEvent& event);
	void OnMenuFoldAll(wxCommandEvent& event);
	void OnMenuFoldOthers(wxCommandEvent& event);
	void OnMenuUnfoldAll(wxCommandEvent& event);
	void OnMenuBookmarkNext(wxCommandEvent& event);
	void OnMenuBookmarkPrevious(wxCommandEvent& event);
	void OnMenuBookmarkToggle(wxCommandEvent& event);
	void OnMenuBookmarkClear(wxCommandEvent& event);
	void OnMenuCommit(wxCommandEvent& event);
	void OnMenuShowProject(wxCommandEvent& event);
	void OnMenuShowSymbols(wxCommandEvent& event);
	void OnMenuShowSnippets(wxCommandEvent& event);
	void OnMenuSymbols(wxCommandEvent& event);
	void OnMenuRevisionHistory(wxCommandEvent& event);
	void OnMenuUndoHistory(wxCommandEvent& event);
	void OnMenuShowCommandOutput(wxCommandEvent& event);
	void OnMenuLineNumbers(wxCommandEvent& event);
	void OnMenuIndentGuide(wxCommandEvent& event);
	void OnMenuWordWrap(wxCommandEvent& event);
	void OnMenuPreview(wxCommandEvent& event);
	void OnMenuStatusbar(wxCommandEvent& event);
	void OnMenuHelp(wxCommandEvent& event);
	void OnMenuGotoForum(wxCommandEvent& event);
	void OnMenuWebsite(wxCommandEvent& event);
	void OnMenuAbout(wxCommandEvent& event);
	void OnMenuBuy(wxCommandEvent& event);
	void OnMenuRegister(wxCommandEvent& event);
	void OnMenuEolDos(wxCommandEvent& event);
	void OnMenuEolUnix(wxCommandEvent& event);
	void OnMenuEolMac(wxCommandEvent& event);
	void OnMenuEolNative(wxCommandEvent& event);
	void OnMenuBOM(wxCommandEvent& event);
	void OnMenuFindCommand(wxCommandEvent& event);
	void OnMenuReloadBundles(wxCommandEvent& event);
	void OnMenuDebugBundles(wxCommandEvent& event);
	void OnMenuEditBundles(wxCommandEvent& event);
	void OnMenuManageBundles(wxCommandEvent& event);
	void OnMenuBundleAction(wxCommandEvent& event);

	void OnMenuKeyDiagnostics(wxCommandEvent& event);
	void OnTabsShowDropdown(wxCommandEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnNotebook(wxAuiNotebookEvent& event);
	void OnNotebookDClick(wxAuiNotebookEvent& event);
	void OnNotebookContextMenu(wxAuiNotebookEvent& event);
	void OnNotebookDragDone(wxAuiNotebookEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnActivate(wxActivateEvent& event);
	void OnCloseTab(wxAuiNotebookEvent& event);
	void OnTabClosed(wxAuiNotebookEvent& event);
	void OnDoCloseTab(wxCommandEvent& event);
	void OnCloseOtherTabs(wxCommandEvent& event);
	void OnCloseAllTabs(wxCommandEvent& event);
	void OnCopyPathToClipboard(wxCommandEvent& event);
	void OnSubmenuSyntax(wxCommandEvent& event);
	void OnSubmenuEncoding(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMove(wxMoveEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnPaneClose(wxAuiManagerEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnFilesChanged(wxFilesChangedEvent& event);
	//void OnMenuRevTooltip(wxCommandEvent& event);
	//void OnMenuIncomming(wxCommandEvent& event);
	//void OnMenuIncommingTool(wxCommandEvent& event);
	//void OnMenuHighlightUsers(wxCommandEvent& event);
	//void OnMenuDocOpen(wxCommandEvent& event);
	//void OnMenuDocShare(wxCommandEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	DECLARE_EVENT_TABLE();

	// Static Notification handlers
	static void OnDocChange(EditorFrame* self, void* data, int filter);
	static void OnOpenDoc(EditorFrame* self, void* data, int filter);
	static void OnIncommingChanged(EditorFrame* self, void* data, int filter);
	static void OnBundleActionsReloaded(EditorFrame* self, void* data, int filter);
	static void OnBundlesReloaded(EditorFrame* self, void* data, int filter);

	void TogglePane(wxWindow* targetPane, bool showPane);

	// Member variables
	CatalystWrapper m_catalyst;
	Dispatcher& dispatcher;
	eSettings& m_generalSettings;
	eFrameSettings m_settings;
	TmSyntaxHandler& m_syntax_handler;

	wxImageList imageList;
	RemoteThread* m_remoteThread;
	DirWatcher* m_dirWatcher;

	// State
	bool m_sizeChanged;
	bool m_needStateSave;
	bool m_keyDiags;
	bool m_inAskReload;
	ChangeCheckerThread* m_changeCheckerThread;

	// Main Panel
	wxPanel* panel;
	EditorCtrl* editorCtrl;
	wxBoxSizer* box;
	wxBoxSizer*	editorbox;
	SearchPanel* m_searchPanel;
	eAuiNotebook* m_tabBar;

	// Menus
	wxMenu* m_tabMenu;
	wxMenu* m_recentFilesMenu;
	wxMenu* m_recentProjectsMenu;
	wxMenu* m_syntaxMenu;
	wxMenu* m_wrapMenu;

	// wxAUI and Panes
	wxAuiManager m_frameManager;
	UndoHistory* undoHistory;
	DocHistory* documentHistory;
	//Incomming* incommingPane;
	HtmlOutputPane* m_outputPane;
	ProjectPane* m_projectPane;
	BundlePane* m_bundlePane;
	DiffDirPane* m_diffPane;
	SymbolList* m_symbolList;
	FindInProjectDlg* m_findInProjectDlg;
	SnippetList* m_snippetList;

	// Statusbar
	StatusBar* m_pStatBar;
	//wxToolBarToolBase* m_pIncommingTool;
	//const wxBitmap m_incommingBmp;
	//const wxBitmap m_incommingFullBmp;

	// Preview
	PreviewDlg* m_previewDlg;

	// Last Active Tab
	bool m_ctrlHeldDown;
	int m_lastActiveTab;
	unsigned int m_contextTab;

	// Settings
	cxWrapMode m_wrapMode;
	bool m_showGutter;
	bool m_showIndent;
	bool m_searchHighlight;
	bool m_userHighlight;

	// tab state
	bool m_softTabs;
	int m_tabWidth;

	wxArrayString m_recentFiles;
	wxArrayString m_recentProjects;

	// Printing
	wxPageSetupDialogData m_printData;

	wxBitmap bitmap; // shared by all EditorCtrls in this frame

	DECLARE_DYNAMIC_CLASS_NO_COPY(EditorFrame)
};

#endif // __EDITORFRAME_H__
