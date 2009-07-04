#ifndef __PROJECTINFOHANDLER_H__
#define __PROJECTINFOHANDLER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <map>

#include <wx/filename.h>
#include "ProjectInfo.h"

class ProjectInfoHandler {
public:
	void SetRoot(const wxFileName& path);
	const wxFileName& GetRoot() const {return m_prjPath;};
	bool HasProject() const {return m_prjPath.IsOk();};

	// Root info
	const cxProjectInfo& GetRootInfo() const {return m_projectInfo;};
	void SaveRootInfo() const;

	bool GetDirAndFileLists(const wxString& path, wxArrayString& dirs, wxArrayString& files) const;

	// Filters
	void GetFilters(const wxString& path, wxArrayString& incDirs, wxArrayString& excDirs, wxArrayString& incFiles, wxArrayString& excFiles) const;
	static bool MatchFilter(const wxString& name, const wxArrayString& incFilter, const wxArrayString& excFilter);

	// GotoFile triggers
	const std::map<wxString,wxString>& GetTriggers() const {return m_projectInfo.triggers;};
	void SetTrigger(const wxString& trigger, const wxString& path);
	void ClearTrigger(const wxString& trigger);

private:

	// Member variables
	wxFileName m_prjPath;
	cxProjectInfo m_projectInfo;
};

#endif // __PROJECTINFOHANDLER_H__
