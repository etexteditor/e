#include "ShellRunner.h"
#include "eApp.h"
#include "eDocumentPath.h"

// Initialize statics
wxString ShellRunner::s_bashCmd;
wxString ShellRunner::s_bashEnv;

ShellRunner::ShellRunner(void){}
ShellRunner::~ShellRunner(void){}

//
// Runs the given command in an appropriate shell, returning stdout, stderr and the result code.
// If an internal error occurs, such as invalid inputs to this fuction, -1 is returned.
//
long ShellRunner::RawShell(const vector<char>& command, const vector<char>& input, vector<char>* output, vector<char>* errorOut, cxEnv& env, bool isUnix, const wxString& cwd) {
	if (command.empty()) return -1;

#ifdef __WXMSW__
	if (isUnix && !((eApp*)wxTheApp)->InitCygwin()) return -1;
#endif // __WXMSW__

	// Create temp file with command
	wxFileName tmpfilePath = ((eApp*)wxTheApp)->GetAppDataPath();
	tmpfilePath.SetFullName(isUnix ? wxT("tmcmd") : wxT("tmcmd.bat"));
	wxFile tmpfile(tmpfilePath.GetFullPath(), wxFile::write);
	if (!tmpfile.IsOpened()) return -1;

	tmpfile.Write(&command[0], command.size());
	tmpfile.Close();

	wxString execCmd;

	// Look for shebang
	if (command.size() > 2 && command[0] == '#' && command[1] == '!') {
		// Ignore leading whitespace
		unsigned int i = 2;
		for (; i < command.size() && isspace(command[i]); ++i);

		// Get the interpreter path
		unsigned int start = i;
		for (; i < command.size() && !isspace(command[i]); ++i);
		wxString cmd(&command[start], wxConvUTF8, i - start);

		// Rest of line is argument for interpreter
		for (start = i; i < command.size() && command[i] != '\n'; ++i);
		wxString args(&command[start], wxConvUTF8, i - start);

#ifdef __WXMSW__
		// Convert possible unix path to windows
		const wxString newpath = eDocumentPath::CygwinPathToWin(cmd);
		if (newpath.empty()) return -1;
		execCmd = newpath + args;
#else
		execCmd = cmd + args;
#endif // __WXMSW__
		execCmd += wxT(" \"") + tmpfilePath.GetFullPath() + wxT("\"");
	}
	else if (isUnix) {
		if (s_bashCmd.empty()) {
#ifdef __WXMSW__
			s_bashCmd = eDocumentPath::s_cygPath + wxT("\\bin\\bash.exe \"") + tmpfilePath.GetFullPath() + wxT("\"");
#else
            s_bashCmd = wxT("bash \"") + tmpfilePath.GetFullPath() + wxT("\"");
#endif

			wxFileName initPath = ((eApp*)wxTheApp)->GetAppPath();
			initPath.AppendDir(wxT("Support"));
			initPath.AppendDir(wxT("lib"));
			initPath.SetFullName(wxT("bash_init.sh"));
			if (initPath.FileExists()) {
				s_bashEnv = initPath.GetFullPath();
			}
		}

		env.SetEnv(wxT("BASH_ENV"), s_bashEnv);
		execCmd = s_bashCmd;
	}
#ifdef __WXMSW__
	else {
		// Windows native runs as bat files
		// (needs double quotes for path)
		execCmd = wxT("cmd /C \"\"") + tmpfilePath.GetFullPath() + wxT("\"\"");
	}
#endif // __WXMSW__

	// Get ready for execution
	cxExecute exec(env, cwd);
	bool debugOutput = false; // default setting
	((eApp*)wxTheApp)->GetSettingBool(wxT("bundleDebug"), debugOutput);
	exec.SetDebugLogging(debugOutput);

	// Exec the command
	wxLogDebug(wxT("Running command: %s"), execCmd.c_str());
	int resultCode = exec.Execute(execCmd, input);

	// Get the output
	if (resultCode != -1) {
		if (output) output->swap(exec.GetOutput());
		if (errorOut) errorOut->swap(exec.GetErrorOut());
	}

	return resultCode;
}

wxString ShellRunner::GetBashCommand(const wxString& cmd, cxEnv& env) {
#ifdef __WXMSW__
	if (!((eApp*)wxTheApp)->InitCygwin()) return wxEmptyString;
#endif

	if (s_bashEnv.empty()) {
		wxFileName initPath = ((eApp*)wxTheApp)->GetAppPath();
		initPath.AppendDir(wxT("Support"));
		initPath.AppendDir(wxT("lib"));
		initPath.SetFullName(wxT("bash_init.sh"));
		if (initPath.FileExists()) {
			s_bashEnv = initPath.GetFullPath();
		}
	}
	env.SetEnv(wxT("BASH_ENV"), s_bashEnv);

#ifdef __WXMSW__
	return eDocumentPath::s_cygPath + wxT("\\bin\\bash.exe -c \"") + cmd + wxT("\"");
#else
    return wxT("bash -c \"") + cmd + wxT("\"");
#endif
}
