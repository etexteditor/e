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

#include "BundleList.h"
#include "IFrameSymbolService.h"
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

BEGIN_EVENT_TABLE(BundleList, wxPanel)
	EVT_TEXT(CTRL_SEARCH, BundleList::OnSearch)
	EVT_TEXT_ENTER(CTRL_SEARCH, BundleList::OnAction)
	EVT_LISTBOX_DCLICK(CTRL_ALIST, BundleList::OnAction)
END_EVENT_TABLE()

	void GetCurrentActions(std::vector<const tmAction*>& actions);
	void FilterActions(std::vector<const tmAction*>& actions, std::vector<const tmAction*>& result);

BundleList::BundleList(EditorFrame& services, bool keepOpen):
	wxPanel(dynamic_cast<wxWindow*>(&services), wxID_ANY),
		m_editorFrame(services),
		m_editorCtrl(services.GetEditorCtrl()),
		m_syntaxHandler(services.GetSyntaxHandler()),
		m_keepOpen(keepOpen)
{
	// Create ctrls
	m_searchCtrl = new wxTextCtrl(this, CTRL_SEARCH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_listBox = new ActionList(this, CTRL_ALIST, m_bundleStrings);

	UpdateList();

	/*
	PListHandler& plist = m_syntaxHandler.GetPListHandler();
	int bundleId = 10;
	const PListDict infoDict = plist.GetBundleInfo(bundleId);
	const wxString bundleName = infoDict.wxGetString("name");
	const vector<unsigned int> snippets =  plist.GetList(BUNDLE_SNIPPET, bundleId);
	
	for (unsigned int n = 0; n < snippets.size(); ++n) {
		const unsigned int snippetId = snippets[n];

		const PListDict snipDict = plist.Get(BUNDLE_SNIPPET, bundleId, snippetId);
		const wxString snipName = snipDict.wxGetString("name");
		const wxString uuid = snipDict.wxGetString("uuid");
		const wxString trigger = snipDict.wxGetString("tabTrigger");

		wxString label = trigger+wxT(" -> ")+snipName;

		printf("%s\n", snipName);
		int i = 0;
		while (i < snipDict.GetSize()) {
			const char* key = snipDict.GetKey(i);
			const char* value = snipDict.GetString(key);
			printf("%s=%s\n", key, value);
			++i;
		}

		m_bundleStrings.Add(label);
	}*/

	// Add custom event handler (for up/down key events in the search box)
	m_searchCtrl->Connect(wxEVT_CHAR, wxKeyEventHandler(BundleList::OnSearchChar), NULL, this);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_searchCtrl, 0, wxEXPAND);
		mainSizer->Add(m_listBox, 1, wxEXPAND);

	SetSizer(mainSizer);
}

void BundleList::UpdateList() {
	std::vector<const tmAction*> actionList, filteredActions;
	GetCurrentActions(actionList);
	FilterActions(actionList, filteredActions);
	
	m_bundleStrings.Empty();
	wxString name, trigger, label;
	const tmAction* action;
	for(unsigned int c = 0; c < filteredActions.size(); ++c) {
		/*action = filteredActions[c];
		name = action->name;
		trigger = action->trigger;

		if(trigger.empty()) {
			label = name;
		} else if(name.empty()) {
			label = trigger;
		} else {
			label = trigger+wxT(" -> ")+name;
		}*/

		label = filteredActions[c]->trigger;

		m_bundleStrings.Add(label);
	}
	m_listBox->SetAllItems();
}

void BundleList::GetCurrentActions(std::vector<const tmAction*>& actions) {
	m_editorCtrl->GetAllActions(actions);
}

bool BundleList::FilterAction(const tmAction* action) {
	return !action->trigger.empty() && action->IsSnippet(); // && !action->name.empty();
}

bool sortComparator(const tmAction* a, const tmAction* b) {
	return a->trigger.Lower() < b->trigger.Lower();
}

void BundleList::FilterActions(std::vector<const tmAction*>& actions, std::vector<const tmAction*>& result) {
	//remove any actions that are not snippets or dont have a trigger text
	for(unsigned int c = 0; c < actions.size(); ++c) {
		if(FilterAction(actions[c])) {
			result.push_back(actions[c]);
		}
	}
	
	//sort the actions for convenience
	sort(result.begin(), result.end(), sortComparator);

	//remove duplicates

}

bool BundleList::Destroy() {
	// delayed destruction: the panel will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}
#if 0
void SymbolList::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	EditorChangeType newStatus;
	IEditorSymbols* editorSymbols = dynamic_cast<IEditorSymbols*>(m_parentFrame.GetEditorAndChangeType(this->m_editorChangeState, newStatus));

	// If we lost the editor, or there was no change, then do nothing.
	if(!editorSymbols || (newStatus == ECT_NO_CHANGE)) return;

	bool newEditor = (newStatus == ECT_NEW_EDITOR);
	this->m_editorSymbols = editorSymbols;

	// Reload symbols
	m_symbols.clear();
	m_symbolStrings.Empty();
	const int res = m_editorSymbols->GetSymbols(m_symbols);
	if (res == 1) {
		// reload symbol strings
		for (vector<SymbolRef>::const_iterator p = m_symbols.begin(); p != m_symbols.end(); ++p) {
			const SymbolRef& sr = *p;
			m_symbolStrings.Add(m_editorSymbols->GetSymbolString(sr));
		}
		m_listBox->SetAllItems();
	}
	else if (res == 2) { // DEBUG: double implementation to view path in crash dump (remove when bug is solved)
		// reload symbol strings
		for (vector<SymbolRef>::const_iterator p = m_symbols.begin(); p != m_symbols.end(); ++p) {
			const SymbolRef& sr = *p;
			m_symbolStrings.Add(m_editorSymbols->GetSymbolString(sr));
		}
		m_listBox->SetAllItems();
	}
	else {
		m_listBox->SetAllItems(); // clear list
		return;
	}

	// Save the change state for next time around, reseting the change token if we have a new editor.
	this->m_editorChangeState = editorSymbols->GetChangeState();
	if (newEditor) this->m_editorChangeState.changeToken = 0;

	// Keep scrollpos so we can stay at the same pos
	const unsigned int scrollPos = m_listBox->GetScrollPos(wxVERTICAL);

	// Set current symbol
	if (!m_symbols.empty()) {
		if (!newEditor) 	m_listBox->SetScrollPos(wxVERTICAL, scrollPos);
	}
}
#endif

void BundleList::OnSearch(wxCommandEvent& event) {
	m_listBox->Find(event.GetString());
}

void BundleList::OnSearchChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_UP:
		m_listBox->SelectPrev();
		return;
	case WXK_DOWN:
		m_listBox->SelectNext();
		return;
	case WXK_ESCAPE:
		//if (event.ShiftDown()) m_parentFrame.CloseSymbolList();
		//if (m_editorSymbols) m_parentFrame.FocusEditor(); //m_editorCtrl->SetFocus();
		return;
	}

	// no, we didn't process it
    event.Skip();
}

void BundleList::OnAction(wxCommandEvent& WXUNUSED(event)) {
	if(m_listBox->GetSelectedCount() != 1) return;

	/*
	const int hit = m_listBox->GetSelectedAction();

	if (hit != wxNOT_FOUND && hit < (int)m_symbols.size()) {
		// Go to symbol
		m_editorSymbols->GotoSymbolPos(m_symbols[hit].start);

		// Close symbollist if user holds down shift or "m_keepOpen" is false.
		if (!m_keepOpen || wxGetKeyState(WXK_SHIFT)) {
			m_parentFrame.CloseSymbolList();
		}
		else if (!m_searchCtrl->IsEmpty()) {
			m_searchCtrl->Clear();
		}
	}*/
}

// --- ActionList --------------------------------------------------------

BEGIN_EVENT_TABLE(BundleList::ActionList, SearchListBox)
	EVT_LEFT_DOWN(BundleList::ActionList::OnLeftDown)
END_EVENT_TABLE()

BundleList::ActionList::ActionList(wxWindow* parent, wxWindowID id, const wxArrayString& actions):
	SearchListBox(parent, id), 
	m_actions(actions)
{
	SetAllItems();
}

void BundleList::ActionList::SetAllItems() {
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

void BundleList::ActionList::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const {
	const bool isCurrent = IsCurrent(n);

	if (isCurrent) dc.SetTextForeground(m_hlTextColor);
	else dc.SetTextForeground(m_textColor);

	const aItem& ai = m_items[n];
	const vector<unsigned int>& hl = ai.hlChars;
	const wxString& name = *ai.action;

	// Draw action name
	DrawItemText(dc, rect, name, hl, isCurrent);
}

void BundleList::ActionList::Find(const wxString& searchtext, bool refresh) {
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

int BundleList::ActionList::GetSelectedAction() const {
	const int sel = GetSelection();
	if (sel == -1) return wxNOT_FOUND;
	else return m_items[sel].id;
}

void BundleList::ActionList::OnLeftDown(wxMouseEvent& event)
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
