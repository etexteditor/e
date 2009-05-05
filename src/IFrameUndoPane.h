#ifndef __IFRAMEUNDOPANE_H__
#define __IFRAMEUNDOPANE_H__

class wxString;

class IFrameUndoPane {
public:
	virtual void SetUndoPaneCaption(const wxString& caption) = 0;
};

#endif