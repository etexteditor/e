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
#include "EditorCtrl.h"

Styler_Selections::Styler_Selections(const DocumentWrapper& rev, const Lines& lines, const tmTheme& theme, EditorCtrl& editorCtrl)
: m_doc(rev), m_lines(lines), m_editorCtrl(editorCtrl),
  m_theme(theme), m_searchHighlightColor(m_theme.searchHighlightColor),
  m_selectionHighlightColor(m_theme.selectionColor),
  m_enabled(false)
{
}

void Styler_Selections::Invalidate() {
	m_selections.clear();
	m_nextSelection = -1;
	m_enabled = false;
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
	m_editorCtrl.Select(m_selections[m_nextSelection].start, m_selections[m_nextSelection].end);
	m_editorCtrl.DrawLayout();
}

void Styler_Selections::PreviousSelection() {
	if(!m_enabled) return;

	m_nextSelection--;
	if(m_nextSelection < 0) {
		m_nextSelection = m_selections.size()-1;
	}

	m_editorCtrl.RemoveAllSelections();
	m_editorCtrl.SetPos(m_selections[m_nextSelection].start);
	m_editorCtrl.Select(m_selections[m_nextSelection].start, m_selections[m_nextSelection].end);
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
	unsigned int pos = m_editorCtrl.GetPos();
	if(start <= pos && end >= pos) {
		sr.SetBackgroundColor(start, end, m_selectionHighlightColor);
	} else {
		sr.SetBackgroundColor(start, end, m_searchHighlightColor);
	}
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
		} else if(end >= pos) {
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

	unsigned int length = end_pos - start_pos;
	unsigned int size = m_selections.size();

	for(unsigned int c = 0; c < size; c++) {
		unsigned int start = m_selections[c].start;
		unsigned int end = m_selections[c].end;

		if(start_pos > end) {
			//deleted text is all after, so ignore
		} else if(end_pos < start) {
			//deleted text is all before, so shift
			m_selections[c].start -= length;
			m_selections[c].end -= length;
		} else if(start_pos == start && end_pos == end) {
			//deleted all text in the selection, but don't get rid of the selection
			m_selections[c].end = start;
		} else if(start_pos >= start && end_pos <= end) {
			//deleted text in all inside the selection, so shift end
			m_selections[c].end -= length;
		} else {
			//deleted text in some way encompasses teh whole selection, so ditch it
			if(c <= m_nextSelection) {
				m_nextSelection--;
			}
			m_selections.erase(m_selections.begin()+c);
			c--;
			size--;
		}
	}

	m_enabled = m_selections.size() > 0;
}

void Styler_Selections::ApplyDiff(std::vector<cxChange>& WXUNUSED(changes)) {
	Invalidate();
}