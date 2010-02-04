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

#ifndef __SNIPPETLIST_H__
#define __SNIPPETLIST_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "SearchListBox.h"
#include "EditorFrame.h"
#include "tmAction.h"

#include <vector>
#include <deque>

class SnippetList : public wxPanel {
public:
	SnippetList(EditorFrame& services);
	bool Destroy();
	void UpdateList();
	void UpdateSearchText();
	bool ScopeChanged(const deque<const wxString*> scope);
	void GetCurrentActions();
	bool FilterAction(const tmAction*);
	void FilterActions(vector<const tmAction*>& actions, vector<const tmAction*>& result);

private:

	class ActionList : public SearchListBox {
	public:
		ActionList(wxWindow* parent, wxWindowID id, const wxArrayString& actions);
		void SetAllItems();
		void Find(const wxString& text, bool refresh=true);

	private:
		void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;

		class aItem {
		public:
			aItem() : id(0), action(NULL), rank(0) {};
			aItem(unsigned int id, const wxString* a, const vector<unsigned int>& hl)
				: id(id), action(a), hlChars(hl), rank(SearchListBox::CalcRank(hl)) {};
			bool operator<(const aItem& ai) const {
				if (rank < ai.rank) return true;
				else if (rank > ai.rank) return false;
				else if (action && ai.action) return *action < *ai.action;
				else return false;
			}
			unsigned int id;
			const wxString* action;
			vector<unsigned int> hlChars;
			unsigned int rank;
		};

		const wxArrayString& m_actions;
		vector<aItem> m_items;
		wxString m_searchText;
	};

	// Member variables
	EditorFrame& m_editorFrame;
	EditorCtrl* m_editorCtrl;
	TmSyntaxHandler& m_syntaxHandler;

	// Widgets
	ActionList* m_listBox;

	vector<const tmAction*> m_previousActions;
	vector<const tmAction*> m_filteredActions;
	deque<const wxString*> m_previousScope;

	wxArrayString m_bundleStrings;
};

#endif // __SNIPPETLIST_H__
