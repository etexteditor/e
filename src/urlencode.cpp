#include "urlencode.h"

wxString UrlEncode::Encode(const wxString& url) {
	const wxChar* uri = url.c_str();
	wxString newUrl;

	while(*uri) {
		if(IsUnreserved(*uri) || *uri == wxT('/') || *uri == wxT(':'))
			newUrl += *uri++;
		else Escape(newUrl, *uri++);
	}

	return newUrl;
}

wxString UrlEncode::EncodeFilename(const wxString& filename) {
	const wxChar* uri = filename.c_str();
	wxString newUrl;

	while(*uri) {
		if(*uri == wxT('"') || *uri == wxT('/') || *uri == wxT('\\')
			|| *uri == wxT('*') || *uri == wxT('?') || *uri == wxT('<')
			|| *uri == wxT('>') || *uri == wxT('|') || *uri == wxT(':'))
			Escape(newUrl, *uri++);
		else newUrl += *uri++;
	}

	return newUrl;
}

wxString UrlEncode::EscapeUrl(const wxString& url) {
	// The each part of the url has to be escaped separately
	// so that the separators are preserved.
	wxString escUrl;

	// Protocol
	int pos = url.Find(wxT("://"));
	if (pos == wxNOT_FOUND) return url;
	pos += 3;
	escUrl += url.substr(0, pos);

	wxString address = url.substr(pos);

	escUrl += Encode(address);

	return escUrl;
}
