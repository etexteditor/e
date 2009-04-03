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

#ifndef __URLENCODE_H__
#define __URLENCODE_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "wx/uri.h"

class UrlEncode : public wxURI {
public:
	static wxString Encode(const wxString& url) {
		const wxChar* uri = url.c_str();
		wxString newUrl;

		while(*uri) {
			if(IsUnreserved(*uri) || *uri == wxT('/') || *uri == wxT(':'))
				newUrl += *uri++;
			else Escape(newUrl, *uri++);
		}

		return newUrl;
	};

	static wxString EncodeFilename(const wxString& filename) {
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
	};

	static wxString EscapeUrl(const wxString& url) {
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
	};
};

#endif // __URLENCODE_H__
