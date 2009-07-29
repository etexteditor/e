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

#ifndef __COMPAREDLG_H__
#define __COMPAREDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

class wxFilePickerCtrl;
class eSettings;

class CompareDlg : public wxDialog {
public:
	CompareDlg(wxWindow *parent, eSettings& settings);

	// Access to file paths
	wxString GetLeftPath() const;
	wxString GetRightPath() const;
	void SetLeftPath(const wxString& path);
	void SetRightPath(const wxString& path);

private:
	// Event handlers
	void OnButtonOk(wxCommandEvent& evt);
	void OnBrowseLeft(wxCommandEvent& evt);
	void OnBrowseRight(wxCommandEvent& evt);
	DECLARE_EVENT_TABLE();

	wxComboBox* m_leftPathCtrl;
	wxComboBox* m_rightPathCtrl;

	eSettings& m_settings;
};

#endif // __COMPAREDLG_H__
