#ifndef __GOTOLINEDLG_H__
#define __GOTOLINEDLG_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/spinctrl.h>

enum {
	CTRL_LINESPIN=100
};

class GotoLineDlg : public wxDialog {
public:
	GotoLineDlg(wxWindow *parent, unsigned int pos, unsigned int min, unsigned int max);
	unsigned int GetLine() const;

private:
	// Event handlers
	void OnEnter(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	wxSpinCtrl* m_lineCtrl;
};

BEGIN_EVENT_TABLE(GotoLineDlg, wxDialog)
	EVT_TEXT_ENTER(CTRL_LINESPIN, GotoLineDlg::OnEnter)
END_EVENT_TABLE()

GotoLineDlg::GotoLineDlg(wxWindow *parent, unsigned int pos, unsigned int min, unsigned int max)
: wxDialog (parent, wxID_ANY, wxEmptyString, wxDefaultPosition) {
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

#endif // __GOTOLINEDLG_H__
