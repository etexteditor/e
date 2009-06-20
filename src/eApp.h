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

#ifndef __EAPP_H__
#define __EAPP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/snglinst.h>
#include <wx/dynlib.h>

#ifdef __WXDEBUG__
	#include <wx/ffile.h>
#endif

#include "Catalyst.h"
#include "mk4.h"
#include "eIpcServer.h"
#include "eSettings.h"
#include "IAppPaths.h"
#include "IExecuteAppCommand.h"

// pre-declearations
class TmSyntaxHandler;
class PListHandler;
class EditorFrame;

// Constants
#define ID_UPDATES_AVAILABLE 100


class eApp : public wxApp, 
	public IGetSettings, 
	public IAppPaths, 
	public IExecuteAppCommand
{
public:
	virtual bool OnInit();
	int OnExit();

	const wxString& VersionName() const { return m_version_name; }
	const unsigned int VersionId() const { return m_version_id; }

	// Execute internal commands
	virtual bool ExecuteCmd(const wxString& cmd);
	virtual bool ExecuteCmd(const wxString& cmd, wxString& result);

	virtual const wxString& GetAppPath() const {return m_appPath;};
	virtual const wxString& GetAppDataPath() const {return m_appDataPath;};

	// Settings functions
	const wxLongLong& GetId() const {cxLOCK_READ((*m_catalyst)) return catalyst.GetId(); cxENDLOCK};
	virtual eSettings& GetSettings(){return m_settings;};

	wxString GetAppTitle();

	// Registration functions
	bool IsRegistered() const {return m_pCatalyst->IsRegistered();};
	int DaysLeftOfTrial() const {return m_pCatalyst->DaysLeftOfTrial();};
	int TotalDays() const {return m_pCatalyst->DaysLeftOfTrial();};
	const wxString& RegisteredUserName() const {return m_pCatalyst->RegisteredUserName();};
	const wxString& RegisteredUserEmail() const {return m_pCatalyst->RegisteredUserEmail();};

    // Suppress default console handling even if wxWidgets was compiled with --enable-cmdline
#if wxUSE_CMDLINE_PARSER
    virtual bool OnCmdLineParsed(wxCmdLineParser&) {return true;}
    virtual bool OnCmdLineHelp(wxCmdLineParser&) {return true;}
    virtual bool OnCmdLineError(wxCmdLineParser&) {return true;}
#endif // wxUSE_CMDLINE_PARSER
        
private:
	// Member variables
	EditorFrame* frame;
	wxString m_version_name;
	unsigned int m_version_id;

public:

#ifdef __WXDEBUG__
	void OnAssertFailure(const wxChar *file, int line, const wxChar *cond, const wxChar *msg);
#endif  //__WXDEBUG__

private:
	// Member functions
	void ClearState();
	void ClearLayout();
	wxRect DetermineFrameSize();

	bool SendArgsToInstance();
	wxString ExtractPosArgs(const wxString& cmd, unsigned int& lineNum, unsigned int& columnNum) const;

#ifdef __WXMSW__
	void SendCommandToServer(HWND hWndRecv, const wxString& cmd);
#endif


#ifdef __WXMSW__
	// Callback for ASProtect to set days left
	static void __declspec(dllexport) __stdcall GetTrialDays(int Total, int Left);
#endif

	// Event handlers
	void OnUpdatesAvailable(wxCommandEvent& event);
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	wxDynamicLibrary blackboxLib;
	wxSingleInstanceChecker* m_checker;
	Catalyst* m_pCatalyst;
	CatalystWrapper* m_catalyst;
	TmSyntaxHandler* m_pSyntaxHandler;
	PListHandler* m_pListHandler;
	wxString m_appPath;
	wxString m_appDataPath;
	wxArrayString m_openStack;

#ifndef __WXMSW__
	eServer* m_server;
#endif

	// Option vars
	wxArrayString m_files;
	unsigned long m_lineNum;
	unsigned long m_columnNum;

	// Settings
	eSettings m_settings;

#ifdef __WXDEBUG__
	wxFFile m_logFile;
#endif //__WXDEBUG__
};


DECLARE_APP(eApp)

#ifndef __WXMSW__
class eClient: public wxClient {
public:
	eClient(eApp& app) : app(app) {}
	wxConnectionBase *OnMakeConnection(void) { return new eConnection(app); }

private:
	eApp& app;
};
#endif


#endif // __EAPP_H__
