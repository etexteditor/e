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

#include "EditorPrintout.h"
#include "IPrintableDocument.h"
#include "tmTheme.h"
#include "FixedLine.h"
#include "LineListWrap.h"
#include "Catalyst.h"
#include "BracketHighlight.h"

// Dummy vars for line
const std::vector<interval> s_sel;
const BracketHighlight s_brackets;
const unsigned int s_lastpos = 0;
const bool s_isShadow = false;

EditorPrintout::EditorPrintout(const IPrintableDocument& printDoc, const tmTheme& theme):
	wxPrintout(printDoc.GetName()), 
	m_printDoc(printDoc), m_theme(theme),
	m_line(NULL), m_lineList(NULL) {}

EditorPrintout::~EditorPrintout() {
	delete m_lineList;
	delete m_line;
}

unsigned int digits_in_number(unsigned int number) {
	unsigned int count = 1; // minimum is one
	while ((number /= 10) != 0) ++count;
	return count;
}

void EditorPrintout::OnPreparePrinting() {
	// Make page size scale with screen resolution
	MapScreenSizeToPage();
	const wxRect size = GetLogicalPageRect();

	// Set the current themes font
	const wxFont& font = m_theme.font;
	wxDC& dc = *GetDC();
	dc.SetFont(font);

	// Initialize line info

	m_line = new FixedLine(dc, m_printDoc.GetDocument(), s_sel, s_brackets, s_lastpos, s_isShadow, m_theme);
	m_line->SetPrintMode();
	m_line->Init();
	m_line->SetWordWrap(cxWRAP_SMART);
	m_lineList = new LineListWrap(*m_line, m_printDoc.GetDocument());
	m_lineList->SetOffsets(m_printDoc.GetOffsets());

	// Calc gutter width
	const wxSize digit_ext = dc.GetTextExtent(wxT("0"));
	const unsigned int max_num_width = digits_in_number(m_lineList->size());
	m_gutter_width = (max_num_width * digit_ext.x) + 8;

	m_line->SetWidth(size.width - m_gutter_width);
	m_lineList->invalidate();
	m_lineList->prepare_all();

	m_page_height = size.height - (m_line->GetCharHeight() * 2); // room for footer

	// Calculate page list
	if (m_lineList->size()) {
		unsigned int bottom_ypos = m_page_height;

		// Calc lines on pages
		while (bottom_ypos <= m_lineList->height()) {
			const unsigned int lastline = m_lineList->find_ypos(bottom_ypos)-1;
			m_pages.push_back(lastline);
			bottom_ypos = m_lineList->bottom(lastline) + m_page_height;
		}

		if (bottom_ypos > m_lineList->height()) 
			m_pages.push_back(m_lineList->last());
	}
}

void EditorPrintout::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo) {
	wxASSERT(m_line && m_lineList);

	*minPage = 1;
	*maxPage = m_pages.empty() ? 1 : (int)m_pages.size();
	*pageFrom = 1;
	*pageTo = 1;
}

bool EditorPrintout::OnPrintPage(int pageNum) {
	wxASSERT(m_line && m_lineList);

	if (m_pages.empty()) return true;

	unsigned int page_ndx = pageNum-1; // page numbers start from one
	const unsigned int lastline = m_pages[page_ndx];
	unsigned int line_id = page_ndx ? m_pages[page_ndx-1]+1 : 0;

	// Make page size scale with screen resolution
	MapScreenSizeToPage();
	const wxRect rect = GetLogicalPageRect();
	unsigned int ypos = rect.y;

	// We may have gotten a new dc, so we need a new line
	wxDC& dc = *GetDC();
	const wxFont& font = m_theme.font;
	dc.SetFont(font);

	FixedLine line(dc, m_printDoc.GetDocument(), s_sel, s_brackets, s_lastpos, s_isShadow, m_theme);
	line.SetPrintMode();
	line.Init();
	line.SetWordWrap(cxWRAP_SMART);
	line.SetWidth(rect.width - m_gutter_width);

	// Draw gutter separator
	const unsigned int sep_xpos = m_gutter_width - 5;
	const unsigned int number_right = m_gutter_width - 8;
	dc.DrawLine(sep_xpos, rect.y, sep_xpos, rect.y + m_page_height);

	// Draw lines
	while (line_id <= lastline) {
		// Draw line number
		const wxString linenumber = wxString::Format(wxT("%u"), line_id+1);
		const wxSize ln_ext = dc.GetTextExtent(linenumber);
		dc.DrawText(linenumber, number_right - ln_ext.x, ypos);

		// Draw line
		line.SetLine(m_lineList->offset(line_id), m_lineList->end(line_id));
		line.DrawLine(rect.x + m_gutter_width, ypos, rect, false);

		ypos += line.GetHeight();
		++line_id;
	}

	// Draw footer title
	ypos = rect.GetBottom() - m_line->GetCharHeight();
	dc.DrawText(m_printDoc.GetName(), 0, ypos);

	// Draw footer pagenum
	const wxString pagefooter = wxString::Format(wxT("%d / %d"), pageNum, m_pages.size());
	const wxSize extent = dc.GetTextExtent(pagefooter);
	dc.DrawText(pagefooter, rect.GetRight() - extent.x, ypos);

	return true;
}
