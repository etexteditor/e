#include "Bookmarks.h"
#include "Catalyst.h"
#include "Lines.h"

typedef std::vector<cxBookmark>::iterator bookmark_iter;

const std::vector<cxBookmark>& Bookmarks::GetBookmarks() const {
	return this->bookmarks;
}

unsigned int Bookmarks::NextBookmark(const unsigned int line_id) const {
	if (bookmarks.empty()) return NO_BOOKMARK;

	// Find first bookmark after current line
	for (std::vector<cxBookmark>::const_iterator p = bookmarks.begin(); p != bookmarks.end(); ++p)
		if (line_id < p->line_id) return p->line_id;

	return bookmarks.begin()->line_id;
}

unsigned int Bookmarks::PrevBookmark(const unsigned int line_id) const {
	if (bookmarks.empty()) return NO_BOOKMARK;

	// Find first bookmark after current line
	for (std::vector<cxBookmark>::const_reverse_iterator p = bookmarks.rbegin(); p != bookmarks.rend(); ++p)
		if (p->line_id < line_id) return p->line_id;

	return bookmarks.rbegin()->line_id;
}


void Bookmarks::AddBookmark(unsigned int line_id, bool toggle) {
	// wxASSERT(line_id < lines.GetLineCount());

	// Find insert position
	bookmark_iter p = bookmarks.begin();
	while (p != bookmarks.end()) {
		if (p->line_id == line_id) {
			if (toggle) bookmarks.erase(p);
			return; // already added
		}
		else if (line_id < p->line_id) break;
		++p;
	}

	// Insert the new bookmark
	cxBookmark bm;
	bm.line_id = line_id;
	lines.GetLineExtent(line_id, bm.start, bm.end);
	bookmarks.insert(p, bm);
}

void Bookmarks::DeleteBookmark(unsigned int line_id) {
	for (bookmark_iter p = bookmarks.begin(); p != bookmarks.end(); ++p) {
		if (line_id == p->line_id) {
			bookmarks.erase(p);
			return;
		}
		else if (line_id < p->line_id) {
			wxASSERT(false); // no bookmark on line
			return;
		}
	}
}

void Bookmarks::Clear() {bookmarks.clear();}

void Bookmarks::InsertChars(unsigned int pos, unsigned int length) {
	for (bookmark_iter p = bookmarks.begin(); p != bookmarks.end(); ++p) {
		if (pos < p->start) {
			p->start += length;
			p->line_id = lines.GetLineFromStartPos(p->start);
			p->end = lines.GetLineEndpos(p->line_id);
		}
		else if (pos < p->end)
			p->end = lines.GetLineEndpos(p->line_id);
	}
}

void Bookmarks::DeleteChars(unsigned int start, unsigned int end) {
	const unsigned int len = end - start;

	bookmark_iter p = bookmarks.begin();
	// ignore bookmarks before deletion
	while ((p != bookmarks.end()) && (p->end <= start)) ++p;

	while (p != bookmarks.end()) {
		if ((start == p->start && p->end <= end) || (start < p->start && p->start <= end)) {
			// If bookmark is contained within the deleted range, erase it.
			p = bookmarks.erase(p);
			continue;
		}
		else if (p->start <= start) { // Else adjust the end if the bookmarked line had a deletion
			p->end = lines.GetLineEndpos(p->line_id);
			++p;
		}
		else { // Else adjust the start and end down by the deleted amount.
			p->start -= len;
			p->line_id = lines.GetLineFromStartPos(p->start);
			p->end = lines.GetLineEndpos(p->line_id);
			++p;
		}
	}
}

void Bookmarks::ApplyChanges(const std::vector<cxChange>& changes) {
	// if (changes.empty()) return;

	// Check if all has been deleted
	if (lines.IsEmpty()) {
		bookmarks.clear();
		return;
	}

	// Adjust all start positions and remove deleted lines
	int offset = 0;
	for (std::vector<cxChange>::const_iterator ch = changes.begin(); ch != changes.end(); ++ch) {
		const unsigned int len = ch->end - ch->start;
		if (ch->type == cxINSERTION) {
			for (bookmark_iter p = bookmarks.begin(); p != bookmarks.end(); ++p) {
				if (p->start >= ch->start) p->start += len;
			}
			offset += len;
		}
		else { // ch->type == cxDELETION
			const unsigned int start = ch->start + offset;
			const unsigned int end = ch->end + offset;

			bookmark_iter p = bookmarks.begin();
			while (p != bookmarks.end()) {
				if ((p->start > start && p->start <= end) || (p->start == start && ch->lines > 0)) {
					p = bookmarks.erase(p);
					continue;
				}
				else if (p->start > end) p->start -= len;
				++p;
			}
			offset -= len;
		}
	}

	// Update line numbers and ends.
	for (bookmark_iter p = bookmarks.begin(); p != bookmarks.end(); ++p) {
		p->line_id = lines.GetLineFromStartPos(p->start);
		p->end = lines.GetLineEndpos(p->line_id);
	}
}
