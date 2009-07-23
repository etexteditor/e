#include "SeparatorLine.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/dcclient.h>
    #include <wx/settings.h>
#endif

BEGIN_EVENT_TABLE(SeperatorLine, wxControl)
	EVT_PAINT(SeperatorLine::OnPaint)
	EVT_ERASE_BACKGROUND(SeperatorLine::OnEraseBackground)
END_EVENT_TABLE()

bool SeperatorLine::AcceptsFocus() const { return FALSE; }

SeperatorLine::SeperatorLine(wxWindow* parent, wxWindowID id, const wxPoint& pos):
	wxControl(parent, id, pos, wxSize(10, 2)) 
{
	// Make sure sizers min/max height are fixed to 2 pixels
	SetSizeHints(-1, 2, -1, 2);
}

void SeperatorLine::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
	wxPaintDC dc(this);
	// Get view dimensions
	wxSize size = GetClientSize();
	wxPen darkPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID);
    wxPen lightPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT), 1, wxSOLID);

	dc.SetPen(darkPen);
	dc.DrawLine(0, 0, size.x, 0);

	dc.SetPen(lightPen);
	dc.DrawLine(0, 1, size.x, 1);
}

// no evt.Skip() as we don't want the background to be erased
void SeperatorLine::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}
