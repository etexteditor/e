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

#ifndef __SEARCHLISTBOX_H__
#define __SEARCHLISTBOX_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/vlbox.h>

#include <vector>

class SearchListBox : public wxVListBox {
public:
	SearchListBox(wxWindow* parent, wxWindowID id);

	void SelectNext();
	void SelectPrev();

	// utility functions
	static unsigned int CalcRank(const std::vector<unsigned int>& hl);

protected:
	void DrawItemText(wxDC& dc, const wxRect& rect, const wxString& name, const std::vector<unsigned int>& hl, bool isCurrent) const;

	wxCoord OnMeasureItem(size_t n) const;

	// Event handlers
	void OnChar(wxKeyEvent& event);
	DECLARE_EVENT_TABLE();

	wxCoord m_itemHeight;
	wxFont m_font;
	wxFont m_boldFont;
	wxColour m_textColor;
	wxColour m_hlTextColor;
	unsigned int m_topMargin;
	unsigned int m_leftMargin;
};

#endif // __SEARCHLISTBOX_H__
