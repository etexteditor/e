#ifndef __INAMEDDOCMENT_H__
#define __INAMEDDOCMENT_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

#include "Document.h"

class INamedDocument {
public:
	virtual wxString GetName() const = 0;
	virtual const DocumentWrapper& GetDocument() const = 0;
	virtual const vector<unsigned int>& GetOffsets() const = 0;
};

#endif // __INAMEDDOCMENT_H__
