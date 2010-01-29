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

#include "styler_htmlhl.h"

#include "StyleRun.h"
#include "Lines.h"
#include "Document.h"

inline bool isAlphaNumeric(wxChar c) {
#ifdef __WXMSW__
	return ::IsCharAlphaNumeric(c) != 0;
#else
	return wxIsalnum(c);
#endif
}

Styler_HtmlHL::Styler_HtmlHL(const DocumentWrapper& rev, const Lines& lines, const vector<interval>& ranges, const tmTheme& theme, eSettings& settings)
: m_doc(rev), m_lines(lines), m_theme(theme), m_settings(settings),
  m_selectionHighlightColor(m_theme.selectionColor),
  m_searchHighlightColor(m_theme.searchHighlightColor)
{
	initialParse = false;
	m_cursorPosition = m_lines.GetPos();
}

void Styler_HtmlHL::Clear() {
	Reparse();
}

void Styler_HtmlHL::Invalidate() {
	//Reparse();
}

void Styler_HtmlHL::Reparse() {
	initialParse = true;

	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();
		FindAllBrackets(data);
		FindTags(data);
		m_currentTag = FindCurrentTag();
		m_closingTag = FindClosingTag(data);
	cxENDLOCK
}

void Styler_HtmlHL::UpdateCursorPosition(unsigned int pos) {
	m_cursorPosition = pos;
	m_currentTag = FindCurrentTag();
	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();
		m_closingTag = FindClosingTag(data);
	cxENDLOCK
}

void Styler_HtmlHL::FindAllBrackets(const wxChar* data) {
	m_brackets.clear();
	FindBrackets(0, m_doc.GetLength()-1, data);
}

void Styler_HtmlHL::FindBrackets(unsigned int start, unsigned int end, const wxChar* data) {
	vector<unsigned int> buffer;
	
	//copy the existing brackets to a temporary array so we can add them in order, in linear time
	for (vector<unsigned int>::iterator p = m_brackets.begin(); p != m_brackets.end(); ++p) {
		buffer.push_back(*p);
	}
	m_brackets.clear();
	
	unsigned int index = 0;
	for(unsigned int c = start; c < end; c++) {
		switch(data[c]) {
			case '<':
			case '>':
				//now we have the index of a bracket inside the search range
			
				//add any items in buffer that are before this bracket
				for(; index < buffer.size(); index++) {
					if(buffer[index] < c) {
						m_brackets.push_back(buffer[index]);
					} else {
						break;
					}
				}
			
				//now add this bracket
				m_brackets.push_back(c);
				
			break;
		}
	}

	for(; index < buffer.size(); index++) {
		m_brackets.push_back(buffer[index]);
	}
}

bool Styler_HtmlHL::IsValidTag(unsigned int start, unsigned int end, const wxChar* data) {
	start++;
	
	//if there is no tag name, it is not a tag
	if(start == end) return false;
	
	//if it starts with a slash, it might be valid
	if(data[start] == '/') start++;
		
	//if it starts with an alphabetic character, then it is prolly valid
	if(isAlphaNumeric(data[start])) return true;
	
	//TODO: comments
	
	return false;
}

//when inserting/removing a character, i should be able to ignore any brackets before the insertion, i should be able to just add those tags right back in to m_tags
void Styler_HtmlHL::FindTags(const wxChar* data) {
	m_tags.clear();
	vector<unsigned int> buffer;
	bool haveOpenBracket = false;
	int openBracketIndex = -1, closeBracketIndex = -1;
	
	//copy the existing brackets to a temporary array so we can add them in order, in linear time
	for (vector<unsigned int>::iterator p = m_brackets.begin(); p != m_brackets.end(); ++p) {
		if(haveOpenBracket) {
			if(data[*p] == '>') {
				closeBracketIndex = *p;
				if(IsValidTag(openBracketIndex, closeBracketIndex, data)) {
					TagInterval i = TagInterval(openBracketIndex, closeBracketIndex, data);	
					m_tags.push_back(i);
				}
				haveOpenBracket = false;
			} else {
				openBracketIndex = *p;
			}
		} else {
			if(data[*p] == '>') {
				
			} else {
				openBracketIndex = *p;
				haveOpenBracket = true;
			}
		}
	}
}

int Styler_HtmlHL::FindClosingTag(const wxChar* data) {
	if(m_currentTag < 0) return -1;
	TagInterval currentTag = m_tags[m_currentTag];
	if(currentTag.isClosingTag) return -1;
	
	TagInterval* tag;
	int stack = 1;
	
	int i = 0;
	for (vector<TagInterval>::iterator p = m_tags.begin(); p != m_tags.end(); ++p) {
		tag = &(*p);
		if(tag->start > currentTag.end) {
			if(SameTag(*tag, currentTag, data)) {
				stack += tag->isClosingTag ? -1 : 1;
				if(stack == 0) return i;
			}
		}
		i++;
	}
	
	return -1;
}

int Styler_HtmlHL::FindCurrentTag() {
	int i = -1;
	for (vector<TagInterval>::iterator p = m_tags.begin(); p != m_tags.end(); ++p) {
		i++;
		if(p->end < m_cursorPosition) continue;
		if(p->start > m_cursorPosition) return -1;
		return i;
	}
	
	return -1;
}

bool Styler_HtmlHL::SameTag(TagInterval& openTag, TagInterval& closeTag, const wxChar* data) {
	int openIndex = openTag.start+1;
	int closeIndex = closeTag.start+1;
	wxChar c;
	
	//we only need to compare tag names, so <a> and </a> should both match
	if(openTag.isClosingTag) openIndex++;
	if(closeTag.isClosingTag) closeIndex++;
	
	while(true) {
		//we are guaranteed openIndex and closeIndex are less than the doc length because there must be a closing bracket in the tag to get in here
		c = data[openIndex];
		
		//once we hit non-alphanumber characters, then the tags have not differred so far, so it is valid
		if(!isAlphaNumeric(c)) return true;		
		
		if(c != data[closeIndex]) return false;
		
		openIndex++;
		closeIndex++;
	}
}

Styler_HtmlHL::TagInterval::TagInterval(unsigned int start, unsigned int end, const wxChar* data) :
 start(start), end(end), isClosingTag(data[start+1]=='/')
 {
}

void Styler_HtmlHL::Style(StyleRun& sr) {
	if(!initialParse) Reparse();

	if(m_closingTag >= 0) {
		const unsigned int rstart =  sr.GetRunStart();
		const unsigned int rend = sr.GetRunEnd();
		TagInterval closingTag = m_tags[m_closingTag];
		unsigned int start = closingTag.start+2, end = closingTag.end;
		
		if (start > rend) return;

		// Check for overlap (or zero-length sel at start-of-line)
		if ((end > rstart && start < rend) || (start == end && end == rstart)) {
			start = wxMax(rstart, start);
			end   = wxMin(rend, end);

			if(start > end) return;

			sr.SetBackgroundColor(start, end, m_searchHighlightColor);
			sr.SetShowHidden(start, end, true);
		}
	}
#if 0
	if(!ShouldStyle()) return;

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
#endif
}


void Styler_HtmlHL::Insert(unsigned int start, unsigned int length) {
	unsigned int end = start+length;
	int count = m_brackets.size();

	//update all the brackets to point to their new locations
	for(int c = 0; c < count; c++) {
		if(m_brackets[c] >= start) {
			m_brackets[c] += length;
		}
	}

	//do a search for any new brackets inside of the inserted text
	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();
		FindBrackets(start, end, data);

		//we always need to recreate the tags list from scratch
		//it is much to difficult to take care of all the cases that could result
		FindTags(data);
		m_currentTag = FindCurrentTag();
		m_closingTag = FindClosingTag(data);
	cxENDLOCK
}

void Styler_HtmlHL::Delete(unsigned int start, unsigned int end) {
	int count = m_brackets.size();
	int length = end - start;

	//update all the brackets to point to their new locations
	//remove any brackets that were inside the deleted text
	for(int c = 0; c < count; c++) {
		if(m_brackets[c] >= start) {
			if(m_brackets[c] < end) {
				m_brackets.erase(m_brackets.begin()+c);
				count--;
				c--;
			} else {
				m_brackets[c] -= length;
			}
		}
	}

	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();

		//we always need to recreate the tags list from scratch
		//it is much to difficult to take care of all the cases that could result
		FindTags(data);
		m_currentTag = FindCurrentTag();
		m_closingTag = FindClosingTag(data);
	cxENDLOCK
}

void Styler_HtmlHL::ApplyDiff(const std::vector<cxChange>& changes) {
	Reparse();
}