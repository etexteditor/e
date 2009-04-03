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

#include "Cell.h"


Cell::Cell(const wxDC& dc) : dc(dc), width(0), height(0) {
}

void Cell::Destroy() {
	delete this;
}

const int Cell::GetWidth() {
	return width;
}

void Cell::SetStyle(const wxColour& fc, const wxColour& bc) {
	fcolor = fc;
	bgcolor = bc;
	bgmode = wxSOLID;
}


// ----- WordCell -----------------------------------

WordCell::WordCell(const wxDC& dc, const wxString& wrd) : Cell(dc) {
	wxCoord w;
	wxCoord h;
	dc.GetTextExtent(wrd, &w, &h);
	width = w;
	height = h;

	fcolor = *wxBLACK;
    bgcolor = *wxWHITE;
    bgmode = wxTRANSPARENT;

	word = wrd;
}

void WordCell::Destroy() {
	delete this;
}

void WordCell::DrawCell(int xoffset, int yoffset, const wxRect& WXUNUSED(rect)) {
	wxDC& mydc = const_cast<wxDC&>(dc);
	mydc.SetTextForeground(fcolor);
    mydc.SetTextBackground(bgcolor);
    mydc.SetBackgroundMode(bgmode);
        
    mydc.DrawText(word, xoffset, yoffset);
}


// ----- DiffLineCell -----------------------------------

DiffLineCell::DiffLineCell(const wxDC& dc, const DocumentWrapper& dw) 
: Cell(dc), m_doc(dw), line_width(0), char_width(0), subCells(), top_xpos() {
	m_insertColour.Set(192, 255, 192); // PASTEL GREEN
	m_deleteColour.Set(255, 192, 192); // PASTEL RED
	m_hiddenColour = wxTheColourDatabase->Find(wxT("GREY"));
	bgcolor = *wxWHITE;
}

DiffLineCell::~DiffLineCell() {
	Clear();
}

void DiffLineCell::Clear() {
	// Delete subCells
	for(vector<Cell*>::iterator p = subCells.begin(); p != subCells.end(); ++p) {
		(*p)->Destroy();
	}
	subCells.clear();
	top_xpos.clear();
	line_width = 0;
}

void DiffLineCell::AddSubCell(Cell* cell) {
	top_xpos.push_back(line_width);
	line_width += cell->GetWidth();
	subCells.push_back(cell);
}

void DiffLineCell::CalcLayout(const doc_id& rev1, const doc_id& rev2) {
	wxASSERT(rev1.IsOk());
	wxASSERT(rev2.IsOk());
	wxASSERT(char_width > 0);

	Clear();

	cxLOCKDOC_READ(m_doc)
		if (rev1 == rev2) { // root has itself as parent
			int length = doc.GetVersionLength(rev2);
			if (length > 0) {
				length = wxMin(length, (int)doc.GetCharPos(rev2, 0, (width/char_width)+1));
				wxString text = doc.GetTextPart(rev2, 0, length);
				CalcSubCells(text, *wxBLACK, m_insertColour);
			}
		}
		else if (!doc.TextChanged(rev1, rev2)) {
			if (doc.PropertiesChanged(rev1, rev2)) CalcSubCells(_("Properties Changed"), *wxBLACK, bgcolor);
		}
		else {
			bool change = false;
			const vector<match> matchlist = doc.Diff(rev1, rev2);

			unsigned int pos1 = 0;
			unsigned int pos2 = 0;

			unsigned int i = 0;
			while (i < matchlist.size()) { 
				if (line_width > width) {
					// No need to parse something that won't be drawn
					break;
				}

				// Calculate how many chars there are room for in
				// the visible part. Then we can avoid retrieving
				// long strings (which can be *very* long)
				int chars_left = ((width-line_width)/char_width)+1;

				match m = matchlist[i];
				if (pos1 < m.iv1_start_pos) { // Deleted
					unsigned int last_endpos = doc.GetCharPos(rev1, pos1, chars_left);
					CalcSubCells(doc.GetTextPart(rev1, pos1, wxMin(m.iv1_start_pos, last_endpos)), *wxBLACK, m_deleteColour); // PASTEL RED
					change = true;
				}
				if (pos2 < m.iv2_start_pos) { // Inserted
					unsigned int last_endpos = doc.GetCharPos(rev2, pos2, chars_left);
					CalcSubCells(doc.GetTextPart(rev2, pos2, wxMin(m.iv2_start_pos, last_endpos)), *wxBLACK, m_insertColour); // PASTEL GREEN
					change = true;
				}
				if (m.iv2_end_pos > 0 && m.iv2_end_pos > m.iv2_start_pos) {
					if (change) {
						unsigned int last_endpos = doc.GetCharPos(rev2, m.iv2_start_pos, chars_left);
						CalcSubCells(doc.GetTextPart(rev2, m.iv2_start_pos, wxMin(m.iv2_end_pos, last_endpos)), m_hiddenColour, bgcolor);
					}
					else {
						// Get up to 5 chars before first change
						unsigned int startpos = doc.GetCharPos(rev2, m.iv2_end_pos, -5);
						wxString initialText = doc.GetTextPart(rev2, wxMax(m.iv2_start_pos, startpos), m.iv2_end_pos);
						CalcSubCells(initialText, m_hiddenColour, bgcolor);
					}
				}

				pos1 = m.iv1_end_pos;
				pos2 = m.iv2_end_pos;

				++i;
			}
		}
	cxENDLOCK
}

void DiffLineCell::CalcLayoutRange(const doc_id& rev1, const doc_id& rev2, const interval& range) {
	wxASSERT(rev1.IsOk());
	wxASSERT(rev2.IsOk());
	wxASSERT(char_width > 0);

	Clear();

	cxLOCKDOC_READ(m_doc)
		if (rev1 == rev2) { // root has itself as parent
			// Get up to 5 chars before range
			if (range.start > 0) {
				const unsigned int startpos = doc.GetCharPos(rev2, range.start, -5);
				const wxString initialText = doc.GetTextPart(rev2, wxMax(0, startpos), range.start);
				CalcSubCells(initialText, m_hiddenColour, bgcolor);
			}

			// Get the range
			unsigned int chars_left = ((width - line_width) / char_width)+1;
			unsigned int end = wxMin(range.end, doc.GetCharPos(rev2, range.start, chars_left));
			unsigned int length = end - range.start;
			if (length == 0) return;
			const wxString text = doc.GetTextPart(rev2, range.start, length);
			CalcSubCells(text, *wxBLACK, m_insertColour);

			// Get text after range
			chars_left = ((width - line_width) / char_width)+1;
			length = doc.GetCharPos(rev2, range.end, chars_left);
			if (length == 0) return;
			const wxString text2 = doc.GetTextPart(rev2, range.start, length);
			CalcSubCells(text2, m_hiddenColour, bgcolor);
		}
		else {
			bool change = false;
			const vector<match> matchlist = doc.Diff(rev1, rev2);

			unsigned int pos1 = 0;
			unsigned int pos2 = 0;

			// Advance to range start (ignoring sentinel at end)
			unsigned int i = 0;
			while (i+1 < matchlist.size() && matchlist[i].iv2_end_pos < range.start) {
				pos1 = matchlist[i].iv1_end_pos;
				pos2 = matchlist[i].iv2_end_pos;
				++i;
			}

			while (i < matchlist.size()) { 
				if (line_width > width) break; // No need to parse something that won't be drawn

				// Calculate how many chars there are room for in
				// the visible part. Then we can avoid retrieving
				// long strings (which can be *very* long)
				int chars_left = ((width-line_width)/char_width)+1;

				const match& m = matchlist[i];
				if (pos1 < m.iv1_start_pos && pos2 >= range.start && pos2 <= range.end) { // Deleted (and in range)
					const unsigned int last_endpos = doc.GetCharPos(rev1, pos1, chars_left);
					CalcSubCells(doc.GetTextPart(rev1, pos1, wxMin(m.iv1_start_pos, last_endpos)), *wxBLACK, m_deleteColour); // PASTEL RED
					change = true;
				}
				if (pos2 < m.iv2_start_pos && range.start < m.iv2_start_pos) { // Inserted
					// Get text before range
					if (pos2 < range.start) {
						const unsigned int ins_start = wxMax(pos2, doc.GetCharPos(rev2, range.start, -5));
						CalcSubCells(doc.GetTextPart(rev2, ins_start, range.start), m_hiddenColour, bgcolor);
					}

					// Get text in range (only if overlap)
					if (pos2 < range.end && m.iv2_start_pos > range.start) {
						const unsigned int start_pos = wxMax(pos2, range.start);
						const unsigned int last_endpos = wxMin(wxMin(range.end, m.iv2_start_pos), doc.GetCharPos(rev2, start_pos, chars_left));
						CalcSubCells(doc.GetTextPart(rev2, start_pos, last_endpos), *wxBLACK, m_insertColour); // PASTEL GREEN
					}

					// Get text after range
					if (m.iv2_start_pos > range.end) {
						const unsigned int rest_start = wxMax(pos2, range.end);
						const unsigned int rest_end = wxMin(m.iv2_start_pos, doc.GetCharPos(rev2, rest_start, chars_left));
						CalcSubCells(doc.GetTextPart(rev2, rest_start, rest_end), m_hiddenColour, bgcolor);
					}
					
					change = true;
				}
				if (m.iv2_end_pos > 0 && m.iv2_end_pos > m.iv2_start_pos) {
					if (change) {
						const unsigned int last_endpos = doc.GetCharPos(rev2, m.iv2_start_pos, chars_left);
						CalcSubCells(doc.GetTextPart(rev2, m.iv2_start_pos, wxMin(m.iv2_end_pos, last_endpos)), m_hiddenColour, bgcolor);
					}
					else {
						// Get up to 5 chars before first change
						const unsigned int startpos = doc.GetCharPos(rev2, m.iv2_end_pos, -5);
						const wxString initialText = doc.GetTextPart(rev2, wxMax(m.iv2_start_pos, startpos), m.iv2_end_pos);
						CalcSubCells(initialText, m_hiddenColour, bgcolor);
					}
				}

				pos1 = m.iv1_end_pos;
				pos2 = m.iv2_end_pos;
				++i;
			}
		}
	cxENDLOCK
}

void DiffLineCell::CalcSubCells(const wxString& text, const wxColour& fc, const wxColour& bc) {
	unsigned int wordstart = 0;
	unsigned int char_id = 0;

	for(char_id = 0; char_id < text.length(); ++char_id) {
		if (line_width < width) {
			if (text[char_id] == '\n') {
				if (wordstart < char_id) {
					wxString word = text.substr(wordstart, char_id-wordstart);
					WordCell *newcell = new WordCell(dc, word);
					newcell->SetStyle(fc, bc);
					AddSubCell(newcell);
				}
				NewlineImageCell *newcell = new NewlineImageCell(dc); // NewlineMarker
				newcell->SetStyle(fc, bc);
				AddSubCell(newcell);
				wordstart = char_id + 1;
			}
			else if (text[char_id] != ' ') {
				if (text[char_id] == '\t') {
					if (wordstart < char_id) {
						wxString word = text.substr(wordstart, char_id-wordstart);
						WordCell *newcell = new WordCell(dc, word);
						newcell->SetStyle(fc, bc);
						AddSubCell(newcell);
					}
					TabImageCell *newcell = new TabImageCell(dc); // TabMarker
					newcell->SetStyle(fc, bc);
					AddSubCell(newcell);
					wordstart = char_id + 1;
				}
				else if (char_id && (text[char_id-1] == ' ' || text[char_id-1] == '-')) {
					// End of word-cell reached
					wxString word = text.substr(wordstart, char_id-wordstart);
					WordCell *newcell = new WordCell(dc, word);
					newcell->SetStyle(fc, bc);
					AddSubCell(newcell);
					wordstart = char_id;
				}
			}
		}
	}

	if (wordstart != text.size() && line_width < width) {
		// Add last word
		wxString word = text.substr(wordstart);
		WordCell *newcell = new WordCell(dc, word);
		newcell->SetStyle(fc, bc);
		AddSubCell(newcell);
		wordstart = char_id;
	}
}

void DiffLineCell::DrawCell(int xoffset, int yoffset, const wxRect& rect) {
	for(unsigned int i = 0; i < subCells.size(); ++i) {
		(subCells[i])->DrawCell(xoffset + top_xpos[i], yoffset, rect); 
	}
}

int DiffLineCell::SetWidth(int w) {
	wxASSERT(w >= 0);

	width = w;

	if (char_width == 0) {
		// Get the with of a single char
		// needed to avoid loading more text than can be seen
		wxCoord cw;
		wxCoord ch;
		dc.GetTextExtent(wxT("x"), &cw, &ch);
		char_width = cw;
	}

	return width;
}

// ----- NewlineImageCell -----------------------------------

NewlineImageCell::NewlineImageCell(const wxDC& dc) : Cell(dc) {
	wxCoord w;
	wxCoord h;

	dc.GetTextExtent(wxT("X"), &w, &h);
	width = w;
	height = h;
}

void NewlineImageCell::DrawCell(int xoffset, int yoffset, const wxRect& WXUNUSED(rect)) {
	wxDC& mydc = const_cast<wxDC&>(dc);
	mydc.SetPen(wxPen(bgcolor, 1, wxSOLID));
	mydc.SetBrush(wxBrush(bgcolor, wxSOLID));
	mydc.DrawRectangle(xoffset, yoffset, width, height);

	int end_xpos = xoffset + width;
    int x_middle = width/2;
    int y_fourth = height/4;

	mydc.SetPen(wxPen(fcolor, 1, wxSOLID));
	mydc.SetBrush(wxBrush(fcolor, wxSOLID));
    mydc.DrawLine(end_xpos-2, yoffset+y_fourth, end_xpos-2, yoffset + height-y_fourth-1);
    mydc.DrawLine(xoffset+2, yoffset + height-y_fourth-2, end_xpos-2, yoffset + height-y_fourth-2);

	wxPoint triangle_points[3];
    triangle_points[0] = wxPoint(0, height-y_fourth-2);
	triangle_points[1] = wxPoint(x_middle, height-y_fourth-5);
    triangle_points[2] = wxPoint(x_middle, height-y_fourth+1);
	mydc.DrawPolygon(3, triangle_points, xoffset, yoffset);
}

// ----- TabImageCell -----------------------------------

TabImageCell::TabImageCell(const wxDC& dc) : Cell(dc) {
	wxCoord w;
	wxCoord h;

	dc.GetTextExtent(wxT("X"), &w, &h);
	width = w;
	height = h;
}

void TabImageCell::DrawCell(int xoffset, int yoffset, const wxRect& WXUNUSED(rect)) {
	wxDC& mydc = const_cast<wxDC&>(dc);
	mydc.SetPen(wxPen(bgcolor, 1, wxSOLID));
	mydc.SetBrush(wxBrush(bgcolor, wxSOLID));
	mydc.DrawRectangle(xoffset, yoffset, width, height);

	int end_xpos = xoffset + width;
	int x_middle = width/2;
    int y_middle = height/2;
    int y_fourth = height/4;

	mydc.SetPen(wxPen(fcolor, 1, wxSOLID));
	mydc.SetBrush(wxBrush(fcolor, wxSOLID));
    mydc.DrawLine(xoffset, yoffset + y_middle, end_xpos, yoffset + y_middle);

	wxPoint triangle_points[3];
    triangle_points[0] = wxPoint(x_middle, y_fourth);
	triangle_points[1] = wxPoint(width-1, y_middle);
    triangle_points[2] = wxPoint(x_middle, height-y_fourth);
	mydc.DrawPolygon(3, triangle_points, xoffset, yoffset);
}
