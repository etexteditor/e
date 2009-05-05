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

#include "LineListNoWrap.h"
#include "Document.h"
#include "FixedLine.h"

LineListNoWrap::LineListNoWrap(FixedLine& l, const DocumentWrapper& dw)
: m_line(l), m_doc(dw), m_maxWidth(0) {
}

bool LineListNoWrap::IsValidIndex(unsigned int index) const {
	return (index < m_textOffsets.size());
}

unsigned int LineListNoWrap::offset(unsigned int index) {
	wxASSERT(IsValidIndex(index));
	return index ? m_textOffsets[index-1] : 0;
}

unsigned int LineListNoWrap::end(unsigned int index) {
	wxASSERT(IsValidIndex(index));
	return m_textOffsets[index];
}

unsigned int LineListNoWrap::top(unsigned int index) {
	wxASSERT(IsValidIndex(index));
	return index ? index * m_line.GetCharHeight() : 0;
}

unsigned int LineListNoWrap::bottom(unsigned int index) {
	wxASSERT(IsValidIndex(index));
	return (index+1) * m_line.GetCharHeight();
}

unsigned int LineListNoWrap::size() const {
	return m_textOffsets.size();
}

unsigned int LineListNoWrap::last() const {
	wxASSERT(!m_textOffsets.empty());
	return m_textOffsets.size()-1;
}

unsigned int LineListNoWrap::height() const {
	return m_textOffsets.size() * m_line.GetCharHeight();
}

unsigned int LineListNoWrap::length() const {
	return (!m_textOffsets.empty()) ? m_textOffsets.back() : 0;
}

bool LineListNoWrap::IsLineEnd(unsigned int pos) {
	if (!size()) return false;
	wxASSERT(pos <= m_textOffsets.back());

	vector<unsigned int>::const_iterator posline = lower_bound(m_textOffsets.begin(), m_textOffsets.end(), pos);
	return (pos == *posline);
}

unsigned int LineListNoWrap::EndFromPos(unsigned int pos) {
	if (!size()) return 0;
	wxASSERT(pos <= m_textOffsets.back());

	vector<unsigned int>::const_iterator posline = lower_bound(m_textOffsets.begin(), m_textOffsets.end(), pos);
	if (pos != *posline) return *posline;
	return pos == m_textOffsets.back() ? pos : *(++posline);
}

unsigned int LineListNoWrap::StartFromPos(unsigned int pos) {
	if (!size()) return 0;
	wxASSERT(pos <= m_textOffsets.back());

	vector<unsigned int>::const_iterator posline = lower_bound(m_textOffsets.begin(), m_textOffsets.end(), pos);
	if (pos == *posline) return pos;
	return (posline == m_textOffsets.begin()) ? 0 : *(--posline);
}

vector<unsigned int>& LineListNoWrap::GetOffsets() {
	return m_textOffsets;
}

void LineListNoWrap::SetOffsets(const vector<unsigned int>& offsets) {
	m_textOffsets = offsets;
	NewOffsets();
}

// You have to call this after textOffsets heve been modified using GetOffsets()
void LineListNoWrap::NewOffsets() {
	m_lineWidths.resize(m_textOffsets.size());

	// Get all line widths
	m_maxWidth = 0;
	unsigned int lineStart = 0;
	for (unsigned int i = 0; i < m_textOffsets.size(); ++i) {
		const unsigned int lineWidth = m_line.GetQuickLineWidth(lineStart, m_textOffsets[i]);

		m_lineWidths[i] = lineWidth;
		lineStart = m_textOffsets[i];
		if (lineWidth > m_maxWidth) m_maxWidth = lineWidth;
	}
}

void LineListNoWrap::insert(unsigned int index, int newend) {
	wxASSERT(index >= 0 && index <= m_textOffsets.size());
	wxASSERT(newend <= (int)m_doc.GetLength());

	// Insert the text
	m_textOffsets.insert(m_textOffsets.begin()+index, newend);

	// Update the following offsets
	const unsigned int diff = newend - (index ? m_textOffsets[index-1] : 0);
	for (vector<unsigned int>::iterator p = m_textOffsets.begin()+index+1; p != m_textOffsets.end(); ++p) {
		*p += diff;
	}

	// Width of new line depends on syntax and theme
	// so just set it to 0 for now, it will be updated when parsed
	m_lineWidths.insert(m_lineWidths.begin()+index, 0);
}

void LineListNoWrap::insertlines(unsigned int index, vector<unsigned int>& newlines) {
	wxASSERT(index >= 0 && index <= m_textOffsets.size());
	wxASSERT(!newlines.empty());

	// Insert the lines
	m_textOffsets.insert(m_textOffsets.begin()+index, newlines.begin(), newlines.end());

	// Update the following offsets
	const unsigned int diff = newlines.back() - (index ? m_textOffsets[index-1] : 0);
	for (vector<unsigned int>::iterator p = m_textOffsets.begin() + index + newlines.size(); p != m_textOffsets.end(); ++p) {
		*p += diff;
	}

	// Insert width of the new lines
	m_lineWidths.insert(m_lineWidths.begin()+index, newlines.size(), 0);
	const unsigned int newlinesEnd = index + newlines.size();

	// Width of new lines depends on syntax and theme
	// so just set it to 0 for now, it will be updated when parsed
	for (unsigned int i = index; i < newlinesEnd; ++i) {
		m_lineWidths[i] = 0;
	}
}

void LineListNoWrap::update(unsigned int index, unsigned int newend) {
	wxASSERT(IsValidIndex(index));
	wxASSERT(newend <= m_doc.GetLength());

	// Calc the change
	const int tdiff = newend - m_textOffsets[index];

	// Update the changed and following offsets
	for (vector<unsigned int>::iterator p = m_textOffsets.begin()+index; p != m_textOffsets.end(); ++p) {
		*p += tdiff;
	}

	// Width of new line depends on syntax and theme
	// so leave it for now, it will be updated when parsed
}

void LineListNoWrap::update_parsed_line(unsigned int index) {
	wxASSERT(IsValidIndex(index));

	m_line.SetLine((index ? m_textOffsets[index-1] : 0), m_textOffsets[index]);
	const unsigned int oldWidth = m_lineWidths[index];
	const unsigned int lineWidth = m_line.GetUnwrappedWidth();
	m_lineWidths[index] = lineWidth;

	// Calc maxWidth
	if (lineWidth > m_maxWidth) m_maxWidth = lineWidth;
	else if (oldWidth == m_maxWidth) {
		// If this line could have been the widest, we have
		// to calc m_maxWidth against all lines.
		m_maxWidth = *max_element(m_lineWidths.begin(), m_lineWidths.end());
	}
}

void LineListNoWrap::update_line_extent(unsigned int index, unsigned int extent) {
	wxASSERT(IsValidIndex(index));
	if (extent == m_lineWidths[index]) return; // no change

	const unsigned int oldWidth = m_lineWidths[index];
	m_lineWidths[index] = extent;

	// Calc maxWidth
	if (extent > m_maxWidth) m_maxWidth = extent;
	else if (oldWidth == m_maxWidth) {
		// If this line could have been the widest, we have
		// to calc m_maxWidth against all lines.
		m_maxWidth = *max_element(m_lineWidths.begin(), m_lineWidths.end());
	}
}

void LineListNoWrap::remove(unsigned int startline, unsigned int endline) {
	wxASSERT(startline >= 0 && startline < m_textOffsets.size());
	wxASSERT(endline > startline && endline <= m_textOffsets.size());
	wxASSERT(size());

	const unsigned int maxWidth = *max_element(m_lineWidths.begin()+startline, m_lineWidths.begin()+endline);

	// Remove text lines
	const int diff = offset(startline) - end(endline-1);
	m_textOffsets.erase(m_textOffsets.begin()+startline, m_textOffsets.begin()+endline);
	m_lineWidths.erase(m_lineWidths.begin()+startline, m_lineWidths.begin()+endline);

	if (m_textOffsets.empty()) {
		m_maxWidth = 0;
		return;
	}

	// Update the following offsets
	for (vector<unsigned int>::iterator p = m_textOffsets.begin()+startline; p != m_textOffsets.end(); ++p) {
		*p += diff;
	}

	// If one of the lines could have been the widest,
	// we have to calc m_maxWidth against all lines.
	if (maxWidth == m_maxWidth) {
		m_maxWidth = *max_element(m_lineWidths.begin(), m_lineWidths.end());
	}
}

void LineListNoWrap::clear() {
	m_textOffsets.clear();
	m_lineWidths.clear();
	m_maxWidth = 0;
}

int LineListNoWrap::find_offset(int pos) {
	if (!size()) return 0;
	wxASSERT(pos >= 0 && pos <= (int)m_textOffsets.back());

	vector<unsigned int>::iterator posline = lower_bound(m_textOffsets.begin(), m_textOffsets.end(), (unsigned int)pos);
	return distance(m_textOffsets.begin(), posline);
}

int LineListNoWrap::find_ypos(unsigned int ypos) {
	wxASSERT(ypos >= 0 && (ypos <= height() || ypos == 0));

	if (ypos >= height()) return last();
	else return ypos / m_line.GetCharHeight();
}

void LineListNoWrap::invalidate(int WXUNUSED(index)) {
	NewOffsets(); // to recalculate m_maxWidth
}

void LineListNoWrap::Print() {
	wxLogDebug(wxT("\nLineList len=%d"), size());
	wxLogDebug(wxT(" m_lineHeight: %u"), m_line.GetCharHeight());
	wxLogDebug(wxT(" height:     %u"), height());

	for (unsigned int i = 0; i < size(); ++i) {
		wxLogDebug(wxT("  %u: %u %u"), i, m_textOffsets[i], m_lineWidths[i]);
	}
}

void LineListNoWrap::verify(bool WXUNUSED(deep)) const {
	wxASSERT(m_textOffsets.size() == m_lineWidths.size());
}
