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

size_t ConvertFromUTF8toString(const wxCharBuffer& utf8_buff, size_t utf8_buff_len, wxString& text) { // static
	// The length can never be longer in widechars than the bytecount in the uft8
	wxChar* buff = text.GetWriteBuf(utf8_buff_len);

	// Convert to widechar
	const size_t wchar_len = UTF8ToWChar(buff, utf8_buff_len, utf8_buff, utf8_buff_len);
	if (wchar_len == wxCONV_FAILED) return wxCONV_FAILED; // invalid conversion

	text.UngetWriteBuf(wchar_len);

	return wchar_len;
}

// this table gives the length of the UTF-8 encoding from its first character:
const unsigned char tableUtf8Lengths[256] = {
    // single-byte sequences (ASCII):
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 00..0F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10..1F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 20..2F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 30..3F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40..4F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 50..5F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60..6F
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 70..7F

    // these are invalid:
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 80..8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 90..9F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // A0..AF
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // B0..BF
    0, 0,                                            // C0,C1

    // two-byte sequences:
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // C2..CF
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // D0..DF

    // three-byte sequences:
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // E0..EF

    // four-byte sequences:
    4, 4, 4, 4, 4,                                   // F0..F4

    // these are invalid again (5- or 6-byte
    // sequences and sequences for code points
    // above U+10FFFF, as restricted by RFC 3629):
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // F5..FF
};


// Backport of wxMBConvStrictUTF8::ToWChar
size_t UTF8ToWChar(wchar_t *dst, size_t dstLen, const char *src, size_t srcLen) { // static
    wchar_t *out = dstLen ? dst : NULL;
    size_t written = 0;

    if ( srcLen == wxNO_LEN )
        srcLen = strlen(src) + 1;

    for ( const char *p = src; ; p++ )
    {
        if ( !(srcLen == wxNO_LEN ? *p : srcLen) )
        {
            // all done successfully, just add the trailing NULL if we are not
            // using explicit length
            if ( srcLen == wxNO_LEN )
            {
                if ( out )
                {
                    if ( !dstLen )
                        break;

                    *out = L'\0';
                }

                written++;
            }

            return written;
        }

        if ( out && !dstLen-- )
            break;

        wxUint32 code;
        unsigned char c = *p;

        if ( c < 0x80 )
        {
            if ( srcLen == 0 ) // the test works for wxNO_LEN too
                break;

            if ( srcLen != wxNO_LEN )
                srcLen--;

            code = c;
        }
        else
        {
            unsigned len = tableUtf8Lengths[c];
            if ( !len )
                break;

            if ( srcLen < len ) // the test works for wxNO_LEN too
                break;

            if ( srcLen != wxNO_LEN )
                srcLen -= len;

            //   Char. number range   |        UTF-8 octet sequence
            //      (hexadecimal)     |              (binary)
            //  ----------------------+----------------------------------------
            //  0000 0000 - 0000 007F | 0xxxxxxx
            //  0000 0080 - 0000 07FF | 110xxxxx 10xxxxxx
            //  0000 0800 - 0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
            //  0001 0000 - 0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            //
            //  Code point value is stored in bits marked with 'x',
            //  lowest-order bit of the value on the right side in the diagram
            //  above.                                         (from RFC 3629)

            // mask to extract lead byte's value ('x' bits above), by sequence
            // length:
            static const unsigned char leadValueMask[] = { 0x7F, 0x1F, 0x0F, 0x07 };

            // mask and value of lead byte's most significant bits, by length:
            static const unsigned char leadMarkerMask[] = { 0x80, 0xE0, 0xF0, 0xF8 };
            static const unsigned char leadMarkerVal[] = { 0x00, 0xC0, 0xE0, 0xF0 };

            len--; // it's more convenient to work with 0-based length here

            // extract the lead byte's value bits:
            if ( (c & leadMarkerMask[len]) != leadMarkerVal[len] )
                break;

            code = c & leadValueMask[len];

            // all remaining bytes, if any, are handled in the same way
            // regardless of sequence's length:
            for ( ; len; --len )
            {
                c = *++p;
                if ( (c & 0xC0) != 0x80 )
                    return wxCONV_FAILED;

                code <<= 6;
                code |= c & 0x3F;
            }
        }

#ifdef WC_UTF16
        // cast is ok because wchar_t == wxUint16 if WC_UTF16
        if ( encode_utf16(code, (wxUint16 *)out) == 2 )
        {
            if ( out )
                out++;
            written++;
        }
#else // !WC_UTF16
        if ( out )
            *out = code;
#endif // WC_UTF16/!WC_UTF16

        if ( out )
            out++;

        written++;
    }

    return wxCONV_FAILED;
}
