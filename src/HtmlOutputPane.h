#ifndef __HTMLOUTPUTPANE_H__
#define __HTMLOUTPUTPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class IHtmlWnd;
class IHtmlWndBeforeLoadEvent;

class IOpenTextmateURL;

class HtmlOutputPane : public wxPanel {
public:
	HtmlOutputPane(wxWindow *parent, IOpenTextmateURL& opener);
	void SetPage(const wxString& html);
	//void AppendText(const wxString& html);

private:
	static void DecodePath(wxString& path);

	void OnBeforeLoad(IHtmlWndBeforeLoadEvent& event);
	DECLARE_EVENT_TABLE()

	IOpenTextmateURL& m_opener;
	IHtmlWnd* m_browser;
};

#endif
