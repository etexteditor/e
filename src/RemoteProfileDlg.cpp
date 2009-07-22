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

#include "RemoteProfileDlg.h"
#include <wx/notebook.h>
#include "RemoteThread.h"
#include "eSettings.h"

// Ctrl id's
enum {
	CTRL_PROFILELIST = 100,
	CTRL_BUTTON_NEW,
	CTRL_BUTTON_DELETE,
	CTRL_BUTTON_OPEN,
	CTRL_TEXT_NAME
};

BEGIN_EVENT_TABLE(RemoteProfileDlg, wxDialog)
	EVT_BUTTON(CTRL_BUTTON_NEW, RemoteProfileDlg::OnButtonNew)
	EVT_BUTTON(CTRL_BUTTON_DELETE, RemoteProfileDlg::OnButtonDelete)
	EVT_BUTTON(CTRL_BUTTON_OPEN, RemoteProfileDlg::OnButtonOpen)
	EVT_BUTTON(wxID_CANCEL, RemoteProfileDlg::OnButtonCancel)
	EVT_TEXT(CTRL_TEXT_NAME, RemoteProfileDlg::OnTextName)
	EVT_LISTBOX(CTRL_PROFILELIST, RemoteProfileDlg::OnProfileList)
	EVT_CLOSE(RemoteProfileDlg::OnClose)
END_EVENT_TABLE()

RemoteProfileDlg::RemoteProfileDlg(wxWindow *parent, eSettings& settings):
	wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
	m_settings(settings), m_currentProfile(-1) 
{
	SetTitle (_("Remote Profiles"));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* profileSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			wxBoxSizer* listSizer = new wxBoxSizer(wxVERTICAL);
			{
				m_profileList = new wxListBox(this, CTRL_PROFILELIST);
				listSizer->Add(m_profileList, 1, wxEXPAND|wxALL, 5);

				wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
				{
					wxButton* newButton = new wxButton(this, CTRL_BUTTON_NEW, _("New Profile"));
					newButton->SetToolTip(_("Create new profile for remote project"));
					m_deleteButton = new wxButton(this, CTRL_BUTTON_DELETE, _("Delete"));
					m_deleteButton->SetToolTip(_("Delete profile"));
					buttonSizer->Add(newButton, 0, wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5);
					buttonSizer->Add(m_deleteButton, 0, wxALIGN_RIGHT|wxRIGHT, 5);

					m_deleteButton->Enable(false);

					listSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
				}

				profileSizer->Add(listSizer, 1, wxEXPAND|wxRIGHT, 5);
			}

			// Create the notebook
			wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
			{
				// Create the general settings page
				wxPanel* settingsPage = new wxPanel(notebook, wxID_ANY);
				{
					wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2,2, 0, 0);
					{
						gridSizer->AddGrowableCol(1); // col 2 is sizable

						wxStaticText* labelName = new wxStaticText(settingsPage, wxID_ANY, _("Name:"));
						gridSizer->Add(labelName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
						m_profileName = new wxTextCtrl(settingsPage, CTRL_TEXT_NAME);
						m_profileName->SetToolTip(_("Profile name"));
						gridSizer->Add(m_profileName, 1, wxEXPAND|wxALL, 5);

						wxStaticText* labelAddress = new wxStaticText(settingsPage, wxID_ANY, _("Address:"));
						gridSizer->Add(labelAddress, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
						m_profileAddress = new wxTextCtrl(settingsPage, wxID_ANY);
						m_profileAddress->SetToolTip(_("Remote address. (eg. ftp.mydomain.com)"));
						gridSizer->Add(m_profileAddress, 1, wxEXPAND|wxALL, 5);

						wxStaticText* labelDir = new wxStaticText(settingsPage, wxID_ANY, _("Directory:"));
						gridSizer->Add(labelDir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
						m_profileDir = new wxTextCtrl(settingsPage, wxID_ANY);
						m_profileDir->SetToolTip(_("Base dir for project. (eg. /user/www/)"));
						gridSizer->Add(m_profileDir, 1, wxEXPAND|wxALL, 5);

						wxStaticText* labelUsername = new wxStaticText(settingsPage, wxID_ANY, _("Username:"));
						gridSizer->Add(labelUsername, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
						m_profileUsername = new wxTextCtrl(settingsPage, wxID_ANY);
						m_profileUsername->SetToolTip(_("Username for login (leave empty for anonymous)."));
						gridSizer->Add(m_profileUsername, 1, wxEXPAND|wxALL, 5);

						wxStaticText* labelPassword = new wxStaticText(settingsPage, wxID_ANY, _("Password:"));
						gridSizer->Add(labelPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
						m_profilePassword = new wxTextCtrl(settingsPage, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
						m_profilePassword->SetToolTip(_("Password for login (leave empty for anonymous)."));
						gridSizer->Add(m_profilePassword, 1, wxEXPAND|wxALL, 5);

						settingsPage->SetSizerAndFit(gridSizer);
					}

					notebook->AddPage(settingsPage, _("General"), true);
				}

				profileSizer->Add(notebook, 2, wxEXPAND);
			}

			mainSizer->Add(profileSizer, 1, wxEXPAND|wxALL, 5);
		}

		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			m_openButton = new wxButton(this, CTRL_BUTTON_OPEN, _("Open"));
			wxButton* cancelButton = new wxButton(this, wxID_CANCEL, _("Cancel"));
			buttonSizer->Add(m_openButton, 0, wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5);
			buttonSizer->Add(cancelButton, 0, wxALIGN_RIGHT|wxRIGHT, 5);

			mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT|wxBOTTOM, 5);
		}
	}

	SetDefaultItem(m_openButton);
	Init();

	SetSizerAndFit(mainSizer);
	Centre();
}

void RemoteProfileDlg::EnableSettings(bool enable) {
	if (!enable) {
		m_currentProfile = -1;

		m_profileName->Clear();
		m_profileAddress->Clear();
		m_profileDir->Clear();
		m_profileUsername->Clear();
		m_profilePassword->Clear();
	}

	m_profileName->Enable(enable);
	m_profileAddress->Enable(enable);
	m_profileDir->Enable(enable);
	m_profileUsername->Enable(enable);
	m_profilePassword->Enable(enable);

	m_deleteButton->Enable(enable);
	m_openButton->Enable(enable);
}

void RemoteProfileDlg::Init() {
	const unsigned int profile_count = m_settings.GetRemoteProfileCount();

	for (unsigned int i = 0; i < profile_count; ++i) {
		const wxString profileName = m_settings.GetRemoteProfileName(i);
		m_profileList->Append(profileName, (void*)i);
	}

	// Initially no profile is selected
	EnableSettings(false);
}

void RemoteProfileDlg::SetProfile(unsigned int profile_id) {
	wxASSERT((int)profile_id != -1);

	if (m_currentProfile == (intptr_t)profile_id) return;
	else if (m_currentProfile != -1) SaveProfile();

	// Get the profile from db
	const RemoteProfile* rp = m_settings.GetRemoteProfile(profile_id);

	m_currentProfile = (intptr_t)profile_id;
	m_profileName->SetValue(rp->m_name);
	m_profileAddress->SetValue(rp->m_address);
	m_profileDir->SetValue(rp->m_dir);
	m_profileUsername->SetValue(rp->m_username);
	m_profilePassword->SetValue(rp->m_pwd);

	EnableSettings(true);
}

void RemoteProfileDlg::SaveProfile() {
	if (m_currentProfile == -1) return;

	// Only save if setting have been modified
	bool modified = false;
	if (m_profileName->IsModified()) modified = true;
	if (m_profileAddress->IsModified()) modified = true;
	if (m_profileDir->IsModified()) modified = true;
	if (m_profileUsername->IsModified()) modified = true;
	if (m_profilePassword->IsModified()) modified = true;
	if (!modified) return;

	// Create profile
	RemoteProfile rp;
	rp.m_protocol = wxT("ftp");
	rp.m_name = m_profileName->GetValue();
	rp.m_address = m_profileAddress->GetValue();
	rp.m_dir = m_profileDir->GetValue();
	rp.m_username = m_profileUsername->GetValue();
	rp.m_pwd = m_profilePassword->GetValue();

	// Save to db
	m_settings.SetRemoteProfile(m_currentProfile, rp);
}

void RemoteProfileDlg::OnButtonNew(wxCommandEvent& WXUNUSED(event)) {
	// Create the new profile
	RemoteProfile rp;
	rp.m_name = wxT("New Profile");

	// Add to db
	const unsigned int profile_id = m_settings.AddRemoteProfile(rp);

	// Add to list
	const unsigned int item_id = m_profileList->Append(rp.m_name, (void*)profile_id);
	m_profileList->SetSelection(item_id);

	SetProfile(profile_id);
	m_profileName->SetSelection(-1,-1); // select all
	m_profileName->SetFocus();
}

void RemoteProfileDlg::OnButtonDelete(wxCommandEvent& WXUNUSED(event)) {
	// Get current selection
	const int item = m_profileList->GetSelection();
	if (item == wxNOT_FOUND) return;

	EnableSettings(false);

	// Delete profile
	const uintptr_t profile_id = (uintptr_t)m_profileList->GetClientData(item);
	m_settings.DeleteRemoteProfile(profile_id);
	m_profileList->Delete(item);

	// Adjust subsequent profile id's
	for (unsigned int i = 0; i < m_profileList->GetCount(); ++i) {
		const uintptr_t id = (uintptr_t)m_profileList->GetClientData(i);
		if (id > profile_id) {
			m_profileList->SetClientData(i, (void*)(id-1));
		}
	}
}

void RemoteProfileDlg::OnButtonOpen(wxCommandEvent& WXUNUSED(event)) {
	SaveProfile();
	EndModal(wxID_OPEN);
}

void RemoteProfileDlg::OnButtonCancel(wxCommandEvent& WXUNUSED(event)) {
	SaveProfile();
	EndModal(wxID_CANCEL);
}

void RemoteProfileDlg::OnTextName(wxCommandEvent& event) {
	if (m_currentProfile == -1) return;

	const wxString newName = event.GetString();

	// Find the corresponding list item
	for (unsigned int i = 0; i < m_profileList->GetCount(); ++i) {
		if (m_currentProfile == (intptr_t)m_profileList->GetClientData(i)) {
			m_profileList->SetString(i, newName);
			return;
		}
	}
}

void RemoteProfileDlg::OnProfileList(wxCommandEvent& event) {
	const uintptr_t profile_id = (uintptr_t)m_profileList->GetClientData(event.GetInt());
	SetProfile(profile_id);
}

void RemoteProfileDlg::OnClose(wxCloseEvent& WXUNUSED(event)) {
	// Make sure any last changes to the profile is saved
	SaveProfile();
	EndModal(wxID_CANCEL);
}
