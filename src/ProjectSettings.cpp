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

#include "ProjectSettings.h"

#include <wx/notebook.h>
#include <wx/tokenzr.h>
#include <wx/grid.h>

#include "EnvVarsPanel.h"
#include "ProjectInfo.h"
#include "Strings.h"

// ctrl ids
enum {
	ID_INHERIT,
};

BEGIN_EVENT_TABLE(ProjectSettings, wxDialog)
	EVT_CHECKBOX(ID_INHERIT, ProjectSettings::OnInheritCheck)
END_EVENT_TABLE()

ProjectSettings::ProjectSettings(wxWindow* parent, const cxProjectInfo& project, const cxProjectInfo& parentProject)
: wxDialog (parent, wxID_ANY, _("Project Settings"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
  m_projectInfo(project), m_parentProject(parentProject), m_envModified(false)
{
	// Create the notebook
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
	{
		// Create the filter page
		wxPanel* filterPage = new wxPanel(notebook, wxID_ANY);
		{
			m_inheritCheck = new wxCheckBox(filterPage, ID_INHERIT, _("Inherit from parent"));
			m_includeDirs = new wxTextCtrl(filterPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
			m_includeFiles = new wxTextCtrl(filterPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
			m_excludeDirs = new wxTextCtrl(filterPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
			m_excludeFiles = new wxTextCtrl(filterPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

			// Layout
			wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
				pageSizer->Add(m_inheritCheck, 0, wxALL, 5);
				if (project.IsRoot()) pageSizer->Hide(m_inheritCheck);

				wxFlexGridSizer* filterSizer = new wxFlexGridSizer(4, 2, 0, 0);
				filterSizer->AddGrowableCol(0);
				filterSizer->AddGrowableCol(1);
				filterSizer->AddGrowableRow(1);
				filterSizer->AddGrowableRow(3);
					filterSizer->Add(new wxStaticText(filterPage, wxID_ANY, _("Include Dirs:")), 0, wxALL, 5);
					filterSizer->Add(new wxStaticText(filterPage, wxID_ANY, _("Include Files:")), 0, wxALL, 5);
					filterSizer->Add(m_includeDirs, 1, wxEXPAND|wxALL, 5);
					filterSizer->Add(m_includeFiles, 1, wxEXPAND|wxALL, 5);
					filterSizer->Add(new wxStaticText(filterPage, wxID_ANY, _("Exclude Dirs:")), 0, wxALL, 5);
					filterSizer->Add(new wxStaticText(filterPage, wxID_ANY, _("Exclude Files:")), 0, wxALL, 5);
					filterSizer->Add(m_excludeDirs, 1, wxEXPAND|wxALL, 5);
					filterSizer->Add(m_excludeFiles, 1, wxEXPAND|wxALL, 5);
					pageSizer->Add(filterSizer, 1, wxEXPAND|wxALL, 5);

			filterPage->SetSizer(pageSizer);
			notebook->AddPage(filterPage, _("Filters"), true);
		}

		if (project.IsRoot()) {
			m_envPage = new EnvVarsPanel(notebook);
			notebook->AddPage(m_envPage, _("Environment"), true);
		}

		mainSizer->Add(notebook, 1, wxEXPAND|wxALL, 5);
	}
	// Buttons
	mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	// Load filters
	const cxProjectInfo *projectToLoad = NULL;

	if (project.HasFilters() || project.IsRoot()) {
		projectToLoad = &project;
	}
	else {
		projectToLoad = &parentProject;

		m_includeDirs->Disable();
		m_excludeDirs->Disable();
		m_includeFiles->Disable();
		m_excludeFiles->Disable();
		m_inheritCheck->SetValue(true);
	}

	if (projectToLoad) {
		const wxString ind = wxJoin(projectToLoad->includeDirs, wxT('\n'), NULL);
		m_includeDirs->SetValue(ind);

		const wxString inf = wxJoin(projectToLoad->includeFiles, wxT('\n'), NULL);
		m_includeFiles->SetValue(inf);

		const wxString exd = wxJoin(projectToLoad->includeDirs, wxT('\n'), NULL);
		m_excludeDirs->SetValue(exd);

		const wxString exf = wxJoin(projectToLoad->includeFiles, wxT('\n'), NULL);
		m_excludeFiles->SetValue(exf);
	}

	// Load env variables
	if (project.IsRoot()) {
		for (map<wxString,wxString>::const_iterator p = project.env.begin(); p != project.env.end(); ++p) {
			//const unsigned int rowId = m_envList->GetNumberRows();
			//m_envList->InsertRows(rowId);
			//m_envList->SetCellValue(rowId, 0, p->first);
			//m_envList->SetCellValue(rowId, 1, p->second);
		}
	}

	notebook->ChangeSelection(0);

	SetSizerAndFit(mainSizer);
	SetSize(350, 300);
	CentreOnParent();
}

bool ProjectSettings::IsModified() const {
	if (m_inheritCheck->GetValue() == m_projectInfo.HasFilters()) return true;
	if (m_includeDirs->IsModified()) return true;
	if (m_excludeDirs->IsModified()) return true;
	if (m_includeFiles->IsModified()) return true;
	if (m_excludeFiles->IsModified()) return true;

	if (m_projectInfo.IsRoot() && m_envPage->VarsChanged()) return true;

	return false;
}

void ProjectSettings::GetSettings(cxProjectInfo& project) const {
	project.ClearFilters();

	if (!m_inheritCheck->GetValue()) {
		const wxArrayString ind = wxSplit(m_includeDirs->GetValue(), wxT('\n'), NULL);
		const wxArrayString exd = wxSplit(m_excludeDirs->GetValue(), wxT('\n'), NULL);
		const wxArrayString inf = wxSplit(m_includeFiles->GetValue(), wxT('\n'), NULL);
		const wxArrayString exf = wxSplit(m_excludeFiles->GetValue(), wxT('\n'), NULL);
		project.SetFilters(ind, exd, inf, exf);
	}

	if (m_projectInfo.IsRoot()) {
		project.env.clear();

		//for (int i = 0; i < m_envList->GetNumberRows(); ++i) {
		//	project.env[m_envList->GetCellValue(i, 0)] = m_envList->GetCellValue(i, 1);
		//}
	}
}

void ProjectSettings::OnInheritCheck(wxCommandEvent& event) {
	if (event.IsChecked()) {
		m_includeDirs->Clear();
		m_includeFiles->Clear();
		m_excludeDirs->Clear();
		m_excludeFiles->Clear();

		const wxString ind = wxJoin(m_parentProject.includeDirs, wxT('\n'), NULL);
		m_includeDirs->SetValue(ind);

		const wxString inf = wxJoin(m_parentProject.includeFiles, wxT('\n'), NULL);
		m_includeFiles->SetValue(inf);

		const wxString exd = wxJoin(m_parentProject.excludeDirs, wxT('\n'), NULL);
		m_excludeDirs->SetValue(exd);

		const wxString exf = wxJoin(m_parentProject.excludeFiles, wxT('\n'), NULL);
		m_excludeFiles->SetValue(exf);

		m_includeDirs->Disable();
		m_excludeDirs->Disable();
		m_includeFiles->Disable();
		m_excludeFiles->Disable();
	}
	else {
		m_includeDirs->Enable();
		m_excludeDirs->Enable();
		m_includeFiles->Enable();
		m_excludeFiles->Enable();
	}
}
