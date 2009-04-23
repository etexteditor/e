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

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include <wx/spinctrl.h>

class SettingsDlg : public wxDialog {
public:
	SettingsDlg(wxWindow *parent, CatalystWrapper cw);

private:
	void UpdateEncoding();

	// Event handlers
	void OnButtonOk(wxCommandEvent& event);
	void OnButtonLoadPic(wxCommandEvent& event);
	void OnCheckAutoPair(wxCommandEvent& event);
	void OnCheckAutoWrap(wxCommandEvent& event);
	void OnCheckKeepState(wxCommandEvent& event);
	void OnCheckCheckChange(wxCommandEvent& event);
	void OnCheckShowMargin(wxCommandEvent& event);
	void OnCheckWrapMargin(wxCommandEvent& event);
	void OnMarginSpin(wxSpinEvent& event);
	void OnComboEol(wxCommandEvent& event);
	void OnComboEncoding(wxCommandEvent& event);
	void OnCheckBom(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
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

#ifdef __WXMSW__
	// -- UNIX-on-Windws page
	wxTextCtrl* m_ctrlCygdrivePrefix;
#endif
};

#endif // __SETTINGSDLG_H__
