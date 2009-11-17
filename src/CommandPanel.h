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

#ifndef __COMMANDPANEL_H__
#define __COMMANDPANEL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/panel.h>
#include <wx/string.h>
#endif

// pre-definitions
class EditorFrame;

class CommandPanel : public wxPanel {
public:
	CommandPanel(EditorFrame& parentFrame, wxWindow* parent);
	~CommandPanel() {};

	void ShowCommand(const wxString& cmd);

private:
	// Event handlers
	void OnCommandChar(wxKeyEvent& evt);
	void OnCloseButton(wxCommandEvent& evt);
	void OnIdle(wxIdleEvent& evt);
	DECLARE_EVENT_TABLE();

	// Member Ctrls
	wxTextCtrl* m_commandbox;
	wxStaticText* m_selStatic;

	// Member variables
	EditorFrame& m_parentFrame;
	size_t m_rangeCount;
	size_t m_selectionCount;
};

#endif //__COMMANDPANEL_H__