#ifndef __IFRAMEEDITORSERVICE_H__
#define __IFRAMEEDITORSERVICE_H__

// EditorFrame implements this interface, which gathers functions
// used to query & modify the state of, and retreive pointers to, the active editor.

// The editor return type is actually class EditorCtrl*, but we're trying to minimize the dependency on that header.

class IFrameEditorService {
public:
	virtual wxControl* GetEditorAndChangeType(const EditorChangeState& lastChangeState, EditorChangeType& newStatus) = 0;
	virtual void FocusEditor() = 0;
};


#endif // __IFRAMEEDITORSERVICE_H__
