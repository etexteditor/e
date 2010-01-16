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
/**
Make it into subclass.
Pass in current cursor position so the current word is not highlighted as typing.
-Check if the character before or after is a alphanumeric.
Change the highlight color.
Add a setting to disable it.
*/
#include "styler_variablehl.h"

#include "StyleRun.h"
#include "Lines.h"
#include "Document.h"

const unsigned int Styler_VariableHL::EXTSIZE = 1000;

Styler_VariableHL::Styler_VariableHL(const DocumentWrapper& rev, const Lines& lines, const vector<interval>& ranges, const tmTheme& theme, eSettings& settings)
: m_doc(rev), m_lines(lines), m_theme(theme), m_settings(settings),
  m_selectionHighlightColor(m_theme.selectionColor),
  m_searchHighlightColor(m_theme.searchHighlightColor) ,
  m_click(false), m_cursorPosition(0), m_key(-1)
{
	Clear(); // Make sure all variables are empty
}

void Styler_VariableHL::Clear() {
	m_text.Clear();
	m_matches.clear();
	m_search_start = 0;
	m_search_end = 0;
}

void Styler_VariableHL::Invalidate() {
	m_matches.clear();
	m_search_start = 0;
	m_search_end = 0;
}

void Styler_VariableHL::SetCurrentWord(const wxString& text, bool click, unsigned int cursorPosition, int key) {
	if (text != m_text) {
		Clear();
		m_text = text;
	}

	//These should always be updated.
	//The user could hit the arrow up key to move to a new word, in which case the highlighting should change
	m_click = click;
	m_cursorPosition = cursorPosition;
	m_key = key;
}

// Returns 0 for no style
// Returns 1 for 'select' background color
// Returns 2 for 'search' background color
int Styler_VariableHL::GetStyleType(unsigned int start, unsigned int end) {
	if(m_click) return 1;
	if(IsArrowKey(m_key)) {
		return 1;
	} else {
		if(IsCurrentWord(start, end)) {
			return 0;
		} else {
			return 1;
		}
	}
}

bool Styler_VariableHL::IsCurrentWord(unsigned int start, unsigned int end) {
	return start <= m_cursorPosition && end >= m_cursorPosition;
}

bool Styler_VariableHL::IsArrowKey(int key) {
	switch(key) {
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_UP:
		case WXK_NUMPAD_UP:
		case WXK_DOWN:
		case WXK_NUMPAD_DOWN:
		case WXK_HOME:
		case WXK_NUMPAD_HOME:
		case WXK_NUMPAD_BEGIN:
		case WXK_END:
		case WXK_NUMPAD_END:
		case WXK_PAGEUP:
		case WXK_NUMPAD_PAGEUP:
		case WXK_PAGEDOWN:
		case WXK_NUMPAD_PAGEDOWN:
		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
		case WXK_TAB:
		case WXK_NUMPAD_TAB:
		case WXK_ESCAPE:
			return true;
	}
	return false;
}

void Styler_VariableHL::Style(StyleRun& sr) {
	bool shouldStyle = false;
	m_settings.GetSettingBool(wxT("highlightVariables"), shouldStyle);
	if(!shouldStyle) return;

	const unsigned int rstart =  sr.GetRunStart();
	const unsigned int rend = sr.GetRunEnd();

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

	//wxLogDebug(wxT("Style %u %u"), rstart, rend);
	//wxLogDebug(wxT(" %u %u - %u %u"), sr_start, sr_end, m_search_start, m_search_end);
	// Check if we need to do a new search
	if (sr_start < m_search_start || m_search_end < sr_end) {
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

		//wxLogDebug(wxT(" %u %u - %u %u"), sr_start, sr_end, m_search_start, m_search_end);

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

			int styleType = GetStyleType(start, end);
			if(styleType == 1) {
				sr.SetBackgroundColor(start, end, m_selectionHighlightColor);
				sr.SetShowHidden(start, end, true);
			} else if(styleType == 2) {
				sr.SetBackgroundColor(start, end, m_searchHighlightColor);
				sr.SetShowHidden(start, end, true);
			}
		}
	}
}

inline bool isAlphaNumeric(wxChar c) {
#ifdef __WXMSW__
	return ::IsCharAlphaNumeric(c) != 0;
#else
	return wxIsalnum(c);
#endif
}

void Styler_VariableHL::DoSearch(unsigned int start, unsigned int end, bool from_last) {
	wxASSERT(0 <= start && start < m_doc.GetLength());
	wxASSERT(start < end && end <= m_doc.GetLength());

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
	wxChar previousChar, nextChar;
	bool skip;
	while(result.end < end) {
		skip = false;
		cxLOCKDOC_READ(m_doc)
			result = doc.Find(m_text, result.end, true, end);
			if(result.start > 0) {
				previousChar = doc.GetChar(result.start-1);
				if(isAlphaNumeric(previousChar)) skip = true;
			}
			if(result.end < doc.GetLength()) {
				nextChar = doc.GetChar(result.end);
				if(isAlphaNumeric(nextChar)) skip = true;
			}
			//wxLogDebug(wxT("%d %d |%c| |%c|"), result.start, result.end, doc.GetChar(result.start), doc.GetChar(result.end));
		cxENDLOCK

		if (result.error_code < 0 || result.start >= end) break;
		// Avoid never ending loop if zero-length match
		if (result.start == result.end) ++result.end;
		if(skip)continue;

		// Add new match to list
		const interval iv(result.start, result.end);
		if (from_last) m_matches.push_back(iv);
		else {
			// Check if we have hit a previous match
			while (next_match != m_matches.end() && result.end > next_match->start) {
				// if not equivalent, replace and continue
				if (next_match->start == result.start && next_match->end == result.end)	break;
				next_match = m_matches.erase(next_match);
			}

			next_match = m_matches.insert(next_match, iv);
			++next_match;
		}

		// Avoid never ending loop if zero-length match
		if (result.start == result.end) ++result.end;
	}
}

//the insert and delete functions are unnecessary
//Style is called every time the mouse is moved and a key is pressed.
//That means, every time the user inserts or deletes a character, Style will be called via the Onchar event handler

void Styler_VariableHL::ApplyDiff(const vector<cxChange>& WXUNUSED(changes)) {
	Invalidate();
}
