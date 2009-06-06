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

// eApp.cpp: implementation of the eApp class.
//
//////////////////////////////////////////////////////////////////////

// Needed to enable XP-Style common controls
#if defined(__WXMSW__) && !defined(__WXWINCE__)
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df'\"")
#endif

#include "tomcrypt.h" // has to be first to avoid conflict with STL

#include "eApp.h"

#include <wx/filename.h>
#include <wx/file.h>
#include <wx/tokenzr.h>
#include <wx/dialup.h>
#include <wx/protocol/http.h>
#include <wx/txtstrm.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

#include "Dispatcher.h"
#include "plistHandler.h"
#include "tm_syntaxhandler.h"
#include "RemoteThread.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"
#include "eDocumentPath.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>

// Main on Windows is defined in separate e-exe project.
#else
IMPLEMENT_APP(eApp)
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(eApp, wxApp)
	EVT_MENU(ID_UPDATES_AVAILABLE, eApp::OnUpdatesAvailable)
	EVT_IDLE(eApp::OnIdle)
END_EVENT_TABLE()

bool eApp::OnInit() {
	// Initialize variables
	frame = NULL;
	m_pCatalyst = NULL;
	m_catalyst = NULL;
	m_pSyntaxHandler = NULL;
	m_checker = NULL;
#ifndef __WXMSW__
	m_server = NULL;
#endif

#ifdef __WXGTK__
	if(false == wxApp::OnInit()) {
		wxLogError(wxT("Invoking wxInitialize() failed! Exiting..."));
		return false;
	}
#endif

#if defined (__WXGTK__) && defined (__WXDEBUG__)
	wxLog *logger=new wxLogStream(&cout);
	wxLog::SetActiveTarget(logger);
#endif

	SetAppName(wxT("e"));

	// App info
	const wxString appId = wxString::Format(wxT("eApp-%s"), wxGetUserId().c_str());
	m_version_id = 198;  // <-------------- INTERNAL VERSION NUMBER
	m_version_name =  wxT("1.0.35"); // <-- VERSION NAME

	// Option vars
	m_lineNum = 0;
	m_columnNum = 0;
	wxString mate;
	bool clearState = false;
	bool clearLayout = false;
	bool clearUndo = false;
	bool clearBundleCache = false;
	bool checkForUpdate = true;

	// Parse options
	for (int i = 1; i < argc; ++i) {
		const wxString& arg = argv[i];
		if (arg == wxT("--clearstate")) clearState = true;
		else if (arg == wxT("--clearlayout")) clearLayout = true;
		else if (arg == wxT("--clearundo")) clearUndo = true;
		else if (arg == wxT("--clearcache")) clearBundleCache = true;
		else if (arg == wxT("--noupdate")) checkForUpdate = false;
		else if (arg == wxT("--mate")) {
			++i;
			mate = argv[i];
		}
		else if (arg == wxT("-l") || arg == wxT("--line")) {
			if (i < argc+1) {
				++i;
				const wxString& lineStr = argv[i];
				lineStr.ToULong(&m_lineNum);
			}
		}
		else if (arg == wxT("-c") || arg == wxT("--column")) {
			if (i < argc+1) {
				++i;
				const wxString& lineStr = argv[i];
				lineStr.ToULong(&m_columnNum);
			}
		}
		else m_files.Add(arg);
	}

	// Check if there is another instance running
	m_checker = new wxSingleInstanceChecker(appId);
    if (m_checker->IsAnotherRunning()) {
        wxLogDebug(wxT("Another program instance is already running"));
		if (SendArgsToInstance()) return false; // only exit if instance received args
    }

	// curl setup (cleanup in OnExit)
	curl_global_init(CURL_GLOBAL_DEFAULT);

	// Get the application paths
	const wxStandardPathsBase& paths = wxStandardPaths::Get();
	m_appPath = paths.GetExecutablePath().BeforeLast(wxFILE_SEP_PATH) + wxFILE_SEP_PATH;
	m_appDataPath = paths.GetUserDataDir() + wxFILE_SEP_PATH;
	if (!wxDirExists(m_appDataPath)) wxMkdir(m_appDataPath);

#ifdef __WXDEBUG__
	// Open a file for logging
	if (m_logFile.Open(m_appDataPath + wxT("log.txt"), wxT("w"))) {
		wxLog::SetActiveTarget(new wxLogGui());
		new wxLogChain(new wxLogStderr(m_logFile.fp()));
  	}
#endif //__WXDEBUG__

	// Initialize wxWidgets internals
	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxGIFHandler);
	wxImage::AddHandler(new wxICOHandler);
	//wxFileSystem::AddHandler(new wxInternetFSHandler);

	// Set up the database
	m_pCatalyst = new Catalyst(m_appDataPath + wxT("e.db"));
	m_catalyst = new CatalystWrapper(*m_pCatalyst);

	// Quit if trial has expired
	if (m_pCatalyst->IsExpired()) return false;

	// Load Settings
	m_settings.Load(m_appDataPath);
	if (m_settings.IsEmpty()) m_pCatalyst->MoveOldSettings(m_settings);

	// Apply options
	if (clearState) ClearState();
	if (clearLayout) ClearLayout();
	if (clearUndo) {
		m_pCatalyst->ClearDraftsWithoutParent();
		ClearState(); // may contain refs to drafts
	}

	// Parse syntax files
	wxLogDebug(wxT("Loading bundles"));
	m_pListHandler = new PListHandler(GetAppPath(), GetAppDataPath(), clearBundleCache);
	m_pSyntaxHandler = new TmSyntaxHandler(m_pCatalyst->GetDispatcher(), *m_pListHandler);

    // Create the main window
	wxLogDebug(wxT("Creating main frame"));
	frame = new EditorFrame( *m_catalyst, -1, wxT("e"), DetermineFrameSize(), *m_pSyntaxHandler);
	SetTopWindow(frame);

	// Show the main window, bringing it to the top.
	frame->Show(true);
	frame->Raise();
	frame->Update();

	// Open files from saved state
	frame->RestoreState();

	// Open files from command-line options
	if (argc > 1) {
		bool isFirst = true;
		for (unsigned int i = 0; i < m_files.Count(); ++i) {
			const wxString& arg = m_files[i];

			frame->Open(arg, mate);

			if (isFirst) {
				if (m_lineNum || m_columnNum) frame->GotoPos(m_lineNum, m_columnNum);
				isFirst = false;
			}
		}
	}

	// Set up ipc server
#ifdef __WXMSW__
	new eIpcWin(*this); // hidden win to receive ipc messages
#else
	m_server = new eServer(*this);
	m_server->Create(wxT("eServer"));
#endif

#ifdef __WXMSW__
	wxLogDebug(wxT("Initializing cygwin"));
	eDocumentPath::InitCygwinOnce();
#endif

	wxLogDebug(wxT("Checking for modified files"));
	frame->CheckForModifiedFilesAsync();
	wxLogDebug(wxT("Done Checking for modified files"));

	if (checkForUpdate) {
		// Check website to se if there are updates
		CheckForUpdates();
	}

    return true;
}

void eApp::ClearState() {
	// Pages
	m_settings.DeleteAllPageSettings();

	// Tab layout
	m_settings.RemoveSetting(wxT("topwin/tablayout"));
	m_settings.RemoveSetting(wxT("topwin/page_id"));

	m_settings.Save();
}

void eApp::ClearLayout() {
	// Window size and position
	m_settings.RemoveSetting(wxT("topwin/x"));
	m_settings.RemoveSetting(wxT("topwin/y"));
	m_settings.RemoveSetting(wxT("topwin/width"));
	m_settings.RemoveSetting(wxT("topwin/height"));
	m_settings.RemoveSetting(wxT("topwin/ismax"));

	// wxAUI perspective
	m_settings.RemoveSetting(wxT("topwin/panes"));

	// pane settings
	m_settings.RemoveSetting(wxT("symbol_pane"));
	m_settings.RemoveSetting(wxT("showsymbols"));
	m_settings.RemoveSetting(wxT("prvw_pane"));
	m_settings.RemoveSetting(wxT("showpreview"));
	m_settings.RemoveSetting(wxT("showproject"));

	m_settings.Save();
}

#ifdef __WXMSW__
void eApp::SendCommandToServer(HWND hWndRecv, const wxString& cmd) {
	const wxCharBuffer msg = cmd.mb_str(wxConvUTF8);
	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = strlen( msg.data() )+1;
	cds.lpData = (void*)msg.data();
	::SendMessage(hWndRecv, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds );
}
#endif

bool eApp::SendArgsToInstance() {
#ifdef __WXMSW__
	// Get handle to main frame of running instance
	HWND hWndRecv = NULL;
	for (unsigned int i = 0; i < 10; ++i) {
		hWndRecv = ::FindWindow(wxT("wxWindowClassNR"), wxT("eIpcWin"));
		if (hWndRecv) break;
		if (!m_checker->IsAnotherRunning()) {
			// Instance has closed. Just open our selves
			return false;
		}
		wxSleep(2);
	}

	if (!hWndRecv) {
		wxMessageBox(_("Unable to contact the running e instance!\nIf you have a hanging process, you can close it from the Task Manager."));
		return true;
	}

	// Open files from command-line options
	if (!m_files.IsEmpty()) {
		// Open files from command-line options
		bool isFirst = true;
		for (unsigned int i = 0; i < m_files.Count(); ++i) {
			const wxString& arg = m_files[i];

			wxString cmd = wxT("OPEN_FILE ");

			// Add position options
			if (isFirst) {
				if (m_lineNum) cmd += wxString::Format(wxT("LINE=%u "), m_lineNum);
				if (m_columnNum) cmd += wxString::Format(wxT("COL=%u "), m_columnNum);
				isFirst = false;
			}

			if (arg.StartsWith(wxT("txmt:")) ) {
				cmd += arg;
			}
			else {
				if (eDocumentPath::IsRemotePath(arg)) cmd += arg;
				else {
					wxFileName path(arg);
					path.MakeAbsolute();
					cmd += path.GetFullPath();
				}
			}

			SendCommandToServer(hWndRecv, cmd);
		}
	}
	else {
		// Activate and create new document
		const wxString cmd = wxT("NEW_WINDOW");
		SendCommandToServer(hWndRecv, cmd);
	}

	{ // Bring window to front
		const wxString cmd = wxT("VIEW_RAISE");
		SendCommandToServer(hWndRecv, cmd);
	}

	return true;
#else
	eClient client(*this);
	eConnection* connection = NULL;

	// Connect to first instance and tell it to open a new window
	// (it may be busy, so we give it a few tries it it does not respond)
	{
		wxLogNull logNo; // we don't want to report DDE errors to user
		for (unsigned int i = 0; i < 5; ++i) {
			if (i) wxSleep(2);
			connection = (eConnection *)client.MakeConnection(wxT("localhost"), wxT("eServer"), wxT("e"));
			if (connection) break;
		}
	}

	if (!connection)
	{
		wxMessageBox(_("The running e process does not respond.\nIf it has locked up you may have to kill from the Task Manager."), _("e Error"));
	}
	else {
		// The connection may delete itself, so if that happens
		// we want it to invalidate the pointer.
		connection->SetPointer(&connection);

		if (argc > 1) {
			// Open files from command-line options
			bool isFirst = true;
			for (int i = 1; i < argc; ++i) {
				const wxString& arg = argv[i];
				if (arg.StartsWith(wxT("-"))) continue; // ignore options

				wxString cmd = wxT("OPEN_FILE ");

				// Add position options
				if (isFirst) {
					if (m_lineNum) cmd += wxString::Format(wxT("LINE=%u "), m_lineNum);
					if (m_columnNum) cmd += wxString::Format(wxT("COL=%u "), m_columnNum);
					isFirst = false;
				}

				if (arg.StartsWith(wxT("txmt:"))) cmd += arg;
				else {
					if (eDocumentPath::IsRemotePath(arg)) cmd += arg;
					else {
						wxFileName path(arg);
						path.MakeAbsolute();
						cmd += path.GetFullPath();
					}
				}

				wxLogNull logNo; // we don't want to report DDE errors to user
				if (!connection->Execute(cmd)) {
					wxMessageBox(_("The running e process does not respond.\nIt may just be busy, but if it has locked up you can kill it from the Task Manager."), _("e Error"));
				}
			}
		}
		else connection->Execute(wxT("NEW_WINDOW"));

		if (connection) delete connection; // also disconnects
	}
	return true;
#endif
}

void eApp::OnIdle(wxIdleEvent& event) {
	if (!m_openStack.IsEmpty()) {
		const wxString cmd = m_openStack[0];
		m_openStack.RemoveAt(0);
		frame->Open(cmd);
	}

	if (m_pSyntaxHandler) {
		if (m_pSyntaxHandler->DoIdle()) event.RequestMore();
	}

	// Important: wxApp needs to do its idle processing as well
	event.Skip();
}

bool eApp::ExecuteCmd(const wxString& cmd) {
	wxString result;
	return ExecuteCmd(cmd, result);
}

bool eApp::ExecuteCmd(const wxString& cmd, wxString& result) {
	wxASSERT(!cmd.empty());
	if (!frame) return false; // May not be created yet

	wxString exec_cmd = cmd;
	if (exec_cmd.Last() == '\0') exec_cmd.RemoveLast();
	const wxString command = exec_cmd.BeforeFirst(' ');
	const wxString args = exec_cmd.AfterFirst(' ');
	EditorCtrl* eCtrl = frame->GetEditorCtrl();
	if (!eCtrl) return false;

	wxLogDebug(wxT("Execute command: %s "), exec_cmd.c_str());

	if (command == wxT("VIEW_RAISE")) {
		frame->BringToFront();
		return true;
	}

	if (command == wxT("NEW_WINDOW")) {
		frame->AddTab();
		return true;
	}
	
	if (command.StartsWith(wxT("OPEN_FILE"))) {
		wxString path = args;
		wxString mate;

		// Check if we should notify mate on close
		if (command == wxT("OPEN_FILE_WAIT")) {
			mate = args.BeforeFirst(' ');
			path = args.AfterFirst(' ');
		}

		// Extract position vars
		unsigned int lineNum = 0;
		unsigned int columnNum = 0;
		path = ExtractPosArgs(path, lineNum, columnNum);

		// Open the file
		const bool res = frame->Open(path, mate);
		if (res && (lineNum || columnNum)) frame->GotoPos(lineNum, columnNum);
		return res;
	}
	
	if (command == wxT("view_insert")) {
		if (args.empty()) return false;
		if (args.size() == 1) eCtrl->InsertChar((wxChar)args[0]);
		else eCtrl->Insert(args);
		return true;
	}
	
	if (command == wxT("view_delete")) {
		wxString str_start = args.BeforeFirst(wxT(' '));
		wxString str_end = args.AfterFirst(wxT(' '));
		long start, end;
		if (!str_start.ToLong(&start)) return false;
		if (!str_end.ToLong(&end)) return false;
		if (start < 0 || start >= (long)eCtrl->GetLength()) return false;
		if (end <= start || end > (long)eCtrl->GetLength()) return false;

		eCtrl->Delete(start, end);
		return true;
	}
	
	if (command == wxT("view_freeze")) {
		eCtrl->Freeze();
		return true;
	}
	
	if (command == wxT("view_getlength")) {
		int length = eCtrl->GetLength();
		result.Printf(wxT("%d"), length);
		wxLogDebug(wxT("  returning length: %d %s"), length, result.c_str());
		return true;
	}
	
	if (command == wxT("view_setpos")) {
		long pos;
		if (!args.ToLong(&pos)) return false;
		if (pos < 0 || pos > (long)eCtrl->GetLength()) return false;

		eCtrl->SetPos(pos);
		return true;
	}
	
	if (command == wxT("view_getpos")) {
		int pos = eCtrl->GetPos();
		result.Printf(wxT("%d"), pos);
		wxLogDebug(wxT("  returning pos: %d %s"), pos, result.c_str());
		return true;
	}
	
	if (command == wxT("view_getversioncount")) {
		const DocumentWrapper& dw = eCtrl->GetDocument();
		cxLOCKDOC_READ(dw)
			int count = doc.GetVersionCount();
			result.Printf(wxT("%d"), count);
			wxLogDebug(wxT("  returning count: %d %s"), count, result.c_str());
		cxENDLOCK
		return true;
	}
	
	if (command == wxT("view_setversion")) {
		long version_id;
		if (!args.ToLong(&version_id)) return false;
		const DocumentWrapper& dw = eCtrl->GetDocument();
		doc_id di;
		cxLOCKDOC_READ(dw)
			if (version_id < 0 || version_id >= doc.GetVersionCount()) return false;
			di = doc.GetDocument();
		cxENDLOCK
		di.version_id = version_id;
		eCtrl->SetDocument(di);
		return true;
	}

	wxLogDebug(wxT("Execute command: %s "), command.c_str());
	return false;
}

wxString eApp::ExtractPosArgs(const wxString& cmd, unsigned int& lineNum, unsigned int& columnNum) const {
	wxString path = cmd;
	if (path.StartsWith(wxT("LINE="))) {
		wxString lineStr = path.BeforeFirst(wxT(' '));
		lineStr = lineStr.substr(5);
		unsigned long line = 0;
		lineStr.ToULong(&line);
		lineNum = line;
		path = path.AfterFirst(wxT(' '));
	}
	if (path.StartsWith(wxT("COL="))) {
		wxString colStr = path.BeforeFirst(wxT(' '));
		colStr = colStr.substr(5);
		unsigned long col = 0;
		colStr.ToULong(&col);
		columnNum = col;
		path = path.AfterFirst(wxT(' '));
	}
	return path;
}

wxRect eApp::DetermineFrameSize() {
	wxSize scr = wxGetDisplaySize();

	// determine default frame position/size
	wxRect normal;
	if (scr.x <= 700) {
		normal.x = 40 / 2;
		normal.width = scr.x - 40;
	}
	else {
		normal.x = (scr.x - 700) / 2;
		normal.width = 700;
	}
	if (scr.y <= 480) {
		normal.y = 80 / 2;
		normal.height = scr.y - 80;
	}
	else{
		normal.height = scr.y - 120;
		normal.y = (scr.y - normal.height) / 2;
	}

	return normal;
}

int eApp::OnExit() {
	// Release allocated memory
#ifndef __WXMSW__
	if (m_server) delete m_server;
#endif
	if (m_catalyst) delete m_catalyst;
	if (m_pCatalyst) delete m_pCatalyst;
	if (m_checker) delete m_checker;
	if (m_pSyntaxHandler) delete m_pSyntaxHandler;
	if (m_pListHandler) delete m_pListHandler;

#ifdef __WXDEBUG__
	if (blackboxLib.IsLoaded()) blackboxLib.Unload();
#endif  //__WXDEBUG__

	// curl cleanup
	curl_global_cleanup();

	return 0;
}

wxString eApp::GetAppTitle() {
	wxString title;
	if (this->IsRegistered()) title = _("e");
	else {
		int daysleft = this->DaysLeftOfTrial();

		if (daysleft == 1) title = _("e  [UNREGISTERED - 1 DAY LEFT OF TRIAL]");
		else if (daysleft > 1) title = wxString::Format(wxT("e  [UNREGISTERED - %d DAYS LEFT OF TRIAL]"), daysleft);
		else title = _("e  [UNREGISTERED - *TRIAL EXPIRED*]");
	}

#ifdef __WXDEBUG__
	title += wxT(" [DEBUG]");
#endif

	return title;
}

void eApp::CheckForUpdates() {
	// Check if it more than 7 days have gone since last update check
	wxLongLong lastup;
	if (m_settings.GetSettingLong(wxT("lastupdatecheck"), lastup)) {
		wxDateTime sevendaysago = wxDateTime::Now() - wxDateSpan(0, 0, 1, 0);
		wxDateTime lastupdated(lastup);

		if (lastupdated > sevendaysago) return;
	}

	// Check if we have a internet connection
	wxDialUpManager* dup = wxDialUpManager::Create();
	if (dup->IsOnline()) {
		// We have to create the http protocol in main thread
		// it will be deleted in the updater thread
		wxHTTP* http = new wxHTTP;

		// Start the updater thread
		UpdaterThread *updater = new UpdaterThread(http);
		if (updater->Create() == wxTHREAD_NO_ERROR)	updater->Run();
	}
	delete dup; // Clean up
}

void eApp::OnUpdatesAvailable(wxCommandEvent& WXUNUSED(event)) {
	// Ask user if he wants to download new release
	const int answer = wxMessageBox(_("A new release of e is available.\nDo you wish to go the the website to download it now?"), wxT("Program update!"), wxYES_NO|wxICON_EXCLAMATION, frame);
	if (answer == wxYES) {
		// Go to website
		wxLaunchDefaultBrowser(wxT("http://www.e-texteditor.com"));
	}
}

#ifdef __WXDEBUG__
void eApp::OnAssertFailure(const wxChar *file, int line, const wxChar *cond, const wxChar *msg) {
	// We want to insure that catalyst wont commit after an assert
	// to avoid invalid data in the database
	if (m_catalyst) {
		cxLOCK_WRITE((*m_catalyst))
			catalyst.AllowCommit(false);
		cxENDLOCK
	}

	// Close the logfile
	delete wxLog::SetActiveTarget(NULL);
	m_logFile.Close();

#ifdef __WXMSW__
	// Save info about the assert to the registry so that the crash handler can
	// get to it
	wxRegKey regKey(wxT("HKEY_CURRENT_USER\\Software\\e"));
	const wxString error = wxString::Format(wxT("(%s:%d) %s, %s"), file, line, cond, msg);
	regKey.SetValue(wxT("last_assert"), error);
#endif // __WXMSW__

	wxApp::OnAssert(file, line, cond, msg);
}
#endif  //__WXDEBUG__

// --- UpdaterThread -----------------


// Checking for updates is done in a seperate thread since the
// socket can block for some time and we don't want the main thread
// to lock up

void* UpdaterThread::Entry() {
	wxASSERT(m_http);

	const eApp& app = wxGetApp();
	const unsigned int thisVersion = app.VersionId();

	// Set a cookie so the server can count unique requests
	wxString cookie;
	if (app.IsRegistered()) cookie = wxString::Format(wxT("%s.%u.*"), app.GetId().ToString().c_str(), thisVersion);
	else cookie = wxString::Format(wxT("%s.%u.%d"), app.GetId().ToString().c_str(), thisVersion, app.DaysLeftOfTrial());
	m_http->SetHeader(wxT("Cookie"), cookie);

	bool newrelease = false;
	if (m_http->Connect(wxT("www.e-texteditor.com"))) {
		wxInputStream* pStream = m_http->GetInputStream(wxT("/cgi-bin/status"));
		if (pStream) {
			wxTextInputStream text(*pStream);

			while (!pStream->Eof()) {
				wxString line = text.ReadLine();
				if (line.empty()) break;

				if (line.StartsWith(wxT("release"))) {
					wxString id_str = line.AfterFirst(wxT('='));
					id_str = id_str.BeforeFirst(wxT(' '));
					unsigned long id;
					id_str.ToULong(&id);
					if (id > thisVersion) {
						newrelease = true;
						break;
					}
				}
#ifdef __WXDEBUG__
				else if (line.StartsWith(wxT("beta"))) {
					wxString id_str = line.AfterFirst(wxT('='));
					id_str = id_str.BeforeFirst(wxT(' '));
					unsigned long id;
					id_str.ToULong(&id);
					if (id > thisVersion) {
						newrelease = true;
						break;
					}
				}
#endif  //__WXDEBUG__
			}
			delete pStream; // Clean up

			// Remember this time as last update checked
			wxLongLong now = wxDateTime::Now().GetValue();
			eGetSettings().SetSettingLong(wxT("lastupdatecheck"), now);
		}
	}

	// Clean up
	delete m_http;

	if (newrelease) {
		// Notify main thread that there are new updates available
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UPDATES_AVAILABLE);
		wxGetApp().AddPendingEvent(event);
	}

	return NULL;
}

// Gloal method for getting eApp's settings object, without needing to include all of eApp.h
eSettings& eGetSettings(void) {
	return wxGetApp().GetSettings();
}
