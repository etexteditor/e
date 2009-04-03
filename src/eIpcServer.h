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

#ifndef __EIPCSERVER_H__
#define __EIPCSERVER_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/ipc.h>

// pre-definitions
class eApp;

#ifdef __WXMSW__
class eIpcWin : public wxFrame {
public:
	eIpcWin(eApp& app);

	wxString SetMateName();

	bool ShouldPreventAppExit() const {return false;};
private:
	WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
	eApp& m_app;
};

#else

class eServer : public wxServer {
public:
	eServer(eApp& app);
    wxConnectionBase *OnAcceptConnection(const wxString& topic);

private:
	eApp& app;
};

class eConnection : public wxConnection {
public:
	eConnection(eApp& app);
	~eConnection();

    bool OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format);
    wxChar *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format);
    bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    bool OnStartAdvise(const wxString& topic, const wxString& item);

	void SetPointer(eConnection** pConnection) {m_pConnection = pConnection;};

private:
	eApp& app;
	eConnection** m_pConnection;
};

#endif

#endif //__EIPCSERVER_H__
