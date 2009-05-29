#ifndef __ITMTHEMEHANDLER_H__
#define __ITMTHEMEHANDLER_H__

#include "IGetPListHandlerRef.h"

class wxString;
class wxFont;
struct tmTheme;

class ITmThemeHandler:
	public virtual IGetPListHandlerRef
{
public:
	virtual bool SetTheme(const char* uuid) = 0;
	virtual void SetDefaultTheme() = 0;
	virtual const tmTheme& GetTheme() const = 0;
	virtual const wxString& GetCurrentThemeName() const = 0;
	virtual const wxFont& GetFont() const = 0;
	virtual void SetFont(const wxFont& font) = 0;
};

#endif // __ITMTHEMEHANDLER_H__
