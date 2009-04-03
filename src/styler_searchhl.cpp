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

#include "styler_searchhl.h"
#include "EditorCtrl.h"
#include "eApp.h"

const unsigned int Styler_SearchHL::EXTSIZE = 1000;

Styler_SearchHL::Styler_SearchHL(const DocumentWrapper& rev, const Lines& lines, const vector<interval>& ranges)
: m_editor(NULL), m_doc(rev), m_lines(lines), m_searchRanges(ranges),
  m_theme(((eApp*)wxTheApp)->GetSyntaxHandler().GetTheme()), m_hlcolor(m_theme.searchHighlightColor),
  m_rangeColor(m_theme.shadowColor) {

	Clear(); // Make sure all variables are empty
}

void Styler_SearchHL::Init(const EditorCtrl& editor) {
	// We need a pointer to the editor for Regex seaching
	m_editor = &editor;
}

void Styler_SearchHL::Clear() {
	m_text.Clear();
	m_options = 0;
	m_matches.clear();
	m_search_start = 0;
	m_search_end = 0;
}

void Styler_SearchHL::Invalidate() {
	m_matches.clear();
	m_search_start = 0;
	m_search_end = 0;
}

void Styler_SearchHL::SetSearch(const wxString& text, int options) {
	if (text != m_text || options != m_options) {
		Clear();
		m_text = text;
		m_options = options;
	}
}

void Styler_SearchHL::Style(StyleRun& sr) {
	const unsigned int rstart =  sr.GetRunStart();
	const unsigned int rend = sr.GetRunEnd();

	// Style the run with search ranges
	for (vector<interval>::const_iterator r = m_searchRanges.begin(); r != m_searchRanges.end(); ++r) {
		if (r->end > rstart && r->start < rend) {
			unsigned int start = wxMax(rstart, r->start);
			unsigned int end   = wxMin(rend, r->end);
			sr.SetBackgroundColor(start, end, m_rangeColor);
		}
	}

	// No need for more styling if no search text
	if (m_text.empty()) return;

	// Extend stylerun start/end to get better search results (round up to whole EXTSIZEs)
	unsigned int sr_start = rstart> 100 ? rstart - 100 : 0;
	const unsigned int ext_end = ((rend/EXTSIZE) * EXTSIZE) + EXTSIZE;
	unsigned int sr_end = ext_end < m_lines.GetLength() ? ext_end : m_lines.GetLength();

	// Make sure the extended positions are valid
	cxLOCKDOC_READ(m_doc)
		sr_start = doc.GetValidCharPos(sr_start);
		if (sr_end != m_lines.GetLength()) sr_end = doc.GetValidCharPos(sr_end);
	cxENDLOCK

	//wxLogDebug("Style %u %u", rstart, rend);
	//wxLogDebug(" %u %u - %u %u", sr_start, sr_end, m_search_start, m_search_end);
	// Check if we need to do a new search
	if (sr_start < m_search_start || sr_end > m_search_end) {
		// Check if there is overlap so we can just extend the search area
		if (sr_end > m_search_start && sr_start < m_search_end) {
			sr_start = wxMin(sr_start, m_search_start);
			sr_end = wxMax(sr_end, m_search_end);
		}
		else {
			// Else we have to move it
			m_matches.clear();
			m_search_start = 0;
			m_search_end = 0;
		}

		// Do the search
		if (sr_start < m_search_start) {
			// Search from top
			DoSearch(sr_start, sr_end);
		}
		else if (sr_end > m_search_end) {
			// Search from bottom
			DoSearch(sr_start, sr_end, true);
		}
		else wxASSERT(false);

		m_search_start = sr_start;
		m_search_end = sr_end;
	}

	// Style the run with matches
	for (vector<interval>::iterator p = m_matches.begin(); p != m_matches.end(); ++p) {
		if (p->start > rend) break;

		// Check for overlap (or zero-length sel at start-of-line)
		if ((p->end > rstart && p->start < rend) || (p->start == p->end && p->end == rstart)) {
			unsigned int start = wxMax(rstart, p->start);
			unsigned int end   = wxMin(rend, p->end);

			// Only draw it if it is in range
			if (!m_searchRanges.empty()) {
				bool inRange = false;
				for (vector<interval>::const_iterator s = m_searchRanges.begin(); s != m_searchRanges.end(); ++s) {
					if (start >= s->start && start < s->end) {
						inRange = true;
						break;
					}
				}
				if (!inRange) continue;
			}

			sr.SetBackgroundColor(start, end, m_hlcolor);
			sr.SetShowHidden(start, end, true);
		}
	}
}

void Styler_SearchHL::DoSearch(unsigned int start, unsigned int end, bool from_last) {
	wxASSERT(start >= 0 && start < m_doc.GetLength());
	wxASSERT(end > start && end <= m_doc.GetLength());
	wxASSERT(m_editor);

	bool matchcase = m_options & FIND_MATCHCASE;

	vector<interval>::iterator next_match = m_matches.begin();
	if (from_last) {
		// Start search from last match
		if (!m_matches.empty())	start = m_matches.back().end;
	}
	else {
		// Find the match before start pos (if there is any)
		for (; next_match != m_matches.end(); ++next_match) {
			if (next_match->end > start) break;
		}
	}

	//wxLogDebug("  DoSearch %u %u %d", start, end, from_last);

	// Search from start until we hit a previous match (or end)
	search_result result = {-1, 0, start};
	while(result.end < end) {
		cxLOCKDOC_READ(m_doc)
			if (m_options & FIND_USE_REGEX) result = doc.RegExFind(m_text, result.end, matchcase, NULL, end);
			else result = doc.Find(m_text, result.end, matchcase, end);
		cxENDLOCK

		if (result.error_code < 0 || result.start >= end) break;
		else {
			// Add new match to list
			const interval iv(result.start, result.end);
			if (from_last) m_matches.push_back(iv);
			else {
				// Check if we have hit a previous match
				while (next_match != m_matches.end() && result.end > next_match->start) {
					// if not equivalent, replace and continue
					if (next_match->start == result.start && next_match->end == result.end)	break;
					else next_match = m_matches.erase(next_match);
				}

				next_match = m_matches.insert(next_match, iv);
				++next_match;
			}
		}

		// Avoid never ending loop if zero-length match
		if (result.start == result.end) ++result.end;
	}
}

void Styler_SearchHL::Insert(unsigned int pos, unsigned int length) {
	wxASSERT(pos >= 0 && pos < m_doc.GetLength());
	wxASSERT(length >= 0 && pos+length <= m_doc.GetLength());
	if (m_text.empty()) return;

	// Adjust start & end
	if (m_search_start >= pos) m_search_start += length;
	if (m_search_end > pos)	m_search_end += length;
	else return; // Change outside search area

	unsigned int search_start = m_search_start;

	if (!m_matches.empty()) {
		if (pos >= m_matches.back().end) {
			// Do a new search from end of last match
			DoSearch(search_start, m_search_end, true);
			return;
		}
		else {
			// Find first match containing or bigger than pos
			bool is_first = true;
			vector<interval>::iterator p = m_matches.begin();
			while (p != m_matches.end()) {
				if (p->end > pos) {
					// Remember first valid match before pos
					if (is_first) {
						if (p != m_matches.begin()) search_start = (p-1)->end;
						is_first = false;
					}

					if (p->start < pos) {
						// pos inside match. Delete and continue
						p = m_matches.erase(p);
						if (p != m_matches.end()) continue; // new iterator
						else break;
					}
					else {
						// Move match to correct position
						p->start += length;
						p->end += length;
					}
				}
				++p;
			}
		}
	}

	// Do a new search starting from end of first match before pos
	DoSearch(search_start, m_search_end, false);
}

void Styler_SearchHL::Delete(unsigned int start_pos, unsigned int end_pos) {
	wxASSERT(start_pos >= 0 && start_pos <= m_doc.GetLength());
	if (m_text.empty()) return;

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
		else {
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

	// Do a new search starting from end of first match before pos
	DoSearch(search_start, m_search_end, false);
}

void Styler_SearchHL::ApplyDiff(const vector<cxChange>& WXUNUSED(changes)) {
	Invalidate();
}
