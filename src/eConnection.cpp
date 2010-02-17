#include "eConnection.h"
#include "IIpcHandler.h"

eConnection::eConnection(boost::asio::io_service& io_service, hessian_ipc::connection_manager& manager, IIpcHandler& handler)
: hessian_ipc::connection(io_service, manager), m_handler(handler)
{
}

eConnection::~eConnection() {
}

void eConnection::invoke_method() {
	m_handler.handle_call(*this);
}

const hessian_ipc::Call* eConnection::get_call() {
	return request_;
}

hessian_ipc::Writer& eConnection::get_reply_writer() {
	return writer_;
}

void eConnection::reply_done() {
	connection::reply_done();
}