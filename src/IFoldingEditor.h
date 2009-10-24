#ifndef __IFOLDINGEDITOR_H__
#define __IFOLDINGEDITOR_H__

#include <wx/platform.h>
#include <vector>

struct cxFold;
class interval;

class IFoldingEditor {
public:
	virtual bool IsPosInFold(unsigned int pos, unsigned int* fold_start=NULL, unsigned int* fold_end=NULL) = 0;
	virtual void UnFoldParents(unsigned int line_id) = 0;
	virtual const std::vector<cxFold>& GetFolds() const = 0;

	// This isn't used by Lines directly, but is passed to internal data member Line.
	// The only pubilc use of this member is in Lines; the underlying EditorCtrl member
	// is used internally in the editor control.
	virtual const interval& GetHlBracket() const = 0;
};

#endif // __IFOLDINGEDITOR_H__
