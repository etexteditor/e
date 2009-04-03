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

#ifndef __FINDCMDDLG_H__
#define __FINDCMDDLG_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "eApp.h"
#include "SearchListBox.h"
#include "tm_syntaxhandler.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <deque>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class FindCmdDlg : public wxDialog {
public:
	FindCmdDlg(wxWindow *parent, const deque<const wxString*>& scope);
	~FindCmdDlg();
	const tmAction* GetSelection() {return m_cmdList->GetSelectedAction();};

private:
	// Event handlers
	void OnSearch(wxCommandEvent& event);
	void OnAction(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	class ActionList : public SearchListBox {
	public:
		ActionList(wxWindow* parent, wxWindowID id, const vector<const tmAction*>& actions);
		void Find(const wxString& text);
		const tmAction* GetSelectedAction();

	private:
		void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
		void SetAllItems();

		class aItem {
		public:
			aItem() : action(NULL), rank(0) {};
			aItem(const tmAction* a, const vector<unsigned int>& hl);
			bool operator<(const aItem& ai) const {
				if (rank < ai.rank) return true;
				else if (rank > ai.rank) return false;
				else if (action && ai.action) return action->name < ai.action->name;
				else return false;
			}
			const tmAction* action;
			vector<unsigned int> hlChars;
			unsigned int rank;
		};

		const vector<const tmAction*>& m_actions;
		vector<aItem> m_items;
		wxFont m_unifont;
	};

	class SearchEvtHandler : public wxEvtHandler {
	public:
		SearchEvtHandler(FindCmdDlg& parent, ActionList& alist)
			: m_parent(parent), m_actionList(alist) {};
	private:
		void OnChar(wxKeyEvent& event);
		DECLARE_EVENT_TABLE();

		FindCmdDlg& m_parent;
		ActionList& m_actionList;
	};

	// Member variables
	const deque<const wxString*>& m_scope;
	vector<const tmAction*> m_actions;
	wxTextCtrl* m_searchCtrl;
	ActionList* m_cmdList;
};

#endif // __FINDCMDDLG_H_
