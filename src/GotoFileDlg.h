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

#ifndef __GOTOFILEDLG_H__
#define __GOTOFILEDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <vector>

class ProjectInfoHandler;
class cxProjectInfo;
class FileActionList;
class FileEntry;
struct DirState;

class GotoFileDlg : public wxDialog {
public:
	GotoFileDlg(wxWindow *parent, ProjectInfoHandler& project);
	~GotoFileDlg();

	const wxString& GetSelection() const;
	const wxString GetTrigger() const;

private:
	void BuildFileList(const wxString& path);
	void UpdateStatusbar();

	// Event handlers
	void OnSearch(wxCommandEvent& event);
	void OnAction(wxCommandEvent& event);
	void OnListSelection(wxCommandEvent& event);
	void OnSearchChar(wxKeyEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	ProjectInfoHandler& m_project;
	std::vector<FileEntry*> m_files;
	bool m_isDone;

	// Dir traversing state
	bool m_filesLoaded;
	std::vector<DirState*> m_dirStack;
	std::vector<cxProjectInfo*> m_filters;

	// Ctrls
	wxTextCtrl* m_searchCtrl;
	FileActionList* m_cmdList;
	wxStaticText* m_pathStatic;
};

#endif // __GOTOFILEDLG_H__
