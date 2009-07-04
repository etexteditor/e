#ifndef __SHELLRUNNER_H__
#define __SHELLRUNNER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif


#include <vector>

class cxEnv;

class ShellRunner
{
public:
	ShellRunner(void);
	~ShellRunner(void);

	static long RawShell(const std::vector<char>& command, const std::vector<char>& input, std::vector<char>* output, std::vector<char>* errorOut, cxEnv& env, bool isUnix=true, const wxString& cwd=wxEmptyString);
	static wxString RunShellCommand(const std::vector<char>& command, cxEnv& env);

	static wxString GetBashCommand(const wxString& cmd, cxEnv& env);

private:
	static wxString s_bashCmd;
	static wxString s_bashEnv;
};

#endif // __SHELLRUNNER_H__
