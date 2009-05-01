#ifndef __ENV_H__
#define __ENV_H__

#include <wx/string.h>
#include <wx/file.h>

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(disable:4786)
    #pragma warning(push, 1)
#endif
#include <vector>
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class cxEnv {
public:
	cxEnv() {};
	cxEnv(const cxEnv& env) {m_env = env.m_env;};

	void Clear() {m_envStr.clear();};
	bool GetEnv(const wxString& key, wxString& value);
	void SetEnv(const wxString& key, const wxString& value);
	void SetEnv(const map<wxString, wxString>& env);
	void SetIfValue(const wxString& key, const wxString& value);

	void SetToCurrent();

	const char* GetEnvBlock() const;
	void GetEnvBlock(wxString& env) const;
	void WriteEnvToFile(wxFile& file) const;

	void AddSystemVars(const bool isUnix, const wxString &baseAppPath);

private:
	class LineCmp {
	public:
		bool operator()(const vector<char>& a, const vector<char>& b) {
			return strcmp(&*a.begin(), &*b.begin()) < 0;
		}
	};

	mutable map<wxString, wxString> m_env;
	mutable vector<char> m_envStr;
};

#endif //  __ENV_H__