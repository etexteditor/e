#include "TextTip.h"

BEGIN_EVENT_TABLE(TextTip, wxPopupTransientWindow)
	EVT_KEY_DOWN(TextTip::OnKeyDown)
	EVT_KEY_UP(TextTip::OnKeyDown)
	EVT_CHAR(TextTip::OnKeyDown)
	EVT_LEFT_DOWN(TextTip::OnMouseClick)
    EVT_RIGHT_DOWN(TextTip::OnMouseClick)
    EVT_MIDDLE_DOWN(TextTip::OnMouseClick)
	EVT_MOUSE_CAPTURE_LOST(TextTip::OnCaptureLost)
END_EVENT_TABLE()

TextTip::TextTip(wxWindow *parent, const wxString& text, const wxPoint& pos, const wxSize& size, TextTip** windowPtr) : m_windowPtr(windowPtr) {
	Create(parent, wxSIMPLE_BORDER);

	wxLogDebug(wxT("TextTip::TextTip %x"), this);

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

	wxStaticText* t = new wxStaticText(this, -1, text);
	t->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	t->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(t, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 3);

	SetSizerAndFit(sizer);
	Layout();
	Position(pos, size);
	Popup();
	SetFocus();
}

TextTip::~TextTip() {
	if ( m_windowPtr )
    {
        *m_windowPtr = NULL;
    }
}

void TextTip::OnKeyDown(wxKeyEvent& WXUNUSED(event)) {
	Close();
	//if ( !wxPendingDelete.Member(this) ) DismissAndNotify();

	// Pass event on to parent so we don't miss a key entry
	//GetParent()->ProcessEvent(event);
}

void TextTip::OnMouseClick(wxMouseEvent& WXUNUSED(event)) {
    Close();
}

void TextTip::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event)) {
	// WORKAROUND: This event is not caught in parent class
	wxLogDebug(wxT("OnCaptureLost"));
	//if ( !wxPendingDelete.Member(this) ) DismissAndNotify();
	Close();
}

void TextTip::OnDismiss() {
    Close();
	// We can't destroy the window immediately as we might
	// be in the middle of handling some other event.
    //if ( !wxPendingDelete.Member(this) )
		// delayed destruction: the window will be deleted
		// during the next idle loop iteration
        //wxPendingDelete.Append(this);
}

void TextTip::Close() {
	wxLogDebug(wxT("TextTip::Close() %x"), this);
	if (HasCapture()) ReleaseMouse();
	Show(false);
    Destroy();
}

bool TextTip::OnPreKeyDown(wxKeyEvent& WXUNUSED(event)) {
	Close();
	return true;
}
