#include "hessian_values.h"

namespace hessian_ipc {

// calculate numbers of characters in string
size_t utf8_len(const std::string& str) {
	const size_t src_len = str.length();
	size_t len = 0;
	size_t pos = 0;

	while (pos < src_len) {
		const unsigned char c = str[pos];
		if ((c & 0x80) == 0x00) ++pos;
		else if ((c & 0xE0) == 0xC0) pos += 2;
		else if ((c & 0xF0) == 0xE0) pos += 3;
		else if ((c & 0xF8) == 0xF0) pos += 4;
		else throw value_exception("invalid utf8 in string");
		++len;
	}
	if (pos != src_len) throw value_exception("invalid utf8 in string");

	return len;
}

// find byte position of specific character in string
size_t utf8_pos(const std::string& str, size_t pos, size_t offset=0) {
	const size_t src_len = str.length() - offset;
	size_t len = 0;
	size_t p = offset;

	while (len < pos && p < src_len) {
		const unsigned char c = str[p];
		if ((c & 0x80) == 0x00) ++p;
		else if ((c & 0xE0) == 0xC0) p += 2;
		else if ((c & 0xF0) == 0xE0) p += 3;
		else if ((c & 0xF8) == 0xF0) p += 4;
		else throw value_exception("invalid utf8 in string");
		++len;
	}
	if (len != pos) throw value_exception("invalid utf8 in string");

	return p - offset;
}

void Writer::Reset() {
	out.clear();
	objectMap.clear();
}

void Writer::write_null() {
	out.push_back('N');
}

// write boolean
void Writer::write(bool value) {
	out.push_back(value ? 'T' : 'F');
}

// write integer
void Writer::write(int value) {
	if (-0x10 <= value && value <= 0x2f) {
		// pack in single octet
		const unsigned char b8 = 0x90 + (unsigned char)value;
		out.push_back(b8);
	}
	else if (-0x800 <= value && value <= 0x7ff) {
		// pack in two octets
		const unsigned char b16 = 0xc8 + ((value >> 8) & 0x000000FF);
		const unsigned char b8 = value & 0x000000FF;

		out.push_back(b16);
		out.push_back(b8);
	}
	else if (-0x40000 <= value && value <= 0x3ffff) {
		// pack in three octets
		const unsigned char b24 = 0xd4 + ((value >> 16) & 0x000000FF);
		const unsigned char b16 = (value >> 8) & 0x000000FF;
		const unsigned char b8 = value & 0x000000FF;

		out.push_back(b24);
		out.push_back(b16);
		out.push_back(b8);
	}
	else {
		const unsigned char b32 = value >> 24;
		const unsigned char b24 = (value >> 16) & 0x000000FF;
		const unsigned char b16 = (value >> 8) & 0x000000FF;
		const unsigned char b8 = value & 0x000000FF;

		out.push_back('I');
		out.push_back(b32);
		out.push_back(b24);
		out.push_back(b16);
		out.push_back(b8);
	}
}

// write unsigned integer
void Writer::write(unsigned int value) {
	write((int)value);
}

// write 64bit integer
void Writer::write(long long value) {
	if (-0x08 <= value && value <= 0x0f) {
		// pack in single octet
		const unsigned char b8 = 0xe0 + (unsigned char)value;
		out.push_back(b8);
    }
    else if (-0x800 <= value && value <= 0x7ff) {
		// pack in two octets
		const unsigned char b16 = 0xf8 + ((value >> 8) & 0xFF);
		const unsigned char b8 = value & 0xFF;

		out.push_back(b16);
		out.push_back(b8);
    }
    else if (-0x40000 <= value && value <= 0x3ffff) {
		// pack in three octets
		const unsigned char b24 = 0x3c + ((value >> 16) & 0xFF);
		const unsigned char b16 = (value >> 8) & 0xFF;
		const unsigned char b8 = value & 0xFF;

		out.push_back(b24);
		out.push_back(b16);
		out.push_back(b8);
    }
    else if (-0x80000000LL <= value && value <= 0x7fffffffLL) {
		// tag + four octets
		const unsigned char b32 = (value >> 24) & 0xFF;
		const unsigned char b24 = (value >> 16) & 0xFF;
		const unsigned char b16 = (value >> 8) & 0xFF;
		const unsigned char b8 = value & 0xFF;

		out.push_back(0x59); // tag
		out.push_back(b32);
		out.push_back(b24);
		out.push_back(b16);
		out.push_back(b8);
    }
    else {
		// tag + eight octets
		const unsigned char b64 = (value >> 56) & 0xFF;
		const unsigned char b56 = (value >> 48) & 0xFF;
		const unsigned char b48 = (value >> 40) & 0xFF;
		const unsigned char b40 = (value >> 32) & 0xFF;
		const unsigned char b32 = (value >> 24) & 0xFF;
		const unsigned char b24 = (value >> 16) & 0xFF;
		const unsigned char b16 = (value >> 8) & 0xFF;
		const unsigned char b8 = value & 0xFF;

		out.push_back('L'); // tag
		out.push_back(b64);
		out.push_back(b56);
		out.push_back(b48);
		out.push_back(b40);
		out.push_back(b32);
		out.push_back(b24);
		out.push_back(b16);
		out.push_back(b8);
    }
}

// write date
void Writer::write_date(time_t value) {
	// Check if we can encode it as 32-bit minute UTC date
	if (value % 60000LL == 0) {
		const time_t minutes = value / 60000LL;

		if ((minutes >> 31) == 0 || (minutes >> 31) == -1) {
			// tag + four octets
			const unsigned char b32 = (minutes >> 24) & 0xFF;;
			const unsigned char b24 = (minutes >> 16) & 0xFF;
			const unsigned char b16 = (minutes >> 8) & 0xFF;
			const unsigned char b8 = minutes & 0xFF;

			out.push_back(0x4b); // tag
			out.push_back(b32);
			out.push_back(b24);
			out.push_back(b16);
			out.push_back(b8);
			return;
		}
	}

	// Encode as 64-bit millisecond UTC date
	//   tag + eight octets
	const unsigned char b64 = (value >> 56) & 0xFF;
	const unsigned char b56 = (value >> 48) & 0xFF;
	const unsigned char b48 = (value >> 40) & 0xFF;
	const unsigned char b40 = (value >> 32) & 0xFF;
	const unsigned char b32 = (value >> 24) & 0xFF;
	const unsigned char b24 = (value >> 16) & 0xFF;
	const unsigned char b16 = (value >> 8) & 0xFF;
	const unsigned char b8 = value & 0xFF;

	out.push_back(0x4a); // tag
	out.push_back(b64);
	out.push_back(b56);
	out.push_back(b48);
	out.push_back(b40);
	out.push_back(b32);
	out.push_back(b24);
	out.push_back(b16);
	out.push_back(b8);
}

// write string
void Writer::write(const string& value) {
	size_t len = utf8_len(value);
	const char* str = value.c_str();
	
	if (len == 0) out.push_back(0x00);
	else {
		size_t offset = 0;

		// split in chunks
		while (len > 0x8000) {
			size_t sublen = 0x8000;
			size_t bytelen = utf8_pos(value, 0x8000, offset);

			// chunk can't end in high surrogate
			const unsigned char tail_16 = value[offset + bytelen - 2];
			const unsigned char tail_8 = value[offset + bytelen - 1];
			const int tail = (tail_16 << 8) + tail_8;
			if (0xd800 <= tail && tail <= 0xdbff) {
				sublen -= 1;
				bytelen -= 2;
			}

			const unsigned char b16 = ((sublen >> 8) & 0x000000FF);
			const unsigned char b8 = sublen & 0x000000FF;
			out.push_back('R');
			out.push_back(b16);
			out.push_back(b8);
			out.insert(out.end(), str + offset, str + offset + bytelen);

			len -= sublen;
			offset += bytelen;
		}

		if (len <= 0x1f) {
			out.push_back((unsigned char)len); // single octet length
			out.insert(out.end(), str + offset, str + offset + len);
		}
		else if (len <= 0x3ff) {
			// pack in two octets
			const unsigned char b16 = 0x30 + ((len >> 8) & 0x000000FF);
			const unsigned char b8 = len & 0x000000FF;
			out.push_back(b16);
			out.push_back(b8);
			out.insert(out.end(), str + offset, str + offset + len);
		}
		else {
			// tag + double octets
			const unsigned char b16 = (len >> 8) & 0x000000FF;
			const unsigned char b8 = len & 0x000000FF;
			out.push_back('S');
			out.push_back(b16);
			out.push_back(b8);
			out.insert(out.end(), str + offset, str + offset + len);
		}
	}
}

// write binary
void Writer::write_binary(const unsigned char* value, size_t len) {
	size_t offset = 0;

	// split in chunks
	while (len > 0x8000) {
		out.push_back('A');
		out.push_back(0x80);
		out.push_back(0x00);
		out.insert(out.end(), value + offset, value + offset + 0x8000);

		len -= 0x8000;
		offset += 0x8000;
	}

	if (len <= 0x0f) {
		out.push_back(0x20 + (unsigned char)len); // single octet length
	}
	else if (len <= 0x3ff) {
		// pack in two octets
		const unsigned char b16 = 0x34 + ((len >> 8) & 0xFF);
		const unsigned char b8 = len & 0xFF;
		out.push_back(b16);
		out.push_back(b8);
	}
	else {
		// tag + double octets
		const unsigned char b16 = (len >> 8) & 0xFF;
		const unsigned char b8 = len & 0xFF;
		out.push_back('B');
		out.push_back(b16);
		out.push_back(b8);
	}
	out.insert(out.end(), value + offset, value + offset + len);
}

void Writer::write_direct(unsigned char c) {
	out.push_back(c);
}

void Writer::write(const Value& value) {
	value.Write(*this);
}

void Writer::write(const ObjectMixin& value) {
	const string& name = value.GetObjectName();

	// Get object definition
	size_t object_ref;
	map<string,size_t>::const_iterator p = objectMap.find(name);
	if (p == objectMap.end()) {
		// Write object definition
		out.push_back('C');
		value.WriteObjectFieldNames(*this);

		// Add to map for later reference
		object_ref = objectMap.size();
		objectMap[name] = object_ref;
	}
	else object_ref = p->second;

	// Write object header
	if (object_ref <= 0x0f) {
		// packed in one octet
		const unsigned char b8 = 0x60 + (object_ref & 0xFF);
		out.push_back(b8);
	}
	else {
		// tag + integer
		out.push_back('O');
		write(object_ref);
	}

	// Write values
	value.WriteObjectValues(*this);
}


void Writer::call(const string& method) {
	Reset();

	out.push_back('C');
	write(method);
	write(0); // no args
}

void Writer::write_fault(fault_type type, const string& msg) {
	Reset();

	out.push_back('F');

	write("code");
	switch (type) {
	case ProtocolException:
		write("ProtocolException");
		break;
	case NoSuchObjectException:
		write("NoSuchObjectException");
		break;
	case NoSuchMethodException:
		write("NoSuchMethodException");
		break;
	case RequireHeaderException:
		write("RequireHeaderException");
		break;
	case ServiceException:
		write("ServiceException");
		break;
	default:
		throw value_exception("invalid fault type");
	}

	if (!msg.empty()) {
		write("message");
		write(msg);
	}

	out.push_back('Z'); // complete
}


void Call::Print(std::string& out) const {
	out += m_method;
	out.push_back('(');

	for (values::const_iterator p = m_parameters.begin(); p != m_parameters.end(); ++p) {
		if (p != m_parameters.begin()) out += ", ";
		p->Print(out);
	}

	out += ")\n";
}

void Call::Write(Writer& writer) const {
	writer.write_direct('C');
	writer.write(m_method);
	writer.write(m_parameters.size());

	for (values::const_iterator p = m_parameters.begin(); p != m_parameters.end(); ++p) {
		writer.write(*p);
	}
}

} // namespace hessian_ipc