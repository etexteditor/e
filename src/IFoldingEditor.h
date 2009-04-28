#ifndef __IFOLDINGEDITOR_H__
#define __IFOLDINGEDITOR_H__

// This header defines an interface for editor folds.
// This is implemented by EditorCtrl, and is used by Lines.
// We define an interface for this to provide a "wedge" for testing Lines
// w/o needing to pass in a real editor.

class IFoldingEditor {
public:
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

	virtual bool IsPosInFold(unsigned int pos, unsigned int* fold_start=NULL, unsigned int* fold_end=NULL) = 0;
	virtual void UnFoldParents(unsigned int line_id) = 0;
	virtual const vector<cxFold>& GetFolds() const = 0;
};

#endif // __IFOLDINGEDITOR_H__
