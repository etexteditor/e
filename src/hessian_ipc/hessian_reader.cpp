#include "hessian_reader.h"

namespace hessian_ipc {

Reader::Reader() : m_state(start), m_len_left(0)
{
}

void Reader::Reset() {
	m_state = start;
	m_result.reset();
	m_len_left = 0;
	m_stateStack.clear();
}

bool Reader::Parse(vector<unsigned char>::const_iterator begin, vector<unsigned char>::const_iterator end) {
	if (begin == end) return false; // need more input
	m_pos = begin;
	m_end = end;

	while (m_pos != m_end) {
		if (m_state >= value_start && m_state < value_last) {
			const bool result = ParseValue();
			if (!result) return false; // need more input

			if (m_result->IsList()) {
				m_stateStack.push_back(new savedstate(call_method, 0, m_result));
				m_state = value_start;
				continue;
			}

			if (m_stateStack.empty()) return true;
			savedstate& s = m_stateStack.back();

			switch (s.state) {
			case call_method:
				s.value->AsCall().SetMethod(m_result);
				s.state = call_len;
				m_state = value_start;
				continue; //ParseValue();
			case call_len:
				s.len_left = m_result->GetInt();
				s.state = call_args;
				m_state = value_start;
				continue; //ParseValue();
			case call_args:
				s.value->AsCall().AddParameter(m_result);
				m_state = value_start;
				if (--s.len_left) break; //ParseValue();
				else {
					m_result = s.value;
					return true; // call parsed
				}
			default:
				throw value_exception("invalid state");
			}
		}

		switch (m_state) {
		case start:
			{
				const char c = *m_pos;
				++m_pos;

				switch (c) {
				case 'H':
					//m_state = header1;
					break;
				case 'C':
					m_stateStack.push_back(new savedstate(call_method, 0, auto_ptr<Value>(new Call())));
					m_state = value_start;
					break; //ParseValue();
				case 'c':
					m_state = call10_v1;
				case 'R':
					m_state = value_start;
					break; //ParseValue();
				case 'F':
					break;
				default:
					throw value_exception("invalid message tag");
				}
			}
			break;
		/*case call10_v1:
			m_state = call10_v2;
			break;
		case call10_m:
			if (c != 'm') throw value_exception("expected m");
			m_state = call10_len1;
			break;
		case call10_len1;
			m_len_left = ((int)c) << 8;
			m_state = call10_len2;
			break;*/
		default:
			throw value_exception("invalid state");
		}
	}

	return false; // need more input
}

bool Reader::ParseValue() {
	while (m_pos != m_end) {
		const unsigned char c = *m_pos;
		++m_pos;

		switch (m_state) {
		case value_start:
			switch (c) {
			// Null
			case 'N':
				m_result.reset(new Null());
				return true;

			// Boolean
			case 'T':
			case 'F':
				m_result.reset(new Boolean(c == 'T'));
				return true;

			// Integer (packed in one octet)
			case 0x80: case 0x81: case 0x82: case 0x83:
			case 0x84: case 0x85: case 0x86: case 0x87:
			case 0x88: case 0x89: case 0x8a: case 0x8b:
			case 0x8c: case 0x8d: case 0x8e: case 0x8f:
			case 0x90: case 0x91: case 0x92: case 0x93:
			case 0x94: case 0x95: case 0x96: case 0x97:
			case 0x98: case 0x99: case 0x9a: case 0x9b:
			case 0x9c: case 0x9d: case 0x9e: case 0x9f:
			case 0xa0: case 0xa1: case 0xa2: case 0xa3:
			case 0xa4: case 0xa5: case 0xa6: case 0xa7:
			case 0xa8: case 0xa9: case 0xaa: case 0xab:
			case 0xac: case 0xad: case 0xae: case 0xaf:
			case 0xb0: case 0xb1: case 0xb2: case 0xb3:
			case 0xb4: case 0xb5: case 0xb6: case 0xb7:
			case 0xb8: case 0xb9: case 0xba: case 0xbb:
			case 0xbc: case 0xbd: case 0xbe: case 0xbf:
				m_result.reset(new Integer(c - 0x90));
				return true;

			// Integer (packed in two octets)
			case 0xc0: case 0xc1: case 0xc2: case 0xc3:
			case 0xc4: case 0xc5: case 0xc6: case 0xc7:
			case 0xc8: case 0xc9: case 0xca: case 0xcb:
			case 0xcc: case 0xcd: case 0xce: case 0xcf:
				m_result.reset(new Integer((c - 0xc8) << 8));
				m_state = integer_4;
				break;

			// Integer (packed in three octets)
			case 0xd0: case 0xd1: case 0xd2: case 0xd3:
			case 0xd4: case 0xd5: case 0xd6: case 0xd7:
				m_result.reset(new Integer((c - 0xd4) << 16));
				m_state = integer_3;
				break;

			// Integer (tag + 4 octets)
			case 'I':
				m_result.reset(new Integer());
				m_state = integer_1;
				break;

			// 64bit Long (packed in one octet)
			case 0xd8: case 0xd9: case 0xda: case 0xdb:
			case 0xdc: case 0xdd: case 0xde: case 0xdf:
			case 0xe0: case 0xe1: case 0xe2: case 0xe3:
			case 0xe4: case 0xe5: case 0xe6: case 0xe7:
			case 0xe8: case 0xe9: case 0xea: case 0xeb:
			case 0xec: case 0xed: case 0xee: case 0xef:
				m_result.reset(new Long(c - 0xe0));
				return true;

			// 64bit Long (packed in two octets)
			case 0xf0: case 0xf1: case 0xf2: case 0xf3:
			case 0xf4: case 0xf5: case 0xf6: case 0xf7:
			case 0xf8: case 0xf9: case 0xfa: case 0xfb:
			case 0xfc: case 0xfd: case 0xfe: case 0xff:
				m_result.reset(new Long((c - 0xf8) << 8));
				m_state = long_8;
				break;

			// 64bit Long (packed in three octets)
			case 0x38: case 0x39: case 0x3a: case 0x3b:
			case 0x3c: case 0x3d: case 0x3e: case 0x3f:
				m_result.reset(new Long((c - 0x3c) << 16));
				m_state = long_7;
				break;

			// 64bit Long (tag + 4 octets)
			case 0x59:
				m_result.reset(new Long());
				m_state = long_5;
				break;

			// 64bit Long (tag + 8 octets)
			case 'L':
				m_result.reset(new Long());
				m_state = long_1;
				break;

			// 64-bit millisecond UTC date
			case 0x4a:
				m_result.reset(new Date());
				m_state = long_1;
				break;

			// 64-bit minute UTC date
			case 0x4b:
				m_result.reset(new Date());
				m_state = date_1;
				break;

			// Empty string
			case '\0': 
				m_result.reset(new String());
				return true;

			// String (length packed in one octet)
			case 0x01: case 0x02: case 0x03:
			case 0x04: case 0x05: case 0x06: case 0x07:
			case 0x08: case 0x09: case 0x0a: case 0x0b:
			case 0x0c: case 0x0d: case 0x0e: case 0x0f:
			case 0x10: case 0x11: case 0x12: case 0x13:
			case 0x14: case 0x15: case 0x16: case 0x17:
			case 0x18: case 0x19: case 0x1a: case 0x1b:
			case 0x1c: case 0x1d: case 0x1e: case 0x1f:
				{
					if (m_result.get() && m_result->IsStringChunk()) {
						m_result->AsString().SetChunk(false);
					}
					else m_result.reset(new String());

					m_state = string_data;
					m_len_left = c;

					String& s = m_result->AsString();
					s.reserve(s.size() + m_len_left); // we need at least this
				}
				break;

			// String (length packed in two octets)
			case 0x30: case 0x31: case 0x32: case 0x33:
				if (m_result.get() && m_result->IsStringChunk()) {
					m_result->AsString().SetChunk(false); // final
				}
				else m_result.reset(new String());

				m_state = string_len2;
				m_len_left = ((int)c - 0x30);
				break;

			case 'S': // String (tag + len in 2 octets)
				if (m_result.get() && m_result->IsStringChunk()) {
					m_result->AsString().SetChunk(false); // final
				}
				else m_result.reset(new String());
				m_state = string_len1;
				break;

			case 'R': // String chunk (tag + len in 2 octets)
				if (m_result.get() == NULL || !m_result->IsStringChunk()) {
					m_result.reset(new String());
					m_result->AsString().SetChunk();
				}
				m_state = string_len1;
				break;

			
			// Variable-length list
			case 0x55:
				break;

			// Variable-length untyped list
			case 0x57:
				break;

			// Fixed-length list
			case 'V':
				break;

			// Fixed-length untyped list
			case 0x58:
				break;

			// Compact fixed list
			case 0x70: case 0x71: case 0x72: case 0x73:
			case 0x74: case 0x75: case 0x76: case 0x77:
				break;

			// Compact fixed untyped list
			case 0x78: case 0x79: case 0x7a: case 0x7b:
			case 0x7c: case 0x7d: case 0x7e: case 0x7f:
				break;




			default:
				throw value_exception("invalid tag");
			}
			break;

		case integer_1:
			m_result->AsInteger() += ((int)c << 24);
			m_state = integer_2;
			break;
		case integer_2:
			m_result->AsInteger() += ((int)c << 16);
			m_state = integer_3;
			break;
		case integer_3:
			m_result->AsInteger() += ((int)c << 8);
			m_state = integer_4;
			break;
		case integer_4:
			m_result->AsInteger() += c;
			return true;

		case long_1:
			m_result->AsLong() += ((long long)c << 56);
			m_state = long_2;
			break;
		case long_2:
			m_result->AsLong() += ((long long)c << 48);
			m_state = long_3;
			break;
		case long_3:
			m_result->AsLong() += ((long long)c << 40);
			m_state = long_4;
			break;
		case long_4:
			m_result->AsLong() += ((long long)c << 32);
			m_state = long_5;
			break;
		case long_5:
			m_result->AsLong() += ((long long)c << 24);
			m_state = long_6;
			break;
		case long_6:
			m_result->AsLong() += ((long long)c << 16);
			m_state = long_7;
			break;
		case long_7:
			m_result->AsLong() += ((long long)c << 8);
			m_state = long_8;
			break;
		case long_8:
			m_result->AsLong() += c;
			return true;

		case date_1:
			m_result->AsLong() += ((long long)c << 24);
			m_state = long_6;
			break;
		case date_2:
			m_result->AsLong() += ((long long)c << 16);
			m_state = long_7;
			break;
		case date_3:
			m_result->AsLong() += ((long long)c << 8);
			m_state = long_8;
			break;
		case date_4:
			m_result->AsLong() += c;
			m_result->AsDate().AdjustToMinutes();
			return true;

		case string_len1:
			m_len_left += ((int)c << 8);
			m_state = string_len2;
			break;
		case string_len2:
			{
				m_len_left += c;
				String& s = m_result->AsString();
				s.reserve(s.size() + m_len_left); // we need at least this
				m_state = string_data;
			}
			break;
		case string_data:
			m_result->AsString() += c;
			if (--m_len_left) break;
			else if (m_result->IsStringChunk()) {
				m_state = string_next;
				break;
			}
			else return true;

		case string_next:
			switch (c) {
				// String (length packed in one octet)
				case 0x01: case 0x02: case 0x03:
				case 0x04: case 0x05: case 0x06: case 0x07:
				case 0x08: case 0x09: case 0x0a: case 0x0b:
				case 0x0c: case 0x0d: case 0x0e: case 0x0f:
				case 0x10: case 0x11: case 0x12: case 0x13:
				case 0x14: case 0x15: case 0x16: case 0x17:
				case 0x18: case 0x19: case 0x1a: case 0x1b:
				case 0x1c: case 0x1d: case 0x1e: case 0x1f:
				// String (length packed in two octets)
				case 0x30: case 0x31: case 0x32: case 0x33:
				case 'S': // String (tag + len in 2 octets)
				case 'R': // String chunk (tag + len in 2 octets)
					m_state = value_start;
					--m_pos;
					break;
				default:
					throw value_exception("chunked string not completed");
			}
			break;
		default:
			throw value_exception("invalid state");
		}
	}

	return false; // need more input
}

} // namespace hessian