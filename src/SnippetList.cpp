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

/*******************************************************************************
 * Some general notes about how this works:
 * It shows a list of all of the snippets for the current syntax.
 * As the user types, it filters the list based on which snippet's triggers match the word the user typed.
 * 
 * To get the list of snippets, it has to get the current 'scope' from the EditorCtrl.
 * Then it can get a list of all of the snippets in the scope.
 * The snippets are filtered so empty ones / duplicates are removed.
 * All of these are stored in instance variables so they only need to be reloaded when the syntax changes.
 *
 * To function, the UpdateList and UpdateSearchText methods have to be called from the
 * EditorFrame and EditorCtrl classes whenever the syntax is changed or the text/cursor
 * of the document change.
 *
 ******************************************************************************/

#include "SnippetList.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"
#include "SnippetHandler.h"
#include "plistHandler.h"
#include "tm_syntaxhandler.h"

#include <algorithm>

#ifndef WX_PRECOMP
    #include <wx/sizer.h>
    #include <wx/dc.h>
#endif

using namespace std;

SnippetList::SnippetList(EditorFrame& services):
	wxPanel(dynamic_cast<wxWindow*>(&services), wxID_ANY),
		m_editorFrame(services),
		m_editorCtrl(services.GetEditorCtrl()),
		m_syntaxHandler(services.GetSyntaxHandler())
{
	m_listBox = new ActionList(this, 0, m_bundleStrings);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(m_listBox, 1, wxEXPAND);
	SetSizer(mainSizer);
}

void SnippetList::UpdateSearchText() {
	wxString word = m_editorCtrl->GetCurrentWord();
	UpdateList();
	m_listBox->Find(word);
}

void SnippetList::UpdateList() {
	GetCurrentActions();
	
	m_bundleStrings.Empty();
	wxString name, trigger, label;
	for(unsigned int c = 0; c < m_filteredActions.size(); ++c) {

		label = m_filteredActions[c]->trigger;

		m_bundleStrings.Add(label);
	}
	m_listBox->SetAllItems();
}

bool SnippetList::ScopeChanged(const deque<const wxString*> scope) {
	return m_previousScope.empty() || m_previousScope != scope;
}

void SnippetList::GetCurrentActions() {
	if(m_editorFrame.GetEditorCtrl() != NULL)
		m_editorCtrl = m_editorFrame.GetEditorCtrl();

	deque<const wxString*> scope = m_editorCtrl->GetScope();
	if(ScopeChanged(scope)) {
		//copy the new scope into the instance variable
		m_previousScope.clear();
		deque<const wxString*>::iterator scopeIterator;
		for(scopeIterator = scope.begin(); scopeIterator != scope.end(); ++scopeIterator) {
			m_previousScope.push_back(*scopeIterator);
		}

		//get an unfiltered list of the actions
		m_previousActions.clear();
		m_syntaxHandler.GetAllActions(scope, m_previousActions);

		//remove duplicate triggers/empty triggers/non-snippets
		m_filteredActions.clear();
		FilterActions(m_previousActions, m_filteredActions);
	}
}

bool SnippetList::FilterAction(const tmAction* action) {
	return !action->trigger.empty(); // && action->IsSnippet(); // && !action->name.empty();
}

bool sortComparator(const tmAction* a, const tmAction* b) {
	return a->trigger.Lower() < b->trigger.Lower();
}

void SnippetList::FilterActions(vector<const tmAction*>& actions, vector<const tmAction*>& result) {
	//remove any actions that are not snippets or dont have a trigger text
	vector<const tmAction*> tmp;
	for(unsigned int c = 0; c < actions.size(); ++c) {
		if(FilterAction(actions[c])) {
			tmp.push_back(actions[c]);
		}
	}
	
	//sort the actions for convenience and to make removing duplicates easier and faster
	sort(tmp.begin(), tmp.end(), sortComparator);

	//remove duplicates
	if(tmp.size() > 1) {
		const tmAction* previous = tmp[0];
		result.push_back(previous);
		for(unsigned int c = 1; c < tmp.size(); c++) {
			if(previous->trigger != tmp[c]->trigger) {
				previous = tmp[c];
				result.push_back(previous);
			}
		}
	} else {
		if(tmp.size() == 1) {
			result.push_back(tmp[0]);
		}
	}
}

bool SnippetList::Destroy() {
	// delayed destruction: the panel will be deleted during the next idle loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}

// --- ActionList --------------------------------------------------------

SnippetList::ActionList::ActionList(wxWindow* parent, wxWindowID id, const wxArrayString& actions):
	SearchListBox(parent, id), 
	m_actions(actions)
{
	SetAllItems();
}

void SnippetList::ActionList::SetAllItems() {
	m_items.clear();

	Freeze();

	if (m_searchText.empty()) {
		m_items.resize(m_actions.size());
		for (unsigned int i = 0; i < m_actions.size(); ++i) {
			m_items[i].id = i;
			m_items[i].action = &m_actions[i];
		}

		SetItemCount(m_items.size());
	}
	else {
		Find(m_searchText, false);
	}

	SetSelection(-1);
	RefreshAll();
	Thaw();
}

void SnippetList::ActionList::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const {
	const bool isCurrent = IsCurrent(n);

	if (isCurrent) dc.SetTextForeground(m_hlTextColor);
	else dc.SetTextForeground(m_textColor);

	const aItem& ai = m_items[n];
	const vector<unsigned int>& hl = ai.hlChars;
	const wxString& name = *ai.action;

	// Draw action name
	DrawItemText(dc, rect, name, hl, isCurrent);
}

void SnippetList::ActionList::Find(const wxString& searchtext, bool refresh) {
	m_searchText = searchtext; // cache for later updates

	if (searchtext.empty()) {
		SetAllItems();
		return;
	}

	// Convert to upper case for case insensitive search
	const wxString text = searchtext.Upper();

	m_items.clear();
	vector<unsigned int> hlChars;
	for (unsigned int i = 0; i < m_actions.GetCount(); ++i) {
		const wxString& name = m_actions[i];
		unsigned int charpos = 0;
		wxChar c = text[charpos];
		hlChars.clear();

		for (unsigned int textpos = 0; textpos < name.size(); ++textpos) {
			if ((wxChar)wxToupper(name[textpos]) == c) {
				hlChars.push_back(textpos);
				++charpos;
				if (charpos == text.size()) {
					// All chars found.
					m_items.push_back(aItem(i, &m_actions[i], hlChars));
					break;
				}
				else c = text[charpos];
			}
		}
	}

	sort(m_items.begin(), m_items.end());

	Freeze();
	SetItemCount(m_items.size());
	if (refresh) {
		if (m_items.empty()) SetSelection(-1); // deselect
		else SetSelection(0);

		RefreshAll();
	}
	Thaw();
}