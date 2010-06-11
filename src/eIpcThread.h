/*******************************************************************************
 *
 * Copyright (C) 2010, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __EIPCTHREAD_H__
#define __EIPCTHREAD_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "IIpcHandler.h"

// Pre-definitions
class eApp;
class IConnections;
class IIpcServer;

DECLARE_EVENT_TYPE(wxEVT_IPC_CALL, -1)
DECLARE_EVENT_TYPE(wxEVT_IPC_CLOSE, -1)

class eIpcThread : public wxThread, public IIpcHandler {
public:
	eIpcThread(eApp& app);
	virtual void* Entry();

	void stop(); // Threadsafe stop of server

	void handle_call(IConnection& conn);
	void handle_close(IConnection& conn);

private:
	IIpcServer* m_ipcServer;
	eApp& m_app;
};

#endif //__EIPCTHREAD_H__