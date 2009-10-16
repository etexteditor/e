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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/file.h>
	#include <wx/event.h>
#endif

#include <vector>

class cxEnv;
class wxProcessEvent;

class cxExecute : public wxEvtHandler {
public:
	cxExecute(const cxEnv& env, const wxString& cwd=wxEmptyString):
		m_threadDone(false), m_env(env), m_cwd(cwd), m_debugLog(false), m_showWindow(false), m_updateWindow(true) {};

	int Execute(const wxString& command);
	int Execute(const wxString& command, const std::vector<char>& input);
	std::vector<char>& GetOutput() {return m_output;};
	std::vector<char>& GetErrorOut() {return m_errout;};

	void SetDebugLogging(bool doLog) {m_debugLog = doLog;};
	void SetShowWindow(bool doShow) {m_showWindow = doShow;};
	void SetUpdateWindow(bool doUpdate) {m_updateWindow = doUpdate;};

	void ThreadDone(int exitCode);

private:
	void OnEndProcess(wxProcessEvent& event);
	DECLARE_EVENT_TABLE();

	bool m_threadDone;
	int m_exitCode;
	std::vector<char> m_output;
	std::vector<char> m_errout;
	const cxEnv& m_env;
	const wxString& m_cwd;
	bool m_debugLog;
	bool m_showWindow;
	bool m_updateWindow;
};

#endif // __EXECPROCESS_H__
