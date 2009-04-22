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

#include "LineListWrap.h"
#include <algorithm>

// Initializing static constants
const unsigned int LineListWrap::WINSIZE = 200;

LineListWrap::LineListWrap(FixedLine& l, const DocumentWrapper& dw) : line(l), m_doc(dw), lastValidOffset(0), lastValidPos(0),
		firstLoadedPos(0), lastLoadedPos(0), offsetDiff(0), posDiff(0), avr_lineheight(0) {
}

unsigned int LineListWrap::offset(unsigned int index) {
	wxASSERT(0 <= index && index < textOffsets.size());

	if (!index) return 0;

	validate_offsets(index-1);
	return textOffsets[index-1];
}

unsigned int LineListWrap::end(unsigned int index) {
	wxASSERT(0 <= index && index < textOffsets.size());

	validate_offsets(index);
	return textOffsets[index];
}

unsigned int LineListWrap::top(unsigned int index) {
	wxASSERT(index == 0 || (0 < index && index < yPositions.size()));
	
	if (!index) return 0;

	validate_positions(index-1);
	return yPositions[index-1];
}

unsigned int LineListWrap::bottom(unsigned int index) {
	wxASSERT(0 <= index && index < yPositions.size());

	validate_positions(index);
	return yPositions[index];
}

unsigned int LineListWrap::size() const {
	wxASSERT(textOffsets.size() == yPositions.size());

	return textOffsets.size();
}

unsigned int LineListWrap::last() const {
	wxASSERT(textOffsets.size() == yPositions.size());
	wxASSERT(!textOffsets.empty());

	return textOffsets.size()-1;
}

unsigned int LineListWrap::height() const {
	if (yPositions.empty()) return 0;

	wxASSERT(lastLoadedPos != 0);
	wxASSERT(yPositions.size() == size());

	if (lastLoadedPos < yPositions.size()) {
		int last = yPositions[lastLoadedPos-1];
		if (lastValidPos < lastLoadedPos) last += posDiff;

		return last + approxBottom;
	}
	
	if (lastValidPos < yPositions.size()) return yPositions.back() + posDiff;
	
	return yPositions.back();
}

unsigned int LineListWrap::width() const {
	// Should never be called, only used with nowrap
	wxASSERT(false);
	return line.GetDisplayWidth();
};

unsigned int LineListWrap::length() const {
	if (textOffsets.empty()) return 0;

	if (lastValidOffset < textOffsets.size()) return textOffsets.back() + offsetDiff;
	else return textOffsets.back();
}

void LineListWrap::add_line(int end) {
	wxASSERT(!textOffsets.size() || end > (int)textOffsets.back());
	wxASSERT(firstLoadedPos == 0 && lastLoadedPos == 0);

	textOffsets.push_back(end);
}

vector<unsigned int>& LineListWrap::GetOffsets() {
	return textOffsets;
}

void LineListWrap::SetOffsets(const vector<unsigned int>& offsets) {
	wxASSERT(lastValidOffset == 0); // LineList has to be cleared first

	textOffsets = offsets;

	if (textOffsets.size() != yPositions.size()) yPositions.resize(textOffsets.size());
	lastValidOffset = textOffsets.size();
}

// You have to call this after textOffsets heve been modified using GetOffsets()
void LineListWrap::NewOffsets() {
	wxASSERT(lastValidOffset == 0);
	if (textOffsets.size() != yPositions.size()) yPositions.resize(textOffsets.size());
	lastValidOffset = textOffsets.size();

	invalidate();
}

void LineListWrap::insert(unsigned int index, int newend) {
	// WARNING: ll only valid upto index
	wxASSERT(index >= 0 && index <= textOffsets.size());
	wxASSERT(newend <= (int)m_doc.GetLength());

	if (index) {
		validate_offsets(index-1);
		validate_positions(index-1);
	}
	wxASSERT(!index || newend > (int)end(index-1));

	// Insert the text
	textOffsets.insert(textOffsets.begin()+index, newend);

	// Update the offsets
	if (lastValidOffset > index) ++lastValidOffset; // pushed down by insertion
	update_offsetDiff(index, newend - (index ? textOffsets[index-1] : 0));

	// The height of the line depends on the markup, so for now we just
	// set it to the unwrapped height. It will be updated when it has been
	// syntax highlighted.
	if (index) yPositions.insert(yPositions.begin()+index, yPositions[index-1] + line.GetCharHeight());
	else yPositions.insert(yPositions.begin()+index, line.GetCharHeight());

	// Extend the loaded positions window
	if (lastLoadedPos == 0) {
		wxASSERT(firstLoadedPos == 0);
		//firstLoadedPos = 0;
		lastLoadedPos = 1;
	}
	else {
		wxASSERT(lastLoadedPos >= index);
		++lastLoadedPos;
	}

	// Update the positions
	if (lastValidPos > index) ++lastValidPos; // pushed down by insertion
	update_posDiff(index, line.GetCharHeight());

	recalc_approx();

	wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
	wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);
}

void LineListWrap::insertlines(unsigned int index, vector<unsigned int>& newlines) {
	// WARNING: ll only valid upto index
	wxASSERT(index >= 0 && index <= textOffsets.size());
	wxASSERT(!newlines.empty());

	if (index) {
		validate_offsets(index-1);
		validate_positions(index-1);
	}

	// Insert the lines
	textOffsets.insert(textOffsets.begin()+index, newlines.begin(), newlines.end());

	// Update the offsets
	if (lastValidOffset > index) lastValidOffset += newlines.size(); // pushed down by insertion
	update_offsetDiff(index + newlines.size()-1, newlines.back() - (index ? textOffsets[index-1] : 0));

	// Extend the yPositions to match
	yPositions.insert(yPositions.begin()+index, newlines.size(), 0);

	// Set the positions
	int textstart = 0, ypos = 0;
	if (index) {
		textstart = textOffsets[index-1];
		ypos = yPositions[index-1];
	}
	for (unsigned int i = index ; i < index + newlines.size(); ++i) {
		// The height of the line depends on the markup, so for now we just
		// set it to the unwrapped height. It will be updated when it has been
		// syntax highlighted.
		yPositions[i] = ypos += line.GetCharHeight();
		textstart = textOffsets[i];
	}
	
	int last_index = index + newlines.size()-1;

	// Extend l-window to hold new lines
	if (lastLoadedPos == 0) {
		wxASSERT(firstLoadedPos == 0);
		//firstLoadedPos = 0;
		lastLoadedPos = last_index+1;
	}
	else {
		wxASSERT(lastLoadedPos >= index);
		lastLoadedPos += newlines.size();
	}

	// Update the positions
	if (lastValidPos > index) lastValidPos += newlines.size(); // pushed down by insertion
	update_posDiff(last_index, ypos - (index ? yPositions[index-1] : 0));

	recalc_approx();

	wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
	wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);
}

void LineListWrap::update(unsigned int index, unsigned int newend) {
	//wxLogDebug("update %u %u", index, newend);
	//wxLogDebug("  fLoaded=%u lValid=%u lLoaded=%u", firstLoadedPos, lastValidPos, lastLoadedPos);

	// WARNING: ll only valid upto index
	wxASSERT(index >= 0 && index < textOffsets.size());
	//wxASSERT(newend <= m_doc.GetLength());
	
	validate_offsets(index);
	if (index) validate_positions(index-1);

	// Set the text
	int tdiff = newend - textOffsets[index];
	textOffsets[index] = newend;
	update_offsetDiff(index, tdiff);

	// The height of the line depends on the markup, so for now we just keep
	// the old height. It will be updated when it has been syntax highlighted.
}

void LineListWrap::update_parsed_line(unsigned int index) {
	wxASSERT(index < textOffsets.size());

	// Only update if line is in_window or just beyond
	if (index >= firstLoadedPos && index <= lastLoadedPos) {
		// Recalculate line height
		line.SetLine(index ? textOffsets[index-1] : 0, textOffsets[index]);
		const unsigned int line_height = line.GetHeight();
		const unsigned int top_xpos = index ? yPositions[index-1] : 0;

		if (index == lastLoadedPos) {
			yPositions[index] = top_xpos + line_height;
			++lastLoadedPos;
			recalc_approx();
		}
		else {
			// Get the diffs
			unsigned int bottom_xpos = yPositions[index];
			if (index >= lastValidPos) {
				bottom_xpos += posDiff;
			}
			const unsigned int old_height = bottom_xpos - top_xpos;
			//wxLogDebug("  old_height=%d posDiff=%d top_xpos=%d bottom_xpos=%d", old_height, posDiff, top_xpos, bottom_xpos);
			
			// Update position
			if (line_height != old_height || index >= lastValidPos) {
				yPositions[index] = top_xpos + line_height;
				update_posDiff(index, line_height - old_height);

				wxASSERT((bottom(index) - top(index)) == line_height);

				recalc_approx();
			}
		}

		wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
		wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);
	}
}

void LineListWrap::update_line_extent(unsigned int index, unsigned int extent) {
	wxASSERT(index < textOffsets.size());

	// Only update if line is in_window or just beyond
	if (index >= firstLoadedPos && index <= lastLoadedPos) {
		// Make sure the positions we need are valid
		if (index >= lastValidPos) {
			const size_t end = index+1;
			for (size_t i = lastValidPos; i < end; ++i) {
				yPositions[i] += posDiff;
			}
			lastValidPos = end;
			if (lastValidPos == yPositions.size()) posDiff = 0;
		}

		const unsigned int top_ypos = index ? yPositions[index-1] : 0;

		if (index == lastLoadedPos) {
			yPositions[index] = top_ypos + extent;
			++lastLoadedPos;
			recalc_approx();
		}
		else {
			// Get the diffs
			const unsigned int bottom_ypos = yPositions[index];
			const unsigned int old_height = bottom_ypos - top_ypos;
			//wxLogDebug("  old_height=%d posDiff=%d top_xpos=%d bottom_xpos=%d", old_height, posDiff, top_xpos, bottom_xpos);
			
			// Update position
			if (extent != old_height || index >= lastValidPos) {
				yPositions[index] = top_ypos + extent;
				update_posDiff(index, extent - old_height);

				wxASSERT((bottom(index) - top(index)) == extent);

				recalc_approx();
			}
		}

		wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
		wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);
	}
	verify();
}

void LineListWrap::remove(unsigned int startline, unsigned int endline) {
	//wxLogDebug("remove %u %u", startline, endline);
	//wxLogDebug("  fLoaded=%u lValid=%u lLoaded=%u", firstLoadedPos, lastValidPos, lastLoadedPos);
	// WARNING: ll only valid upto startline
	wxASSERT(startline >= 0 && startline < textOffsets.size());
	wxASSERT(endline > startline && endline <= textOffsets.size());
	wxASSERT(size());

	// Check if we are deleting the entire text
	if (startline == 0 && endline == size()) {
		clear();
		return;
	}

	// Validate offsets up-to end of deletion (needed for diff)
	validate_offsets(endline-1);
	
	if (startline) extendwindow(startline-1); // Make sure pos-win reaches top of deletion

	// Remove text lines
	int diff = offset(startline) - end(endline-1);
	textOffsets.erase(textOffsets.begin()+startline, textOffsets.begin()+endline);
	
	// update offsets
	wxASSERT(lastValidOffset >= endline - startline);
	lastValidOffset -= endline - startline;
	if (startline) update_offsetDiff(startline-1, diff);
	else {
		// Top line have been removed, so we have to update the new top
		wxASSERT(!textOffsets.empty());
		if (lastValidOffset == 0) textOffsets[0] += offsetDiff;
		textOffsets[0] += diff;
		update_offsetDiff(startline, diff);
	}

	if (startline >= lastLoadedPos) {
		// We can delete positions without having to think about the pos-window
		yPositions.erase(yPositions.begin()+startline, yPositions.begin()+endline);
	}
	else if (endline <= lastLoadedPos) {
		// Calculate a diff on positions
		int top_ypos = startline ? yPositions[startline-1] : 0;
		if (startline > lastValidPos) top_ypos += posDiff;
		int bottom_ypos = yPositions[endline-1];
		if (endline > lastValidPos) bottom_ypos += posDiff;

		int pdiff = top_ypos - bottom_ypos;

		// resize l-window
		if (startline <= firstLoadedPos && endline >= lastLoadedPos) {
			firstLoadedPos = 0;
			lastLoadedPos = 0;
			lastValidPos = 0;
		}
		else if (startline >= firstLoadedPos) {
			wxASSERT(startline < lastLoadedPos);
			lastLoadedPos -= endline - startline; 
		}
		else wxASSERT(false);

		// Remove position lines
		yPositions.erase(yPositions.begin()+startline, yPositions.begin()+endline);
		
		// FIX: update lastValidPos upto startline
		while (lastValidPos < startline) {
			yPositions[lastValidPos] += posDiff;
			++lastValidPos;
		}
		if (lastValidPos == yPositions.size()) posDiff = 0; // update complete
		if (lastValidPos > startline) { // Have lastValidPos been moved?
			if (lastValidPos <= endline) lastValidPos = startline;
			else lastValidPos -= endline - startline;
		}

		if (startline) update_posDiff(startline-1, pdiff);
		else {
			// Top line have been removed, so we have to update the new top
			wxASSERT(!yPositions.empty());
			if (lastValidPos == 0) yPositions[0] += posDiff;
			yPositions[0] += pdiff;
			update_posDiff(startline, pdiff);
		}
	}
	else {
		// Overlap: We have to truncate pos-win
		lastLoadedPos = startline;
		if (lastValidPos > startline) {
			lastValidPos = startline;
			posDiff = 0;
		}

		// Remove position lines
		yPositions.erase(yPositions.begin()+startline, yPositions.begin()+endline);
	}

	recalc_approx();

	wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
	wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);
}

void LineListWrap::clear() {
	textOffsets.clear();
	yPositions.clear();

	lastValidOffset = 0;
	lastValidPos = 0;
	firstLoadedPos = 0;
	lastLoadedPos = 0;
	offsetDiff = 0;
	posDiff = 0;
	avr_lineheight = 0;
}

bool LineListWrap::IsLineEnd(unsigned int pos) {
	if (!size()) return false;
	validate_offsets(size()-1);
	wxASSERT(pos <= textOffsets.back());

	vector<unsigned int>::const_iterator posline = lower_bound(textOffsets.begin(), textOffsets.end(), pos);
	return (pos == *posline);
}

unsigned int LineListWrap::EndFromPos(unsigned int pos) {
	if (!textOffsets.size()) return 0;
	validate_offsets(textOffsets.size()-1);
	wxASSERT(pos <= textOffsets.back());

	vector<unsigned int>::const_iterator posline = lower_bound(textOffsets.begin(), textOffsets.end(), pos);
	if (pos != *posline) return *posline;
	return pos == textOffsets.back() ? pos : *(++posline);
}

unsigned int LineListWrap::StartFromPos(unsigned int pos) {
	if (!textOffsets.size()) return 0;
	validate_offsets(textOffsets.size()-1);
	wxASSERT(pos <= textOffsets.back());

	vector<unsigned int>::const_iterator posline = lower_bound(textOffsets.begin(), textOffsets.end(), pos);
	if (pos == *posline) return pos;
	return (posline == textOffsets.begin()) ? 0 : *(--posline);
}

int LineListWrap::find_offset(int pos) {
	if (!size()) return 0;
	validate_offsets(size()-1);
	wxASSERT(0 <= pos && pos <= (int)textOffsets.back());

	vector<unsigned int>::iterator posline = lower_bound(textOffsets.begin(), textOffsets.end(), (unsigned int)pos);
	return distance(textOffsets.begin(), posline);
}

int LineListWrap::find_ypos(unsigned int ypos) {
	wxASSERT(ypos >= 0 && ypos <= height());
	wxASSERT(approxTop >= 0);
	verify();

	if (!size()) return 0;
	wxASSERT(lastLoadedPos != 0 && firstLoadedPos < lastLoadedPos);

	bool do_approx = true;

	while (1) {
		// Often we want one just above or below current window
		if (ypos < (unsigned int)approxTop) {
			// Expand window up a bit
			int recalc = extendwindow(uMinus(firstLoadedPos, WINSIZE/2));

			unsigned int new_ypos = uPluss(ypos, recalc);
			new_ypos = wxMin(new_ypos, height());
			if (new_ypos >= (unsigned int)approxTop) ypos = new_ypos;
		}
		else if (lastValidPos && ypos > yPositions[lastValidPos-1]) {
			// Expand window down a bit
			int recalc = extendwindow(wxMin(last(), lastLoadedPos+(WINSIZE/2)));
			wxASSERT(recalc == 0); // there should be no change of approxTop
		}

		if (ypos >= (unsigned int)approxTop) {
			validate_positions(lastLoadedPos-1); // Make sure lastValidPos is at end of window
			wxASSERT(lastValidPos > 0);

			if (ypos <= yPositions[lastValidPos-1]) { // ypos is inside window
				vector<unsigned int>::iterator posline = lower_bound(yPositions.begin() + firstLoadedPos, yPositions.begin() + lastValidPos, (unsigned int)ypos);
				int index = distance(yPositions.begin(), posline);
				wxASSERT(top(index) <= ypos);
				return index;
			}
			else if (lastValidPos == size()) return last(); // recalc of positions may end up with pos outside text
		}

		// Else we have to find the approximate line
		if (do_approx) {
			wxASSERT(avr_lineheight > 0);
			unsigned int index = ypos / avr_lineheight;
			index = wxMin(index, last());
			extendwindow(index);
			do_approx = false; // only try to approximate once
		}
	}

	wxASSERT(false); // We should never reach here
	return 0; 
}

void LineListWrap::invalidate(int index) {
	// Width has changed, recalc new window
	firstLoadedPos = 0;
	lastLoadedPos = 0;
	lastValidPos = 0;
	approxBottom = 0;
	approxTop = 0;
	posDiff = 0;
	avr_lineheight = 0;
	if (index > 100) {
		// We need the index to keep the right line in focus
		extendwindow(index);
		verify();
	}
	else initwindow();
}

void LineListWrap::validate_offsets(unsigned int index) {
	wxASSERT(index >= 0 && index < textOffsets.size());
	wxASSERT(lastValidOffset >= 0 || textOffsets.empty()); 
	wxASSERT(lastValidOffset < textOffsets.size() || offsetDiff == 0);

	if (index >= lastValidOffset && !textOffsets.empty()) {
		vector<unsigned int>::iterator i2 = textOffsets.begin() + index;
		for (vector<unsigned int>::iterator i = textOffsets.begin()+lastValidOffset; i <= i2; ++i) {
			*i += offsetDiff;
		}
		lastValidOffset = index+1;

		// Check if everything is updated
		wxASSERT(lastValidOffset <= textOffsets.size());
		if (lastValidOffset == textOffsets.size()) {
			offsetDiff = 0;
		}
	}
}

void LineListWrap::validate_positions(unsigned int index) {
	wxASSERT(index >= 0 && index < yPositions.size());
	wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
	wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);

	// Extend window to include index
	extendwindow(index);

	if (index >= lastValidPos) {
		vector<unsigned int>::iterator i2 = yPositions.begin() + index;
		for (vector<unsigned int>::iterator i = yPositions.begin()+lastValidPos; i <= i2; ++i) {
			*i += posDiff;
		}

		lastValidPos = index+1;

		// Check if everything is updated
		if (lastValidPos >= yPositions.size()) posDiff = 0;
	}
}

void LineListWrap::update_offsetDiff(unsigned int index, int diff) {
	// index is to the offset just updated
	wxASSERT(index >= 0 && index < textOffsets.size());

	// Check if everything is updated
	if (lastValidOffset == textOffsets.size()) {
		wxASSERT(offsetDiff == 0);
		if (index == textOffsets.size()-1) offsetDiff = 0;
		else if (diff != 0) {
			offsetDiff = diff;
			lastValidOffset = index+1;
		} 
		return;
	}

	if (index+1 < lastValidOffset) {
		// update up-to and including lastValidOffset
		for (++index; index < lastValidOffset; ++index) {
			textOffsets[index] += diff;
			wxASSERT(textOffsets[index] <= m_doc.GetLength());
		}
	}
	else if (index >= lastValidOffset) {
		// We know that index is updated
		lastValidOffset = index+1;
	}

	if (lastValidOffset < textOffsets.size()) offsetDiff += diff;
	else offsetDiff = 0;
}

void LineListWrap::update_posDiff(unsigned int index, int diff) {
	//wxLogDebug("update_posDiff %u %d", index, diff);
	//wxLogDebug("  fLoaded=%u lValid=%u lLoaded=%u", firstLoadedPos, lastValidPos, lastLoadedPos);

	// index is to the position just updated
	wxASSERT(index >= 0 && index < yPositions.size());

	// Check if everything is updated
	if (lastValidPos == yPositions.size()) {
		wxASSERT(posDiff == 0);
		if (index == yPositions.size()-1) posDiff = 0;
		else if (diff != 0) {
			posDiff = diff;
			lastValidPos = index+1;
		}
		return;
	}
	
	if (index+1 < lastValidPos) {
		// update up-to lastValidPos
		for (++index; index < lastValidPos; ++index) {
			yPositions[index] += diff;
		}
	}
	else if (index >= lastValidPos) {
		// We know that index is updated
		lastValidPos = index+1;
	}

	if (lastValidPos < lastLoadedPos) posDiff += diff;
	else posDiff = 0;
}

void LineListWrap::Print() {
	wxLogDebug(wxT("\nLineList len=%d"), size());
	wxLogDebug(wxT(" lastValidOffset: %u"), lastValidOffset);
	wxLogDebug(wxT(" lastValidPos:    %u"), lastValidPos);
	wxLogDebug(wxT(" firstLoadedPos:    %u"), firstLoadedPos);
	wxLogDebug(wxT(" lastLoadedPos:    %u"), lastLoadedPos);
	wxLogDebug(wxT(" offsetDiff:    %d"), offsetDiff);
	wxLogDebug(wxT(" posDiff:    %d"), posDiff);
	wxLogDebug(wxT(" approxTop:   %d"), approxTop);
	wxLogDebug(wxT(" height:     %u"), height());
	if (lastLoadedPos == 0) wxLogDebug(wxT("  _linelist: %d-%d"), firstLoadedPos, lastLoadedPos);
	else wxLogDebug(wxT("  linelist: %d %d (%d-%d -> %d-%d) %u"), lastValidOffset, lastValidPos, firstLoadedPos, lastLoadedPos, approxTop, yPositions[lastLoadedPos-1], height());
	
	for (unsigned int i = 0; i < size(); ++i) {
		wxLogDebug(wxT("  %u: %u %u"), i, textOffsets[i], yPositions[i]);
	}
}

void LineListWrap::initwindow() {
	if (!size()) return;

	if (line.IsValid()) {
		unsigned int endline = wxMin(100, size());
		unsigned int top = 0;

		for (unsigned int i = 0; i < endline; ++i) {
			top = yPositions[i] = top + line.GetQuickLineHeight(offset(i), end(i));
		}

		firstLoadedPos = 0;
		lastLoadedPos = endline;
		lastValidPos = endline;

		// Calculate approximate height of space under window
		if (endline < size()) {
			avr_lineheight = top / endline;
			approxBottom = (size() - endline) * avr_lineheight;
		}
		else approxBottom = 0;

		verify(true);
	}
	else {
		// Fill positions with height of unwrapped lines
		unsigned int ypos = 0;
		const unsigned int charheight = line.GetCharHeight();
		for (vector<unsigned int>::iterator p = yPositions.begin(); p != yPositions.end(); ++p) {
			ypos += charheight;
			*p = ypos;
		}

		firstLoadedPos = 0;
		lastLoadedPos = lastValidPos = size();
		avr_lineheight = charheight;
	}
}

int LineListWrap::extendwindow(unsigned int index) {
	wxASSERT(index >= 0 && index < yPositions.size());

	if (size() == 0) return 0;
	if (index >= firstLoadedPos && index < lastLoadedPos) return 0; // in window

	// Check if we can extend current window
	if (index < firstLoadedPos) { // Index above window
		if (WINSIZE > firstLoadedPos || index > firstLoadedPos-WINSIZE) {
			// extend window to index
			index = uMinus(firstLoadedPos, WINSIZE); // do a full block

			// Load positions in extension
			unsigned int line_bottom = 0;
			for (unsigned int i = index; i < firstLoadedPos; ++i) {
				line_bottom = yPositions[i] = line_bottom + line.GetQuickLineHeight(offset(i), end(i));
			}

			// Calculate new approxTop
			unsigned int new_approxTop = 0;
			if (index) {
				unsigned int loadedheight = yPositions[lastLoadedPos-1] - (firstLoadedPos ? yPositions[firstLoadedPos] : 0);
				avr_lineheight = (yPositions[firstLoadedPos-1] + loadedheight) / (lastLoadedPos - index);
				new_approxTop = index * avr_lineheight;
			}

			// Adjust all positions to new approxTop
			int ext_diff = line_bottom - approxTop;
			for (unsigned int i2 = index; i2 < lastLoadedPos; ++i2) {
				yPositions[i2] += new_approxTop;
				if (i2 >= firstLoadedPos) yPositions[i2] += ext_diff;
			}

			approxTop = new_approxTop;
			firstLoadedPos = index;
			recalc_approx();
			return ext_diff;
		}
	}
	else { // Index below window
		if (lastLoadedPos != 0 && index+1 < lastLoadedPos+WINSIZE) {
			// extend window to index
			//index = wxMin(last(), lastLoadedPos+WINSIZE); // do a full block
			int top = yPositions[lastLoadedPos-1];
			for (unsigned int i = lastLoadedPos; i <= index; ++i) {
				yPositions[i] = top += line.GetQuickLineHeight(offset(i), end(i));
			}
			if (lastValidPos == lastLoadedPos) lastValidPos = index+1;
			lastLoadedPos = index+1;

			return recalc_approx(); // recalc approximations (below only)
		}
	}

	// We have to create a new window
	// The window can't go beyond index
	approxDiff = 0;
	int endline = index;
	index = uMinus(index, WINSIZE);

	int top = 0; 
	if (avr_lineheight > 0) top = approxTop = index * avr_lineheight;
	else wxASSERT(approxTop == 0);
	for (int i = index; i <= endline; ++i) {
		top = yPositions[i] = top + line.GetQuickLineHeight(offset(i), end(i));
	}
	firstLoadedPos = index;
	lastValidPos =lastLoadedPos = endline+1;

	wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);
	wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);

	return recalc_approx(true); // recalc approximations;
}

int LineListWrap::recalc_approx(bool movetop) {
	wxASSERT(lastLoadedPos != 0 && firstLoadedPos < lastLoadedPos);

	unsigned int firsttop = firstLoadedPos > 0 ? yPositions[firstLoadedPos] : 0;
	if (lastLoadedPos > firstLoadedPos) {
		avr_lineheight = (yPositions[lastLoadedPos-1] - firsttop) / (lastLoadedPos - firstLoadedPos);
	}
	else avr_lineheight = line.GetCharHeight();

	if (lastLoadedPos == size()) approxBottom = 0;
	else {
		wxASSERT(lastLoadedPos < size());
		approxBottom = (size() - lastLoadedPos) * avr_lineheight;
	}

	// Move top of window to fit with new approxTop
	if (movetop) {
		unsigned int new_approxTop = 0;
		if (firstLoadedPos)	new_approxTop = firstLoadedPos * avr_lineheight;
		wxASSERT(new_approxTop >= 0);

		//line.SetLine(offset(firstLoadedPos), end(firstLoadedPos));
		//wxASSERT((int)(yPositions[firstLoadedPos] - approxTop) == line.GetHeight());

		int apr_diff = new_approxTop - approxTop;
		wxASSERT(apr_diff >= 0 || -apr_diff <= (int)yPositions[firstLoadedPos]);
		
		for (unsigned int i = firstLoadedPos; i < lastLoadedPos; ++i) {
			yPositions[i] += apr_diff;
		}
		wxASSERT(yPositions[firstLoadedPos] >= 0);
		//wxASSERT((int)(yPositions[firstLoadedPos] - new_approxTop) == line.GetHeight());
		wxASSERT(new_approxTop >= 0 && new_approxTop < yPositions[firstLoadedPos]);

		approxTop = new_approxTop;

		approxDiff += apr_diff;
		return apr_diff;
	}
	else return 0; // No change of approxTop
}

int LineListWrap::prepare(int ypos) {
	verify();

	approxDiff = 0;
	if (!size()) return 0;

	if (ypos == -1) {
		validate_positions(last());
	}
	else {
		// adjust ypos a bit so we are sure to get the line above as well
		find_ypos(uMinus(ypos, 100));
	}
	return approxDiff;
}

bool LineListWrap::in_window(unsigned int pos) {
	if (pos == 0 && size() == 0) return true;
	wxASSERT(size());

	unsigned int index = find_offset(pos);
	return (index >= firstLoadedPos && index < lastLoadedPos);
}

int LineListWrap::OnIdle() {
	if (!size()) return 0;
	wxASSERT(lastLoadedPos != 0 && firstLoadedPos < lastLoadedPos);

	//wxLogDebug(wxT("ll.OnIdle %d %d (%d)"), firstLoadedPos, lastLoadedPos, size());

	if (firstLoadedPos > 0) {
		//wxLogDebug("OnIdle 1");
		unsigned int index = uMinus(firstLoadedPos, 100);
		
		// Load positions in extension
		unsigned int line_bottom = 0;
		for (unsigned int i = index; i < firstLoadedPos; ++i) {
			line.SetLine(offset(i), end(i));
			line_bottom = yPositions[i] = line_bottom + line.GetHeight();
		}

		// Calculate new approxTop
		unsigned int new_approxTop = 0;
		if (index) {
			unsigned int loadedheight = yPositions[lastLoadedPos-1] - (firstLoadedPos ? yPositions[firstLoadedPos] : 0);
			avr_lineheight = (yPositions[firstLoadedPos-1] + loadedheight) / (lastLoadedPos - index);
			new_approxTop = index * avr_lineheight;
		}

		// Adjust all positions to new approxTop
		int ext_diff = line_bottom - approxTop;
		for (unsigned int i2 = index; i2 < lastLoadedPos; ++i2) {
			yPositions[i2] += new_approxTop;
			if (i2 >= firstLoadedPos) yPositions[i2] += ext_diff;
		}

		approxTop = new_approxTop;
		firstLoadedPos = index;
		recalc_approx();

/*		int bottom = yPositions[firstLoadedPos];
		for (unsigned int i = firstLoadedPos; i >= index && i <= firstLoadedPos; --i) {
			line.SetLine(offset(i), end(i));
			yPositions[i] = bottom;
			bottom -= line.GetHeight();
		}
		int diff = yPositions[firstLoadedPos-1] - bottom;
		wxASSERT(bottom < (int)yPositions[firstLoadedPos-1]);
		wxASSERT(diff >= 0);
		approxTop -= diff;
		firstLoadedPos = index;
		int aprx = recalc_approx(true);*/
		
		verify();
		return ext_diff;
	}
	else if (lastLoadedPos != size()) {
		//wxLogDebug("OnIdle 2");
		wxASSERT(lastLoadedPos < size());
		int index = wxMin(lastLoadedPos+100, last());
		int top = yPositions[lastLoadedPos-1];
		for (int i = lastLoadedPos; i <= index; ++i) {
			line.SetLine(offset(i), end(i));
			yPositions[i] = top += line.GetHeight();
		}
		if (lastValidPos == lastLoadedPos && posDiff == 0) lastValidPos = index+1;
		lastLoadedPos = index+1;
		int aprx = recalc_approx(); // recalc approximations (below only)
		
		verify();
		return aprx;
	}
	return 0;
}

// DEBUG ONLY
void LineListWrap::verify(bool deep) const {
#ifdef  __WXDEBUG__
	//Print();

	// Disable deep check for betatesters to avoid uacceptable slow handline of large files
	deep = false;

	// Make sure vector sizes are in sync
	wxASSERT(textOffsets.size() == yPositions.size());
	if (textOffsets.empty()) return;

	wxASSERT(lastValidPos <= yPositions.size());
	wxASSERT(lastLoadedPos <= yPositions.size());
	wxASSERT(lastValidPos > firstLoadedPos && lastValidPos <= lastLoadedPos);

	// Make sure the diff variables get reset
	wxASSERT(lastValidOffset < textOffsets.size() || offsetDiff == 0);
	wxASSERT(lastValidPos < yPositions.size() || posDiff == 0);

	wxASSERT(approxTop >= 0);

	// Do a check to see if there is something in the pos-window
	wxASSERT(lastValidPos == 0 || yPositions[lastValidPos-1] > 0);

	// Check that positions are sequential
	if (lastValidPos > 0) {
		unsigned int prevPos = yPositions[firstLoadedPos];
		for (size_t i = firstLoadedPos+1; i < lastValidPos; ++i) {
			wxASSERT(prevPos < yPositions[i]);
			prevPos = yPositions[i];
		}
	}

	if (deep) {
		// Check that textOffsets are sequential
		if (lastValidOffset > 0) {
			int offset = 0;
			for (unsigned int i = 0; i < textOffsets.size(); ++i) {
				int end = textOffsets[i];
				if (i >= lastValidOffset) end += offsetDiff;

				wxASSERT(end > offset);
				offset = end;
			}
		}
	}
	
	// textOffsets within range?
	int t = lastValidOffset < textOffsets.size() ? textOffsets.back() + offsetDiff: textOffsets.back();
	int l = m_doc.GetLength();
	if (t != l) wxLogDebug(wxT("%d != %d"), t, l);
	wxASSERT(t == l);

	if (deep) {

		// SuperDebug: Verify that no lines contains newlines
		int offset = 0;
		for (unsigned int i = 0; i < textOffsets.size(); ++i) {
			int end = (i >= lastValidOffset ? textOffsets[i] + offsetDiff : textOffsets[i]);
			line.SetLine(offset, end); // verify during breaking
			offset = end;
		}

		// Check that all loaded positions are correct
		if (lastLoadedPos != 0) {
			wxASSERT(firstLoadedPos >= 0 && firstLoadedPos < lastLoadedPos);
			wxASSERT(firstLoadedPos > 0 || approxTop == 0);
			int top = approxTop;
			int offset = 0; if (firstLoadedPos) offset = textOffsets[firstLoadedPos-1];
			if (firstLoadedPos >= lastValidOffset) offset += offsetDiff;
			for (unsigned int i = firstLoadedPos; i < lastLoadedPos; ++i) {
				int bottom = yPositions[i];
				int end = (i >= lastValidOffset ? textOffsets[i] + offsetDiff : textOffsets[i]);
				if (i >= lastValidPos) bottom += posDiff;

				wxASSERT(bottom > top);

				line.SetLine(offset, end);
				wxASSERT(bottom - top == line.GetHeight());

				top = bottom;
				offset = end;
			}
		}
	}

#endif // __WXDEBUG__
}
