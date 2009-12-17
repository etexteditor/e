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

// Ctrl id's
enum {
	CTRL_SEARCH,
	CTRL_ALIST
};

BEGIN_EVENT_TABLE(SnippetList, wxPanel)
	EVT_TEXT(CTRL_SEARCH, SnippetList::OnSearch)
	EVT_TEXT_ENTER(CTRL_SEARCH, SnippetList::OnAction)
	EVT_LISTBOX_DCLICK(CTRL_ALIST, SnippetList::OnAction)
END_EVENT_TABLE()

SnippetList::SnippetList(EditorFrame& services):
	wxPanel(dynamic_cast<wxWindow*>(&services), wxID_ANY),
		m_editorFrame(services),
		m_editorCtrl(services.GetEditorCtrl()),
		m_syntaxHandler(services.GetSyntaxHandler())
{

	// Create ctrls
	m_searchCtrl = new wxTextCtrl(this, CTRL_SEARCH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_listBox = new ActionList(this, CTRL_ALIST, m_bundleStrings);

	UpdateList();

	// Add custom event handler (for up/down key events in the search box)
	m_searchCtrl->Connect(wxEVT_CHAR, wxKeyEventHandler(SnippetList::OnSearchChar), NULL, this);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_searchCtrl, 0, wxEXPAND);
		mainSizer->Add(m_listBox, 1, wxEXPAND);

	SetSizer(mainSizer);
}

void SnippetList::UpdateList() {
	GetCurrentActions();
	
	m_bundleStrings.Empty();
	wxString name, trigger, label;
	const tmAction* action;
	for(unsigned int c = 0; c < m_filteredActions.size(); ++c) {
		/*action = m_filteredActions[c];
		name = action->name;
		trigger = action->trigger;

		if(trigger.empty()) {
			label = name;
		} else if(name.empty()) {
			label = trigger;
		} else {
			label = trigger+wxT(" -> ")+name;
		}*/

		label = m_filteredActions[c]->trigger;

		m_bundleStrings.Add(label);
	}
	m_listBox->SetAllItems();
}

bool SnippetList::ScopeChanged(const deque<const wxString*> scope) {
	return m_previousScope != scope;
	//if(scope.size() != previousScope.size()) return false;
	
}

void SnippetList::GetCurrentActions() {
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
	return !action->trigger.empty() && action->IsSnippet(); // && !action->name.empty();
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
	
	//sort the actions for convenience
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
	// delayed destruction: the panel will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}

void SnippetList::OnSearch(wxCommandEvent& event) {
	UpdateList();
	m_listBox->Find(event.GetString());
}

void SnippetList::OnSearchChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_UP:
		m_listBox->SelectPrev();
		return;
	case WXK_DOWN:
		m_listBox->SelectNext();
		return;
	case WXK_ESCAPE:
		if (event.ShiftDown()) m_editorFrame.CloseSnippetList();
		return;
	}

	// no, we didn't process it
    event.Skip();
}

void SnippetList::OnAction(wxCommandEvent& WXUNUSED(event)) {
	if(m_listBox->GetSelectedCount() != 1) return;

	const int hit = m_listBox->GetSelectedAction();

	if (hit != wxNOT_FOUND && hit < (int)m_filteredActions.size()) {
		//m_editorSymbols->GotoSymbolPos(m_symbols[hit].start);
		//TODO: if they click on the snippet, run it?

		if (wxGetKeyState(WXK_SHIFT)) {
			m_editorFrame.CloseSnippetList();
		} else if (!m_searchCtrl->IsEmpty()) {
			m_searchCtrl->Clear();
		}
	}
}

// --- ActionList --------------------------------------------------------

BEGIN_EVENT_TABLE(SnippetList::ActionList, SearchListBox)
	EVT_LEFT_DOWN(SnippetList::ActionList::OnLeftDown)
END_EVENT_TABLE()

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

int SnippetList::ActionList::GetSelectedAction() const {
	const int sel = GetSelection();
	if (sel == -1) return wxNOT_FOUND;
	else return m_items[sel].id;
}

void SnippetList::ActionList::OnLeftDown(wxMouseEvent& event)
{
    const int item = HitTest(event.GetPosition());
    if ( item != wxNOT_FOUND )
    {
		SetSelection(item);

		// We want to process single clicks like dclick/enter
        wxCommandEvent e(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, GetId());
        e.SetEventObject(this);
        e.SetInt(item);

        (void)GetEventHandler()->ProcessEvent(e);
	}
	else event.Skip();
}
