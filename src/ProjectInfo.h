/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __PROJECTINFO_H__
#define __PROJECTINFO_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(disable:4786)
    #pragma warning(push, 1)
#endif
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class cxProjectInfo {
public:
	cxProjectInfo(): isRoot(false), hasFilters(false) {};
	cxProjectInfo(const cxProjectInfo& info) {
		isRoot = info.isRoot;
		hasFilters = info.hasFilters;
		path = info.path;
		includeDirs = info.includeDirs;
		excludeDirs = info.excludeDirs;
		includeFiles = info.includeFiles;
		excludeFiles = info.excludeFiles;
		env = info.env;
	};

	void Clear() {
		isRoot = false;
		hasFilters = false;
		path.clear();
		includeDirs.Empty();
		excludeDirs.Empty();
		includeFiles.Empty();
		excludeFiles.Empty();
		env.clear();
	};

	bool IsEmpty() const {
		return hasFilters || !env.empty() || !triggers.empty();
	};

	bool IsFileIncluded(const wxString& file_name) const {
		if (!includeFiles.IsEmpty()) {
			bool doInclude = false;
			for (unsigned int i = 0; i < includeFiles.GetCount(); ++i) {
				if (wxMatchWild(includeFiles[i], file_name, false)) {
					doInclude = true;
					break;
				}
			}

			if (!doInclude) return false;
		}

		for (unsigned int i = 0; i < excludeFiles.GetCount(); ++i) {
			if (wxMatchWild(excludeFiles[i], file_name, false)) {
				return false;
			}
		}

		return true;
	};

	bool IsDirectoryIncluded(const wxString& dir_name) const {
		if (!includeFiles.IsEmpty()) {
			bool doInclude = false;
			for (unsigned int i = 0; i < includeDirs.GetCount(); ++i) {
				if (wxMatchWild(includeDirs[i], dir_name, false)) {
					doInclude = true;
					break;
				}
			}

			if (!doInclude) return false;
		}

		for (unsigned int i = 0; i < excludeFiles.GetCount(); ++i) {
			if (wxMatchWild(excludeDirs[i], dir_name, false)) {
				return false;
			}
		}

		return true;
	};

	bool isRoot;
	wxString path;
	bool hasFilters;
	wxArrayString includeDirs;
	wxArrayString excludeDirs;
	wxArrayString includeFiles;
	wxArrayString excludeFiles;
	map<wxString, wxString> env;
	map<wxString, wxString> triggers;
};

#endif // __PROJECTINFO_H__
