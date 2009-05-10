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
#include "ProjectInfo.h"
#include <wx/notebook.h>
#include <wx/tokenzr.h>
#include <wx/grid.h>

// ctrl ids
enum {
	ID_INHERIT,
	ID_BUTTON_ADD,
	ID_BUTTON_DEL,
	ID_ENVLIST
};

BEGIN_EVENT_TABLE(ProjectSettings, wxDialog)
	EVT_CHECKBOX(ID_INHERIT, ProjectSettings::OnInheritCheck)
	EVT_BUTTON(ID_BUTTON_ADD, ProjectSettings::OnButtonAddEnv)
	EVT_BUTTON(ID_BUTTON_DEL, ProjectSettings::OnButtonDelEnv)
	EVT_GRID_CMD_CELL_CHANGE(ID_ENVLIST, ProjectSettings::OnGridChange)
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
			// Create the environment variable page
			wxPanel* envPage = new wxPanel(notebook, wxID_ANY);
			{
				m_envList = new wxGrid(envPage, ID_ENVLIST, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxSUNKEN_BORDER);
				wxButton* addButton = new wxButton(envPage, ID_BUTTON_ADD, wxT("+"), wxDefaultPosition, wxSize(45, -1));
				wxButton* delButton = new wxButton(envPage, ID_BUTTON_DEL, wxT("-"), wxDefaultPosition, wxSize(45, -1));

				m_envList->CreateGrid(0, 2);
				m_envList->SetColLabelValue(0, _("Key"));
				m_envList->SetColLabelValue(1, _("Value"));
				m_envList->SetMargins(0,0);
				m_envList->SetRowLabelSize(0);
				m_envList->SetColLabelSize(19);
				m_envList->EnableEditing(true);
				m_envList->EnableGridLines(true);
				m_envList->EnableDragColSize(true);
				m_envList->EnableDragRowSize(false);
				m_envList->EnableDragGridSize(false);
				m_envList->SetSelectionMode(wxGrid::wxGridSelectRows);

				m_envList->SetColSize(0, 100);
				m_envList->SetColSize(1, 230);

				// Layout
				wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
					pageSizer->Add(m_envList, 1, wxEXPAND|wxALL, 5);
					wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
						buttonSizer->Add(addButton, 0, wxRIGHT, 5);
						buttonSizer->Add(delButton, 0);
						pageSizer->Add(buttonSizer, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5);

				envPage->SetSizer(pageSizer);
				notebook->AddPage(envPage, _("Environment"), true);
			}
		}

		mainSizer->Add(notebook, 1, wxEXPAND|wxALL, 5);
	}
	// Buttons
	mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	// Load filters
	if (project.hasFilters || project.IsRoot()) {
		unsigned int i = 0;
		for (i = 0; i < project.includeDirs.GetCount(); ++i) {
			if (i) m_includeDirs->AppendText(wxT("\n"));
			m_includeDirs->AppendText(project.includeDirs[i]);
		}
		for (i = 0; i < project.includeFiles.GetCount(); ++i) {
			if (i) m_includeFiles->AppendText(wxT("\n"));
			m_includeFiles->AppendText(project.includeFiles[i]);
		}
		for (i = 0; i < project.excludeDirs.GetCount(); ++i) {
			if (i) m_excludeDirs->AppendText(wxT("\n"));
			m_excludeDirs->AppendText(project.excludeDirs[i]);
		}
		for (i = 0; i < project.excludeFiles.GetCount(); ++i) {
			if (i) m_excludeFiles->AppendText(wxT("\n"));
			m_excludeFiles->AppendText(project.excludeFiles[i]);
		}
	}
	else {
		unsigned int i = 0;
		for (i = 0; i < parentProject.includeDirs.GetCount(); ++i) {
			if (i) m_includeDirs->AppendText(wxT("\n"));
			m_includeDirs->AppendText(parentProject.includeDirs[i]);
		}
		for (i = 0; i < parentProject.includeFiles.GetCount(); ++i) {
			if (i) m_includeFiles->AppendText(wxT("\n"));
			m_includeFiles->AppendText(parentProject.includeFiles[i]);
		}
		for (i = 0; i < parentProject.excludeDirs.GetCount(); ++i) {
			if (i) m_excludeDirs->AppendText(wxT("\n"));
			m_excludeDirs->AppendText(parentProject.excludeDirs[i]);
		}
		for (i = 0; i < parentProject.excludeFiles.GetCount(); ++i) {
			if (i) m_excludeFiles->AppendText(wxT("\n"));
			m_excludeFiles->AppendText(parentProject.excludeFiles[i]);
		}

		m_includeDirs->Disable();
		m_excludeDirs->Disable();
		m_includeFiles->Disable();
		m_excludeFiles->Disable();
		m_inheritCheck->SetValue(true);
	}

	// Load env variables
	if (project.IsRoot()) {
		for (map<wxString,wxString>::const_iterator p = project.env.begin(); p != project.env.end(); ++p) {
			const unsigned int rowId = m_envList->GetNumberRows();
			m_envList->InsertRows(rowId);
			m_envList->SetCellValue(rowId, 0, p->first);
			m_envList->SetCellValue(rowId, 1, p->second);
		}
	}

	notebook->ChangeSelection(0);

	SetSizerAndFit(mainSizer);
	SetSize(350, 300);
	CentreOnParent();
}

bool ProjectSettings::IsModified() const {
	if (m_inheritCheck->GetValue() == m_projectInfo.hasFilters) return true;
	if (m_includeDirs->IsModified()) return true;
	if (m_excludeDirs->IsModified()) return true;
	if (m_includeFiles->IsModified()) return true;
	if (m_excludeFiles->IsModified()) return true;

	if (m_projectInfo.IsRoot() && m_envModified) return true;

	return false;
}

void ProjectSettings::GetSettings(cxProjectInfo& project) const {
	project.includeDirs.Empty();
	project.excludeDirs.Empty();
	project.includeFiles.Empty();
	project.excludeFiles.Empty();

	if (m_inheritCheck->GetValue()) {
		project.hasFilters = false;
	}
	else {
		project.hasFilters = true;

		wxStringTokenizer tokens(m_includeDirs->GetValue(), wxT("\n"));
		while (tokens.HasMoreTokens()) {
			const wxString token = tokens.GetNextToken();
			if (!token.empty()) project.includeDirs.Add(token);
		}
		tokens.SetString(m_excludeDirs->GetValue(), wxT("\n"));
		while (tokens.HasMoreTokens()) {
			const wxString token = tokens.GetNextToken();
			if (!token.empty()) project.excludeDirs.Add(token);
		}
		tokens.SetString(m_includeFiles->GetValue(), wxT("\n"));
		while (tokens.HasMoreTokens()) {
			const wxString token = tokens.GetNextToken();
			if (!token.empty()) project.includeFiles.Add(token);
		}
		tokens.SetString(m_excludeFiles->GetValue(), wxT("\n"));
		while (tokens.HasMoreTokens()) {
			const wxString token = tokens.GetNextToken();
			if (!token.empty()) project.excludeFiles.Add(token);
		}
	}

	if (m_projectInfo.IsRoot()) {
		project.env.clear();

		for (int i = 0; i < m_envList->GetNumberRows(); ++i) {
			project.env[m_envList->GetCellValue(i, 0)] = m_envList->GetCellValue(i, 1);
		}
	}
}

void ProjectSettings::OnInheritCheck(wxCommandEvent& event) {
	if (event.IsChecked()) {
		m_includeDirs->Clear();
		m_includeFiles->Clear();
		m_excludeDirs->Clear();
		m_excludeFiles->Clear();

		unsigned int i = 0;
		for (i = 0; i < m_parentProject.includeDirs.GetCount(); ++i) {
			if (i) m_includeDirs->AppendText(wxT("\n"));
			m_includeDirs->AppendText(m_parentProject.includeDirs[i]);
		}
		for (i = 0; i < m_parentProject.includeFiles.GetCount(); ++i) {
			if (i) m_includeFiles->AppendText(wxT("\n"));
			m_includeFiles->AppendText(m_parentProject.includeFiles[i]);
		}
		for (i = 0; i < m_parentProject.excludeDirs.GetCount(); ++i) {
			if (i) m_excludeDirs->AppendText(wxT("\n"));
			m_excludeDirs->AppendText(m_parentProject.excludeDirs[i]);
		}
		for (i = 0; i < m_parentProject.excludeFiles.GetCount(); ++i) {
			if (i) m_excludeFiles->AppendText(wxT("\n"));
			m_excludeFiles->AppendText(m_parentProject.excludeFiles[i]);
		}

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

void ProjectSettings::OnButtonAddEnv(wxCommandEvent& WXUNUSED(event)) {
	const unsigned int gridPos = m_envList->GetNumberRows();
	m_envList->InsertRows(gridPos);
	m_envList->SetGridCursor(gridPos, 0);
	m_envList->MakeCellVisible(gridPos, 0);
	m_envList->ForceRefresh();
	m_envList->EnableCellEditControl();
}

void ProjectSettings::OnButtonDelEnv(wxCommandEvent& WXUNUSED(event)) {
	const int rowId = m_envList->GetGridCursorRow();
	if (rowId != -1) {
		m_envList->DeleteRows(rowId);
		m_envList->ForceRefresh();
		m_envModified = true; // del does not send change event?
	}
}

void ProjectSettings::OnGridChange(wxGridEvent& WXUNUSED(event)) {
	m_envModified = true;
}
