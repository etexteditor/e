#ifndef __ICONNECTION_H__
#define __ICONNECTION_H__

// wxWidgets and boost asio uses diffent versions of windows includes.
// So to avoid conflicts these interfaces are used to totally decouple
// the two modules.

#include "hessian_ipc/hessian_values.h"

class IConnection {
public:
	virtual const hessian_ipc::Call* get_call() = 0; // The request recieved (may be NULL)
	virtual hessian_ipc::Writer& get_reply_writer() = 0;
	virtual void reply_done() = 0;
	virtual void notifier_done() = 0;
};


#endif // __ICONNECTION_H__