#ifndef __MOZILLA_HTML_WND_H__
#define __MOZILLA_HTML_WND_H__

#include "IHtmlWnd.h"

#ifdef FEAT_BROWSER
#include <wxmozilla/wxMozillaBrowser.h>
class wxMozillaBrowser;

class wxMozillaHtmlWin : public wxMozillaBrowser, public virtual IHtmlWnd {
public:
	wxMozillaHtmlWin(wxWindow *parent, wxWindowID id);
	virtual ~wxMozillaHtmlWin();
	virtual bool LoadString(wxString html);
	virtual void LoadUrl(const wxString &_url, const wxString &_frame = wxEmptyString, bool keepHistory=false);
	virtual bool Refresh(wxHtmlRefreshLevel level);
	virtual bool GoBack();
        virtual bool GoForward();
	virtual wxString GetRealLocation();
};
#endif //FEAT_BROWSER

#endif
