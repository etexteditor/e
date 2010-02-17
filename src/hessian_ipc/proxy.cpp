#include "proxy.h"

namespace hessian_ipc {

proxy::proxy() : m_io_service(), m_socket(m_io_service), m_reply(8192) {
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(m_io_service);
    tcp::resolver::query query("localhost", "9000");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      m_socket.close();
      m_socket.connect(*endpoint_iterator++, error);
    }
    if (error) throw boost::system::system_error(error);
}

const Value& proxy::do_hessian_call(const vector<unsigned char>& call) {
    // Send the request.
    boost::asio::write(m_socket, boost::asio::buffer(call));

    // Read the response
	m_reader.Reset();
	for (;;) {
		const size_t n = m_socket.read_some(boost::asio::buffer(m_reply));

		vector<unsigned char>::const_iterator begin = m_reply.begin();
		vector<unsigned char>::const_iterator end = m_reply.begin() + n;

		if (m_reader.Parse(begin, end)) break; // request complete
	}

	return *m_reader.GetResultValue();
}

const Value& proxy::call(const string& method) {
	m_writer.call(method);
	const vector<unsigned char>& out = m_writer.GetOutput();

	return do_hessian_call(out);
}

} //namespace hessian_ipc
