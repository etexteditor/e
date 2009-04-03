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

#ifndef __REMOTEPROFILEDLG_H__
#define __REMOTEPROFILEDLG_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"

// pre-definitions
class CatalystWrapper;

class RemoteProfileDlg : public wxDialog {
public:
	RemoteProfileDlg(wxWindow *parent, CatalystWrapper cw);

	intptr_t GetCurrentProfile() const {return m_currentProfile;};

private:
	void Init();
	void EnableSettings(bool enable);
	void SetProfile(unsigned int profile_id);
	void SaveProfile();

	// Event handlers
	void OnButtonNew(wxCommandEvent& event);
	void OnButtonDelete(wxCommandEvent& event);
	void OnButtonOpen(wxCommandEvent& event);
	void OnButtonCancel(wxCommandEvent& event);
	void OnTextName(wxCommandEvent& event);
	void OnProfileList(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	DECLARE_EVENT_TABLE();

	// Ctrls
	wxListBox* m_profileList;
	wxTextCtrl* m_profileName;
	wxTextCtrl* m_profileAddress;
	wxTextCtrl* m_profileDir;
	wxTextCtrl* m_profileUsername;
	wxTextCtrl* m_profilePassword;
	wxButton* m_deleteButton;
	wxButton* m_openButton;

	// Member variables
	CatalystWrapper m_catalyst;
	intptr_t m_currentProfile;
};


#endif // __REMOTEPROFILEDLG_H__
