#ifndef __SYMBOLREF_H__
#define __SYMBOLREF_H__

#include <wx/string.h>

struct SymbolRef {
	unsigned int start;
	unsigned int end;
	const wxString* transform;
};

#endif // __SYMBOLREF_H__
