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

#ifndef __BUNDLELIST_H__
#define __BUNDLELIST_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/panel.h>
    #include <wx/textctrl.h>
#endif

#include "SearchListBox.h"
#include "IEditorSymbols.h"
#include "EditorFrame.h"
#include "tmAction.h"

#include <vector>

class IFrameSymbolService;

class BundleList : public wxPanel {
public:
	BundleList(EditorFrame& services, bool keepOpen=true);
	bool Destroy();
	void UpdateList();
	void GetCurrentActions(std::vector<const tmAction*>& actions);
	bool FilterAction(const tmAction*);
	void FilterActions(std::vector<const tmAction*>& actions, std::vector<const tmAction*>& result);

private:
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
			aItem(unsigned int id, const wxString* a, const std::vector<unsigned int>& hl)
				: id(id), action(a), hlChars(hl), rank(SearchListBox::CalcRank(hl)) {};
			bool operator<(const aItem& ai) const {
				if (rank < ai.rank) return true;
				else if (rank > ai.rank) return false;
				else if (action && ai.action) return *action < *ai.action;
				else return false;
			}
			unsigned int id;
			const wxString* action;
			std::vector<unsigned int> hlChars;
			unsigned int rank;
		};

		const wxArrayString& m_actions;
		std::vector<aItem> m_items;
		wxString m_searchText;
	};

	// Member variables
	IFrameSymbolService& m_editorFrame;
	EditorCtrl* m_editorCtrl;
	TmSyntaxHandler& m_syntaxHandler;

	// Widgets
	wxTextCtrl* m_searchCtrl;
	ActionList* m_listBox;

	bool m_keepOpen;
	wxArrayString m_bundleStrings;
};

#endif // __BUNDLELIST_H__
