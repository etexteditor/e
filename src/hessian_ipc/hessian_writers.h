#ifndef HESSIAN_WRITERS_H
#define HESSIAN_WRITERS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "hessian_values.h"

namespace hessian_ipc {

// template implementations

template<class T> void Writer::call(const string& method, const T& arg) {
	Reset();

	out.push_back('C');
	write(method);
	write(1);
	write(arg);
}

template<class T1, class T2> void Writer::call(const string& method, const T1& arg1, const T2& arg2) {
	Reset();

	out.push_back('C');
	write(method);
	write(2);
	write(arg1);
	write(arg2);
}

template<class T> void Writer::write_reply(const T& value) {
	Reset();

	out.push_back('R');
	write(value);
}

template<class T> void Writer::write(const vector<T>& value) {
	const size_t len = value.size();

	out += x57; // tag for variable-length untyped list
	for (vector<T>::const_iterator p = value.begin(); p != value.end(); ++p) {
		write(*p);
	}
	out.push_back('Z');
}

template<class T1, class T2> void Writer::write(const map<T1,T2>& value) {
	const size_t len = value.size();

	out.push_back('H'); // tag for untyped map
	for (map<T1,T2>::const_iterator p = value.begin(); p != value.end(); ++p) {
		write(*(p->first));
		write(*(p->second));
	}
	out.push_back('Z');
}

} // namespace hessian_ipc

#endif //HESSIAN_WRITERS_H
