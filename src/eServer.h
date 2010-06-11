#ifndef __ESERVER_H__
#define __ESERVER_H__

#include "IIpcServer.h"
#include "hessian_ipc/server.h"

// Pre-definitions
class IIpcHandler;

class eServer : public hessian_ipc::server, public IIpcServer {
public:
	explicit eServer(IIpcHandler& handler);
	virtual ~eServer() {};

	virtual void run();
	virtual void stop();
	virtual void destroy();

	hessian_ipc::connection* new_connection();

private:
	IIpcHandler& m_handler;
};

#endif // __ESERVER_H__