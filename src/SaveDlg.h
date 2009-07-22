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

#ifndef __SAVEDLG_H__
#define __SAVEDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

class SaveDlg : public wxDialog {
public:
	SaveDlg(wxWindow *parent, const wxArrayString& paths);

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

#endif // __SAVEDLG_H_
