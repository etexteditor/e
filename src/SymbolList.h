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

#ifndef __SYMBOLLIST_H__
#define __SYMBOLLIST_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "SearchListBox.h"
#include "IEditorSymbols.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class IFrameSymbolService;

class SymbolList : public wxPanel {
public:
	SymbolList(wxWindow& parent, IFrameSymbolService& services);

	bool Destroy();

private:
	void OnIdle(wxIdleEvent& event);
	void OnSearch(wxCommandEvent& event);
	void OnAction(wxCommandEvent& event);
	void OnSearchChar(wxKeyEvent& event);
	DECLARE_EVENT_TABLE();

	class ActionList : public SearchListBox {
	public:
		ActionList(wxWindow* parent, wxWindowID id, const wxArrayString& actions);
		void SetAllItems();

		void Find(const wxString& text, bool refresh=true);
		int GetSelectedAction() const;

	private:
		void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;

		void OnLeftDown(wxMouseEvent& event);
		DECLARE_EVENT_TABLE();

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
	IFrameSymbolService& m_parentFrame;
	wxTextCtrl* m_searchCtrl;
	ActionList* m_listBox;

	// Editor state
	IEditorSymbols* m_editorSymbols;
	EditorChangeState m_editorChangeState;

	vector<SymbolRef> m_symbols;
	wxArrayString m_symbolStrings;
};

#endif // __SYMBOLLIST_H__
