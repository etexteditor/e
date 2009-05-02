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

#include "Lines.h"
#include "doc_byte_iter.h"
#include "Document.h"
#include "IFoldingEditor.h"
#include "Fold.h"

Lines::Lines(wxDC& dc, DocumentWrapper& dw, IFoldingEditor& editorCtrl, const tmTheme& theme)
: dc(dc), m_doc(dw), m_editorCtrl(editorCtrl), NewlineTerminated(false), pos(0), lastpos(0),
  line(dc, dw, selections, editorCtrl.GetHlBracket(), lastpos, m_isSelShadow, theme),
  m_theme(theme), m_lastSel(-1), m_marginChars(0), m_marginPos(0),
  selections(), m_isSelShadow(false),
  m_wrapMode(cxWRAP_NONE), ll(NULL), llWrap(line, dw), llNoWrap(line, dw)
{
	// WARNING: Do not touch the document here
	// it is not locked during construction
	caretpos.x = 0;
	caretpos.y = 0;

	if (m_wrapMode != cxWRAP_NONE) ll = &llWrap;
	else ll = &llNoWrap;
	line.SetWordWrap(m_wrapMode);

#ifdef __WXDEBUG__
	m_verifyEnabled = true;
#endif
}

void Lines::Init() {
	// We can not count on the dc being valid in the constructor
	// so we have to do initialization using it here

	line.Init();
}

void Lines::SetLine(unsigned int lineId) {
	wxASSERT(lineId < ll->size());

	const unsigned int linesize = line.SetLine(ll->offset(lineId), ll->end(lineId));

	// The size we have for this line may have been an approximation
	// so we update to the real size here. Calculating the extent of
	// a line can be en expensive operation. so generally we use
	// quick approximations, and then correct them when we need to
	// parse the line anyways (like drawing it og getting caret pos).
	ll->update_line_extent(lineId, linesize);
}

void Lines::SetWordWrap(cxWrapMode wrapMode) {
	if (wrapMode == m_wrapMode) return;
	m_wrapMode = wrapMode;

	// Cache the offsets
	vector<unsigned int> offsets;
	ll->GetOffsets().swap(offsets);

	llWrap.clear();
	llNoWrap.clear();

	line.SetWordWrap(wrapMode);
	if (wrapMode != cxWRAP_NONE) {
		ll = &llWrap;
		llWrap.GetOffsets().swap(offsets);
		llWrap.NewOffsets();
		ll->invalidate();
	}
	else {
		ll = &llNoWrap;
		llNoWrap.GetOffsets().swap(offsets);
		llNoWrap.NewOffsets();
	}
}

void Lines::ShowIndent(bool showIndent) {
	line.ShowIndentGuides(showIndent);
}

bool Lines::ShowMargin(unsigned int marginChars) {
	if (marginChars == m_marginChars) return false; // no update needed

	m_marginChars = marginChars;
	m_marginPos = marginChars * line.GetCharWidth();
	return true;
}

int Lines::GetHeight() const {
	if (NewlineTerminated) return FoldedYPos(ll->height() + line.GetCharHeight());
	else return FoldedYPos(ll->height());
}

int Lines::GetLineHeight() const {
	return line.GetCharHeight();
}

unsigned int Lines::GetLength() const {
	return ll->length();
}

unsigned int Lines::GetLineCount(bool includeVirtual) const {
	if (includeVirtual && NewlineTerminated) return ll->size()+1;
	else return ll->size();
}

unsigned int Lines::GetLastLine() const {
	if (ll->size() == 0) return 0;
	else if (NewlineTerminated) return ll->size();
	else return ll->size()-1;
}

void Lines::SetWidth(unsigned int newwidth, unsigned int index) {
	if (newwidth == line.GetDisplayWidth()) return;

	line.SetWidth(newwidth);

	ll->widthchanged(index);
	ll->verify();
}

void Lines::UpdateParsedLine(unsigned int line_id) {
	ll->update_parsed_line(line_id);
}

int Lines::GetWidth() {
	return m_wrapMode == cxWRAP_NONE ? ll->width() : line.GetDisplayWidth();
}

void Lines::UpdateFont() {
	line.UpdateFont();
	m_marginPos = m_marginChars * line.GetCharWidth();

	ll->invalidate();
}

void Lines::SetTabWidth(unsigned int width) {
	line.SetTabWidth(width);
	ll->invalidate();
}

unsigned int Lines::GetPos() const {
	wxASSERT(pos >= 0 && pos <= ll->length());
	return pos;
}

unsigned int Lines::GetLastpos() {
	wxASSERT(lastpos >= 0 && lastpos <= ll->length());
	return lastpos;
}

void Lines::SetPos(unsigned int newpos, bool update_lastpos) {
	wxASSERT(newpos >= 0 && newpos <= GetLength());
	if (newpos > GetLength()) newpos = GetLength(); // DEFENSIVE: there has been some bug reports with invalid pos

	if (update_lastpos) lastpos = pos;
	pos = newpos;
	
	// Only set caret if line is valid
	if (line.GetDisplayWidth()) SetCaretPos();

	if (m_editorCtrl.IsPosInFold(pos)) {
		const unsigned int line_id = GetLineFromCharPos(pos);
		m_editorCtrl.UnFoldParents(line_id);
	}
}

void Lines::SetLastpos(unsigned int newpos) {
	wxASSERT(newpos >= 0 && newpos <= ll->length());
	lastpos = newpos;
}

/*bool Lines::CaretVisible() {
	// Checks if caret is in linelist window
	// might not be visible in current display
	if (ll->in_window(pos)) {
		//SetCaretPos();
		return true;
	}
	else return false;
}*/

wxPoint Lines::GetCaretPos() const {
	return wxPoint(caretpos.x, FoldedYPos(caretpos.y));
}

wxPoint Lines::GetCharPos(unsigned int char_pos) {
	wxASSERT(char_pos >= 0 && char_pos <= GetLength());

	wxPoint cpos(0, 0);

	// Find the line with the position in it
	unsigned int posline = ll->find_offset(char_pos);
	if (posline == ll->size()) {
		return cpos; // empty text
	}

	if (char_pos != ll->end(posline) || (char_pos == ll->length() && !NewlineTerminated)) {
		// Get position in line
		SetLine(posline);
		cpos = line.GetCaretPos(char_pos-ll->offset(posline));

		// Move it to correct ypos
		cpos.y += ll->top(posline);
	}
	else {
		// After newline, go to next line
		cpos.y = ll->bottom(posline);
	}

	cpos.y = FoldedYPos(cpos.y);
	return cpos;
}

void Lines::UpdateCaretPos() {
	SetCaretPos(true);
}

void Lines::SetCaretPos(bool update) {
	ll->verify();
	wxPoint cpos(0, 0);

	// Find the line with the position in it
	unsigned int posline = ll->find_offset(pos);
	if (posline == ll->size()) {
		caretpos = cpos;
		return;
	}

	if (pos != ll->end(posline) || (pos == ll->length() && !NewlineTerminated)) {
		// Get position in line
		SetLine(posline);

		// if we are just updating, make sure position is as close to previous as possible
		if (update && caretpos.x == 0) cpos = line.GetCaretPos(pos-ll->offset(posline), true);
		else cpos = line.GetCaretPos(pos-ll->offset(posline));

		// Move it to correct ypos
		cpos.y += ll->top(posline);

		wxASSERT(cpos.y < (int)GetUnFoldedHeight());
	}
	else {
		// After newline, go to next line
		cpos.y = ll->bottom(posline);

		wxASSERT(cpos.y < (int)GetUnFoldedHeight());
	}

	caretpos = cpos;
}

wxRect Lines::GetFoldIndicatorRect(unsigned int line_id) {
	wxASSERT(line_id < ll->size()); // virtual line cannot be folded

	SetLine(line_id);

	const unsigned int top_ypos = GetYPosFromLine(line_id);
	wxRect rect = line.GetFoldIndicatorRect();
	rect.y += top_ypos;

	return rect;
}

bool Lines::IsOverFoldIndicator(const wxPoint& point) {
	if (point.y < GetHeight()) {
		// Get the line
		const unsigned int line_id = GetLineFromYPos(point.y);
		if (line_id < ll->size()) {
			const unsigned int top_ypos = GetYPosFromLine(line_id);
			SetLine(line_id);

			return line.IsOverFoldIndicator(wxPoint(point.x, point.y - top_ypos));
		}
	}
	return false;
}

bool Lines::IsAtTabPoint(){
	unsigned int posline = ll->find_offset(pos);
	if (posline == ll->size()) return true;

	if (pos != ll->end(posline) || (pos == ll->length() && !NewlineTerminated)) {
		// Get position in line
		SetLine(posline);

		return line.IsTabPos(pos - ll->offset(posline));
	}

	return true; // start of next line
}

int Lines::GetCurrentLine() {
	if (ll->size() == 0) return 0; // WARNING: This line do not exist yet

	unsigned int lineid = GetLineFromCharPos(pos);

	// Notice that this can return the last (virtual) line
	return lineid;
}

unsigned int Lines::GetLineStartpos(unsigned int lineid) {
	wxASSERT(lineid >= 0 && lineid <= ll->size());

	if (ll->size() == 0) return 0;

	if (lineid > ll->last()) {
		// last (virtual) line
		wxASSERT(lineid == ll->size() && NewlineTerminated);
		return ll->length();
	}
	
	return ll->offset(lineid);
}

bool Lines::isLineVirtual(unsigned int lineid) {
	wxASSERT(lineid >= 0 && lineid <= ll->size());
	wxASSERT(NewlineTerminated || lineid != ll->size());

	return lineid == ll->size() && NewlineTerminated;
}

unsigned int Lines::GetLineEndpos(unsigned int lineid, bool stripnewline) {
	wxASSERT(lineid >= 0 && lineid <= ll->size());

	if (ll->size() == 0) return 0;

	if (stripnewline) {
		// We want the pos right before the newline
		if (lineid == ll->last() && !NewlineTerminated) return ll->length();
		if (lineid > ll->last()) return ll->length();
		return ll->end(lineid)-1;
	}

	if (lineid > ll->last()) {
		wxASSERT(lineid == ll->size() && NewlineTerminated);
		return ll->length();
	}

	return ll->end(lineid);
}

bool Lines::IsLineEmpty(unsigned int lineid) {
	wxASSERT(lineid <= ll->size());

	if (ll->size() == 0) return true;
	if (lineid > ll->last()) return true; // last virtual line
	return (ll->offset(lineid)+1 >= ll->end(lineid));
}

bool Lines::IsLineEnd(unsigned int pos) {
	wxASSERT(pos <= GetLength());
	return ll->IsLineEnd(pos);
}

bool Lines::IsLineEnd(unsigned int line_id, unsigned int pos) {
	wxASSERT(line_id <= ll->size());

	if (line_id == ll->size()) return (pos == ll->length());
	else return (pos == ll->end(line_id));
}

bool Lines::IsLineStart(unsigned int line_id, unsigned int pos) {
	wxASSERT(line_id <= ll->size());

	if (line_id == ll->size()) return (pos == ll->length());
	else return (pos == ll->offset(line_id));
}

bool Lines::IsBeforeNewline(unsigned int pos) {
	wxASSERT(pos <= GetLength());

	const unsigned int lineid = GetLineFromCharPos(pos);
	const unsigned int beforeNewline = GetLineEndpos(lineid, true);

	return (pos == beforeNewline);
}

unsigned int Lines::GetLineEndFromPos(unsigned int pos) {
	wxASSERT(pos <= GetLength());
	
	const unsigned int endPos = ll->EndFromPos(pos);
	return endPos;
}

unsigned int Lines::GetLineStartFromPos(unsigned int pos) {
	wxASSERT(pos <= GetLength());

	return ll->StartFromPos(pos);
}

int Lines::GetLineFromCharPos(unsigned int char_pos) {
	wxASSERT(char_pos >= 0 && char_pos <= GetLength());

	if (char_pos == 0 && GetLength() == 0) return 0; // empty doc

	// Find the line with the position in it
	int lineid = ll->find_offset(char_pos);
	if (char_pos == ll->end(lineid) && (char_pos != GetLength() || NewlineTerminated)) {
		// last pos in the line is same as first in next line
		return lineid + 1;
	}
	return lineid;
}

unsigned int Lines::GetLineFromStartPos(unsigned int char_pos) {
	wxASSERT(char_pos <= GetLength());

	if (char_pos == 0) return 0;

	const unsigned int line_id = ll->find_offset(char_pos) + 1;
	wxASSERT(ll->offset(line_id) == char_pos);

	return line_id;
}

int Lines::GetLineFromYPos(int folded_ypos) {
	wxASSERT(folded_ypos >= 0);
	const unsigned int ypos = UnFoldedYPos(folded_ypos+1); // + one pixel to make sure we enter line

	if (folded_ypos == 0) return 0;
	if (ypos > ll->height()) {
		wxASSERT(NewlineTerminated);
		return ll->size();
	}

	return ll->find_ypos(ypos);
}

int Lines::GetYPosFromLine(unsigned int lineid) {
	if (lineid == 0) return 0;
	if (lineid < ll->size()) return FoldedYPos(ll->top(lineid));

	if (!(NewlineTerminated && lineid == ll->size())) {
		const doc_id di = m_doc.GetDoc().GetDocument();
		wxString msg = wxString::Format(wxT("Out-of-bounds %d, %d, %d (%d,%d,%d)"), NewlineTerminated, lineid, ll->size(), di.type, di.document_id, di.version_id);
		wxFAIL_MSG(msg);
	}
	//wxASSERT(NewlineTerminated && lineid == ll->size());
	return FoldedYPos(ll->bottom(lineid-1));
}

int Lines::GetBottomYPosFromLine(unsigned int lineid) {
	wxASSERT(lineid > 0 || ll->size() > 0); // empty line has no bottom

	if (lineid < ll->size()) return FoldedYPos(ll->bottom(lineid));

	wxASSERT(NewlineTerminated && lineid == ll->size());
	return FoldedYPos(ll->height() + line.GetCharHeight());
}

int Lines::PrepareYPos(int folded_ypos) {
	// Check if ypos has moved during postion validation
	const unsigned int ypos = UnFoldedYPos(folded_ypos);
	const int approxDiff = ll->prepare(ypos);

	if (approxDiff) {
		const unsigned int new_ypos = ypos + approxDiff;
		const unsigned int adj_ypos = FoldedYPos(new_ypos); 
		return adj_ypos - folded_ypos; // diff adjusted for folding
	}

	return 0;
}

void Lines::AddStyler(Styler& styler) {
	line.AddStyler(styler);
}

bool Lines::IsSelected() const {
	return !selections.empty();
}

bool Lines::IsMultiSelected() const {
	return selections.size() > 1;
}

bool Lines::IsSelectionMultiline() {
	if (selections.empty()) return false;

	for (vector<interval>::const_iterator i = selections.begin(); i != selections.end(); ++i) {
		unsigned int startline = GetLineFromCharPos(i->start);
		unsigned int endline = GetLineFromCharPos(i->end);

		if (startline != endline) return true;
	}

	return false; // No multiline selections found
}

int Lines::AddSelection(unsigned int start, unsigned int end) {
	wxASSERT(start >= 0 && start <= GetLength());
	wxASSERT(end >= 0 && end <= GetLength());
	//wxASSERT(start != end || m_isSelShadow);

	interval iv(start, end);
	if (start > end) {
		// Backward selection, reverse..
		iv.start = end;
		iv.end = start;
	}
	vector<interval>::iterator i;
	//wxLogDebug(wxT("sel %d,%d"), iv.start, iv.end);

	if (selections.empty())	i = selections.insert(selections.end(), iv);
	else {
		i = selections.begin();
		while (i != selections.end()) {
			if (iv.end < (*i).start) break;
			else if (iv.start >= (*i).start && iv.end <= (*i).end) {
				//wxLogDebug("  new full %d,%d", (*i).start, (*i).end);
				i = selections.erase(i); // new selection fully contained
				break;
			}
			else if ((*i).start >= iv.start && (*i).end <= iv.end) {
				//wxLogDebug("  old full");
				i = selections.erase(i); // old selection fully contained
				if (i == selections.end()) break;
				else continue; // avoid incrementing iter
			}
			else if ((*i).end > iv.start && (*i).end < iv.end) {
				//wxLogDebug("  overlap1");
				(*i).end = iv.start; // remove overlap
			}
			else if ((*i).start < iv.end && (*i).start > iv.start) {
				//wxLogDebug("  overlap2");
				(*i).start = iv.end; // remove overlap
				break;
			}

			++i;
		}
		wxASSERT(i >= selections.begin());
		wxASSERT(i <= selections.end());
		i = selections.insert(i, iv);
	}

	// Return an index to the new selection
	m_lastSel = distance(selections.begin(), i);
	return m_lastSel;
}

int Lines::UpdateSelection(unsigned int sel_id, unsigned int start, unsigned int end) {
	//wxLogDebug("UpdateSelection %d %d %d", sel_id, start, end);
	wxASSERT(!selections.empty() && sel_id < selections.size());
	wxASSERT(start >= 0 && start <= GetLength());
	wxASSERT(end >= 0 && end <= GetLength());

	if (start == end) {
		selections.erase(selections.begin()+sel_id);
		m_lastSel = -1;
	}
	else if (selections.size() == 1) {
		interval iv(start, end);
		if (start > end) {
			// Backward selection, reverse..
			iv.start = end;
			iv.end = start;
		}

		selections[0] = iv;
		m_lastSel = 0;
	}
	else {
		selections.erase(selections.begin()+sel_id);
		m_lastSel = AddSelection(start, end);
	}

	return m_lastSel;
}

void Lines::RemoveSelection(unsigned int sel_id) {
	wxASSERT(!selections.empty() && sel_id < selections.size());
	selections.erase(selections.begin()+sel_id);

	if (m_lastSel == (int)sel_id) m_lastSel = -1;
}

void Lines::RemoveAllSelections(bool checkShadow, unsigned int pos) {
	wxASSERT(pos <= GetLength());

	bool doClean = true;

	// In shadow mode we only remove selections if pos is outside current selections
	if (checkShadow && m_isSelShadow) {
		for (vector<interval>::const_iterator iv = selections.begin(); iv != selections.end(); ++iv) {
			if (pos >= iv->start && pos <= iv->end) {
				doClean = false;
				break;
			}
		}
	}

	if (doClean) {
		selections.clear();
		m_isSelShadow = false;
	}

	m_lastSel = -1;
}

const vector<interval>& Lines::GetSelections() const {
	return selections;
}

void Lines::Clear() {
	ll->clear();
	line.FlushCache();
	//if (m_wrapMode != cxWRAP_NONE) line.SetWidth(0);
	pos = 0;
	lastpos = 0;
	selections.clear();
	caretpos.x = 0;
	caretpos.y = 0;
	m_isSelShadow = false;
}

cxFileResult Lines::LoadText(const wxFileName& path, wxFontEncoding enc, const wxString& mirror) {
	wxASSERT(path.IsOk());

	// Load the text
	vector<unsigned int> textOffsets;
	cxFileResult result;
	cxLOCKDOC_WRITE(m_doc)
		result = doc.LoadText(path, textOffsets, enc, mirror);
	cxENDLOCK
	if (result != cxFILE_OK) return result;

	Clear(); // Clean up first
	line.Invalidate(); // avoid styling lines prematurely
	ll->SetOffsets(textOffsets);

	// Check if we end with a newline
	if (m_doc.GetLength()) {
		cxLOCKDOC_READ(m_doc)
			const wxChar lastChar = doc.GetChar(doc.GetValidCharPos(doc.GetLength()-1));
			NewlineTerminated = (lastChar == '\n');
		cxENDLOCK
	}
	else NewlineTerminated = false;

	ll->invalidate();
	return result;
}

void Lines::ReLoadText() {
	// Clean up first
	Clear();
	line.Invalidate(); // avoid styling lines prematurely

	// Get the new text offsets
	cxLOCKDOC_READ(m_doc)
		doc.GetLines(ll->GetOffsets());
	cxENDLOCK
	ll->NewOffsets();

	// Check if we end with a newline
	if (m_doc.GetLength()) {
		cxLOCKDOC_READ(m_doc)
			const wxChar lastChar = doc.GetChar(doc.GetValidCharPos(doc.GetLength()-1));
			NewlineTerminated = (lastChar == '\n');
		cxENDLOCK
	}
	else NewlineTerminated = false;
}

void Lines::InsertChar(unsigned int pos, const wxChar& newtext, unsigned int byte_len) {
	wxASSERT(pos >= 0 && pos <= GetLength());

	line.FlushCache(pos);

	if (ll->size() == 0) {
		// No text yet, create a new line
		ll->insert(0, byte_len);
		NewlineTerminated = (newtext == wxT('\n'));
		ll->verify(true);
		return;
	}

	if (pos == ll->length()) {
		// Extending end. Check if we need a new line
		if (NewlineTerminated) ll->insert(ll->size(), ll->length()+byte_len);
		else ll->update(ll->size()-1, ll->length()+byte_len);
		NewlineTerminated = (newtext == '\n');
		ll->verify(true);
		return;
	}

	// Find the line with insertion in it
	unsigned int changedline = ll->find_offset(pos);

	// If inserting after newline, move to next line first
	if (pos == ll->end(changedline)) ++changedline;
	wxASSERT(changedline < ll->size());

	// How much line left after insertion?
	int linerest = ll->end(changedline) - pos;
	wxASSERT(linerest >= 0);

	// Insert the char
	if (newtext == '\n') {
		ll->update(changedline, pos+byte_len);
		if (linerest > 0) {
			ll->insert(changedline+1, ll->end(changedline) + linerest);
		}
	}
	else {
		ll->update(changedline, ll->end(changedline)+byte_len);
	}

	ll->verify(true);
}


void Lines::Insert(unsigned int pos, unsigned int byte_len) {
	wxASSERT(pos >= 0 && pos <= GetLength());
	wxASSERT(byte_len > 0);

	line.FlushCache(pos);

	int changedline = -1;
	int linerest = 0;
	bool ins_after_newline = false;
	if (ll->size()) {
		// Find the line with insertion in it
		changedline = ll->find_offset(pos);

		// If inserting after newline, move to next line first
		if (pos == ll->end(changedline) && ((unsigned int)changedline != ll->last() || NewlineTerminated)) {
			ins_after_newline = true;
			++changedline;
		}

		// How much line left after insertion?
		if ((unsigned int)changedline <= ll->last()) linerest = ll->end(changedline) - pos;
	}

	// Extending end. Check if we end with a newline
	int inspos = changedline;
	const unsigned int end_pos = pos + byte_len;
	vector<unsigned int> newlines; newlines.reserve(byte_len/35);
	cxLOCKDOC_READ(m_doc)
		doc_byte_iter dbi(doc, end_pos-1);
		const bool ends_with_newline = (*dbi == '\n');
		if (pos == ll->length()) NewlineTerminated = ends_with_newline;

		// Check if insertion was between lines (no existing lines changed)
		if (ins_after_newline && ends_with_newline) {
			changedline = -1;
			linerest = 0;
		}

		// Create new lines
		bool do_update = true;
		for (dbi.SetIndex(pos); dbi.GetIndex() < (int)end_pos; ++dbi) {
			if (*dbi == '\n') {
				const unsigned int line_end = dbi.GetIndex()+1;
				if (do_update) {
					if (changedline != -1) {
						if ((unsigned int)changedline <= ll->last()) { // don't update on virtual line
							ll->update(changedline, line_end);
							++inspos; do_update = false;
							continue;
						}
					}
					do_update = false;
				}
				newlines.push_back(line_end);
			}
		}
	cxENDLOCK

	// Insert the new lines
	if (inspos == -1) inspos = 0;
	int lastline = inspos;
	if(!newlines.empty()) {
		ll->insertlines(inspos, newlines);

		lastline += (int)newlines.size(); // set to line after last changed line
	}

	// Insert the rest of the text
	if (ll->size() == 0) ll->insert(0, byte_len); // insert in empty text
	else {
		wxChar lastchar;
		cxLOCKDOC_READ(m_doc)
			lastchar = doc.GetChar(doc.GetPrevCharPos(end_pos));
		cxENDLOCK
		if (lastchar != wxT('\n')) {
			// insert text from after last newline
			if (changedline == lastline && changedline != -1 && (unsigned int)changedline <= ll->last()) {
				ll->update(changedline, pos + byte_len + linerest); // no newlines found, just update
			}
			else ll->insert(lastline, pos + byte_len + linerest); // insert after
		}
		else if (linerest) ll->insert(lastline, pos + byte_len + linerest); // linerest pushed to new line
	}

	Verify(true);
}

void Lines::Delete(unsigned int startpos, unsigned int endpos) {
	wxASSERT(startpos >= 0 && startpos < GetLength());

	if (startpos == endpos) return;
	wxASSERT(endpos > startpos && endpos <= GetLength());

	// Make sure lastpos stays valid
	if (lastpos > startpos) {
		const int adjpos = lastpos - (endpos-startpos);
		lastpos = wxMax((int)startpos, adjpos);
	}

	line.FlushCache(startpos);

	// Find the line with startpos in it
	unsigned int firstline = ll->find_offset(startpos);
	if (startpos == ll->end(firstline)) ++firstline;

	if (endpos == GetLength()) {
		if (startpos == 0) {
			// Delete entire text
			ll->remove(0, ll->size());
			NewlineTerminated = false;
			Verify(true);
			return;
		}
		else {
			// Check if the new text will be newline terminated
			NewlineTerminated = (startpos == ll->offset(firstline));
		}
	}

	// Quick update if contained in one line
	if (endpos <= ll->end(firstline)) {
		if (startpos == ll->offset(firstline) && endpos == ll->end(firstline)) {
			// Delete entire line
			ll->remove(firstline, firstline+1);
		}
		else {
			if (endpos == ll->end(firstline) && firstline < ll->size()-1) {
				// newline deleted, we have to merge with next line
				int newend = ll->end(firstline+1) - (endpos - startpos);
				ll->remove(firstline, firstline+1);
				ll->update(firstline, newend);
			}
			else ll->update(firstline,  ll->end(firstline) - (endpos - startpos));
		}
		Verify(true);
		return;
	}

	// Find the line with endpos in it
	const unsigned int lastline = ll->find_offset(endpos);
	wxASSERT(firstline != lastline);

	if (startpos == ll->offset(firstline) && endpos == ll->end(lastline)) {
		// Fully contained, delete all lines
		ll->remove(firstline, lastline+1);
	}
	else {
		const unsigned int lastlineend = ll->end(lastline);

		// Keep first line (add eventual rest from lastline)
		if (endpos == lastlineend && lastline < ll->size()-1) {
			// newline deleted, we also have to merge with next line
			int newend = ll->end(lastline+1) - (endpos - startpos);
			ll->remove(firstline, lastline+1);
			ll->update(firstline, newend);
		}
		else if (lastlineend >= endpos) {
			const unsigned int rest = lastlineend - endpos;
			ll->remove(firstline, lastline);
			ll->update(firstline, startpos+rest);
		}
		else {
			wxASSERT(false);
		}
	}

	Verify(true);
}

void Lines::ApplyDiff(vector<cxChange>& changes) {
#ifdef __WXDEBUG__
	// Linelist is not valid during application of diff
	m_verifyEnabled = false;
#endif

	int offset = 0;
	for (vector<cxChange>::iterator p = changes.begin(); p != changes.end(); ++p) {
		const unsigned int len = p->end - p->start;

		if (p->type == cxINSERTION) {
			Insert(p->start, len);
			offset += len;

			// Cache number of inserted lines
			p->lines = GetLineFromCharPos(p->end) - GetLineFromCharPos(p->start);
		}
		else { // if (p->type == cxDELETION)
			const unsigned int start = p->start + offset;
			const unsigned int end = p->end + offset;

			// Cache number of deleted lines
			p->lines = GetLineFromCharPos(end) - GetLineFromCharPos(start);

			Delete(start, end);
			offset -= len;
		}
	}

#ifdef __WXDEBUG__
	m_verifyEnabled = true;
	ll->verify(true);
#endif
}

bool Lines::IsCaretInPreparedPos() {
	// Checks we can update caret without doing any validations that
	// could move positions.
	return ll->in_window(pos);
}

void Lines::Draw(int xoffset, int yoffset, wxRect& rect) {
	wxASSERT(rect.y >= 0);
	ll->verify();

	// Unfold rect
	const unsigned int top_ypos = UnFoldedYPos(rect.y);
	const unsigned int bottom_ypos = UnFoldedYPos(rect.GetBottom());

	const unsigned int currentLine = GetLineFromCharPos(pos);
	const unsigned int height = ll->height();

	if (currentLine == ll->size() && bottom_ypos > height) {
		// Highlight the background of last (virtual) line
		dc.SetBrush(wxBrush(m_theme.lineHighlightColor));
		dc.SetPen(wxPen(m_theme.lineHighlightColor));
		dc.DrawRectangle(0, FoldedYPos(ll->height()) + yoffset, rect.width, line.GetCharHeight());
	}

	if (top_ypos < height) {
		// Get the first visible line
		unsigned int firstline = ll->find_ypos(top_ypos);

		// Prepare for foldings
		const vector<cxFold>& folds = m_editorCtrl.GetFolds();
		const cxFold target(firstline);
		vector<cxFold>::const_iterator nextFold = lower_bound(folds.begin(), folds.end(), target);

		// Draw one line at a time
		const unsigned int linecount = ll->size();
		while (firstline < linecount) {
			const unsigned int top = FoldedYPos(ll->top(firstline));
			SetLine(firstline);

			// Check if we should highlight the line background
			if (firstline == currentLine) {
				dc.SetBrush(wxBrush(m_theme.lineHighlightColor));
				dc.SetPen(wxPen(m_theme.lineHighlightColor));

				// Highlight the background
				dc.DrawRectangle(0, top + yoffset, rect.width, line.GetHeight());
			}

			// Check if line is folded
			bool isFolded = false;
			if (nextFold != folds.end() && nextFold->line_id == firstline) {
				if (nextFold->type == cxFOLD_START_FOLDED) isFolded = true;
				else ++nextFold;
			}

			// Draw the line
			line.DrawLine(xoffset, top + yoffset, rect, isFolded);

#ifdef __WXDEBUG__
			int h = line.GetHeight();
			int t = ll->top(firstline);
			int b = ll->bottom(firstline);
			if (b- t != h) {
				wxLogDebug(wxT("%d: %d - %d != %d"), firstline, b, t, h);
				t = ll->top(firstline);
				Print();
			}
			wxASSERT(b - t == h);
#endif  //__WXDEBUG__

			// Check if this was last visible line
			if (ll->bottom(firstline) > bottom_ypos) break;

			// Advance to next visible line
			++firstline;

			// Move over folds
			if (isFolded) {
				// Advance to end of fold
				firstline += nextFold->count;
				while (nextFold != folds.end() && nextFold->line_id < firstline) {
					++nextFold;
				}
			}
		}
	}

	Verify();

	// Draw margen line
	if (m_marginPos) {
		dc.SetPen(wxPen(m_theme.invisiblesColor));
		const unsigned int xpos = xoffset + m_marginPos;
		const unsigned int top = 0; // lines can have drawn above rect
		const unsigned int ypos = yoffset + rect.y;
		dc.DrawLine(xpos, top, xpos, ypos + rect.height);
	}

	// Reset styles to avoid leaking font settings to others
	// who are using the same dc.
	line.ResetStyle();
}

void Lines::Print() {
	//for (int i = 0; i < ll->size(); ++i) {
	//	wxLogDebug("%d: %d  %d", i, ll->end(i), ll->bottom(i));
	//}
	ll->Print();
}

void Lines::Verify(bool deep) {
#ifdef __WXDEBUG__
	if (m_verifyEnabled) ll->verify(deep);
#endif
}

int Lines::GetPosFromXY(int xpos, int folded_ypos, bool allowOutbound) {
	if (ll->size() == 0) return 0;

	unsigned int ypos = UnFoldedYPos(folded_ypos);

	// Positions might move outside textarea
	if (xpos < 0) xpos = 0;
	else if (xpos > GetWidth()) xpos = GetWidth();

	if (ypos < 0) ypos = 0;
	else if ((unsigned int)ypos > ll->height()) {
		// Clicked below lines
		return allowOutbound ? ll->length() : -1;
	}

	// Find the line clicked on
	int l = ll->find_ypos(ypos);
	SetLine(l);

	wxASSERT(ll->top(l) <= ypos);
	full_pos fp = line.ClickOnLine(xpos, ypos - ll->top(l));

	if (!allowOutbound && fp.xy_outbound && !fp.on_newline) return -1;
	else return fp.pos + ll->offset(l);
}

full_pos Lines::ClickOnLine(int xpos, int folded_ypos, bool doSetPos) {
	lastpos = pos;

	full_pos fp = {0, 0, 0, false, false};
	if (ll->size() == 0) return fp;

	if (folded_ypos < 0) folded_ypos = 0;
	unsigned int ypos = UnFoldedYPos(folded_ypos);

	// Positions might move outside textarea
	if (xpos < 0) xpos = 0;
	else if (xpos > GetWidth()) xpos = GetWidth();
	if (ypos < 0) ypos = 0;
	else if ((unsigned int)ypos >= ll->height()) { // Clicked below lines
		fp.xy_outbound = true;
		fp.pos = ll->length();

		unsigned int fold_start;
		if (m_editorCtrl.IsPosInFold(fp.pos, &fold_start)) {
			// If doc ends with a fold we have to move pos
			fp.pos = fold_start;
			const wxPoint cpos = GetCharPos(fp.pos);
			fp.caret_xpos = cpos.x;
			fp.caret_ypos = cpos.y;
		}
		else if (NewlineTerminated) {
			fp.caret_ypos = ll->height();
		}
		else {
			SetLine(ll->size()-1);
			const wxPoint cpos = line.GetCaretPos(ll->length()-ll->offset(ll->size()-1));

			fp.caret_xpos = cpos.x;
			fp.caret_ypos = ll->top(ll->size()-1) + cpos.y;
		}

		if (doSetPos) pos = fp.pos; caretpos.x = fp.caret_xpos; caretpos.y = fp.caret_ypos;
		fp.caret_ypos = FoldedYPos(fp.caret_ypos);
		return fp;
	}

	// Find the line clicked on
	int l = ll->find_ypos(ypos);
	SetLine(l);

	wxASSERT(ll->top(l) <= ypos);
	fp = line.ClickOnLine(xpos, ypos - ll->top(l));
	fp.pos += ll->offset(l);
	fp.caret_ypos += ll->top(l);

	if (doSetPos) pos = fp.pos; caretpos.x = fp.caret_xpos; caretpos.y = fp.caret_ypos;
	fp.caret_ypos = FoldedYPos(fp.caret_ypos);
	return fp;
}

full_pos Lines::MovePosUp(int xpos) {
	if (xpos == -1) xpos = caretpos.x;

	const int up_pos = FoldedYPos(caretpos.y) - 1;
	if (up_pos >= 0) {
		return ClickOnLine(xpos, up_pos);
	}

	const full_pos fp = {pos, caretpos.x, FoldedYPos(caretpos.y)};
	return fp;
}

full_pos Lines::MovePosDown(int xpos) {
	if (xpos == -1) xpos = caretpos.x;

	const unsigned int down_pos = FoldedYPos(caretpos.y) + line.GetCharHeight() + 1;
	return ClickOnLine(xpos, down_pos);
}

unsigned int Lines::GetUnFoldedHeight() const {
	return NewlineTerminated ? ll->height() + line.GetCharHeight() : ll->height();
}

unsigned int Lines::FoldedYPos(unsigned int ypos) const {
	const unsigned int height = GetUnFoldedHeight();
	wxASSERT(ypos <= height);

	const vector<cxFold>& m_folds = m_editorCtrl.GetFolds();
	unsigned int folded_ypos = ypos;

	vector<cxFold>::const_iterator p = m_folds.begin();
	while (p != m_folds.end()) {
		if (p->type == cxFOLD_START_FOLDED) {
			// Check if we have passed ypos
			const unsigned int line_bottom = ll->bottom(p->line_id);
			if (line_bottom >= ypos) break;

			// Find bottom ypos
			const unsigned int lastline = p->line_id + p->count;
			const unsigned int fold_bottom = (lastline == ll->size()) ? height : ll->bottom(lastline);
			wxASSERT_MSG(ypos >= fold_bottom || ypos == height, wxT("ypos in fold"));

			// Adjust ypos
			folded_ypos -= (fold_bottom - line_bottom);

			// Advance to end of fold
			while (p != m_folds.end() && p->line_id <= lastline) ++p;
		}
		else ++p;
	}

	return folded_ypos;
}

unsigned int Lines::UnFoldedYPos(unsigned int ypos) const {
	//const unsigned int height = GetUnFoldedHeight();
	// you can click outside text - wxASSERT((int)ypos <= height);
	wxASSERT((int)ypos >= 0);

	const vector<cxFold>& m_folds = m_editorCtrl.GetFolds();
	unsigned int unfolded_ypos = ypos;

	vector<cxFold>::const_iterator p = m_folds.begin();
	while (p != m_folds.end()) {
		if (p->type == cxFOLD_START_FOLDED) {
			// Check if we have passed ypos
			const unsigned int line_bottom = ll->bottom(p->line_id);
			if (line_bottom >= unfolded_ypos) break;

			// Find bottom ypos
			const unsigned int lastline = p->line_id + p->count;
			const unsigned int fold_bottom = (lastline == ll->size()) ? GetUnFoldedHeight() : ll->bottom(lastline);

			// Adjust ypos
			unfolded_ypos += (fold_bottom - line_bottom);

			// Advance to end of fold
			while (p != m_folds.end() && p->line_id <= lastline) ++p;
		}
		else ++p;
	}

	return unfolded_ypos;
}

vector<cxLineChange> Lines::ChangesToFullLines(const vector<cxChange>& changes) {
	unsigned int change_line_start = GetLength();
	unsigned int change_line_end = change_line_start;
	int change_diff = 0;
	int change_lines = 0;
	int offset = 0;
	vector<cxLineChange> linechanges;

	// Convert changes to span full lines
	// (Make sure lines are valid before calling this)
	for (vector<cxChange>::const_iterator p = changes.begin(); p != changes.end(); ++p) {
		const unsigned int change_start = p->type == cxINSERTION ? p->start : p->start + offset;
		const unsigned int line_start = GetLineStartFromPos(change_start);

		// Add changed line(s)
		if (change_start > change_line_end) {
			const cxLineChange lc = {change_line_start, change_line_end, change_diff, change_lines};
			linechanges.push_back(lc);

			change_line_end = change_line_start = GetLength();
			change_lines = change_diff = 0;
		}

		const unsigned int len = p->end - p->start;

		if (p->type == cxINSERTION) {
			offset += len;
			
			change_line_start = wxMin(GetLineStartFromPos(p->start), change_line_start);
			change_line_end = GetLineEndFromPos(p->end);
			change_diff += len;
			change_lines += p->lines;
		}
		else { // if (p->type == cxDELETION)
			offset -= len;

			change_line_start = wxMin(line_start, change_line_start);
			change_line_end = GetLineEndFromPos(change_start);
			change_diff -= len;
			change_lines -= p->lines;
		}
	}
	if (!changes.empty()) {
		const cxLineChange lc = {change_line_start, change_line_end, change_diff, change_lines};
		linechanges.push_back(lc);
	}

	return linechanges;
}
