#ifndef __IPROJECTMANAGER_H__
#define __IPROJECTMANAGER_H__

class IProjectManager {
public:
	virtual bool HasProject() const = 0;
	virtual const wxFileName& GetProject() const = 0;

	virtual bool LoadProjectInfo(const wxString& path, bool onlyFilters, cxProjectInfo& projectInfo) const = 0;

	virtual const map<wxString,wxString>& GetTriggers() const = 0;
	virtual void SetTrigger(const wxString& trigger, const wxString& path) = 0;
	virtual void ClearTrigger(const wxString& trigger) = 0;
};

#endif // __IPROJECTMANAGER_H__
