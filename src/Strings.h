#ifndef __STRINGS_H__
#define __STRINGS_H__

#include "wx/arrstr.h"

int wxCMPFUNC_CONV wxStringSortAscendingNoCase(wxString* s1, wxString* s2);

int HexToNumber(wxChar hexDigit);

void SimpleHtmlEncode(wxString& s);
wxString URLDecode(const wxString &value);

// Back-port of wxJoin and wxSplit from newer versions of wxWidgets

// ----------------------------------------------------------------------------
// helper functions for working with arrays
// ----------------------------------------------------------------------------

// by default, these functions use the escape character to escape the
// separators occuring inside the string to be joined, this can be disabled by
// passing '\0' as escape

WXDLLIMPEXP_BASE wxString wxJoin(const wxArrayString& arr,
                                 const wxChar sep,
                                 const wxChar escape = wxT('\\'));

WXDLLIMPEXP_BASE wxArrayString wxSplit(const wxString& str,
                                       const wxChar sep,
                                       const wxChar escape = wxT('\\'));


#endif // __STRINGS_H__
