/*******************************************************************************
 *
 * Copyright (C) 2010, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#include "MacroPane.h"
#include "Macro.h"
#include "EditorFrame.h"
#include <wx/artprov.h>

// Ctrl id's
enum {
	ID_CMDLIST = 100,
	ID_BUTTON_REC,
	ID_BUTTON_NEW,
	ID_BUTTON_DEL,
	ID_BUTTON_UP,
	ID_BUTTON_DOWN,
	ID_BUTTON_SAVE,
	ID_ARG_GRID
};

BEGIN_EVENT_TABLE(MacroPane, wxPanel)
	EVT_BUTTON(ID_BUTTON_REC, MacroPane::OnButtonRec)
	EVT_BUTTON(ID_BUTTON_DEL, MacroPane::OnButtonDel)
	EVT_BUTTON(ID_BUTTON_UP, MacroPane::OnButtonUp)
	EVT_BUTTON(ID_BUTTON_DOWN, MacroPane::OnButtonDown)
	EVT_BUTTON(ID_BUTTON_SAVE, MacroPane::OnButtonSave)
	EVT_LISTBOX(ID_CMDLIST, MacroPane::OnCmdList)
	EVT_GRID_CMD_CELL_CHANGE(ID_ARG_GRID, MacroPane::OnGridChanged)
	EVT_IDLE(MacroPane::OnIdle)
END_EVENT_TABLE()

MacroPane::MacroPane(EditorFrame& frame, wxWindow* parent, eMacro& macro)
: wxPanel(parent, wxID_ANY), m_parentFrame(frame), m_recState(false), m_macro(macro)
{
	// Create ctrls
	m_bitmapStartRec = wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_BUTTON);
	m_bitmapStopRec = wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON);
	m_buttonRec = new wxBitmapButton(this, ID_BUTTON_REC, m_bitmapStartRec);
	//m_buttonNew = new wxBitmapButton(this, ID_BUTTON_NEW, wxArtProvider::GetBitmap(wxART_NEW, wxART_BUTTON));
	m_buttonDel = new wxBitmapButton(this, ID_BUTTON_DEL, wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON));
	m_buttonUp = new wxBitmapButton(this, ID_BUTTON_UP, wxArtProvider::GetBitmap(wxART_GO_UP, wxART_BUTTON));
	m_buttonDown = new wxBitmapButton(this, ID_BUTTON_DOWN, wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_BUTTON));
	m_buttonSave = new wxBitmapButton(this, ID_BUTTON_SAVE, wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON));
	m_cmdList = new wxListBox(this, ID_CMDLIST);
	m_argsGrid = new wxGrid(this, ID_ARG_GRID);

	UpdateButtons();

	// Configure grid
	m_argsGrid->CreateGrid(0, 2);
	m_argsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);
	m_argsGrid->SetRowLabelSize(0);
	m_argsGrid->SetColLabelSize(0);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
			buttonSizer->Add(m_buttonRec, 0, wxALIGN_LEFT);
			buttonSizer->AddStretchSpacer();
			//buttonSizer->Add(m_buttonNew, 0, wxALIGN_RIGHT);
			buttonSizer->Add(m_buttonDel, 0, wxALIGN_RIGHT);
			buttonSizer->Add(m_buttonUp, 0, wxALIGN_RIGHT);
			buttonSizer->Add(m_buttonDown, 0, wxALIGN_RIGHT);
			buttonSizer->Add(m_buttonSave, 0, wxALIGN_RIGHT);
			mainSizer->Add(buttonSizer, 0, wxEXPAND);
		mainSizer->Add(m_cmdList, 3, wxEXPAND);
		mainSizer->Add(m_argsGrid, 1, wxEXPAND);

	SetSizer(mainSizer);
}

void MacroPane::UpdateButtons() {
	const int ndx = m_cmdList->GetSelection();
	if (ndx == wxNOT_FOUND) {
		m_buttonDel->Disable();
		m_buttonUp->Disable();
		m_buttonDown->Disable();
	}
	else {
		m_buttonDel->Enable();
		m_buttonUp->Enable(ndx > 0);
		m_buttonDown->Enable(ndx+1 < (int)m_macro.GetCount());
	}
}

void MacroPane::UpdateArgsGrid() {
	if (m_argsGrid->GetNumberRows()) {
		m_argsGrid->DeleteRows(0, m_argsGrid->GetNumberRows());
	}

	const int ndx = m_cmdList->GetSelection();

	if (ndx != wxNOT_FOUND) {
		const int ndx = m_cmdList->GetSelection();
		const eMacroCmd& cmd = m_macro.GetCommand(ndx);

		m_argsGrid->AppendRows(cmd.GetArgCount());

		for (size_t i = 0; i < cmd.GetArgCount(); ++i) {
			const wxString& name = cmd.GetArgName(i);
			const wxVariant& arg = cmd.GetArg(i);

			m_argsGrid->SetCellValue(i, 0, name);
			m_argsGrid->SetReadOnly(i, 0);

			if (arg.GetType() == wxT("bool")) {
				m_argsGrid->SetCellRenderer(i, 1, new wxGridCellBoolRenderer);
				m_argsGrid->SetCellEditor(i, 1, new wxGridCellBoolEditor);
				if (arg.GetBool()) m_argsGrid->SetCellValue(i, 1, wxT("1"));
			}
			else if (arg.GetType() == wxT("string")) m_argsGrid->SetCellValue(i, 1, arg.GetString());
		}
	}
}

void MacroPane::OnMacroChanged() {
	for (size_t i = m_cmdList->GetCount(); i < m_macro.GetCount(); ++i) {
		const eMacroCmd& cmd = m_macro.GetCommand(i);
		m_cmdList->Append(cmd.GetName());
	}
}

void MacroPane::OnButtonRec(wxCommandEvent& WXUNUSED(evt)) {
	if (m_recState) m_macro.EndRecording();
	else {
		m_macro.StartRecording();
		m_parentFrame.FocusEditor();
	}
}

void MacroPane::OnButtonDel(wxCommandEvent& WXUNUSED(evt)) {
	const int ndx = m_cmdList->GetSelection();
	if (ndx == wxNOT_FOUND) return;

	m_macro.Delete(ndx);
}

void MacroPane::OnButtonUp(wxCommandEvent& WXUNUSED(evt)) {
	const int ndx = m_cmdList->GetSelection();
	if (ndx == wxNOT_FOUND) return;

	m_cmdList->Select(ndx-1);
	m_macro.MoveUp(ndx);
}

void MacroPane::OnButtonDown(wxCommandEvent& WXUNUSED(evt)) {
	const int ndx = m_cmdList->GetSelection();
	if (ndx == wxNOT_FOUND) return;

	m_cmdList->Select(ndx+1);
	m_macro.MoveDown(ndx);
}

void MacroPane::OnButtonSave(wxCommandEvent& WXUNUSED(evt)) {
	m_parentFrame.SaveMacro();
}

void MacroPane::OnCmdList(wxCommandEvent& WXUNUSED(evt)) {
	UpdateArgsGrid();
	UpdateButtons();
}

void MacroPane::OnGridChanged(wxGridEvent& evt) {
	const int cmd_ndx = m_cmdList->GetSelection();
	if (cmd_ndx == wxNOT_FOUND) return;
	const int arg_ndx = evt.GetRow();

	eMacroCmd& cmd = m_macro.GetCommand(cmd_ndx);
	wxVariant& arg = cmd.GetArg(arg_ndx);
	const wxString val = m_argsGrid->GetCellValue(arg_ndx, 1);

	if (arg.GetType() == wxT("bool")) {
		arg = (val != wxT("0"));
	}
	else if (arg.GetType() == wxT("string")) {
		arg = val;
	}
}

void MacroPane::OnIdle(wxIdleEvent& WXUNUSED(evt)) {
	if (!IsShown()) return;

	if (m_macro.IsRecording()) {
		if (!m_recState) {
			m_buttonRec->SetBitmapLabel(m_bitmapStopRec);
			m_buttonRec->SetToolTip(_("Stop Recording"));
			m_recState = true;
		}
	}
	else {
		if (m_recState) {
			m_buttonRec->SetBitmapLabel(m_bitmapStartRec);
			m_buttonRec->SetToolTip(_("Start Recording"));
			m_recState = false;
		}
	}

	const bool canSave = m_parentFrame.IsBundlePaneShownAndSelected();
	if (m_buttonSave->IsEnabled() != canSave) m_buttonSave->Enable(canSave);

	if (m_macro.IsModified()) {
		const int sel = m_cmdList->GetSelection();
		m_cmdList->Clear();

		for (size_t i = 0; i < m_macro.GetCount(); ++i) {
			const eMacroCmd& cmd = m_macro.GetCommand(i);
			m_cmdList->Append(cmd.GetName());
		}

		// Re-select item
		if (sel != wxNOT_FOUND && sel < (int)m_cmdList->GetCount()) {
			m_cmdList->Select(sel);
		}
		UpdateButtons();
		UpdateArgsGrid();

		m_macro.ResetMod();
	}
}