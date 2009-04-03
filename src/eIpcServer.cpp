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

#include "eIpcServer.h"
#include "eApp.h"

#ifdef __WXMSW__

// ---- eIpcWin -----------------------------------

eIpcWin::eIpcWin(eApp& app)
: wxFrame(NULL, wxID_ANY, wxT("eIpcWin"), wxPoint(-100,-100)), m_app(app) {
}

wxString eIpcWin::SetMateName() {
	unsigned int count = 1;
	while (1) {
		const wxString name = wxString::Format(wxT("emateWin%u"), count);

		// Check if the window name is free to use
		HWND hWndRecv = ::FindWindow(wxT("wxWindowClassNR"), name);
		if (!hWndRecv) {
			SetLabel(name);
			return name;
		}
		else ++count;
	}

	wxASSERT(false); // we should never end here
	return wxEmptyString;
}

WXLRESULT eIpcWin::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) {
	if (nMsg == WM_COPYDATA) {
		COPYDATASTRUCT* pData = (COPYDATASTRUCT*)lParam;
		const wxString cmd((const char*)pData->lpData, wxConvUTF8, pData->cbData);

		wxLogDebug(wxT("Received WM_COPYDATA: %s"), cmd);

		if (cmd.StartsWith(wxT("OPEN_FILE"))) {
			m_app.ExecuteCmd(cmd);
		}
		else if (cmd == wxT("NEW_WINDOW")) {
			m_app.ExecuteCmd(cmd);
		}
		else if (cmd == wxT("DONE")) {
			// We have been called as a mate and the editing is done
			Destroy(); // Closes app (this is only top-level win)
		}
		return 1; // msg processed
	}

	// Let the window do it's own message handling
	return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
}

#else

// ---- eServer -----------------------------------

eServer::eServer(eApp& app) : app(app) {
}

wxConnectionBase* eServer::OnAcceptConnection(const wxString& topic)
{
    if ( topic == _T("e") )
        return new eConnection(app);

    // unknown topic
    return NULL;
}

eConnection::eConnection(eApp& app)
: app(app), m_pConnection(NULL) {
}

eConnection::~eConnection() {
	// OnDisconnect deletes itself, so if possible
	// we want to zero the refering pointer so
	// that we do not get accessed after deletion.
	if (m_pConnection) *m_pConnection = NULL;
}

bool eConnection::OnExecute(const wxString& WXUNUSED(topic),
                             wxChar *data,
                             int WXUNUSED(size),
                             wxIPCFormat WXUNUSED(format))
{
    wxString fullcommand(data);
	return app.ExecuteCmd(fullcommand);
}

bool eConnection::OnPoke(const wxString& WXUNUSED(topic),
                          const wxString& item,
                          wxChar *data,
                          int WXUNUSED(size),
                          wxIPCFormat WXUNUSED(format))
{
    wxLogStatus(wxT("Poke command: %s = %s"), item.c_str(), data);
    return TRUE;
}

wxChar* eConnection::OnRequest(const wxString& WXUNUSED(topic),
                              const wxString& item,
                              int * size,
                              wxIPCFormat WXUNUSED(format))
{
	wxString result;
	if (app.ExecuteCmd(item, result)) {
		// WARNING: This creates a memory leak
		// TODO: Find another way to return a persistent string
		wxString* persist_result = new wxString(result);
		*size = result.size()+1;

		return (wxChar*)persist_result->c_str();
	}
	else return NULL;
}

bool eConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
                                 const wxString& WXUNUSED(item))
{
    return TRUE;
}

#endif
