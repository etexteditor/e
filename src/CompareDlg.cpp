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
#include <wx/filepicker.h>
#include "eSettings.h"

// Ctrl id's
enum {
	ID_BUTTON_BROWSELEFT,
	ID_BUTTON_BROWSERIGHT,
	ID_LEFTPATH,
	ID_RIGHTPATH
};

BEGIN_EVENT_TABLE(CompareDlg, wxDialog)
	EVT_BUTTON(wxID_OK, CompareDlg::OnButtonOk)
	EVT_BUTTON(ID_BUTTON_BROWSELEFT, CompareDlg::OnBrowseLeft)
	EVT_BUTTON(ID_BUTTON_BROWSERIGHT, CompareDlg::OnBrowseRight)
	EVT_TEXT(ID_LEFTPATH, CompareDlg::OnPathChanged)
	EVT_TEXT(ID_RIGHTPATH, CompareDlg::OnPathChanged)
END_EVENT_TABLE()


CompareDlg::CompareDlg(wxWindow *parent, eSettings& settings):
	wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
	m_settings(settings)
{
	SetTitle (_("Compare..."));

	// Create ctrls
	m_leftPathCtrl = new wxComboBox(this, ID_LEFTPATH);
	m_rightPathCtrl = new wxComboBox(this, ID_RIGHTPATH);
	wxButton* leftBrowse = new wxButton(this, ID_BUTTON_BROWSELEFT, _("Browse"));
	wxButton* rightBrowse = new wxButton(this, ID_BUTTON_BROWSERIGHT, _("Browse"));

	// Add recents
	wxArrayString recentPaths;
	m_settings.GetRecentDiffs(recentPaths, SP_LEFT);
	if (!recentPaths.empty()) {
		m_leftPathCtrl->Append(recentPaths);
		m_leftPathCtrl->SetSelection(0);
	}
	recentPaths.clear();
	m_settings.GetRecentDiffs(recentPaths, SP_RIGHT);
	if (!recentPaths.empty()) {
		m_rightPathCtrl->Append(recentPaths);
		m_rightPathCtrl->SetSelection(0);
	}

	// Create layout
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		wxFlexGridSizer* pathsizer = new wxFlexGridSizer(3, 2);
			pathsizer->AddGrowableCol(1, 1);
			pathsizer->Add(new wxStaticText(this, wxID_ANY, _("Left:")), 0, wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(m_leftPathCtrl, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(leftBrowse, 0, wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(new wxStaticText(this, wxID_ANY, _("Right:")), 0, wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(m_rightPathCtrl, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL);
			pathsizer->Add(rightBrowse, 0, wxALIGN_CENTER_VERTICAL);
			sizer->Add(pathsizer, 0, wxEXPAND|wxALL, 5);
		sizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	SetSizerAndFit(sizer);
	SetSize(400, -1);

	UpdateOkButton();
}

void CompareDlg::OnButtonOk(wxCommandEvent& evt) {
	const wxString leftPath = m_leftPathCtrl->GetValue();
	const wxString rightPath = m_rightPathCtrl->GetValue();

	m_settings.AddRecentDiff(leftPath, SP_LEFT);
	m_settings.AddRecentDiff(rightPath, SP_RIGHT);

	// If we get here the paths are valid
	evt.Skip(); 
}

void CompareDlg::OnBrowseLeft(wxCommandEvent& WXUNUSED(evt)) {
	wxFileDialog dlg(this);
	dlg.SetPath(m_leftPathCtrl->GetValue());

	if (dlg.ShowModal() != wxID_OK) return;

	m_leftPathCtrl->SetValue(dlg.GetPath());
}

void CompareDlg::OnBrowseRight(wxCommandEvent& WXUNUSED(evt)) {
	wxFileDialog dlg(this);
	dlg.SetPath(m_rightPathCtrl->GetValue());

	if (dlg.ShowModal() != wxID_OK) return;

	m_rightPathCtrl->SetValue(dlg.GetPath());
}

void CompareDlg::OnPathChanged(wxCommandEvent& WXUNUSED(evt)) {
	UpdateOkButton();
}

void CompareDlg::UpdateOkButton() {
	bool isValid = false;
	const wxString leftPath = m_leftPathCtrl->GetValue();
	const wxString rightPath = m_rightPathCtrl->GetValue();

	if (!leftPath.empty() && ! rightPath.empty()) {
		if (wxDirExists(leftPath) && wxDirExists(rightPath)) isValid = true;
		else if (wxFileExists(leftPath) && wxFileExists(rightPath)) isValid = true;
	}

	wxWindow* okButton = FindWindowById(wxID_OK);
	if (okButton) okButton->Enable(isValid);
}

wxString CompareDlg::GetLeftPath() const
{
	return m_leftPathCtrl->GetValue();
}

wxString CompareDlg::GetRightPath() const
{
	return m_rightPathCtrl->GetValue();
}

void CompareDlg::SetLeftPath( const wxString& path )
{
	m_leftPathCtrl->SetValue(path);
}

void CompareDlg::SetRightPath( const wxString& path )
{
	m_rightPathCtrl->SetValue(path);
}