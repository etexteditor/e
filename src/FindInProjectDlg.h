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

#ifndef _FINDINPROJECTDLG_H_
#define _FINDINPROJECTDLG_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#ifdef __WXMSW__
#include "IEHtmlWin.h"
#endif
#include "IHtmlWnd.h"
#include "pcre.h"

// pre-definitions
class EditorFrame;
class MMapBuffer;
class ProjectInfoHandler;
class wxFileName;
class ProjectInfoHandler;
class SearchThread;

class FindInProjectDlg : public wxDialog {
public:
	FindInProjectDlg(EditorFrame& parentFrame, const ProjectInfoHandler& projectPane);
	~FindInProjectDlg();

	void SetPattern(const wxString& pattern);
 
private:
	// Event handlers
	void OnSearch(wxCommandEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnBeforeLoad(IHtmlWndBeforeLoadEvent& event);
	DECLARE_EVENT_TABLE();

	// member ctrl's
	wxTextCtrl* m_searchCtrl;
	wxButton* m_searchButton;
	wxCheckBox* m_caseCheck;
	wxCheckBox* m_regexCheck;
	wxStaticText* m_pathStatic;
#ifdef __WXMSW__
	wxIEHtmlWin* m_browser;
#else
	IHtmlWnd* m_browser;
#endif

	// member variables
	EditorFrame& m_parentFrame;
	const ProjectInfoHandler& m_projectPane;
	SearchThread* m_searchThread;
	wxString m_output;
	bool m_inSearch;
};

#endif /* _FINDINPROJECTDLG_H_ */
