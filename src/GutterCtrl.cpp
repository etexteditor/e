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

#include "GutterCtrl.h"
#include "Fold.h"
#include "tmTheme.h"
#include "EditorCtrl.h"

unsigned int _gutter_digits_in_number(unsigned int number) {
	unsigned int count = 1; // minimum is one
	while ((number /= 10) != 0) ++count;
	return count;
}

BEGIN_EVENT_TABLE(GutterCtrl, wxControl)
	EVT_PAINT(GutterCtrl::OnPaint)
	EVT_SIZE(GutterCtrl::OnSize)
	EVT_ERASE_BACKGROUND(GutterCtrl::OnEraseBackground)
	EVT_LEFT_DOWN(GutterCtrl::OnMouseLeftDown)
	EVT_LEFT_DCLICK(GutterCtrl::OnMouseLeftDClick)
	EVT_LEFT_UP(GutterCtrl::OnMouseLeftUp)
	EVT_MOTION(GutterCtrl::OnMouseMotion)
	EVT_LEAVE_WINDOW(GutterCtrl::OnMouseLeave)
	EVT_MOUSE_CAPTURE_LOST(GutterCtrl::OnCaptureLost)
END_EVENT_TABLE()

GutterCtrl::GutterCtrl(EditorCtrl& parent, wxWindowID id): 
	wxControl(&parent, id, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE),
	m_editorCtrl(parent), 
	m_mdc(), m_bitmap(1,1), m_width(0), m_gutterLeft(true), 
	m_showBookmarks(true), 
	m_showFolds(true), m_currentFold(NULL), m_posBeforeFoldClick(-1),
	m_theme(m_editorCtrl.m_theme), m_bgcolor(m_theme.gutterColor),
	m_currentSel(-1)
{
	m_mdc.SelectObject(m_bitmap);
	if (!m_mdc.Ok()) wxLogError(wxT("wxMemoryDC() constructor was failed in creating!"));

	UpdateTheme(true);

// FIXME Do not set max values as CalcLayout would get another idea of width
#if 0
	// Calculate width of control
	m_max_digits = 1;
	const int width = m_digit_width*m_max_digits + 7;

	// Make sure sizes get the right min/max sizes
	SetSizeHints(width, -1, width, -1);
#endif

	// Set to standard cursor (otherwise it will inherit from editorCtrl)
	SetCursor(*wxSTANDARD_CURSOR);
}

void GutterCtrl::UpdateTheme(bool forceRecalculateDigitWidth) {
	const bool hasNewFont = m_mdc.GetFont() != m_theme.font;

	if (hasNewFont) m_mdc.SetFont(m_editorCtrl.GetEditorFont());

	if (forceRecalculateDigitWidth || hasNewFont) {
		// Get the width of a single digit
		wxCoord w;
		wxCoord h;
		m_mdc.GetTextExtent(wxT('0'), &w, &h);
		m_digit_width = w;
	}

	// Create colors
	const int red = m_bgcolor.Red();
	const int blue = m_bgcolor.Blue();
	const int green = m_bgcolor.Green();
	if (red - 140 < 0) {
		m_hlightcolor.Set(0xFF & (red+20), 0xFF & (green+20), 0xFF & (blue+20)); // Pastel purple (slightly darker)
		m_edgecolor.Set(0xFF & (red+100), 0xFF & (green+100), 0xFF & (blue+100)); // Pastel purple (darker)
		m_numbercolor.Set(0xFF & (red+140), 0xFF & (green+140), 0xFF & (blue+140)); // Pastel purple (even darker)
	}
	else {
		m_hlightcolor.Set(wxMax(0,red-20), wxMax(0,green-20), wxMax(0,blue-20)); // Pastel purple (slightly darker)
		m_edgecolor.Set(wxMax(0,red-100), wxMax(0,green-100), wxMax(0,blue-100)); // Pastel purple (darker)
		m_numbercolor.Set(wxMax(0,red-140), wxMax(0,green-140), wxMax(0,blue-140)); // Pastel purple (even darker)
	}
	m_mdc.SetBackground(wxBrush(m_bgcolor));

	// Draw open fold button
	wxMemoryDC mdc;
	m_bmFoldOpen = wxBitmap(9, 9);
	mdc.SelectObject(m_bmFoldOpen);
	mdc.Clear();
	mdc.SetPen(m_edgecolor);
	mdc.SetBrush(*wxWHITE_BRUSH);
	mdc.DrawRectangle(0, 0, 9, 9);
	mdc.DrawLine(2, 4, 7, 4);

	// Draw closed fold button
	m_bmFoldClosed = wxBitmap(9, 9);
	mdc.SelectObject(m_bmFoldClosed);
	mdc.Clear();
	mdc.SetPen(m_edgecolor);
	mdc.SetBrush(*wxWHITE_BRUSH);
	mdc.DrawRectangle(0, 0, 9, 9);
	mdc.DrawLine(2, 4, 7, 4);
	mdc.DrawLine(4, 2, 4, 7);

	// Draw bookmark
	m_bmBookmark = wxBitmap(10, 10);
	mdc.SelectObject(m_bmBookmark);
	mdc.SetBackground(wxBrush(m_bgcolor));
	mdc.Clear();
	mdc.SetBrush(m_edgecolor);
	mdc.DrawCircle(5,5,5);
}

unsigned GutterCtrl::CalcLayout(unsigned int height) {
	const Lines& lines = m_editorCtrl.m_lines;

	// Calculate the number of digits in max linenumber; reserve space for at least 2
	const int digits = wxMax(_gutter_digits_in_number(lines.GetLineCount()), 2);

	// Resize control
	m_max_digits = digits;
	m_width = m_digit_width*m_max_digits + 7;
	m_numberX = 3;
	if (m_showBookmarks) {
		m_width += 10;
		m_numberX += 10;
	}
	m_foldStartX = m_width;
	if (m_showFolds) {
		m_width += 10;
		m_foldStartX -= 2; // compensate for border
	}

	SetSize(m_width, height);
	if (!m_gutterLeft) {
		SetPosition(wxPoint(m_editorCtrl.GetClientSize().x - m_width, 0));
	}
	return m_width;
}

void GutterCtrl::SetGutterRight(bool doMove) {
	m_gutterLeft = !doMove;
	if (m_gutterLeft) SetPosition(wxPoint(0,0)); // reset pos
}

void GutterCtrl::DrawGutter(wxDC& dc) {
	Lines& lines = m_editorCtrl.m_lines;

	const wxSize size = GetClientSize();
	m_mdc.Clear();

	const unsigned int bg_xpos = m_gutterLeft ? size.x-1 : 0;
	const unsigned int edge_xpos = m_gutterLeft ? size.x-2 : 1;

	// Draw the edge
	m_mdc.SetPen(m_theme.backgroundColor);
	m_mdc.DrawLine(bg_xpos, 0, bg_xpos, size.y);
	m_mdc.SetPen(m_edgecolor);
	m_mdc.DrawLine(edge_xpos, 0, edge_xpos, size.y);

	// Draw the line numbers
	m_mdc.SetTextForeground(m_numbercolor);
	wxString number;
	const int scrollPos = m_editorCtrl.scrollPos;

	const unsigned int firstline = lines.GetLineFromYPos(scrollPos);
	const unsigned int linecount = lines.GetLineCount();

	// Prepare for foldings
	const vector<cxFold>& folds = m_editorCtrl.GetFolds();
	vector<cxFold>::const_iterator nextFold = folds.begin();
	const unsigned int line_middle = lines.GetLineHeight() / 2;
	vector<const cxFold*> foldStack;
	if (m_showFolds) {
		m_editorCtrl.UpdateFolds();

#ifdef __WXDEBUG__
		bool debug = false;
		if (debug) {
			for (vector<cxFold>::const_iterator f = folds.begin(); f != folds.end(); ++f) {
				const wxString indent(wxT('.'), f->indent);
				wxLogDebug(wxT("%d: %s%d"), f->line_id, indent.c_str(), f->type);
			}
		}
#endif

		for (nextFold = folds.begin(); nextFold != folds.end() && nextFold->line_id < firstline; ++nextFold) {
			if (nextFold->type != cxFOLD_END) {
				foldStack.push_back(&*nextFold);
				continue;
			}

			// check if end marker matches any starter on the stack
			for (vector<const cxFold*>::reverse_iterator p = foldStack.rbegin(); p != foldStack.rend(); ++p) {
				if ((*p)->indent != nextFold->indent) continue;

				foldStack.erase(p.base()-1, foldStack.end()); // pop
				break;
			}
		}
	}

	// Prepare for bookmarks
	const vector<cxBookmark>& bookmarks = m_editorCtrl.GetBookmarks();
	vector<cxBookmark>::const_iterator nextBookmark = bookmarks.begin();
	while(nextBookmark != bookmarks.end() && nextBookmark->line_id < firstline) ++nextBookmark;

	// Draw each line
	for (unsigned int i = firstline; i < linecount; ++i) {
		number.Printf(wxT("%*u"), m_max_digits, i+1);
		const int ypos = lines.GetYPosFromLine(i) - scrollPos;
		if (ypos > size.y) break;

		// Highlight selections
		if (m_currentSel != -1 &&
			((i >= m_sel_startline && i <= m_sel_endline) ||
			 (i >= m_sel_endline && i <= m_sel_startline))) {

			const int ypos2 = lines.GetBottomYPosFromLine(i) - scrollPos;
			m_mdc.SetPen(m_hlightcolor);
			m_mdc.SetBrush(wxBrush(m_hlightcolor, wxSOLID));
			m_mdc.DrawRectangle(0, ypos, size.x-2, ypos2-ypos);
		}

		// Draw bookmark
		if (m_showBookmarks && nextBookmark != bookmarks.end() && nextBookmark->line_id == i) {
			//m_mdc.DrawText(wxT("\u066D"), 3, ypos);
			m_mdc.DrawBitmap(m_bmBookmark, 2, ypos + line_middle - 5);
			++nextBookmark;
		}

		// Draw the line number
		m_mdc.DrawText(number, m_numberX, ypos);

		// Draw fold markers
		if (m_showFolds) {
			bool drawFoldLine = (!foldStack.empty());

			if (nextFold != folds.end() && nextFold->line_id == i) {
				if (nextFold->type == cxFOLD_START) {
					const int ypos2 = lines.GetBottomYPosFromLine(i) - scrollPos;
					const unsigned int box_y = ypos + line_middle - 5;
					m_mdc.DrawBitmap(m_bmFoldOpen, m_foldStartX, box_y);

					if (&*nextFold == m_currentFold) m_mdc.SetPen(wxPen(m_edgecolor, 2));
					else m_mdc.SetPen(m_edgecolor);
					m_mdc.DrawLine(m_foldStartX+4, box_y+9, m_foldStartX+4, ypos2);

					foldStack.push_back(&*nextFold);
					drawFoldLine = false;
					++nextFold;
				}
				else if (nextFold->type == cxFOLD_START_FOLDED) {
					const unsigned int box_y = ypos + line_middle - 5;
					m_mdc.DrawBitmap(m_bmFoldClosed, m_foldStartX, box_y);
					drawFoldLine = false;

					// Advance to end of fold
					i += nextFold->count;
					while (nextFold != folds.end() && nextFold->line_id <= i) ++nextFold;
				}
				else if (nextFold->type == cxFOLD_END) {
					if (!foldStack.empty()) {
						// check if end marker matches any starter on the stack (ignore unmatched)
						for (vector<const cxFold*>::reverse_iterator f = foldStack.rbegin(); f != foldStack.rend(); ++f) {
							if (nextFold->indent == (*f)->indent) {
								vector<const cxFold*>::iterator fb = (++f).base();

								// Check if we should highlight fold line
								if (*fb == m_currentFold) m_mdc.SetPen(wxPen(m_edgecolor, 2));
								else m_mdc.SetPen(m_edgecolor);

								// If we are closing other folds, we want to leave a gap
								const unsigned int ytop = (fb < foldStack.end()-1) ? ypos + 2 : ypos;

								// Draw end marker
								const unsigned int middle_y = ypos + line_middle+1;
								m_mdc.DrawLine(m_foldStartX+4, ytop, m_foldStartX+4, middle_y);
								m_mdc.DrawLine(m_foldStartX+4, middle_y, m_foldStartX+9, middle_y);

								foldStack.erase(fb, foldStack.end()); // pop
								drawFoldLine = false;
								break;
							}
						}
					}
					++nextFold;
				}
			}

			if (drawFoldLine) {
				const int ypos2 = lines.GetBottomYPosFromLine(i) - scrollPos;

				// Check if we should highlight fold line
				if (!foldStack.empty() && foldStack.back() == m_currentFold) {
					m_mdc.SetPen(wxPen(m_edgecolor, 2));
				}
				else m_mdc.SetPen(m_edgecolor);

				m_mdc.DrawLine(m_foldStartX+4, ypos, m_foldStartX+4, ypos2);
			}
		}
	}

	// Copy MemoryDC to Display
#ifdef __WXMSW__
	::BitBlt(GetHdcOf(dc), 0, 0,(int)size.x, (int)size.y, GetHdcOf(m_mdc), 0, 0, SRCCOPY);
#else
	dc.Blit(0, 0, size.x, size.y, &m_mdc, 0, 0);
#endif
}


void GutterCtrl::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	// Re-Blit MemoryDC to Display
	wxSize size = GetClientSize();
	dc.Blit(0, 0,  size.x, size.y, &m_mdc, 0, 0);
}


void GutterCtrl::OnSize(wxSizeEvent& WXUNUSED(event)) {
	const wxSize size = GetClientSize();

	if (size.x <= 0 || size.y <= 0) return; // no need to update

	// resize the bitmap used for doublebuffering
	if (m_bitmap.GetWidth() < size.x || m_bitmap.GetHeight() < size.y) {
		m_bitmap = wxBitmap(size.x, size.y);
		m_mdc.SelectObject(m_bitmap);
	}

	// Don't draw the new layout before asked by frame
	// DrawGutter();
}

void GutterCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
	// # no evt.skip() as we don't want the control to erase the background
}

void GutterCtrl::OnMouseLeftDown(wxMouseEvent& event) {
	//wxLogDebug("OnMouseLeftDown");
	//wxASSERT(m_editorCtrl);
	wxASSERT(m_currentSel == -1);
	Lines& lines = m_editorCtrl.m_lines;

	// Get Mouse location
	const int x = event.GetX();
	const int y = event.GetY() + m_editorCtrl.scrollPos;

	// Handle bookmarks
	if (m_showBookmarks && x < (int)m_numberX) {
		// Find out which line was clicked on
		Lines& lines = m_editorCtrl.m_lines;
		if ((int)y < lines.GetHeight()) {
			const unsigned int line_id = lines.GetLineFromYPos(y);
			m_editorCtrl.AddBookmark(line_id, true /*toggle*/);
			DrawGutter(); // Redraw gutter to show bookmarks
			return;
		}
	}

	// Handle folding
	if (m_showFolds && x > (int)m_foldStartX) {
		ClickOnFold(y);
		return;
	}

	bool hasSelection = false;
	interval sel(0, 0);
	if (event.ShiftDown() && lines.IsSelected()) {
		sel = lines.GetSelections()[lines.GetLastSelection()];
		hasSelection = true;
	}

	// If not multiselecting or extending, remove previous selections
	if (!event.ControlDown()) {
		lines.RemoveAllSelections();
	}

	// Find out which line was clicked on
	if (y < lines.GetHeight()) {
		const unsigned int line_id = lines.GetLineFromYPos(y);

		// Select the line
		if (!lines.isLineVirtual(line_id)) {
			int startpos = lines.GetLineStartpos(line_id);
			int endpos = lines.GetLineEndpos(line_id, false);

			if (hasSelection) {
				startpos = wxMin(startpos, (int)sel.start);
				endpos = wxMax(endpos, (int)sel.end);
			}

			m_currentSel = lines.AddSelection(startpos, endpos);
			lines.SetPos(endpos);
			m_editorCtrl.SetFocus();

			m_sel_startline = m_sel_endline = line_id;
		}
		m_sel_startoutside = false;
	}
	else {
		const unsigned int linecount = lines.GetLineCount();
		m_sel_startline = m_sel_endline = linecount ? linecount-1 : 0;
		m_sel_startoutside = true;

		if (hasSelection) {
			m_currentSel = lines.AddSelection(sel.start, lines.GetLength());
			lines.SetPos(lines.GetLength());
			m_editorCtrl.SetFocus();
		}
	}

	// Make sure we capure all mouse events
	// this is released in OnMouseLeftUp()
	CaptureMouse();

	// Redraw the editCtrl to show new selection
	m_editorCtrl.DrawLayout();
}

void GutterCtrl::OnMouseLeftDClick(wxMouseEvent& event) {
	if (!m_showFolds) return;
	if (event.GetX() > (int)m_foldStartX) {
		const int y = event.GetY() + m_editorCtrl.scrollPos;
		Lines& lines = m_editorCtrl.m_lines;

		if (0 <= y && y < lines.GetHeight()) {
			const unsigned int line_id = lines.GetLineFromYPos(y);

			vector<cxFold*> foldStack = m_editorCtrl.GetFoldStack(line_id);
			if (!foldStack.empty() && foldStack.back()->type == cxFOLD_START) {
				lines.RemoveAllSelections(); // first click will have selected fold
				if (m_posBeforeFoldClick != -1) lines.SetPos(m_posBeforeFoldClick); // and moved pos

				m_editorCtrl.Fold(foldStack.back()->line_id);
				m_editorCtrl.DrawLayout();
			}
		}
	}
}

void GutterCtrl::ClickOnFold(unsigned int y) {
	Lines& lines = m_editorCtrl.m_lines;
	m_posBeforeFoldClick = -1;

	// Find out which line was clicked on
	if ((int)y < lines.GetHeight()) {
		const unsigned int line_id = lines.GetLineFromYPos(y);

		// Check if it is a fold marker
		const vector<cxFold>& folds = m_editorCtrl.GetFolds();
		const cxFold target(line_id);
		vector<cxFold>::const_iterator p = lower_bound(folds.begin(), folds.end(), target);

		if (p != folds.end() && p->line_id == line_id) {
			if (p->type == cxFOLD_START) {
				m_editorCtrl.Fold(line_id);
				m_editorCtrl.DrawLayout();
				return;
			}
			else if (p->type == cxFOLD_START_FOLDED) {
				m_editorCtrl.UnFold(line_id);
				m_editorCtrl.DrawLayout();
				return;
			}
		}

		// If a fold line is clicked, we want to select the entire fold
		m_posBeforeFoldClick = lines.GetPos(); // cache for doubleclick
		m_editorCtrl.SelectFold(line_id);
		m_editorCtrl.DrawLayout();
	}
}

void GutterCtrl::OnMouseMotion(wxMouseEvent& event) {
	Lines& lines = m_editorCtrl.m_lines;

	// Get Mouse location
	const int y = event.GetY() + m_editorCtrl.scrollPos;

	if (event.LeftIsDown() && HasCapture()) {
		// Find out what is under mouse
		unsigned int line_id;
		if (y < 0) line_id = 0;
		else if (y < lines.GetHeight()) {
			line_id = lines.GetLineFromYPos(y);
		}
		else {
			if (m_sel_startoutside && m_currentSel != -1) {
				// Make sure we remove current selection
				m_currentSel = lines.UpdateSelection(m_currentSel, 0, 0);
				lines.SetPos(lines.GetLength());
				m_editorCtrl.DrawLayout();
				return;
			}

			const unsigned int linecount = lines.GetLineCount();
			line_id = linecount ? linecount-1 : 0;
		}

		// Select the lines
		if (line_id != m_sel_endline) {
			m_sel_endline = line_id;

			int sel_start = lines.GetLineStartpos(wxMin(m_sel_startline, m_sel_endline));
			int sel_end = lines.GetLineEndpos(wxMax(m_sel_startline, m_sel_endline), false);

			if (sel_start == sel_end) {
				lines.RemoveAllSelections();
				m_currentSel = -1;
			}
			else {
				// Update the lines selection info
				if (m_currentSel == -1) {
					m_currentSel = lines.AddSelection(sel_start, sel_end);
				}
				else {
					m_currentSel = lines.UpdateSelection(m_currentSel, sel_start, sel_end);
				}
				lines.SetPos(m_sel_endline < m_sel_startline ? sel_start : sel_end);
			}

			m_editorCtrl.MakeCaretVisible(); // also ensures scrolling if outside window
			m_editorCtrl.DrawLayout();
		}
	}
	else if (event.GetX() > (int)m_foldStartX && 0 <=y && y < lines.GetHeight()) {
		const unsigned int line_id = lines.GetLineFromYPos(y);
		vector<cxFold*> foldStack = m_editorCtrl.GetFoldStack(line_id);
		if (!foldStack.empty()) {
			m_currentFold = foldStack.back();
			DrawGutter(); // Redraw gutter to show highlights
			return;
		}
	}

	if (m_currentFold) {
		m_currentFold = NULL;
		DrawGutter(); // Redraw gutter to remove highlights
	}
}

void GutterCtrl::OnMouseLeave(wxMouseEvent& WXUNUSED(event)) {
	if (m_currentFold) {
		m_currentFold = NULL;
		DrawGutter(); // Redraw gutter to remove highlights
	}
}

void GutterCtrl::OnMouseLeftUp(wxMouseEvent& WXUNUSED(event)) {
	if (HasCapture()) {
		// Reset state variables
		m_currentSel = -1;

		// Release the capure made in OnMouseLeftDown()
		ReleaseMouse();

		// Redraw gutter to remove highlights
		DrawGutter();
	}
}

void GutterCtrl::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event)) {
	// Reset state variables
	m_currentSel = -1;

	// Redraw gutter to remove highlights
	DrawGutter();
}
