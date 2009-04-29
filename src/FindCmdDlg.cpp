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

#include "FindCmdDlg.h"
#include <algorithm>
#include "eApp.h"

// Ctrl id's
enum {
	CTRL_SEARCH,
	CTRL_ALIST
};

BEGIN_EVENT_TABLE(FindCmdDlg, wxDialog)
	EVT_TEXT(CTRL_SEARCH, FindCmdDlg::OnSearch)
	EVT_TEXT_ENTER(CTRL_SEARCH, FindCmdDlg::OnAction)
	EVT_LISTBOX_DCLICK(CTRL_ALIST, FindCmdDlg::OnAction)
END_EVENT_TABLE()

FindCmdDlg::FindCmdDlg(wxWindow *parent, const deque<const wxString*>& scope)
:  wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER), m_scope(scope) {

	SetTitle (_("Select Bundle Item"));

	// Get list of all commands available from current scope
	((eApp*)wxTheApp)->GetSyntaxHandler().GetAllActions(m_scope, m_actions);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	// Search Box
	m_searchCtrl = new wxTextCtrl(this, CTRL_SEARCH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	mainSizer->Add(m_searchCtrl, 0, wxEXPAND);

	// Command List
	m_cmdList = new ActionList(this, CTRL_ALIST, m_actions);
	mainSizer->Add(m_cmdList, 1, wxEXPAND);

	// Set new evtHandler to intercept up/down & escape
	m_searchCtrl->PushEventHandler(new SearchEvtHandler(*this, *m_cmdList));

	SetSizer(mainSizer);
	SetSize(400, 500);
	Centre();
}

FindCmdDlg::~FindCmdDlg() {
	m_searchCtrl->PopEventHandler(true);
}

void FindCmdDlg::OnSearch(wxCommandEvent& event) {
	m_cmdList->Find(event.GetString());
}

void FindCmdDlg::OnAction(wxCommandEvent& WXUNUSED(event)) {
	if(m_cmdList->GetSelectedCount() == 1) EndModal(wxID_OK);
}

// ---  SearchEvtHandler --------------------------------------------------------

BEGIN_EVENT_TABLE(FindCmdDlg::SearchEvtHandler, wxEvtHandler)
	EVT_CHAR(FindCmdDlg::SearchEvtHandler::OnChar)
END_EVENT_TABLE()

void FindCmdDlg::SearchEvtHandler::OnChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_UP:
		m_actionList.SelectPrev();
		return;
	case WXK_DOWN:
		m_actionList.SelectNext();
		return;
	case WXK_ESCAPE:
		m_parent.EndModal(wxID_CANCEL);
		return;
	}

	// no, we didn't process it
    event.Skip();
}

// --- ActionList --------------------------------------------------------

FindCmdDlg::ActionList::ActionList(wxWindow* parent, wxWindowID id, const vector<const tmAction*>& actions)
: SearchListBox(parent, id), m_actions(actions) {
	// We need a unicode font
	const unsigned int fontsize = (unsigned int)(m_font.GetPointSize() * 0.9);
	m_unifont = wxFont(fontsize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
		false, wxT("Lucida Sans Unicode"));

	SetAllItems();
}

void FindCmdDlg::ActionList::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const {
	const bool isCurrent = IsCurrent(n);

	if (isCurrent) dc.SetTextForeground(m_hlTextColor);
	else dc.SetTextForeground(m_textColor);
	const unsigned int ypos = rect.y + m_topMargen;
	unsigned int rightBorder = rect.GetRight();

	const tmAction& action = *m_items[n].action;
	const vector<unsigned int>& hl = m_items[n].hlChars;

	wxString name = action.name;
	if (action.bundle) {
		name += wxT(" - ");
		name += action.bundle->name;
	}

	if ( !action.trigger.empty() )
	{
		const wxString trig =  action.trigger + wxT("\x21E5");

		dc.SetFont(m_unifont);
		int trig_width, trig_height;
		dc.GetTextExtent(trig, &trig_width, &trig_height);

		// Draw a grey rounded rect as background for trigger
		const unsigned int bg_height = trig_height + 1;
		const unsigned int bg_width = trig_width + 10;
		const unsigned int bg_xpos = rect.GetWidth()-2-bg_width;
		const unsigned int bg_ypos = rect.y+ (int)((rect.GetHeight()-bg_height)/2.0);
		dc.SetPen(*wxLIGHT_GREY_PEN);
		dc.SetBrush(*wxLIGHT_GREY_BRUSH);
		dc.DrawRoundedRectangle(bg_xpos, bg_ypos, bg_width, bg_height, 2);

		// Draw the trigger text
		const wxColour prevColour = dc.GetTextForeground();
		dc.SetTextForeground(*wxBLACK);
		dc.DrawText(trig, bg_xpos + 5, bg_ypos);
		dc.SetTextForeground(prevColour);

		rightBorder = bg_xpos;
	}
	else if (!action.key.shortcut.empty()) {
		const wxString& shortcut = action.key.shortcut;

		dc.SetFont(m_font);
		int accel_width, accel_height;
        dc.GetTextExtent(shortcut, &accel_width, &accel_height);

		const unsigned int xpos = rect.GetWidth()-2-accel_width;
		dc.DrawText(shortcut, xpos, ypos);

		rightBorder = xpos;
	}

	// Calc extension width
	static const wxString ext = wxT("..  ");
	dc.SetFont(m_font);
	int w, h;
	dc.GetTextExtent(ext, &w, &h);
	const unsigned int extwidth = w;

	// See if we have to resize the action name to fit
	// note that this is not 100% correct as bold chars take up a bit more space.
	unsigned int len = name.length();
	dc.GetTextExtent(name, &w, &h);
	if (w > (int)rightBorder) {
		do {
			name.resize(--len);
			dc.GetTextExtent(name, &w, &h);
		} while (len > 0 && w + extwidth > rightBorder);
		name += ext;
	}

	// Draw action name
	DrawItemText(dc, rect, name, hl, isCurrent);
}

void FindCmdDlg::ActionList::SetAllItems() {
	m_items.clear();
	m_items.resize(m_actions.size());

	for (unsigned int i = 0; i < m_actions.size(); ++i) {
		m_items[i].action = m_actions[i];
	}

	sort(m_items.begin(), m_items.end());
	SetItemCount(m_items.size());
	SetSelection(-1);
	RefreshAll();
}

void FindCmdDlg::ActionList::Find(const wxString& searchtext) {
	if (searchtext.empty()) {
		SetAllItems();
		return;
	}

	// Convert to upper case for case insensitive search
	const wxString text = searchtext.Upper();

	m_items.clear();
	vector<unsigned int> hlChars;
	for (unsigned int i = 0; i < m_actions.size(); ++i) {
		const wxString& name = m_actions[i]->name;
		unsigned int charpos = 0;
		wxChar c = text[charpos];
		hlChars.clear();

		for (unsigned int textpos = 0; textpos < name.size(); ++textpos) {
			if ((wxChar)wxToupper(name[textpos]) == c) {
				hlChars.push_back(textpos);
				++charpos;
				if (charpos == text.size()) {
					// All chars found.
					m_items.push_back(aItem(m_actions[i], hlChars));
					break;
				}
				else c = text[charpos];
			}
		}
	}

	sort(m_items.begin(), m_items.end());
	SetItemCount(m_items.size());

	if (m_items.empty()) SetSelection(-1); // deselect
	else SetSelection(0);

	RefreshAll();
}

const tmAction* FindCmdDlg::ActionList::GetSelectedAction() {
	const int sel = GetSelection();
	if (sel == -1) return NULL;
	else return m_items[sel].action;
}


// --- aItem --------------------------------------------------------

FindCmdDlg::ActionList::aItem::aItem(const tmAction* a, const vector<unsigned int>& hl)
: action(a), hlChars(hl) {
	// Calculate rank (total distance between chars)
	rank = 0;
	if (hlChars.size() > 1) {
		unsigned int prev = hlChars[0]+1;
		for (vector<unsigned int>::const_iterator p = hlChars.begin()+1; p != hlChars.end(); ++p) {
			rank += *p - prev;
			prev = *p+1;
		}
	}
}
