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
	needReparse = true;
	needReparseTags = true;
	m_cursorPosition = m_lines.GetPos();
}

void Styler_HtmlHL::Clear() {
	Reparse();
}

void Styler_HtmlHL::Invalidate() {
	needReparse = true;
	needReparseTags = true;
	//This somtimes causes a segfault when opening a new document.
	//Setting needReparse to false will cause it to call Reparse when Style is called next.
	//Reparse();
}

bool Styler_HtmlHL::ShouldStyle() {
	bool shouldStyle = false;
	m_settings.GetSettingBool(wxT("highlightHtml"), shouldStyle);

	//force the styler to reparse the whole document the next time it is enabled
	if(!shouldStyle) {
		needReparse = true;
		needReparseTags = true;
	}
	return shouldStyle;
}

void Styler_HtmlHL::Reparse() {
    //Reparse finds every bracket (< and >) in the document and then determines which of those are valid tags.
    //It has to do a full scan of the document to do this though.
    
	needReparse = false;
	needReparseTags = true;

	cxLOCKDOC_READ(m_doc)
		FindAllBrackets(doc);
	cxENDLOCK
}

void Styler_HtmlHL::UpdateCursorPosition() {
    //m_editorCtrl.GetPos cannot be called from the insert/delete methods.  EditorCtrl has not update pos yet, so pos will always be incorrect at that stage.
    //Instead, we just need to check it every time we style or select parent content.
    //If the position has changed, then we need to find which tag contains the cursor.  If the document needs to be reparsed, then we need to find the tags as well.

	unsigned int pos = m_editorCtrl.GetPos();
	if(pos == m_cursorPosition && !needReparseTags) return;

	m_cursorPosition = pos;
	cxLOCKDOC_READ(m_doc)
	    if(needReparseTags) FindTags(doc);
		m_currentTag = FindCurrentTag();
		m_matchingTag = FindMatchingTag(doc, m_currentTag);
	cxENDLOCK
	
    needReparseTags = false;
}

void Styler_HtmlHL::SelectParentTag() {
	//The Insert/Delete methods return if the highlight html option is not checked.
	//This feature can still work without that, and it probably should.  However, we need to parse the doc to find the tags first.
	if(!ShouldStyle()) Reparse();
	UpdateCursorPosition();

	cxLOCKDOC_READ(m_doc)
		int closingTag, openingTag;

		vector<interval> selections = m_editorCtrl.GetSelections();
		if(selections.size() == 0) {
			//nothing is currently selected, so lets just find the parent tag 
			closingTag = FindParentClosingTag(m_cursorPosition);
			if(closingTag < 0) return;

			openingTag = FindMatchingTag(doc, closingTag);
			if(openingTag < 0) return;

		} else {
			interval first = selections[0];
			closingTag = FindParentClosingTag(first.end);
			if(closingTag < 0) return;

			openingTag = FindMatchingTag(doc, closingTag);
			if(openingTag < 0) return;

			//check if the selection matches the content of the current tag pair.  if it does, then select the parent tag
			if(first.start == m_tags[openingTag].end+1 && first.end == m_tags[closingTag].start) {
				closingTag = FindParentClosingTag(m_tags[closingTag].end);
				if(closingTag < 0) return;
				openingTag = FindMatchingTag(doc, closingTag);
				if(openingTag < 0) return;
			}
		}
		m_editorCtrl.Select(m_tags[openingTag].end+1, m_tags[closingTag].start);
	cxENDLOCK
}

void Styler_HtmlHL::FindAllBrackets(const Document& doc) {
	m_brackets.clear();
	FindBrackets(0, doc.GetLength(), doc);
}

bool Styler_HtmlHL::FindBrackets(unsigned int start, unsigned int end, const Document& doc) {
	vector<unsigned int> buffer;
	bool foundBracket = false;
	
	//If there are no brackets in the inserted text, then we dont need to do anything
	wxChar chr;
	for(unsigned int c = start; c < end; c++) {
		chr = doc.GetChar(c);
		if(chr == '<' || chr == '>') {
			foundBracket = true;
			break;
		}
	}
	if(!foundBracket) return false;

	//copy the existing brackets to a temporary array so we can add them in order, in linear time
	unsigned int index = 0;
	for(index = 0; index < m_brackets.size(); index++) {
		buffer.push_back(m_brackets[index]);
	}
	index = 0;
	m_brackets.clear();
	
	for(unsigned int c = start; c < end; c++) {
		switch(doc.GetChar(c)) {
			case '<':
			case '>':
				//now we have the index of a bracket inside the search range
			
				//add any items in buffer that are before this bracket
				for(; index < buffer.size(); index++) {
					if(buffer[index] < c) {
						m_brackets.push_back(buffer[index]);
					} else {
						index--;
						break;
					}
				}
			
				//now add this bracket
				m_brackets.push_back(c);
				foundBracket = true;
				
			break;
		}
	}

	//add any existing brackets that occur after the last bracket from the new text
	for(; index < buffer.size(); index++) {
		m_brackets.push_back(buffer[index]);
	}
	
	return true;
}

bool Styler_HtmlHL::IsValidTag(unsigned int start, unsigned int end, const Document& doc) {
	start++;
	
	//if it starts with a slash, it might be valid
	if(doc.GetChar(start) == '/') start++;
	
	//if there is no tag name, it is not a tag
	if(start == end) return true;
		
	//if it starts with an alphabetic character, then it is prolly valid
	if(isAlphaNumeric(doc.GetChar(start))) return true;
	
	//TODO: comments
	
	return false;
}

bool Styler_HtmlHL::IsOpenComment(const Document& doc, unsigned int bracket) {
	//<!--
	if(bracket+3 >= doc.GetLength()) return false;
	return doc.GetChar(bracket+1) == '!' && doc.GetChar(bracket+2) == '-' && doc.GetChar(bracket+3) == '-';
}

bool Styler_HtmlHL::IsCloseComment(const Document& doc, unsigned int bracket) {
	//-->
	if(bracket-2 < 0) return false;
	return doc.GetChar(bracket-1) == '-' && doc.GetChar(bracket-2) == '-';
}

//when inserting/removing a character, i should be able to ignore any brackets before the insertion, i should be able to just add those tags right back in to m_tags
void Styler_HtmlHL::FindTags(const Document& doc) {
	m_tags.clear();
	bool haveOpenBracket = false, inComment = false;;
	int openBracketIndex = -1, closeBracketIndex = -1, size = (int)m_brackets.size(), index;
	
	for(int c = 0; c < size; c++) {
		index = m_brackets[c];
		if(inComment) {
			if(doc.GetChar(index) == '>' && IsCloseComment(doc, index)) {
				inComment = false;
				haveOpenBracket = false;
			}
			continue;
		}

		if(haveOpenBracket) {
			if(doc.GetChar(index) == '>') {
				closeBracketIndex = index;
				if(IsValidTag(openBracketIndex, closeBracketIndex, doc)) {
					TagInterval i = TagInterval(openBracketIndex, closeBracketIndex, doc);	
					m_tags.push_back(i);
				}
				haveOpenBracket = false;
			} else {
				openBracketIndex = index;
				inComment = IsOpenComment(doc, index);
			}
		} else {
			if(doc.GetChar(index) == '>') {
			} else {
				openBracketIndex = index;
				haveOpenBracket = true;
				inComment = IsOpenComment(doc, index);
			}
		}
	}
}

int Styler_HtmlHL::FindMatchingTag(const Document& doc, int tag) {
	if(tag < 0) return -1;
	TagInterval currentTag = m_tags[tag];
	int stack = 1, size = (int)m_tags.size();

	if(currentTag.isClosingTag) {
		//search in reverse to find the matching opening tag	
		for(int c = tag-1; c >= 0; c--) {
			if(SameTag(m_tags[c], currentTag, doc)) {
				stack += m_tags[c].isClosingTag ? 1 : -1;
				if(stack == 0) return c;
			}
		}
	} else {
		//search forward to find the matching closing tag
		for(int c = tag+1; c < size; c++) {
			if(SameTag(m_tags[c], currentTag, doc)) {
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
	for(int c = 0; c < size; c++) {
		if(m_tags[c].end < m_cursorPosition) continue;
		if(m_tags[c].start > m_cursorPosition) return -1;
		return c;
	}
	return -1;
}

bool Styler_HtmlHL::SameTag(TagInterval& openTag, TagInterval& closeTag, const Document& doc) {
	int openIndex = openTag.start+1;
	int closeIndex = closeTag.start+1;
	wxChar start, end;
	
	//we only need to compare tag names, so <a> and </a> should both match
	if(openTag.isClosingTag) openIndex++;
	if(closeTag.isClosingTag) closeIndex++;
	
	while(true) {
		//we are guaranteed openIndex and closeIndex are less than the doc length because there must be a closing bracket in the tag to get in here
		start = doc.GetChar(openIndex);
		if(start >= 'A' && start <= 'Z') {
		    start -= (wxChar)('A' - 'a');
		}
		
		end = doc.GetChar(closeIndex);
		if(end >= 'A' && end <= 'Z') {
		    end -= (wxChar)('A' - 'a');
		}
		
		//once we hit non-alphanumber characters, then the tags have not differred so far, so it is valid
		if(!isAlphaNumeric(start)) {
			//save the position of the end of the tag name so we dont have to recompute it when we highlight the tag later
			openTag.tagNameEnd = openIndex;

			if(!isAlphaNumeric(end)) {
				closeTag.tagNameEnd = closeIndex;
				return true;
			}

			return false;
		}
		if(start != end ) return false;
		
		openIndex++;
		closeIndex++;
	}
}

Styler_HtmlHL::TagInterval::TagInterval(unsigned int start, unsigned int end, const Document& doc) :
 start(start), end(end), tagNameEnd(0)
 {
	 isSelfClosingTag = doc.GetChar(end-1) == '/';
	 isClosingTag = isSelfClosingTag || doc.GetChar(start+1) == '/';
}

void Styler_HtmlHL::Style(StyleRun& sr) {
	if(!ShouldStyle()) return;

	if(needReparse) Reparse();
	UpdateCursorPosition();

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
	//wxLogDebug(wxT("INSERT: %d %d %d"), start, length, m_cursorPosition);
	if(!ShouldStyle()) return;

	if(needReparse) return;

	unsigned int end = start+length;
	int count = m_brackets.size();

	//update all the brackets to point to their new locations
	for(int c = 0; c < count; c++) {
		if(m_brackets[c] >= start) {
			m_brackets[c] += length;
		}
	}
	
	bool foundBrackets = false;
	//do a search for any new brackets inside of the inserted text
	cxLOCKDOC_READ(m_doc)
		foundBrackets = FindBrackets(start, end, doc);
	cxENDLOCK
	
	if(foundBrackets) {
		needReparse = true;
	} else {
		if(!needReparseTags) {
			int size = (int) m_tags.size();
			for(int c = 0; c < size; c++) {
				//The start and end brackets should be able to just be adjusted based on the length as long as no other brackets were inserted
				if(m_tags[c].start >= start) {
					m_tags[c].start += length;
					m_tags[c].tagNameEnd = 0;
				}
				if(m_tags[c].end >= start) {
					m_tags[c].end += length;
					m_tags[c].tagNameEnd = 0;
				}
			}
		}
	}
}

void Styler_HtmlHL::Delete(unsigned int start, unsigned int end) {
	//wxLogDebug(wxT("DELETE:  %d %d %d"), start, end, m_cursorPosition);
	if(!ShouldStyle()) return;

	if(needReparse) return;

	int count = m_brackets.size();
	int length = end - start;

	//update all the brackets to point to their new locations
	//remove any brackets that were inside the deleted text
	bool erasedBracket = false;
	for(int c = 0; c < count; c++) {
		if(m_brackets[c] >= start) {
			if(m_brackets[c] < end) {
				m_brackets.erase(m_brackets.begin()+c);
				erasedBracket = true;
				count--;
				c--;
			} else {
				m_brackets[c] -= length;
			}
		}
	}
	
	if(!erasedBracket && !needReparseTags) {
		int size = (int) m_tags.size();
		for(int c = 0; c < size; c++) {
			if(m_tags[c].start >= end) {
				m_tags[c].start -= length;
				m_tags[c].tagNameEnd = 0;
			}
			if(m_tags[c].end >= end) {
				m_tags[c].end -= length;
				m_tags[c].tagNameEnd = 0;
			}
		}
	} else {
		needReparse = true;
	}
}

void Styler_HtmlHL::ApplyDiff(const std::vector<cxChange>& WXUNUSED(changes)) {
	Reparse();
}