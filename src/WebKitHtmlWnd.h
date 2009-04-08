/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __WEBKIT_HTML_WND_H__
#define __WEBKIT_HTML_WND_H__

#include "IHtmlWnd.h"

#ifdef FEAT_BROWSER
#include <wx/WebView.h>

class wxBrowser : public wxWebView, public virtual IHtmlWnd {
public:
	wxBrowser(wxWindow *parent, wxWindowID id);
	virtual ~wxBrowser();
	virtual wxWindow* GetWindow();
	virtual bool LoadString(wxString html);
	virtual void LoadUrl(const wxString &_url, const wxString &_frame = wxEmptyString, bool keepHistory=false);
	virtual bool Refresh(wxHtmlRefreshLevel level);
	virtual bool GoBack();
        virtual bool GoForward();
	virtual wxString GetRealLocation();
protected:
	wxString m_realLocation;

};
#endif //FEAT_BROWSER

#endif
