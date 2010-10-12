#ifndef __IIPCHANDLER_H__
#define __IIPCHANDLER_H__

// wxWidgets and boost asio uses diffent versions of windows includes.
// So to avoid conflicts these interfaces are used to totally decouple
// the two modules.

//Pre-definitions
class IConnection;

class IIpcHandler {
public:
	virtual void handle_call(IConnection& conn) = 0;
	virtual void handle_close(IConnection& conn) = 0;
};


#endif // __IIPCHANDLER_H__