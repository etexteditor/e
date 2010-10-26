#ifndef __IIPCSERVER_H__
#define __IIPCSERVER_H__

// wxWidgets and boost asio uses diffent versions of windows includes.
// So to avoid conflicts these interfaces are used to totally decouple
// the two modules.

#include "IIpcHandler.h"

class IIpcServer {
public:
	virtual void run() = 0;
	virtual void stop() = 0;
	virtual void destroy() = 0;
};

IIpcServer* NewIpcServer(IIpcHandler& handler);

#endif // __IIPCSERVER_H__