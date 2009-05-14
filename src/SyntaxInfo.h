#ifndef __SYNTAXINFO_H__
#define __SYNTAXINFO_H__

#include "tmAction.h"

class matcher;

class cxSyntaxInfo : public tmAction {
public:
	cxSyntaxInfo(unsigned int bundleid, unsigned int syntaxid)
		: bundleId(bundleid), syntaxId(syntaxid), topmatcher(NULL) {};
    virtual ~cxSyntaxInfo() {};

	bool IsOk() const {return !name.empty();};
	bool IsSyntax() const {return true;};

	wxArrayString filewild;
	wxString firstline;
	unsigned int bundleId;
	unsigned int syntaxId;
	matcher* topmatcher;
};

#endif // __SYNTAXINFO_H__
