#include "CommandPanel.h"
#include "CloseButton.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"

// Ctrl id's
enum {
    COMMANDP_CLOSE = 100,
	COMMANDP_BOX,
};

BEGIN_EVENT_TABLE(CommandPanel, wxPanel)
	EVT_BUTTON(COMMANDP_CLOSE, CommandPanel::OnCloseButton)
	EVT_IDLE(CommandPanel::OnIdle)
END_EVENT_TABLE()

CommandPanel::CommandPanel(EditorFrame& parentFrame, wxWindow* parent)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE),
  m_parentFrame(parentFrame) {
	// Create the controls
	CloseButton* closeButton = new CloseButton(this, COMMANDP_CLOSE);
	wxStaticText* commandlabel = new wxStaticText(this, wxID_ANY, _("Command: "));
	m_commandbox = new wxTextCtrl(this, COMMANDP_BOX, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_selStatic = new wxStaticText(this, wxID_ANY, wxEmptyString);

	// Create the sizer layout
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->AddSpacer(5);
		sizer->Add(closeButton, 0, wxALIGN_CENTER|wxRIGHT, 2);
		sizer->Add(commandlabel, 0, wxALIGN_CENTER);
		sizer->Add(m_commandbox, 3, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
		sizer->Add(m_selStatic, 0, wxALIGN_CENTER);
		sizer->AddSpacer(5);

	SetSizer(sizer);

	// Set the correct size
	const wxSize minsize = sizer->GetMinSize();
	SetSize(minsize);

	// Make sure sizes get the right min/max sizes
	SetSizeHints(minsize.x, minsize.y, -1, minsize.y);
}

void CommandPanel::ShowCommand(const wxString& cmd) {
	m_commandbox->SetValue(cmd);
}

void CommandPanel::OnCloseButton(wxCommandEvent& WXUNUSED(evt)) {
	m_parentFrame.ShowCommandMode(false);
}

void CommandPanel::OnIdle(wxIdleEvent& WXUNUSED(evt)) {
	const EditorCtrl* editor = m_parentFrame.GetEditorCtrl();
	if (!editor) return;

	const size_t selectionCount = editor->GetSelections().size();
	const size_t rangeCount = editor->GetSearchRange().size();
	if (selectionCount == m_selectionCount && rangeCount == m_rangeCount) return;

	wxString seltext;
	if (rangeCount == 1) seltext += wxT("1 range");
	else if (rangeCount) seltext += wxString::Format(wxT("%d ranges"), rangeCount);
	
	if (selectionCount) {
		if (!seltext.empty()) seltext += wxT(", ");
		if (selectionCount == 1) seltext += wxT("1 selection");
		else seltext += wxString::Format(wxT("%d selections"), selectionCount);
	}
	if (!seltext.empty()) seltext.insert(0, wxT(" ")); // spacer

	m_selStatic->SetLabel(seltext);
	GetSizer()->Layout();

	m_selectionCount = selectionCount;
	m_rangeCount = rangeCount;
}