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

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include <wx/snglinst.h>
#include <wx/dynlib.h>
#include "mk4.h"
#include "eIpcServer.h"
#include <wx/ffile.h>

// pre-declearations
class eApp;
//#include "tm_syntaxhandler.h"
class TmSyntaxHandler;
class EditorFrame;

// Constants
#define ID_UPDATES_AVAILABLE 100


class eApp : public wxApp
{
public:
	virtual bool OnInit();
	int OnExit();

	// Member functions
	bool ExecuteCmd(const wxString& cmd);
	bool ExecuteCmd(const wxString& cmd, wxString& result);
	TmSyntaxHandler& GetSyntaxHandler() {return *m_pSyntaxHandler;};

	const wxString& GetAppPath() const {return m_appPath;};
	const wxString& GetAppDataPath() const {return m_appDataPath;};

	// Settings functions
	const wxLongLong& GetId() const {cxLOCK_READ((*m_catalyst)) return catalyst.GetId(); cxENDLOCK};
	int GetPageCount() const {cxLOCK_READ((*m_catalyst)) return catalyst.GetPageCount(); cxENDLOCK};
	void DeletePageSettings(int page_id) {cxLOCK_WRITE((*m_catalyst)) catalyst.DeletePageSettings(page_id); cxENDLOCK};
	void SetSettingBool(const wxString& name, bool value) {cxLOCK_WRITE((*m_catalyst)) catalyst.SetSettingBool(name, value); cxENDLOCK};
	void SetSettingInt(const wxString& name, int value) {cxLOCK_WRITE((*m_catalyst)) catalyst.SetSettingInt(name, value); cxENDLOCK};
	void SetSettingLong(const wxString& name, wxLongLong value) {cxLOCK_WRITE((*m_catalyst)) catalyst.SetSettingLong(name, value); cxENDLOCK};
	void SetSettingString(const wxString& name, const wxString& value) {cxLOCK_WRITE((*m_catalyst)) catalyst.SetSettingString(name, value); cxENDLOCK};
	bool GetSettingBool(const wxString& name, bool& value) const {cxLOCK_READ((*m_catalyst)) return catalyst.GetSettingBool(name, value); cxENDLOCK};
	bool GetSettingInt(const wxString& name, int& value) const {cxLOCK_READ((*m_catalyst)) return catalyst.GetSettingInt(name, value); cxENDLOCK};
	bool GetSettingLong(const wxString& name, wxLongLong& value) const {cxLOCK_READ((*m_catalyst)) return catalyst.GetSettingLong(name, value); cxENDLOCK};
	bool GetSettingString(const wxString& name, wxString& value) const {cxLOCK_READ((*m_catalyst)) return catalyst.GetSettingString(name, value); cxENDLOCK};

	// Registration functions
	bool IsRegistered() const {return m_pCatalyst->IsRegistered();};
	int DaysLeftOfTrial() const {return m_pCatalyst->DaysLeftOfTrial();};
	int TotalDays() const {return m_pCatalyst->DaysLeftOfTrial();};
	const wxString& RegisteredUserName() const {return m_pCatalyst->RegisteredUserName();};
	const wxString& RegisteredUserEmail() const {return m_pCatalyst->RegisteredUserEmail();};

#ifdef __WXMSW__
	// Entry point to Cygwin installation on Windows
	bool InitCygwin(bool silent=false);
#endif

    // Suppress default console handling even if wxWidgets was compiled with --enable-cmdline
#if wxUSE_CMDLINE_PARSER
    virtual bool OnCmdLineParsed(wxCmdLineParser&) {return true;}
    virtual bool OnCmdLineHelp(wxCmdLineParser&) {return true;}
    virtual bool OnCmdLineError(wxCmdLineParser&) {return true;}
#endif // wxUSE_CMDLINE_PARSER
        
	// Member variables
	EditorFrame* frame;
	wxString m_version_name;
	unsigned int m_version_id;

#ifdef __WXDEBUG__
	void OnAssertFailure(const wxChar *file, int line, const wxChar *cond, const wxChar *msg);
#endif  //__WXDEBUG__

private:
	// Member functions
	void ClearState();
	void ClearLayout();
	wxRect DetermineFrameSize();
	void CheckForUpdates();

	bool SendArgsToInstance();
	wxString ExtractPosArgs(const wxString& cmd, unsigned int& lineNum, unsigned int& columnNum) const;

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
	c4_Storage* m_settings;
	c4_View m_vPages;
	c4_View m_v64hash;
	c4_View m_vInthash;
	c4_View m_vStringhash;

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

class wxHTTP; // pre-declaration

class UpdaterThread : public wxThread {
public:
	UpdaterThread(wxHTTP* http) : m_http(http) {};
	virtual void *Entry();

private:
	wxHTTP* m_http;
};

#endif // __EAPP_H__
