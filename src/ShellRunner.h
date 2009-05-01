#ifndef __SHELLRUNNER_H__
#define __SHELLRUNNER_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Execute.h"

class ShellRunner
{
public:
	ShellRunner(void);
	~ShellRunner(void);

	static long RawShell(const vector<char>& command, const vector<char>& input, vector<char>* output, vector<char>* errorOut, cxEnv& env, bool isUnix=true, const wxString& cwd=wxEmptyString);
	static wxString ShellRunner::RunShellCommand(const vector<char>& command, cxEnv& env);

	static wxString GetBashCommand(const wxString& cmd, cxEnv& env);

private:
	static wxString s_bashCmd;
	static wxString s_bashEnv;
};

#endif // __SHELLRUNNER_H__
