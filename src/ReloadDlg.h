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

#ifndef __RELOADDLG_H__
#define __RELOADDLG_H__

#include "wx/wxprec.h"

class ReloadDlg : public wxDialog {
public:
	ReloadDlg(wxWindow *parent, const wxArrayString& paths);

	// Access to checklistbox
	int GetCount() const;
	bool IsChecked(int item) const;

private:
	// Event handlers
	void OnYes(wxCommandEvent& event);
	void OnNo(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	wxCheckListBox* checklist;
};

BEGIN_EVENT_TABLE(ReloadDlg, wxDialog)
	EVT_BUTTON(wxID_YES, ReloadDlg::OnYes)
	EVT_BUTTON(wxID_NO, ReloadDlg::OnNo)
END_EVENT_TABLE()

ReloadDlg::ReloadDlg(wxWindow *parent, const wxArrayString& paths)
: wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ) {
	SetTitle (_("Files changed on disk"));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(new wxStaticText(this, -1, _("Do you want to reload modified files?")), 0, wxALL, 5);

	checklist = new wxCheckListBox(this, -1,wxDefaultPosition, wxDefaultSize, paths);
	for (unsigned int i = 0; i < checklist->GetCount(); ++i) {
		checklist->Check(i);
	}
	sizer->Add(checklist, 1, wxEXPAND|wxLEFT|wxRIGHT, 5);

	// Buttons
	sizer->Add(CreateButtonSizer(wxYES|wxNO), 0, wxEXPAND|wxALL, 5);
	SetAffirmativeId(wxID_YES);
	SetEscapeId(wxID_NO);

	SetSizerAndFit(sizer);

	Centre();
}

int ReloadDlg::GetCount() const {
	return checklist->GetCount();
}

bool ReloadDlg::IsChecked(int item) const {
	return checklist->IsChecked(item);
}

void ReloadDlg::OnYes(wxCommandEvent& WXUNUSED(event)) {
	EndModal(wxID_YES);
}

void ReloadDlg::OnNo(wxCommandEvent& WXUNUSED(event)) {
	EndModal(wxID_NO);
}

#endif // __RELOADDLG_H_
