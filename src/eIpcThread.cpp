#include "eIpcThread.h"
#include "eApp.h"
#include "IConnection.h"
#include "IIpcServer.h"

DEFINE_EVENT_TYPE(wxEVT_IPC_CALL)
DEFINE_EVENT_TYPE(wxEVT_IPC_CLOSE)

eIpcThread::eIpcThread(wxEvtHandler& app) : m_ipcServer(NULL), m_app(app) {
	Create();
	Run();
}

void* eIpcThread::Entry() {
	m_ipcServer = NewIpcServer(*this);
	m_ipcServer->run();

	m_ipcServer->destroy(); // will clean up after itself
	return NULL;
}

void eIpcThread::stop() {
	// Threadsafe stop of server
	if (m_ipcServer) m_ipcServer->stop();
}

void eIpcThread::handle_call(IConnection& conn) {
	wxCommandEvent event(wxEVT_IPC_CALL, wxID_ANY);
	event.SetClientData(&conn);

	// Notify app that there is a new call (threadsafe)
	m_app.AddPendingEvent(event);
}

void eIpcThread::handle_close(IConnection& conn) {
	wxCommandEvent event(wxEVT_IPC_CLOSE, wxID_ANY);
	event.SetClientData(&conn);

	// Notify app that there is a new call (threadsafe)
	m_app.AddPendingEvent(event);
}
