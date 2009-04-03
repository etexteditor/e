
#include "MozillaHtmlWnd.h"

wxMozillaHtmlWin::wxMozillaHtmlWin(wxWindow *parent, wxWindowID id) : wxMozillaBrowser(parent, id) {
}

bool wxMozillaHtmlWin::LoadString(wxString html) {
	wxMozillaBrowser::InsertHTML(html);
	return true;
}

void wxMozillaHtmlWin::LoadUrl(const wxString &_url,
	const wxString &_frame /*= wxEmptyString*/,
	bool keepHistory /*=false*/) {
	wxMozillaBrowser::LoadURL(_url);
}

bool wxMozillaHtmlWin::Refresh(wxHtmlRefreshLevel level) {
	return wxMozillaBrowser::Reload();
}

bool wxMozillaHtmlWin::GoBack() {
	return wxMozillaBrowser::GoBack();
}

bool wxMozillaHtmlWin::GoForward() {
	return wxMozillaBrowser::GoForward();
}

wxString wxMozillaHtmlWin::GetRealLocation() {
	return wxMozillaBrowser::GetURL();
}

wxMozillaHtmlWin::~wxMozillaHtmlWin() {
}
