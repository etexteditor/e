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

#include "SearchListBox.h"
#include <wx/wx.h>

BEGIN_EVENT_TABLE(SearchListBox, wxVListBox)
	EVT_CHAR(SearchListBox::OnChar)
END_EVENT_TABLE()

SearchListBox::SearchListBox(wxWindow* parent, wxWindowID id)
: wxVListBox(parent, id, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER) {
	m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	m_boldFont = m_font;
	m_boldFont.SetWeight(wxFONTWEIGHT_BOLD);
	m_textColor = wxColour(wxT("DARK GREY"));
	m_hlTextColor = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);

	// Get item height
	wxCoord x;
	wxClientDC dc(this);
	dc.SetFont(m_font);
	dc.GetTextExtent(wxT("Xj"), &x, &m_itemHeight);
	m_itemHeight += 4; // Add a bit of margin

	m_topMargen = 1;
	m_leftMargen = 1;
}

wxCoord SearchListBox::OnMeasureItem(size_t WXUNUSED(n)) const {
	return m_itemHeight;
}

void SearchListBox::DrawItemText(wxDC& dc, const wxRect& rect, const wxString& name, const vector<unsigned int>& hl, bool isCurrent) const {
	const unsigned int ypos = rect.y + m_topMargen;
	
	// Draw action name
	if (hl.empty()) {
		dc.SetFont(m_font);
		if (!isCurrent) dc.SetTextForeground(m_textColor);
		dc.DrawText(name, rect.x + m_leftMargen, ypos);
	}
	else {
		unsigned int lastxpos = rect.x + m_leftMargen;
		unsigned int lastchar = 0;
		const unsigned int len = name.length();
		int w, h;

		// Draw the command name, highlighting chars from search
		for (vector<unsigned int>::const_iterator p = hl.begin(); p != hl.end(); ++p) {
			const unsigned int e = (*p > len-1) ? len-1 : *p;
			
			if (lastchar < e) {
				wxString str = name.substr(lastchar, e - lastchar);
				dc.SetFont(m_font);

				dc.GetTextExtent(str, &w, &h);

				if (!isCurrent) dc.SetTextForeground(m_textColor);
				dc.DrawText(str, lastxpos, ypos);

				// Move xpos
				lastxpos += w;
			}
			
			// highlight char
			if (e == *p) {
				const wxString charStr(name[e]);
				dc.SetFont(m_boldFont);
				if (!isCurrent) dc.SetTextForeground(*wxBLACK);
				dc.DrawText(charStr, lastxpos, ypos);

				// Move xpos
				dc.GetTextExtent(charStr, &w, &h);
				lastxpos += w;
			}
			
			lastchar = e + 1;
			if (lastchar == len) break;
		}

		if (lastchar < name.size()) {
			dc.SetFont(m_font);
			if (!isCurrent) dc.SetTextForeground(m_textColor);
			dc.DrawText(name.substr(lastchar), lastxpos, ypos);
		}
	}
}

void SearchListBox::SelectNext() {
	int sel = GetSelection();
	if (sel == -1) sel = 0;
	else ++sel;

	if (sel < (int)GetItemCount()) SetSelection(sel);
}

void SearchListBox::SelectPrev() {
	int sel = GetSelection();
	if (sel == -1) sel = GetItemCount()-1;
	else --sel;

	if (sel >= 0) SetSelection(sel);
}

void SearchListBox::OnChar(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_RETURN) {
		const int item = GetSelection();

		if (item != -1) {
			wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, GetId());
			event.SetEventObject(this);
			event.SetInt(item);

			(void)GetEventHandler()->ProcessEvent(event);
		}
	}
	else event.Skip();
}

unsigned int SearchListBox::CalcRank(const vector<unsigned int>& hl) { // static
	unsigned int rank = 0;
	if (hl.size() > 1) {					
		unsigned int prev = hl[0]+1;
		for (vector<unsigned int>::const_iterator p = hl.begin()+1; p != hl.end(); ++p) {
			rank += *p - prev;
			prev = *p+1;
		}
	}

	return rank;
}
