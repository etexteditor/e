#ifndef __BUNDLEINFO_H__
#define __BUNDLEINFO_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/string.h>
   #include <wx/datetime.h>
#endif

class cxBundleInfo {
public:
	cxBundleInfo() {};
	cxBundleInfo(int id, const wxString& name, const wxDateTime& modDate, bool isDisabled=false)
		: id(id), dirName(name), modDate(modDate), isDisabled(isDisabled) {};
	int id;
	wxString dirName;
	wxDateTime modDate;
	bool isDisabled;
};

#endif // __BUNDLEINFO_H__
