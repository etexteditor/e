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

#ifndef __PROJECTSETTINGS_H__
#define __PROJECTSETTINGS_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/dialog.h>
#endif

class cxProjectInfo;
class wxCheckBox;
class wxTextCtrl;
class wxCommandEvent;
class EnvVarsPanel;

class ProjectSettings : public wxDialog {
public:
	ProjectSettings(wxWindow* parent, const cxProjectInfo& project, const cxProjectInfo& parentProject);

	bool IsModified() const;
	void GetSettings(cxProjectInfo& project) const;

private:
	void OnInheritCheck(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()

	// Member varibles
	const cxProjectInfo& m_projectInfo;
	const cxProjectInfo& m_parentProject;
	bool m_envModified;

	// Controls
	wxCheckBox* m_inheritCheck;
	wxTextCtrl* m_includeDirs;
	wxTextCtrl* m_includeFiles;
	wxTextCtrl* m_excludeDirs;
	wxTextCtrl* m_excludeFiles;
	EnvVarsPanel* m_envPage;
};

#endif // __PROJECTSETTINGS_H__
