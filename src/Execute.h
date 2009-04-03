/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __EXECPROCESS_H__
#define __EXECPROCESS_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/process.h>
#include <wx/file.h>

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(disable:4786)
    #pragma warning(push, 1)
#endif
#include <vector>
#include <map>
#include <algorithm>
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

	void SetToCurrent();

	const char* GetEnvBlock() const;
	void GetEnvBlock(wxString& env) const;
	void WriteEnvToFile(wxFile& file) const;

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



class cxExecute : public wxEvtHandler {
public:
	cxExecute(const cxEnv& env, const wxString& cwd=wxEmptyString)
		: m_threadDone(false), m_env(env), m_cwd(cwd), m_debugLog(false), m_showWindow(false), m_updateWindow(true) {};

	int Execute(const wxString& command);
	int Execute(const wxString& command, const vector<char>& input);
	vector<char>& GetOutput() {return m_output;};
	vector<char>& GetErrorOut() {return m_errout;};

	void SetDebugLogging(bool doLog) {m_debugLog = doLog;};
	void SetShowWindow(bool doShow) {m_showWindow = doShow;};
	void SetUpdateWindow(bool doUpdate) {m_updateWindow = doUpdate;};

	void ThreadDone(int exitCode);

private:

	class cxExecuteThread : public wxThread {
	public:
		cxExecuteThread(const wxString& command, const vector<char>& input, vector<char>& output, vector<char>& errout, cxExecute& evtHandler, const cxEnv& env, const wxString& cwd, bool doShow);
		int Execute();
		void Terminate() {m_isTerminated = true;};

		virtual void *Entry();

	private:
		bool CreateChildProcess();
		void WriteToPipe();
		void ReadFromPipe();

		// Member variables
		bool m_isTerminated;
		const wxString& m_command;
		cxExecute& m_evtHandler;
		const vector<char>& m_input;
		vector<char>& m_output;
		vector<char>& m_errout;
		const cxEnv& m_env;
		const wxString& m_cwd;
		bool m_showWindow;
		int m_pid;

#ifdef __WXMSW__
		// Win32 handles
		DWORD  m_dwExitCode;
		PROCESS_INFORMATION m_pi;
		HANDLE m_hChildStdinRd;
		HANDLE m_hChildStdinWr;
		HANDLE m_hChildStdoutRd;
		HANDLE m_hChildStdoutWr;
		HANDLE m_hInputFile;
#else
		// POSIX pipes
		int m_dwExitCode;
		int m_stdin[2];
		int m_stdout[2];
#endif
	};

	// Event Handlers
	void OnEndProcess(wxProcessEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	bool m_threadDone;
	int m_exitCode;
	vector<char> m_output;
	vector<char> m_errout;
	const cxEnv& m_env;
	const wxString& m_cwd;
	bool m_debugLog;
	bool m_showWindow;
	bool m_updateWindow;
};

#endif // __EXECPROCESS_H__
