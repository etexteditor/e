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

RunCmdDlg::RunCmdDlg(wxWindow *parent)
:  wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {
	SetTitle (_("Filter Through Command"));

	wxGridBagSizer* gridBagSizer = new wxGridBagSizer(5,5);

	wxStaticText* cmdLabel = new wxStaticText(this, wxID_ANY, _("Command:"));
	gridBagSizer->Add(cmdLabel, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);

	m_cmdCtrl = new wxTextCtrl(this, wxID_ANY);
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
	wxButton* executeButton = new wxButton(this, wxID_OK, _("Execute"));
	executeButton->SetDefault();
	wxStdDialogButtonSizer* buttonSizer = new wxStdDialogButtonSizer;
	buttonSizer->AddButton(cancelButton);
	buttonSizer->AddButton(executeButton);
	buttonSizer->Realize();
	gridBagSizer->Add(buttonSizer, wxGBPosition(2,1), wxGBSpan(1,2), wxALIGN_BOTTOM);

	gridBagSizer->AddGrowableCol(2);
	gridBagSizer->AddGrowableRow(2);

	SetSizer(gridBagSizer);
	gridBagSizer->SetSizeHints(this);
	Centre();
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
