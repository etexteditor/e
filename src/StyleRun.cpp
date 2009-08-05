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

#include "StyleRun.h"

// Initialize static vars
std::map<int,wxFont> StyleRun::s_fontCache;

StyleRun::StyleRun(const tmTheme& theme, FastDC& dc):
	m_theme(theme), m_dc(dc), m_enableBold(true), m_printMode(false)
{
	// Initialize default style
	m_default_style.foregroundcolor = &m_theme.foregroundColor;
	m_default_style.backgroundcolor = NULL;
	m_default_style.fontStyle = wxFONTFLAG_DEFAULT;
	m_default_style.show_hidden = false;
}

void StyleRun::SetPrintMode() {
	m_default_style.foregroundcolor = wxBLACK;
	m_default_style.backgroundcolor = NULL;
	m_default_style.fontStyle = wxFONTFLAG_DEFAULT;
	m_default_style.show_hidden = false;
	m_printMode = true;
}

void StyleRun::Init(unsigned int start, unsigned int end) {
	if (!m_printMode) {
		// Update default style (theme may have changed)
		m_default_style.foregroundcolor = &m_theme.foregroundColor;
	}

	// Initialize run (fully spanned by default style)
	m_styles.clear();
	m_styles.push_back(m_default_style);
	m_styles[0].start = start;
	m_styles[0].end = end;

	m_extendBgColor = wxNullColour;
}

unsigned int StyleRun::GetStyleAtPos(unsigned int pos) const {
	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		const StyleSR& sr = m_styles[i];
		if (sr.start <= pos && pos < sr.end) return i;
	}

	wxASSERT(false);
	return 0;
}

void StyleRun::ApplyStyle(unsigned int style_id) const {
	wxASSERT(style_id >= 0 && style_id < m_styles.size());

	const StyleSR& s = m_styles[style_id];

	// Foreground Color
	if (s.foregroundcolor) {
		m_dc.SetTextForeground(*s.foregroundcolor);
	}

	// Background Color
	if (s.backgroundcolor && *s.backgroundcolor == m_theme.backgroundColor) {
		m_dc.SetBackgroundMode(wxTRANSPARENT);
	}
	else if (s.backgroundcolor) {
		m_dc.SetTextBackground(*s.backgroundcolor);
		m_dc.SetBackgroundMode(wxSOLID);
	}
	else {
		m_dc.SetTextBackground(wxNullColour);
		m_dc.SetBackgroundMode(wxTRANSPARENT);
	}

	// Font styles
	ApplyFontStyle(s.fontStyle);
}

void StyleRun::ResetStyle() const {
	m_dc.SetTextForeground(*wxBLACK);
	m_dc.SetTextBackground(wxNullColour);
	m_dc.SetBackgroundMode(wxTRANSPARENT);

	// Set all font flags to normal
	m_dc.SetFontStyles(wxFONTFLAG_DEFAULT, s_fontCache);
}

bool StyleRun::DoExtendBgAtPos(unsigned int pos) const {
	wxASSERT(pos <= m_styles.back().end);
	if (!m_extendBgColor.Ok()) return false;
	if (m_extendBgColor == m_theme.backgroundColor) return false;

	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		const StyleSR& sr = m_styles[i];

		if (pos >= sr.start && pos < sr.end) {
			const wxColour& bgcolor = *sr.backgroundcolor;
			if (bgcolor.Ok() && bgcolor == m_extendBgColor) return true;
			break;
		}
	}

	return false;
}

void StyleRun::SetForegroundColor(unsigned int start, unsigned int end, const wxColour& color) {
	wxASSERT(start >= m_styles[0].start && start < m_styles.back().end);
	wxASSERT(end >= start && end <= m_styles.back().end);

	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		if (start == m_styles[i].start) {

			if (m_styles[i].end <= end) {
				m_styles[i].foregroundcolor = &color;

				start = m_styles[i].end;
				if (start == end) break;
			}
			else {
				// We have to split the style (and change first part)
				m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
				m_styles[i].end = end;
				m_styles[i].foregroundcolor = &color;
				m_styles[i+1].start = end;
				break;
			}
		}
		else if (start < m_styles[i].end) {
			// We have to split the style (and change last part in next round)
			m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
			m_styles[i].end = start;
			m_styles[i+1].start = start;
			continue;
		}
	}
}

void StyleRun::SetBackgroundColor(unsigned int start, unsigned int end, const wxColour& color) {
	wxASSERT(start >= m_styles[0].start && start <= m_styles.back().end);
	wxASSERT(end >= start && end <= m_styles.back().end);

	// We might have a zero-length selection at end
	if (start == m_styles.back().end) {
		wxASSERT(start == end);
		m_styles.insert(m_styles.end(), m_styles.back());
		m_styles.back().start = start;
		m_styles.back().backgroundcolor = &color;
		return;
	}

	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		StyleSR& s = m_styles[i];

		if (start == s.start) {

			if (s.end <= end) {
				// style fully spanned
				s.backgroundcolor = &color;

				start = s.end;
				if (start == end) break;
			}
			else {
				// We have to split the style (and change first part)
				m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
				m_styles[i].end = end;
				m_styles[i].backgroundcolor = &color;
				m_styles[i+1].start = end;
				break;
			}
		}
		else if (start < s.end) {
			// We have to split the style (and change last part in next round)
			m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
			m_styles[i].end = start;
			m_styles[i+1].start = start;
			continue;
		}
	}
}

void StyleRun::SetFontStyle(unsigned int start, unsigned int end, int fontStyle) {
	wxASSERT(start >= m_styles[0].start && start < m_styles.back().end);
	wxASSERT(end >= start && end <= m_styles.back().end);

	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		if (start == m_styles[i].start) {

			if (m_styles[i].end <= end) {
				// style fully spanned
				m_styles[i].fontStyle = fontStyle;

				start = m_styles[i].end;
				if (start == end) break;
			}
			else {
				// We have to split the style (and change first part)
				m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
				m_styles[i].end = end;
				m_styles[i].fontStyle = fontStyle;
				m_styles[i+1].start = end;
				break;
			}
		}
		else if (start < m_styles[i].end) {
			// We have to split the style (and change last part in next round)
			m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
			m_styles[i].end = start;
			m_styles[i+1].start = start;
			continue;
		}
	}
}

void StyleRun::SetShowHidden(unsigned int start, unsigned int end, bool do_show) {
	wxASSERT(start >= m_styles[0].start && start < m_styles.back().end);
	wxASSERT(end >= start && end <= m_styles.back().end);

	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		if (start == m_styles[i].start) {

			if (m_styles[i].end <= end) {
				// style fully spanned
				m_styles[i].show_hidden = do_show;

				start = m_styles[i].end;
				if (start == end) break;
			}
			else {
				// We have to split the style (and change first part)
				m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
				m_styles[i].end = end;
				m_styles[i].show_hidden = do_show;
				m_styles[i+1].start = end;
				break;
			}
		}
		else if (start < m_styles[i].end) {
			// We have to split the style (and change last part in next round)
			m_styles.insert(m_styles.begin()+i+1, m_styles[i]);
			m_styles[i].end = start;
			m_styles[i+1].start = start;
			continue;
		}
	}
}

void StyleRun::Print() const{
	for (unsigned int i = 0; i < m_styles.size(); ++i) {
		wxLogDebug(wxT("%u: start:%u end:%u show:%d"), i, m_styles[i].start, m_styles[i].end, m_styles[i].show_hidden);
	}
}
