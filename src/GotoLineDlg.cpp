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

#include "GotoLineDlg.h"
#include <wx/spinctrl.h>

enum {
	CTRL_LINESPIN=100
};


BEGIN_EVENT_TABLE(GotoLineDlg, wxDialog)
	EVT_TEXT_ENTER(CTRL_LINESPIN, GotoLineDlg::OnEnter)
END_EVENT_TABLE()

GotoLineDlg::GotoLineDlg(wxWindow *parent, unsigned int pos, unsigned int min, unsigned int max):
	wxDialog (parent, wxID_ANY, wxEmptyString, wxDefaultPosition)
{
	SetTitle(_("Go to Line"));

	// Create controls
	m_lineCtrl = new wxSpinCtrl(this, CTRL_LINESPIN);
	m_lineCtrl->SetRange(min, max);
	m_lineCtrl->SetValue(pos);

	// Create Layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_lineCtrl, 0, wxEXPAND|wxALL, 5);
		mainSizer->Add(CreateStdDialogButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

	m_lineCtrl->SetFocus();
	m_lineCtrl->SetSelection(-1, -1); // select all text

	SetSizerAndFit(mainSizer);
	Centre();
}

unsigned int GotoLineDlg::GetLine() const {
	return m_lineCtrl->GetValue();
}

void GotoLineDlg::OnEnter(wxCommandEvent& WXUNUSED(event)) {
	EndModal(wxID_OK);
}
