#ifndef HESSIAN_IPC_CONNECTION_MANAGER_H
#define HESSIAN_IPC_CONNECTION_MANAGER_H

#include <set>
#include <boost/noncopyable.hpp>
#include "connection.h"

namespace hessian_ipc {

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class connection_manager
  : private boost::noncopyable
{
public:
	void start(connection_ptr c); // Add the specified connection to the manager and start it.
	void stop(connection_ptr c);  // Stop the specified connection.
	void stop_all();              // Stop all connections.

private:
	// Member variables
	std::set<connection_ptr> connections_; // The managed connections.
};

} // namespace hessian_ipc

#endif // HESSIAN_IPC_CONNECTION_MANAGER_HPP
