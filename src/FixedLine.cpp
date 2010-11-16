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

#include "FixedLine.h"
#include "FastDC.h"
#include <algorithm>
#include "Document.h"
#include "tmTheme.h"
#include "styler.h"
#include "Utf.h"
#include "BracketHighlight.h"

// Initialize static variables
wxString FixedLine::s_text;

FixedLine::FixedLine(wxDC& dc, const DocumentWrapper& dw, const vector<interval>& sel, const BracketHighlight& brackets,
					 const unsigned int& lastpos, const bool& isShadow, const tmTheme& theme):
	dc((FastDC&)dc), 
	m_doc(dw), 
	textstart(0), textend(0), 
	m_lineLen(0), m_lineBufferLen(0), 
	width(0), old_width(0), 
	m_tabChars(4), 
	lastpos(lastpos), 
	m_isSelShadow(isShadow), 
	m_brackets(brackets),
	m_theme(theme), 
	m_sr(m_theme, (FastDC&)dc),
	selections(sel),
	m_wrapMode(cxWRAP_NONE), 
	m_showIndent(false), 
	m_indentWidth(0),
	bmFold(1, 1), bmNewline(1, 1), bmSpace(1, 1), bmTab(1, 1)
{
#ifdef __WXDEBUG__
	m_inSetLine = false;
#endif
}

void FixedLine::SetPrintMode() {
	m_sr.SetPrintMode();
}

void FixedLine::Init() {
	// We can not count on the dc being valid in the constructor
	// so we have to do initialization using it here
	UpdateFont();
}

void FixedLine::UpdateFont() {
	m_sr.ClearCache();

	m_isFontFixedWidth = dc.GetFont().IsFixedWidth();

	// Get dimensions of a single whitespace character
	wxCoord w;
	wxCoord h;
	dc.GetTextExtent(wxT(' '), &w, &h);
	charwidth = w;
	charheight = h+1; // add room for underline
	dc.GetTextExtent(wxT('X'), &w, &h);
	m_nlwidth = w;

	// Create images of hidden chars
	// Draw hidden newline
	wxMemoryDC mdc;
	bmNewline = wxBitmap(m_nlwidth, charheight, 1);
	mdc.SelectObject(bmNewline);
	mdc.Clear();
	const int x_middle = m_nlwidth/2;
	const int y_fourth = charheight/4;
	wxPoint triangle_points[3];
	triangle_points[0] = wxPoint(0, charheight-y_fourth-2);
	triangle_points[1] = wxPoint(x_middle, charheight-y_fourth-5);
	triangle_points[2] = wxPoint(x_middle, charheight-y_fourth+1);
	mdc.SetPen(*wxBLACK_PEN);
	mdc.SetBrush(*wxBLACK_BRUSH);
	mdc.DrawLine(m_nlwidth-2, y_fourth, m_nlwidth-2, charheight-y_fourth-1);
	mdc.DrawLine(2, charheight-y_fourth-2, m_nlwidth-2, charheight-y_fourth-2);
	mdc.DrawPolygon(3, triangle_points, 0, 0);

	// Draw hidden space
	bmSpace = wxBitmap(charwidth, charheight, 1);
	mdc.SelectObject(bmSpace);
	mdc.Clear();
	mdc.SetPen(wxPen(*wxBLACK, 2, wxSOLID));
	const int y_middle = charheight/2;
	mdc.DrawCircle(charwidth/2, y_middle, 1);

	// Redraw tab image
	SetTabWidth(m_tabChars);

	// Draw fold indicator
	m_foldWidth = charwidth*2;
	m_foldHeight = charheight/2;
	bmFold = wxBitmap(m_foldWidth, m_foldHeight, 1);
	mdc.SelectObject(bmFold);
	mdc.Clear();
	mdc.SetPen(*wxBLACK_PEN);
	mdc.SetBrush(*wxBLACK_BRUSH);
	mdc.DrawRoundedRectangle(0, 0, m_foldWidth, m_foldHeight, 2);
	const unsigned int foldMiddle = m_foldHeight / 2;
	const unsigned int foldPart = m_foldWidth / 4;
	mdc.SetPen(*wxWHITE_PEN);
	mdc.SetBrush(*wxWHITE_BRUSH);
	mdc.DrawCircle(foldPart, foldMiddle, 1);
	mdc.DrawCircle(m_foldWidth / 2, foldMiddle, 1);
	mdc.DrawCircle(m_foldWidth - foldPart, foldMiddle, 1);

	// Create transparency mask for fold indicator
	wxBitmap bmMask = wxBitmap(m_foldWidth, m_foldHeight, 1);
	mdc.SelectObject(bmMask);
	mdc.SetPen(*wxBLACK_PEN);
	mdc.SetBrush(*wxBLACK_BRUSH);
	mdc.DrawRectangle(0, 0, m_foldWidth, m_foldHeight);
	mdc.SetPen(*wxWHITE_PEN);
	mdc.SetBrush(*wxWHITE_BRUSH);
	mdc.DrawRoundedRectangle(0, 0, m_foldWidth, m_foldHeight, 2);
	mdc.SelectObject(wxNullBitmap);
	bmFold.SetMask(new wxMask(bmMask));

	// Invalidate cache
	textstart = 0;
	textend = 0;
}

unsigned int FixedLine::SetLine(unsigned int startpos, unsigned int endpos, bool cache) {
	m_docLen = m_doc.GetLength();
/*
#ifdef __WXDEBUG__
	wxASSERT(!m_inSetLine); // re-entrancy check
	m_inSetLine = true;

	wxASSERT(startpos >= 0 && startpos < m_docLen);
	wxASSERT(endpos > startpos && endpos <= m_docLen);

	if (startpos > 0) {
		wxChar c;
		cxLOCKDOC_READ(m_doc)
			c = doc.GetChar(startpos - 1);
		cxENDLOCK
		wxASSERT(c == wxT('\n'));
	}
	if (endpos < m_docLen) {
		wxChar c;
		cxLOCKDOC_READ(m_doc)
			c = doc.GetChar(endpos - 1);
		cxENDLOCK
		wxASSERT(c == wxT('\n'));
	}
#endif
*/
	if (!cache || startpos != textstart || endpos != textend) {
		textstart = startpos;
		textend = endpos;
		m_lineLen = endpos - startpos;

		// Check if we need to resize line buffer
		if (m_lineBufferLen < m_lineLen) {
			m_lineBufferLen = m_lineLen;
			m_lineBuffer = wxCharBuffer(m_lineBufferLen); // wxCharBuffer allocs room for & adds nullbyte at len
		}

		// Cache the current line in lineBuffer (as UTF-8)
		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(textstart, textend, (unsigned char*)m_lineBuffer.data());
		cxENDLOCK

		// Style the lines
		m_sr.Init(textstart, textend);
		for (vector<Styler*>::iterator si = m_stylers.begin(); si != m_stylers.end(); ++si) {
			(*si)->Style(m_sr);
		}

		m_extents.clear();
		m_extents.reserve(m_lineLen);
		unsigned int xpos = 0;
		unsigned int lastpos = 0;
		unsigned int style_start = 0;
		int fontStyle = m_sr.GetFontStyle(0);
		char* dbi = m_lineBuffer.data();

		// Build list of text extends (totalling) compensating for tabs and styles
		// There is one extent per byte in the text. In utf8 chars composed out of multiple
		// bytes, they will all have same value.
		for (unsigned int style_id = 0; style_id < m_sr.GetStyleCount(); ++style_id) {
			const unsigned int style_end = m_sr.GetStyleEnd(style_id) - textstart;

			// Ignore zero length styles
			if (style_start == style_end) continue;

			// Check for style change
			if (m_sr.GetFontStyle(style_id) != fontStyle) {
				if (style_start > lastpos) {
					m_sr.ApplyFontStyle(fontStyle);

					// Get extends for current segment
					m_extsBuf.clear();
					const unsigned int seg_len = style_start-lastpos;
					ConvertFromUTF8toString(dbi+lastpos, seg_len, m_textBuf);
					dc.GetPartialTextExtents(m_textBuf, m_extsBuf);
					wxASSERT(!m_textBuf.empty() && m_extsBuf.size() == m_textBuf.size());

					// Add to main list adjusted for offset
					unsigned int extpos = 0;
					unsigned int offset = xpos;
					for (unsigned int i = lastpos; i < style_start; ++i) {
						if ((dbi[i] & 0xC0) == 0x80) m_extents.push_back(xpos); // Only count first byte of UTF-8 multibyte chars
						else if (dbi[i] == '\t') {
							// Add tab extend
							xpos = ((xpos / tabwidth)+1) * tabwidth; // GetTabPoint(xpos);
							offset += xpos - (m_extsBuf[extpos] + offset);
							m_extents.push_back(xpos);
							++extpos;
						}
						else {
							xpos = m_extsBuf[extpos] + offset;
							m_extents.push_back(xpos);
							++extpos;
						}
					}

					// Small hack to make lines that end with italics not cut off the edge of the last character
					if (fontStyle &= wxFONTFLAG_ITALIC) {
						m_extents.back() += 2;
						xpos = m_extents.back();
					}
				}

				lastpos = style_start;
				fontStyle = m_sr.GetFontStyle(style_id);
			}

			style_start = style_end;
		}

		// Add extends for last segment
		if (lastpos < m_lineLen) {
			m_sr.ApplyFontStyle(fontStyle);

			// Get extends for current segment
			m_extsBuf.clear();
			const unsigned int seg_len = m_lineLen-lastpos;
			ConvertFromUTF8toString(dbi+lastpos, seg_len, m_textBuf);
			dc.GetPartialTextExtents(m_textBuf, m_extsBuf);
			wxASSERT(!m_textBuf.empty() && m_extsBuf.size() == m_textBuf.size());

			// Add to main list adjusted for offset
			unsigned int extpos = 0;
			unsigned int offset = xpos;
			for (unsigned int i = lastpos; i < m_lineLen; ++i) {
				if ((dbi[i] & 0xC0) == 0x80) m_extents.push_back(xpos); // Only count first byte of UTF-8 multibyte chars
				else if (dbi[i] == '\t') {
					// Add tab extend
					xpos = ((xpos / tabwidth)+1) * tabwidth; // GetTabPoint(xpos);
					offset += xpos - (m_extsBuf[extpos] + offset);
					m_extents.push_back(xpos);
					++extpos;
				}
				else {
					xpos = m_extsBuf[extpos] + offset;
					m_extents.push_back(xpos);
					++extpos;
				}
			}
		}

		wxASSERT(m_extents.size() == m_lineLen);
		BreakLine();
	}
	else if (width != old_width) {
		wxASSERT(m_extents.size() == m_lineLen);
		BreakLine();
	}

#ifdef __WXDEBUG__
	m_inSetLine = false; // re-entrancy check
#endif

	return (m_wrapMode == cxWRAP_NONE) ? m_lineWidth : height;
}

void FixedLine::FlushCache(unsigned int pos) {
	// Invalidate cache if change happened before end
	if (textend > pos) {
		textstart = 0;
		textend = 0;
	}
}

int FixedLine::SetWidth(int newwidth) {
	wxASSERT(newwidth >= 0);

	if (newwidth != width) {
		width = newwidth;

		// Changing the width invalidates cache
		textstart = 0;
		textend = 0;
	}
	return height;
}

void FixedLine::SetTabWidth(unsigned int tabChars) {
	wxASSERT(tabChars > 0);

	tabwidth = tabChars * charwidth;
	m_tabChars = tabChars;

	// Draw hidden tab arrow
	bmTab = wxBitmap(tabwidth, charheight, 1);
	wxMemoryDC mdc;
	mdc.SelectObject(bmTab);
	mdc.Clear();
	mdc.SetPen(*wxBLACK_PEN);
	const int y_middle = charheight/2;
	mdc.DrawLine(1, y_middle, tabwidth-1, y_middle);
	mdc.DrawLine(tabwidth-charwidth+2, 3, tabwidth-1, y_middle);
	mdc.DrawLine(tabwidth-charwidth+2, charheight-4, tabwidth-1, y_middle);
}

unsigned int FixedLine::GetLineWidth() const {
	return  (m_wrapMode == cxWRAP_NONE) ? m_lineWidth : width;
}

int FixedLine::GetHeight() const {
	wxASSERT(width);
	wxASSERT(!(textstart == 0 && textend == 0)); // line has to be valid

	return height;
}

unsigned int FixedLine::GetQuickLineWidth(unsigned int startpos, unsigned int endpos) {
	// This function gives a quick approximation of the total width of the line (without breaking).
	// It ignores elements like tabs, bold chars and asian extra-wide chars.
	if (m_isFontFixedWidth) {
		const unsigned int len = endpos - startpos;
		return len * charwidth;
	}

	// Proportional width font
	wxString text;
	cxLOCKDOC_READ(m_doc)
		text = doc.GetTextPart(startpos, endpos);
	cxENDLOCK

	const wxSize extent = dc.GetTextExtent(text);
	return extent.x;
}

unsigned int FixedLine::GetQuickLineHeight(unsigned int startpos, unsigned int endpos) {
	wxASSERT(m_wrapMode != cxWRAP_NONE);

	// This function gives a quick (and very rough) approximation of the total height of the line.
	// It ignores elements like tabs, bold chars and asian extra-wide chars.
	if (m_isFontFixedWidth) {
		const unsigned int len = endpos - startpos;
		const unsigned int linewidth = len * charwidth;
		unsigned int breaklines = linewidth / width;
		if (linewidth % width) ++breaklines;

		return breaklines * charheight;
	}

	// Proportional width font
	wxString text;
	cxLOCKDOC_READ(m_doc)
		text = doc.GetTextPart(startpos, endpos);
	cxENDLOCK

	wxArrayInt widths;
	dc.GetPartialTextExtents(text, widths);

	if (widths.Last() <= width) return charheight;

	unsigned int breaklines = 1;
	unsigned int margin = width;

	for (size_t i = 0; i < widths.Count(); ++i) {
		if (widths[i] > (int)margin) {
			++breaklines;
			margin += width;
		}
	}

	return breaklines * charheight;
}

wxPoint FixedLine::GetCaretPos(unsigned int pos, bool tryfront) const {
	wxASSERT(width > 0);
	wxASSERT(textstart + pos <= textend);

	// Find the line
	vector<unsigned int>::const_iterator posline = lower_bound(breakpoints.begin(), breakpoints.end(), pos);
	int linestart = 0; if (posline != breakpoints.begin()) linestart = *(posline-1);

	int xpos = 0;

	// Calculate the caret xpos
	if (pos != 0) {
		if ((tryfront || m_indentWidth) && pos == *posline && posline+1 != breakpoints.end()) {
			// if old caretpos was at the pos 0 on a line after a linebreak
			// we will try to keep it there, but only if break is still at
			// same point
			++posline;

			if (posline > breakpoints.begin()) xpos = m_indentWidth; // Smart wrap
		}
		else {
			if (posline > breakpoints.begin()) xpos = m_indentWidth; // Smart wrap

			// Find the pos
			if (linestart == 0) xpos += m_extents[pos-1];
			else xpos += m_extents[pos-1] - m_extents[linestart-1];
		}
	}

	// if last chars are whitespaces, xpos may be outside display
	// move caret to be wisible
	if (m_wrapMode != cxWRAP_NONE) {
		xpos = wxMin(xpos, width-2);
	}

	return wxPoint(xpos, distance(breakpoints.begin(), posline)*charheight);
}


void FixedLine::DrawLine(int xoffset, int yoffset, const wxRect& WXUNUSED(rect), bool isFolded) {
	wxASSERT(width > 0);

	// Style the lines
	m_sr.Init(textstart, textend);
	for (vector<Styler*>::iterator si = m_stylers.begin(); si != m_stylers.end(); ++si) {
		(*si)->Style(m_sr);
	}

	// Style with selections
	bool endstyle = false;
	for(vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
		if((textstart < iv->end && iv->start < textend) || (iv->start == textstart && iv->end == textstart)) {
			// Calculate enclosed interval
			int s_start = wxMax(textstart, (*iv).start);
            int s_end   = wxMin(textend, (*iv).end);

			const wxColour& useBackground = m_isSelShadow ? m_theme.shadowColor : m_theme.selectionColor;
			m_sr.SetBackgroundColor(s_start, s_end, useBackground);
			m_sr.SetShowHidden(s_start, s_end, true);
		}
		else if (iv->start == textend && iv->end == textend) {
			// We might have to draw a selection caret at the end
			endstyle = true;
			const wxColour& useBackground = m_isSelShadow ? m_theme.shadowColor : m_theme.selectionColor;
			m_sr.SetBackgroundColor(textend, textend, useBackground);
		}
	}

	// Highlight matching brackets (can only be single byte utf char)
	if (m_brackets.HasOrderedInterval()) {
		if (textstart <= m_brackets.Start() && m_brackets.Start() < textend) {
			m_sr.SetFontStyle(m_brackets.Start(), m_brackets.Start()+1, wxFONTFLAG_BOLD);
			m_sr.SetBackgroundColor(m_brackets.Start(), m_brackets.Start()+1, m_theme.bracketHighlightColor);
		}
		if (textstart <= m_brackets.End() && m_brackets.End() < textend) {
			m_sr.SetFontStyle(m_brackets.End(), m_brackets.End()+1, wxFONTFLAG_BOLD);
			m_sr.SetBackgroundColor(m_brackets.End(), m_brackets.End()+1, m_theme.bracketHighlightColor);
		}
	}

	bool isIndent = true;
	unsigned int style_id = 0;
	unsigned int next_style_id = 0;
	unsigned int next_style_start = 0;
	unsigned int linestart = 0;
	int ypos = 0;
	const char* const buf = (const char*)m_lineBuffer.data();
	for (vector<unsigned int>::iterator i = breakpoints.begin(); i != breakpoints.end(); ++i) {
		wxASSERT(ypos + charheight <= height);
		unsigned int breakpoint = *i;

		unsigned int segstart = linestart;
		int segoffset = 0;

		// Smart wrap
		if (m_indentWidth && i > breakpoints.begin()) {
			if (dc.GetBackgroundMode() != wxTRANSPARENT) {
				dc.SetPen(dc.GetTextBackground());
				dc.SetBrush(wxBrush(dc.GetTextBackground(), wxSOLID));
				dc.DrawRectangle(xoffset + segoffset, yoffset+ypos, m_indentWidth, charheight);
			}

			if (m_sr.ShowHidden(style_id)) {
				dc.SetBrush(wxBrush(m_theme.invisiblesColor, wxBDIAGONAL_HATCH));
				dc.DrawRectangle(xoffset + segoffset, yoffset+ypos, m_indentWidth, charheight);
			}

			segoffset = m_indentWidth;
		}

		// if we are not wrapping we want to start at first visible char
		unsigned int pos = linestart;
		if (m_wrapMode == cxWRAP_NONE && xoffset < 0) {
			// find first visible char
			if (m_extents.back() < (unsigned int)-xoffset) segstart = pos = breakpoint; // no part of line visible (may clip newline)
			else {
				vector<unsigned int>::iterator p = lower_bound(m_extents.begin(), m_extents.end(), (unsigned int)-xoffset);
				linestart = segstart = pos = distance(m_extents.begin(), p);
				segoffset = pos ? m_extents[pos-1] : 0;
				style_id = m_sr.GetStyleAtPos(textstart + pos);
				next_style_id = style_id+1;
				next_style_start = m_sr.GetStyleEnd(style_id) - textstart;
				m_sr.ApplyStyle(style_id);
			}
		}

		for (const char* dbi = buf + linestart; pos < breakpoint; ++dbi, ++pos) {
			// Check for style change
			if (pos == next_style_start) {
				if (pos > segstart) {
					segoffset += DrawText(xoffset, segoffset, yoffset+ypos, segstart, pos);
					segstart = pos;
				}

				unsigned int style_end = m_sr.GetStyleEnd(next_style_id) - textstart;
				if (next_style_start == style_end) {
					// zero length style. Draw a caret and go to next style
					m_sr.ApplyStyle(next_style_id++);
					if (dc.GetTextBackground() != wxNullColour) {
						dc.SetPen(dc.GetTextBackground());
						dc.SetBrush(wxBrush(dc.GetTextBackground(), wxSOLID));
						dc.DrawRectangle(xoffset + segoffset, yoffset+ypos, 3, charheight);
					}
					next_style_start = m_sr.GetStyleEnd(next_style_id) - textstart;
				}
				else next_style_start = style_end;

				style_id = next_style_id++;
				m_sr.ApplyStyle(style_id);
			}

			// Check for special chars
			switch (*dbi) {
			case ' ':
				if (isIndent || m_sr.ShowHidden(style_id)) {
					if (pos > segstart) {
						segoffset += DrawText(xoffset, segoffset, yoffset+ypos, segstart, pos);
					}

					if (m_sr.ShowHidden(style_id)) {
						const unsigned int xpos = xoffset + segoffset;
						wxColour textcolor = dc.GetTextForeground(); // Buffer text color
						dc.SetTextForeground(m_theme.invisiblesColor);
						dc.DrawBitmap(bmSpace, xpos, yoffset+ypos, false);
						dc.SetTextForeground(textcolor); // Re-set text color
					}
					else if (dc.GetBackgroundMode() != wxTRANSPARENT) {
						dc.SetPen(dc.GetTextBackground());
						dc.SetBrush(wxBrush(dc.GetTextBackground(), wxSOLID));
						dc.DrawRectangle(xoffset + segoffset, yoffset+ypos, charwidth, charheight);
					}

					// Indent guides
					if (isIndent && m_showIndent && segoffset > 0 && IsTabPos(segstart)) {
						dc.SetPen(m_theme.invisiblesColor);
						for (unsigned int y = yoffset+ypos+1; (int)y < yoffset+ypos+charheight; y += 3) {
							dc.DrawPoint(xoffset + segoffset, y);
						}
					}

					segoffset += charwidth;
					segstart = pos+1;
				}
				break;

			case '\t':
				{
					if (pos > segstart) {
						segoffset += DrawText(xoffset, segoffset, yoffset+ypos, segstart, pos);
					}

					if (m_sr.ShowHidden(style_id)) {
						wxColour textcolor = dc.GetTextForeground(); // Buffer text color
						dc.SetTextForeground(m_theme.invisiblesColor);

						if(GetTabPoint(segoffset)-segoffset < tabwidth) {
							const int ntwidth = GetTabPoint(segoffset)-segoffset;
							dc.DrawBitmap(bmTab.GetSubBitmap(wxRect(tabwidth-ntwidth, 0, ntwidth, charheight)), xoffset + segoffset, yoffset+ypos, false);
						}
						else dc.DrawBitmap(bmTab, xoffset + segoffset, yoffset+ypos, false);

						dc.SetTextForeground(textcolor); // Re-set text color
					}
					else if (dc.GetBackgroundMode() != wxTRANSPARENT) {
						const int ntwidth = GetTabPoint(segoffset)-segoffset;
						dc.SetPen(dc.GetTextBackground());
						dc.SetBrush(wxBrush(dc.GetTextBackground(), wxSOLID));
						dc.DrawRectangle(xoffset + segoffset, yoffset+ypos, ntwidth, charheight);
					}

					// Indent guides
					if (isIndent && m_showIndent && segoffset > 0 && IsTabPos(segstart)) {
						dc.SetPen(m_theme.invisiblesColor);
						for (unsigned int y = yoffset+ypos+1; (int)y < yoffset+ypos+charheight; y += 3) {
							dc.DrawPoint(xoffset + segoffset, y);
						}
					}

					segoffset = GetTabPoint(segoffset);
					segstart = pos+1;
				}
				break;

			case '\n':
				wxASSERT(textstart + pos == textend-1);
				if (pos > segstart) {
					segoffset += DrawText(xoffset, segoffset, yoffset+ypos, segstart, pos);
				}

				if (m_sr.ShowHidden(style_id)) {
					wxColour textcolor = dc.GetTextForeground(); // Buffer text color
					dc.SetTextForeground(m_theme.invisiblesColor);
					dc.DrawBitmap(bmNewline, xoffset + segoffset, yoffset+ypos, false);
					dc.SetTextForeground(textcolor); // Re-set text color
					//segoffset += charwidth;
				}

				// If this is the terminating (last-in-doc) newline and we have
				// an endstyle, we have to draw the caret on the next (virtual) line
				if (endstyle && (textstart + pos == m_docLen-1)) {
					unsigned int style_end = m_sr.GetStyleEnd(next_style_id) - textstart;
					if (next_style_start == style_end) {
						// zero length style. Draw a caret.
						ypos += charheight; // move to next (virtual) line
						m_sr.ApplyStyle(next_style_id);
						dc.SetPen(dc.GetTextBackground());
						dc.SetBrush(wxBrush(dc.GetTextBackground(), wxSOLID));
						dc.DrawRectangle(0, yoffset+ypos, 3, charheight);
					}
				}

				// Check if the background hightlight should be extended
				if (m_sr.DoExtendBg()) {
					dc.SetPen(m_sr.GetExtendBgColor());
					dc.SetBrush(wxBrush(m_sr.GetExtendBgColor(), wxSOLID));
					const unsigned int xpos = xoffset + segoffset;
					dc.DrawRectangle(xpos, yoffset+ypos, width-xpos, charheight);
				}

				//return; // Newline is always last character in line
				segoffset += m_nlwidth;
				segstart = pos+1;
				endstyle = false; // avoid drawing zerolength selection twice
				break;

			default:
				isIndent = false;
			}
		}

		// Draw text after last special char
		if (segstart < breakpoint) {
			segoffset += DrawText(xoffset, segoffset, yoffset+ypos, segstart, breakpoint);
		}

		// Check if the background hightlight should be extended
		if (m_sr.DoExtendBgAtPos(textstart + breakpoint)) {
			dc.SetPen(m_sr.GetExtendBgColor());
			dc.SetBrush(wxBrush(m_sr.GetExtendBgColor(), wxSOLID));
			const unsigned int xpos = xoffset + segoffset;
			dc.DrawRectangle(xpos, yoffset+ypos, width-xpos, charheight);
		}

		// End-of-line
		if (i == breakpoints.end()-1) {
			// If we are at end of text (no newline), we might have to draw a zero-length selection
			if (endstyle) {
				unsigned int style_end = m_sr.GetStyleEnd(next_style_id) - textstart;
				if (next_style_start == style_end) {
					// zero length style. Draw a caret.
					m_sr.ApplyStyle(next_style_id);
					dc.SetPen(dc.GetTextBackground());
					dc.SetBrush(wxBrush(dc.GetTextBackground(), wxSOLID));
					dc.DrawRectangle(xoffset + segoffset, yoffset+ypos, 3, charheight);
				}
			}

			// Draw fold indicator after last char
			if (isFolded) {
				dc.SetTextForeground(m_theme.invisiblesColor);
				dc.SetTextBackground(m_theme.backgroundColor);
				const unsigned int foldyoffset = (charheight - m_foldHeight) / 2;

				// Enabling transparency draws as monochrome
				dc.DrawBitmap(bmFold, xoffset + segoffset, yoffset+ypos+foldyoffset, false);
			}
		}

		linestart = breakpoint;
		ypos += charheight;
	}
}

unsigned int FixedLine::DrawText(int xoffset, int x, int y, unsigned int start, unsigned int end) {
	const char* const buf = (const char*)m_lineBuffer.data();
	const unsigned int full_extent = start ? m_extents[end-1] - m_extents[start-1] : m_extents[end-1];
	const int xpos = xoffset + x;
	
	// if we do not word wrap, the line can be *very* long (long enough that DrawText won't draw it)
	// so we strip it to the visible part (may also improve performance).
	if (m_wrapMode == cxWRAP_NONE && xpos + (int)full_extent > width) {
		// Find last visible char
		const unsigned int right_border = width - xoffset;
		vector<unsigned int>::iterator p_start = m_extents.begin() + start;
		vector<unsigned int>::iterator p = lower_bound(p_start, m_extents.end(), right_border);
		
		unsigned short tmp = utf8_len(buf[distance(m_extents.begin(), p)]);
		if ((5 == tmp) || (tmp > distance(p, m_extents.end()))) {
			// bad symbol or incomplete string
			wxFAIL;
		} else {
			p += tmp; // may be partially visible
		}

		const unsigned int seglen = distance(p_start, p);
		const unsigned int segend = start + seglen;

		const unsigned int extent = start ? m_extents[segend-1] - m_extents[start-1] : m_extents[segend-1];
		ConvertFromUTF8toString(buf + start, seglen, m_textBuf);
		dc.FastDrawText(m_textBuf, xpos, y, extent, charheight);
	}
	else {
		ConvertFromUTF8toString(buf + start, end-start, m_textBuf);
		dc.FastDrawText(m_textBuf, xpos, y, full_extent, charheight);
	}

	return full_extent;
}

void FixedLine::BreakLine() {
	old_width = width; // make sure we only linebreak again if width changes

	breakpoints.clear();
	m_indentWidth = 0;

	// Handle lines with no word-wrap
	if (m_wrapMode == cxWRAP_NONE) {
		breakpoints.push_back(m_lineLen);
		m_lineWidth = m_extents.back();
		height = charheight;
		return;
	}

	// We cannot do linebreaks when width has not been set
	wxASSERT(width);

	// No need to break line if it all fits
	if ((int)m_extents.back() <= width) {
		breakpoints.push_back(m_lineLen);
		height = charheight;
		return;
	}

	/*int lastBreakpoint = 0;
	int newBreakpoint = 0;
	int linewidth = 0;
	int lastwidth = 0;*/

	//bool countIndent = (m_wrapMode == cxWRAP_SMART);

	//doc_byte_iter dbi(db, textstart);
	unsigned char* dbi = (unsigned char*)m_lineBuffer.data();
	//unsigned int char_id = 0;

	// Find width of smartwrap
	if (m_wrapMode == cxWRAP_SMART) {
		for (unsigned int i = 0; i < m_lineLen; ++i) {
			if (dbi[i] == '\t' || dbi[i] == ' ') m_indentWidth = m_extents[i];
			else break;
		}
	}

	unsigned int adj_width = width;
	unsigned int lastbreak = 0;
	vector<unsigned int>::iterator p = lower_bound(m_extents.begin(), m_extents.end(), adj_width);
	while (p != m_extents.end()) {
		const unsigned int lastchar_id = distance(m_extents.begin(), p);

		// Find the breakpoint
		if (dbi[lastchar_id] == ' ') lastbreak = lastchar_id+1; // It is ok for whitespace to extend beyond view
		else if (lastchar_id == lastbreak) lastbreak = lastchar_id+1; // Not even room for the first char, put it there anyways.
		else {
			unsigned int char_id = lastchar_id;
			const unsigned int limit = lastbreak;

			// Go back until we find a breaking char
			do {
				--char_id;

				const char c = dbi[char_id];
				if (c == ' ' || c == '\t' || c == '-') {
					lastbreak = char_id+1;
					break;
				}
			} while (char_id != limit);

			// If word is too long to fit line, break at char level
			if (lastbreak == limit) lastbreak = lastchar_id;
		}
		breakpoints.push_back(lastbreak);

		// Adjust width to end of next line
		const unsigned int lastbreakpos = m_extents[lastbreak-1];
		adj_width = lastbreakpos + (width - m_indentWidth);

		p = lower_bound(m_extents.begin()+lastbreak, m_extents.end(), adj_width);
	}

	/*for (; char_id < m_lineLen; ++dbi, ++char_id) {
		// Only count first byte of UTF-8 multibyte chars
		if ((*dbi & 0xC0) == 0x80) continue;

		lastwidth = linewidth;

		switch (*dbi) {
		case ' ':
			lastBreakpoint = newBreakpoint = char_id+1;
			linewidth += charwidth;
			break;

		case '\t':
			lastBreakpoint = char_id+1;
			linewidth += tabwidth;
			break;

		case '\n':
			wxASSERT(char_id == m_lineLen-1);
			break;

		case '-':
			newBreakpoint = char_id+1;
			// fall-through

		default:
			// Smart indent
			if (countIndent) {
				if (linewidth < width) { // disable smart wrap on very thin windows
					m_indentWidth = linewidth;
				}
				countIndent = false;
			}

			linewidth += charwidth;
		}

		// Check if we should break
		if (linewidth > width) {
			if (lastBreakpoint) {
				// break at last breakpoint
				breakpoints.push_back(lastBreakpoint);
				char_id = lastBreakpoint-1;
				dbi = (unsigned char*)m_lineBuffer.data() + char_id;
			}
			else {
				// word too long to fit line, break at char level
				if (lastwidth == 0 || (!countIndent && lastwidth == (int)m_indentWidth)) {
					// Get valid ndx of next char
					unsigned int next_char_id = char_id + 1;
					unsigned char* dbi2 = (unsigned char*)m_lineBuffer.data() + next_char_id;
					while ((*dbi2 & 0xC0) == 0x80) {
						++dbi2;
						++next_char_id;
					}

					// not even room for the first char, put it
					// there anyways.
					breakpoints.push_back(next_char_id);
				}
				else {
					breakpoints.push_back(char_id);
					do { // Go to previous *valid* char
						--char_id; // to next line
						--dbi;
					} while ((*dbi & 0xC0) == 0x80);
				}
			}
			lastBreakpoint = newBreakpoint = 0;
			linewidth = m_indentWidth;
			countIndent = false;
		}
		else lastBreakpoint = newBreakpoint;
	}*/
	if (breakpoints.empty() || breakpoints.back() < m_lineLen) {
		breakpoints.push_back(m_lineLen);
	}

	height = charheight * (int)breakpoints.size();
}

bool FixedLine::IsTabPos(unsigned int pos) const {
	wxASSERT(pos <= m_lineLen);

	// Calculate pos with tabs expanded
	unsigned int tabpos = 0;
	unsigned char* dbi = (unsigned char*)m_lineBuffer.data();
	unsigned char* end = dbi + pos;
	for (; dbi < end; ++dbi) {
		// Only count first byte of UTF-8 multibyte chars
		if ((*dbi & 0xC0) == 0x80) continue;

		if (*dbi == '\t') tabpos += m_tabChars;
		else tabpos += 1;
	}

	return (tabpos == 0) || (tabpos % m_tabChars == 0);
}

int FixedLine::GetTabPoint(int xpos) const {
	wxASSERT(xpos >= 0);

	int tabpoint = ((xpos / tabwidth)+1) * tabwidth;

	// If word-wrapped, tab can only extend to border
	if (m_wrapMode != cxWRAP_NONE) {
		tabpoint = wxMin(tabpoint, width);
	}

	return tabpoint;
}

wxRect FixedLine::GetFoldIndicatorRect() const {
	const unsigned int lastpos = textend - textstart;
	wxPoint point = GetCaretPos(lastpos);

	return wxRect(point.x, point.y, m_foldWidth, charheight);
}

bool FixedLine::IsOverFoldIndicator(const wxPoint& point) const {
	const full_pos fp = ClickOnLine(point.x, point.y);
	const unsigned int pos = textstart + fp.pos;

	if (fp.xy_outbound && (pos == textend || pos == textend-1)) {
		const unsigned int line_end_xpos = (pos == textend) ? fp.caret_xpos : fp.caret_xpos + charwidth;
		if (point.x >= (int)line_end_xpos && point.x <= (int)(line_end_xpos + m_foldWidth)) return true;
	}

	return false;
}

full_pos FixedLine::ClickOnLine(int xpos, int ypos) const {
	wxASSERT(textstart < textend);
	wxASSERT(xpos >= 0);
	wxASSERT(ypos >= 0 && ypos <= height);

	// Find the line
	unsigned int line_id = 0;
	if (ypos) { line_id = ypos / charheight; if (ypos % charheight == 0) --line_id;	}
	unsigned int lineoffset = 0; if (line_id) lineoffset = breakpoints[line_id-1];

	bool xy_outbound = false;
	bool onNewline = false;
	unsigned int char_id = lineoffset;
	int char_xpos = 0;

	// Smart wrap
	if (line_id) {
		if (xpos <= (int)m_indentWidth) {
			const full_pos fp = {char_id, m_indentWidth, line_id*charheight, xy_outbound, true};
			return fp;
		}
	}

	const unsigned int indentWidth = line_id ? m_indentWidth : 0;
	const unsigned int offset = lineoffset ? m_extents[lineoffset-1] : 0;
	const unsigned int adj_xpos = (xpos + offset) - indentWidth;
	const unsigned int lastcharpos = breakpoints[line_id];

	// Calculate the caret xpos
	while (char_id < lastcharpos) {
		const unsigned int char_end = m_extents[char_id];
		if (char_end > adj_xpos) {
			const unsigned int char_start = char_id ? m_extents[char_id-1] : 0;
			const char* dbi = m_lineBuffer.data() + char_id;

			// Check if which half we have clicked in
			if (adj_xpos > char_start + ((char_end - char_start) / 2)) {
				char_xpos = (char_end - offset) + indentWidth;

				// Go to next *valid* char
				do {++char_id; ++dbi;} while ((*dbi & 0xC0) == 0x80);
			}
			else char_xpos = (char_start - offset) + indentWidth;

			// Check if we have clicked on the newline
			if (*dbi == '\n') {
				onNewline = true;
				xy_outbound = true;
			}

			break;
		}
		else ++char_id;
	}

	/*// Calculate the caret xpos
	//doc_byte_iter dbi(db, textstart + char_id);
	unsigned char* dbi = (unsigned char*)m_lineBuffer.data() + char_id;
	while (char_id < breakpoints[line_id]) {
		if ((*dbi & 0xC0) != 0x80) { // Only count first byte of UTF-8 multibyte chars
			if (*dbi == '\t') {
				int tabpos = GetTabPoint(char_xpos);
				if (xpos < tabpos) {
					// Check if we have clicked in right half
					if (xpos > char_xpos + ((tabpos - char_xpos) / 2)) {
						do { // Go to next *valid* char
							++char_id;
							++dbi;
						} while ((*dbi & 0xC0) == 0x80);
						char_xpos = tabpos;
					}
					break;
				}
				else char_xpos = tabpos;
			}
			else if (*dbi == '\n') {
				if (xpos < char_xpos + charwidth) onNewline = true;
				xy_outbound = true;
				break;
			}
			else {
				if (xpos < char_xpos + charwidth) {
					// Check if we have clicked in right half
					if (xpos > char_xpos + (charwidth / 2)) {
						do { // Go to next *valid* char
							++char_id;
							++dbi;
						} while ((*dbi & 0xC0) == 0x80);
						char_xpos += charwidth;
					}
					break;
				}
				else char_xpos += charwidth;
			}
		}

		++char_id;
		++dbi;
	}*/

	// If the click was beyond end-of-line, we should set pos before the
	// terminating newline, or if none, at the end of line
	const unsigned int bpoint = breakpoints[line_id];
	if (char_id >= bpoint) {
		unsigned char* line = (unsigned char*)m_lineBuffer.data();

		if (bpoint && line[bpoint-1] == '\n') {
			char_id = bpoint-1; // Set caret before newline
		}
		else char_id  = bpoint; // Set caret after last char
		xy_outbound = true;
	}

	const full_pos fp = {char_id, char_xpos, line_id*charheight, xy_outbound, onNewline};
	//wxLogTrace("FixedClick %d,%d - %d,%d - %d", xpos, ypos, line_id, char_id, pos);
	return fp;
}

void FixedLine::AddStyler(Styler& styler) {
	m_stylers.push_back(&styler);
}
