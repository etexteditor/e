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

#ifndef __TIMELINE_H__
#define __TIMELINE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/datetime.h>

#include <vector>

class Timeline : public wxControl {
public:
	Timeline(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void Clear() {m_items.clear();};
	int AddItem(const wxDateTime& date);
	void Scroll(int pos);
	void ReDraw() {wxClientDC dc(this);DrawTimeline(dc);};

protected:
	wxSize DoGetBestSize() const {return wxSize(40,-1);};

private:
	void DrawTimeline(const wxRect& rect);
	void DrawTimeline(wxDC& dc);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	DECLARE_EVENT_TABLE();

	wxMemoryDC m_mdc;
	wxBitmap m_bitmap;

	wxColour m_bgcolor;
	wxColour m_numbercolor;
	wxColour m_edgecolor;
	wxColour m_hlightcolor;
	wxFont   m_dayFont;
	wxFont   m_labelFont;

	bool m_needRedrawing;
	int m_scrollPos;

	struct item {
		wxDateTime date;
		int ypos;
	};

	int m_itemHeight;
	std::vector<item> m_items;
};

#endif // __TIMELINE_H__
