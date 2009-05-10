#ifndef __IUPDATEPANEL_H__
#define __IUPDATEPANEL_H__

// This simple interface allows an EditorCtrl to tell the
// panel that it is embedded in to update.
class IUpdatePanel {
public:
	virtual void UpdatePanel() = 0;
};

#endif // __IUPDATEPANEL_H__
