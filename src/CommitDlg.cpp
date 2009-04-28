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

#include "CommitDlg.h"

CommitDlg::CommitDlg(wxWindow *parent)
: wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ) {
	SetTitle (_("Make Milestone"));
	
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	// Description
	sizer->Add(new wxStaticText(this, wxID_ANY, _("Comment:")), 0, wxALL, 5);
	m_descCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""),wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	sizer->Add(m_descCtrl, 1, wxEXPAND|wxLEFT|wxRIGHT, 5);

	// Label
	wxBoxSizer* labelSizer = new wxBoxSizer(wxHORIZONTAL);
	labelSizer->Add(new wxStaticText(this, wxID_ANY, _("Label:")), 0, wxALIGN_CENTER_VERTICAL);
	m_labelCtrl = new wxTextCtrl(this, wxID_ANY);
	labelSizer->Add(m_labelCtrl, 1, wxEXPAND|wxLEFT, 5);
	sizer->Add(labelSizer, 0 , wxEXPAND|wxALL, 5);

	// Buttons
	sizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);
	
	m_descCtrl->SetFocus();
	SetSizerAndFit(sizer);
	SetSize(350, 200);
}
