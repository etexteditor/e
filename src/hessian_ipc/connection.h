#ifndef HESSIAN_IPC_CONNECTION_H
#define HESSIAN_IPC_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include "hessian_reader.h"

namespace hessian_ipc {

class connection_manager;

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection>,
    private boost::noncopyable
{
public:
	// Construct a connection with the given io_service.
	explicit connection(boost::asio::io_service& io_service, connection_manager& manager);
	virtual ~connection();

	// Get the socket associated with the connection.
	boost::asio::ip::tcp::socket& socket();

	void start(); // Start the first asynchronous operation for the connection.
	void stop();  // Stop all asynchronous operations associated with the connection.

	void reply_done();

protected:
	// Method handling
	virtual void invoke_method();

	// Functions used by method handlers
	int get_parameter_int(size_t pos);
	template<class T> void reply(const T& reply) {
		writer_.write_reply(reply);
	}

	// Message handling
	const hessian_ipc::Call* request_; // The request recieved (may be NULL)
	hessian_ipc::Reader reader_;
	hessian_ipc::Writer writer_;

private:
	// Handle completion of read operations
	void handle_read(const boost::system::error_code& e, size_t bytes_transferred);
	void send_reply();

	// Handle completion of write operations
	void handle_write(const boost::system::error_code& e);
	void handle_write_and_close(const boost::system::error_code& e);

	// Handle errors
	void protocol_error(const string& msg);
	void method_error(const string& msg);
	void service_error(const string& msg);

	// Member variables
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;    // Socket for the connection.
	connection_manager& connection_manager_; // The manager for this connection.
	std::vector<unsigned char> buffer_;      // Buffer for incoming data.
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace hessian_ipc

#endif // HESSIAN_IPC_CONNECTION_H
