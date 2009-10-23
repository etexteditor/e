#ifndef __EBROWSER_DEF_H__
#define __EBROWSER_DEF_H__

#include <wx/platform.h>

#if defined (__WXMSW__)
//	#define eBrowser wxIEHtmlWin
	class wxIEHtmlWin;
	typedef wxIEHtmlWin eBrowser;
#elif defined (__WXGTK__)
//	#define eBrowser wxBrowser
	class wxBrowser;
	typedef wxBrowser eBrowser;
#endif

class IHtmlWnd;
class IHtmlWndBeforeLoadEvent;

#endif
