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

#ifndef __REMOTELOGINDLG_H__
#define __REMOTELOGINDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class RemoteLoginDlg : public wxDialog {
public:
	RemoteLoginDlg(wxWindow *parent, const wxString& username, const wxString& site, bool askToSave);

	virtual wxString GetUsername() const;
	virtual wxString GetPassword() const;
	virtual bool GetSaveProfile() const;

private:
	wxTextCtrl* m_profileUsername;
	wxTextCtrl* m_profilePassword;
	wxCheckBox* m_saveProfile;
};

#endif // __REMOTELOGINDLG_H__
