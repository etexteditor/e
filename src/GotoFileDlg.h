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

#include <wx/dir.h>
#include "SearchListBox.h"

class IProjectManager;
class cxProjectInfo;

class GotoFileDlg : public wxDialog {
public:
	GotoFileDlg(wxWindow *parent, IProjectManager& project);
	~GotoFileDlg();

	const wxString& GetSelection() const {return m_cmdList->GetSelectedAction()->path;};
	const wxString GetTrigger() const {return m_searchCtrl->GetValue();};

	class FileEntry {
	public:
		FileEntry() {};
		FileEntry(const wxString& dirpath, const wxString& filename);
		void SetPath(const wxString& path);
		void Clear();

		wxString name;
		wxString nameLower;
		wxString path;
	};

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

	class ActionList : public SearchListBox {
	public:
		ActionList(wxWindow* parent, wxWindowID id, const vector<FileEntry*>& actions);

		void Find(const wxString& text, const map<wxString,wxString>& triggers);
		const FileEntry* GetSelectedAction();

		void UpdateList(bool reloadAll = false);

	private:
		void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
		int FindPath(const wxString& path) const;
		void AddActionIfMatching(const wxString& searchtext, FileEntry* action);

		class aItem {
		public:
			aItem() : action(NULL), rank(0) {};
			aItem(const FileEntry* a, const vector<unsigned int>& hl);
			bool operator<(const aItem& ai) const {
				if (rank < ai.rank) return true;
				else if (rank > ai.rank) return false;
				else if (action && ai.action) return action->name < ai.action->name;
				else return false;
			}
			void swap(aItem& ai);
			const FileEntry* action;
			vector<unsigned int> hlChars;
			unsigned int rank;
		};

		void iter_swap(vector<aItem>::iterator a, vector<aItem>::iterator b) {
			a->swap(*b);
		};

		const vector<FileEntry*>& m_actions;
		vector<aItem> m_items;
		wxString m_searchText;

		FileEntry m_tempEntry;
		unsigned int m_actionCount;
	};

	class DirState {
	public:
		wxDir dir;
		cxProjectInfo* info;
		const cxProjectInfo* filter;
		wxString prefix;
		wxString nextDirName;
	};

	// Member variables
	IProjectManager& m_project;
	vector<FileEntry*> m_files;
	bool m_isDone;

	// Dir traversing state
	bool m_filesLoaded;
	vector<DirState*> m_dirStack;
	vector<cxProjectInfo*> m_filters;

	// Ctrls
	wxTextCtrl* m_searchCtrl;
	ActionList* m_cmdList;
	wxStaticText* m_pathStatic;
};

#endif // __GOTOFILEDLG_H__
