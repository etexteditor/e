#ifndef __FOLD_H__
#define __FOLD_H__

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
