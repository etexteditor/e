#ifndef __FOLD_H__
#define __FOLD_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

// This header defines an interface for editor folds.
// This is implemented by EditorCtrl, and is used by Lines.
// We define an interface for this to provide a "wedge" for testing Lines
// w/o needing to pass in a real editor.

enum cxFoldType {
	cxFOLD_START,
	cxFOLD_START_FOLDED,
	cxFOLD_END
};

struct cxFold {
public:
	cxFold(unsigned int line, cxFoldType type, unsigned int indent);
	cxFold(unsigned int line) : line_id(line) {};
	bool operator<(const cxFold& f) const {return line_id < f.line_id;};
	bool operator<(unsigned int line) const {return line_id < line;};
	unsigned int line_id;
	cxFoldType type;
	unsigned int count;
	unsigned int indent;
};

#endif // __FOLD_H__
