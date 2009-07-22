#include "Strings.h"
#include "wx/tokenzr.h"

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
