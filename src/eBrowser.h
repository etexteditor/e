#ifndef __EBROWSER_H__
#define __EBROWSER_H__

#include <wx/platform.h>

/*
	Include the proper browser include for MSW or GTK.
	#define 'eBrowser' to the platform specific browser class.
	Provide a "NewBrowser" factory function for creating browser controls.
*/

#if defined (__WXMSW__)
#include "IEHtmlWin.h"

inline wxIEHtmlWin* NewBrowser(wxWindow * parent, wxWindowID id,
				const wxPoint& point = wxDefaultPosition, 
                const wxSize& size = wxDefaultSize) 
{
	return new wxIEHtmlWin(parent, id, point, size);
}
#elif defined (__WXGTK__)
#include "WebKitHtmlWnd.h"

inline wxBrowser* NewBrowser(wxWindow * parent, wxWindowID id,
				const wxPoint& point = wxDefaultPosition, 
                const wxSize& size = wxDefaultSize) 
{
	return new wxBrowser(parent, id, point, size);
}
#endif

#endif
