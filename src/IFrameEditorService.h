#ifndef __IFRAMEEDITORSERVICE_H__
#define __IFRAMEEDITORSERVICE_H__

// EditorFrame implements this interface, which gathers functions
// used to query & modify the state of, and retreive pointers to, the active editor.
class IFrameEditorService {
public:
	virtual EditorCtrl* GetEditorCtrl() = 0;
	virtual EditorCtrl* GetEditorAndChangeType(const EditorChangeState& lastChangeState, EditorChangeType& newStatus) = 0;
	virtual void FocusEditor() = 0;
};


#endif // __IFRAMEEDITORSERVICE_H__
