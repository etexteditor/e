#include "eIpcThread.h"
#include "eApp.h"
#include "IConnection.h"
#include "IIpcServer.h"

DEFINE_EVENT_TYPE(wxEVT_IPC_CALL)

eIpcThread::eIpcThread(eApp& app) : m_ipcServer(NULL), m_app(app) {
	Create();
	Run();
}

void* eIpcThread::Entry() {
	m_ipcServer = NewIpcServer(*this);
	m_ipcServer->run();

	delete m_ipcServer;
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
