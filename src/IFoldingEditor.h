#ifndef __IFOLDINGEDITOR_H__
#define __IFOLDINGEDITOR_H__

#include <wx/platform.h>
// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

struct cxFold;
class interval;

class IFoldingEditor {
public:
	virtual bool IsPosInFold(unsigned int pos, unsigned int* fold_start=NULL, unsigned int* fold_end=NULL) = 0;
	virtual void UnFoldParents(unsigned int line_id) = 0;
	virtual const vector<cxFold>& GetFolds() const = 0;

	// This isn't used by Lines directly, but is passed to internal data member Line.
	// The only use of this member is in Lines (though of course the underlying EditorCtrl member
	// is used internally in the editor control.
	virtual const interval& GetHlBracket() const = 0;
};

#endif // __IFOLDINGEDITOR_H__
