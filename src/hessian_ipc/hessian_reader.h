#ifndef HESSIAN_READER_H
#define HESSIAN_READER_H

#include "hessian_values.h"
#include <memory>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;

namespace hessian_ipc {

	class Reader {
	public:
		Reader();

		void Reset();
		bool Parse(vector<unsigned char>::const_iterator begin, vector<unsigned char>::const_iterator end);
		
		const Value* GetResultValue() {return m_result.get();};
		const Call* GetResultCall() {return &m_result->AsCall();};
		vector<unsigned char>::const_iterator GetEndPos() {return m_pos;};

	private:
		bool ParseValue();

		enum State {
			start,
			call_method,
			call_len,
			call_args,
			string_chunk,

			call10_v1,
			call10_v2,
			call10_m,
			call10_len1,
			call10_len2,
			call10_method,
			call10_args,

			// value states
			value_start,
			integer_1,
			integer_2,
			integer_3,
			integer_4,
			long_1,
			long_2,
			long_3,
			long_4,
			long_5,
			long_6,
			long_7,
			long_8,
			date_1,
			date_2,
			date_3,
			date_4,
			string_len1,
			string_len2,
			string_data,
			string_next,
			proxy_handle,
			value_last
		};

		class savedstate {
		public:
			savedstate(State s, size_t ll, auto_ptr<Value> v)
				: state(s), len_left(ll), value(v) {};
			State state;
			size_t len_left;
			auto_ptr<Value> value;
		};

		// Member variables
		vector<unsigned char>::const_iterator m_pos;
		vector<unsigned char>::const_iterator m_end;
		State m_state;
		size_t m_len_left;
		auto_ptr<Value> m_result;
		boost::ptr_vector<savedstate> m_stateStack;
	};

} // namespace hessian

#endif //HESSIAN_READER_H
