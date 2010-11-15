#include "eConnection.h"
#include "IIpcHandler.h"

eIpcConnection::eIpcConnection(boost::asio::io_service& io_service, hessian_ipc::connection_manager& manager, IIpcHandler& handler)
: hessian_ipc::connection(io_service, manager), m_handler(handler)
{
}

eIpcConnection::~eIpcConnection() {
}

void eIpcConnection::invoke_method() {
	m_handler.handle_call(*this);
}

void eIpcConnection::on_close() {
	m_handler.handle_close(*this);
}

const hessian_ipc::Call* eIpcConnection::get_call() {
	return request_;
}

hessian_ipc::Writer& eIpcConnection::get_reply_writer() {
	return writer_;
}

void eIpcConnection::reply_done() {
	connection::reply_done();
}

void eIpcConnection::notifier_done() {
	connection::notifier_done();
}
