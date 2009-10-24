#ifndef __IEDITORSEARCH_H__
#define __IEDITORSEARCH_H__

#include "FindFlags.h"

class wxString;

class IEditorSearch {
public:
	virtual cxFindResult Find(const wxString& text, int options=0) = 0;
	virtual cxFindResult FindNext(const wxString& text, int options=0) = 0;
	virtual cxFindResult FindPrevious(const wxString& text, int options=0) = 0;
	virtual bool Replace(const wxString& searchtext, const wxString& replacetext, int options=0) = 0;
	virtual int ReplaceAll(const wxString& searchtext, const wxString& replacetext, int options=0) = 0;
	virtual void ClearSearchHighlight() = 0;
};

#endif // __IEDITORSEARCH_H__
