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

#include "RemoteLoginDlg.h"

RemoteLoginDlg::RemoteLoginDlg(wxWindow *parent, const wxString& username, const wxString& site, bool askToSave):
	wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	SetTitle (_("Login failed"));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	{
		wxStaticText* labelInfo = new wxStaticText(this, wxID_ANY, _("Please enter new login for\n") + site);
		mainSizer->Add(labelInfo, 0, wxALL, 5);

		wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2,2, 0, 0);
		{
			gridSizer->AddGrowableCol(1); // col 2 is sizable

			wxStaticText* labelUsername = new wxStaticText(this, wxID_ANY, _("Username:"));
			gridSizer->Add(labelUsername, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			m_profileUsername = new wxTextCtrl(this, wxID_ANY);
			m_profileUsername->SetToolTip(_("Username for login"));
			gridSizer->Add(m_profileUsername, 1, wxEXPAND|wxALL, 5);

			wxStaticText* labelPassword = new wxStaticText(this, wxID_ANY, _("Password:"));
			gridSizer->Add(labelPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			m_profilePassword = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
			m_profilePassword->SetToolTip(_("Password for login"));
			gridSizer->Add(m_profilePassword, 1, wxEXPAND|wxALL, 5);

			mainSizer->Add(gridSizer, 1, wxEXPAND|wxALL, 5);
		}

		m_saveProfile = new wxCheckBox(this, wxID_ANY, _("Save as profile"));
		mainSizer->Add(m_saveProfile, 0, wxALL, 5);

		// Buttons
		mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);
	}

	if (!askToSave)
		mainSizer->Hide(m_saveProfile);

	if (username.empty()) m_profileUsername->SetFocus();
	else {
		m_profileUsername->SetValue(username);
		m_profilePassword->SetFocus();
	}

	SetSizerAndFit(mainSizer);
	Centre();
}

wxString RemoteLoginDlg::GetUsername() const { return m_profileUsername->GetValue(); }
wxString RemoteLoginDlg::GetPassword() const { return m_profilePassword->GetValue(); }
bool RemoteLoginDlg::GetSaveProfile() const { return m_saveProfile->IsChecked(); }
