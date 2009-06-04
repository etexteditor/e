#include "EnvVarsPanel.h"
#include <wx/grid.h>

enum {
	ID_BUTTON_ADD,
	ID_BUTTON_DEL,
	ID_ENVLIST
};

BEGIN_EVENT_TABLE(EnvVarsPanel, wxPanel)
	EVT_BUTTON(ID_BUTTON_ADD, EnvVarsPanel::OnButtonAddEnv)
	EVT_BUTTON(ID_BUTTON_DEL, EnvVarsPanel::OnButtonDelEnv)
	EVT_GRID_CMD_CELL_CHANGE(ID_ENVLIST, EnvVarsPanel::OnGridChange)
END_EVENT_TABLE()


EnvVarsPanel::EnvVarsPanel(wxWindow*parent, wxWindowID id): 
	wxPanel(parent, id),
	m_varsChanged(false)
{
	m_envList = new wxGrid(this, ID_ENVLIST, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxSUNKEN_BORDER);
	wxButton* addButton = new wxButton(this, ID_BUTTON_ADD, wxT("+"), wxDefaultPosition, wxSize(45, -1));
	wxButton* delButton = new wxButton(this, ID_BUTTON_DEL, wxT("-"), wxDefaultPosition, wxSize(45, -1));

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

	wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
		pageSizer->Add(m_envList, 1, wxEXPAND|wxALL, 5);
		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
			buttonSizer->Add(addButton, 0, wxRIGHT, 5);
			buttonSizer->Add(delButton, 0);
			pageSizer->Add(buttonSizer, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5);

	this->SetSizer(pageSizer);
}

EnvVarsPanel::~EnvVarsPanel(void){}

void EnvVarsPanel::OnButtonAddEnv(wxCommandEvent& WXUNUSED(event)) {
	const unsigned int gridPos = m_envList->GetNumberRows();
	m_envList->InsertRows(gridPos);
	m_envList->SetGridCursor(gridPos, 0);
	m_envList->MakeCellVisible(gridPos, 0);
	m_envList->ForceRefresh();
	m_envList->EnableCellEditControl();
}

void EnvVarsPanel::OnButtonDelEnv(wxCommandEvent& WXUNUSED(event)) {
	const int rowId = m_envList->GetGridCursorRow();
	if (rowId != -1) {
		m_envList->DeleteRows(rowId);
		m_envList->ForceRefresh();
		m_varsChanged = true;
	}
}

void EnvVarsPanel::OnGridChange(wxGridEvent& WXUNUSED(event)) {
	m_varsChanged = true;
}

void EnvVarsPanel::AddVars(const map<wxString,wxString>& vars) {
	for (map<wxString,wxString>::const_iterator p = vars.begin(); p != vars.end(); ++p) {
		const unsigned int rowId = m_envList->GetNumberRows();
		m_envList->InsertRows(rowId);
		m_envList->SetCellValue(rowId, 0, p->first);
		m_envList->SetCellValue(rowId, 1, p->second);
	}
}

void EnvVarsPanel::GetVars(map<wxString,wxString>& vars) {
	for (int i = 0; i < m_envList->GetNumberRows(); ++i) {
		vars[m_envList->GetCellValue(i, 0)] = m_envList->GetCellValue(i, 1);
	}
}
