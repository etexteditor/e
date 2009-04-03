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

#include "Utf.h"

unsigned int utf8_len(char c) {
	if ((c & 0x80) == 0x00) return 1;
	else if ((c & 0xE0) == 0xC0) return 2;
	else if ((c & 0xF0) == 0xE0) return 3;
	else if ((c & 0xF8) == 0xF0) return 4;
	else return 5; // invalid length
}

size_t ConvertFromUTF8(const wxCharBuffer& utf8_buff, const wxMBConv& conv, wxWCharBuffer& wchar_buff,
						size_t& wchar_buff_len, wxCharBuffer& dest_buff, size_t& dest_buff_len,
						size_t char_len) { // static
	// Calculate length of conversion to widechar
	size_t wchar_len = wxConvUTF8.MB2WC(NULL, utf8_buff, 0);
	if (wchar_len == (size_t)-1) return (size_t)-1; // invalid conversion

	// Extend widechar buffer if needed
	if (wchar_buff_len < wchar_len + sizeof(wxChar)) {
		wchar_buff_len = wchar_len + sizeof(wxChar);
		wchar_buff = wxWCharBuffer(wchar_buff_len);
	}

	// Convert to widechar
	wchar_len = wxConvUTF8.MB2WC(wchar_buff.data(), utf8_buff, wchar_buff_len);
	if (wchar_len == (size_t)-1) return (size_t)-1; // invalid conversion

	// Calculate length of conversion to dest encoding
	size_t dest_len = conv.WC2MB(NULL, wchar_buff, 0);
	if (dest_len == (size_t)-1) return (size_t)-1; // invalid conversion

	// Extend dest buffer if needed
	if (dest_buff_len < dest_len + char_len) {
		dest_buff_len = dest_len + char_len;
		dest_buff = wxCharBuffer(dest_buff_len);
	}

	// Convert to dest encoding
	dest_len = conv.WC2MB(dest_buff.data(), wchar_buff,  dest_buff_len);
	if (dest_len == (size_t)-1) return (size_t)-1; // invalid conversion

	return dest_len;
}

size_t ConvertToUTF8(const char* source, const size_t source_len, const wxMBConv& conv, wxCharBuffer& temp_buff,
					 size_t& temp_buff_len, wxWCharBuffer& wchar_buff, size_t& wchar_buff_len,
					 wxCharBuffer& utf8_buff, size_t& utf8_buff_len, size_t char_len) { // static
	// We have to copy the source string to a temporary buffer so that we can
	// make it null terminated.
	if (temp_buff_len < source_len+char_len) {
		temp_buff_len = source_len+char_len;
		temp_buff = wxCharBuffer(temp_buff_len);
	}
	memcpy(temp_buff.data(), source, source_len);
	for (unsigned int i = 0; i < char_len; ++i) temp_buff.data()[source_len+i] = '\0';

	// Calculate length of conversion to widechar
	size_t wchar_len = conv.MB2WC(NULL, temp_buff, 0);
	if (wchar_len == (size_t)-1) return (size_t)-1; // invalid conversion

	// Extend widechar buffer if needed
	if (wchar_buff_len < wchar_len + sizeof(wxChar)) {
		wchar_buff_len = wchar_len + sizeof(wxChar);
		wchar_buff = wxWCharBuffer(wchar_buff_len);
	}

	// Convert to widechar
	wchar_len = conv.MB2WC(wchar_buff.data(), temp_buff, wchar_buff_len);
	if (wchar_len == (size_t)-1) return (size_t)-1; // invalid conversion

	// Calculate length of conversion to UTF-8
	size_t utf8_len = wxConvUTF8.WC2MB(NULL, wchar_buff, 0);
	if (utf8_len == (size_t)-1) return (size_t)-1; // invalid conversion

	// Extend UTF-8 buffer if needed
	if (utf8_buff_len < utf8_len) {
		utf8_buff_len = utf8_len + 1;
		utf8_buff = wxCharBuffer(utf8_buff_len);
	}

	// Convert to UTF-8
	utf8_len = wxConvUTF8.WC2MB(utf8_buff.data(), wchar_buff, utf8_buff_len);
	if (utf8_len == (size_t)-1) return (size_t)-1; // invalid conversion

	return utf8_len;
}
