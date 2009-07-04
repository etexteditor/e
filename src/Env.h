#ifndef __ENV_H__
#define __ENV_H__

#include <wx/string.h>
#include <wx/file.h>

#include <vector>
#include <map>

class cxEnv {
public:
	cxEnv() {};
	cxEnv(const cxEnv& env) {m_env = env.m_env;};

	void Clear() {m_envStr.clear();};
	bool GetEnv(const wxString& key, wxString& value);
	void SetEnv(const wxString& key, const wxString& value);
	void SetEnv(const std::map<wxString, wxString>& env);
	void SetIfValue(const wxString& key, const wxString& value);

	void SetToCurrent();

	const char* GetEnvBlock() const;
	void GetEnvBlock(wxString& env) const;
	void WriteEnvToFile(wxFile& file) const;

	void AddSystemVars(const bool isUnix, const wxString &baseAppPath);

private:
	mutable std::map<wxString, wxString> m_env;
	mutable std::vector<char> m_envStr;
};

#endif //  __ENV_H__
