#ifndef __ISETTINGS_H__
#define  __ISETTINGS_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/longlong.h>
	#include <wx/string.h>
#endif

class ISettings {
public:
	virtual bool GetSettingBool(const wxString& name, bool& value) const = 0;
	virtual bool GetSettingInt(const wxString& name, int& value) const = 0;
	virtual bool GetSettingLong(const wxString& name, wxLongLong& value) const = 0;
	virtual bool GetSettingString(const wxString& name, wxString& value) const = 0;
};

#endif
