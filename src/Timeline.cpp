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

#include "Timeline.h"

BEGIN_EVENT_TABLE(Timeline, wxControl)
	EVT_PAINT(Timeline::OnPaint)
	EVT_SIZE(Timeline::OnSize)
	EVT_ERASE_BACKGROUND(Timeline::OnEraseBackground)
END_EVENT_TABLE()

Timeline::Timeline(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxControl(parent, id, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE), \
	  m_mdc(), m_bitmap(1,1), m_scrollPos(0)
{
	// Initialize variables
	m_itemHeight = 18;

	// Set the colors
	m_bgcolor.Set(192, 192, 255); // Pastel purple
	m_hlightcolor.Set(172, 172, 235); // Pastel purple (slightly darker)
	m_edgecolor.Set(92, 92, 155); // Pastel purple (darker)
	m_numbercolor.Set(52, 52, 115); // Pastel purple (even darker)

	// Set the fonts
	m_dayFont = wxFont(9, wxMODERN, wxNORMAL, wxNORMAL);
	m_labelFont = wxFont(12, wxDEFAULT, wxNORMAL, wxFONTWEIGHT_BOLD);

	// Initialize the memoryDC for double buffering
	m_mdc.SelectObject(m_bitmap);
	m_mdc.SetBackground(wxBrush(m_bgcolor));
	m_mdc.SetFont(m_dayFont);

	if (false == m_mdc.Ok()) {
		wxLogError(wxT("wxMemoryDC() constructor was failed in creating!"));	
	}
}

int Timeline::AddItem(const wxDateTime& date) {
	item new_item;
	new_item.date = date;

	if (m_items.empty()) new_item.ypos = 0;
	else if (date.IsSameDate(m_items.back().date)) {
		new_item.ypos = m_items.back().ypos + m_itemHeight;
	}
	else if (date.IsEarlierThan(m_items.back().date)) {
		// Dates have to be sequential
		new_item.date = m_items.back().date; // force to same date as previous
		new_item.ypos = m_items.back().ypos + m_itemHeight;
	}
	else {
		// Calculate number of days between dates
		wxDateTime firstDay = m_items.back().date;
		wxDateTime secondDay = date;
		firstDay.ResetTime();
		secondDay.ResetTime();

		wxTimeSpan time = secondDay - firstDay;
		int days = time.GetDays();

		// BUG WORKAROUND: Because of shift to daylight saving time
		// the count of days might be one less.
		if (time.GetHours() % 24 == 23) ++days;

		wxASSERT(days > 0);

		new_item.ypos = m_items.back().ypos + (days * m_itemHeight);
	}

	m_items.push_back(new_item);

	return new_item.ypos;
}

void Timeline::DrawTimeline(wxDC& dc) {
	const wxSize size = GetClientSize();
	const wxRect rect(0, m_scrollPos, size.x, size.y);

	DrawTimeline(rect);

	// Copy MemoryDC to Display
	dc.Blit(0, 0, size.x, size.y, &m_mdc, 0, 0);

	m_needRedrawing = false;
}

void Timeline::DrawTimeline(const wxRect& rect) {
	const wxSize size = GetClientSize();
	int adj_ypos = rect.y - m_scrollPos;
	const int adj_ybottom = adj_ypos + rect.height;

	// Set the clipping region
	m_mdc.DestroyClippingRegion();
	m_mdc.SetClippingRegion(rect.x, adj_ypos, rect.width, rect.height);

	// Clear the background
	m_mdc.SetBrush(m_bgcolor);
	m_mdc.SetPen(m_bgcolor);
	m_mdc.DrawRectangle(0, adj_ypos, size.x, rect.height);

	// Draw the edge
	m_mdc.SetPen(m_edgecolor);
	m_mdc.DrawLine(size.x-1, adj_ypos, size.x-1, adj_ybottom);

	// Find the first visible day
	wxDateTime date;
	unsigned int next_item = 0;
	int y_pos = 0;
	if (m_items.empty()) {
		date = wxDateTime::Now();
	}
	else {
		// Find the first visible item (or the first below visible area)
		for (unsigned int i = 0; i < m_items.size(); ++i) {
			const int item_ybottom = m_items[i].ypos + m_itemHeight;
			if (item_ybottom > rect.y) {
				next_item = i;
				break;
			}
		}

		date = m_items[next_item].date;
		y_pos = m_items[next_item].ypos;

		if (next_item > 0) {
			if (date.IsSameDate(m_items[next_item-1].date)) {
				// If the first item does not have a visible day (there are items from the
				// same day above it), we have to find the last item on the day
				while (next_item+1 < m_items.size() && date.IsSameDate(m_items[next_item+1].date)) {
					++next_item;
				}

				// Add one day (to get first visible day)
				date += wxTimeSpan::Day();
				y_pos = m_items[next_item].ypos + m_itemHeight;

				++next_item; // first item with visible day (might not exists)
			}
			else {
				// Calculate first visible (non-item) date
				const int distance_to_ytop = m_items[next_item].ypos - rect.y;
				int days_to_ytop = distance_to_ytop / m_itemHeight;
				if (distance_to_ytop > 0 && distance_to_ytop % m_itemHeight) ++days_to_ytop;

				date -= wxDateSpan(0, 0, 0, days_to_ytop);
				y_pos = m_items[next_item].ypos - (days_to_ytop * m_itemHeight);
			}
		}
	}

	adj_ypos = y_pos - m_scrollPos;
	int day_of_month = date.GetDay();

	// Calculate y-pos for start of first month
	int month_ypos = adj_ypos;
	if (m_items.empty()) month_ypos = adj_ypos - ((day_of_month-1) * m_itemHeight);
	else {
		const wxDateTime month_start(1, date.GetMonth(), date.GetYear());
		wxDateTime d = date;
		d.ResetTime();
		int prev_item = next_item-1;

		while (d > month_start) {
			month_ypos -= m_itemHeight;

			// There can be multible items on a single day
			if (prev_item >= 0 && d.IsSameDate(m_items[prev_item].date)) --prev_item;
			while (prev_item >= 0 && d.IsSameDate(m_items[prev_item].date)) {
				month_ypos -= m_itemHeight;
				--prev_item;
			}

			d -= wxTimeSpan::Day();
		}
	}

	// Draw the label for the first month
	// (which may be partially or fully hidden)
	{
		const wxString first_monthyear = date.Format(wxT("%B %Y"));
		int textwidth;
		int textheight;
		m_mdc.SetFont(m_labelFont);
		m_mdc.GetTextExtent(first_monthyear, &textwidth, &textheight);
		const int month_label_ybottom = month_ypos + textwidth + 10;
		if (month_label_ybottom > adj_ypos) {
			m_mdc.DrawRotatedText(first_monthyear, 0, month_label_ybottom, 90);
		}
	}

	// Prepare for day drawing
	int days_in_month = wxDateTime::GetNumberOfDays(date.GetMonth(), date.GetYear());
	m_mdc.SetFont(m_dayFont);
	m_mdc.SetTextForeground(m_numbercolor);
	wxString number;
	const int day_xpos = size.x - 17;

	while (adj_ypos < adj_ybottom) {
		if (day_of_month > days_in_month) {
			// Draw a seperator line
			m_mdc.SetPen(m_edgecolor);
			m_mdc.DrawLine(0, adj_ypos-1, size.x, adj_ypos-1);

			// Draw month/year label for the next month
			wxString monthyear = date.Format(wxT("%B %Y"));
			int textwidth;
			int textheight;
			m_mdc.SetFont(m_labelFont);
			m_mdc.GetTextExtent(monthyear, &textwidth, &textheight);
			m_mdc.DrawRotatedText(monthyear, 0, adj_ypos+textwidth+10, 90);
			m_mdc.SetFont(m_dayFont);

			// Go to next month
			day_of_month = 1;
			days_in_month = wxDateTime::GetNumberOfDays(date.GetMonth(), date.GetYear());
		}

		// Check if we should draw a week seperator
		wxDateTime::WeekDay weekday = date.GetWeekDay();
		if (weekday == wxDateTime::Mon) {
			m_mdc.SetPen(m_edgecolor);
			m_mdc.DrawLine(day_xpos, adj_ypos-1, size.x, adj_ypos-1);
		}

		// Draw the day
		//wxString dayname = wxDateTime::GetWeekDayName(weekday, wxDateTime::Name_Abbr);
		//number.Printf(wxT("%.1s %2d"), dayname, day_of_month);
		number.Printf(wxT("%2d"), day_of_month);
		if (!date.IsWorkDay()) {
			// Draw a darker background to highlight holidays
			m_mdc.SetBrush(m_hlightcolor);
			m_mdc.SetPen(m_hlightcolor);
			m_mdc.DrawRectangle(day_xpos, adj_ypos, (size.x-1)-day_xpos, m_itemHeight);

		}
		m_mdc.DrawText(number, day_xpos, adj_ypos);

		// There can be multible items on a single day
		adj_ypos += m_itemHeight;
		if (next_item < m_items.size() && date.IsSameDate(m_items[next_item].date)) ++next_item;
		while (next_item < m_items.size() && date.IsSameDate(m_items[next_item].date)) {
			if (!date.IsWorkDay()) {
				// Draw a darker background to highlight holidays
				m_mdc.DrawRectangle(day_xpos, adj_ypos, (size.x-1)-day_xpos, m_itemHeight);
			}

			adj_ypos += m_itemHeight;
			++next_item;
		}

		++day_of_month;
		date += wxTimeSpan::Day();
	}
}

void Timeline::Scroll(int pos) {
	if (pos == m_scrollPos) return;

	const wxSize size = GetClientSize();
	wxRect rect;

	if (m_needRedrawing) {
		m_scrollPos = pos;
		rect = wxRect(0,m_scrollPos,size.x,size.y);
	}
	else {
		// If there is overlap, then move the image
		if (pos + size.y > m_scrollPos && pos < m_scrollPos + size.y) {
			const int top = wxMax(pos, m_scrollPos);
			const int bottom = wxMin(pos, m_scrollPos) + size.y;
			const int overlap_height = bottom - top;
			const int diff = m_scrollPos - pos;

			const int adj_source_ypos = top - m_scrollPos;
			const int adj_dest_ypos = adj_source_ypos + diff;

			m_mdc.DestroyClippingRegion();
			m_mdc.Blit(0, adj_dest_ypos, size.x, overlap_height, &m_mdc, 0, adj_source_ypos);

			const int new_height = size.y - overlap_height;
			const int newtop = (top == pos) ? bottom : pos;
			m_scrollPos = pos;
			rect = wxRect(0,newtop,size.x,new_height);
		}
		else {
			m_scrollPos = pos;
			rect = wxRect(0,m_scrollPos,size.x,size.y);
		}
	}

	DrawTimeline(rect);

	// Copy updated MemoryDC to Display
	wxClientDC dc(this);
	dc.Blit(0, 0,  size.x, size.y, &m_mdc, 0, 0);

	m_needRedrawing = false;
}

void Timeline::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	if (m_needRedrawing) {
		DrawTimeline(dc);
	}
	else {
		// Re-Blit MemoryDC to Display
		wxSize size = GetClientSize();
		dc.Blit(0, 0,  size.x, size.y, &m_mdc, 0, 0);
	}
}

void Timeline::OnSize(wxSizeEvent& WXUNUSED(event)) {
	wxSize size = GetClientSize();

	if (size.x <= 0 || size.y <= 0) return; // no need to update

	// resize the bitmap used for doublebuffering
	if (m_bitmap.GetWidth() < size.x || m_bitmap.GetHeight() < size.y) {
		m_bitmap = wxBitmap(size.x, size.y);
		m_mdc.SelectObject(m_bitmap);
	}

	// Draw the new layout
	wxClientDC dc(this);
	DrawTimeline(dc);
}

void Timeline::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
	// # no evt.skip() as we don't want the control to erase the background
}
