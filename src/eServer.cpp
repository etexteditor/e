#include "eServer.h"
#include "eConnection.h"

eServer::eServer(IIpcHandler& handler)
: hessian_ipc::server("localhost", "9000"), m_handler(handler)
{
}

void eServer::run() {
	server::run();
}

void eServer::stop() {
	server::stop();
}

hessian_ipc::connection* eServer::new_connection() {
	return new eConnection(io_service_, connection_manager_, m_handler);
}

IIpcServer* NewIpcServer(IIpcHandler& handler) {
	return new eServer(handler);
}