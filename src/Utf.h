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

#ifndef __UTF_H__
#define __UTF_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// UTF-8 Support functions

unsigned int utf8_len(char c);

size_t ConvertToUTF8(const char* source, const size_t source_len, const wxMBConv& conv, wxCharBuffer& temp_buff,
						size_t& temp_buff_len, wxWCharBuffer& wchar_buff, size_t& wchar_buff_len,
						wxCharBuffer& utf8_buff, size_t& utf8_buff_len, size_t char_len);
size_t ConvertFromUTF8(const wxCharBuffer& utf8_buff, const wxMBConv& conv, wxWCharBuffer& wchar_buff,
						size_t& wchar_buff_len, wxCharBuffer& dest_buff, size_t& dest_buff_len, size_t char_len);

size_t ConvertFromUTF8toString(const wxCharBuffer& utf8_buff, size_t utf8_buff_len, wxString& text);

#endif // __UTF_H__
