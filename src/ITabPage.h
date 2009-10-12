#ifndef __ITABPAGE_H__
#define __ITABPAGE_H__

// Pre-definitions
class EditorCtrl;
class eFrameSettings;

class ITabPage {
public:
	virtual EditorCtrl* GetActiveEditor() = 0;
	virtual const char** RecommendedIcon() const = 0;
	virtual void SaveSettings(unsigned int i, eFrameSettings& settings) = 0;
};

#endif // __ITABPAGE_H__
