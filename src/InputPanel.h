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

#ifndef __INPUTPANEL_H__
#define __INPUTPANEL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/panel.h>
#include <wx/string.h>
#endif

// pre-definitions
class EditorFrame;

class InputPanel : public wxPanel {
public:
	InputPanel(EditorFrame& parentFrame, wxWindow* parent);
	~InputPanel() {};

	void Set(unsigned int notifier_id, const wxString& cmd);
	unsigned int GetNotifierId() const {return m_notifier_id;};

private:
	// Event handlers
	void OnCloseButton(wxCommandEvent& evt);
	void OnInputChanged(wxCommandEvent& evt);
	void OnInputEnter(wxCommandEvent& evt);
	DECLARE_EVENT_TABLE();

	// Member Ctrls
	wxTextCtrl* m_inputbox;
	wxStaticText* m_caption;

	// Member variables
	EditorFrame& m_parentFrame;
	unsigned int m_notifier_id;
};

#endif //__INPUTPANEL_H