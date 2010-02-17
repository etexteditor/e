#include <boost/bind.hpp>
#include "connection.h"
#include "connection_manager.h"


namespace hessian_ipc {

connection::connection(boost::asio::io_service& io_service, connection_manager& manager)
  : io_service_(io_service), socket_(io_service), connection_manager_(manager), request_(NULL)
{
}

connection::~connection() {
}

boost::asio::ip::tcp::socket& connection::socket() {
	return socket_;
}

void connection::start() {
	buffer_.resize(8192); // initial buffer size

	// Read first request
	socket_.async_read_some(boost::asio::buffer(buffer_),
	  boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void connection::stop() {
	socket_.close();
}

void connection::invoke_method() {
	throw hessian_ipc::value_exception("Unknown method");
}

void connection::handle_read(const boost::system::error_code& e, size_t bytes_transferred) {
	if (!e) {
		try {
			vector<unsigned char>::const_iterator begin = buffer_.begin();
			vector<unsigned char>::const_iterator end = buffer_.begin() + bytes_transferred;
			
			// Parse the request
			if (reader_.Parse(begin, end)) {
				// Invoke handler
				request_ = reader_.GetResultCall();
				invoke_method();
			}

			socket_.async_read_some(boost::asio::buffer(buffer_),
			  boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		catch (hessian_ipc::value_exception& e) {
			protocol_error(e.what()); // Send fault
		}
		catch (exception& e) {
			method_error(e.what()); // Send fault
		}
	}
	else if (e != boost::asio::error::operation_aborted) {
		connection_manager_.stop(shared_from_this());
	}
}

void connection::reply_done() {
	// notify connection that it can send the reply (threadsafe)
	io_service_.post(boost::bind(&connection::send_reply, this));
}

void connection::send_reply() {
	// Prepare for reading next request
	reader_.Reset();
	request_ = NULL;

	// Send the reply
	const vector<unsigned char>& reply = writer_.GetOutput();
	boost::asio::async_write(socket_, boost::asio::buffer(reply),
	  boost::bind(&connection::handle_write, shared_from_this(),
		boost::asio::placeholders::error));
}

void connection::protocol_error(const string& msg) {
	writer_.write_fault(hessian_ipc::ProtocolException, msg);
	const vector<unsigned char>& reply = writer_.GetOutput();

	// Send reply and close
	boost::asio::async_write(socket_, boost::asio::buffer(reply),
      boost::bind(&connection::handle_write_and_close, shared_from_this(),
        boost::asio::placeholders::error));
}

void connection::method_error(const string& msg) {
	writer_.write_fault(hessian_ipc::NoSuchMethodException, msg);
	const vector<unsigned char>& reply = writer_.GetOutput();

	// Send reply
	boost::asio::async_write(socket_, boost::asio::buffer(reply),
      boost::bind(&connection::handle_write, shared_from_this(),
        boost::asio::placeholders::error));
}

void connection::service_error(const string& msg) {
	writer_.write_fault(hessian_ipc::ServiceException, msg);
	const vector<unsigned char>& reply = writer_.GetOutput();

	// Send reply
	boost::asio::async_write(socket_, boost::asio::buffer(reply),
      boost::bind(&connection::handle_write, shared_from_this(),
        boost::asio::placeholders::error));
}

void connection::handle_write(const boost::system::error_code& e) {
	if (!e) {
		// Read next request
		socket_.async_read_some(boost::asio::buffer(buffer_),
			  boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else if (e != boost::asio::error::operation_aborted) {
		connection_manager_.stop(shared_from_this());
	}
}

void connection::handle_write_and_close(const boost::system::error_code& e) {
	if (!e)	{
		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}

	if (e != boost::asio::error::operation_aborted)	{
		connection_manager_.stop(shared_from_this());
	}
}

int connection::get_parameter_int(size_t pos) {
	if (pos >= request_->GetParameterCount()) throw hessian_ipc::value_exception("Wrong number of arguments.");
	const hessian_ipc::Value& arg = request_->GetParameter(pos);
	return arg.GetInt();
}

} // namespace hessian_ipc
