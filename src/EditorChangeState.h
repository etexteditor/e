#ifndef __EDITORCHANGESTATE_H__
#define __EDITORCHANGESTATE_H__

enum EditorChangeType {
	ECT_NO_EDITOR = 0,
	ECT_NEW_EDITOR,
	ECT_EDITOR_CHANGED,
	ECT_NO_CHANGE
};

struct EditorChangeState {
	EditorChangeState():id(0), changeToken(0){}
	EditorChangeState(const unsigned int _id, const unsigned int _changeToken):id(_id), changeToken(_changeToken){}
	unsigned int id;
	unsigned int changeToken;

	bool operator==(const EditorChangeState& ecs) const throw() {return id == ecs.id && changeToken == ecs.changeToken;};
	bool operator!=(const EditorChangeState& ecs) const throw() {return id != ecs.id || changeToken != ecs.changeToken;};
};

class IGetChangeState {
public:
	virtual EditorChangeState GetChangeState() const = 0;
};

#endif // __EDITORCHANGESTATE_H__
