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

#include "SaveDlg.h"

BEGIN_EVENT_TABLE(SaveDlg, wxDialog)
	EVT_BUTTON(wxID_YES, SaveDlg::OnYes)
	EVT_BUTTON(wxID_NO, SaveDlg::OnNo)
END_EVENT_TABLE()

SaveDlg::SaveDlg(wxWindow *parent, const wxArrayString& paths)
: wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ) {
	SetTitle (_("Save modified files"));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(new wxStaticText(this, -1, _("Do you want to save changes to modified files?")), 0, wxALL, 5);

	checklist = new wxCheckListBox(this, -1,wxDefaultPosition, wxDefaultSize, paths);
	for (unsigned int i = 0; i < checklist->GetCount(); ++i) {
		checklist->Check(i);
	}
	sizer->Add(checklist, 1, wxEXPAND|wxLEFT|wxRIGHT, 5);

	// Buttons
	sizer->Add(CreateButtonSizer(wxYES|wxNO|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	SetSizerAndFit(sizer);

	Centre();
}

int SaveDlg::GetCount() const {
	return checklist->GetCount();
}

bool SaveDlg::IsChecked(int item) const {
	return checklist->IsChecked(item);
}


void SaveDlg::OnYes(wxCommandEvent& WXUNUSED(event)) {
	EndModal(wxID_YES);
}

void SaveDlg::OnNo(wxCommandEvent& WXUNUSED(event)) {
	EndModal(wxID_NO);
}
