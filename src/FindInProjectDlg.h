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

#include <map>
#include <vector>
using namespace std;

// pre-definitions
class EditorFrame;
class MMapBuffer;
class ProjectInfoHandler;
class wxFileName;
class ProjectInfoHandler;

class FindInProjectDlg : public wxDialog {
public:
	FindInProjectDlg(EditorFrame& parentFrame, const ProjectInfoHandler& projectPane);
	~FindInProjectDlg();
 
private:

	class SearchThread : public wxThread {
	public:
		SearchThread();
		virtual void* Entry();
		void DeleteThread();

		void StartSearch(const wxString& path, const wxString& pattern, bool matchCase, bool regex);
		void CancelSearch();
		bool IsSearching() const {return m_isSearching;};
		bool LastError() const {return m_lastError;};

		bool GetCurrentPath(wxString& currentPath);
		bool UpdateOutput(wxString& output);

	private:
		struct FileMatch {
			unsigned int line;
			unsigned int column;
			wxFileOffset start;
			wxFileOffset end;
		};

		struct SearchInfo {
			wxString pattern;
			wxString patternUpper;
			wxCharBuffer UTF8buffer;
			wxCharBuffer UTF8bufferUpper;
			size_t byte_len;
			char charmap[256];
			char lastChar;
			bool matchCase;
			pcre* regex;
			wxString output;
		};

		void SearchDir(const wxString& path, const SearchInfo& si, ProjectInfoHandler& infoHandler);
		void DoSearch(const MMapBuffer& buf, const SearchInfo& si, vector<FileMatch>& matches) const;
		void WriteResult(const MMapBuffer& buf, const wxFileName& filepath, vector<FileMatch>& matches);
		bool PrepareSearchInfo(SearchInfo& si, const wxString& pattern, bool matchCase, bool regex);

		// Member variables
		bool m_isSearching;
		bool m_isWaiting;
		bool m_stopSearch;
		wxString m_path;
		wxString m_pattern;
		bool m_matchCase;
		bool m_regex;
		bool m_lastError;
		wxString m_currentPath;
		wxString m_output;
		wxCriticalSection m_outputCrit;

		wxMutex m_condMutex;
		wxCondition m_startSearchCond;
	};

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
