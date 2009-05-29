#ifndef __IPRINTABLEDOCUMEHT_H__
#define __IPRINTABLEDOCUMEHT_H__

#include "Document.h"

class IPrintableDocument {
public:
	virtual wxString GetName() const = 0;
	virtual const DocumentWrapper& GetDocument() const = 0;
	virtual const vector<unsigned int>& GetOffsets() const = 0;
};

#endif // __IPRINTABLEDOCUMEHT_H__
