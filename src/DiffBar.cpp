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

#include "DiffBar.h"
#include "EditorCtrl.h"
#include "StyleRun.h"
#include "DiffPanel.h"

BEGIN_EVENT_TABLE(DiffBar, wxControl)
	EVT_PAINT(DiffBar::OnPaint)
	EVT_SIZE(DiffBar::OnSize)
	EVT_ERASE_BACKGROUND(DiffBar::OnEraseBackground)
	EVT_LEFT_UP(DiffBar::OnMouseLeftUp)
	EVT_MOTION(DiffBar::OnMouseMotion)
	EVT_LEAVE_WINDOW(DiffBar::OnMouseLeave)
END_EVENT_TABLE()

const unsigned int DiffBar::s_bracketWidth = 5;

DiffBar::DiffBar(wxWindow* parent, CatalystWrapper& cw, EditorCtrl* leftEditor, EditorCtrl* rightEditor):
	wxControl(parent, wxID_ANY, wxPoint(-100,-100), wxSize(40,100), wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE),
	m_catalyst(cw), m_leftEditor(leftEditor), m_rightEditor(rightEditor), m_leftStyler(m_diffs, true), m_rightStyler(m_diffs, false),
	m_needRedraw(false), m_needTransform(false), m_highlight(-1) 
{
	SetMinSize(wxSize(40, -1));
	SetMaxSize(wxSize(40, -1));
}

void DiffBar::Init() {
	// Register stylers
	m_leftEditor->AddStyler(m_leftStyler);
	m_rightEditor->AddStyler(m_rightStyler);
}

void DiffBar::SetCallbacks() {
	m_leftEditor->SetBeforeRedrawCallback((void(*)(void*))OnBeforeEditorRedraw, this);
	m_leftEditor->SetAfterRedrawCallback((void(*)(void*))OnLeftEditorRedrawn, this);

	m_rightEditor->SetBeforeRedrawCallback((void(*)(void*))OnBeforeEditorRedraw, this);
	m_rightEditor->SetAfterRedrawCallback((void(*)(void*))OnRightEditorRedrawn, this);

	m_leftEditor->SetScrollCallback((void(*)(void*))OnLeftEditorScroll, this);
	m_rightEditor->SetScrollCallback((void(*)(void*))OnRightEditorScroll, this);
}

void DiffBar::SetDiff() {
	// Clean up
	m_matchlist.clear();

	// Get the diff
	DocumentWrapper& doc1 = m_leftEditor->GetDocument();
	DocumentWrapper& doc2 = m_rightEditor->GetDocument();
	cxLOCK_WRITE(m_catalyst)
		Document& d1 = doc1.GetDoc();
		Document& d2 = doc2.GetDoc();
		catalyst.GetDiff(d1, d2, m_matchlist);

		// Set callbacks
		d1.SetChangeCallback(OnLeftDocumentChanged, this);
		d2.SetChangeCallback(OnRightDocumentChanged, this);
	cxENDLOCK

	m_needTransform = true;
}

void DiffBar::TransformMatchlist() {
	m_diffs.clear();
	m_diffs.reserve(m_matchlist.size()+2);
	m_lineMatches.clear();
	m_lineMatches.reserve(m_matchlist.size());

	// Transform to insertions/deletions
	size_t pos1 = 0;
	size_t pos2 = 0;
	for (list<cxMatch>::const_iterator p = m_matchlist.begin(); p != m_matchlist.end(); ++p) {
		if (pos1 < p->start1()) { // Deleted
			m_diffs.push_back(Change(cxDELETION, pos1, p->start1(), pos2));
		}
		if (pos2 < p->start2()) { // Inserted
			m_diffs.push_back(Change(cxINSERTION, pos2, p->start2(), pos1));
		}

		pos1 = p->end1();
		pos2 = p->end2();
	}

	// Find matching lines in editorCtrls
	unsigned int leftCount = 0;
	unsigned int rightCount = 0;
	for (std::vector<Change>::const_iterator c = m_diffs.begin(); c != m_diffs.end(); ++c) {
		LineMatch m;

		// Convert matches to lines
		if (c->type == cxDELETION) {
			m.left_start = m_leftEditor->GetLineFromPos(c->start_pos);
			m.left_end = m_leftEditor->GetLineFromPos(c->end_pos);
			const bool toLineEnd = m_leftEditor->IsLineEnd(c->end_pos);
			if (!m_leftEditor->IsLineStart(m.left_end, c->end_pos)) ++m.left_end;
			m.right_start = m_rightEditor->GetLineFromPos(c->rev_pos);
			m.right_end = (toLineEnd && m_rightEditor->IsLineStart(m.right_start, c->rev_pos)) ? m.right_start : m.right_start+1;
			leftCount = c->end_pos - c->start_pos;
		}
		else {
			m.right_start = m_rightEditor->GetLineFromPos(c->start_pos);
			m.right_end = m_rightEditor->GetLineFromPos(c->end_pos);
			const bool toLineEnd = m_rightEditor->IsLineEnd(c->end_pos);
			if (!m_rightEditor->IsLineStart(m.right_end, c->end_pos)) ++m.right_end;
			m.left_start = m_leftEditor->GetLineFromPos(c->rev_pos);
			m.left_end = (toLineEnd && m_leftEditor->IsLineStart(m.left_start, c->rev_pos)) ? m.left_start : m.left_start+1;
			rightCount = c->end_pos - c->start_pos;
		}

		// Make sure markers will get correct coloring
		m.left_type = (leftCount > 0) ? cxDELETION : cxINSERTION;
		m.right_type = (rightCount > 0) ? cxINSERTION : cxDELETION;	

		if (m_lineMatches.empty()) m_lineMatches.push_back(m);
		else {
			// Check if matches overlap
			LineMatch& b = m_lineMatches.back();
			if (m.left_start <= b.left_end && m.right_start <= b.right_end) {
				b.left_end = wxMax(m.left_end, b.left_end);
				b.right_end = wxMax(m.right_end, b.right_end);
				if (m.left_type == cxDELETION) b.left_type = cxDELETION;
				if (m.right_type == cxINSERTION) b.right_type = cxINSERTION;
			}
			else {
				m_lineMatches.push_back(m);
			}
			leftCount = rightCount = 0;
		}
	}

	m_needTransform = false;
	((DiffPanel*)m_parent)->UpdateMarkBars();
}

void DiffBar::Swap() {
	for (list<cxMatch>::iterator p = m_matchlist.begin(); p != m_matchlist.end(); ++p) {
		const size_t temp = p->offset1;
		p->offset1 = p->offset2;
		p->offset2 = temp;
	}

	EditorCtrl* temp = m_leftEditor;
	m_leftEditor = m_rightEditor;
	m_rightEditor = temp;

	m_leftStyler.SwapSide();
	m_rightStyler.SwapSide();

	// Re-set the editor callbacks
	SetCallbacks();

	// Re-set document callbacks
	DocumentWrapper& doc1 = m_leftEditor->GetDocument();
	DocumentWrapper& doc2 = m_rightEditor->GetDocument();
	cxLOCKDOC_WRITE(doc1)
		Document& d2 = doc2.GetDoc();
		doc.SetChangeCallback(OnLeftDocumentChanged, this);
		d2.SetChangeCallback(OnRightDocumentChanged, this);
	cxENDLOCK

	TransformMatchlist();
	m_needRedraw = true;
}

void DiffBar::DrawLayout(wxDC& dc) {
	const wxSize size = GetClientSize();
	const unsigned int leftIndentPos = s_bracketWidth;
	const unsigned int rightIndentPos = size.x - s_bracketWidth;

	const unsigned int leftTopLine = m_leftEditor->GetTopLine();
	const unsigned int rightTopLine = m_rightEditor->GetTopLine();

	// Ignore all matches above toplines
	std::vector<LineMatch>::const_iterator p = m_lineMatches.begin();
	for (; p != m_lineMatches.end(); ++p) {
		if (p->left_end >= leftTopLine || p->right_end >= rightTopLine) break;
	}

	Lines& leftLines = m_leftEditor->GetLines();
	Lines& rightLines = m_rightEditor->GetLines();
	const unsigned int leftScrollPos = m_leftEditor->GetScrollPos();
	const unsigned int rightScrollPos = m_rightEditor->GetScrollPos();

	// Draw all visible matches
	dc.Clear();
	dc.SetPen(*wxGREY_PEN);
	for (; p != m_lineMatches.end(); ++p) {
		const int leftTop = leftLines.GetYPosFromLine(p->left_start) - leftScrollPos;
		const int rightTop = rightLines.GetYPosFromLine(p->right_start) - rightScrollPos;
		
		// Break if outside visible
		if (leftTop > size.y && rightTop > size.y) break;

		int leftBottom = leftTop;
		int rightBottom = rightTop;

		// Highlight on mouseover
		if ((int)p->left_start == m_highlight) {
			//wxPen hlPen(*wxGREY_PEN);
			//hlPen.SetWidth(2);
			dc.SetPen(*wxBLACK_PEN);
		}

		// Draw left bracket
		dc.DrawLine(0, leftTop, leftIndentPos, leftTop);
		if (p->left_start != p->left_end) {
			leftBottom = leftLines.GetBottomYPosFromLine(p->left_end-1) - leftScrollPos;
			dc.DrawLine(leftIndentPos, leftTop, leftIndentPos, leftBottom);
			dc.DrawLine(0, leftBottom, leftIndentPos, leftBottom);
		}

		// Draw right bracket
		dc.DrawLine(rightIndentPos, rightTop, size.x, rightTop);
		if (p->right_start != p->right_end) {
			rightBottom = rightLines.GetBottomYPosFromLine(p->right_end-1) - rightScrollPos;
			dc.DrawLine(rightIndentPos, rightTop, rightIndentPos, rightBottom);
			dc.DrawLine(rightIndentPos, rightBottom, size.x, rightBottom);
		}

		// Draw connecting line
		dc.DrawLine(leftIndentPos, (leftTop + leftBottom) / 2, rightIndentPos, (rightTop + rightBottom) / 2);

		if ((int)p->left_start == m_highlight) dc.SetPen(*wxGREY_PEN); // reset
	}
}

void DiffBar::OnMouseLeave(wxMouseEvent& WXUNUSED(evt)) {
	if (m_highlight != -1) {
		m_highlight = -1;
		wxClientDC dc(this);
		DrawLayout(dc);
	}
}

std::vector<DiffBar::LineMatch>::const_iterator DiffBar::OnLeftBracket(int y) {
	const int ypos = y + m_leftEditor->GetScrollPos();
	Lines& lines = m_leftEditor->GetLines();
	if (ypos > lines.GetHeight()) return m_lineMatches.end();

	const unsigned int line_id = lines.GetLineFromYPos(ypos);

	// Are we over a bracket?
	std::vector<LineMatch>::const_iterator p = m_lineMatches.begin();
	for (; p != m_lineMatches.end(); ++p) {
		if (p->left_end >= line_id) break;
	}

	if (p != m_lineMatches.end()) {
		if (p->left_end == line_id){
			// Only matches between lines are hit by line below
			if (p->left_start == p->left_end) return p;
		}
		else if (p->left_start <= line_id) return p;
	}

	return m_lineMatches.end();
}

std::vector<DiffBar::LineMatch>::const_iterator DiffBar::OnRightBracket(int y) {
	const int ypos = y + m_rightEditor->GetScrollPos();
	Lines& lines = m_rightEditor->GetLines();
	if (ypos > lines.GetHeight()) return m_lineMatches.end();

	const unsigned int line_id = lines.GetLineFromYPos(ypos);

	// Are we over a bracket?
	std::vector<LineMatch>::const_iterator p = m_lineMatches.begin();
	for (; p != m_lineMatches.end(); ++p) {
		if (p->right_end >= line_id) break;
	}

	if (p != m_lineMatches.end()) {
		if (p->right_end == line_id){
			// Only matches between lines are hit by line below
			if (p->right_start == p->right_end) return p;
		}
		else if (p->right_start <= line_id) return p;
	}

	return m_lineMatches.end();
}

void DiffBar::OnMouseMotion(wxMouseEvent& evt) {
	const wxSize size = GetClientSize();
	const wxPoint mpos = evt.GetPosition();
	const unsigned int rightBracket = size.x - s_bracketWidth;
	int highlight = -1;

	if (mpos.x <= (int)s_bracketWidth) {
		std::vector<LineMatch>::const_iterator p = OnLeftBracket(mpos.y);
		if( p != m_lineMatches.end()) highlight = p->left_start;
	}
	else if (mpos.x >= (int)rightBracket) {
		std::vector<LineMatch>::const_iterator p = OnRightBracket(mpos.y);
		if( p != m_lineMatches.end()) highlight = p->left_start;
	}

	if (m_highlight == highlight) return;
	m_highlight = highlight;
	wxClientDC dc(this);
	DrawLayout(dc);
}

void DiffBar::OnMouseLeftUp(wxMouseEvent& evt) {
	const wxSize size = GetClientSize();
	const wxPoint mpos = evt.GetPosition();
	
	if (mpos.x <= (int)s_bracketWidth) {
		// Have we clicked a bracket?
		std::vector<LineMatch>::const_iterator p = OnLeftBracket(mpos.y);
		if (p == m_lineMatches.end()) return;

		// Get positions
		Lines& lines = m_leftEditor->GetLines();
		Lines& rightLines = m_rightEditor->GetLines();
		const unsigned int left_start = lines.GetLineStartpos(p->left_start);
		const unsigned int left_end = lines.GetLineStartpos(p->left_end);
		const unsigned int right_start = rightLines.GetLineStartpos(p->right_start);
		const unsigned int right_end = rightLines.GetLineStartpos(p->right_end);

		// Copy all changes contained in this match
		if (right_start < right_end) m_rightEditor->RawDelete(right_start, right_end);
		if (left_start < left_end) {
			const wxString text = m_leftEditor->GetText(left_start, left_end);
			m_rightEditor->RawInsert(right_start, text);
		}
		m_rightEditor->Freeze();

		// Adjust matches
		SetDiff();
		m_needRedraw = true;

		// Redraw all
		m_leftEditor->ReDraw();
		m_rightEditor->ReDraw();
	}
	else if (mpos.x >= size.x - (int)s_bracketWidth) {
		// Have we clicked a bracket?
		std::vector<LineMatch>::const_iterator p = OnRightBracket(mpos.y);
		if (p == m_lineMatches.end()) return;

		// Get positions
		Lines& lines = m_leftEditor->GetLines();
		Lines& rightLines = m_rightEditor->GetLines();
		const unsigned int left_start = lines.GetLineStartpos(p->left_start);
		const unsigned int left_end = lines.GetLineStartpos(p->left_end);
		const unsigned int right_start = rightLines.GetLineStartpos(p->right_start);
		const unsigned int right_end = rightLines.GetLineStartpos(p->right_end);

		// Copy all changes contained in this match
		if (left_start < left_end) m_leftEditor->RawDelete(left_start, left_end);
		if (right_start < right_end) {
			const wxString text = m_rightEditor->GetText(right_start, right_end);
			m_leftEditor->RawInsert(left_start, text);
		}
		m_leftEditor->Freeze();

		// Adjust matches
		SetDiff();
		m_needRedraw = true;

		// Redraw all
		m_leftEditor->ReDraw();
		m_rightEditor->ReDraw();
	}
}

void DiffBar::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	DrawLayout(dc);
}

void DiffBar::OnSize(wxSizeEvent& WXUNUSED(event)) {
	wxClientDC dc(this);
	DrawLayout(dc);
}

// # no evt.skip() as we don't want the control to erase the background
void DiffBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

void DiffBar::OnBeforeEditorRedraw(void* data) { // static
	DiffBar* self = (DiffBar*)data;
	if (self->m_needTransform) self->TransformMatchlist();
}

void DiffBar::OnLeftEditorRedrawn(void* data) { // static
	DiffBar* self = (DiffBar*)data;
	if (!self->m_needRedraw) return;
	self->m_needRedraw = false;

	// Mod will also affect highlighting in opposite editor
	self->m_rightEditor->ReDraw();

	wxClientDC dc(self);
	self->DrawLayout(dc);
}

void DiffBar::OnRightEditorRedrawn(void* data) { // static
	DiffBar* self = (DiffBar*)data;
	if (!self->m_needRedraw) return;
	self->m_needRedraw = false;

	// Mod will also affect highlighting in opposite editor
	self->m_leftEditor->ReDraw();

	wxClientDC dc(self);
	self->DrawLayout(dc);
}

void DiffBar::OnLeftEditorScroll(void* data) { // static
	DiffBar* self = (DiffBar*)data;
	
	if (!self->m_matchlist.empty() && !wxGetKeyState(WXK_SHIFT)) {
		Lines& leftLines = self->m_leftEditor->GetLines();
		Lines& rightLines = self->m_rightEditor->GetLines();

		// Get middle line
		const wxSize size = self->GetClientSize();
		const unsigned int scrollPos = self->m_leftEditor->GetScrollPos();
		unsigned int middlePos = scrollPos + (size.y / 2);
		if ((int)middlePos >= leftLines.GetHeight()) middlePos = wxMax(leftLines.GetHeight()-1,0);
		const unsigned int middleLine = leftLines.GetLineFromYPos(middlePos);
		const unsigned int linePos = leftLines.GetLineStartpos(middleLine);
		const unsigned int lineYpos = leftLines.GetYPosFromLine(middleLine);

		// Find matching pos
		unsigned int pos = 0;
		for (list<cxMatch>::const_iterator p = self->m_matchlist.begin(); p != self->m_matchlist.end(); ++p) {
			if (linePos >= p->start1() && linePos < p->end1()) {
				pos = p->start2() + (linePos - p->start1());
				break;
			}
			else if (linePos < p->start1()) {
				pos = p->start2();
				break;
			}
		}

		// Match up middle
		const unsigned int line = rightLines.GetLineFromCharPos(pos);
		const unsigned int ypos = rightLines.GetYPosFromLine(line);
		const unsigned int relPos = lineYpos - scrollPos;
		const unsigned int newYpos = (relPos < ypos) ? ypos - relPos : 0;
		self->m_rightEditor->SetScrollCallback(NULL, NULL); // avoid loop
		self->m_rightEditor->SetScroll(newYpos);
		self->m_rightEditor->SetScrollCallback((void(*)(void*))OnRightEditorScroll, self);
	}

	wxClientDC dc(self);
	self->DrawLayout(dc);
}

void DiffBar::OnRightEditorScroll(void* data) { // static
	DiffBar* self = (DiffBar*)data;

	if (!self->m_matchlist.empty() && !wxGetKeyState(WXK_SHIFT)) {
		Lines& leftLines = self->m_leftEditor->GetLines();
		Lines& rightLines = self->m_rightEditor->GetLines();

		// Get middle line
		const wxSize size = self->GetClientSize();
		const unsigned int scrollPos = self->m_rightEditor->GetScrollPos();
		unsigned int middlePos = scrollPos + (size.y / 2);
		if ((int)middlePos >= rightLines.GetHeight()) middlePos = wxMax(rightLines.GetHeight()-1,0);
		const unsigned int middleLine = rightLines.GetLineFromYPos(middlePos);
		const unsigned int linePos = rightLines.GetLineStartpos(middleLine);
		const unsigned int lineYpos = rightLines.GetYPosFromLine(middleLine);

		// Find matching pos
		unsigned int pos = 0;
		for (list<cxMatch>::const_iterator p = self->m_matchlist.begin(); p != self->m_matchlist.end(); ++p) {
			if (linePos >= p->start2() && linePos < p->end2()) {
				pos = p->start1() + (linePos - p->start2());
				break;
			}
			else if (linePos < p->start2()) {
				pos = p->start1();
				break;
			}
		}

		// Match up middle
		const unsigned int line = leftLines.GetLineFromCharPos(pos);
		const unsigned int ypos = leftLines.GetYPosFromLine(line);
		const unsigned int relPos = lineYpos - scrollPos;
		const unsigned int newYpos = (relPos < ypos) ? ypos - relPos : 0;
		self->m_leftEditor->SetScrollCallback(NULL, NULL); // avoid loop
		self->m_leftEditor->SetScroll(newYpos);
		self->m_leftEditor->SetScrollCallback((void(*)(void*))OnLeftEditorScroll, self);
	}

	wxClientDC dc(self);
	self->DrawLayout(dc);
}

void DiffBar::OnLeftDocumentChanged(cxChangeType type, unsigned int pos, unsigned int len, void* data) { // static
	DiffBar* self = (DiffBar*)data;

	if (type == cxVERSIONCHANGE) {
		// Re-calc diff
		self->SetDiff();
		self->m_needRedraw = true;
		return;
	}

	if (self->m_matchlist.empty()) return;
	if (pos >= self->m_matchlist.back().end1()) return;

	list<cxMatch>::iterator p = self->m_matchlist.begin();

	if (type == cxINSERTION) {
		// Find position
		while (p != self->m_matchlist.end()) {
			if (pos <= p->end1()) break;
			++p;
		}

		// Check if we have to split the match
		if (pos > p->start1() && pos < p->end1()) {
			cxMatch& m = *(p++);
			const size_t newlen = pos - m.start1();
			const size_t rest = m.length() - newlen;
			self->m_matchlist.insert(p, cxMatch(pos + len, m.start2()+newlen, rest));
			m.len = newlen;
		}
		else if (pos == p->end2()) ++p;

		// Adjust following matches
		while (p != self->m_matchlist.end()) {
			p->offset1 += len;
			++p;
		}
	}
	else {
		const size_t del_end = pos + len;

		// Find position
		while (p != self->m_matchlist.end()) {
			if (pos <= p->start1() && del_end >= p->end1()) { // fully enclosed
				p = self->m_matchlist.erase(p);
				continue;
			}
			if (del_end > p->start1() && pos < p->end1()) { // overlap
				if (pos <= p->start1()) { // first part deleted
					const size_t del_len = del_end - p->start1();
					p->offset1 = pos;
					p->offset2 += del_len;
					p->len -= del_len;
				}
				else if (del_end >= p->end1()) {  // ending part deleted
					const size_t del_len = p->end1() - pos;
					p->len -= del_len;
				}
				else { // deletion in middle (split)
					cxMatch& m = *(p++);
					const size_t newlen = pos - m.start1();
					const size_t rest = (m.length() - newlen) - len;
					self->m_matchlist.insert(p, cxMatch(pos, m.start2()+newlen+len, rest));
					m.len = newlen;
					break;
				}
			}
			else if (del_end <= p->start1()) break;
			++p;
		}

		// Adjust following matches
		while (p != self->m_matchlist.end()) {
			p->offset1 -= len;
			++p;
		}
	}

	self->m_needTransform = true;
	self->m_needRedraw = true;
}

void DiffBar::OnRightDocumentChanged(cxChangeType type, unsigned int pos, unsigned int len, void* data) { // static
	DiffBar* self = (DiffBar*)data;

	if (type == cxVERSIONCHANGE) {
		// Re-calc diff
		self->SetDiff();
		self->m_needRedraw = true;
		return;
	}

	if (self->m_matchlist.empty()) return;
	if (pos >= self->m_matchlist.back().end2()) return;

	list<cxMatch>::iterator p = self->m_matchlist.begin();

	if (type == cxINSERTION) {
		// Find position
		while (p != self->m_matchlist.end()) {
			if (pos <= p->end2()) break;
			++p;
		}

		// Check if we have to split the match
		if (pos > p->start2() && pos < p->end2()) {
			cxMatch& m = *(p++);
			const size_t newlen = pos - m.start2();
			const size_t rest = m.length() - newlen;
			self->m_matchlist.insert(p, cxMatch(m.start1()+newlen, pos + len, rest));
			m.len = newlen;
		}
		else if (pos == p->end2()) ++p;

		// Adjust following matches
		while (p != self->m_matchlist.end()) {
			p->offset2 += len;
			++p;
		}
	}
	else {
		const size_t del_end = pos + len;

		// Find position
		while (p != self->m_matchlist.end()) {
			if (pos <= p->start2() && del_end >= p->end2()) { // fully enclosed
				p = self->m_matchlist.erase(p);
				continue;
			}
			if (del_end > p->start2() && pos < p->end2()) { // overlap
				if (pos <= p->start2()) { // first part deleted
					const size_t del_len = del_end - p->start2();
					p->offset2 = pos;
					p->offset1 += del_len;
					p->len -= del_len;
				}
				else if (del_end >= p->end2()) {  // ending part deleted
					const size_t del_len = p->end2() - pos;
					p->len -= del_len;
				}
				else { // deletion in middle (split)
					cxMatch& m = *(p++);
					const size_t newlen = pos - m.start2();
					const size_t rest = (m.length() - newlen) - len;
					self->m_matchlist.insert(p, cxMatch(m.start1()+newlen+len, pos, rest));
					m.len = newlen;
					break;
				}
			}
			else if (del_end <= p->start2()) break;
			++p;
		}

		// Adjust following matches
		while (p != self->m_matchlist.end()) {
			p->offset2 -= len;
			++p;
		}
	}

	self->m_needTransform = true;
	self->m_needRedraw = true;
}

// ---- DiffStyler ------------------------------

DiffBar::DiffStyler::DiffStyler(const std::vector<Change>& diffs, bool isLeft):
	m_isLeft(isLeft), m_diffs(diffs)
{
	m_insColor.Set(192, 255, 192); // PASTEL GREEN
	m_delColor.Set(255, 192, 192); // PASTEL RED
}

void DiffBar::DiffStyler::Style(StyleRun& sr) {
	if (m_diffs.empty()) return;

	const unsigned int rstart =  sr.GetRunStart();
	const unsigned int rend = sr.GetRunEnd();
	const cxChangeType primaryType = m_isLeft ? cxDELETION : cxINSERTION;
	const wxColor& primaryColor = m_isLeft ? m_delColor : m_insColor;
	const wxColor& secondaryColor = m_isLeft ? m_insColor : m_delColor;

	for (std::vector<Change>::const_iterator c = m_diffs.begin(); c != m_diffs.end(); ++c) {
		if (c->type == primaryType) {
			if (c->end_pos > rstart && c->start_pos < rend) {
				const unsigned int start = wxMax(rstart, c->start_pos);
				const unsigned int end   = wxMin(rend, c->end_pos);

				sr.SetBackgroundColor(start, end, primaryColor);
				sr.SetShowHidden(start, end, true);
			}
			if (c->end_pos > rend) break;
		}
		else {
			if (c->rev_pos > rend) break;
			else if (c->rev_pos > rstart) {
				sr.SetBackgroundColor(c->rev_pos, c->rev_pos, secondaryColor); // zero-len
			}
		}
	}
}
