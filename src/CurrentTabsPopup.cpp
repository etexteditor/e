#include "CurrentTabsPopup.h"

CurrentTabsPopup::CurrentTabsPopup(wxWindow* parent, const wxPoint& pos):
	wxDialog(parent, wxID_ANY, wxEmptyString, pos, wxDefaultSize, wxNO_BORDER)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizerAndFit(mainSizer);

	Show();
}
