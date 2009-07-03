#include "HtmlOutputPane.h"
#include "IOpenTextmateURL.h"
#include "eDocumentPath.h"

#if defined (__WXMSW__)
    #include "IEHtmlWin.h"
#elif defined (__WXGTK__)
    #ifdef FEAT_BROWSER
        #include "WebKitHtmlWnd.h"
    #endif
#endif

enum { ID_THE_BROWSER };

BEGIN_EVENT_TABLE(HtmlOutputPane, wxPanel)
	EVT_HTMLWND_BEFORE_LOAD(ID_THE_BROWSER, HtmlOutputPane::OnBeforeLoad)
END_EVENT_TABLE()

HtmlOutputPane::HtmlOutputPane(wxWindow *parent, IOpenTextmateURL& opener):
	wxPanel(parent, wxID_ANY), 
	m_opener(opener) 
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

#ifdef FEAT_BROWSER

#if defined (__WXMSW__)
	// IE Control
	m_browser = new wxIEHtmlWin(this, ID_THE_BROWSER);
#elif defined (__WXGTK__)
	// WebKit control
	m_browser = new wxBrowser(this, ID_THE_BROWSER);
#endif

	mainSizer->Add(m_browser->GetWindow(), 1, wxEXPAND);
#endif // FEAT_BROWSER

	SetSizer(mainSizer);
}

void HtmlOutputPane::SetPage(const wxString& text) {
#ifdef FEAT_BROWSER

#ifdef __WXMSW__
	// Convert cygwin paths to windows
	wxString html = text;
	unsigned int pos = 0;
	while (pos < html.size()) {
		const size_t startpos = html.find(eDocumentPath::CygdrivePrefix(), pos);
		if (startpos == wxString::npos) break;
		++pos; // to advance if we continue the loop

		// Find the end of path
		unsigned int endpos = startpos+10;
		while (endpos < html.size()) {
			const wxChar c = html[endpos];
			if (c == wxT('"') || c == wxT('\'') || c == wxT('>') || c == wxT('<')) break;
			++endpos;
		}

		// Convert the path
		wxString path = html.substr(startpos, endpos - startpos);
		path = eDocumentPath::CygwinPathToWin(path);
		DecodePath(path); // Spaces transformed to %20 in paths confuses ie

		html.replace(startpos, endpos - startpos, path);

		pos = startpos + path.size();
	}

	m_browser->LoadString(html);
#else
	m_browser->LoadString(text);
#endif

#endif //FEAT_BROWSER
}

void HtmlOutputPane::AppendText(const wxString& html) {
#ifdef FEAT_BROWSER
#ifdef __WXMSW__
	m_browser->AppendString(html);
#endif //__WXMSW__
#endif //FEAT_BROWSER
}

void HtmlOutputPane::OnBeforeLoad(IHtmlWndBeforeLoadEvent& event) {
    const wxString url = event.GetURL();
	if (url == wxT("about:blank")) return;

	if (url.StartsWith(wxT("txmt://open"))) {
		m_opener.OpenTxmtUrl(url);

		// Don't try to open it in browser
		event.Cancel(true);
		return;
	}
	
	if (url.StartsWith(wxT("tm-file://"))) {
		wxString path = url.substr(10);

#ifdef __WXMSW__
		path = eDocumentPath::CygwinPathToWin(path); // path may be in unix format, so we have to convert it
#endif

		DecodePath(path); // Spaces transformed to %20 in paths confuses ie
		m_browser->LoadUrl(path);

		// Don't try to open it in browser
		event.Cancel(true);
		return;
	}
}

void HtmlOutputPane::DecodePath(wxString& path) { // static
	// Spaces transformed to %20 in paths confuses ie
	for (unsigned int i2 = 0; i2 < path.size(); ++i2) {
		if (path[i2] == wxT('%') && path.size() > i2+3 && path[i2+1] == wxT('2') && path[i2+2] == wxT('0')) {
			path.replace(i2, 3, wxT(" "));
		}
	}
}
