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

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif
#include <wx/filepicker.h>

class CompareDlg : public wxDialog {
public:
	CompareDlg(wxWindow *parent);

	// Access to file paths
	wxString GetLeftPath() const {return m_leftPathCtrl->GetPath();};
	wxString GetRightPath() const {return m_rightPathCtrl->GetPath();};
	void SetLeftPath(const wxString& path) {m_leftPathCtrl->SetPath(path);};
	void SetRightPath(const wxString& path) {m_rightPathCtrl->SetPath(path);};

private:
	// Event handlers
	void OnButtonOk(wxCommandEvent& evt);
	DECLARE_EVENT_TABLE();

	wxFilePickerCtrl* m_leftPathCtrl;
	wxFilePickerCtrl* m_rightPathCtrl;
};

#endif // __COMPAREDLG_H__
