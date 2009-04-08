#ifndef __I_HTML_WND_H__
#define __I_HTML_WND_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

enum wxHtmlRefreshLevel
{
	wxHTML_REFRESH_NORMAL = 0,
	wxHTML_REFRESH_IFEXPIRED = 1,
	wxHTML_REFRESH_CONTINUE = 2,
	wxHTML_REFRESH_COMPLETELY = 3
};

/* pure virtual class to define HTML view window for E */
class IHtmlWnd {
public:
	virtual ~IHtmlWnd() {};
	virtual wxWindow* GetWindow() = 0;
	virtual bool LoadString(wxString html) = 0;
	virtual void LoadUrl(const wxString &_url, const wxString &_frame = wxEmptyString, bool keepHistory=false) = 0;
	virtual bool Refresh(wxHtmlRefreshLevel level) = 0;
	virtual bool GoBack() = 0;
        virtual bool GoForward() = 0;
	virtual wxString GetRealLocation() = 0;
};

#endif

