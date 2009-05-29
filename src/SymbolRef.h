#ifndef __SYMBOLREF_H__
#define __SYMBOLREF_H__

class wxString;

struct SymbolRef {
	unsigned int start;
	unsigned int end;
	const wxString* transform;
};

#endif // __SYMBOLREF_H__
