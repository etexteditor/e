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

#include "WebKitHtmlWnd.h"

wxBrowser::wxBrowser(wxWindow *parent, wxWindowID id) : wxWebView(parent, id), m_realLocation(wxT("")) {
}

wxWindow* wxBrowser::GetWindow() {
    return static_cast<wxWindow*>(this);
}

bool wxBrowser::LoadString(wxString html) {
	wxWebView::SetPageSource(html);
	m_realLocation = wxT("file://");
	return true;
}

void wxBrowser::LoadUrl(const wxString &_url,
	const wxString &_frame /*= wxEmptyString*/,
	bool keepHistory /*=false*/) {
	wxWebView::LoadURL(_url);
	m_realLocation = _url;
}

bool wxBrowser::Refresh(wxHtmlRefreshLevel level) {
	wxWebView::Reload();
	return true;
}

bool wxBrowser::GoBack() {
	return wxWebView::GoBack();
}

bool wxBrowser::GoForward() {
	return wxWebView::GoForward();
}

wxString wxBrowser::GetRealLocation() {
	return m_realLocation;
}

wxBrowser::~wxBrowser() {
}
