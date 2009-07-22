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

#include "CompareDlg.h"

BEGIN_EVENT_TABLE(CompareDlg, wxDialog)
	EVT_BUTTON(wxID_OK, CompareDlg::OnButtonOk)
END_EVENT_TABLE()


CompareDlg::CompareDlg(wxWindow *parent)
: wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ) {
	SetTitle (_("Compare Files..."));

	// Create ctrls
	m_leftPathCtrl = new wxFilePickerCtrl(this, wxID_ANY);
	m_rightPathCtrl = new wxFilePickerCtrl(this, wxID_ANY);

	// Create layout
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		wxFlexGridSizer* pathsizer = new wxFlexGridSizer(2, 2);
			pathsizer->AddGrowableCol(1, 1);
			pathsizer->Add(new wxStaticText(this, wxID_ANY, _("Left:")), 0, wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(m_leftPathCtrl, 0, wxEXPAND);
			pathsizer->Add(new wxStaticText(this, wxID_ANY, _("Right:")), 0, wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(m_rightPathCtrl, 0, wxEXPAND);
			sizer->Add(pathsizer, 0, wxEXPAND|wxALL, 5);
		sizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	SetSizerAndFit(sizer);
	SetSize(400, -1);
}

void CompareDlg::OnButtonOk(wxCommandEvent& evt) {
	const wxString leftPath = m_leftPathCtrl->GetPath();
	if (leftPath.empty() || !wxFileExists(leftPath)) {
		wxMessageBox(_("Left path is not valid"), _("Error"), wxICON_ERROR);
		return;
	}

	const wxString rightPath = m_rightPathCtrl->GetPath();
	if (rightPath.empty() || !wxFileExists(rightPath)) {
		wxMessageBox(_("Right path is not valid"), _("Error"), wxICON_ERROR);
		return;
	}

	// If we get here the paths are valid
	evt.Skip(); 
}
