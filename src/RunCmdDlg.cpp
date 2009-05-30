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

#include "RunCmdDlg.h"

#include <wx/gbsizer.h>

#include "tmCommand.h"
#include "eSettings.h"

// control ids
enum
{
	COMMAND_BOX = 100,
};


class CommandEvtHandler : public wxEvtHandler {
public:
	CommandEvtHandler(RunCmdDlg* parent): m_parent(parent) {};
private:
	void OnClose(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	RunCmdDlg* m_parent;
};

BEGIN_EVENT_TABLE(CommandEvtHandler, wxEvtHandler)
	EVT_BUTTON(wxID_OK, CommandEvtHandler::OnClose)
END_EVENT_TABLE()

void CommandEvtHandler::OnClose(wxCommandEvent& event){
	m_parent->UpdateCommandHistory();
	 event.Skip();
}

RunCmdDlg::RunCmdDlg(wxWindow *parent, eSettings& settings):
	wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
	m_settings(settings)
{
	SetTitle (_("Filter Through Command"));

	wxGridBagSizer* gridBagSizer = new wxGridBagSizer(5,5);

	wxStaticText* cmdLabel = new wxStaticText(this, wxID_ANY, _("Command:"));
	gridBagSizer->Add(cmdLabel, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);

	m_cmdCtrl = new wxComboBox(this, COMMAND_BOX);
	RefreshCommandHistory();

	gridBagSizer->Add(m_cmdCtrl, wxGBPosition(0,1), wxGBSpan(1,2), wxGROW);

	wxArrayString inputOptions;
	inputOptions.Add(_("None"));
	inputOptions.Add(_("Selection"));
	inputOptions.Add(_("Document"));
	m_inputBox = new wxRadioBox(this, wxID_ANY, _("Input"), wxDefaultPosition, wxDefaultSize, inputOptions, 1);
	m_inputBox->SetSelection(1);
	gridBagSizer->Add(m_inputBox, wxGBPosition(1,1));

	wxArrayString outputOptions;
	outputOptions.Add(_("Discard"));
	outputOptions.Add(_("Replace Selection"));
	outputOptions.Add(_("Replace Document"));
	outputOptions.Add(_("Insert as Text"));
	outputOptions.Add(_("Insert as Snippet"));
	outputOptions.Add(_("Show as HTML"));
	outputOptions.Add(_("Show as Tooltip"));
	outputOptions.Add(_("Create New Document"));
	m_outputBox = new wxRadioBox(this, wxID_ANY, _("Output"), wxDefaultPosition, wxDefaultSize, outputOptions, 1);
	m_outputBox->SetSelection(1);
	gridBagSizer->Add(m_outputBox, wxGBPosition(1,2), wxDefaultSpan, wxGROW);

	wxButton* cancelButton = new wxButton(this, wxID_CANCEL);

	m_executeButton = new wxButton(this, wxID_OK, _("Execute"));
	m_executeButton->SetDefault();
	m_executeButton->PushEventHandler(new CommandEvtHandler(this));

	wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer;
	buttonSizer->AddButton(cancelButton);
	buttonSizer->AddButton(m_executeButton);
	buttonSizer->Realize();
	gridBagSizer->Add(buttonSizer, wxGBPosition(2,1), wxGBSpan(1,2), wxALIGN_BOTTOM);

	gridBagSizer->AddGrowableCol(2);
	gridBagSizer->AddGrowableRow(2);

	SetSizer(gridBagSizer);
	gridBagSizer->SetSizeHints(this);
	Centre();
}

RunCmdDlg::~RunCmdDlg() {
	m_executeButton->PopEventHandler(true);
}

tmCommand RunCmdDlg::GetCommand() const {
	tmCommand cmd;

	const wxString cmdStr = m_cmdCtrl->GetValue();
	cmd.name = cmdStr;

	const wxCharBuffer buf = cmdStr.mb_str(wxConvUTF8);
	const size_t len = strlen(buf.data());
	vector<char> content(buf.data(), buf.data()+len);
	cmd.SwapContent(content);

	switch(m_inputBox->GetSelection()) {
	case 0:
		cmd.input = tmCommand::ciNONE;
		break;
	case 1:
		cmd.input = tmCommand::ciSEL;
		break;
	case 2:
		cmd.input = tmCommand::ciDOC;
		break;
	}

	switch(m_outputBox->GetSelection()) {
	case 0:
		cmd.output = tmCommand::coNONE;
		break;
	case 1:
		cmd.output = tmCommand::coSEL;
		break;
	case 2:
		cmd.output = tmCommand::coDOC;
		break;
	case 3:
		cmd.output = tmCommand::coINSERT;
		break;
	case 4:
		cmd.output = tmCommand::coSNIPPET;
		break;
	case 5:
		cmd.output = tmCommand::coHTML;
		break;
	case 6:
		cmd.output = tmCommand::coTOOLTIP;
		break;
	case 7:
		cmd.output = tmCommand::coNEWDOC;
		break;
	}

	return cmd;
}

void RunCmdDlg::RefreshCommandHistory() {
	// Clear() also deletes the text in search box, so we have to cache it
	const wxString repText = m_cmdCtrl->GetValue();
	m_cmdCtrl->Clear();
	m_cmdCtrl->SetValue(repText);

	for (size_t i = 0; i < m_settings.GetFilterCommandHistoryCount(); i++) {
		const wxString pattern = m_settings.GetFilterCommand(i);
		m_cmdCtrl->Append(pattern);
	}
}

void RunCmdDlg::UpdateCommandHistory() {
	if (m_cmdCtrl->GetValue().IsEmpty()) return;

	if (m_settings.AddFilterCommand(m_cmdCtrl->GetValue())) {
		RefreshCommandHistory();
	}
}
