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
#include "EditorCtrl.h"

inline bool isAlphaNumeric(wxChar c) {
#ifdef __WXMSW__
	return ::IsCharAlphaNumeric(c) != 0;
#else
	return wxIsalnum(c);
#endif
}

/**
 * This features highlights matching html tags in a document.
 * To do this efficiently, it maintains a list of all the brackets (< and >) in a document.
 * When text is inserted or deleted, only the changed brackets must be added/removed to the list,
 * thus avooiding a costly full document scan.
 * Each time the document is changed, this list of brackets is scanned to find pairs that form a valid tag.
 * When the cursor moves in any way, only the list of tags needs to be scanned to find 
 * the tag that is currently in focus, and its matching tag, if any.
 */
Styler_HtmlHL::Styler_HtmlHL(const DocumentWrapper& rev, const Lines& lines, const tmTheme& theme, eSettings& settings, EditorCtrl& editorCtrl)
: m_doc(rev), m_lines(lines), m_theme(theme), m_settings(settings), m_editorCtrl(editorCtrl),
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
	initialParse = false;
	//This causes a segfault when opening a new document.
	//Setting initialParse to false will cause it to call Reparse when Style is called next.
	//Reparse();
}

bool Styler_HtmlHL::ShouldStyle() {
	bool shouldStyle = false;
	m_settings.GetSettingBool(wxT("highlightHtml"), shouldStyle);

	//force the styler to reparse the whole document the next time it is enabled
	if(!shouldStyle) initialParse = false;
	return shouldStyle;
}

void Styler_HtmlHL::Reparse() {
	initialParse = true;

	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();
		FindAllBrackets(data);
		FindTags(data);
		m_currentTag = FindCurrentTag();
		m_matchingTag = FindMatchingTag(data, m_currentTag);
	cxENDLOCK
}

void Styler_HtmlHL::UpdateCursorPosition(unsigned int pos) {
	m_cursorPosition = pos;
	m_currentTag = FindCurrentTag();
	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();
		m_matchingTag = FindMatchingTag(data, m_currentTag);
	cxENDLOCK
}

void Styler_HtmlHL::SelectParentTag() {
	//The Insert/Delete methods return if the highlight html option is not checked.
	//This feature can still work without that, and it probably should.  However, we need to parse the doc to find the tags first.
	if(!ShouldStyle()) Reparse();

	cxLOCKDOC_READ(m_doc)
		wxString text = doc.GetText();
		const wxChar* data = text.c_str();
		int closingTag, openingTag;

		vector<interval> selections = m_editorCtrl.GetSelections();
		if(selections.size() == 0) {
			//nothing is currently selected, so lets just find the parent tag 
			closingTag = FindParentClosingTag(m_cursorPosition);
			if(closingTag < 0) return;

			openingTag = FindMatchingTag(data, closingTag);
			if(openingTag < 0) return;

		} else {
			interval first = selections[0];
			closingTag = FindParentClosingTag(first.end);
			if(closingTag < 0) return;

			openingTag = FindMatchingTag(data, closingTag);
			if(openingTag < 0) return;

			//check if the selection matches the content of the current tag pair.  if it does, then select the parent tag
			if(first.start == m_tags[openingTag].end+1 && first.end == m_tags[closingTag].start) {
				closingTag = FindParentClosingTag(m_tags[closingTag].end);
				if(closingTag < 0) return;
				openingTag = FindMatchingTag(data, closingTag);
				if(openingTag < 0) return;
			}
		}
		m_editorCtrl.Select(m_tags[openingTag].end+1, m_tags[closingTag].start);
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
	for(unsigned int c = start; c < end; ++c) {
		switch(data[c]) {
			case '<':
			case '>':
				//now we have the index of a bracket inside the search range
			
				//add any items in buffer that are before this bracket
				for(; index < buffer.size(); ++index) {
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

	//add any existing brackets that occur after the last bracket from the new text
	for(; index < buffer.size(); ++index) {
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
	bool haveOpenBracket = false;
	int openBracketIndex = -1, closeBracketIndex = -1, size = (int)m_brackets.size(), index;
	
	//copy the existing brackets to a temporary array so we can add them in order, in linear time
	for(int c = 0; c < size; ++c) {
		index = m_brackets[c];
		if(haveOpenBracket) {
			if(data[index] == '>') {
				closeBracketIndex = index;
				if(IsValidTag(openBracketIndex, closeBracketIndex, data)) {
					TagInterval i = TagInterval(openBracketIndex, closeBracketIndex, data);	
					m_tags.push_back(i);
				}
				haveOpenBracket = false;
			} else {
				openBracketIndex = index;
			}
		} else {
			if(data[index] == '>') {
				
			} else {
				openBracketIndex = index;
				haveOpenBracket = true;
			}
		}
	}
}

int Styler_HtmlHL::FindMatchingTag(const wxChar* data, int tag) {
	if(tag < 0) return -1;
	TagInterval currentTag = m_tags[tag];
	int stack = 1, size = (int)m_tags.size();

	if(currentTag.isClosingTag) {
		//search in reverse to find the matching opening tag	
		for(int c = tag-1; c >= 0; --c) {
			if(SameTag(m_tags[c], currentTag, data)) {
				stack += m_tags[c].isClosingTag ? 1 : -1;
				if(stack == 0) return c;
			}
		}
	} else {
		//search forward to find the matching closing tag
		for(int c = tag+1; c < size; ++c) {
			if(SameTag(m_tags[c], currentTag, data)) {
				stack += m_tags[c].isClosingTag ? -1 : 1;
				if(stack == 0) return c;
			}
		}
	}
	
	return -1;
}

int Styler_HtmlHL::FindParentClosingTag(unsigned int searchPosition) {
	int stack = 1;
	int size = (int)m_tags.size();

	for(int c = 0; c < size; c++) {
		//scan until we find tags that are after the cursor's position
		if(m_tags[c].start < searchPosition) continue;
		if(m_tags[c].isSelfClosingTag) continue;
		if(m_tags[c].isClosingTag) {
			stack--;
			if(stack == 0) return c;
		} else {
			stack++;
		}
	}
	return -1;
}

int Styler_HtmlHL::FindCurrentTag() {
	int size = (int) m_tags.size();
	for(int c = 0; c < size; ++c) {
		if(m_tags[c].end < m_cursorPosition) continue;
		if(m_tags[c].start > m_cursorPosition) return -1;
		return c;
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
		if(!isAlphaNumeric(c)) {
			//save the position of the end of the tag name so we dont have to recompute it when we highlight the tag later
			openTag.tagNameEnd = openIndex;
			closeTag.tagNameEnd = closeIndex;
			return true;		
		}
		
		if(c != data[closeIndex]) return false;
		
		openIndex++;
		closeIndex++;
	}
}

Styler_HtmlHL::TagInterval::TagInterval(unsigned int start, unsigned int end, const wxChar* data) :
 start(start), end(end), tagNameEnd(0)
 {
	 isSelfClosingTag = data[end-1] == '/';
	 isClosingTag = isSelfClosingTag || data[start+1]=='/';
}

void Styler_HtmlHL::Style(StyleRun& sr) {
	if(!ShouldStyle()) return;

	if(!initialParse) Reparse();

	if(m_matchingTag >= 0) {
		const unsigned int rstart =  sr.GetRunStart();
		const unsigned int rend = sr.GetRunEnd();
		TagInterval closingTag = m_tags[m_matchingTag];
		unsigned int start = closingTag.start+2, end = closingTag.end;
		if(!closingTag.isClosingTag) {
			start = closingTag.start + 1;
			end = closingTag.tagNameEnd;
		}
		
		if (start > rend) return;

		// Check for overlap (or zero-length sel at start-of-line)
		if ((end > rstart && start < rend) || (start == end && end == rstart)) {
			start = wxMax(rstart, start);
			end = wxMin(rend, end);

			if(start > end) return;

			sr.SetBackgroundColor(start, end, m_searchHighlightColor);
			sr.SetShowHidden(start, end, true);
		}
	}
}


void Styler_HtmlHL::Insert(unsigned int start, unsigned int length) {
	if(!ShouldStyle()) return;

	if(!initialParse) {
		Reparse();
		return;
	}

	unsigned int end = start+length;
	int count = m_brackets.size();

	//update all the brackets to point to their new locations
	for(int c = 0; c < count; ++c) {
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
		m_matchingTag = FindMatchingTag(data, m_currentTag);
	cxENDLOCK
}

void Styler_HtmlHL::Delete(unsigned int start, unsigned int end) {
	if(!ShouldStyle()) return;

	if(!initialParse) {
		Reparse();
		return;
	}

	int count = m_brackets.size();
	int length = end - start;

	//update all the brackets to point to their new locations
	//remove any brackets that were inside the deleted text
	for(int c = 0; c < count; ++c) {
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
		m_matchingTag = FindMatchingTag(data, m_currentTag);
	cxENDLOCK
}

void Styler_HtmlHL::ApplyDiff(const std::vector<cxChange>& WXUNUSED(changes)) {
	Reparse();
}