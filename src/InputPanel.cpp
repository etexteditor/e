#include "InputPanel.h"
#include "CloseButton.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"

// Ctrl id's
enum {
    INPUTP_CLOSE = 100,
	INPUTP_BOX,
};

BEGIN_EVENT_TABLE(InputPanel, wxPanel)
	EVT_BUTTON(INPUTP_CLOSE, InputPanel::OnCloseButton)
END_EVENT_TABLE()

InputPanel::InputPanel(EditorFrame& parentFrame, wxWindow* parent)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE),
  m_parentFrame(parentFrame) {
	// Create the controls
	CloseButton* closeButton = new CloseButton(this, INPUTP_CLOSE);
	m_caption  = new wxStaticText(this, wxID_ANY, _("Input: "));
	m_inputbox = new wxTextCtrl(this, INPUTP_BOX, wxEmptyString, wxDefaultPosition, wxDefaultSize);

	m_inputbox->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(InputPanel::OnInputChanged), NULL, this);
	m_inputbox->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(InputPanel::OnInputEnter), NULL, this);

	// Create the sizer layout
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->AddSpacer(5);
		sizer->Add(closeButton, 0, wxALIGN_CENTER|wxRIGHT, 2);
		sizer->Add(m_caption, 0, wxALIGN_CENTER);
		sizer->Add(m_inputbox, 3, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
		sizer->AddSpacer(5);

	SetSizer(sizer);

	// Set the correct size
	const wxSize minsize = sizer->GetMinSize();
	SetSize(minsize);

	// Make sure sizes get the right min/max sizes
	SetSizeHints(minsize.x, minsize.y, -1, minsize.y);
}

void InputPanel::Set(unsigned int notifier_id, const wxString& cmd) {
	m_notifier_id = notifier_id;
	m_caption->SetLabel(cmd);
	m_inputbox->Clear();
}

void InputPanel::OnInputChanged(wxCommandEvent& evt) {
	m_parentFrame.OnInputPanelChanged(m_notifier_id, evt.GetString());
	evt.Skip();
}

void InputPanel::OnInputEnter(wxCommandEvent& WXUNUSED(evt)) {
	m_parentFrame.HideInputPanel();
}

void InputPanel::OnCloseButton(wxCommandEvent& WXUNUSED(evt)) {
	m_parentFrame.HideInputPanel();
}
