#ifndef HESSIAN_IPC_SERVER_HPP
#define HESSIAN_IPC_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "connection.h"
#include "connection_manager.h"

namespace hessian_ipc {

// The top-level class of the ipc server.
class server
: private boost::noncopyable
{
public:
	// Construct the server to listen on the specified TCP address and port
	explicit server(const std::string& address, const std::string& port);
	virtual ~server() {};

	void run();  // Run the server's io_service loop.
	virtual void stop(); // Stop the server.

	virtual connection* new_connection(); // create new connection

protected:
	// Member variables
	boost::asio::io_service io_service_;      // The io_service used to perform asynchronous operations.
	connection_manager connection_manager_;   // The connection manager which owns all live connections.

private:
	// Handle completion of an asynchronous accept operation.
	void handle_accept(const boost::system::error_code& e);

	// Handle a request to stop the server.
	void handle_stop();

	// Member variables
	boost::asio::ip::tcp::acceptor acceptor_; // Acceptor used to listen for incoming connections.
	connection_ptr new_connection_;           // The next connection to be accepted.
};

} // namespace hessian_ipc

#endif // HESSIAN_IPC_SERVER_HPP
