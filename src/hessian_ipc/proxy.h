#ifndef HESSIAN_IPC_PROXY_H
#define HESSIAN_IPC_PROXY_H

#include <boost/asio.hpp>
#include "hessian_reader.h"

using boost::asio::ip::tcp;

namespace hessian_ipc {

class proxy {
public:
	proxy();

	const Value& call(const string& method);
	template<class T> const Value& call(const string& method, const T& arg);

	template<class T> int call_int(const string& method, const T& arg);
	template<class T> long long call_long(const string& method, const T& arg);
	template<class T> std::string call_string(const string& method, const T& arg);

private:
	const Value& do_hessian_call(const vector<unsigned char>& call);

	// Member variables
	boost::asio::io_service m_io_service;
	tcp::socket m_socket;
	std::vector<unsigned char> m_reply;
	Reader m_reader;
	Writer m_writer;
};

// template implementations

template<class T> const Value& proxy::call(const string& method, const T& arg) {
	m_writer.call(method, arg);
	const string& out = m_writer.GetOutput();

	return do_hessian_call(vector<unsigned char>(out.begin(), out.end()));
}

template<class T> int proxy::call_int(const string& method, const T& arg) {
	const Value& response = call(method, arg);
	return response.GetInt();
}

template<class T> long long proxy::call_long(const string& method, const T& arg) {
	const Value& response = call(method, arg);
	return response.GetLong();
}

template<class T> std::string proxy::call_string(const string& method, const T& arg) {
	const Value& response = call(method, arg);
	return response.GetString();
}

} // namespace hessian_ipc

#endif //HESSIAN_IPC_PROXY_H
