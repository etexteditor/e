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

#include "SymbolList.h"
#include "EditorCtrl.h"
#include "EditorFrame.h"

// Ctrl id's
enum {
	CTRL_SEARCH,
	CTRL_ALIST
};

BEGIN_EVENT_TABLE(SymbolList, wxPanel)
	EVT_IDLE(SymbolList::OnIdle)
	EVT_TEXT(CTRL_SEARCH, SymbolList::OnSearch)
	EVT_TEXT_ENTER(CTRL_SEARCH, SymbolList::OnAction)
	EVT_LISTBOX_DCLICK(CTRL_ALIST, SymbolList::OnAction)
END_EVENT_TABLE()

SymbolList::SymbolList(EditorFrame& parent)
: wxPanel((wxWindow*)&parent, wxID_ANY),
  m_parentFrame(parent), m_editorCtrl(NULL) {
	// Create ctrls
	m_searchCtrl = new wxTextCtrl(this, CTRL_SEARCH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_listBox = new ActionList(this, CTRL_ALIST, m_symbolStrings);

	// Add custom event handler (for up/down key events in the search box)
	m_searchCtrl->Connect(wxEVT_CHAR, wxKeyEventHandler(SymbolList::OnSearchChar), NULL, this);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_searchCtrl, 0, wxEXPAND);
		mainSizer->Add(m_listBox, 1, wxEXPAND);

	SetSizer(mainSizer);
}

bool SymbolList::Destroy() {
	// delayed destruction: the panel will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}

void SymbolList::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	EditorCtrl* editorCtrl = m_parentFrame.GetEditorCtrl();
	if (!editorCtrl) return;
	const int id = editorCtrl->GetId();

	// In rare cases a new editorCtrl may get same address as
	// a previous one, so we also track the window id.
	const bool newEditorCtrl = (editorCtrl != m_editorCtrl || id != m_editorCtrlId);

	if (newEditorCtrl) m_changeToken = 0; // reset token
	const unsigned int currentChangeToken = editorCtrl->GetChangeToken();

	// Only update if the editorCtrl has changed
	if (newEditorCtrl || m_changeToken != currentChangeToken) { // || m_pos != editorCtrl->GetPos()) {
		m_editorCtrl = editorCtrl;
		m_editorCtrlId = id;

		// Reload symbols
		m_symbols.clear();
		m_symbolStrings.Empty();
		const int res = editorCtrl->GetSymbols(m_symbols);
		if (res == 1) {
			// reload symbol strings
			for (vector<SymbolRef>::const_iterator p = m_symbols.begin(); p != m_symbols.end(); ++p) {
				const SymbolRef& sr = *p;
				m_symbolStrings.Add(editorCtrl->GetSymbolString(sr));
			}
			m_listBox->SetAllItems();
		}
		else if (res == 2) { // DEBUG: double implementation to view path in crash dump (remove when bug is solved)
			// reload symbol strings
			for (vector<SymbolRef>::const_iterator p = m_symbols.begin(); p != m_symbols.end(); ++p) {
				const SymbolRef& sr = *p;
				m_symbolStrings.Add(editorCtrl->GetSymbolString(sr));
			}
			m_listBox->SetAllItems();
		}
		else {
			m_listBox->SetAllItems(); // clear list
			return;
		}

		// Get editor state
		m_pos = editorCtrl->GetPos();
		m_changeToken = currentChangeToken;

		// Keep scrollpos so we can stay at the same pos
		const unsigned int scrollPos = m_listBox->GetScrollPos(wxVERTICAL);

		// Set current symbol
		if (!m_symbols.empty()) {
			/*// Set new symbols
			bool currentSet = false;
			unsigned int id = 0;
			for (vector<Styler_Syntax::SymbolRef>::const_iterator p = m_symbols.begin(); p != m_symbols.end(); ++p) {
				// Select current
				if (!currentSet && m_pos < p->start) {
					if (p != m_symbols.begin()) {
						SetSelection(id-1, true);
					}
					currentSet = true;
				}

				++id;
			}

			// Current symbol may be the last and therefore not checked above
			if (!currentSet && m_pos >= m_symbols.back().start) {
				SetSelection(id-1, true);
			}*/

			// Keep same position
			if (!newEditorCtrl) {
				m_listBox->SetScrollPos(wxVERTICAL, scrollPos);
			}
		}
	}
}

void SymbolList::OnSearch(wxCommandEvent& event) {
	m_listBox->Find(event.GetString());
}

void SymbolList::OnSearchChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_UP:
		m_listBox->SelectPrev();
		return;
	case WXK_DOWN:
		m_listBox->SelectNext();
		return;
	case WXK_ESCAPE:
		if (event.ShiftDown()) m_parentFrame.CloseSymbolList();
		if (m_editorCtrl) m_editorCtrl->SetFocus();
		return;
	}

	// no, we didn't process it
    event.Skip();
}

void SymbolList::OnAction(wxCommandEvent& WXUNUSED(event)) {
	if (!m_editorCtrl) return;

	if(m_listBox->GetSelectedCount() == 1) {
		const int hit = m_listBox->GetSelectedAction();

		if (hit != wxNOT_FOUND && hit < (int)m_symbols.size()) {
			// Go to symbol
			m_editorCtrl->SetPos(m_symbols[hit].start);
			m_editorCtrl->MakeCaretVisibleCenter();
			m_editorCtrl->ReDraw();
			m_editorCtrl->SetFocus();

			// Close symbollist if user holds down shift
			if (wxGetKeyState(WXK_SHIFT)) {
				m_parentFrame.CloseSymbolList();
			}
			else if (!m_searchCtrl->IsEmpty()) {
				m_searchCtrl->Clear();
			}
		}
	}
}

// --- ActionList --------------------------------------------------------

BEGIN_EVENT_TABLE(SymbolList::ActionList, SearchListBox)
	EVT_LEFT_DOWN(SymbolList::ActionList::OnLeftDown)
END_EVENT_TABLE()

SymbolList::ActionList::ActionList(wxWindow* parent, wxWindowID id, const wxArrayString& actions)
: SearchListBox(parent, id), m_actions(actions) {

	SetAllItems();
}

void SymbolList::ActionList::SetAllItems() {
	m_items.clear();

	Freeze();

	if (m_searchText.empty()) {
		m_items.resize(m_actions.size());
		for (unsigned int i = 0; i < m_actions.size(); ++i) {
			m_items[i].id = i;
			m_items[i].action = &m_actions[i];
		}

		//sort(m_items.begin(), m_items.end());
		SetItemCount(m_items.size());
	}
	else {
		Find(m_searchText, false);
	}

	SetSelection(-1);
	RefreshAll();
	Thaw();
}

void SymbolList::ActionList::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const {
	const bool isCurrent = IsCurrent(n);

	if (isCurrent) dc.SetTextForeground(m_hlTextColor);
	else dc.SetTextForeground(m_textColor);

	const aItem& ai = m_items[n];
	const vector<unsigned int>& hl = ai.hlChars;
	const wxString& name = *ai.action;

	// Draw action name
	DrawItemText(dc, rect, name, hl, isCurrent);
}

void SymbolList::ActionList::Find(const wxString& searchtext, bool refresh) {
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

int SymbolList::ActionList::GetSelectedAction() const {
	const int sel = GetSelection();
	if (sel == -1) return wxNOT_FOUND;
	else return m_items[sel].id;
}

void SymbolList::ActionList::OnLeftDown(wxMouseEvent& event)
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
