#ifndef HESSIAN_VALUES_H
#define HESSIAN_VALUES_H

#include <exception>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <map>

using namespace std;

namespace hessian_ipc {
	// Pre-declarations
	class Value;
	class Integer;
	class Long;
	class Date;
	class String;
	class Call;
	class Writer;

	enum fault_type {
		ProtocolException,
		NoSuchObjectException,
		NoSuchMethodException,
		RequireHeaderException,
		ServiceException
	};

	// Exception
	class value_exception : public exception {
	public:
		explicit value_exception(const string& what): _message(what) {}
		virtual ~value_exception() throw() {}

		virtual const char* what() const throw() {return _message.c_str();}; //Get error message.
		virtual void raise() const {throw *this;}; //Re-throw.
	private:
		string _message;
	};

	class ObjectMixin {
	public:
		virtual const string& GetObjectName() const = 0;
		virtual void WriteObjectFieldNames(Writer& writer) const = 0;
		virtual void WriteObjectValues(Writer& writer) const = 0;
	};

	class Writer {
	public:
		Writer() {};

		void Reset();
		const vector<unsigned char>& GetOutput() const {return out;};

		// Calls
		void call(const string& method);
		template<class T> void call(const string& method, const T& arg);
		template<class T1, class T2> void call(const string& method, const T1& arg1, const T2& arg2);

		template<class T> void write_reply(const T& value);
		void write_fault(fault_type type, const string& msg);

		// Variable writers
		void write(bool value);
		void write(int value);
		void write(unsigned int value);
		void write(long long value);
		void write(const string& value);
		void write(const Value& value);
		void write(const ObjectMixin& value);
		template<class T> void write(const vector<T>& value);
		template<class T1, class T2> void write(const map<T1,T2>& value);

		void write_null();
		void write_date(time_t value);
		void write_binary(const unsigned char* value, size_t len);
		void write_direct(unsigned char c);

	private:
		vector<unsigned char> out;
		map<string,size_t> objectMap;
	};

	// abstract base class for all values
	class Value {
	public:
		// get the value type
		virtual bool IsNull() const {return false;};
		virtual bool IsBinary() const {return false;};
		virtual bool IsBoolean() const {return false;};
		virtual bool IsInt() const {return false;};
		virtual bool IsLong() const {return false;};
		virtual bool IsDouble() const {return false;};
		virtual bool IsString() const {return false;};
		virtual bool IsDate() const {return false;};
		virtual bool IsList() const {return false;};
		virtual bool IsMap() const {return false;};
		virtual bool IsObject() const {return false;};
		virtual bool IsCall() const {return false;};
		virtual bool IsStringChunk() const {return false;};

		// get the value
		virtual int GetInt() const {throw value_exception("invalid value access");};
		virtual long long GetLong() const {throw value_exception("invalid value access");};
		virtual time_t GetDate() const {throw value_exception("invalid value access");};
		virtual bool GetBoolean() const {throw value_exception("invalid value access");};
		virtual const string& GetString() const {throw value_exception("invalid value access");};

		// get the real type
		virtual Integer& AsInteger() {throw value_exception("invalid value type");};
		virtual Long& AsLong() {throw value_exception("invalid value type");};
		virtual Date& AsDate() {throw value_exception("invalid value type");};
		virtual String& AsString() {throw value_exception("invalid value type");};
		virtual Call& AsCall() {throw value_exception("invalid value type");};

		// dump
		virtual void Print(std::string& out) const = 0;
		virtual void Write(Writer& writer) const = 0;
	};

	class Null : public Value {
	public:
		Null() {};

		// get the value type
		bool IsNull() const {return true;};

		// dump
		void Print(string& out) const {out += "null";};
		void Write(Writer& writer) const {writer.write_null();};
	};

	class Boolean : public Value {
	public:
		Boolean(bool value) : m_value(value) {};

		// get the value type
		bool IsBoolean() const {return true;};

		// get the value
		bool GetBoolean() const {return m_value;};

		// dump
		void Print(string& out) const {out += m_value ? "true" : "false";};
		void Write(Writer& writer) const {writer.write(m_value);};

	private:
		bool m_value;
	};

	class Integer : public Value {
	public:
		Integer() : m_value(0) {};
		Integer(int value) : m_value(value) {};

		// get the value type
		bool IsInteger() const {return true;};

		// get the value
		int GetInt() const {return m_value;};
		Integer& AsInteger() {return *this;};

		// set value
		int operator+=(int v) {m_value += v; return m_value;}

		// dump
		void Print(string& out) const {out += boost::lexical_cast<std::string>(m_value);};
		void Write(Writer& writer) const {writer.write(m_value);};

	private:
		int m_value;
	};

	class Long : public Value {
	public:
		Long() : m_value(0) {};
		Long(long long value) : m_value(value) {};

		// get the value type
		bool IsLong() const {return true;};

		// get the value
		long long GetLong() const {return m_value;};
		Long& AsLong() {return *this;};

		// set value
		long long operator+=(long long v) {m_value += v; return m_value;}

		// dump
		void Print(string& out) const {out += boost::lexical_cast<std::string>(m_value);};
		void Write(Writer& writer) const {writer.write(m_value);};

	protected:
		long long m_value;
	};

	class Date : public Long {
	public:
		Date() {};
		Date(time_t value) : Long(value) {};

		// get the value type
		bool IsDate() const {return true;};

		// get the value
		Date& AsDate() {return *this;};

		// set value
		long long operator+=(long long v) {m_value += v; return m_value;}
		void AdjustToMinutes() {m_value *= 60000LL;}

		// dump
		void Print(string& out) const {out += boost::lexical_cast<std::string>(m_value);};
		void Write(Writer& writer) const {writer.write_date(m_value);};
	};

	class String : public Value {
	public:
		String() : m_isChunk(false) {};
		String(const string& value) : m_value(value), m_isChunk(false) {};

		// get the value type
		bool IsString() const {return true;};
		bool IsStringChunk() const {return m_isChunk;};

		// get the value
		const string& GetString() const {return m_value;};
		String& AsString() {return *this;};
		size_t size() const {return m_value.size();};

		// set value
		string& operator+=(char c) {m_value += c; return m_value;};
		void reserve(size_t len) {m_value.reserve(len);};
		void SetChunk(bool chunk=true) {m_isChunk = chunk;};

		// dump
		void Print(string& out) const {out += '"'; out += m_value; out += '"';};
		void Write(Writer& writer) const {writer.write(m_value);};

	private:
		string m_value; // utf-8 encoded
		bool m_isChunk;
	};

	class List : public Value {
	public:
		List() {};

		// get the value type
		bool IsList() const {return true;};

		// get the value
		List& AsList() {return *this;};

		// set value
		void Add(auto_ptr<Value> v) {m_values.push_back(v);};

	private:
		boost::ptr_vector<Value> m_values;

	};

	class Call : public Value {
	public:
		Call() {};
		Call(const string& method) {m_method = method;};

		// get the value type
		bool IsCall() const {return true;};
		Call& AsCall() {return *this;};

		// get the value
		const string& GetMethod() const {return m_method;};
		size_t GetParameterCount() const {return m_parameters.size();};
		const Value& GetParameter(size_t index) const {return m_parameters[index];};

		// set value
		void SetMethod(auto_ptr<Value> value) {m_method = value->GetString();};
		void AddParameter(auto_ptr<Value> value) {m_parameters.push_back(value);};

		// dump
		void Print(string& out) const;
		void Write(Writer& writer) const;

	private:
		string m_method; // utf-8 encoded
		typedef boost::ptr_vector<Value> values;
		values m_parameters;
	};

} // namespace hessian_ipc

#include "hessian_writers.h"

#endif //HESSIAN_VALUES_H