#ifndef __IPRINTABLEDOCUMEHT_H__
#define __IPRINTABLEDOCUMEHT_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

#include "Document.h"

class IPrintableDocument {
public:
	virtual wxString GetName() const = 0;
	virtual const DocumentWrapper& GetDocument() const = 0;
	virtual const vector<unsigned int>& GetOffsets() const = 0;
};

#endif // __IPRINTABLEDOCUMEHT_H__
