/*******************************************************************************
 *
 * Copyright (C) 2010, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __ECONNECTION_H__
#define __ECONNECTION_H__

#include "hessian_ipc/connection.h"
#include "IConnection.h"

// Pre-definitions
class IIpcHandler;

class eConnection : public hessian_ipc::connection, public IConnection {
public:
	explicit eConnection(boost::asio::io_service& io_service, hessian_ipc::connection_manager& manager, IIpcHandler& handler);
	virtual ~eConnection();

	// Method handling
	void invoke_method();

	// Interface methods
	virtual const hessian_ipc::Call* get_call(); // The request recieved (may be NULL)
	virtual hessian_ipc::Writer& get_reply_writer();
	virtual void reply_done(); // notify connection that it can send the reply (threadsafe)

private:
	// Member variables
	IIpcHandler& m_handler;
};


#endif //__ECONNECTION_H__
