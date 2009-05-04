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

#include "Execute.h"
#include "eApp.h"

#ifndef WX_PRECOMP
	#include <wx/filename.h>
	#include <wx/log.h>
#endif

#ifndef __WXMSW__
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#endif

#include "Env.h"

BEGIN_EVENT_TABLE(cxExecute, wxEvtHandler)
	EVT_END_PROCESS(99, cxExecute::OnEndProcess)
END_EVENT_TABLE()

int cxExecute::Execute(const wxString& command) {
	const vector<char> input; // no input
	return Execute(command, input);
}

int cxExecute::Execute(const wxString& command, const vector<char>& input) {
	// Clear state variables
	m_threadDone = false;

	// Check if debug output is enabled
	wxFile logFile;
	if (m_debugLog) {
		// Create temp file with command
		wxFileName logfilePath = wxGetApp().GetAppDataPath();
		logfilePath.SetFullName(wxT("tmcmd.log"));
		logFile.Open(logfilePath.GetFullPath(), wxFile::write);
		if (logFile.IsOpened()) {
			const wxString cmd = wxT("Running command:\n") + command + wxT("\n\n");
			logFile.Write(cmd);

			logFile.Write(wxT("Environment:\n"));
			m_env.WriteEnvToFile(logFile);

			logFile.Write(wxT("Input:\n"));
			if (!input.empty()) logFile.Write(&*input.begin(), input.size());
			logFile.Write(wxT("\n"));
		}
	}
	if (command.empty()) return -1;

#ifdef __WXDEBUG__
	//wxStopWatch sw;
#endif  //__WXDEBUG__

	// Create the execute thread
	cxExecuteThread* execThread = new cxExecuteThread(command, input, m_output, m_errout, *this, m_env, m_cwd, m_showWindow);

	// Launch the process
	const int pid = execThread->Execute();
	if (pid == -1) return -1; // Process creation failed

#ifdef __WXDEBUG__
	//wxLogDebug(wxT("wxExecute started at %ldms"), sw.Time());
#endif  //__WXDEBUG__


	// Wait for the process to finish
	while (!m_threadDone) {
		// Check if user is pressing esc to cancel
		if (wxGetKeyState(WXK_ESCAPE)) {
			wxLogDebug(wxT("Killing process %d"), pid);

			// Notify the thread that it is being killed
			execThread->Terminate();

			wxKill(pid, wxSIGKILL, NULL, wxKILL_CHILDREN);

			if (m_debugLog && logFile.IsOpened()) {
				logFile.Write(wxT("Process was killed by user.\n"));
			}

			return -1;
		}

		// Update the windows
		if (m_updateWindow) {
#ifdef __WXMSW__
			// WORKAROUND: Yield resets busy cursor
			// so we have to buffer it and reset it after yielding
			HCURSOR currentCursor = ::GetCursor();
#endif //__WXMSW__

			wxSafeYield();

#ifdef __WXMSW__
			::SetCursor(currentCursor);
			//wxSetCursor(*wxHOURGLASS_CURSOR); // WORKAROUND: Yield resets busy cursor
#endif //__WXMSW__
		}

		// We want to avoid using 100% cpu
		wxMilliSleep(50);
	}

#ifdef __WXDEBUG__
	//wxLogDebug(wxT("wxExecute took %ldms to execute"), sw.Time());
#endif  //__WXDEBUG__

	if (m_debugLog && logFile.IsOpened()) {
		// stdout and stderr is currently mixed
		logFile.Write(wxT("Output:\n"));
		if (!m_output.empty()) logFile.Write(&*m_output.begin(), m_output.size());
		logFile.Write(wxT("\n"));

		const wxString ret = wxString::Format(wxT("Command returned with exitcode:\n%d\n"), m_exitCode);
		logFile.Write(ret);
	}

	return m_exitCode;
}

void cxExecute::ThreadDone(int exitCode) {
	m_exitCode = exitCode;
	m_threadDone = true;
}

void cxExecute::OnEndProcess(wxProcessEvent& event) {
	m_exitCode = event.GetExitCode();
	m_threadDone = true;
}


// ------ cxExecuteThread -----------------------------------------------------

#define BUFSIZE 4096

cxExecute::cxExecuteThread::cxExecuteThread(const wxString& command, const vector<char>& input, vector<char>& output, vector<char>& errout, cxExecute& evtHandler, const cxEnv& env, const wxString& cwd, bool doShow)
: m_isTerminated(false), m_command(command), m_evtHandler(evtHandler), m_input(input),  m_output(output),
  m_errout(errout), m_env(env), m_cwd(cwd), m_showWindow(doShow) {
}

int cxExecute::cxExecuteThread::Execute() {
#ifdef __WXMSW__
	SECURITY_ATTRIBUTES saAttr;

	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT.
	if (! CreatePipe(&m_hChildStdoutRd, &m_hChildStdoutWr, &saAttr, 0)) {
		wxLogDebug(wxT("Stdout pipe creation failed\n"));
		return -1;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	SetHandleInformation( m_hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);

	// Create a pipe for the child process's STDIN.
	if (! CreatePipe(&m_hChildStdinRd, &m_hChildStdinWr, &saAttr, 0)) {
		wxLogDebug(wxT("Stdin pipe creation failed\n"));
		return -1;
	}

	// Ensure the write handle to the pipe for STDIN is not inherited.
	SetHandleInformation( m_hChildStdinWr, HANDLE_FLAG_INHERIT, 0);
#else
	if (pipe( m_stdin ) < 0) {
		wxLogDebug(wxT("Stdin pipe creation failed\n"));
		return -1;
	}
	if (pipe( m_stdout ) < 0) {
		wxLogDebug(wxT("Stdout pipe creation failed\n"));
		return -1;
	}
#endif

	// Now create the child process.
	const bool success = CreateChildProcess();
	if (!success) {
	  wxLogDebug(wxT("Create process failed"));
	  return -1;
	}

	// Start the thread
	Create();
	Run();

	return m_pid;
}

void* cxExecute::cxExecuteThread::Entry() {
#ifdef __WXDEBUG__
//	wxStopWatch sw;
#endif  //__WXDEBUG__

	// NOTE: We have to be careful with accessing m_input and m_output
	// as the parent class may have closed down. Always check
	// m_isTerminated before accessing them.

	// Write to pipe that is the standard input for a child process.
	wxLogDebug(wxT("Exec: %s"), m_command.c_str());
	WriteToPipe();
	wxLogDebug(wxT("  written to stdin"));

	// Read from pipe that is the standard output for child process.
	ReadFromPipe();
	wxLogDebug(wxT("  read from stdout"));
	wxLogDebug(wxT("  waiting for process to terminate"));

#ifdef __WXMSW__
	// Wait for process to terminate
	if (WaitForSingleObject(m_pi.hProcess, INFINITE) == WAIT_FAILED) {
		wxLogDebug(wxT("Waiting for process failed"));
		return NULL;
	}

	// Get the return code
	if (!GetExitCodeProcess(m_pi.hProcess, &m_dwExitCode) )
	{
		wxLogDebug(wxT("GetExitCodeProcess failed"));
	}
#else
	int status;
	int result;

	// Wait for process to terminate
	do {
		result = waitpid( m_pid, &status, 0 );
	} while(result == -1 && errno == EINTR);
	if (result < 0) {
		wxLogDebug(wxT("Waiting for process failed"));
		return NULL;
	}

	// Save the return code
	if (! WIFEXITED(status)) {
		wxLogDebug(wxT("Process did not exited normally"));
	} else {
		m_dwExitCode = WEXITSTATUS(status);
	}
#endif

	wxLogDebug(wxT("  terminated with exitcode: %d"), m_dwExitCode);

#ifdef __WXDEBUG__
//	wxLogDebug(wxT("process took %ldms to execute"), sw.Time());
#endif  //__WXDEBUG__

	// Notify parent that the process is completed
	if (!m_isTerminated) m_evtHandler.ThreadDone(m_dwExitCode);
	//wxProcessEvent event(99, m_pi.dwProcessId, m_dwExitCode);
	//m_evtHandler.AddPendingEvent(event);

	return NULL;
}

#ifdef __WXMSW__
bool cxExecute::cxExecuteThread::CreateChildProcess()
{
	// Create the env block
	wxString env;
	m_env.GetEnvBlock(env);

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory( &m_pi, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure.
	STARTUPINFO siStartInfo;
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = m_hChildStdoutWr;
	siStartInfo.hStdOutput = m_hChildStdoutWr;
	siStartInfo.hStdInput = m_hChildStdinRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	siStartInfo.dwFlags |= STARTF_USESHOWWINDOW;

	if (m_showWindow) siStartInfo.wShowWindow = SW_SHOW;
	else siStartInfo.wShowWindow = SW_HIDE;

	// Create the child process.
	BOOL bFuncRetn = CreateProcess(NULL,
      (wxChar*)m_command.c_str(),     // command line
      NULL,          // process security attributes
      NULL,          // primary thread security attributes
      TRUE,          // handles are inherited
      CREATE_UNICODE_ENVIRONMENT,           // creation flags
      (char*)env.c_str(),                   // environment
      m_cwd.empty() ? NULL : m_cwd.c_str(), // current directory
      &siStartInfo,  // STARTUPINFO pointer
      &m_pi);  // receives PROCESS_INFORMATION

	if (bFuncRetn == 0) {
		wxLogDebug(wxT("CreateProcess failed"));
		return false;
	}
	else {
	  m_pid = m_pi.dwProcessId;
	  wxLogDebug(wxT("  started process %d"), m_pid);

	  // Close any unnecessary handles.
	  CloseHandle(m_pi.hThread);

	  return true;
	}
}

void cxExecute::cxExecuteThread::WriteToPipe()
{
	// Write input to to the process's stdIn pipe.
	if (!m_input.empty()) {
		unsigned int bytesWritten = 0;

		while (bytesWritten < m_input.size()) {
			if (m_isTerminated) break;

			DWORD dwWritten = 0;
			if (! WriteFile(m_hChildStdinWr, &m_input[bytesWritten], wxMin(BUFSIZE, m_input.size()-bytesWritten), &dwWritten, NULL)) {
				//wxLogDebug(wxT("WriteToPipe failed %d"), dwWritten);
				break;
			}

			bytesWritten += dwWritten;
			//wxLogDebug(wxT("WriteToPipe %d"), dwWritten);
		}
	}

	// Close the pipe handle so the child process stops reading.
	if (! CloseHandle(m_hChildStdinWr)) {
		wxLogDebug(wxT("Close pipe failed"));
	}
}

void cxExecute::cxExecuteThread::ReadFromPipe()
{
	DWORD dwRead;
	char chBuf[BUFSIZE];

	// Close the write end of the pipe before reading from the
	// read end of the pipe.
	if (!CloseHandle(m_hChildStdoutWr)) wxLogDebug(wxT("Closing handle failed"));

	// Read output from the child process
	for (;;)
	{
		if( !ReadFile( m_hChildStdoutRd, chBuf, BUFSIZE, &dwRead, NULL) || dwRead == 0) {
			//wxLogDebug(wxT("ReadFromPipe failed %d"), dwRead);
			break;
		}

		if (m_isTerminated) break;

		m_output.insert(m_output.end(), chBuf, chBuf+dwRead);
		//wxLogDebug(wxT("ReadFromPipe %d"), dwRead);
	}
}

#else
bool cxExecute::cxExecuteThread::CreateChildProcess()
{
	const char *env = m_env.GetEnvBlock();

	// Create the env array
	vector<const char *> environ_p;
	for(const char *envp_cursor = env; *envp_cursor != '\0'; envp_cursor += strlen(envp_cursor)+1) {
		environ_p.push_back(envp_cursor);
	}
	environ_p.push_back(NULL);

	// Create the child process.
	m_pid = fork();
	if (m_pid < 0) {
		wxLogDebug(wxT("fork failed"));
		return false;
	} else if (m_pid == 0) {
		// child - set up handles and run command
		close(m_stdin[1]);
		dup2(m_stdin[0], 0);
		close(m_stdin[0]);

		close(m_stdout[0]);
		dup2(m_stdout[1], 1);
		dup2(m_stdout[1], 2);
		close(m_stdout[1]);

		if (! m_cwd.empty()) {
			chdir(m_cwd.utf8_str());
		}

		const char * argv[] = {"/bin/sh", "-c", strdup(m_command.utf8_str()), NULL}; // would be nice to parse it ourselves, or better yet convert callers to pass argument here
		execve(argv[0], (char**)argv, (char**) &*environ_p.begin());
		exit(-1); // only gets executed if execve failed
	} else {
		// parent - close child side of handles
		close(m_stdin[0]);
		close(m_stdout[1]);

		wxLogDebug(wxT("  started process %d"), m_pid);
	}
	return true;
}

void cxExecute::cxExecuteThread::WriteToPipe()
{
	// Write input to to the process's stdIn pipe.
	if (!m_input.empty()) {
		unsigned int bytesWritten = 0;

		while (bytesWritten < m_input.size()) {
			if (m_isTerminated) break;

			int written = write(m_stdin[1], &m_input[bytesWritten], wxMin(BUFSIZE, m_input.size()-bytesWritten));
			if (written < 0) {
				//wxLogDebug(wxT("WriteToPipe failed %d"), written);
				break;
			}

			bytesWritten += written;
			//wxLogDebug(wxT("WriteToPipe %d"), written);
		}
	}

	// Close the pipe handle so the child process stops reading.
	close(m_stdin[1]);
}

void cxExecute::cxExecuteThread::ReadFromPipe()
{
	char chBuf[BUFSIZE];

	// Read output from the child process
	for (;;)
	{
		int _read = read(m_stdout[0], chBuf, BUFSIZE);
		if (_read < 0) {
			//wxLogDebug(wxT("ReadFromPipe failed %d"), _read);
			break;
		} else if (_read == 0) {
			break; // pipe closed
		}

		if (m_isTerminated) break;

		m_output.insert(m_output.end(), chBuf, chBuf+_read);
		//wxLogDebug(wxT("ReadFromPipe %d"), _read);
	}
}
#endif // __WXMSW__
