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

#include "wx/filename.h"

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
	cxProjectInfo();
	cxProjectInfo(const cxProjectInfo& info);
	cxProjectInfo(const wxFileName &rootPath, const wxString& path, bool onlyFilters);

	void Clear();

	void ClearFilters();
	void SetFilters(const wxArrayString& ind, const wxArrayString& exd, const wxArrayString& inf, const wxArrayString& exf);

	bool Load(const wxFileName& rootPath, const wxString& path, bool onlyFilters);
	void Save(const wxString& rootPath) const;

	bool IsEmpty() const;

	bool IsFileIncluded(const wxString& file_name) const;
	bool IsDirectoryIncluded(const wxString& dir_name) const;

	bool IsRoot() const { return isRoot; }
	bool HasFilters() const { return hasFilters; }

	wxString path;
	wxArrayString includeDirs;
	wxArrayString excludeDirs;
	wxArrayString includeFiles;
	wxArrayString excludeFiles;
	map<wxString, wxString> env;
	map<wxString, wxString> triggers;

private:
	bool isRoot;
	bool hasFilters;
};

#endif // __PROJECTINFO_H__
