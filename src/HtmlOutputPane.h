#ifndef __HTMLOUTPUTPANE_H__
#define __HTMLOUTPUTPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "IHtmlWnd.h"

#ifdef __WXMSW__
class wxIEHtmlWin;
#endif

class IOpenTextmateURL;

class HtmlOutputPane : public wxPanel {
public:
	HtmlOutputPane(wxWindow *parent, IOpenTextmateURL& opener);
	void SetPage(const wxString& html);
	void AppendText(const wxString& html);

private:
	static void DecodePath(wxString& path);

	// Event handlers
	void OnBeforeLoad(IHtmlWndBeforeLoadEvent& event);

	IOpenTextmateURL& m_opener;

#ifdef __WXMSW__
	wxIEHtmlWin* m_browser;
#else
	IHtmlWnd* m_browser;
#endif

	DECLARE_EVENT_TABLE()
};

#endif
