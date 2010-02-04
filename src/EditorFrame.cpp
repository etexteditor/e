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

#include "EditorFrame.h"
#include "EditorCtrl.h"

#include <wx/regex.h>
#include <wx/icon.h>
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/ffile.h>
#include <wx/dir.h>
#include <wx/fontmap.h>
#include <wx/sysopt.h>
#include <wx/mstream.h>
#include <wx/progdlg.h>
#include <wx/printdlg.h>
#include <wx/recguard.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>

#include "eAbout.h"
#include "eApp.h"
#include "SaveDlg.h"
#include "tm_syntaxhandler.h"
#include "eDockArt.h"
#include "CommitDlg.h"
#include "ReloadDlg.h"
#include "OpenDocDlg.h"
#include "ShareDlg.h"
#include "SettingsDlg.h"
#include "ProjectPane.h"
#include "ThemeEditor.h"
#include "BundleManager.h"
#include "PreviewDlg.h"
#include "BundleMenu.h"
#include "GotoLineDlg.h"
#include "GotoFileDlg.h"
#include "SymbolList.h"
#include "ChangeCheckerThread.h"
#include "RemoteThread.h"
#include "RemoteProfileDlg.h"
#include "RemoteLoginDlg.h"
#include "EditorPrintout.h"
#include "EditorBundlePanel.h"
#include "BundlePane.h"
#include "UndoHistory.h"
#include "DocHistory.h"
#include "eDocumentPath.h"
#include "SearchPanel.h"
#include "StatusBar.h"
#include "DirWatcher.h"
#include "FindInProjectDlg.h"
#include "DiffPanel.h"
#include "CompareDlg.h"
#include "HtmlOutputPane.h"
#include "Strings.h"
#include "CurrentTabsPopup.h"
#include "eauibook.h"
#include "DiffDirPane.h"
#include "SnippetList.h"

#ifdef __WXMSW__
// For multi-monitor-aware position restore on Windows, include WinUser.h
#include "Winuser.h"
#include <wx/msw/registry.h>
#endif


class FrameDropTarget : public wxFileDropTarget {
public:
	FrameDropTarget(EditorFrame& parent): m_parent(parent) {};
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

private:
	EditorFrame& m_parent;
};

bool FrameDropTarget::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxArrayString& filenames) {
	m_parent.SetCursor(wxCURSOR_WAIT);

	for (unsigned int i = 0; i < filenames.GetCount(); ++i) {
		const wxString& path = filenames[i];
		m_parent.Open(path);
	}

	m_parent.SetCursor(*wxSTANDARD_CURSOR);
	return true;
}


enum EditorFrame_IDs { CTRL_TABBAR=100 };

wxString EditorFrame::DefaultFileFilters = wxT("All files (*.*)|*.*|Text files (*.txt)|*.txt|") \
						wxT("Batch Files (*.bat)|*.bat|INI Files (*.ini)|*.ini|") \
						wxT("C/C++ Files (*.c, *.cpp, *.cxx)|*.c;*.cpp;*.cxx|") \
						wxT("Header Files (*.h, *.hpp, *.hxx)|*.h;*.hpp;*.hxx|") \
						wxT("HTML Files (*.html, *.htm, *.css)|*.html;*.htm;*.css|") \
						wxT("Perl Files (*.pl, *.pm, *.pod)|*.pl;*.pm;*.pod|") \
						wxT("Python Files (*.py, *.pyw)|*.py;*.pyw");

// This sets RTTI info but without the dynamic part
wxIMPLEMENT_CLASS_COMMON1(EditorFrame, wxFrame, NULL)

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
	EVT_SIZE(EditorFrame::OnSize)

	EVT_MENU_OPEN(EditorFrame::OnOpeningMenu)

	// File menu
	EVT_MENU(wxID_NEW, EditorFrame::OnMenuNew)
	EVT_MENU(wxID_OPEN, EditorFrame::OnMenuOpen)
	EVT_MENU(MENU_DIFF, EditorFrame::OnMenuCompareFiles)

	EVT_MENU(MENU_NEWWINDOW, EditorFrame::OnMenuNewWindow)

	EVT_MENU(wxID_SAVE, EditorFrame::OnMenuSave)
	EVT_MENU(wxID_SAVEAS, EditorFrame::OnMenuSaveAs)
	EVT_MENU(MENU_SAVEALL, EditorFrame::OnMenuSaveAll)
	EVT_MENU(MENU_COMMIT, EditorFrame::OnMenuCommit)

	EVT_MENU(MENU_OPENPROJECT, EditorFrame::OnMenuOpenProject)
	EVT_MENU(MENU_OPENREMOTE, EditorFrame::OnMenuOpenRemote)
	EVT_MENU(MENU_CLOSEPROJECT, EditorFrame::OnMenuCloseProject)
	EVT_MENU(wxID_PAGE_SETUP, EditorFrame::OnMenuPageSetup)
	EVT_MENU(wxID_PRINT, EditorFrame::OnMenuPrint)
	EVT_MENU(MENU_CLOSE, EditorFrame::OnMenuClose)
	EVT_MENU(MENU_CLOSEWINDOW, EditorFrame::OnMenuCloseWindow)
	EVT_MENU(wxID_EXIT, EditorFrame::OnMenuExit)

	// Edit menu
	EVT_MENU(wxID_UNDO, EditorFrame::OnMenuUndo)
	EVT_MENU(wxID_REDO, EditorFrame::OnMenuRedo)
	EVT_MENU(wxID_CUT, EditorFrame::OnMenuCut)
	EVT_MENU(wxID_COPY, EditorFrame::OnMenuCopy)
	EVT_MENU(wxID_PASTE, EditorFrame::OnMenuPaste)

	EVT_MENU(wxID_SELECTALL, EditorFrame::OnMenuSelectAll)
	EVT_MENU(MENU_SELECTWORD, EditorFrame::OnMenuSelectWord)
	EVT_MENU(MENU_SELECTLINE, EditorFrame::OnMenuSelectLine)
	EVT_MENU(MENU_SELECTSCOPE, EditorFrame::OnMenuSelectScope)
	EVT_MENU(MENU_SELECTFOLD, EditorFrame::OnMenuSelectFold)
	EVT_MENU(MENU_SELECTTAG, EditorFrame::OnMenuSelectTag)

	EVT_MENU(wxID_FIND, EditorFrame::OnMenuFind)
	EVT_MENU(MENU_FIND_IN_PROJECT, EditorFrame::OnMenuFindInProject)
	EVT_MENU(MENU_FIND_IN_SEL, EditorFrame::OnMenuFindInSel)
	EVT_MENU(MENU_FIND_NEXT, EditorFrame::OnMenuFindNext)
	EVT_MENU(MENU_FIND_PREVIOUS, EditorFrame::OnMenuFindPrevious)
	EVT_MENU(MENU_FIND_CURRENT, EditorFrame::OnMenuFindCurrent)
	EVT_MENU(wxID_REPLACE, EditorFrame::OnMenuReplace)

	EVT_MENU(MENU_EDIT_THEME, EditorFrame::OnMenuEditTheme)
	EVT_MENU(MENU_SETTINGS, EditorFrame::OnMenuSettings)

	// View menu
	EVT_MENU(MENU_SHOWPROJECT, EditorFrame::OnMenuShowProject)
	EVT_MENU(MENU_SHIFT_PROJECT_FOCUS, EditorFrame::OnShiftProjectFocus)
	EVT_MENU(MENU_SHOWSYMBOLS, EditorFrame::OnMenuShowSymbols)
	EVT_MENU(MENU_SHOWSNIPPETS, EditorFrame::OnMenuShowSnippets)
	EVT_MENU(MENU_REVHIS, EditorFrame::OnMenuRevisionHistory)
	EVT_MENU(MENU_UNDOHIS, EditorFrame::OnMenuUndoHistory)
	EVT_MENU(MENU_COMMANDOUTPUT, EditorFrame::OnMenuShowCommandOutput)
	EVT_MENU(MENU_PREVIEW, EditorFrame::OnMenuPreview)

	EVT_MENU(MENU_LINENUM, EditorFrame::OnMenuLineNumbers)
	EVT_MENU(MENU_INDENTGUIDE, EditorFrame::OnMenuIndentGuide)

	EVT_MENU(MENU_WRAP_NONE, EditorFrame::OnMenuWordWrap)
	EVT_MENU(MENU_WRAP_NORMAL, EditorFrame::OnMenuWordWrap)
	EVT_MENU(MENU_WRAP_SMART, EditorFrame::OnMenuWordWrap)

	EVT_MENU(MENU_STATUSBAR, EditorFrame::OnMenuStatusbar)

	// Folds...
	EVT_MENU(MENU_FOLDTOGGLE, EditorFrame::OnMenuFoldToggle)
	EVT_MENU(MENU_FOLDALL, EditorFrame::OnMenuFoldAll)
	EVT_MENU(MENU_FOLDOTHERS, EditorFrame::OnMenuFoldOthers)
	EVT_MENU(MENU_UNFOLDALL, EditorFrame::OnMenuUnfoldAll)

	// Text menu
	// Convert...
	EVT_MENU(MENU_UPPERCASE, EditorFrame::OnMenuConvUpper)
	EVT_MENU(MENU_LOWERCASE, EditorFrame::OnMenuConvLower)
	EVT_MENU(MENU_TITLECASE, EditorFrame::OnMenuConvTitle)
	EVT_MENU(MENU_REVERSECASE, EditorFrame::OnMenuConvReverse)
			// Transpose
	EVT_MENU(MENU_REVSEL, EditorFrame::OnMenuRevSel)

	EVT_MENU(MENU_INDENTLEFT, EditorFrame::OnMenuIndentLeft)
	EVT_MENU(MENU_INDENTRIGHT, EditorFrame::OnMenuIndentRight)
	EVT_MENU(MENU_TABSTOSPACES, EditorFrame::OnMenuTabsToSpaces)
	EVT_MENU(MENU_SPACESTOTABS, EditorFrame::OnMenuSpacesToTabs)
	EVT_MENU(MENU_COMPLETE, EditorFrame::OnMenuCompleteWord)

	EVT_MENU(MENU_FILTER, EditorFrame::OnMenuFilter)
	EVT_MENU(MENU_RUN, EditorFrame::OnMenuRunCurrent)

	// Navigation menu
	// Bookmarks...
	EVT_MENU(MENU_BOOKMARK_TOGGLE, EditorFrame::OnMenuBookmarkToggle)
	EVT_MENU(MENU_BOOKMARK_NEXT, EditorFrame::OnMenuBookmarkNext)
	EVT_MENU(MENU_BOOKMARK_PREVIOUS, EditorFrame::OnMenuBookmarkPrevious)
	EVT_MENU(MENU_BOOKMARK_CLEAR, EditorFrame::OnMenuBookmarkClear)

	// Document tabs...
	EVT_MENU(MENU_GO_NEXT_TAB, EditorFrame::OnMenuNextTab)
	EVT_MENU(MENU_GO_PREV_TAB, EditorFrame::OnMenuPrevTab)
	EVT_MENU(MENU_GO_LAST_TAB, EditorFrame::OnMenuLastTab)

			// Go to Header
	EVT_MENU(MENU_OPEN_EXT, EditorFrame::OnMenuOpenExt) 
	EVT_MENU(MENU_GOTO_FILE, EditorFrame::OnMenuGotoFile)
	EVT_MENU(MENU_GOTO_SYMBOLS, EditorFrame::OnMenuSymbols)
	EVT_MENU(MENU_GOTO_BRACKET, EditorFrame::OnMenuGotoBracket)
	EVT_MENU(MENU_GOTO_LINE, EditorFrame::OnMenuGotoLine)

	// Help menu
	EVT_MENU(wxID_HELP_CONTENTS, EditorFrame::OnMenuHelp)
	EVT_MENU(MENU_HELP_FORUM, EditorFrame::OnMenuGotoForum)
	EVT_MENU(MENU_BUY, EditorFrame::OnMenuBuy)
	EVT_MENU(MENU_REGISTER, EditorFrame::OnMenuRegister)
	EVT_MENU(MENU_WEBSITE, EditorFrame::OnMenuWebsite)
	EVT_MENU(wxID_ABOUT, EditorFrame::OnMenuAbout)

	EVT_MENU(MENU_EOL_DOS, EditorFrame::OnMenuEolDos)
	EVT_MENU(MENU_EOL_UNIX, EditorFrame::OnMenuEolUnix)
	EVT_MENU(MENU_EOL_MAC, EditorFrame::OnMenuEolMac)
	EVT_MENU(MENU_EOL_NATIVE, EditorFrame::OnMenuEolNative)
	EVT_MENU(MENU_BOM, EditorFrame::OnMenuBOM)
	EVT_MENU(MENU_FINDCMD, EditorFrame::OnMenuFindCommand)
	EVT_MENU(MENU_RELOAD_BUNDLES, EditorFrame::OnMenuReloadBundles)
	EVT_MENU(MENU_DEBUG_BUNDLES, EditorFrame::OnMenuDebugBundles)
	EVT_MENU(MENU_EDIT_BUNDLES, EditorFrame::OnMenuEditBundles)
	EVT_MENU(MENU_MANAGE_BUNDLES, EditorFrame::OnMenuManageBundles)
	EVT_MENU(MENU_KEYDIAG, EditorFrame::OnMenuKeyDiagnostics)

	// Dynamic sub-menus
	EVT_MENU_RANGE(1000, 1999, EditorFrame::OnSubmenuSyntax)
	EVT_MENU_RANGE(2000, 2999, EditorFrame::OnSubmenuEncoding)
	EVT_MENU_RANGE(3000, 3999, EditorFrame::OnMenuOpen)
	EVT_MENU_RANGE(4000, 4099, EditorFrame::OnMenuOpenRecentFile)
	EVT_MENU_RANGE(4100, 4199, EditorFrame::OnMenuOpenRecentProject)
	EVT_MENU_RANGE(9000, 11999, EditorFrame::OnMenuBundleAction)
	EVT_MENU_RANGE(40000, 49999, EditorFrame::OnMenuGotoTab)

	EVT_ERASE_BACKGROUND(EditorFrame::OnEraseBackground)
	EVT_CLOSE(EditorFrame::OnClose)
	EVT_ACTIVATE(EditorFrame::OnActivate)
	EVT_AUINOTEBOOK_PAGE_CLOSE(CTRL_TABBAR, EditorFrame::OnCloseTab)
	EVT_AUINOTEBOOK_PAGE_CLOSED(CTRL_TABBAR, EditorFrame::OnTabClosed)
	EVT_AUINOTEBOOK_DRAG_DONE(CTRL_TABBAR, EditorFrame::OnNotebookDragDone)
	EVT_AUINOTEBOOK_PAGE_CHANGED(CTRL_TABBAR, EditorFrame::OnNotebook)
	EVT_AUINOTEBOOK_BG_DCLICK(CTRL_TABBAR, EditorFrame::OnNotebookDClick)
	EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(CTRL_TABBAR, EditorFrame::OnNotebookContextMenu)
	EVT_AUI_PANE_CLOSE(EditorFrame::OnPaneClose)
	//EVT_COMMAND(1, wxEVT_CHANGED_TAB, EditorFrame::OnNotebook)

	EVT_MENU(MENU_TABS_NEW, EditorFrame::OnMenuNew)
	EVT_MENU(MENU_TABS_CLOSE, EditorFrame::OnDoCloseTab)
	EVT_MENU(MENU_TABS_CLOSE_OTHER, EditorFrame::OnCloseOtherTabs)
	EVT_MENU(MENU_TABS_CLOSE_ALL, EditorFrame::OnCloseAllTabs)
	EVT_MENU(MENU_TABS_COPY_PATH, EditorFrame::OnCopyPathToClipboard)
	EVT_MENU(MENU_TABS_SHOWDROPDOWN, EditorFrame::OnTabsShowDropdown)

	EVT_MOVE(EditorFrame::OnMove)
	EVT_MAXIMIZE(EditorFrame::OnMaximize)
	EVT_MOUSE_CAPTURE_LOST (EditorFrame::OnMouseCaptureLost)
	EVT_IDLE(EditorFrame::OnIdle)
	EVT_KEY_UP(EditorFrame::OnKeyUp)
	EVT_FILESCHANGED(EditorFrame::OnFilesChanged)

	EVT_MOUSEWHEEL(EditorFrame::OnMouseWheel)

	//EVT_MENU(MENU_DOC_OPEN, EditorFrame::OnMenuDocOpen)
	//EVT_MENU(MENU_DOC_SHARE, EditorFrame::OnMenuDocShare)
	//EVT_MENU(MENU_REVTOOLTIP, EditorFrame::OnMenuRevTooltip)
	//EVT_MENU(MENU_INCOMMING, EditorFrame::OnMenuIncomming)
	//EVT_MENU(MENU_INCOMMING_TOOLBAR, EditorFrame::OnMenuIncommingTool)
	//EVT_MENU(MENU_HL_USERS, EditorFrame::OnMenuHighlightUsers)
	//EVT_MENU(wxID_PREVIEW, EditorFrame::OnMenuPrintPreview)
END_EVENT_TABLE()

EditorFrame::EditorFrame(CatalystWrapper cat, unsigned int frameId,  const wxString& title, const wxRect& rect, TmSyntaxHandler& syntax_handler):
	m_catalyst(cat),
	dispatcher(cat.GetDispatcher()),
	m_generalSettings(eGetSettings()),
	m_settings(m_generalSettings.GetFrameSettings(frameId)),
	m_syntax_handler(syntax_handler),

	m_sizeChanged(false), m_needStateSave(true), m_keyDiags(false), m_inAskReload(false),
	m_changeCheckerThread(NULL), editorCtrl(0), m_recentFilesMenu(NULL), m_recentProjectsMenu(NULL), m_bundlePane(NULL), m_diffPane(NULL),
	m_symbolList(NULL), m_findInProjectDlg(NULL), m_pStatBar(NULL), m_snippetList(NULL),
	m_previewDlg(NULL), m_ctrlHeldDown(false), m_lastActiveTab(0), m_showGutter(true), m_showIndent(false),
	bitmap(1,1)
	//,m_incommingBmp(incomming_xpm), m_incommingFullBmp(incomming_full_xpm)
{
	Create(NULL, wxID_ANY, title, rect.GetPosition(), rect.GetSize(), wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE|wxWANTS_CHARS, wxT("eMainFrame"));

	m_remoteThread = new RemoteThread();

	// Create the dirwatcher (will not be explicitly deleted, deletes as thread on app exit)
	m_dirWatcher = new DirWatcher();

	// Create the FrameManager
	m_frameManager.SetManagedWindow(this);
	m_frameManager.SetFlags(m_frameManager.GetFlags() | wxAUI_MGR_TRANSPARENT_DRAG | wxAUI_MGR_ALLOW_ACTIVE_PANE);
	m_frameManager.SetArtProvider(new ModernDockArt(this));

	// Create statusbar
	InitStatusbar();

	Freeze();
	{
		// Load Settings
		LoadSize();
		InitMemberSettings();

		// Set drop target so files can be dragged from explorer
		SetDropTarget(new FrameDropTarget(*this));

		box = new wxBoxSizer(wxVERTICAL);
		panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE);

		// set up default notebook style
		const int notebook_style = wxAUI_NB_TOP
								| wxAUI_NB_TAB_MOVE
								| wxAUI_NB_TAB_SPLIT
								| wxAUI_NB_SCROLL_BUTTONS
								| wxAUI_NB_CLOSE_ON_ALL_TABS
								| wxAUI_NB_WINDOWLIST_BUTTON
								| wxAUI_NB_MIDDLE_CLICK_CLOSE
								| wxNO_BORDER;
		m_tabBar = new eAuiNotebook(panel, CTRL_TABBAR,
									wxDefaultPosition,
									wxSize(430,200),
									notebook_style);
		m_tabBar->SetArtProvider(new ModernTabArt());
		m_tabBar->Refresh();
		box->Add(m_tabBar, 1, wxEXPAND);

		// Create and add the searchpanel
		m_searchPanel = new SearchPanel(*this, panel, wxID_ANY, wxDefaultPosition, wxSize(10,25));
		box->Add(m_searchPanel, 0, wxEXPAND);
		box->Show(m_searchPanel, false);

		// Layout main components
		box->Layout();
		panel->SetSizer(box);

		//// Layout the Docks in the FrameManager
		// The main text area
		m_frameManager.AddPane(panel, wxAuiPaneInfo().Name(wxT("Center")).CenterPane().PaneBorder(false));

		// Add the Document History Pane
		documentHistory = new DocHistory(m_catalyst, GetId(), this, wxID_ANY); 
		m_frameManager.AddPane(documentHistory, wxAuiPaneInfo().Name(wxT("History")).Hide().Right().Caption(_("History")).BestSize(wxSize(150,200)));

		// Add the Undo History Pane
		undoHistory = new UndoHistory(m_catalyst, this, GetId(), this, wxID_ANY);
		m_frameManager.AddPane(undoHistory, wxAuiPaneInfo().Name(wxT("Undo")).Hide().Right().Caption(_("Undo")).BestSize(wxSize(150,50)));

		m_outputPane = new HtmlOutputPane(this, *this);
		m_frameManager.AddPane(m_outputPane, wxAuiPaneInfo().Name(wxT("Output")).Hide().Bottom().Caption(_("Output")).BestSize(wxSize(150,100)));

		// Project dock
		m_projectPane = new ProjectPane(*this, this);
		m_frameManager.AddPane(m_projectPane, wxAuiPaneInfo().Name(wxT("Project")).Left().Caption(_("Project")).BestSize(wxSize(150,50)));

		// See if we have saved the layout of the panes
		wxString perspective;
		const bool perspectiveSaved = m_settings.GetSettingString(wxT("topwin/panes"), perspective);
		if (perspectiveSaved && !perspective.empty()) {
			m_frameManager.LoadPerspective(perspective, false);

			// Always hide the output pane on startup (since we have not saved content)
			wxAuiPaneInfo& outPane = m_frameManager.GetPane(m_outputPane);
			outPane.Hide();

			// Previous versions may have saved centerpane with borders
			wxAuiPaneInfo& centerPane = m_frameManager.GetPane(panel);
			centerPane.PaneBorder(false);
		}

		// Open project from last session
		bool showProject;
		if (!m_settings.GetSettingBool(wxT("showproject"), showProject)) showProject = false;
		
		wxString projectPath;
		const bool hasProject = m_settings.GetSettingString(wxT("project"), projectPath);

#ifdef __WXGTK__ // FIXME
		Show();
#endif

		if (showProject && hasProject && !projectPath.empty())
			OpenProject(projectPath);

		// Check if we should show web preview
		{
			bool showpreview = false;
			m_settings.GetSettingBool(wxT("showpreview"), showpreview);
			if (showpreview) ShowWebPreview();
		}

		// Check if we should show symbol list
		{
			bool showsymbols = false;
			m_settings.GetSettingBool(wxT("showsymbols"), showsymbols);
			if (showsymbols) ShowSymbolList();
		}

		m_frameManager.Update();

		InitAccelerators();
		InitMenus();

#ifdef __WXMSW__
		// set the application icon
		wxIcon icon(wxICON(APP_ICON));
		SetIcon(icon);
#endif

		// Add empty document
		AddTab();
	}
	Thaw();

	// Make sure that we get notified when the document changes
	dispatcher.SubscribeC(wxT("DOC_NEWREVISION"), (CALL_BACK)OnDocChange, this);
	dispatcher.SubscribeC(wxT("DOC_UPDATEREVISION"), (CALL_BACK)OnDocChange, this);
	dispatcher.SubscribeC(wxT("FRAME_OPEN_DOC"), (CALL_BACK)OnOpenDoc, this);
	dispatcher.SubscribeC(wxT("INCOMMING_CHANGED"), (CALL_BACK)OnIncommingChanged, this);
	dispatcher.SubscribeC(wxT("BUNDLE_ACTIONS_RELOADED"), (CALL_BACK)OnBundleActionsReloaded, this);
	dispatcher.SubscribeC(wxT("BUNDLES_RELOADED"), (CALL_BACK)OnBundlesReloaded, this);
}

EditorFrame::~EditorFrame() {
	dispatcher.UnSubscribe(wxT("DOC_NEWREVISION"), (CALL_BACK)OnDocChange, this);
	dispatcher.UnSubscribe(wxT("DOC_UPDATEREVISION"), (CALL_BACK)OnDocChange, this);
	dispatcher.UnSubscribe(wxT("FRAME_OPEN_DOC"), (CALL_BACK)OnOpenDoc, this);
	dispatcher.UnSubscribe(wxT("INCOMMING_CHANGED"), (CALL_BACK)OnIncommingChanged, this);
	dispatcher.UnSubscribe(wxT("BUNDLE_ACTIONS_RELOADED"), (CALL_BACK)OnBundleActionsReloaded, this);
	dispatcher.UnSubscribe(wxT("BUNDLES_RELOADED"), (CALL_BACK)OnBundlesReloaded, this);

	// deinitialize the frame manager
    m_frameManager.UnInit();

	if (undoHistory) undoHistory->Destroy();
	if (m_changeCheckerThread) m_changeCheckerThread->Kill(); // may be locked on network drive
}


// Loads settings that end up as member variables
void EditorFrame::InitMemberSettings() {
	if (!m_settings.GetSettingInt(wxT("wrapmode"), (int&)m_wrapMode)) {
		// fallback to old settings format
		bool wordwrap = cxWRAP_NORMAL;
		if (m_generalSettings.GetSettingBool(wxT("wordwrap"), wordwrap))
			m_wrapMode = wordwrap ? cxWRAP_NORMAL : cxWRAP_NONE;
	}

#ifdef __WXMSW__
	// Save info about the wordwrap mode to the registry so that the crash handler
	// can include it in crash report
	wxRegKey regKey(wxT("HKEY_CURRENT_USER\\Software\\e"));
	const wxString ww = wxString::Format(wxT("%d"), (int)m_wrapMode);
	regKey.SetValue(wxT("ww"), ww);
#endif // __WXMSW__

	if (!m_settings.GetSettingBool(wxT("showgutter"), m_showGutter)) m_showGutter = true;
	if (!m_settings.GetSettingBool(wxT("hl_users"), m_userHighlight)) m_userHighlight = true;
	if (!m_settings.GetSettingBool(wxT("softtabs"), m_softTabs)) m_softTabs = false;
	if (!m_settings.GetSettingInt(wxT("tabwidth"), m_tabWidth)) m_tabWidth = 4;
	if (!m_settings.GetSettingBool(wxT("showindent"), m_showIndent)) m_showIndent = false;
}

void EditorFrame::InitStatusbar() {
	bool showStatusbar = true;
	m_generalSettings.GetSettingBool(wxT("statusbar"), showStatusbar);
	if (showStatusbar) CreateAndSetStatusbar();
}

void EditorFrame::InitAccelerators() {
	const unsigned int accelcount = 5;
	wxAcceleratorEntry entries[accelcount];
	entries[0].Set(wxACCEL_CTRL|wxACCEL_SHIFT, (int)'P', MENU_SHIFT_PROJECT_FOCUS);
	entries[1].Set(wxACCEL_NORMAL, WXK_F3, MENU_FIND_NEXT);
	entries[2].Set(wxACCEL_SHIFT, WXK_F3, MENU_FIND_PREVIOUS);
	entries[3].Set(wxACCEL_CTRL, WXK_F3, MENU_FIND_CURRENT);
	entries[4].Set(wxACCEL_CTRL, WXK_F4, MENU_CLOSE);
	wxAcceleratorTable accel(accelcount, entries);
	SetAcceleratorTable(accel);
}

void EditorFrame::InitMenus() {
	wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

	// Encoding (in Import submenu)
	wxMenu* importMenu = new wxMenu;
	size_t enc_count = wxFontMapper::GetSupportedEncodingsCount();
	for (size_t enc_id = 0; enc_id < enc_count; ++enc_id) {
		const wxFontEncoding enc = wxFontMapper::GetEncoding(enc_id);
		wxString enc_name = wxFontMapper::GetEncodingDescription(enc);
		if (enc == wxFONTENCODING_DEFAULT)
			enc_name += wxT(" (") + wxFontMapper::GetEncodingName(wxLocale::GetSystemEncoding()) + wxT(")");

		importMenu->Append(3000+enc_id, enc_name, enc_name);
	}
	// "Save format" menu (in file menu)
	wxMenu* formatMenu = new wxMenu;
	CreateEncodingMenu(*formatMenu);

	// File menu
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append(wxID_NEW, _("&New\tCtrl+N"), _("New File"));
	fileMenu->Append(wxID_OPEN, _("&Open...\tCtrl+O"), _("Open File"));
	fileMenu->Append(MENU_DIFF, _("&Compare..."), _("Compare..."));
	fileMenu->Append(MENU_NEWWINDOW, _("New Window"), _("Open New Window"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_SAVE, _("&Save\tCtrl+S"), _("Save File"));
	fileMenu->Append(wxID_SAVEAS, _("Save &As...\tCtrl-Shift-S"), _("Save File as"));
	fileMenu->Append(MENU_SAVEALL, _("Save A&ll\tCtrl-Shift-A"), _("Save All Files"));
	fileMenu->Append(MENU_COMMIT, _("&Make Milestone...\tCtrl+M"), _("Make Milestone of current revision"));
	fileMenu->AppendSeparator();
	fileMenu->Append(MENU_OPENPROJECT, _("Open &Dir as Project..."), _("Open Dir as Project"));
	fileMenu->Append(MENU_OPENREMOTE, _("Open Remote Folder (&FTP)..."), _("Open Remote Folder"));
	fileMenu->Append(MENU_CLOSEPROJECT, _("Close Pro&ject"), _("Close Project"));
	fileMenu->AppendSeparator();
	fileMenu->Append(MENU_SAVEFORMAT, _("Sa&ve format"), formatMenu, _("Save format"));
	fileMenu->Append(MENU_IMPORT, _("&Import"), importMenu, _("Import"));
	fileMenu->AppendSeparator();
	m_recentFilesMenu = new wxMenu;
	fileMenu->Append(MENU_RECENT_FILES, _("&Recent Files"), m_recentFilesMenu, _("Open recent Files"));
	m_recentProjectsMenu = new wxMenu;
	fileMenu->Append(MENU_RECENT_PROJECTS, _("&Recent Projects"), m_recentProjectsMenu, _("Open recent Projects"));
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_PAGE_SETUP, _("Page Set&up..."), _("Page setup"));
	//fileMenu->Append(wxID_PREVIEW, _("Print Pre&view"), _("Preview"));
	fileMenu->Append(wxID_PRINT, _("&Print..."), _("Print"));
	fileMenu->AppendSeparator();
	fileMenu->Append(MENU_CLOSE, _("&Close Tab\tCtrl+W"), _("Close Tab"));
	fileMenu->Append(MENU_TABS_CLOSE_ALL, _("Close all &Tabs"), _("Close all Tabs"));
	fileMenu->Append(MENU_TABS_CLOSE_OTHER, _("Clos&e other Tabs\tCtrl+Alt+W"), _("Close other Tabs"));
	fileMenu->Append(MENU_CLOSEWINDOW, _("Close &Window"), _("Close Window"));
	fileMenu->Append(wxID_EXIT, _("E&xit"), _("Exit"));
	menuBar->Append(fileMenu, _("&File"));

	UpdateRecentFiles();

	// Syntax submenu (in edit menu)
	m_syntaxMenu = new wxMenu;
	ResetSyntaxMenu();

	// Edit menu
	wxMenu *editMenu = new wxMenu;
	editMenu->Append(wxID_UNDO, _("&Undo\tCtrl+Z"), _("Undo"));
	editMenu->Append(wxID_REDO, _("&Redo\tCtrl+Y"), _("Redo"));
	editMenu->AppendSeparator();
	editMenu->Append(wxID_CUT, _("&Cut\tCtrl+X"), _("Cut"));
	editMenu->Append(wxID_COPY, _("&Copy\tCtrl+C"), _("Copy"));
	editMenu->Append(wxID_PASTE, _("&Paste\tCtrl+V"), _("Paste"));
	editMenu->AppendSeparator();

	wxMenu* selectMenu = new wxMenu;
		selectMenu->Append(wxID_SELECTALL, _("&All\tCtrl+A"), _("Select All"));
		selectMenu->Append(MENU_SELECTWORD, _("&Word\tCtrl+Shift+W"), _("Select Word"));
		selectMenu->Append(MENU_SELECTLINE, _("&Line\tCtrl+Shift+L"), _("Select Line"));
		selectMenu->Append(MENU_SELECTSCOPE, _("&Current Scope\tCtrl+Shift+Space"), _("Select Current Scope"));
		selectMenu->Append(MENU_SELECTFOLD, _("Current &Fold\tShift-F1"), _("Select Current Fold"));
		selectMenu->Append(MENU_SELECTTAG, _("Tag &Content\tCtrl+,"), _("Select Parent Tag Content"));
		editMenu->Append(MENU_SELECT, _("&Select"), selectMenu,  _("Select"));

	editMenu->AppendSeparator();
	wxMenu* findMenu = new wxMenu;
		findMenu->Append(wxID_FIND, _("&Find\tCtrl+F"), _("Find"));
		findMenu->Append(MENU_FIND_IN_SEL, _("Find &in Selection\tCtrl+Shift+F"), _("Find in Selection"));
		findMenu->Append(MENU_FIND_IN_PROJECT, _("Find in &Project\tCtrl-Alt-F"), _("Find in Project"));
		findMenu->Append(wxID_REPLACE, _("&Replace\tCtrl+R"), _("Replace"));
		editMenu->Append(MENU_FIND_AND_REPLACE,  _("&Find and Replace"), findMenu, _("Find and Replace"));

	editMenu->AppendSeparator();
	editMenu->Append(MENU_SYNTAX, _("S&yntax"), m_syntaxMenu, _("Syntax"));
	editMenu->AppendSeparator();
	editMenu->Append(MENU_EDIT_THEME, _("Edit &Theme..."), _("Edit Theme"));
	editMenu->Append(MENU_SETTINGS, _("S&ettings..."), _("Edit Settings"));
	menuBar->Append(editMenu, _("&Edit"));

	// View menu
	wxMenu *viewMenu = new wxMenu;
	viewMenu->Append(MENU_SHOWPROJECT, _("&Project Pane\tCtrl-P"), _("Show Project Pane"), wxITEM_CHECK);
	//viewMenu->Append(MENU_INCOMMING, _("&Incoming\tF2"), _("Show Incomming Documents"), wxITEM_CHECK);
	//viewMenu->Check(MENU_INCOMMING, true);
	viewMenu->Append(MENU_SHOWSYMBOLS, _("&Symbol List\tCtrl+Alt+L"), _("Show Symbol List"), wxITEM_CHECK);
	viewMenu->Append(MENU_SHOWSNIPPETS, _("&Snippet List\tCtrl+Alt+S"), _("Show Snippet List"), wxITEM_CHECK);
	viewMenu->Append(MENU_REVHIS, _("&Revision History\tF6"), _("Show Revision History"), wxITEM_CHECK);
	viewMenu->Check(MENU_REVHIS, true);
	viewMenu->Append(MENU_UNDOHIS, _("&Undo History\tF7"), _("Show Undo History"), wxITEM_CHECK);
	viewMenu->Check(MENU_UNDOHIS, true);
	viewMenu->Append(MENU_COMMANDOUTPUT, _("&Command Output\tF12"), _("Show Command Output"), wxITEM_CHECK);
	viewMenu->Check(MENU_COMMANDOUTPUT, false);
	viewMenu->Append(MENU_PREVIEW, _("Web Pre&view\tCtrl+Alt+P"), _("Show Web Preview"), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MENU_LINENUM, _("&Line Numbers"), _("Show Line Numbers"), wxITEM_CHECK);
	viewMenu->Check(MENU_LINENUM, m_showGutter);
	viewMenu->Append(MENU_INDENTGUIDE, _("&Indent Guides"), _("Show Indent Guides"), wxITEM_CHECK);
	viewMenu->Check(MENU_INDENTGUIDE, m_showIndent);
	viewMenu->AppendSeparator();
	m_wrapMenu = new wxMenu;
		m_wrapMenu->Append(MENU_WRAP_NONE, _("&None"), _("Disable Word Wrap"), wxITEM_CHECK);
		m_wrapMenu->Check(MENU_WRAP_NONE, (m_wrapMode == cxWRAP_NONE));
		m_wrapMenu->Append(MENU_WRAP_NORMAL, _("Nor&mal"), _("Enable Word Wrap"), wxITEM_CHECK);
		m_wrapMenu->Check(MENU_WRAP_NORMAL, (m_wrapMode == cxWRAP_NORMAL));
		m_wrapMenu->Append(MENU_WRAP_SMART, _("&Smart"), _("Smart Word Wrap"), wxITEM_CHECK);
		m_wrapMenu->Check(MENU_WRAP_SMART, (m_wrapMode == cxWRAP_SMART));
		viewMenu->Append(MENU_WORDWRAP, _("&Word Wrap"), m_wrapMenu, _("Word Wrap"));
	viewMenu->AppendSeparator();
	//viewMenu->Append(MENU_HL_USERS, _("&Highlight Authors"), _("Highlight authors"), wxITEM_CHECK);
	//viewMenu->AppendSeparator();
	viewMenu->Append(MENU_STATUSBAR, _("Show Status&bar"), _("Show Statusbar"), wxITEM_CHECK);
	viewMenu->AppendSeparator();
	viewMenu->Append(MENU_FOLDTOGGLE, _("&Toggle Fold\tF1"), _("Toggle Fold"));
	viewMenu->Append(MENU_FOLDALL, _("Fold &All\tCtrl-F1"), _("Fold All"));
	viewMenu->Append(MENU_FOLDOTHERS, _("Fold &Others\tAlt-F1"), _("Fold Others"));
	viewMenu->Append(MENU_UNFOLDALL, _("&Unfold All\tCtrl-Alt-F1"), _("Unfold &All"));
	menuBar->Append(viewMenu, _("&View"));

	// Text menu
	wxMenu *textMenu = new wxMenu;
		wxMenu *convMenu = new wxMenu;
		convMenu->Append(MENU_UPPERCASE, _("to &Uppercase\tCtrl-U"), _("Convert to Uppercase"));
		convMenu->Append(MENU_LOWERCASE, _("to &Lowercase\tCtrl-Shift-U"), _("Convert to Lowercase"));
		convMenu->Append(MENU_TITLECASE, _("to &Titlecase\tCtrl-Alt-U"), _("Convert to Titlecase"));
		convMenu->Append(MENU_REVERSECASE, _("to &Opposite Case\tAlt-G"), _("Convert to Opposite Case"));
		convMenu->AppendSeparator();
		convMenu->Append(MENU_REVSEL, _("T&ranspose\tCtrl+T"), _("Transpose text"));
	textMenu->Append(MENU_TEXTCONV, _("&Convert"), convMenu,  _("Convert Selections"));
	textMenu->AppendSeparator();
	textMenu->Append(MENU_INDENTLEFT, _("Shift &Left"), _("Shift Left"));
	textMenu->Append(MENU_INDENTRIGHT, _("Shift &Right"), _("Shift Right"));
	textMenu->AppendSeparator();
	textMenu->Append(MENU_TABSTOSPACES, _("&Tabs to Spaces"), _("Tabs to Spaces"));
	textMenu->Append(MENU_SPACESTOTABS, _("&Spaces to Tabs"), _("Spaces to Tabs"));
	textMenu->AppendSeparator();
	textMenu->Append(MENU_COMPLETE, _("Complete &Word\tEscape"), _("Complete Word"));
	textMenu->AppendSeparator();
	textMenu->Append(MENU_FILTER, _("&Filter Through Command...\tCtrl-H"), _("Filter Through Command..."));
	textMenu->Append(MENU_RUN, _("&Run current line/selection\tCtrl-Alt-R"), _("Run current line/selection"));
	menuBar->Append(textMenu, _("&Text"));

	// Navigation menu
	wxMenu *navMenu = new wxMenu;
	navMenu->Append(MENU_BOOKMARK_TOGGLE, _("Add &Bookmark\tCtrl-F2"), _("Add Bookmark"));
	navMenu->Append(MENU_BOOKMARK_NEXT, _("&Next Bookmark\tF2"), _("Go to Next Bookmark"));
	navMenu->Append(MENU_BOOKMARK_PREVIOUS, _("&Previous Bookmark\tShift-F2"), _("Go to Previous Bookmark"));
	navMenu->Append(MENU_BOOKMARK_CLEAR, _("&Remove All Bookmarks\tCtrl-Shift-F2"), _("Remove All Bookmarks"));
	navMenu->AppendSeparator();
	navMenu->Append(MENU_GO_NEXT_TAB, _("N&ext Tab\tCtrl-Tab"), _("Next Tab"));
	navMenu->Append(MENU_GO_PREV_TAB, _("Pre&vious Tab\tCtrl-Shift-Tab"), _("Previous Tab"));
	navMenu->Append(MENU_TABS_SHOWDROPDOWN, _("Go to &Tab...\tCtrl-0"), _("Go to Tab..."));
	navMenu->Append(MENU_GO_LAST_TAB, _("L&ast used Tab\tCtrl-Alt-0"), _("Last used Tab"));
	m_tabMenu = new wxMenu; // Tab submenu (in navigation menu)
	navMenu->Append(MENU_TABS, _("Ta&bs"), m_tabMenu, _("Tabs"));
	navMenu->AppendSeparator();
	navMenu->Append(MENU_OPEN_EXT, _("Go to &Header/Source\tCtrl-Alt-Up"), _(""));
	navMenu->Append(MENU_GOTO_FILE, _("Go to &File...\tCtrl-Shift-T"), _("Go to File..."));
	navMenu->Append(MENU_GOTO_SYMBOLS, _("Go to &Symbol...\tCtrl-L"), _("Show Symbol List"));
	navMenu->Append(MENU_GOTO_BRACKET, _("Go to &Matching Bracket\tCtrl-B"), _("Go to Matching Bracket"));
	navMenu->Append(MENU_GOTO_LINE, _("Go to &Line...\tCtrl-G"), _("Go to Line..."));
	menuBar->Append(navMenu, _("&Navigation"));

	// Document menu
	/*wxMenu *docMenu = new wxMenu;
	//docMenu->Append(MENU_DOC_OPEN, _("&Retrieve Versioned Doc..."), _("&Retrieve Versioned Document"));
	docMenu->Append(MENU_COMMIT, _("&Make Milestone...\tCtrl+M"), _("Make Milestone of current revision"));
	docMenu->Append(MENU_DOC_SHARE, _("&Share..."), _("&Share Document"));
	docMenu->AppendSeparator();
	docMenu->Append(MENU_REVTOOLTIP, _("Author &Info...\tCtrl+L"), _("Show Author info for current character"));
	menuBar->Append(docMenu, _("&Document")); */

	// Bundles menu
	wxMenu* bundleMenu = GetBundleMenu();
	menuBar->Append(bundleMenu, _("&Bundles"));

	// Help menu
	wxMenu *helpMenu = new wxMenu;
	if (!wxGetApp().IsRegistered()) {
		// These are deleted again in RemoveRegMenus()
		// BEWARE of changing their IDs and positions!
		helpMenu->Append(MENU_BUY, _("&Buy"), _("Buy"));
		helpMenu->Append(MENU_REGISTER, _("&Enter Registration Code"), _("Enter Registration Code"));
		helpMenu->AppendSeparator();
	}
	helpMenu->Append(wxID_HELP_CONTENTS, _("&Help Contents"), _("Help Contents"));
	helpMenu->Append(MENU_HELP_FORUM, _("&Go to Support &Forum"), _("Go to Support Forum"));
	helpMenu->Append(MENU_WEBSITE, _("Go to &Website"), _("Goto Website"));
	helpMenu->AppendSeparator();
	helpMenu->Append(wxID_ABOUT, _("&About e"), _("About"));
	menuBar->Append(helpMenu, _("&Help"));
	//helpMenu->AppendSeparator();
	//helpMenu->Append(MENU_KEYDIAG, _("Key &Diagnostics"), _("Key Diagnostics"), wxITEM_CHECK);

	// associate the menu bar with the frame
	SetMenuBar(menuBar);
}

void EditorFrame::RestoreState() {
#ifdef __WXMSW__ //LINUX: removed until wxWidgets rebuild
	const unsigned int pagecount = m_settings.GetPageCount();

	if (pagecount <= 0) {
		// Set last active tab to current
		m_lastActiveTab = m_tabBar->GetSelection();
		return;
	}

	wxLogDebug(wxT("Opening %d documents from last session"), pagecount);

	// Only show progress dialog if more than 3 pages
	wxProgressDialog* dlg = NULL;
	const wxString msg = _("Opening documents from last session");
	if (pagecount > 3)
		dlg = new wxProgressDialog(wxT("Progress"), msg, pagecount, this, wxPD_APP_MODAL|wxPD_SMOOTH);

	Freeze();
	// Remove initial empty document
	wxASSERT(m_tabBar->GetPageCount() == 1 && editorCtrl->IsEmpty());
	DeletePage(0, true);

	// Get selection and layout
	int page_id;
	wxString tablayout;
	bool hasSelection = m_settings.GetSettingInt(wxT("topwin/page_id"), page_id);
	m_settings.GetSettingString(wxT("topwin/tablayout"), tablayout);

	// Open documents from last session
	// CheckForModifiedFiles() is called from eApp::OnInit()
	for (unsigned int i = 0; i < pagecount; ++i) {
		const bool isDiff = m_settings.IsPageDiff(i);
		wxString mirrorPath;

		// Check that the paths are still mirrored (not needed for unsaved)
		bool isMirrored = true;
		unsigned int sp = isDiff ? 1 : 0;
		const unsigned int sp_end = isDiff ? 3 : 1; // diffs have both left and rightd
		for (; sp < sp_end; ++sp) {
			mirrorPath = m_settings.GetPagePath(i, (SubPage)sp);
			if (mirrorPath.empty()) continue;
			const doc_id mirrorDoc = m_settings.GetPageDoc(i, (SubPage)sp);
			
			cxLOCK_READ(m_catalyst)
				isMirrored = catalyst.VerifyMirror(mirrorPath, mirrorDoc);
				if (!isMirrored) break;
			cxENDLOCK
		}
		if (!isMirrored) continue;

		// Update progress dialog
		if (dlg) {
			if (isDiff) mirrorPath = _("Diff");
			else if (mirrorPath.empty()) mirrorPath = _("Untitled");

			dlg->Update(i, msg + wxT("\n") + mirrorPath);
		}

		// Create the page
		wxWindow* page = NULL;
		if (isDiff) {
			DiffPanel* diff = new DiffPanel(m_tabBar, *this, m_catalyst, bitmap);
			diff->RestoreSettings(i, m_settings);
			page = diff;
		}
		else if (eDocumentPath::IsBundlePath(mirrorPath)) {
			page = new EditorBundlePanel(i, m_tabBar, *this, m_catalyst, bitmap);
		}
		else {
			EditorCtrl* ec = NULL;
			page = ec = new EditorCtrl(i, m_catalyst, bitmap, m_tabBar, *this);

			// Remote files may fail to be downloaded
			if (ec->IsRemote() && !ec->GetFilePath().IsOk()) {
				delete ec;
				if ((int)i >= page_id) hasSelection = false;
				// TODO: better handling of deleted pages
				continue;
			}
		}
		
		page->Hide();
		AddTab(page);
	}

	// There might have been pages we could not open (remote)
	// So if all are gone, we have to add an initial tab
	if (m_tabBar->GetPageCount() == 0) AddTab();
	else {
		if (!tablayout.empty()) {
			m_tabBar->LoadPerspective(tablayout);
			UpdateTabMenu();
		}
		else if (hasSelection) m_tabBar->SetSelection(page_id);
	}
	Thaw();

	if (dlg) delete dlg;

	// Set last active tab to current
	m_lastActiveTab = m_tabBar->GetSelection();

	//The Snippet handler would occasionally segfault when it was initialized in its previous place.
	//Moving it after the above call to SetSyntax seems to fix the problem
	// Check if we should show snippet list
	{
		bool showsnippets = false;
		m_settings.GetSettingBool(wxT("showsnippets"), showsnippets);
		if (showsnippets) ShowSnippetList();
	}

#endif // __WXMSW__
}

wxMenu* EditorFrame::GetBundleMenu() {
	wxMenu* bundleMenu = m_syntax_handler.GetBundleMenu(); // Uses IDs in range 9000-9999
	if (!bundleMenu) bundleMenu = new wxMenu;

	bool enableDebug = false; // default setting
	m_settings.GetSettingBool(wxT("bundleDebug"), enableDebug);

	wxMenu *funcMenu = new wxMenu;
	funcMenu->Append(MENU_EDIT_BUNDLES, _("Show Bundle &Editor\tCtrl-Shift-B"), _("Show Bundle Editor"), wxITEM_CHECK);
	funcMenu->Append(MENU_MANAGE_BUNDLES, _("&Manage Bundles"), _("Show Bundle Manager"));
	funcMenu->AppendSeparator();
	funcMenu->Append(MENU_DEBUG_BUNDLES, _("Enable &Debug Mode"), _("Enable Debug Mode"), wxITEM_CHECK);
	funcMenu->Check(MENU_DEBUG_BUNDLES, enableDebug);
	funcMenu->Append(MENU_RELOAD_BUNDLES, _("&Reload Bundles"), _("Reload Bundles"));

	bundleMenu->PrependSeparator();
	bundleMenu->Prepend(MENU_BUNDLE_FUNCTIONS, _("&Edit Bundles"), funcMenu,  _("Edit Bundles"));
	bundleMenu->PrependSeparator();
	bundleMenu->Prepend(MENU_FINDCMD, _("&Select Bundle Item...\tCtrl-Alt-T"), _("Select Bundle Item..."));

	return bundleMenu;
}

void EditorFrame::ResetBundleMenu() {
	wxMenuBar* menuBar = GetMenuBar();
	const int menuNdx = menuBar->FindMenu(_("&Bundles"));
	if (menuNdx == wxNOT_FOUND) return;

	// Delete the bundles menu
	wxMenu* oldMenu = menuBar->Remove(menuNdx);
	if (oldMenu) delete oldMenu;

	// Insert new bundles menu
	wxMenu* bundleMenu = GetBundleMenu();
	menuBar->Insert(menuNdx, bundleMenu, _("&Bundles"));
}

void EditorFrame::ResetSyntaxMenu() {
	// Clear all items
	while (m_syntaxMenu->GetMenuItemCount())
		m_syntaxMenu->Destroy(m_syntaxMenu->FindItemByPosition(0));

	// Get syntaxes and sort
	vector<cxSyntaxInfo*> syntaxes = m_syntax_handler.GetSyntaxes();
	sort(syntaxes.begin(), syntaxes.end(), tmActionCmp());

	for (unsigned int i = 0; i < syntaxes.size(); ++i) {
        BundleMenuItem *item = new BundleMenuItem(m_syntaxMenu, 1000+i, *syntaxes[i], wxITEM_CHECK);
		m_syntaxMenu->Append(item);
        item->AfterInsert();
	}
}

void EditorFrame::CreateEncodingMenu(wxMenu& menu) const {
	// "Line endings" submenu (in "Save format" menu)
	wxMenu* lineendMenu = new wxMenu;
	lineendMenu->Append(MENU_EOL_DOS, _("&DOS/Windows (CR LF)"), _("DOS/Windows (CR LF)"), wxITEM_CHECK);
	lineendMenu->Append(MENU_EOL_UNIX, _("&Unix (LF)"), _("Unix (LF)"), wxITEM_CHECK);
	lineendMenu->Append(MENU_EOL_MAC, _("&Mac (CR)"), _("Mac (CR)"), wxITEM_CHECK);
	lineendMenu->AppendSeparator();
	lineendMenu->Append(MENU_EOL_NATIVE, _("&Always save native"), _("Always save native"), wxITEM_CHECK);

	// Encoding (in "Save format" menu) & Import submenu
	wxMenu* encodingMenu = new wxMenu;
	const size_t enc_count = wxFontMapper::GetSupportedEncodingsCount();
	for (size_t enc_id = 0; enc_id < enc_count; ++enc_id) {
		const wxFontEncoding enc = wxFontMapper::GetEncoding(enc_id);
		wxString enc_name = wxFontMapper::GetEncodingDescription(enc);
		if (enc == wxFONTENCODING_DEFAULT)
			enc_name += wxT(" (") + wxFontMapper::GetEncodingName(wxLocale::GetSystemEncoding()).Lower() + wxT(")");

		encodingMenu->Append(2000+enc_id, enc_name, enc_name, wxITEM_CHECK);
	}

	// Build the menu
	menu.Append(MENU_LINEEND, _("&Line Endings"), lineendMenu, _("Line Endings"));
	menu.Append(MENU_ENCODING, _("&Encoding"), encodingMenu, _("Encoding"));
	menu.Append(MENU_BOM, _("&Byte Order Marker (BOM)"), _("Byte Order Marker (BOM)"), wxITEM_CHECK);
}

void EditorFrame::UpdateEncodingMenu(wxMenu& menu) const {
	// Set Line endings
	wxTextFileType eol = editorCtrl->GetEOL();
	if (eol == wxTextFileType_None) eol = wxTextBuffer::typeDefault;
	menu.FindItem(MENU_EOL_DOS)->Check(eol == wxTextFileType_Dos);
	menu.FindItem(MENU_EOL_UNIX)->Check(eol == wxTextFileType_Unix);
	menu.FindItem(MENU_EOL_MAC)->Check(eol == wxTextFileType_Mac);
	bool forceNative = false; // default value
	m_settings.GetSettingBool(wxT("force_native_eol"), forceNative);
	menu.FindItem(MENU_EOL_NATIVE)->Check(forceNative);
	menu.FindItem(MENU_EOL_DOS)->Enable(!forceNative);
	menu.FindItem(MENU_EOL_UNIX)->Enable(!forceNative);
	menu.FindItem(MENU_EOL_MAC)->Enable(!forceNative);

	// Set the current encoding
	wxMenuItem* encodingItem = menu.FindItem(MENU_ENCODING); // "Encoding submenu item"
	if (!encodingItem) return;

	wxMenu* encodingMenu = encodingItem->GetSubMenu();
	if (!encodingMenu) return;

	wxFontEncoding enc = editorCtrl->GetEncoding();

	// Set checkmarks
	wxMenuItemList& items = encodingMenu->GetMenuItems();
	for(unsigned int i = 0; i < items.GetCount(); ++i)
		items[i]->Check(wxFontMapper::GetEncoding(i) == enc);

	// Check if we can set BOM
	wxMenuItem* bomItem = menu.FindItem(MENU_BOM); // "Byte-Order-Marker item"
	if (!bomItem) return;

	const bool encodingAllowsBOM = (enc == wxFONTENCODING_UTF7 || 
		enc == wxFONTENCODING_UTF8 || enc == wxFONTENCODING_UTF16LE ||
		enc == wxFONTENCODING_UTF16BE || enc == wxFONTENCODING_UTF32LE || enc == wxFONTENCODING_UTF32BE);

	bomItem->Enable(encodingAllowsBOM);
	bomItem->Check(editorCtrl->GetBOM());
}

void EditorFrame::CreateAndSetStatusbar() {
	m_pStatBar = new StatusBar(*this, wxID_ANY, &m_syntax_handler);

	SetStatusBar(m_pStatBar);
	SetStatusBarPane(-1); // disable help display
}

void EditorFrame::ShowOutput(const wxString& title, const wxString& output) {
	wxAuiPaneInfo& outPane = m_frameManager.GetPane(m_outputPane);

	outPane.Caption(title);
	m_outputPane->SetPage(output);
	outPane.Show();
	this->GetMenuBar()->Check(MENU_COMMANDOUTPUT, true);

	m_frameManager.Update();
}

void EditorFrame::CheckForModifiedFilesAsync() {
	if (m_changeCheckerThread) return; // check in progress

	// On network drives, looking at files can lock up for a long
	// time. So we do it in a separate thread.
	vector<ChangeCheckerThread::ChangePath> changeList;

	// Build a list of paths and dates in current documents
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		//TODO: DiffPanel has two editors (GetEditorCtrlFromPage only get active)
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		
		const wxString& mirrorPath = page->GetPath();
		if (mirrorPath.empty()) continue;

		// Get mirror info
		doc_id di;
		wxDateTime mDate;
		cxLOCK_READ(m_catalyst)
			if (!catalyst.GetFileMirror(mirrorPath, di, mDate)) continue;
		cxENDLOCK

		// Check if this change have been marked to be skipped
		wxDateTime skipDate = page->GetModSkipDate();

		// Bundle items are checked for changes straight away
		// (but results will come back with rest)
		bool isModified = false;
		if (page->IsBundleItem()) {
			// Get the bundle modDate
			BundleItemType bundleType;
			unsigned int bundleId;
			unsigned int itemId;
			const PListHandler& plistHandler = m_syntax_handler.GetPListHandler();
			if (!plistHandler.GetBundleItemFromUri(mirrorPath, bundleType, bundleId, itemId)) continue;
			const PListDict itemDict = plistHandler.Get(bundleType, bundleId, itemId);
			const wxDateTime bundleModDate = itemDict.GetModDate();
			
			// Only added if modified
			if (mDate == bundleModDate || (skipDate.IsValid() && bundleModDate == skipDate)) continue;

			skipDate = bundleModDate;
			isModified = true;
		}

		// Add to change list
		ChangeCheckerThread::ChangePath path;
		path.path = mirrorPath; // may be remote
		path.remoteProfile = page->GetRemoteProfile();
		path.date = mDate;
		path.skipDate = skipDate;
		path.isModified = isModified;
		changeList.push_back(path);
	}

	// Start separate thread checking for modified files
	if (!changeList.empty())
		new ChangeCheckerThread(changeList, *this, GetRemoteThread(), m_changeCheckerThread);
}

void EditorFrame::OnFilesChanged(wxFilesChangedEvent& event) {
	wxLogDebug(wxT("OnFilesChanged"));
	if (wxPendingDelete.Member(this)) return; // no need to worry about changes if we are closing
	if (m_inAskReload) return; // nested calls

	const wxArrayString& paths = event.GetChangedFiles();
	const vector<wxDateTime> modDates = event.GetModDates();
	vector<unsigned int> pathsToPages;
	vector<wxDateTime> pageDates;

	// Match paths with documents
	const unsigned int count = paths.GetCount();
	for (unsigned int i = 0; i < count; ++i) {
		const wxString& path = paths[i];

		// Find doc with current path
		const unsigned int pageCount = m_tabBar->GetPageCount();
		for (unsigned int p = 0; p < pageCount; ++p) {
			const EditorCtrl* page = GetEditorCtrlFromPage(p);
			const wxString filePath = page->GetPath();
			if (path == filePath) {
				pathsToPages.push_back(p);
				pageDates.push_back(modDates[i]);
				break;
			}
		}
	}

	AskToReloadMulti(pathsToPages, pageDates);

	wxLogDebug(wxT("OnFilesChanged done"));
}

/*
void EditorFrame::CheckForModifiedFiles() {
	// Check if we have any documents with modified files
	vector<unsigned int> paths_to_pages;
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = (EditorCtrl*)m_tabBar->GetPage(i);
		if (page->IsRemote()) continue;

		const wxString& mirrorPath = page->GetPath();
		if (mirrorPath.empty()) continue;

		// Get mirror info
		doc_id di;
		wxDateTime mDate;
		cxLOCK_READ(m_catalyst)
			if (!catalyst.GetFileMirror(mirrorPath, di, mDate)) continue;
		cxENDLOCK

		// Check if file exists
		const wxFileName fn = page->GetFilePath();
		if (!fn.FileExists()) continue;

		wxDateTime modDate = fn.GetModificationTime();
		if (!modDate.IsValid()) continue; // file could be locked by another app

		// Check if this change have been marked to be skipped
		const wxDateTime& skipDate = page->GetModSkipDate();
		if (skipDate.IsValid() && modDate == skipDate) continue;

		bool isChanged = false;
		if (!mDate.IsValid()) isChanged = true;
		else {
			// Windows does not really handle the minor parts of file dates
			mDate.SetMillisecond(0);
			modDate.SetMillisecond(0);

			if (mDate != modDate) isChanged = true;
		}

		if (isChanged) {
			wxLogDebug(wxT("Modified file detected:"));
			wxLogDebug(wxT("  %s"), mirrorPath);
			if (mDate.IsValid()) wxLogDebug(wxT("  mirror: %s disk: %s"), mDate.FormatTime(), modDate.FormatTime());
			else wxLogDebug(wxT("  mirror: - disk: %s"), modDate.FormatTime());

			paths_to_pages.push_back(i);
		}
	}

	AskToReloadMulti(paths_to_pages);
}*/

void EditorFrame::AskToReloadMulti(const vector<unsigned int>& pathToPages, const vector<wxDateTime>& modDates) {
	if (m_inAskReload) return; // nested-calls
	if (pathToPages.empty()) return;

	// When dialog is closed we will get a second activate event
	// this var make us aware of this so we can avoid asking twice
	m_inAskReload = true;

	// Build list of paths (some may be remote)
	wxArrayString paths;
	for (unsigned int p = 0; p < pathToPages.size(); ++p) {
		EditorCtrl* ec = GetEditorCtrlFromPage(pathToPages[p]);
		paths.Add(ec->GetPath());
	}

	// Ask the user if he wants to reload them
	ReloadDlg reloaddlg(this, paths);
	const int result = reloaddlg.ShowModal();

	if (result == wxID_YES) {
		for (unsigned int i = 0; i < pathToPages.size(); ++i) {
			EditorCtrl* ec = GetEditorCtrlFromPage(pathToPages[i]);
			if (!ec) {
				wxFAIL_MSG(wxT("invalid page"));
				continue;
			}

			const wxString& path = paths[i];

			if (reloaddlg.IsChecked(i)) {
				// Get editor document & vertical scroll positions
				const int pos = ec->GetPos();
				const int topline = ec->GetTopLine();

				const cxFileResult fileResult = ec->LoadText(path, ec->GetRemoteProfile());

				if (fileResult == cxFILE_OK) {
					ec->SetDocumentAndScrollPosition(pos, topline);
					if (ec == editorCtrl) editorCtrl->ReDraw();
				}
				else {
					if (fileResult == cxFILE_DOWNLOAD_ERROR) {} // do nothing, download code has reported error
					else if (fileResult == cxFILE_CONV_ERROR)
						wxMessageBox(_T("Could not read file: ") + path + _T("\nTry importing with another encoding"), _T("e Error"), wxICON_ERROR);
					else
						wxMessageBox(_T("Could not open file: ") + path, _T("e Error"), wxICON_ERROR);

					// Set status to modified
					cxLOCK_WRITE(m_catalyst)
						catalyst.SetFileMirrorToModified(path, ec->GetDocID());
					cxENDLOCK
				}
			}
			else {
				cxLOCK_WRITE(m_catalyst)
					catalyst.SetFileMirrorToModified(path, ec->GetDocID());
				cxENDLOCK

				// Mark editorCtrl to skip reloading of this change
				ec->SetModSkipDate(modDates[i]);
			}

			UpdateWindowTitle(); // Update tabs and title
		}
	}
	else {
		// Mark all editorCtrls to skip reloading of this change
		for (unsigned int i = 0; i < pathToPages.size(); ++i) {
			EditorCtrl* ec = GetEditorCtrlFromPage(pathToPages[i]);
			if (!ec) {
				wxFAIL_MSG(wxT("invalid page"));
				continue;
			}

			ec->SetModSkipDate(modDates[i]);
		}
	}

	m_inAskReload = false;
}

void EditorFrame::RemoveRegMenus() {
	int menuid = GetMenuBar()->FindMenu(_("Help"));
	wxASSERT(menuid != wxNOT_FOUND);
	wxMenu* helpmenu = GetMenuBar()->GetMenu(menuid);
	helpmenu->Delete(helpmenu->FindItemByPosition(2)); // separator
	helpmenu->Delete(MENU_BUY); // Buy
	helpmenu->Delete(MENU_REGISTER); // Register
}

void EditorFrame::OpenDocument(const doc_id& di) {
	wxASSERT(editorCtrl);

	if (editorCtrl->IsEmpty()) editorCtrl->SetDocument(di); // Reuse the current editorCtrl if possible
	else {
		EditorCtrl* ec = new EditorCtrl(di, wxT(""), m_catalyst, bitmap, m_tabBar, *this);
		AddTab(ec);
	}

	editorCtrl->SetFocus();
	UpdateWindowTitle();
	editorCtrl->ReDraw();
}

void EditorFrame::AddTab(wxWindow* page) {
	wxASSERT(m_tabBar);
	Freeze();
	
	// Default is just creating a new editorCtrl
	EditorCtrl* ec = NULL;
	if (page == NULL) {
		page = ec = new EditorCtrl(m_catalyst, bitmap, m_tabBar, *this);
		if (!ec || !ec->IsOk()) {
			Thaw();

			// Clean up
			delete ec;

			// Notify the user of the error
			wxMessageDialog dlg(this, _("Not enough memory to create new page"), wxT("e"), wxOK|wxICON_ERROR);
			dlg.Centre();
			dlg.ShowModal();
			return;
		}
	}

	// Get the actual editorCtrl (may be embedded in panel)
	if (!ec)
		ec = (dynamic_cast<ITabPage*>(page))->GetActiveEditor();

	wxString tabText = ec->GetName();
	if (tabText.empty()) tabText = _("Untitled");

	// Add the new tab
	wxString prevSyntax;
	if(editorCtrl) {
		// Hide the previous page
		//editorbox->Show(editorCtrl, false);

		// Get the syntax of the previous page
		prevSyntax = editorCtrl->GetSyntaxName();
	}
	// Set syntax
	const wxString syntax = ec->GetSyntaxName();
	if (syntax.empty()) {
		if (ec->IsEmpty() && !prevSyntax.empty()) ec->SetSyntax(prevSyntax);
		else ec->SetSyntax(wxT("Plain Text")); // default syntax
	}
	//if (editorCtrl) editorCtrl->EnableRedraw(false);
	
	// Get tab icon
	const char** iconxpm = (dynamic_cast<ITabPage*>(page))->RecommendedIcon();
	const wxBitmap tabIcon = wxBitmap(iconxpm);
	
	ec->EnableRedraw(true);
	editorCtrl = ec;
	m_tabBar->AddPage(page, tabText, true, tabIcon);
	UpdateWindowTitle();
	Thaw();

	// Notify that we are editing a new document
	dispatcher.Notify(wxT("WIN_CHANGEDOC"), editorCtrl, GetId());
	UpdateTabMenu();
}

ITabPage* EditorFrame::GetPage(size_t idx) {
	return dynamic_cast<ITabPage*>(m_tabBar->GetPage(idx));
}

void EditorFrame::UpdateWindowTitle() {
	if (!editorCtrl) return; // Can be called before editorCtrl is set

	const wxString name = editorCtrl->GetName();

	wxString filename;
	if (!name.empty()) filename = name;
	else filename = _("Untitled");

	const wxString path = editorCtrl->GetPath();
	wxString title = path.empty() ? filename : path;

	if (editorCtrl->IsModified()) {
#ifdef __WXMSW__
		wxString modifiedBug = wxT("\x2022 ");
		title = modifiedBug + title;
		filename = modifiedBug + filename;
#else
		wxString modifiedBug = wxT(" *");
		title += modifiedBug;
		filename += modifiedBug;
#endif
	}

	title += wxT( " - ");
	title += wxGetApp().GetAppTitle();

	SetTitle(title);

	const int ndx = m_tabBar->GetSelection();
	if (m_tabBar->GetPageText(ndx) != filename)
		m_tabBar->SetPageText(ndx, filename);
}

void EditorFrame::UpdateTabs() {
	for( size_t i = 0; i < m_tabBar->GetPageCount(); ++i)	{
		const EditorCtrl* page = GetEditorCtrlFromPage(i);
		
		const wxString name = page->GetName();
		wxString title;
		
		if (!name.empty()) title = name;
		else title = _("Untitled");
		
		if (editorCtrl->IsModified()) {
#ifdef __WXMSW__
			wxString modifiedBug = wxT("\x2022 ");
			title = modifiedBug + title;
#else
			wxString modifiedBug = wxT(" *");
			title += modifiedBug;
#endif
		}
		
		if (m_tabBar->GetPageText(i) != title) m_tabBar->SetPageText(i, title);	
	}
}

// May be NULL, always check in reciever
EditorCtrl* EditorFrame::GetEditorCtrl() { return editorCtrl; }
// May be NULL, always check in reciever. Downcast.
IEditorSearch* EditorFrame::GetSearch() { return editorCtrl; }

// This method returns true if the active editor is different from the one
// passed in, or if the active editor is the same but there has been an edit.
wxControl* EditorFrame::GetEditorAndChangeType(const EditorChangeState& lastChangeState, EditorChangeType& newStatus) {
	if (editorCtrl == NULL) {
		newStatus = ECT_NO_EDITOR;
		return NULL;
	}

	EditorChangeState currentState = editorCtrl->GetChangeState();
	if (currentState.id != lastChangeState.id)
		newStatus = ECT_NEW_EDITOR;
	else if (currentState.changeToken != lastChangeState.changeToken)
		newStatus = ECT_EDITOR_CHANGED;
	else
		newStatus = ECT_NO_CHANGE;

	return editorCtrl;
}

void EditorFrame::FocusEditor() {
	if (editorCtrl != NULL) editorCtrl->SetFocus();
}

EditorCtrl* EditorFrame::GetEditorCtrlFromPage(size_t page_idx) {
	wxWindow* page = m_tabBar->GetPage(page_idx);
	if (!page) return NULL;

	return (dynamic_cast<ITabPage*>(page))->GetActiveEditor();
}

void EditorFrame::BringToFront() {
	if (!IsShown()) return;

	// Make sure the frame is not iconized
	if (IsIconized()) Iconize(false);

#ifdef __WXMSW__
	if(::GetForegroundWindow() == GetHwnd()) return;
	else {
		const DWORD foregroundThreadID = ::GetWindowThreadProcessId(::GetForegroundWindow(), NULL);
		const DWORD appThreadID = ::GetWindowThreadProcessId(GetHwnd(), NULL);

		// Windows 98/2000 doesn't want to foreground a window when some other
		// window has keyboard focus
		if (foregroundThreadID != appThreadID) {
			::AttachThreadInput(foregroundThreadID, appThreadID, true);
			::BringWindowToTop(GetHwnd());
			::SetForegroundWindow(GetHwnd());
			::AttachThreadInput(foregroundThreadID, appThreadID, false);
		}
		else {
			::BringWindowToTop(GetHwnd());
			::SetForegroundWindow(GetHwnd());
		}
	}
#else
	Raise();
#endif //__WXMSW__
}

void EditorFrame::GotoPos(int line, int column) {
	wxASSERT(editorCtrl);

	editorCtrl->SetPos(line, column);
	editorCtrl->MakeCaretVisibleCenter();
	editorCtrl->ReDraw();
	editorCtrl->SetFocus();
}

bool EditorFrame::HasProject() const {
	wxASSERT(m_projectPane);
	return m_projectPane->IsShown() && m_projectPane->HasProject();
}

bool EditorFrame::CloseProject() {
	m_projectPane->Clear();
	m_settings.RemoveSetting(wxT("project"));
	return true;
}

bool EditorFrame::IsProjectRemote() const {
	return m_projectPane->IsRemote();
}

const wxFileName& EditorFrame::GetRootPath() const {
	wxASSERT(m_projectPane);
	return m_projectPane->GetRootPath();
}

wxArrayString EditorFrame::GetSelectionsInProject() const {
	wxASSERT(m_projectPane);
	return m_projectPane->GetSelections();
}

const map<wxString,wxString>& EditorFrame::GetProjectEnv() const {
	wxASSERT(m_projectPane);
	return m_projectPane->GetEnv();
}

bool EditorFrame::OpenTxmtUrl(const wxString& url) {
	if (!url.StartsWith(wxT("txmt:"))) return false;

	// get file, line & column
	static wxRegEx fileRx(wxT("[&?]url=file://([^&]+)"));
	static wxRegEx bundleRx(wxT("[&?]url=(bundle://[^&]+)"));
	static wxRegEx lineRx(wxT("[&?]line=([[:digit:]]+)"));
	static wxRegEx columnRx(wxT("[&?]column=([[:digit:]]+)"));
	static wxRegEx selRx(wxT("[&?]sel=([[:digit:]]+)"));

	wxString file;
	int line = -1;
	int column = -1;
	int sel = 0;
	bool isBundleItem = false;

	if (fileRx.Matches(url)) file = fileRx.GetMatch(url, 1);
	else if (bundleRx.Matches(url)) {
		file = bundleRx.GetMatch(url, 1);
		isBundleItem = true;
	}
	if (lineRx.Matches(url)) {
		long ref;
		if (lineRx.GetMatch(url, 1).ToLong(&ref)) line = ref;
	}
	if (columnRx.Matches(url)) {
		long ref;
		if (columnRx.GetMatch(url, 1).ToLong(&ref)) column = ref;
	}
	if (selRx.Matches(url)) {
		long ref;
		if (selRx.GetMatch(url, 1).ToLong(&ref)) sel = ref;
	}

	if (!file.empty()) {
		file = URLDecode(file);

#ifdef __WXMSW__
		// path may be in unix format, so we have to convert it
		if (!isBundleItem) file = eDocumentPath::CygwinPathToWin(file);
#endif // __WXMSW__

		if (isBundleItem) {
			if (!OpenRemoteFile(file)) 
				return false;
		}
		else if (wxDir::Exists(file)) {
			wxFileName dirPath(file, wxEmptyString);
			dirPath.MakeAbsolute();
			return OpenDirProject(dirPath);
		}
		else {
			wxFileName path(file);
			path.MakeAbsolute();
			if (!OpenFile(path)) 
				return false;
		}
	}

	// Goto position
	if (line != -1 || column != -1) GotoPos(line, column);
	
	// Select the number of chars specified in sel
	if (sel > 0) {
		const unsigned int pos = editorCtrl->GetPos();
		editorCtrl->Select(pos, pos+sel);
		editorCtrl->ReDraw(); // show selection
	}

	return true;
}

void EditorFrame::ReopenFiles(wxArrayString& files, unsigned long firstLine, unsigned long firstColumn, wxString& mate) {
	for (unsigned int i = 0; i < files.GetCount(); ++i) {
		const wxString& arg = files[i];
		Open(arg, mate);

		if (i == 0) // The line/column position apply to the first file only.
			if (firstLine || firstColumn) GotoPos(firstLine, firstColumn);
	}
}

bool EditorFrame::Open(const wxString& path, const wxString& mate) {
	// Handle paths in txmt format
	if (path.StartsWith(wxT("txmt:")) )
		return OpenTxmtUrl(path);

	// Handle remote paths
	if (eDocumentPath::IsRemotePath(path)) {
		wxRegEx domain(wxT("^.*://[^/]+$")); // domains don't need ending slash

		if (path.Last() == wxT('/')) 
			return OpenRemoteProjectFromUrl(path);

		if (domain.Matches(path)) 
			return OpenRemoteProjectFromUrl(path + wxT('/'));

		if (!OpenRemoteFile(path))
			return false;

		Update();
		return true;
	}

	if (eDocumentPath::IsBundlePath(path)) 
		return OpenRemoteFile(path);

	if (wxDir::Exists(path)) {
		wxFileName dirPath(path, wxEmptyString);
		dirPath.MakeAbsolute(); // Handle local paths
		return OpenDirProject(dirPath);
	}

	// Handle local paths
	wxFileName filepath = path;
	filepath.MakeAbsolute();

	if (!OpenFile(filepath, wxFONTENCODING_SYSTEM, mate)) return false;
	Update();
	return true;
}

bool EditorFrame::OpenProject(const wxString& prj) {
	if (prj == wxT("cx:bundles")) {
		ShowBundlePane();
		return true;
	}
	
	if (eDocumentPath::IsRemotePath(prj)) return OpenRemoteProjectFromUrl(prj);

	const wxFileName path = prj;
	return OpenDirProject(path);
}

bool EditorFrame::OpenDirProject(const wxFileName& path) {
	if (!path.IsOk() || !path.IsDir() || !path.DirExists()) return false;

	if (!m_projectPane->SetProject(path)) return false;

	ShowProjectPane(path.GetFullPath());
	return true;
}

bool EditorFrame::OpenRemoteProjectFromUrl(const wxString& url) {
	// Get (or create) matching profile
	const RemoteProfile* rp = GetRemoteProfile(url, true);

	if (rp && rp->IsValid()) return OpenRemoteProject(rp);
	return false;
}

bool EditorFrame::OpenRemoteProject(const RemoteProfile* rp) {
	wxASSERT(rp && rp->IsValid());

	m_projectPane->SetRemoteProject(rp);
	ShowProjectPane(rp->GetUrl());
	return true;
}

void EditorFrame::ShowProjectPane(const wxString& project) {
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));
	projectPane.Caption(_("Project"));

	// Pane may be in bundle mode
	if (projectPane.window != m_projectPane) {
		projectPane.Window(m_projectPane);
		if (m_bundlePane) m_bundlePane->Hide();
		if (m_diffPane) m_diffPane->Hide();
	}
	projectPane.Show();
	m_frameManager.Update();

	if (!project.empty()) {
		m_settings.SetSettingBool(wxT("showproject"), true);
		m_settings.SetSettingString(wxT("project"), project);
		m_generalSettings.AddRecentProject(project);
		UpdateRecentFiles();
	}

	// Bring frame to front
	BringToFront();
}

void EditorFrame::ShowBundlePane() {
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));
	projectPane.Caption(_("Bundles"));

	const bool showPane = !projectPane.IsShown() || projectPane.window != m_bundlePane;

	if (showPane) {
		if (!m_bundlePane) m_bundlePane = new BundlePane(*this, &m_syntax_handler);

		if (projectPane.window != m_bundlePane) {
			projectPane.Window(m_bundlePane);
			if (m_projectPane) m_projectPane->Hide();
			if (m_diffPane) m_diffPane->Hide();
		}

		projectPane.Show();
		m_frameManager.Update();
	} 
	else {
		// WORKAROUND: Aui does not save size between hide/show
		projectPane.BestSize(projectPane.window->GetSize());
		projectPane.Hide();
		m_frameManager.Update();
		editorCtrl->SetFocus();
	}

	m_settings.SetSettingBool(wxT("showproject"), showPane);
	if (showPane) m_settings.SetSettingString(wxT("project"), wxT("cx:bundles"));
}

void EditorFrame::ShowDiffPane(const wxString& path1, const wxString& path2) {
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));
	projectPane.Caption(_("Compare"));

	if (!m_diffPane) m_diffPane = new DiffDirPane(*this);

	if (projectPane.window != m_diffPane) {
		projectPane.Window(m_diffPane);

		if (m_projectPane) m_projectPane->Hide();
		if (m_bundlePane) m_bundlePane->Hide();
	}

	m_diffPane->SetDiff(path1, path2);

	projectPane.Show();
	m_frameManager.Update();
}


void EditorFrame::OnMenuShowProject(wxCommandEvent& WXUNUSED(event)) {
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));
	projectPane.Caption(_("Project"));

	const bool showPane = !projectPane.IsShown() || projectPane.window != m_projectPane;

	if (showPane) {
		// Pane may be in bundle mode
		if (projectPane.window != m_projectPane) {
			projectPane.Window(m_projectPane);
			if (m_bundlePane) m_bundlePane->Hide();
		}

		projectPane.Show();
		m_frameManager.Update();
	}
	else {
		// WORKAROUND: Aui does not save size between hide/show
		projectPane.BestSize(projectPane.window->GetSize());
		projectPane.Hide();
		m_frameManager.Update();
		editorCtrl->SetFocus();
	}

	m_settings.SetSettingBool(wxT("showproject"), showPane);
	if (showPane) m_settings.SetSettingString(wxT("project"), m_projectPane->GetProjectString());
}

void EditorFrame::OnShiftProjectFocus(wxCommandEvent& WXUNUSED(event)) {
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));

	if (!projectPane.IsShown()) {
		projectPane.Show();
		m_frameManager.Update();
		projectPane.window->SetFocus();
	}
	else {
		// switch focus
		if (FindFocus() == editorCtrl)
			projectPane.window->SetFocus();
		else
			editorCtrl->SetFocus();
	}
}

bool EditorFrame::OpenFile(const wxFileName& path, wxFontEncoding enc, const wxString& mate) {
	wxASSERT(path.IsAbsolute());

	// Verify that the path is valid
	if (!path.IsOk()) {
		wxMessageBox(path.GetFullPath() + _T(" is not a file!"), _T("e Error"), wxICON_ERROR);
		return false;
	}

	// We might be passed a dir by accident
	if (path.IsDir() || wxDir::Exists(path.GetFullPath())) {
		wxMessageBox(path.GetFullPath() + _T(" is a directory, not a file!"), _T("e Error"), wxICON_ERROR);
		return false;
	}

	const wxString filepath = path.GetFullPath();
	return DoOpenFile(filepath, enc, NULL, mate);
}

void EditorFrame::UpdateRenamedFile(const wxFileName& path, const wxFileName& newPath) {
	// Check if there is a mirror that matches the file
	const wxString filepath = path.GetFullPath();

	doc_id di;
	wxDateTime modifiedDate;
	bool isMirror;
	cxLOCK_READ(m_catalyst)
		isMirror = catalyst.GetFileMirror(filepath, di, modifiedDate);
	cxENDLOCK

	if(!isMirror) return;

	unsigned int i;
	EditorCtrl* existingControl = this->GetEditorCtrlFromFile(filepath, i);
	if (!existingControl) return;

	cxLOCK_WRITE(m_catalyst)
		catalyst.SetFileMirror(newPath.GetFullPath(), di, modifiedDate);
	cxENDLOCK

	cxLOCKDOC_WRITE(existingControl->GetDocument())
		doc.SetPropertyName(wxFileName(newPath.GetLongPath()).GetFullName());
	cxENDLOCK

	m_tabBar->SetSelection(i);
	existingControl->SetPath(newPath.GetFullPath());
	UpdateWindowTitle();
	existingControl->ReDraw();

	// Bring frame to front
	BringToFront();
	existingControl->SetFocus();
}

bool EditorFrame::OpenRemoteFile(const wxString& url, const RemoteProfile* rp) {
	return DoOpenFile(url, wxFONTENCODING_SYSTEM, rp);
}

wxString EditorFrame::DownloadFile(const wxString& url, const RemoteProfile* rp) {
	wxASSERT(rp && rp->IsValid());

	const wxString buffPath = GetAppPaths().CreateTempAppDataFile();

	while(1) {
		wxBeginBusyCursor();
		const CURLcode res = GetRemoteThread().Download(url, buffPath, *rp);
		wxEndBusyCursor();

		if (res == CURLE_OK)
			break; // download succeded

		if (res == CURLE_LOGIN_DENIED) {
			if (!AskRemoteLogin(rp))
				return wxEmptyString;
		}
		else {
			wxString msg = _T("Could not download file: ") + url;
			msg += wxT("\n") + GetRemoteThread().GetLastError();
			wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
			return wxEmptyString;
		}
	}

	return buffPath;
}

bool EditorFrame::UploadFile(const wxString& url, const wxString& buffPath, const RemoteProfile* rp) {
	while(1) {
		wxBeginBusyCursor();
		const CURLcode res =  this->GetRemoteThread().UploadAndDate(url, buffPath, *rp);
		wxEndBusyCursor();

		if (res == CURLE_OK)
			break; // upload succeded

		if (res == CURLE_LOGIN_DENIED) {
			if (!this->AskRemoteLogin(rp))
				return false;
		}
		else {
			wxString msg = _T("Could not upload file: ") + url;
			msg += wxT("\n") + this->GetRemoteThread().GetLastError();
			wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
			return false;
		}
	}

	return true;
}

wxDateTime EditorFrame::GetRemoteDate(const wxString& url, const RemoteProfile* rp) {
	wxASSERT(rp && rp->IsValid());
	return GetRemoteThread().GetModDate(url, *rp);
}

// Asks our settings to get or create a matching remote profile.
const RemoteProfile* EditorFrame::GetRemoteProfile(const wxString& url, bool withDir) {
	wxASSERT(eDocumentPath::IsRemotePath(url));
	return m_generalSettings.GetRemoteProfileFromUrl(url, withDir);
}

bool EditorFrame::AskRemoteLogin(const RemoteProfile* rp) {
	RemoteLoginDlg dlg(this, rp->m_username, rp->m_address, rp->IsTemp());
	if (dlg.ShowModal() != wxID_OK)
		return false;

	// This also updates rp with the new login
	m_generalSettings.SetRemoteProfileLogin(rp, dlg.GetUsername(), dlg.GetPassword(), dlg.GetSaveProfile());
	return true;
}

EditorCtrl* EditorFrame::GetEditorCtrlFromFile(const wxString& filepath, unsigned int& page_idx) {
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* editorCtrl = GetEditorCtrlFromPage(i);

		// Paths on windows are case-insensitive
#ifdef __WXMSW__
		if (filepath.CmpNoCase(editorCtrl->GetPath()) == 0) { 
#else
		if (filepath == editorCtrl->GetPath()) {
#endif
			page_idx = i;
			return editorCtrl;
		}
	}

	return NULL;
}

DiffPanel* EditorFrame::GetDiffPaneFromFiles(const wxString& path1, const wxString& path2, unsigned int& page_idx) const {
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		wxWindow* page = m_tabBar->GetPage(i);
		if (!page->IsKindOf(CLASSINFO(DiffPanel))) continue;
		
		DiffPanel* diffPage = (DiffPanel*)page;

		if (diffPage->CmpPaths(path1, path2)) {
			page_idx = i;
			return diffPage;
		}
	}

	return NULL;
}

bool EditorFrame::DoOpenFile(const wxString& filepath, wxFontEncoding enc, const RemoteProfile* rp, const wxString& mate) {
	wxBusyCursor busy;

	bool isCurrent = false;
	const bool isBundleItem = eDocumentPath::IsBundlePath(filepath);

	if (!editorCtrl) {
		// This should never happen, but there have been
		// error reports about it so we recover if it happens
		wxFAIL_MSG(wxT("No editorCtrl in OpenFile"));
		AddTab();
	}
	EditorCtrl* ec = this->editorCtrl;

	// Check if there is a mirror that matches the file
	doc_id di;
	wxDateTime modifiedDate;
	bool isMirror;
	cxLOCK_READ(m_catalyst)
		isMirror = catalyst.GetFileMirror(filepath, di, modifiedDate);
	cxENDLOCK

	if(isMirror) {
		unsigned int i;
		EditorCtrl* existingControl = this->GetEditorCtrlFromFile(filepath, i);
		if (existingControl) {
			m_tabBar->SetSelection(i);
			ec = existingControl;
			isCurrent = true;
		}
	}

	// Reuse the current editorCtrl if possible
	wxWindow* page = ec;
	if (!isCurrent) {
		if (isBundleItem) {
			EditorBundlePanel* bundlePanel = new EditorBundlePanel(m_tabBar, *this, m_catalyst, bitmap);
			page = bundlePanel;
			ec = bundlePanel->GetActiveEditor();
		}
		else if (!editorCtrl->IsEmpty()) 
			page = ec = new EditorCtrl(m_catalyst, bitmap, m_tabBar, *this);
	}

	// Load the text (or update with changes)
	const cxFileResult result = ec->OpenFile(filepath, enc, rp, mate);
	if (result != cxFILE_OK) {
		if (result == cxFILE_DOWNLOAD_ERROR) {} // do nothing, download code has reported error
		else if (result == cxFILE_CONV_ERROR) {
			const wxString msg = _T("Could not read file: ") + filepath + _T("\nTry importing with another encoding");
			wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
		}
		else {
			wxString msg = _T("Could not open file: ") + filepath;
			wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
		}

		if (ec != editorCtrl) 
			delete page; // clean up
		return false;
	}

	// Add to recent files list
	m_generalSettings.AddRecentFile(filepath);
	UpdateRecentFiles();

	// Create and draw the new page
	if (ec != editorCtrl) {
		// Bundle items do not reuse empty editorCtrls
		if (isBundleItem && editorCtrl->IsEmpty()) DeletePage(m_tabBar->GetSelection());
		
		AddTab(page);
	}
	UpdateWindowTitle();
	editorCtrl->ReDraw();

	// Bring frame to front
	BringToFront();
	page->SetFocus();

	return true;
}

// When multiple documents are unsaved, ask the user which ones to save,
// and allow the user to cancel the operation.
//
// Returns: false if the operation was cancelled, else true
bool EditorFrame::AskToSaveMulti(int keep_tab) {
	wxASSERT(keep_tab == -1 || (keep_tab >= 0 && (unsigned int)keep_tab < m_tabBar->GetPageCount()));

	// Check if we have any unsaved documents
	wxArrayString paths;
	vector<int> paths_to_pages;
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);

		if (page->IsModified() && (keep_tab == -1 || i != (unsigned int)keep_tab)) {
			wxString path = page->GetPath();
			if (path.empty()) path = page->GetName();
			if (path.empty()) path = _("Untitled");

			paths.Add(path);
			paths_to_pages.push_back(i);
		}
	}

	if (paths.IsEmpty())
		return true;

	// Prompt the user to save some documents or cancel the operation.
	SaveDlg savedlg(this, paths);
	int result = savedlg.ShowModal();

	// User chose to cancel operation
	if (result == wxID_CANCEL)
		return false;

	// User chose not to save unsaved documents.
	if (result == wxID_NO)
		return true;
	
	// Otherwise, result == wxID_YES
	wxASSERT(result == wxID_YES);

	for (int i=0; i < savedlg.GetCount(); ++i) {
		if (!savedlg.IsChecked(i)) continue;

		m_tabBar->SetSelection(paths_to_pages[i]);

		EditorCtrl* page = GetEditorCtrlFromPage(paths_to_pages[i]);
		if (!page->SaveText())
			return false; // Cancel if save failed
	}

	return true;
}

void EditorFrame::SaveAllFilesInProject() {
	if (!HasProject()) return;

	const wxString projectPath = GetRootPath().GetFullPath();

	// Save all files that are modified and in current project
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);

		if (page->IsModified()) {
			wxString path = page->GetPath();
			if (path.empty()) continue;

			if (!path.StartsWith(projectPath)) continue;
			page->SaveText();
		}
	}

	UpdateTabs();
}

void EditorFrame::ShowSearch(bool show, bool replace) {
	if (show) {
		m_searchPanel->InitSearch(editorCtrl->GetSelFirstLine(), replace);
		box->Show(m_searchPanel);
	}
	else {
		box->Show(m_searchPanel, false);
		editorCtrl->SetFocus();
	}
	box->Layout();
}

bool EditorFrame::IsSearching() const { return m_searchPanel->IsShown(); }

bool EditorFrame::GetSetting(const wxString& name) const {
	if (name == wxT("search/highlight")) return m_searchHighlight;
	if (name == wxT("editor/linenumbers")) return m_showGutter;

	wxASSERT(false);
	return false; // We should never reach here
}

void EditorFrame::SetSetting(const wxString& name, bool value) {
	if (name == wxT("search/highlight")) m_searchHighlight = value;
	else wxASSERT(false);
}

void EditorFrame::SetSoftTab(bool isSoft)  {
	if (isSoft == m_softTabs) return;

	// Save setting
	m_settings.SetSettingBool(wxT("softtabs"), isSoft);
	m_softTabs = isSoft;

	// update all editor pages
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		page->SetTabWidth(m_tabWidth, m_softTabs);
	}
}

void EditorFrame::SetTabWidth(unsigned int width) {
	wxASSERT(width > 0);
	if ((int)width == m_tabWidth) return;

	// Save setting
	m_settings.SetSettingInt(wxT("tabwidth"), width);
	m_tabWidth = width;

	// Invalidate all editor pages
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		page->SetTabWidth(m_tabWidth, m_softTabs);
	}

	// Redraw current
	editorCtrl->ReDraw();
}

void EditorFrame::OnOpeningMenu(wxMenuEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl); // should not be possible, but seen in crash reports
	if (!editorCtrl) return;

	wxMenu& fileMenu = *GetMenuBar()->GetMenu(0);
	UpdateEncodingMenu(fileMenu);

	wxAuiPaneInfo& revHistoryPane = m_frameManager.GetPane(documentHistory);
	wxMenuItem* rhItem = GetMenuBar()->FindItem(MENU_REVHIS); // "Revision History"
	if (rhItem) rhItem->Check(revHistoryPane.IsShown());

	wxAuiPaneInfo& undoHistoryPane = m_frameManager.GetPane(undoHistory);
	wxMenuItem* uhItem = GetMenuBar()->FindItem(MENU_UNDOHIS); // "Undo History"
	if (uhItem) uhItem->Check(undoHistoryPane.IsShown());

	// Project & Bundle Pane
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));

	// Project Pane
	wxMenuItem* spItem = GetMenuBar()->FindItem(MENU_SHOWPROJECT);
	if (spItem) {
		const bool showingProjectPane = projectPane.IsShown() && projectPane.window == m_projectPane;
		spItem->Check(showingProjectPane);
	}

	// Bundle Pane
	wxMenuItem* bundleItem = GetMenuBar()->FindItem(MENU_EDIT_BUNDLES);
	if (spItem) {
		const bool showingBundlePane = projectPane.IsShown() && projectPane.window == m_bundlePane;
		bundleItem->Check(showingBundlePane);
	}

	// Find in Project
	wxMenuItem* fpItem = GetMenuBar()->FindItem(MENU_FIND_IN_PROJECT);
	if (fpItem) fpItem->Enable(m_projectPane->HasProject());

	// "Show Symbol List"
	wxMenuItem* slItem = GetMenuBar()->FindItem(MENU_SHOWSYMBOLS);
	if (slItem) slItem->Check(m_symbolList != NULL);

	// "Show Snippet List"
	wxMenuItem* snItem = GetMenuBar()->FindItem(MENU_SHOWSNIPPETS);
	if (snItem) snItem->Check(m_snippetList != NULL);

	// "Highlight Authors"
	wxMenuItem* hlItem = GetMenuBar()->FindItem(MENU_HL_USERS);
	if (hlItem) hlItem->Check(m_userHighlight);

	// Show Statusbar
	wxMenuItem* sbItem = GetMenuBar()->FindItem(MENU_STATUSBAR);
	if (sbItem) sbItem->Check(m_pStatBar != NULL);

	// Go to File
	wxMenuItem* gfItem = GetMenuBar()->FindItem(MENU_GOTO_FILE);
	if (gfItem) gfItem->Enable(m_projectPane->HasProject());

	// Set the selected syntax
	wxMenuItem* syntaxItem = GetMenuBar()->FindItem(MENU_SYNTAX); // "Syntax submenu item"
	if (syntaxItem) {
		wxMenu* syntaxMenu = syntaxItem->GetSubMenu();
		if (syntaxMenu) {
			const wxString& syntaxName = editorCtrl->GetSyntaxName();

			// Set checkmarks
			wxMenuItemList& items = syntaxMenu->GetMenuItems();
			for(unsigned int i = 0; i < items.GetCount(); ++i) {
				const bool found_syntax_menu = items[i]->GetLabel() == syntaxName;
				items[i]->Check(found_syntax_menu);
			}
		}
	}

	// Set current tab
	const wxMenuItemList& tabItems = m_tabMenu->GetMenuItems();
	const unsigned int currentTab = m_tabBar->GetSelection();
	for (unsigned int i = 0; i < tabItems.GetCount(); ++i) {
		wxMenuItem* item = tabItems[i];
		item->Check(item->GetId() - 40000 == (int)currentTab);
	}

	// Web preview
	wxMenuItem* previewItem = GetMenuBar()->FindItem(MENU_PREVIEW);
	if (previewItem) previewItem->Check(m_previewDlg != NULL);

	// Save-As is not implemented for bundles items (yet)
	wxMenuItem* saveasItem = GetMenuBar()->FindItem(wxID_SAVEAS);
	saveasItem->Enable(!editorCtrl->IsBundleItem());

	// We handle key events on our own, so disable accels (from menus)
	//SetAcceleratorTable(wxNullAcceleratorTable);
}

void EditorFrame::OnMenuFindCommand(wxCommandEvent& WXUNUSED(event)) {
	if (editorCtrl) editorCtrl->DoActionFromDlg();
}

void EditorFrame::OnMenuReloadBundles(wxCommandEvent& WXUNUSED(event)) {
	wxBusyCursor wait;

	// Reload bundles (will send it's own event to reset bundle menu if needed)
	m_syntax_handler.LoadBundles(cxUPDATE);
	
	// If we have an active BundleEditor, it has to reload the new bundles
	if (m_bundlePane) m_bundlePane->LoadBundles();
}

void EditorFrame::OnMenuEditBundles(wxCommandEvent& WXUNUSED(event)) {
	ShowBundlePane();
}

void EditorFrame::OnMenuManageBundles(wxCommandEvent& WXUNUSED(event)) {
	ShowBundleManager();
}

void EditorFrame::ShowBundleManager() {
	BundleManager dlg(this, this->GetRemoteThread(), &m_syntax_handler);
	dlg.ShowModal();

	// If we have an active BundleEditor, it has to reload the new bundles
	if (dlg.NeedBundleReload())
		if (m_bundlePane) m_bundlePane->LoadBundles();
}

void EditorFrame::OnMenuDebugBundles(wxCommandEvent& event) {
	const bool enableDebug = event.IsChecked();
	m_settings.SetSettingBool(wxT("bundleDebug"), enableDebug);
}

void EditorFrame::OnMenuBundleAction(wxCommandEvent& event) {
	if (!editorCtrl) return;

#if defined (__WXMSW__)
	if (wxIsShiftDown()) {
#else
	if (wxGetKeyState(WXK_SHIFT)) {
#endif
		// If shift is down, we want to edit the bundle item
		const wxString bundlePath = m_syntax_handler.GetBundleItemUriFromMenu(event.GetId());
		if (!bundlePath.empty()) OpenRemoteFile(bundlePath);
	}
	else m_syntax_handler.DoBundleAction(event.GetId(), *editorCtrl);
}

void EditorFrame::OnMenuKeyDiagnostics(wxCommandEvent& event) {
	m_keyDiags = event.IsChecked();
}

void EditorFrame::OnSubmenuSyntax(wxCommandEvent& event) {
	wxMenuItem* syntaxItem = GetMenuBar()->FindItem(event.GetId());
	if (syntaxItem && editorCtrl) editorCtrl->SetSyntax(syntaxItem->GetLabel(), true);
}

void EditorFrame::OnSubmenuEncoding(wxCommandEvent& event) {
	wxASSERT(editorCtrl);

	unsigned int encoding_id = event.GetId() % 2000;
	editorCtrl->SetEncoding(wxFontMapper::GetEncoding(encoding_id));
}

void EditorFrame::OnMenuEolDos(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl);
	editorCtrl->SetEOL(wxTextFileType_Dos);
}

void EditorFrame::OnMenuEolUnix(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl);
	editorCtrl->SetEOL(wxTextFileType_Unix);
}

void EditorFrame::OnMenuEolMac(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl);
	editorCtrl->SetEOL(wxTextFileType_Mac);
}

void EditorFrame::OnMenuEolNative(wxCommandEvent& event) {
	wxASSERT(editorCtrl);
	m_settings.SetSettingBool(wxT("force_native_eol"), event.IsChecked());
}

void EditorFrame::OnMenuBOM(wxCommandEvent& event) {
	wxASSERT(editorCtrl);
	editorCtrl->SetBOM(event.IsChecked());
}

void EditorFrame::OnMenuNew(wxCommandEvent& WXUNUSED(event)) {
	AddTab();
}

void EditorFrame::OnNotebookDClick(wxAuiNotebookEvent& WXUNUSED(event)) {
	AddTab();
}

void EditorFrame::OnMenuNewWindow(wxCommandEvent& WXUNUSED(event)) {
	wxGetApp().NewFrame();
}

void EditorFrame::OnNotebookContextMenu(wxAuiNotebookEvent& event) {
	// Save the id of the tab under context menu, so that
	// the event handlers know which to affect.
	m_contextTab = event.GetSelection();

	// Create the popup menu
	wxMenu tabPopupMenu;
	tabPopupMenu.Append(MENU_TABS_NEW, _("&New Tab"), _("New Tab"));
	tabPopupMenu.AppendSeparator();
	tabPopupMenu.Append(MENU_TABS_CLOSE, _("&Close Tab\tCtrl-W"), _("Close Tab"));
	tabPopupMenu.Append(MENU_TABS_CLOSE_OTHER, _("Close &other Tabs\tCtrl-Alt-W"), _("Close other Tabs"));
	tabPopupMenu.Append(MENU_TABS_CLOSE_ALL, _("Close &all Tabs"), _("Close all Tabs"));
	tabPopupMenu.AppendSeparator();
	tabPopupMenu.Append(MENU_TABS_COPY_PATH, _("Copy &Path to Clipboard"), _("Copy Path to Clipboard"));

	// Disable copy path if no path in tab
	const EditorCtrl* ec = GetEditorCtrlFromPage(m_contextTab);
	if(ec->GetPath().empty()) tabPopupMenu.Enable(MENU_TABS_COPY_PATH, false);

	// show popup menu
	PopupMenu(&tabPopupMenu);
}

void EditorFrame::OnCopyPathToClipboard(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_contextTab < m_tabBar->GetPageCount());

	const EditorCtrl* ec = GetEditorCtrlFromPage(m_contextTab);
	const wxString path = ec->GetPath();

	// Copy the path to the clipboard
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxTextDataObject(path));
		wxTheClipboard->Close();
	}
}

wxString EditorFrame::GetSaveDir() const {
	wxASSERT(editorCtrl);

	// First try same dir as current tab
	if (!editorCtrl->IsRemote()) {
		const wxFileName& currentPath = editorCtrl->GetFilePath();
		if (currentPath.IsOk() && currentPath.IsAbsolute())
			return currentPath.GetPath();
	}

	// Else current project
	if (HasProject() && !IsProjectRemote()) {
		const wxFileName& projectPath = m_projectPane->GetRootPath();
		return projectPath.GetPath();
	}

	// Last used dir
	wxString lastDir;
	if (m_settings.GetSettingString(wxT("last_open_dir"), lastDir))
		return lastDir;

	// If nothing else is availble, return current working dir
	return wxGetCwd();
}

void EditorFrame::OnMenuOpen(wxCommandEvent& event) {
	wxArrayString filenames;
	{
		// Get the last used dir
		const wxString lastDir = GetSaveDir();

		wxFileDialog dlg(this, _T("Choose a file"), lastDir, _T(""), EditorFrame::DefaultFileFilters, wxFD_OPEN|wxFD_MULTIPLE);
		if (dlg.ShowModal() != wxID_OK)
			return;

		m_settings.SetSettingString(wxT("last_open_dir"), dlg.GetDirectory());

		dlg.GetPaths(filenames);
	}

	// Check if we have to conv from a custom encoding
	wxFontEncoding enc = wxFONTENCODING_SYSTEM;
	if (event.GetId() != wxID_OPEN) {
		unsigned int encoding_id = event.GetId() % 3000;
		enc = wxFontMapper::GetEncoding(encoding_id);
	}

	SetCursor(wxCURSOR_WAIT);
	
	// Only show progress dialog if more than 3 files
	wxProgressDialog* progress_window = NULL;
	const wxString msg = _("Opening selected documents");
	if (filenames.GetCount() > 3) {
		progress_window = new wxProgressDialog(wxT("Progress"), msg, filenames.GetCount(), this, wxPD_APP_MODAL|wxPD_SMOOTH);
		Freeze();
	}

	for (unsigned int i = 0; i < filenames.GetCount(); ++i) {
		if (progress_window)
			progress_window->Update(i, msg + wxT("\n") + filenames[i].c_str());

		wxFileName newpath = filenames[i];
		if (!OpenFile(newpath, enc))
			break;

		if (!progress_window)
			Update();
	}

	if (progress_window) {
		delete progress_window;
		Thaw();
	}

	SetCursor(*wxSTANDARD_CURSOR);
}

void EditorFrame::OnMenuCompareFiles(wxCommandEvent& WXUNUSED(event)) {
	CompareDlg dlg(this, m_generalSettings);
	if (dlg.ShowModal() != wxID_OK) return;

	const wxString leftPath = dlg.GetLeftPath();
	const wxString rightPath = dlg.GetRightPath();

	Compare(leftPath, rightPath);
}

void EditorFrame ::Compare(const wxString& leftPath, const wxString& rightPath) {
	if (wxDirExists(leftPath) && wxDirExists(rightPath)) {
		ShowDiffPane(leftPath, rightPath);
	}
	else if (wxFileExists(leftPath) && wxFileExists(rightPath)) {
		// Check if the diff is already open
		unsigned int page_id;
		DiffPanel* page = GetDiffPaneFromFiles(leftPath, rightPath, page_id);
		if (page) {
			m_tabBar->SetSelection(page_id);
			return;
		}

		DiffPanel* diff = new DiffPanel(m_tabBar, *this, m_catalyst, bitmap);
		diff->SetDiff(leftPath, rightPath);
		AddTab(diff);
	}
}

void EditorFrame::OnMenuOpenProject(wxCommandEvent& WXUNUSED(event)) {
	// Get the last used dir
	wxString lastDir;
	if (!m_settings.GetSettingString(wxT("project"), lastDir)) lastDir = wxGetCwd();

	wxDirDialog dlg(this, _("Open Project"), lastDir, wxDD_NEW_DIR_BUTTON);
	if (dlg.ShowModal() != wxID_OK) return;

	const wxFileName path(dlg.GetPath(), wxEmptyString);
	OpenDirProject(path);
}

void EditorFrame::OnMenuOpenRemote(wxCommandEvent& WXUNUSED(event)) {
	RemoteProfileDlg dlg(this, this->m_generalSettings);
	if (dlg.ShowModal() != wxID_OPEN) return;

	const int profile_id = dlg.GetCurrentProfile();
	if (profile_id == -1) return;

	// Get the profile from db
	const RemoteProfile* rp = m_generalSettings.GetRemoteProfile(profile_id);
	if (rp) OpenRemoteProject(rp);
}

void EditorFrame::OnMenuCloseProject(wxCommandEvent& WXUNUSED(event)) {
	CloseProject();
}

void EditorFrame::OnMenuOpenRecentFile(wxCommandEvent& event) {
	const unsigned int ndx = event.GetId() - 4000;
	if (ndx < m_recentFiles.GetCount())
		Open(m_recentFiles[ndx]);
}

void EditorFrame::OnMenuOpenRecentProject(wxCommandEvent& event) {
	const unsigned int ndx = event.GetId() - 4100;
	if (ndx < m_recentProjects.GetCount())
		Open(m_recentProjects[ndx]);
}

void EditorFrame::OnMenuSave(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SaveText();
	UpdateWindowTitle();
}

void EditorFrame::OnMenuSaveAs(wxCommandEvent& WXUNUSED(event)) {
	if (!editorCtrl->SaveText(true)) return;
	UpdateWindowTitle();

	// Add to recent files list
	const wxString path = editorCtrl->GetPath();
	if (!path.empty()) {
		m_generalSettings.AddRecentFile(path);
		UpdateRecentFiles();
	}
}

void EditorFrame::OnMenuSaveAll(wxCommandEvent& WXUNUSED(event)) {
	// Save all files that are modified
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		if (!page->IsModified()) continue;

		wxString path = page->GetPath();
		if (path.empty()) continue;

		page->SaveText();
	}

	UpdateWindowTitle();
	UpdateTabs();
}

void EditorFrame::OnMenuPageSetup(wxCommandEvent& WXUNUSED(event)) {
	wxPageSetupDialog pageSetupDialog(this, &m_printData);
    if (pageSetupDialog.ShowModal() == wxID_OK)
		m_printData = pageSetupDialog.GetPageSetupData();
}

/*void EditorFrame::OnMenuPrintPreview(wxCommandEvent& WXUNUSED(event)) {
	// Show Print Preview 
	wxPrintPreview *preview = new wxPrintPreview(new EditorPrintout(*editorCtrl));
	if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _T("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}*/

void EditorFrame::OnMenuPrint(wxCommandEvent& WXUNUSED(event)) {
	EditorPrintout printout(*editorCtrl, this->m_syntax_handler.GetTheme());

	wxPrintDialogData printDialogData(m_printData.GetPrintData());
	wxPrinter printer(&printDialogData);

	if (printer.Print(this, &printout, true /*prompt*/)) {
		// Keep the new print settings
        m_printData.SetPrintData(printer.GetPrintDialogData().GetPrintData());
    }
    else {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_T("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _T("Printing"), wxOK);
    }
}

void EditorFrame::OnMenuClose(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_tabBar->GetPageCount() > 0);
	if (m_tabBar->GetPageCount() == 1 && editorCtrl->IsEmpty()) return;

	const int tab_id = m_tabBar->GetSelection();
	CloseTab(tab_id);
}

void EditorFrame::OnMenuCloseWindow(wxCommandEvent& WXUNUSED(event)) {
	Close();
}

void EditorFrame::OnMenuExit(wxCommandEvent& WXUNUSED(event)) {
	wxGetApp().CloseAllFrames();
}

void EditorFrame::OnMenuUndo(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->DoUndo();
}

void EditorFrame::OnMenuRedo(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->Redo();
}

void EditorFrame::OnMenuCut(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->OnCut();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuCopy(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->OnCopy();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuPaste(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->OnPaste();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuFind(wxCommandEvent& WXUNUSED(event)) {
	ShowSearch();
}

void EditorFrame::OnMenuFindInProject(wxCommandEvent& WXUNUSED(event)) {
	if (!m_findInProjectDlg) m_findInProjectDlg = new FindInProjectDlg(*this, m_projectPane->GetInfoHandler());
	m_findInProjectDlg->Show();
	m_findInProjectDlg->SetFocus();

	if (editorCtrl->IsSelected())
		m_findInProjectDlg->SetPattern(editorCtrl->GetSelFirstLine());
}

void EditorFrame::OnMenuFindInSel(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SetSearchRange();
	ShowSearch();
}

void EditorFrame::OnMenuFindNext(wxCommandEvent& WXUNUSED(event)) {
	if (m_searchPanel->HasSearchString()) {
		m_searchPanel->FindNext();
		if (!m_searchPanel->IsActive()) editorCtrl->SetFocus();
	}
	else ShowSearch(true);
}

void EditorFrame::OnMenuFindPrevious(wxCommandEvent& WXUNUSED(event)) {
	if (m_searchPanel->HasSearchString()) {
		m_searchPanel->FindPrevious();
		if (!m_searchPanel->IsActive()) editorCtrl->SetFocus();
	}
	else ShowSearch(true);
}

void EditorFrame::OnMenuFindCurrent(wxCommandEvent& WXUNUSED(event)) {
	// Get current word or selection
	const wxString searchText = editorCtrl->IsSelected() ? editorCtrl->GetSelFirstLine() : editorCtrl->GetCurrentWord();

	// Find first match (don't show panel if not alread shown)
	m_searchPanel->InitSearch(searchText, false);
	m_searchPanel->FindNext();
}

void EditorFrame::OnMenuReplace(wxCommandEvent& WXUNUSED(event)) {
	ShowSearch(true, true);
}

void EditorFrame::OnMenuSelectAll(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SelectAll();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSelectWord(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SelectWord(editorCtrl->GetPos());
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSelectLine(wxCommandEvent& WXUNUSED(event)) {
	//wxLogDebug(m_tabBar->SavePerspective());

	editorCtrl->SelectCurrentLine();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSelectScope(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SelectScope();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSelectFold(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SelectFold();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSelectTag(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SelectParentTag();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuEditTheme(wxCommandEvent& WXUNUSED(event)) {
	ThemeEditor dlg(this, m_syntax_handler);
	dlg.ShowModal();
}

void EditorFrame::OnMenuConvUpper(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ConvertCase(cxUPPERCASE);
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuConvLower(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ConvertCase(cxLOWERCASE);
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuConvTitle(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ConvertCase(cxTITLECASE);
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuConvReverse(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ConvertCase(cxREVERSECASE);
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuRevSel(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->Transpose();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuCompleteWord(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->DoCompletion();
}

void EditorFrame::OnMenuIndentLeft(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->DedentSelectedLines();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuIndentRight(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->IndentSelectedLines();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuTabsToSpaces(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->TabsToSpaces();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSpacesToTabs(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->SpacesToTabs();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuSettings(wxCommandEvent& WXUNUSED(event)) {
	SettingsDlg dlg(this, m_catalyst, m_generalSettings);
	dlg.ShowModal();
}

void EditorFrame::OnMenuFilter(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->FilterThroughCommand();
}

void EditorFrame::OnMenuRunCurrent(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->RunCurrentSelectionAsCommand(false);
}

bool EditorFrame::OnPreKeyUp(wxKeyEvent& event) {
	// We do not seem to be able to catch EVT_KEY_UP for the frame
	// so we catch it manually here.

	// We need to know when the ctrl key is slipped for
	// tab navigation
	if (event.GetKeyCode() == WXK_CONTROL) {
		wxLogDebug(wxT("KeyUp2"));
		m_ctrlHeldDown = false;
	}
	return false;
}

void EditorFrame::OnKeyUp(wxKeyEvent& event) {
	wxLogDebug(wxT("KeyUp %d"), event.GetKeyCode());
	if (event.GetKeyCode() == WXK_CONTROL)
		m_ctrlHeldDown = false;

	event.Skip();
}

void EditorFrame::OnMenuNextTab(wxCommandEvent& evt) {
	// The user may have configured it to go to last active tab
	bool gotoLastTab = false;
	m_settings.GetSettingBool(wxT("gotoLastTab"), gotoLastTab);
	if (gotoLastTab) {
		OnMenuLastTab(evt);
		return;
	}

	// Go to next tab
	m_tabBar->SelectNextTab();
}

void EditorFrame::OnMenuPrevTab(wxCommandEvent& WXUNUSED(event)) {
	m_tabBar->SelectPrevTab();
}

void EditorFrame::OnMenuLastTab(wxCommandEvent& WXUNUSED(event)) {
	// If this is the first time pressed, we go to the last used tab
	if (m_lastActiveTab != -1 && m_lastActiveTab != m_tabBar->GetSelection() && m_lastActiveTab < (int)m_tabBar->GetPageCount()) {
		wxLogDebug(wxT("Going to last active page: %d"), m_lastActiveTab);
		m_tabBar->SetSelection(m_lastActiveTab);
	}
}

void EditorFrame::OnMenuGotoTab(wxCommandEvent& event) {
	const unsigned int tabId = event.GetId() - 40000;
	if (tabId < m_tabBar->GetPageCount()) 
		m_tabBar->SetSelection(m_tabBar->TabToPage(tabId));
}

void EditorFrame::OnMenuGotoLastTab(wxCommandEvent& WXUNUSED(event)) {
	const unsigned int tabcount = m_tabBar->GetPageCount();
	if (tabcount) 
		m_tabBar->SetSelection(m_tabBar->TabToPage(tabcount-1));
}

void EditorFrame::OnTabsShowDropdown(wxCommandEvent& WXUNUSED(event)) {
	vector<OpenTabInfo*> tabInfo;

	wxString rootPath = wxEmptyString;
	if (this->HasProject())
		rootPath = this->GetRootPath().GetPath();

	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);

		wxFileName tabPath = page->GetFilePath();

		wxString relativePath = wxEmptyString;
		const wxString path = tabPath.GetPath();

		if (this->HasProject() && path.StartsWith(rootPath)) {
			// Get relative path
			tabPath.MakeRelativeTo(rootPath);

			relativePath += wxT(".");
			relativePath += wxFileName::GetPathSeparator();
		}

		relativePath += tabPath.GetPath();

		tabInfo.push_back(new OpenTabInfo(page->GetName(), relativePath));
	}

	CurrentTabsPopup dialog(this, tabInfo, m_tabBar->GetSelection());
	if (dialog.ShowModal() == wxID_OK) {
		int newTabIndex = dialog.GetSelectedTabIndex();
		if (newTabIndex != -1 && newTabIndex != m_tabBar->GetSelection())
			m_tabBar->SetSelection(newTabIndex);
	}

	for (vector<OpenTabInfo*>::iterator p = tabInfo.begin(); p != tabInfo.end(); ++p)
		delete *p;
}

void EditorFrame::UpdateTabMenu() {
#ifdef __WXMSW__ //LINUX: removed until wxWidgets rebuild
	wxASSERT(m_tabMenu);

	Freeze();

	// Delete all items
	const wxMenuItemList menus = m_tabMenu->GetMenuItems();
	for (unsigned int m = 0; m < menus.GetCount(); ++m)
		m_tabMenu->Destroy(menus[m]);

	// Add Tab items
	const unsigned int tabcount = m_tabBar->GetPageCount();
	for (unsigned int i = 0; i < tabcount; ++i) {
		const int page_idx = m_tabBar->TabToPage(i);
		const EditorCtrl* ec = GetEditorCtrlFromPage(page_idx);

		wxString label = ec->GetName();
		if (label.empty()) label = _("Untitled");

		if (i < 8) label += wxString::Format(wxT("\tCtrl-%u"), i+1);
		else if (i == tabcount-1) label += wxT("\tCtrl-9");

		m_tabMenu->AppendCheckItem(40000 + i, label);
	}

	Thaw();
#endif
}

void EditorFrame::UpdateRecentFiles() {
	// This may be called before menus are created
	if (!m_recentFilesMenu || !m_recentProjectsMenu) return;

	m_recentFiles.clear();
	m_recentProjects.clear();
	m_generalSettings.GetRecentFiles(m_recentFiles);
	m_generalSettings.GetRecentProjects(m_recentProjects);

	Freeze();

	// Update RecentFiles
	while (m_recentFilesMenu->GetMenuItemCount())
		m_recentFilesMenu->Delete(m_recentFilesMenu->FindItemByPosition(0));

	for (unsigned int i = 0; i < m_recentFiles.GetCount(); i++) {
		wxString filename = m_recentFiles[i];
		if (i < 9)
			filename = wxString::Format(wxT("&%d %s"), i+1, filename);
		m_recentFilesMenu->Append(4000 + i, filename);
	}

	// Update RecentProjects
	while (m_recentProjectsMenu->GetMenuItemCount())
		m_recentProjectsMenu->Delete(m_recentProjectsMenu->FindItemByPosition(0));

	for (unsigned int i = 0; i < m_recentProjects.GetCount(); i++) {
		wxString filename = m_recentProjects[i];
		if (i < 9)
			filename = wxString::Format(wxT("&%d %s"), i+1, filename);
		m_recentProjectsMenu->Append(4100 + i, filename);
	}

	Thaw();
}

void EditorFrame::OnMenuOpenExt(wxCommandEvent& WXUNUSED(event)) {
	if (editorCtrl->IsRemote()) return;

	const wxFileName& path = editorCtrl->GetFilePath();
	if (!path.IsOk()) return;

	// Get all files with same name, but different extensions
	wxArrayString files;
	const wxString spec = path.GetName() + wxT(".*");
	wxDir::GetAllFiles(path.GetPath(), &files, spec, wxDIR_FILES);

	if (files.GetCount() <= 1) return;

	// Find current file
	const int fileId = files.Index(path.GetFullPath());
	if (fileId == wxNOT_FOUND) return;

	// Open next file with same base name
	if (fileId == (int)files.GetCount()-1) OpenFile(files[0]);
	else OpenFile(files[fileId+1]);
}

void EditorFrame::OnMenuGotoBracket(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl);
	editorCtrl->GotoMatchingBracket();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuGotoLine(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl);

	const unsigned int line = editorCtrl->GetCurrentLineNumber();
	const unsigned int max = editorCtrl->GetLineCount();

	GotoLineDlg dlg(this, line, 1, max);
	if (dlg.ShowModal() == wxID_OK) {
		editorCtrl->SetPos(dlg.GetLine(), 0);
		editorCtrl->MakeCaretVisibleCenter();
		editorCtrl->ReDraw();
	}
}

void EditorFrame::OnMenuGotoFile(wxCommandEvent& WXUNUSED(event)) {
	if (!m_projectPane->HasProject()) return;

	GotoFileDlg dlg(this, m_projectPane->GetInfoHandler());
	if (dlg.ShowModal() == wxID_OK) OpenFile(dlg.GetSelection());
}

void EditorFrame::OnMenuFoldToggle(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ToggleFold();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuFoldAll(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->FoldAll();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuFoldOthers(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->FoldOthers();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuUnfoldAll(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->UnFoldAll();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuBookmarkNext(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->GotoNextBookmark();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuBookmarkPrevious(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->GotoPrevBookmark();
	editorCtrl->MakeCaretVisible();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuBookmarkToggle(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ToogleBookmarkOnCurrentLine();
	editorCtrl->ReDraw();
}

void EditorFrame::OnMenuBookmarkClear(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ClearBookmarks();
	editorCtrl->ReDraw();
}

/*
void EditorFrame::OnMenuDocOpen(wxCommandEvent& WXUNUSED(event)) {
	OpenDocDlg dlg(this, m_catalyst);
	if (dlg.ShowModal() == wxID_OK && dlg.HasSelection()) {
		const doc_id di = dlg.GetSelectedDoc();

		OpenDocument(di);
	}
}

void EditorFrame::OnMenuDocShare(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(editorCtrl);

	const doc_id& di = editorCtrl->GetDocID();

	ShareDlg dlg(this, m_catalyst, di);
	dlg.ShowModal();
}
*/
void EditorFrame::OnMenuCommit(wxCommandEvent& WXUNUSED(event)) {
	// Get optional Label & Description from user
	CommitDlg dlg(this);
	if (dlg.ShowModal() != wxID_OK) return;

	editorCtrl->Commit(dlg.GetLabel(), dlg.GetDescription());

	// Save the page settings
	SaveState();

	// By commiting a user indicates that he wants us to protect his document,
	// so we want to commit the db before continuing.
	cxLOCK_WRITE(m_catalyst)
		catalyst.Commit();
	cxENDLOCK

	// Notify that we are editing a new document
	dispatcher.Notify(wxT("WIN_CHANGEDOC"), editorCtrl, GetId());
}
/*
void EditorFrame::OnMenuRevTooltip(wxCommandEvent& WXUNUSED(event)) {
	editorCtrl->ShowRevTooltip();
}

void EditorFrame::OnMenuIncomming(wxCommandEvent& event) {
	wxAuiPaneInfo& inPane = m_frameManager.GetPane(incommingPane);
	if (event.IsChecked()) {
		incommingPane->MakeLastItemVisible();
		inPane.Show();
	}
	else inPane.Hide();
	m_frameManager.Update();
}

void EditorFrame::OnMenuIncommingTool(wxCommandEvent& WXUNUSED(event)) {
	wxAuiPaneInfo& inPane = m_frameManager.GetPane(incommingPane);
	if (!inPane.IsShown()) {
		inPane.Show();
		m_frameManager.Update();
	}
}*/

void EditorFrame::TogglePane(wxWindow* targetPane, bool showPane) {
	wxAuiPaneInfo& pane = m_frameManager.GetPane(targetPane);
	pane.Show(showPane);
	m_frameManager.Update();
}

void EditorFrame::OnMenuRevisionHistory(wxCommandEvent& event) {
	TogglePane(documentHistory, event.IsChecked());
}

void EditorFrame::OnMenuUndoHistory(wxCommandEvent& event) {
	TogglePane(undoHistory, event.IsChecked());
}

void EditorFrame::OnMenuShowCommandOutput(wxCommandEvent& event) {
	TogglePane(m_outputPane, event.IsChecked());
}

void EditorFrame::OnMenuLineNumbers(wxCommandEvent& event) {
	if (event.IsChecked() == m_showGutter) return;

	m_showGutter = event.IsChecked();
	m_settings.SetSettingBool(wxT("showgutter"), m_showGutter);

	// Toggle showing of linenumbers in all editorCtrls
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		page->SetShowGutter(m_showGutter);
	}
}

void EditorFrame::OnMenuIndentGuide(wxCommandEvent& event) {
	if (event.IsChecked() == m_showIndent) return;

	m_showIndent = event.IsChecked();
	m_settings.SetSettingBool(wxT("showindent"), m_showIndent);

	// Toggle showing of indent guides in all editorCtrls
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		page->SetShowIndent(m_showIndent);
	}
}

void EditorFrame::OnMenuWordWrap(wxCommandEvent& event) {
	switch (event.GetId()) {
	case MENU_WRAP_NONE:
		m_wrapMode = cxWRAP_NONE;
		break;
	case MENU_WRAP_NORMAL:
		m_wrapMode = cxWRAP_NORMAL;
		break;
	case MENU_WRAP_SMART:
		m_wrapMode = cxWRAP_SMART;
		break;
	default:
		wxASSERT(false);
	}

	m_wrapMenu->Check(MENU_WRAP_NONE, (m_wrapMode == cxWRAP_NONE));
	m_wrapMenu->Check(MENU_WRAP_NORMAL, (m_wrapMode == cxWRAP_NORMAL));
	m_wrapMenu->Check(MENU_WRAP_SMART, (m_wrapMode == cxWRAP_SMART));

	m_settings.SetSettingInt(wxT("wrapmode"), m_wrapMode);

	// Toggle wordwrap in all editorCtrls
	for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* page = GetEditorCtrlFromPage(i);
		page->SetWordWrap(m_wrapMode);
	}

#ifdef __WXMSW__
	// Save info about the wordwrap mode to the registry so that the crash handler
	// can include it in crash report
	wxRegKey regKey(wxT("HKEY_CURRENT_USER\\Software\\e"));
	const wxString ww = wxString::Format(wxT("%d"), (int)m_wrapMode);
	regKey.SetValue(wxT("ww"), ww);
#endif // __WXMSW__
}
/*
void EditorFrame::OnMenuHighlightUsers(wxCommandEvent& event) {
	wxASSERT(editorCtrl);

	if (event.IsChecked()) {
		editorCtrl->ActivateHighlight();
		m_userHighlight = true;
	}
	else {
		editorCtrl->DisableHighlight();
		m_userHighlight = false;
	}

	cxLOCK_WRITE(m_catalyst)
		catalyst.SetSettingBool(wxT("hl_users"), m_userHighlight);
	cxENDLOCK
}
*/
void EditorFrame::OnMenuPreview(wxCommandEvent& event) {
	if (event.IsChecked()) ShowWebPreview();
	else CloseWebPreview();
}

void EditorFrame::OnMenuShowSymbols(wxCommandEvent& event) {
	if (event.IsChecked()) ShowSymbolList();
	else CloseSymbolList();
}

void EditorFrame::OnMenuShowSnippets(wxCommandEvent& event) {
	if (event.IsChecked()) ShowSnippetList();
	else CloseSnippetList();
}

void EditorFrame::OnMenuSymbols(wxCommandEvent& WXUNUSED(event)) {
	if (m_symbolList) {
		// If symbol list is already open, we switch focus
		if (FindFocus() == editorCtrl) m_symbolList->SetFocus();
		else editorCtrl->SetFocus();
	}
	else {
		// Open symbol list
		ShowSymbolList(false);
		m_symbolList->SetFocus();
	}
}

void EditorFrame::ShowWebPreview() {
	if (m_previewDlg) return; // already shown

	// Create the pane
	m_previewDlg = new PreviewDlg(*this);
	wxAuiPaneInfo paneInfo;
	paneInfo.Name(wxT("IEpreview")).Right().Caption(_("Web Preview")).BestSize(wxSize(150,50)); // defaults

	// Load pane settings
	wxString panePerspective;
	m_settings.GetSettingString(wxT("prvw_pane"), panePerspective);
	m_settings.SetSettingBool(wxT("showpreview"), true);
	if (!panePerspective.empty()) m_frameManager.LoadPaneInfo(panePerspective, paneInfo);

	// Load preview settings
	m_previewDlg->LoadSettings(m_settings);

	// Add to manager
	m_frameManager.AddPane(m_previewDlg, paneInfo);
	m_frameManager.Update();
}

void EditorFrame::ShowSymbolList(bool keepOpen) {
	if (m_symbolList) return; // already shown

	// Create the pane
	m_symbolList = new SymbolList(*this, keepOpen);
	wxAuiPaneInfo paneInfo;
	paneInfo.Name(wxT("Symbols")).Right().Caption(_("Symbols")).BestSize(wxSize(150,50)); // defaults

	// Load pane settings
	wxString panePerspective;
	m_settings.GetSettingString(wxT("symbol_pane"), panePerspective);
	m_settings.SetSettingBool(wxT("showsymbols"), true);
	if (!panePerspective.empty()) m_frameManager.LoadPaneInfo(panePerspective, paneInfo);

	// Add to manager
	m_frameManager.AddPane(m_symbolList, paneInfo);
	m_frameManager.Update();
}

void EditorFrame::CloseSymbolList() {
	if (!m_symbolList) return; // already closed

	wxAuiPaneInfo& pane = m_frameManager.GetPane(m_symbolList);

	// Save pane settings
	const wxString panePerspective = m_frameManager.SavePaneInfo(pane);
	m_settings.SetSettingString(wxT("symbol_pane"), panePerspective);
	m_settings.SetSettingBool(wxT("showsymbols"), false);

	// Delete the symbol pane
	m_frameManager.DetachPane(m_symbolList);
	m_symbolList->Hide();
	m_symbolList->Destroy();
	m_symbolList = NULL;
	m_frameManager.Update();
}

void EditorFrame::ShowSnippetList() {
	if (m_snippetList) return; // already shown
	
	// Create the pane
	m_snippetList = new SnippetList(*this);
	wxAuiPaneInfo paneInfo;
	paneInfo.Name(wxT("Snippet Shortcuts")).Right().Caption(_("Snippet Shortcuts")).BestSize(wxSize(150,50)); // defaults

	// Load pane settings
	wxString panePerspective;
	m_settings.GetSettingString(wxT("snippet_pane"), panePerspective);
	m_settings.SetSettingBool(wxT("showsnippets"), true);
	if (!panePerspective.empty()) m_frameManager.LoadPaneInfo(panePerspective, paneInfo);

	// Add to manager
	m_frameManager.AddPane(m_snippetList, paneInfo);
	m_frameManager.Update();
}

void EditorFrame::CloseSnippetList() {
	if (!m_snippetList) return; // already closed

	wxAuiPaneInfo& pane = m_frameManager.GetPane(m_snippetList);

	// Save pane settings
	const wxString panePerspective = m_frameManager.SavePaneInfo(pane);
	m_settings.SetSettingString(wxT("snippet_pane"), panePerspective);
	m_settings.SetSettingBool(wxT("showsnippets"), false);

	// Delete the snippet pane
	m_frameManager.DetachPane(m_snippetList);
	m_snippetList->Hide();
	m_snippetList->Destroy();
	m_snippetList = NULL;
	m_frameManager.Update();
}

void EditorFrame::RefreshSnippetList() {
	if (!m_snippetList) return;
	m_snippetList->UpdateList();
	m_snippetList->UpdateSearchText();
}

void EditorFrame::CloseWebPreview() {
	if (!m_previewDlg) return; // already closed

	wxAuiPaneInfo& pane = m_frameManager.GetPane(m_previewDlg);

	// Save pane settings
	const wxString panePerspective = m_frameManager.SavePaneInfo(pane);
	m_settings.SetSettingString(wxT("prvw_pane"), panePerspective);
	m_settings.SetSettingBool(wxT("showpreview"), false);

	// Save preview settings
	m_previewDlg->SaveSettings(m_settings);

	// Delete the preview pane
	m_frameManager.DetachPane(m_previewDlg);
	m_previewDlg->Hide();
	m_previewDlg->Destroy();
	m_previewDlg = NULL;
	m_frameManager.Update();
}

void EditorFrame::SetWebPreviewTitle(const wxString& title) {
	if (!m_previewDlg) return; // already closed

	wxAuiPaneInfo& pane = m_frameManager.GetPane(m_previewDlg);
	pane.Caption(title);
	m_frameManager.Update();
}


void EditorFrame::SetUndoPaneCaption(const wxString& caption) {
	wxAuiPaneInfo& pane = m_frameManager.GetPane(wxT("Undo"));
	pane.Caption(caption);
	m_frameManager.Update();
}

void EditorFrame::OnPaneClose(wxAuiManagerEvent& event) {

	if (event.GetPane()->window == m_previewDlg) {
		CloseWebPreview();

		// We have already deleted the window
		// so we don't want aui to do any close handling
		event.Veto();
	}
	else if (event.GetPane()->window == m_symbolList) {
		CloseSymbolList();

		// We have already deleted the window
		// so we don't want aui to do any close handling
		event.Veto();
	}
	else if (event.GetPane()->window == m_snippetList) {
		CloseSnippetList();

		// We have already deleted the window
		// so we don't want aui to do any close handling
		event.Veto();
	}
}

void EditorFrame::OnMenuStatusbar(wxCommandEvent& event) {
	const bool showStatusbar = event.IsChecked();
	if (showStatusbar && !m_pStatBar) {
		CreateAndSetStatusbar();
		m_settings.SetSettingBool(wxT("statusbar"), true);
	}
	else if (!showStatusbar && m_pStatBar) {
		delete m_pStatBar;
		m_pStatBar = NULL;
		SetStatusBar(NULL);
		m_settings.SetSettingBool(wxT("statusbar"), false);
	}
}

void EditorFrame::OnMenuHelp(wxCommandEvent& WXUNUSED(event)) {
	//m_msHtmlHelp.DisplayContents();
	wxLaunchDefaultBrowser(wxT("http://www.e-texteditor.com/wiki"));
}

void EditorFrame::OnMenuAbout(wxCommandEvent& event) {
	int result = 0;
	cxLOCK_READ(m_catalyst)
		eAbout aboutdlg(this, catalyst);
		result = aboutdlg.ShowModal();
	cxENDLOCK

	if (result == 11) OnMenuRegister(event);
}

void EditorFrame::OnMenuGotoForum(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser(wxT("http://www.e-texteditor.com/forum"));
}

void EditorFrame::OnMenuWebsite(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser(wxT("http://www.e-texteditor.com"));
}

void EditorFrame::OnMenuBuy(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser(wxT("http://www.e-texteditor.com/order.html"));
}

void EditorFrame::OnMenuRegister(wxCommandEvent& WXUNUSED(event)) {
	cxLOCK_WRITE(m_catalyst)
		catalyst.ShowRegisterDlg(this);
	cxENDLOCK

	if (!wxGetApp().IsRegistered()) return;

	UpdateWindowTitle(); // to remove title bar nag
	RemoveRegMenus();

	// Show about dialog to confirm
	cxLOCK_READ(m_catalyst)
		eAbout aboutdlg(this, catalyst);
		aboutdlg.ShowModal();
	cxENDLOCK
}

// # no evt.skip() as we don't want the control to erase the background
void EditorFrame::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

void EditorFrame::OnNotebook(wxAuiNotebookEvent& event) {
	if (m_tabBar->GetPageCount() == 0) return;

	wxLogDebug(wxT("Changing page from %d to %d"), event.GetOldSelection(), event.GetSelection());

	// Track last active tab
	m_lastActiveTab = event.GetOldSelection();

	unsigned int idx = event.GetSelection();
	wxASSERT(idx < m_tabBar->GetPageCount());

	// Set page settings
	editorCtrl = GetEditorCtrlFromPage(idx);

	UpdateWindowTitle();

	// Notify that we are editing a new document
	dispatcher.Notify(wxT("WIN_CHANGEDOC"), editorCtrl, GetId());
}

void EditorFrame::UpdateNotebook() {
	EditorCtrl* page = GetEditorCtrlFromPage(m_tabBar->GetSelection());
	if (editorCtrl == page) return;

	editorCtrl = page;
	UpdateWindowTitle();

	// Notify that we are editing a new document
	dispatcher.Notify(wxT("WIN_CHANGEDOC"), editorCtrl, GetId());
}

void EditorFrame::OnNotebookDragDone(wxAuiNotebookEvent& WXUNUSED(event)) {
	UpdateTabMenu();
}

void EditorFrame::OnClose(wxCloseEvent& event) {
	// Check if we should keep state
	bool keep_state = true; // default
	m_settings.GetSettingBool(wxT("keepState"), keep_state);
	if (wxGetKeyState(WXK_SHIFT)) keep_state = true; // override

	// Check if we have any unsaved documents
	// and ask the user if he wants to save them
	if (AskToSaveMulti() == false) {
		event.Veto(); // Cancel if user aborted
		return;
	}

#ifdef __WXMSW__
	// Show nag screen if the trial is about to expire
	if (!wxGetApp().IsRegistered() && wxGetApp().DaysLeftOfTrial() <= 5) {
		int result = 0;
		cxLOCK_READ(m_catalyst)
			eAbout aboutdlg(this, catalyst);
			result = aboutdlg.ShowModal();
		cxENDLOCK

		if (result == 11) {
			wxCommandEvent dummyEvent;
			OnMenuRegister(dummyEvent);
			event.Veto(); // Cancel close
			return;
		}
	}
#endif

	wxLogDebug(wxT("OnClose"));
	// Keep State
	if (keep_state) {
		m_needStateSave = true; // save even if win is no longer active
		SaveState();
	}
	else SaveSize();

	Show(false);

	// Close any open dialogs
	if (m_previewDlg) m_previewDlg->Destroy();
	if (m_findInProjectDlg) {
		m_findInProjectDlg->Hide();
		m_findInProjectDlg->Destroy();
	}

	// Close all open documents
	for (int i = m_tabBar->GetPageCount()-1; i >= 0; --i) {
		if (keep_state) {
			EditorCtrl* page = GetEditorCtrlFromPage(i);
			page->EnableRedraw(false); // avoid accidental redraw during close
			page->Close();
		}
		else DeletePage(i);
	}
	editorCtrl = NULL; // avoid dangling pointer

	// Clean up state info
	if (!keep_state || !wxGetApp().IsLastFrame()) {
		m_settings.SetSettingBool(wxT("showproject"), false);
		m_generalSettings.RemoveFrame(m_settings);
	}

	Destroy();
}

void EditorFrame::OnActivate(wxActivateEvent& event) {
	wxLogDebug(wxT("EditorFrame::OnActivate %d"), event.GetActive());

	// Always skip to allow internal processing
	event.Skip();

	if (!event.GetActive()) return;
	if (!editorCtrl) return;

	if (IsShown()) {
		// If the frame get focus we want to pass it to the currently active editorCtrl
		editorCtrl->SetFocus();

		if (!m_inAskReload) {
			// Should we check for changed files?
			bool doCheckChange = true;  // default
			m_settings.GetSettingBool(wxT("checkChange"), doCheckChange);

			// Check if any open files have been modified (in separate thread)
			if (doCheckChange)
				CheckForModifiedFilesAsync();
		}

		// State should only be saved when the window is active
		m_needStateSave = true;
	}
	else {
		// Commit the documents & settings while we are inactive
		// TODO: Wait until we have been idle for some time
		/*SaveState();
		cxLOCK_WRITE(m_catalyst)
			catalyst.Commit();
		cxENDLOCK*/
	}

	//wxLogDebug(wxT("EditorFrame::OnActivate Done"));
}

void EditorFrame::OnSize(wxSizeEvent& event) {
	//wxLogDebug(wxT("EditorFrame::OnSize"));
	m_sizeChanged = true; // Save pos & size in OnIdle()
	event.Skip();
}

void EditorFrame::OnMove(wxMoveEvent& event) {
	m_sizeChanged = true; // Save pos & size in OnIdle()
	event.Skip();
}

void EditorFrame::OnMaximize(wxMaximizeEvent& event) {
	const bool ismax = IsMaximized();
	m_settings.SetSettingBool(wxT("topwin/ismax"), ismax);
	event.Skip();
}

void EditorFrame::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event)) {
}

void EditorFrame::LoadSize() {
	int x, y, width, height;
	bool ismax;

	if(m_settings.GetSettingBool(wxT("topwin/ismax"), ismax) && ismax) Maximize(true);

	if (!m_settings.GetSettingInt(wxT("topwin/x"), x)) x = -1;
	if (!m_settings.GetSettingInt(wxT("topwin/y"), y)) y = -1;
	if (!m_settings.GetSettingInt(wxT("topwin/width"), width)) width = -1;
	if (!m_settings.GetSettingInt(wxT("topwin/height"), height)) height = -1;

	if (!(x >= -1 && y >= -1 && width >= -1 && height >= -1))
		wxLogDebug(wxT("EditorFrame::LoadSize() - invalid values!"));

	// Dont try to set default size if maximized
	if (ismax && (x < 0 || y < 0 || width < 0 || height < 0)) return;

#ifndef __WXMSW__
	// Make sure that window fits on current screen
	const int screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	const int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	if (y >= -5 &&  screenHeight && y + height <= screenHeight && x < screenWidth) {
		SetSize(x, y, width, height, wxSIZE_USE_EXISTING);
	}
#else
	POINT pt; pt.x=x; pt.y = y;
	if (::MonitorFromPoint( pt, MONITOR_DEFAULTTONULL ) != NULL)
		SetSize(x, y, width, height, wxSIZE_USE_EXISTING);
#endif
}

void EditorFrame::SaveSize() {
	// NOTE: When adding settings, remember to also add
	//       them to eApp::ClearState()

	const wxRect rect = GetRect();
	const bool ismax = IsMaximized();

	if (!ismax) {
		if (!(rect.x >= 0 && rect.y >= 0 && rect.width >= 0 && rect.height >= 0))
			wxLogDebug(wxT("EditorFrame::SaveSize() - invalid values!"));

		m_settings.SetSettingInt(wxT("topwin/x"), rect.x);
		m_settings.SetSettingInt(wxT("topwin/y"), rect.y);
		m_settings.SetSettingInt(wxT("topwin/width"), rect.width);
		m_settings.SetSettingInt(wxT("topwin/height"), rect.height);
	}
	m_settings.SetSettingBool(wxT("topwin/ismax"), ismax);

	m_sizeChanged = false;
}

void EditorFrame::SaveState() {
	// NOTE: When adding settings, remember to also add them to eApp::ClearState()
	if (!m_needStateSave) return;
	if (!editorCtrl) return;

	if (m_sizeChanged) SaveSize();

	//wxLogDebug(wxT("SaveState"));

#ifdef __WXMSW__ //LINUX: removed until wxWidgets rebuild
	// Clear out previous state
	// (may be out-of-date with pagecount)
	m_settings.DeleteAllPageSettings();

	// Go through editorCtrls and see if any have changed
	const unsigned int pageCount = m_tabBar->GetPageCount();
	if (pageCount > 1 || !editorCtrl->IsEmpty()) { // don't save state if just a single empty page
		for (unsigned int i = 0; i < pageCount; ++i)
			GetPage(i)->SaveSettings(i, m_settings);
	}
	const wxString tablayout = m_tabBar->SavePerspective();

	// WORKAROUND: Aui does not save size between hide/show
	wxAuiPaneInfo& projectPane = m_frameManager.GetPane(wxT("Project"));
	if (projectPane.IsShown())
		projectPane.BestSize(projectPane.window->GetSize());

	// Get The frame layout
	const wxString perspective = m_frameManager.SavePerspective();

	// Save web preview layout (only needed if visible)
	if (m_previewDlg) {
		wxAuiPaneInfo& pane = m_frameManager.GetPane(m_previewDlg);

		// Save pane settings
		const wxString panePerspective = m_frameManager.SavePaneInfo(pane);
		m_settings.SetSettingString(wxT("prvw_pane"), panePerspective);
		m_settings.SetSettingBool(wxT("showpreview"), true);

		// Save preview settings
		m_previewDlg->SaveSettings(m_settings);
	}

	// Save symbol list layout
	if (m_symbolList) {
		wxAuiPaneInfo& pane = m_frameManager.GetPane(m_symbolList);

		// Save pane settings
		const wxString panePerspective = m_frameManager.SavePaneInfo(pane);
		m_settings.SetSettingString(wxT("symbol_pane"), panePerspective);
		m_settings.SetSettingBool(wxT("showsymbols"), true);
	}

	// Save snippet list layout
	if (m_snippetList) {
		wxAuiPaneInfo& pane = m_frameManager.GetPane(m_snippetList);

		// Save pane settings
		const wxString panePerspective = m_frameManager.SavePaneInfo(pane);
		m_settings.SetSettingString(wxT("snippet_pane"), panePerspective);
		m_settings.SetSettingBool(wxT("showsnippets"), true);
	}

	m_settings.SetSettingBool(wxT("showproject"), projectPane.IsShown());
	m_settings.SetSettingString(wxT("topwin/panes"), perspective);
	if (m_tabBar->GetPageCount() != 0) m_settings.SetSettingInt(wxT("topwin/page_id"), m_tabBar->GetSelection());
	else m_settings.SetSettingInt(wxT("topwin/page_id"), 0);
	m_settings.SetSettingString(wxT("topwin/tablayout"), tablayout);
#endif

	// Only save once if window is inactive
	if (!IsActive()) m_needStateSave = false;
}

void EditorFrame::OnIdle(wxIdleEvent& event) {
	if (!editorCtrl) {event.Skip();return;}
	wxASSERT(m_tabBar->GetPageCount() != 0);

	//wxLogDebug(wxT("EditorFrame::OnIdle"));

	SaveState();
	event.Skip();
}

bool EditorFrame::CloseTab(unsigned int tab_id, bool removetab) {
	wxASSERT(tab_id >= 0 && tab_id < m_tabBar->GetPageCount());

	EditorCtrl* page = GetEditorCtrlFromPage(tab_id);

	// The page should always be valid, but there has been
	// bug reports with invalid pages.
	// wxASSERT(page);

	if (page && page->IsModified()) {
		m_tabBar->SetSelection(tab_id);

		wxString path = page->GetPath();
		if (path.empty()) path = page->GetName();

		// Ask the user if he wants to save it
		wxString message;
		if (path.empty()) message = _("The untitled file has been modified\n\nSave changes?");
		else {
			message = path;
			message += _(" has been modified!\n\nSave changes?");
		}

		wxMessageDialog dlg(this, message, wxT("e"), wxYES_NO|wxCANCEL|wxICON_EXCLAMATION);
		dlg.Centre();

		int result = dlg.ShowModal();
		if (result == wxID_CANCEL) return false; // Veto to close
		
		if (result == wxID_YES) {
			if (!page->SaveText()) return false; // Cancel if save failed
		}
	}

	Freeze(); // optimize redrawing
	bool result = DeletePage(tab_id, removetab);

	// If we deleted last editCtrl, then we have to create a new empty one
	if (m_tabBar->GetPageCount() == 0) AddTab();

	Thaw(); // optimize redrawing

	return result;
}

void EditorFrame::OnCloseTab(wxAuiNotebookEvent& event) {
	unsigned int tab_id = event.GetSelection();
	wxASSERT(tab_id >= 0 && tab_id < m_tabBar->GetPageCount());

	if (CloseTab(tab_id, false) == false) event.Veto(); // Veto the close
}

void EditorFrame::OnTabClosed(wxAuiNotebookEvent& WXUNUSED(event)) {
	// If we deleted last editCtrl, then we have to create a new empty one
	if (m_tabBar->GetPageCount() == 0) AddTab();

	UpdateTabMenu();
}

void EditorFrame::OnDoCloseTab(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_contextTab < m_tabBar->GetPageCount());
	CloseTab(m_contextTab, true);
}

void EditorFrame::OnCloseOtherTabs(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_tabBar->GetPageCount() != 0);

	const unsigned int keep_tab_id = m_contextTab;
	wxASSERT(keep_tab_id >= 0 && keep_tab_id < m_tabBar->GetPageCount());

	if (AskToSaveMulti(keep_tab_id) == false) return;

	// Delete pages and close the tabs
	Freeze(); // optimize redrawing
	for (unsigned int tab_id = m_tabBar->GetPageCount()-1; m_tabBar->GetPageCount() > 1; --tab_id)
		if (tab_id != keep_tab_id) DeletePage(tab_id, true);

	Thaw(); // optimize redrawing

	// The TabBar should ensure that the last page is selected
	wxASSERT(m_tabBar->GetPageCount() == 1 && editorCtrl == GetEditorCtrlFromPage(0));
}

void EditorFrame::OnCloseAllTabs(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_tabBar->GetPageCount() != 0);

	if (m_tabBar->GetPageCount() == 1 && editorCtrl->IsEmpty()) return;

	if (AskToSaveMulti() == false) return;

	// Delete pages and close the tabs
	Freeze(); // optimize redrawing
	for (unsigned int tab_id = m_tabBar->GetPageCount()-1; m_tabBar->GetPageCount() >= 1; --tab_id) {
		DeletePage(tab_id, true);
	}

	// Since we deleted all editCtrls, we have to create a new empty one
	AddTab();
	Thaw(); // optimize redrawing
}

bool EditorFrame::DeletePage(unsigned int page_id, bool removetab) {
	wxASSERT(page_id < m_tabBar->GetPageCount());

	EditorCtrl* ec = GetEditorCtrlFromPage(page_id);
	const doc_id di = ec->GetDocID();

	if(!ec->Close()) return false; //Vetoed close

	// Notify PreviewDlg that the tab is closing (it might be pinned)
	if (m_previewDlg) m_previewDlg->PageClosed(ec);

	// Notify that we are closing the page (there might be subscribers with refs)
	dispatcher.Notify(wxT("WIN_CLOSEPAGE"), editorCtrl, GetId());

	if (ec == editorCtrl) editorCtrl = NULL; // Make sure we don't accidentally use deleted ctrl

	//eGetSettings().DeletePageSettings(page_id);
	//pages.erase(pages.begin() + page_id);

	if (di.IsDraft()) {
		// Check if there are other pages that use the same document
		bool document_in_use = false;
		for (unsigned int i = 0; i < m_tabBar->GetPageCount(); ++i) {
			EditorCtrl* page = GetEditorCtrlFromPage(i);

			if (di.SameDoc(page->GetDocID())) document_in_use = true;
		}

		// Delete the document if this was the last ref
		// and it is unmodified since creation/loading
		if (!document_in_use) {
			cxLOCK_WRITE(m_catalyst)
				if (catalyst.IsDraftDeletableOnExit(di)) {
					catalyst.DeleteDraft(di);

					// Notify subscribers that the document has been deleted
					dispatcher.Notify(wxT("DOC_DELETED"), &di, 0);
				}
			cxENDLOCK
		}
	}

	if (removetab) m_tabBar->DeletePage(page_id);

	return true;
}

// static notification handler
void EditorFrame::OnDocChange(EditorFrame* self, void* data, int WXUNUSED(filter)) {
	if (!self->editorCtrl) return; // Might be called before editorCtrl is initialized

	const doc_id& di = *(const doc_id*)data;
	if (!di.SameDoc(self->editorCtrl->GetDocID())) return;

	// Update title after doc change so that the modification status
	// is correct.
	// TODO: only update after first mod
	self->UpdateWindowTitle();
}

// static notification handler
void EditorFrame::OnOpenDoc(EditorFrame* self, void* data, int WXUNUSED(filter)) {
	if (!self->editorCtrl) return;

	const uintptr_t docId = (uintptr_t)data;
	const doc_id di(DOCUMENT, docId, 0);

	// If current doc is same just keep it
	bool sameDoc;
	cxLOCK_READ(self->m_catalyst)
		sameDoc = catalyst.InSameHistory(di, self->editorCtrl->GetDocID());
	cxENDLOCK
	if (sameDoc) {
		self->editorCtrl->SetFocus();
		return;
	}

	// Check if an revision of the document is already in an open tab
	for (unsigned int i = 0; i < self->m_tabBar->GetPageCount(); ++i) {
		EditorCtrl* const editor = self->GetEditorCtrlFromPage(i);

		cxLOCK_READ(self->m_catalyst)
			sameDoc = catalyst.InSameHistory(di, editor->GetDocID());
		cxENDLOCK
		if (sameDoc) {
			self->m_tabBar->SetSelection(i);
			return;
		}
	}

	// Open doc in new tab
	doc_id headDoc;
	cxLOCK_READ(self->m_catalyst)
		headDoc = catalyst.GetDocHead(docId);
	cxENDLOCK
	if (self->editorCtrl->IsEmpty()) self->editorCtrl->SetDocument(headDoc);
	else {
		EditorCtrl* ec = new EditorCtrl(headDoc, wxT(""), self->m_catalyst, self->bitmap, self->m_tabBar, *self);
		self->AddTab(ec);
	}
}

// static notification handler
void EditorFrame::OnIncommingChanged(EditorFrame* WXUNUSED(self), void* WXUNUSED(data), int WXUNUSED(filter)) {
	/*if (self->m_pToolBar) {
		bool hasIncomming;
		cxLOCK_READ(self->m_catalyst)
			hasIncomming = catalyst.HasIncomming();
		cxENDLOCK

		// Change toolbar image (only if needed since it needs a total rebuild)
		if (self->m_pToolBar->GetToolClientData(MENU_INCOMMING_TOOLBAR) != (wxObject*)hasIncomming) {
			const wxBitmap& bmpIncomming = hasIncomming ? self->m_incommingFullBmp : self->m_incommingBmp;
			self->m_pIncommingTool->SetNormalBitmap(bmpIncomming);
			self->m_pToolBar->SetToolClientData(MENU_INCOMMING_TOOLBAR, (wxObject*)hasIncomming);
			self->m_pToolBar->Realize();
		}
	}*/
}

void EditorFrame::OnBundleActionsReloaded(EditorFrame* self, void* WXUNUSED(data), int WXUNUSED(filter)) {
	self->ResetBundleMenu();
}

void EditorFrame::OnBundlesReloaded(EditorFrame* self, void* WXUNUSED(data), int WXUNUSED(filter)) {
	self->ResetSyntaxMenu();
	self->ResetBundleMenu();
	self->CheckForModifiedFilesAsync();
}

void EditorFrame::OnMouseWheel(wxMouseEvent& event) {
	// If no one else handled the event, send it to the editor.
	this->editorCtrl->ProcessMouseWheel(event);
}
