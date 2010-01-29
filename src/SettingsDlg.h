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

#ifndef __SETTINGSDLG_H__
#define __SETTINGSDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "Catalyst.h"

// Pre-declarations
class eSettings;
class wxSpinCtrl;
class wxSpinEvent;
class EnvVarsPanel;

class SettingsDlg : public wxDialog {
public:
	SettingsDlg(wxWindow *parent, CatalystWrapper cw, eSettings& settings);

private:
	wxPanel* CreateSettingsPage(wxWindow* parent);
	wxPanel* CreateEncodingPage(wxWindow* parent);
	wxPanel* CreateProfilePage(wxWindow* parent);
	wxPanel* CreateUpdatePage(wxWindow* parent);

	void UpdateEncoding();

#ifdef __WXMSW__
	wxPanel* CreateUnixPage(wxWindow* parent);
	void UpdateUnixPage();
#endif

	// Event handlers
	void OnButtonOk(wxCommandEvent& event);
	void OnButtonLoadPic(wxCommandEvent& event);
	void OnButtonCygwinAction(wxCommandEvent& event);
	void OnButtonCheckForUpdates(wxCommandEvent& event);
	void OnCheckAutoPair(wxCommandEvent& event);
	void OnCheckAutoWrap(wxCommandEvent& event);
	void OnCheckKeepState(wxCommandEvent& event);
	void OnCheckCheckChange(wxCommandEvent& event);
	void OnCheckShowMargin(wxCommandEvent& event);
	void OnCheckWrapMargin(wxCommandEvent& event);
	void OnCheckCheckForUpdates(wxCommandEvent& event);
	void OnCheckAtomicSave(wxCommandEvent& event);
	void OnCheckLastTab(wxCommandEvent& event);
	void OnCheckHighlightVariables(wxCommandEvent& event);
	void OnCheckHighlightHtml(wxCommandEvent& event);
	void OnMarginSpin(wxSpinEvent& event);
	void OnComboEol(wxCommandEvent& event);
	void OnComboEncoding(wxCommandEvent& event);
	void OnCheckBom(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	eSettings& m_settings;
	CatalystWrapper m_catalyst;
	wxImage m_userImage;

	// Change trackers
	bool m_ctUserPic;

	// Controls
	// -- Profile page
	wxTextCtrl* m_ctrlUserName;
	wxStaticBitmap* m_ctrlUserPic;

	// -- Encoding page
	wxComboBox* m_defLine;
	wxComboBox* m_defEnc;
	wxCheckBox* m_defBom;

	// -- Settings page
	wxSpinCtrl* m_marginSpin;
	wxCheckBox* m_wrapMargin;

	// --Environmental Variables page
	EnvVarsPanel *m_envPage;

#ifdef __WXMSW__
	// -- UNIX-on-Windws page
	wxPanel* m_unixPage;

	wxStaticText* m_labelCygInitValue;
	wxStaticText* m_labelBashPathValue;
	wxStaticText* m_labelCygdriveValue;
	wxButton* m_cygwinButton;
#endif

	// Updates page
	wxCheckBox* m_checkForUpdatesAtStartup;
	wxButton* m_checkForUpdatesButton;
};

#endif // __SETTINGSDLG_H__
