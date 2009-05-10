#ifndef __IPROJECTMANAGER_H__
#define __IPROJECTMANAGER_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/string.h>
#endif
#include "wx/filename.h"
#include "ProjectInfo.h"

class IProjectManager {
public:
	virtual bool HasProject() const = 0;
	virtual const wxFileName& GetRootPath() const = 0;

	virtual const map<wxString,wxString>& GetTriggers() const = 0;
	virtual void SetTrigger(const wxString& trigger, const wxString& path) = 0;
	virtual void ClearTrigger(const wxString& trigger) = 0;
};

#endif // __IPROJECTMANAGER_H__
