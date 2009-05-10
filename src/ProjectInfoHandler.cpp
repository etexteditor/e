#include "ProjectInfoHandler.h"
#include "Strings.h"
#include "wx/dir.h"

void ProjectInfoHandler::SetRoot(const wxFileName& path) {
	m_prjPath = path;
	m_projectInfo.Clear();

	const wxString rootPath = path.GetPath() + wxFILE_SEP_PATH;
	m_projectInfo.Load(rootPath, rootPath, false);
	//LoadProjectInfo(rootPath, false, m_projectInfo);
}

void ProjectInfoHandler::SaveRootInfo() const {
	m_projectInfo.Save(m_prjPath.GetPath());
	//SaveProjectInfo(m_projectInfo);
}

bool ProjectInfoHandler::GetDirAndFileLists(const wxString& path, wxArrayString& dirs, wxArrayString& files) const {
	wxASSERT(!path.empty());

	// Open directory
	wxDir d;
	d.Open(path);
	if (!d.IsOpened()) return false;

	// Check for project settings
	wxArrayString includeDirs;
	wxArrayString excludeDirs;
	wxArrayString includeFiles;
	wxArrayString excludeFiles;
	GetFilters(path, includeDirs, excludeDirs, includeFiles, excludeFiles);

	// Get all subdirs
	wxString eachFilename;
 	int style = wxDIR_DIRS;
    if (d.GetFirst(&eachFilename, wxEmptyString, style))
    {
        do
        {
            if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
            {
                if (MatchFilter(eachFilename, includeDirs, excludeDirs)) {
					dirs.Add(eachFilename);
				}
            }
        }
        while (d.GetNext(&eachFilename));
    }
	dirs.Sort(wxStringSortAscendingNoCase);

	// Get all files
	style = wxDIR_FILES;
	if (d.GetFirst(&eachFilename, wxEmptyString, style))
    {
        do
        {
            if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
            {
                if (MatchFilter(eachFilename, includeFiles, excludeFiles)) {
					files.Add(eachFilename);
				}
            }
        }
        while (d.GetNext(&eachFilename));
    }
	files.Sort(wxStringSortAscendingNoCase);

	return true;
}

void ProjectInfoHandler::GetFilters(const wxString& path, wxArrayString& incDirs, wxArrayString& excDirs, wxArrayString& incFiles, wxArrayString& excFiles) const {
	wxFileName dirPath(path, wxEmptyString);

	while(1) { // Eventually we will walk back to the project root, so the first if condition will return.
		if (m_prjPath == dirPath) {
			// Return root filters
			incDirs = m_projectInfo.includeDirs;
			excDirs = m_projectInfo.excludeDirs;
			incFiles = m_projectInfo.includeFiles;
			excFiles = m_projectInfo.excludeFiles;
			return;
		}

		cxProjectInfo info;
		
		if (info.Load(m_prjPath, dirPath.GetPath(), true)) {
			incDirs = info.includeDirs;
			excDirs = info.excludeDirs;
			incFiles = info.includeFiles;
			excFiles = info.excludeFiles;
			return;
		}

		// See if we can inherit filters from parent
		dirPath.RemoveLastDir();
	}
}


void ProjectInfoHandler::SetTrigger(const wxString& trigger, const wxString& path) {
	m_projectInfo.triggers[trigger] = path;
	SaveRootInfo();
}

void ProjectInfoHandler::ClearTrigger(const wxString& trigger) {
	m_projectInfo.triggers.erase(trigger);
}

bool ProjectInfoHandler::MatchFilter(const wxString& name, const wxArrayString& incFilter, const wxArrayString& excFilter) { // static
	if (!incFilter.IsEmpty()) {
		bool doInclude = false;
		for (unsigned int i = 0; i < incFilter.GetCount(); ++i) {
			if (wxMatchWild(incFilter[i], name, false)) {
				doInclude = true;
				break;
			}
		}

		if (!doInclude) return false;
	}

	for (unsigned int i = 0; i < excFilter.GetCount(); ++i) {
		if (wxMatchWild(excFilter[i], name, false)) {
			return false;
		}
	}

	return true;
}
