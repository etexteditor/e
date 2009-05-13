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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/string.h"
#endif

#include "wx/uri.h"

class UrlEncode : public wxURI {
public:
	static wxString Encode(const wxString& url);
	static wxString EncodeFilename(const wxString& filename);
	static wxString EscapeUrl(const wxString& url);
};

#endif // __URLENCODE_H__
