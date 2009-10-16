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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/ipc.h>


class IExecuteAppCommand;

#ifdef __WXMSW__
class eIpcWin : public wxFrame {
public:
	eIpcWin(IExecuteAppCommand& app);
	wxString SetMateName();
	bool ShouldPreventAppExit() const {return false;};

private:
	WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
	IExecuteAppCommand& m_app;
};

#else

class eServer : public wxServer {
public:
	eServer(IExecuteAppCommand& app);
    wxConnectionBase *OnAcceptConnection(const wxString& topic);

private:
	IExecuteAppCommand& app;
};

class eConnection : public wxConnection {
public:
	eConnection(IExecuteAppCommand& app);
	~eConnection();

    bool OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format);
    wxChar *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format);
    bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    bool OnStartAdvise(const wxString& topic, const wxString& item);

	void SetPointer(eConnection** pConnection) {m_pConnection = pConnection;};

private:
	IExecuteAppCommand& app;
	eConnection** m_pConnection;
};

#endif

#endif //__EIPCSERVER_H__
