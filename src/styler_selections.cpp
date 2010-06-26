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

#include "styler_selections.h"

#include "StyleRun.h"
#include "Lines.h"
#include "Document.h"

Styler_Selections::Styler_Selections(const DocumentWrapper& rev, const Lines& lines, const tmTheme& theme, EditorCtrl& editorCtrl)
: m_doc(rev), m_lines(lines), 
  m_theme(theme), m_searchHighlightColor(m_theme.searchHighlightColor),
  m_selectionHighlightColor(m_theme.shadowColor) ,
  m_enabled(falsed)
{
}

void Styler_Selections::EnableNavigation() {
	m_enabled = !m_enabled;
	
	if(m_enabled) {
		m_nextSelection = -1;
		m_selections = m_editorCtrl.GetSelections();
		NextSelection();
	}
}

void Styler_Selections::NextSelection() {
	if(!m_enabled) return;

	m_nextSelection++;
	if(m_nextSelection >= m_selections.size()) {
		m_nextSelection = 0;
	}

	m_editorCtrl.RemoveAllSelections();
	m_editorCtrl.SetPos(m_selections[m_nextSelection].start);
	m_editorCtrlSelect(m_selections[m_nextSelection].start, m_selections[m_nextSelection].end);
	m_editorCtrl.DrawLayout();
}

void Styler_Selections::PreviousSelection() {
	if(!m_enabled) return;

	m_nextSelection-;
	if(m_nextSelection < 0) {
		m_nextSelection = m_selections.size()-1;
	}

	m_editorCtrl.RemoveAllSelections();
	m_editorCtrl.SetPos(m_selections[m_nextSelection].start);
	m_editorCtrlSelect(m_selections[m_nextSelection].start, m_selections[m_nextSelection].end);
	m_editorCtrl.DrawLayout();
}

void Styler_Selections::Style(StyleRun& sr) {
	if(!m_enabled) return;

	const unsigned int rstart =  sr.GetRunStart();
	const unsigned int rend = sr.GetRunEnd();

	for(unsigned int c = 0; c < m_selections.size(); c++) {
		unsigned int start = m_selections[c].start;
		unsigned int end = m_selections[c].end;

		if (start > rend) break;

		// Check for overlap (or zero-length sel at start-of-line)
		if ((end > rstart && start < rend) || (start == end && end == rstart)) {
			start = wxMax(rstart, start);
			end   = wxMin(rend, end);
			
			ApplyStyle(sr, start, end);
		}
	}
}

void Styler_Selections::ApplyStyle(StyleRun& sr, unsigned int start, unsigned int end) {
	sr.SetBackgroundColor(start, end, m_hlcolor);
	sr.SetShowHidden(start, end, true);
}

void Styler_Selections::Insert(unsigned int pos, unsigned int length) {
	wxASSERT(0 <= pos && pos < m_doc.GetLength());
	wxASSERT(0 <= length && pos+length <= m_doc.GetLength());

	if(!m_enabled) return;

	for(unsigned int c = 0; c < m_selections.size(); c++) {
		unsigned int start = m_selections[c].start;
		unsigned int end = m_selections[c].end;

		if(start > pos) {
			m_selections[c].start += length;
			m_selections[c].end += length;
		} else if(end > pos) {
			m_selections[c].end += length;
		}
	}
}

void Styler_Selections::Delete(unsigned int start_pos, unsigned int end_pos) {
	wxASSERT(0 <= start_pos && start_pos <= m_doc.GetLength());
	if(!m_enabled) return;

	if (start_pos == end_pos) return;
	wxASSERT(end_pos > start_pos);

	// Check if we have deleted the entire document
	if (m_doc.GetLength() == 0) {
		Invalidate();
		return;
	}

	// Adjust start & end
	unsigned int length = end_pos - start_pos;
	if (m_search_start >= start_pos) {
		if (m_search_start > end_pos) m_search_start -= length;
		else m_search_start = start_pos;
	}
	if (m_search_end > start_pos) {
		if (m_search_end > end_pos) m_search_end -= length;
		else m_search_end = start_pos;
	}
	else return; // Change after search area, no need to re-search

	unsigned int search_start = m_search_start;

	if (!m_matches.empty()) {
		if (start_pos >= m_matches.back().end) {
			// Do a new search from end of last match
			DoSearch(search_start, m_search_end, true);
			return;
		}

		// Find matches touched by deletion and remove those. Update all following
		bool is_first = true;
		vector<interval>::iterator p = m_matches.begin();
		while (p != m_matches.end()) {
			if (p->end > start_pos) {
				// Remember first valid match before pos
				if (is_first) {
					if (p != m_matches.begin()) search_start = (p-1)->end;
					is_first = false;
				}

				if (p->start < end_pos) {
					// pos inside match. Delete and continue
					p = m_matches.erase(p);
					if (p != m_matches.end()) continue; // new iterator
					else break;
				}
				else {
					// Move match to correct position
					p->start -= length;
					p->end -= length;
				}
			}
			++p;
		}
	}
}