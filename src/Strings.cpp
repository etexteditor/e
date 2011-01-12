#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/tokenzr.h>

#include "Strings.h"
#include "Utf.h"

#ifdef __WXMSW__
void InplaceConvertCRLFtoLF(wxString& text) {
	// WINDOWS ONLY!! newline conversion
	// The build-in replace is too slow so we do
	// it manually (at the price of some memory)
	//copytext.Replace(wxT("\r\n"), wxT("\n"));
	wxString newtext;
	newtext.reserve(text.size());
	unsigned int lastPos = 0;
	for (unsigned int i = 0; i < text.size(); ++i) {
		if (text[i] == wxT('\r')) {
			newtext += text.substr(lastPos, i - lastPos);
			lastPos = i + 1;
		}
	}
	if (lastPos < text.size()) newtext += text.substr(lastPos, text.size() - lastPos);
	text.swap(newtext);
}
#endif

int wxCMPFUNC_CONV wxStringSortAscendingNoCase(wxString* s1, wxString* s2)
{
    return s1->CmpNoCase(*s2);
}

void SimpleHtmlEncode(wxString& s) {
	s.Replace(wxT("&"), wxT("&amp;"));
	s.Replace(wxT("<"), wxT("&lt;"));
	s.Replace(wxT(">"), wxT("&gt;"));
}

int HexToNumber(wxChar hexDigit){
	if (wxT('0') <= hexDigit && hexDigit <= wxT('9'))
		return hexDigit - wxT('0');

	if (wxT('a') <= hexDigit && hexDigit <= wxT('f'))
		return hexDigit - wxT('a') + 10;

	if (wxT('A') <= hexDigit && hexDigit <= wxT('F'))
		return hexDigit - wxT('A') + 10;

	// illegal digit.
	return -1;
}

// Decode an encoded URL string.
wxString URLDecode(const wxString &value) {
	wxString szDecoded;
	wxString szEncoded = value;

	unsigned int nEncodedPos = 0;

	// Replace + with space
	szEncoded.Replace(wxT("+"), wxT(" "));

	// Replace hex encoded characters
	while( nEncodedPos < szEncoded.length() ) {
		wxChar nextChar = szEncoded.GetChar(nEncodedPos);
		nEncodedPos++;

		if(nextChar != wxT('%')) szDecoded.Append( nextChar );
		else
		{
			if( isxdigit(szEncoded.GetChar(nEncodedPos)) && isxdigit(szEncoded.GetChar(nEncodedPos+1)) ) {
				
				int n1 = HexToNumber(szEncoded.GetChar(nEncodedPos));
				int n2 = HexToNumber(szEncoded.GetChar(nEncodedPos+1));

				szDecoded.Append( (wxChar) ((n1 << 4) | n2) );
				nEncodedPos += 2;
			}
		}
	}

	return szDecoded;
}


// Returns the indent level of the given text, in spaces.
unsigned int CountTextIndent(const wxString& text, const unsigned int tabWidth) {
	unsigned int indent = 0;
	for (unsigned int i = 0; i < text.size(); ++i) {
		const wxChar c = text[i];

		if (c == '\t') {
			// it is ok to have a few spaces before tab (making one mixed tab)
			const unsigned int spaces = tabWidth - (indent % tabWidth);
			indent += spaces;
		}
		else if (c == ' ') indent++;
		else break;
	}

	return indent;
}


// ===========================================================================
// wxJoin and wxSplit
// ===========================================================================

wxString wxJoin(const wxArrayString& arr, const wxChar sep, const wxChar escape)
{
    size_t count = arr.size();
    if ( count == 0 )
        return wxEmptyString;

    wxString str;

    // pre-allocate memory using the estimation of the average length of the
    // strings in the given array: this is very imprecise, of course, but
    // better than nothing
    str.reserve(count*(arr[0].length() + arr[count-1].length()) / 2);

    if ( escape == wxT('\0') )
    {
        // escaping is disabled:
        for ( size_t i = 0; i < count; i++ )
        {
            if (i) str += sep;
            str += arr[i];
        }
    }
    else // use escape character
    {
        for ( size_t n = 0; n < count; n++ )
        {
            if ( n ) str += sep;

            for ( wxString::const_iterator i = arr[n].begin(),end = arr[n].end();
                  i != end;++i )
            {
                const wxChar ch = *i;
                if ( ch == sep )
                    str += escape;      // escape this separator
                str += ch;
            }
        }
    }

    str.Shrink(); // release extra memory if we allocated too much
    return str;
}

wxArrayString wxSplit(const wxString& str, const wxChar sep, const wxChar escape)
{
    if ( escape == wxT('\0') )
    {
        // simple case: we don't need to honour the escape character
        return wxStringTokenize(str, sep, wxTOKEN_RET_EMPTY_ALL);
    }

    wxArrayString ret;
    wxString curr;
    wxChar prev = wxT('\0');

    for ( wxString::const_iterator i = str.begin(),end = str.end();
          i != end; ++i )
    {
        const wxChar ch = *i;

        if ( ch == sep )
        {
            if ( prev == escape )
            {
                // remove the escape character and don't consider this
                // occurrence of 'sep' as a real separator
                *curr.rbegin() = sep;
            }
            else // real separator
            {
                ret.push_back(curr);
                curr.clear();
            }
        }
        else // normal character
        {
            curr += ch;
        }

        prev = ch;
    }

    // add the last token
    if ( !curr.empty() || prev == sep )
        ret.Add(curr);

    return ret;
}

bool DetectTextEncoding(const char* buffer, size_t len, wxFontEncoding& encoding, unsigned int& BOM_len) {
	wxASSERT(buffer);
	if (!buffer || len == 0) return false;

	const char* buff_ptr = buffer;
	const char* buff_end = &buffer[len];
	wxFontEncoding enc = wxFONTENCODING_DEFAULT;

	// Check if the buffer starts with a BOM (Byte Order Marker)
	if (len >= 2) {
		if (len >= 4 && memcmp(buffer, "\xFF\xFE\x00\x00", 4) == 0) {enc = wxFONTENCODING_UTF32LE; BOM_len = 4;}
		else if (len >= 4 && memcmp(buffer, "\x00\x00\xFE\xFF", 4) == 0) {enc = wxFONTENCODING_UTF32BE; BOM_len = 4;}
		else if (memcmp(buffer, "\xFF\xFE", 2) == 0) {enc = wxFONTENCODING_UTF16LE; BOM_len = 2;}
		else if (memcmp(buffer, "\xFE\xFF", 2) == 0) {enc = wxFONTENCODING_UTF16BE; BOM_len = 2;}
		else if (len >= 3 && memcmp(buffer, "\xEF\xBB\xBF", 3) == 0) {enc = wxFONTENCODING_UTF8; BOM_len = 3;}
		else if (len >= 5 && memcmp(buffer, "\x2B\x2F\x76\x38\x2D", 5) == 0) {enc = wxFONTENCODING_UTF7; BOM_len = 5;}

		buff_ptr += BOM_len;
	}

	// If the file starts with a leading < (less) sign, it is probably an XML file
	// and we can determine the encoding by how the sign is encoded.
	if (enc == wxFONTENCODING_DEFAULT && len >= 2) {
		if (len >= 4 && memcmp(buffer, "\x3C\x00\x00\x00", 4) == 0) enc = wxFONTENCODING_UTF32LE;
		else if (len >= 4 && memcmp(buffer, "\x00\x00\x00\x3C", 4) == 0) enc = wxFONTENCODING_UTF32BE;
		else if (memcmp(buffer, "\x3C\x00", 2) == 0) enc = wxFONTENCODING_UTF16LE;
		else if (memcmp(buffer, "\x00\x3C", 2) == 0) enc = wxFONTENCODING_UTF16BE;
	}

	// Unicode Detection
	if (enc == wxFONTENCODING_DEFAULT) {
		unsigned int null_byte_count = 0;
		unsigned int utf_bytes = 0;
		unsigned int good_utf_count = 0;
		unsigned int bad_utf_count = 0;
		unsigned int bad_utf32_count = 0;
		unsigned int bad_utf16_count = 0;
		unsigned int nl_utf32le_count = 0;
		unsigned int nl_utf32be_count = 0;
		unsigned int nl_utf16le_count = 0;
		unsigned int nl_utf16be_count = 0;

		while (buff_ptr != buff_end) {
			if (*buff_ptr == 0) ++null_byte_count;

			// Detect UTF-8 by scanning for invalid sequences
			if (utf_bytes == 0) {
				if ((*buff_ptr & 0xC0) == 0x80 || *buff_ptr == 0) ++bad_utf_count;
				else {
					utf_bytes = utf8_len(*buff_ptr) - 1;
					if (utf_bytes > 3) {
						++bad_utf_count;
						utf_bytes = 0;
					}
				}
			}
			else if ((*buff_ptr & 0xC0) == 0x80) {
				--utf_bytes;
				if (utf_bytes == 0) ++good_utf_count;
			}
			else {
				++bad_utf_count;
				utf_bytes = 0;
			}

			// Detect UTF-32 by scanning for newlines (and lack of null chars)
			if ((wxUIntPtr)buff_ptr % 4 == 0 && buff_ptr+4 <= buff_end) {
				if (*((wxUint32*)buff_ptr) == 0) ++bad_utf32_count;
				if (*((wxUint32*)buff_ptr) == wxUINT32_SWAP_ON_BE(0x0A)) ++nl_utf32le_count;
				if (*((wxUint32*)buff_ptr) == wxUINT32_SWAP_ON_LE(0x0A)) ++nl_utf32be_count;
			}

			// Detect UTF-16 by scanning for newlines (and lack of null chars)
			if ((wxUIntPtr)buff_ptr % 2 == 0  && buff_ptr+4 <= buff_end) {
				if (*((wxUint16*)buff_ptr) == 0) ++bad_utf16_count;
				if (*((wxUint16*)buff_ptr) == wxUINT16_SWAP_ON_BE(0x0A)) ++nl_utf16le_count;
				if (*((wxUint16*)buff_ptr) == wxUINT16_SWAP_ON_LE(0x0A)) ++nl_utf16be_count;
			}

			++buff_ptr;
		}

		if (bad_utf_count == 0) enc = wxFONTENCODING_UTF8;
		else if (bad_utf32_count == 0 && nl_utf32le_count > len / 400) enc = wxFONTENCODING_UTF32LE;
		else if (bad_utf32_count == 0 && nl_utf32be_count > len / 400) enc = wxFONTENCODING_UTF32BE;
		else if (bad_utf16_count == 0 && nl_utf16le_count > len / 200) enc = wxFONTENCODING_UTF16LE;
		else if (bad_utf16_count == 0 && nl_utf16be_count > len / 200) enc = wxFONTENCODING_UTF16BE;
		else if (null_byte_count) return false; // Maybe this is a binary file?
	}

	// If we can't detect encoding and it does not contain null bytes just set it to the default encoding.
	if (enc == wxFONTENCODING_DEFAULT)
		enc = wxFONTENCODING_SYSTEM;

	encoding = enc;
	return true;
}

bool IsWordChar(const wxChar c) {
	return (wxIsalnum(c) || c == '_');
}

bool IsBigWordChar(const wxChar c) {
	return !wxIsspace(c);
}

