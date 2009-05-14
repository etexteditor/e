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

#include "styler_syntax.h"

#include <algorithm>

#include "StyleRun.h"
#include "Document.h"
#include "tm_syntaxhandler.h"
#include "tmStyle.h"
#include "Lines.h"
#include "matchers.h"

const unsigned int Styler_Syntax::EXTSIZE = 1000;

Styler_Syntax::Styler_Syntax(const DocumentWrapper& dw, Lines& lines, TmSyntaxHandler& syntaxHandler)
: m_doc(dw), m_syntaxHandler(syntaxHandler), m_lines(lines), m_syntax_end(0), m_updateLineHeight(false) {
	m_topMatches.subMatcher = NULL;
	m_topStyle = NULL;

#ifdef __WXDEBUG__
	m_verifyEnabled = true;
#endif
}

bool Styler_Syntax::IsParsed() const {
	return !IsOk() || m_syntax_end == m_doc.GetLength();
}

void Styler_Syntax::Clear() {
	Invalidate();
	m_topMatches.subMatcher = NULL;
	m_topStyle = NULL;
	m_syntaxName.Clear();
}

void Styler_Syntax::Invalidate() {
	m_topMatches.flags = 0;
	m_topMatches.matches.clear();
	m_syntax_end = 0;
}

void Styler_Syntax::ReStyle() {
	// Check if base syntax has a style
	// (disabled until styles get more dynamic handling of transparency)
	/*const wxString& topScope = m_topMatches.subMatcher->GetName();
	if (!topScope.empty()) {
		deque<const wxString*> scopes;
		scopes.push_front(&topScope);
		m_topStyle = m_syntaxHandler.GetStyle(scopes);
	}*/

	// Style matches
	ReStyleSub(m_topMatches);
}

void Styler_Syntax::ReStyleSub(const submatch& sm) {
	for (auto_vector<stxmatch>::const_iterator p = sm.matches.begin(); p != sm.matches.end(); ++p) {
		stxmatch& m = *(*p);

		m.st = GetStyle(m);

		// Check if there are submatches
		if (m.subMatch.get()) {
			ReStyleSub(*m.subMatch);
		}
	}
}

bool Styler_Syntax::UpdateSyntax() {
	const cxSyntaxInfo* si = m_syntaxHandler.GetSyntax(m_doc);
	if (!si) return false; // No new syntax found

	if (si->topmatcher != m_topMatches.subMatcher) {
		Clear(); // Prepare for new syntax
		m_syntaxName = si->name;
		m_topMatches.subMatcher = si->topmatcher;
	}

	return true;
}

void Styler_Syntax::SetSyntax(const wxString& syntaxName, const wxString& ext) {
	const cxSyntaxInfo* si = m_syntaxHandler.GetSyntax(syntaxName, ext);
	if (!si) {
		wxASSERT(false);
		Clear(); // No syntax found
		return;
	}

	if (si->topmatcher != m_topMatches.subMatcher) {
		Clear(); // Prepare for new syntax
		m_syntaxName = si->name;
		m_topMatches.subMatcher = si->topmatcher;

		// Check if base syntax has a style
		// (disabled until styles get more dynamic handling of transparency)
		/*const wxString& topScope = m_topMatches.subMatcher->GetName();
		if (!topScope.empty()) {
			deque<const wxString*> scopes;
			scopes.push_front(&topScope);
			m_topStyle = m_syntaxHandler.GetStyle(scopes);
		}*/
	}
}

const deque<const wxString*> Styler_Syntax::GetScope(unsigned int pos) {
	wxASSERT(pos <= m_doc.GetLength());
	if(!m_topMatches.subMatcher) return deque<const wxString*>(); // no active syntax

	// Make sure the syntax is valid
	ParseTo(pos);

	deque<const wxString*> scopes;
	const wxString& topScope = m_topMatches.subMatcher->GetName();
	if (!topScope.empty()) {
		scopes.push_back(&m_topMatches.subMatcher->GetName());
	}

	GetSubScope(pos, m_topMatches, scopes);

	return scopes;
}

void Styler_Syntax::GetSubScope(unsigned int pos, const submatch& sm, deque<const wxString*>& scopes) const {
	for (auto_vector<stxmatch>::const_iterator p = sm.matches.begin(); p != sm.matches.end(); ++p) {
		const stxmatch& m = *(*p);

		if (pos >= m.start && pos < m.end) {
			if (!m.m_name.empty()) scopes.push_back( &m.m_name );

			// Check if there are submatches
			if (m.subMatch.get()) {
				wxASSERT(pos >= m.start);
				GetSubScope(pos - m.start, *m.subMatch, scopes);
			}
			return;
		}
		else if (m.start > pos) break;
	}
}

const deque<interval> Styler_Syntax::GetScopeIntervals(unsigned int pos) const {
	wxASSERT(pos <= m_doc.GetLength());
	if(!HaveActiveSyntax()) return deque<interval>();

	deque<interval> scopes;
	const wxString& topScope = m_topMatches.subMatcher->GetName();
	if (!topScope.empty()) {
		scopes.push_back(interval(0, m_doc.GetLength()));
	}

	GetSubScopeIntervals(pos, 0, m_topMatches, scopes);
	return scopes;
}

void Styler_Syntax::GetSubScopeIntervals(unsigned int pos, unsigned int offset, const submatch& sm, deque<interval>& scopes) const {
	for (auto_vector<stxmatch>::const_iterator p = sm.matches.begin(); p != sm.matches.end(); ++p) {
		const stxmatch& m = *(*p);

		if (pos >= m.start && pos < m.end) {
			if (!m.m_name.empty()) {
				scopes.push_back(interval(offset+m.start, offset+m.end));
			}

			// Check if there are submatches
			if (m.subMatch.get()) {
				wxASSERT(pos >= m.start);
				GetSubScopeIntervals(pos - m.start, offset+m.start, *m.subMatch, scopes);
			}
			return;
		}
		else if (m.start > pos) break;
	}
}

const style* Styler_Syntax::GetStyle(stxmatch& m) const {
	// Build the list of scopes
	deque<const wxString*> scopes;
	if (!m.m_name.empty()) scopes.push_back(&m.m_name);
	const stxmatch* p = m.parent;
	while (p) {
		if (!p->m_name.empty()) {
			scopes.push_front(&p->m_name);
		}
		p = p->parent;
	}

	const wxString& topScope = m_topMatches.subMatcher->GetName();
	if (!topScope.empty()) {
		scopes.push_front(&topScope);
	}

	const style* st = m_syntaxHandler.GetStyle(scopes);
	return st;
}

void Styler_Syntax::GetTextWithScopes(unsigned int start, unsigned int end, vector<char>& text) {
	wxASSERT(start <= end);
	wxASSERT(end <= m_doc.GetLength());

	// Make sure syntax is valid
	if (m_syntax_end < end) {
		DoSearch(m_syntax_end, end, end);
	}

	text.reserve((end - start) * 2);

	// Start tag
	const wxString& topScope = m_topMatches.subMatcher->GetName();
	const wxCharBuffer name = topScope.mb_str();
	const size_t len = strlen(name.data());
	if (len) {
		text.push_back('<');
		text.insert(text.end(), name.data(), name.data() + len);
		text.push_back('>');
	}

	XmlText(0, m_topMatches, start, end, text);

	// End tag
	if (len) {
		text.push_back('<');
		text.push_back('/');
		text.insert(text.end(), name.data(), name.data() + len);
		text.push_back('>');
	}
}

void Styler_Syntax::XmlText(unsigned int offset, const submatch& sm, unsigned int start, unsigned int end, vector<char>& text) const {
	unsigned int textstart = start;

	for (auto_vector<stxmatch>::const_iterator p = sm.matches.begin(); p != sm.matches.end(); ++p) {
		const stxmatch& m = *(*p);

		// Check if there is overlap
		if (m.end <= start) continue;
		if (m.start >= m.end) break;
		const unsigned int matchstart = wxMax(start, m.start);
		const unsigned int matchend = wxMin(end, m.end);

		// Print text before submatch
		if (textstart < matchstart) {
			const unsigned int len = matchstart - textstart;
			text.resize(text.size() + len);
			cxLOCKDOC_READ(m_doc)
				doc.GetTextPart(offset + textstart, offset + matchstart, (unsigned char*)(&*text.end() - len));
			cxENDLOCK
		}

		// Start tag
		const wxCharBuffer name = m.m_name.mb_str();
		const size_t len = strlen(name.data());
		if (len) {
			text.push_back('<');
			text.insert(text.end(), name.data(), name.data() + len);
			text.push_back('>');
		}

		// Subscopes
		if (m.subMatch.get()) {
			XmlText(offset + m.start, *m.subMatch, matchstart - m.start, matchend - m.start, text);
		}
		else {
			// Print text contained in submatch
			if (matchstart < matchend) {
				const unsigned int len = matchend - matchstart;
				text.resize(text.size() + len);
				cxLOCKDOC_READ(m_doc)
					doc.GetTextPart(offset + matchstart, offset + matchend, (unsigned char*)(&*text.end() - len));
				cxENDLOCK
			}
		}

		// End tag
		if (len) {
			text.push_back('<');
			text.push_back('/');
			text.insert(text.end(), name.data(), name.data() + len);
			text.push_back('>');
		}

		textstart = matchend;
	}

	// Print text after last submatch
	if (textstart < end) {
		const unsigned int len = end - textstart;
		text.resize(text.size() + len);
		cxLOCKDOC_READ(m_doc)
			doc.GetTextPart(offset + textstart, offset + end, (unsigned char*)(&*text.end() - len));
		cxENDLOCK
	}
}

void Styler_Syntax::GetSymbols(vector<SymbolRef>& symbols) const {
	if(!HaveActiveSyntax()) return;

	deque<const wxString*> scopes;

	// Add top scope
	const wxString& topScope = m_topMatches.subMatcher->GetName();
	if (!topScope.empty()) {
		scopes.push_back(&m_topMatches.subMatcher->GetName());
	}

	// Check for matching symbol
	const wxString* transform;
	if (m_syntaxHandler.ShowSymbol(scopes, transform)) {
		const SymbolRef sr = {0, m_doc.GetLength(), transform};
		symbols.push_back(sr);
	}
	else {
		// Go into subscopes
		GetSubSymbols(0, m_topMatches, scopes, symbols);
	}
}

void Styler_Syntax::GetSubSymbols(unsigned int offset, const submatch& sm, deque<const wxString*>& scopes, vector<SymbolRef>& symbols) const {
	for (auto_vector<stxmatch>::const_iterator p = sm.matches.begin(); p != sm.matches.end(); ++p) {
		const stxmatch& m = *(*p);

		if (!m.m_name.empty()) {
			// Add new scope
			scopes.push_back( &m.m_name );

			// Check for matching symbol
			const wxString* transform;
			if (m_syntaxHandler.ShowSymbol(scopes, transform)) {
				const SymbolRef sr = {offset+m.start, offset+m.end, transform};
				symbols.push_back(sr);

				scopes.pop_back();
				continue;
			}
		}

		if (m.subMatch.get()) {
			// Go into subscopes
			GetSubSymbols(offset + m.start, *m.subMatch, scopes, symbols);
		}

		if (!m.m_name.empty()) {
			// Remove current scope
			scopes.pop_back();
		}
	}
}

void Styler_Syntax::Style(StyleRun& sr) {
	if (!HaveActiveSyntax()) return;

	unsigned int sr_end = sr.GetRunEnd();

	// Check if we need to do a new search
	if (sr_end > m_syntax_end) {
		// Make sure the extended position is valid and extends
		// from start-of-line to end-of-line
		unsigned int sr_start;
		cxLOCKDOC_READ(m_doc)
			sr_start = doc.GetLineStart(m_syntax_end);
		cxENDLOCK

		// Extend stylerun to get better search results (round up to whole EXTSIZEs)
		const unsigned int ext = ((sr_end / EXTSIZE) + 1) * EXTSIZE;
		sr_end =  ext < m_lines.GetLength() ? ext : m_lines.GetLength();
		sr_end = m_lines.GetLineEndFromPos(sr_end);

		DoSearch(sr_start, sr_end, sr_end);
	}

	// Apply base style
	if (m_topStyle) {
		const unsigned int start =  sr.GetRunStart();
		const unsigned int end = sr.GetRunEnd();
		if (m_topStyle->foregroundcolor != wxNullColour) sr.SetForegroundColor(start, end, m_topStyle->foregroundcolor);
		if (m_topStyle->backgroundcolor != wxNullColour) {
			sr.SetBackgroundColor(start, end, m_topStyle->backgroundcolor);
			sr.SetExtendBgColor(m_topStyle->backgroundcolor);
		}
		if (m_topStyle->fontflags != wxFONTFLAG_DEFAULT) sr.SetFontStyle(start, end, m_topStyle->fontflags);
	}

	// Style the run
	DoStyle(sr, 0, m_topMatches.matches);
}

void Styler_Syntax::DoStyle(StyleRun& sr, unsigned int offset, const auto_vector<stxmatch>& matches) {
	const unsigned int rstart =  sr.GetRunStart();
	const unsigned int rend = sr.GetRunEnd();
	const unsigned int styleStart = offset < rstart ? rstart - offset : 0;
	const stxmatch m(wxEmptyString, NULL, styleStart, 0, 0, 0, NULL);

	if (matches.empty()) return;

	auto_vector<stxmatch>::const_iterator p = lower_bound(matches.begin(), matches.end(), &m, stxmatch_start_less());
	if (p != matches.begin()) --p;

	for (; p != matches.end(); ++p) {
		const unsigned int mStart = (*p)->start+offset;
		const unsigned int mEnd   = (*p)->end+offset;

		if (mStart > rend) break;

		// Check for overlap
		if (mEnd > rstart && mStart < rend) {
			const unsigned int start = wxMax(rstart, mStart);
			const unsigned int end = wxMin(rend, mEnd);

			const style* st = (*p)->st;
			if (st) {
				if (st->foregroundcolor != wxNullColour) sr.SetForegroundColor(start, end, st->foregroundcolor);
				if (st->backgroundcolor != wxNullColour) {
					sr.SetBackgroundColor(start, end, st->backgroundcolor);
					if (mEnd > rend) sr.SetExtendBgColor(st->backgroundcolor);
				}
				if (st->fontflags != wxFONTFLAG_DEFAULT) sr.SetFontStyle(start, end, st->fontflags);
			}

			// Check if there are submatches
			if ((*p)->subMatch.get()) {
				 DoStyle(sr, (*p)->start+offset, (*p)->subMatch->matches);
			}
		}
	}
}

void Styler_Syntax::DoSearch(unsigned int start, unsigned int end, unsigned int limit) {
	wxASSERT(0 <= start && start < m_doc.GetLength());
	wxASSERT(start < end && end <= m_doc.GetLength());
	wxASSERT(limit <= m_doc.GetLength());
	wxASSERT(HaveActiveSyntax());
	
	// Don't try to parse if there is no valid parser
	if (!HaveActiveSyntax()) {
		m_syntax_end = m_lines.GetLength();
		return;
	}

	wxLogDebug(wxT("DoSearch %u-%u %u"), start, end, limit);

	// Make sure we don't get gaps in the parsing
	if (start > m_syntax_end) start = m_syntax_end;

	// Initialize SearchInfo
	SearchInfo si;
	si.pos = start;
	si.line_id = m_lines.GetLineFromCharPos(start);
	si.lineStart = m_lines.GetLineStartpos(si.line_id);
	si.lineEnd = m_lines.GetLineEndpos(si.line_id, false);
	cxLOCKDOC_READ(m_doc)
		doc.GetTextPart(si.lineStart, si.lineEnd, si.line);
	cxENDLOCK
	si.lineLen = si.lineEnd - si.lineStart;
	si.changeEnd = end;
	si.limit = limit;
	si.hitLimit = false;
	si.done = false;

	//wxLogDebug(wxT("  si %u-%u-%u,%u"), si.pos,si.line_id, si.lineStart, si.lineEnd);

	// Do the search
	m_syntax_end = Search(m_topMatches, si, 0, m_syntax_end, NULL);

#ifdef __WXDEBUG__
	Verify();
#endif  //__WXDEBUG__
}

unsigned int Styler_Syntax::Search(submatch& submatches, SearchInfo& si, unsigned int scopeStart, unsigned int scopeEnd, stxmatch* scope) {
	const unsigned int adjPos = si.pos - scopeStart;
	//const unsigned int adjEnd = si.changeEnd - scopeStart;

	// Find the first match after or containing start
	stxmatch m(wxEmptyString, NULL, 0, adjPos, 0, 0, NULL);
	auto_vector<stxmatch>& matches = submatches.matches;
	auto_vector<stxmatch>::iterator next_match = lower_bound(matches.begin(), matches.end(), &m, stxmatch_end_less());

	// Check if we are the end scope
	// (if we contain changeEnd and has no submatches containing it)
	bool isEndScope = false;
	if (scopeStart < si.changeEnd && scopeEnd > si.changeEnd) {
		m.end = si.changeEnd - scopeStart;
		auto_vector<stxmatch>::iterator end_match = lower_bound(next_match, matches.end(), &m, stxmatch_end_less());

		// We are only the endscope if changeEnd is free and there is no open span
		// bordering up to it.
		if (end_match == matches.end() || (*end_match)->start >= m.end ||
			((*end_match)->end == m.end && (!(*end_match)->subMatch.get() || (*end_match)->subMatch->flags & cxSPAN_IS_CLOSED))) {
			isEndScope = true;
		}
	}

	// Check if we should enter and search inside match
	if (next_match != matches.end()) {
		// In spans with contentName, we have to enter the content
		if (scope && scope->subMatch->flags & cxSPAN_HAS_CONTENT) {
			if (scope->subMatch->flags & cxSPAN_HAS_STARTER && next_match == matches.begin() && (*next_match)->end == adjPos) {
				++next_match;
				wxASSERT(next_match != matches.end()); // content span has been deleted
			}

			// Verify that we are in starter, middle or ender
			wxASSERT(next_match == matches.begin() || (*next_match)->subMatch->flags & cxSPAN_IS_CONTENT || matches.size() == 3); // in ender
		}

		stxmatch* m = *next_match;
		const bool isSpan = m->subMatch.get() && m->subMatch->subMatcher;

		// Pos will always point to start-of-line or end of a match, so if pos
		// is contained in a match it has to be a span which we have to enter.
		if (isSpan) {
			const bool isContentSpan = (m->subMatch->flags & cxSPAN_IS_CONTENT) != 0;

			if (m->start < adjPos || (isContentSpan && m->start == adjPos)) {
				const bool spanClosed = m->subMatch->flags & cxSPAN_IS_CLOSED;

				if (spanClosed && m->end == adjPos) {
					// Don't enter closed matches if we are at end-of-match
					++next_match;
				}
				else {
					const unsigned int subScopeStart = scopeStart + m->start;

					// Re-init span (content spans have been reinited by parent)
					if (!(m->subMatch->flags & cxSPAN_IS_CONTENT)) {
						ReInitSpan(*((span_matcher*)m->subMatch->subMatcher), subScopeStart, si);
					}

					// Search inside span
					m->end = Search(*m->subMatch, si, subScopeStart, scopeStart + m->end, m) - scopeStart;

					// Delete any following matches we might have overwritten
					auto_vector<stxmatch>::iterator p = next_match+1;
					while ( p != matches.end() && m->end > (*p)->start) {
						p = matches.erase(p); // remove overlapped match
					}

					next_match = p;
				}
			}
			else if (m->end == adjPos) ++next_match;
		}
		else if (m->end == adjPos) ++next_match;
	}

	// Search related vars
	static const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	int zeromatch = -1; // avoid looping on zero-length matches

	// Get matcher
	matcher& subMatcher = *submatches.subMatcher;
	if (!subMatcher.IsInitialized()) subMatcher.Init();

	// Search from start until we hit a previous match (or end)
	for(;;) {
		// Check if we can end search
		// 1. At end of last changed line and still in same scope.
		// 2. Hit limit
		if (si.done) return wxMax(scopeEnd, si.pos);
		else {
			// The syntax highlighting may have changed the height of
			// the line, so if we are done with line, update it.
			/*if (m_updateLineHeight && si.pos == si.lineEnd) {
				if (m_syntax_end < si.lineEnd) m_syntax_end = si.lineEnd; // avoid search loop
				m_lines.UpdateParsedLine(si.line_id);
			}*/

			// Check if we can end this search
			if (!si.hitLimit && isEndScope && (si.pos == si.changeEnd)) {
				si.done = true;
				return wxMax(scopeEnd, si.pos);
			}
			else if (si.pos >= si.limit) {
				// If we hit limit before closing a span we have to keep it open
				// and remove all following matches
				si.hitLimit = true;
				submatches.flags &= ~cxSPAN_IS_CLOSED;
				if (next_match != matches.end()) matches.erase(next_match, matches.end());
				return si.limit;
			}
			else if (si.pos == si.lineEnd) {
				// Advance to next line
				++si.line_id;
				si.lineStart = si.lineEnd;
				si.lineEnd = m_lines.GetLineEndpos(si.line_id, false);
				cxLOCKDOC_READ(m_doc)
					doc.GetTextPart(si.lineStart, si.lineEnd, si.line);
				cxENDLOCK
				si.lineLen = si.lineEnd - si.lineStart;
				zeromatch = -1;
			}
		}

		// Do the search
		const unsigned int offset = si.pos - si.lineStart;
		unsigned int callout_id;
		const int rc = subMatcher.Match(&*si.line.begin(), offset, si.lineLen, callout_id, ovector, OVECCOUNT, zeromatch);
		zeromatch = -1;

		if (rc < 0) {
			// Remove any old matches between pos and end-of-line
			while (next_match != matches.end() && scopeStart + (*next_match)->start < si.lineEnd) {
				next_match = matches.erase(next_match);
			}

			if (rc == PCRE_ERROR_NULL) {
				// Invalid pattern
				si.done = true;
				return si.limit;
			}
			else {
				// Go to end-of-line
				si.pos = si.lineEnd;
				continue;
			}
		}
		else {
			// Get match interval
			const unsigned int callout_start = si.lineStart + ovector[0]; 
			const unsigned int callout_end = si.lineStart + ovector[1];

			// Move pos to end of match
			si.pos = callout_end;

			// Only span start & end is allowed to be an empty match
			// otherwise we have to move one char ahead to avoid eternal loop
			/*if (callout_start == callout_end && !(subMatcher.IsSpanStart(callout_id) || subMatcher.IsSpanEnd(callout_id))) {
				wxASSERT(false);
				++si.pos;
				continue;
			}*/

			const bool isSpanStart = subMatcher.IsSpanStart(callout_id);
			matcher& m = subMatcher.GetCallout(callout_id);
			const unsigned int matchStart = callout_start - scopeStart;
			const unsigned int matchEnd =  si.pos - scopeStart;

			// Check if we have start overlapping with following match
			if (next_match != matches.end() && callout_start == scopeStart + (*next_match)->start) {
				// If we are both spans and the starters seem equivalent
				// we just enter and continue
				stxmatch& sm = *(*next_match);
				if (isSpanStart && sm.m_matcher == &m) {
					auto_vector<stxmatch>& submatches = sm.subMatch->matches;
					if (!submatches.empty()) {
						const stxmatch* starter = submatches[0];
						if (starter->start == 0 && starter->end == matchEnd-matchStart) {

							ReInitSpan(*((span_matcher*)sm.subMatch->subMatcher), callout_start, si);
							sm.end = Search(*sm.subMatch, si, callout_start, scopeStart + sm.end, *next_match) - scopeStart;
							++next_match;

							// Check if we have overwritten following matches
							while (next_match != matches.end() && si.pos > scopeStart + (*next_match)->start) {
								next_match = matches.erase(next_match);
							}
							continue;
						}
					}
				}
			}

			const bool isSpanEnd = subMatcher.IsSpanEnd(callout_id);

			if (callout_start != callout_end || isSpanStart) {
				if (isSpanEnd && submatches.flags & cxSPAN_IS_CONTENT) {
					// If we are in a content span, the ender belongs to the parent
					// So we ignore it and let the parent find it.
					si.pos = callout_start;
				}
				else {
					// Create the new match
					auto_ptr<stxmatch> iv(new stxmatch(m.GetName(), &m, matchStart, matchEnd, NULL, NULL, scope));

					// Style It
					iv->st = GetStyle(*iv);

					// Check if the match is span or has any captures
					if (isSpanStart) {
						CreateSpan(callout_start, callout_end, subMatcher, callout_id, si, iv.get(), rc, ovector);
						iv->end = si.pos - scopeStart;

						// Avoid eternal loop with zero-length spans
						if (iv->start == iv->end) zeromatch = callout_id;
					}
					else if (m.HasCaptures()) {
						AddCaptures(m, *iv, scopeStart, si, rc, ovector);
					}

					// Add new match to list
					next_match = matches.insert(next_match, iv);
					++next_match;

					if (isSpanEnd) submatches.flags |= cxSPAN_HAS_ENDER;
				}
			}

			// Check if we can close the span
			if (isSpanEnd) {
				// Close span
				submatches.flags |= cxSPAN_IS_CLOSED;

				// Remove any leftover matches
				if (next_match != matches.end()) {
					matches.erase(next_match, matches.end());
				}

				return si.pos;
			}
			else {
				// Check if we have overwritten following matches
				while (next_match != matches.end() && si.pos > scopeStart + (*next_match)->start) {
					next_match = matches.erase(next_match);
				}
			}

			// Avoid eternal loop with faulty regexs
			if (callout_start == callout_end && callout_end == si.pos) zeromatch = callout_id;
		}
	}
}

void Styler_Syntax::AddCaptures(matcher& m, stxmatch& sm, unsigned int offset, const SearchInfo& si, int rc, int* ovector) {
	wxASSERT(m.HasCaptures());
	wxASSERT(sm.subMatch.get() == NULL);
	wxASSERT(offset + sm.start >= si.lineStart && offset + sm.start < si.lineEnd);

	// Handle captures inside eachother
	if (rc > 0) {
		vector<unsigned int> offsets;
		vector<interval> ivs;
		vector<stxmatch*> mts;

		// All intervals are in absolute numbers
		const interval iv(offset + sm.start, offset + sm.end);
		wxASSERT(iv.start == si.lineStart + ovector[0] && iv.end == si.lineStart + ovector[1]);

		ivs.push_back(iv);
		mts.push_back(&sm);

		for (unsigned int i = 1; (int)i < rc; ++i) {
			if (ovector[2*i] == -1) continue;

			const wxString& name = m.GetCaptureName(i);
			if (name.empty()) continue;

			const interval capiv(si.lineStart + ovector[2*i], si.lineStart + ovector[2*i+1]);

			// Get the right parent match
			while(ivs.size() > 1 && capiv.end > ivs.back().end) {
				ivs.pop_back();
				mts.pop_back();
			}
			stxmatch& parent = *mts.back();

			// We have to adjust the interval against the parent offset (which is absolute)
			const int cap_start = capiv.start - ivs.back().start;
			const int cap_end = capiv.end - ivs.back().start;

			// Captures outside match (like in a non-capturing part)
			// are not currently supported
			const int parentLen = ivs.back().end - ivs.back().start;
			if (cap_start < 0 || cap_end > parentLen) {
				continue;
			}

			// Create submatch list if not there
			if (!parent.subMatch.get()) {
				parent.subMatch = auto_ptr<submatch>(new submatch);
				parent.subMatch->subMatcher = NULL; // matches with captures distinguishes from spans by not having subMatcher
			}

			// Create the new match
			auto_ptr<stxmatch> cap(new stxmatch(name, &m, cap_start, cap_end, NULL, NULL, &parent));

			wxASSERT(capiv.end <= offset + sm.end);

			ivs.push_back(capiv);
			mts.push_back(cap.get());

			cap->st = GetStyle(*cap); // style the match
			parent.subMatch->matches.push_back(cap);
		}
	}
}

void Styler_Syntax::CreateSpan(unsigned int starterStart, unsigned int starterEnd, matcher& subMatcher, unsigned int id, SearchInfo& si, stxmatch* scope, int rc, int* ovector) {
	wxASSERT(scope->subMatch.get() == NULL); // Newly created span should not yet have submatches

	// Create submatches for the new span
	scope->subMatch = auto_ptr<submatch>(new submatch);
	submatch& span_sub = *scope->subMatch;
	span_sub.subMatcher = &subMatcher.GetCallout(id);
	wxASSERT(span_sub.subMatcher && span_sub.subMatcher->IsSpan());


	// Check if the span ender contains refs which need to be updated
	span_matcher* sm = (span_matcher*)span_sub.subMatcher;
	if (!sm->IsInitialized()) sm->Init();
	ReInitSpan(*sm, starterStart, si);

	// Add the span-starter match
	if (starterEnd > starterStart) {
		const unsigned int span_id = subMatcher.GetSubId(id);
		matcher* const spanstarter = sm->GetStartMember(span_id);

		auto_ptr<stxmatch> spanstart(new stxmatch(spanstarter->GetName(), spanstarter, 0, starterEnd - starterStart, NULL, NULL, scope));
		spanstart->st = GetStyle(*spanstart); // style the match

		// Check if the match has any captures
		if (spanstarter->HasCaptures()) {
			AddCaptures(*spanstarter, *spanstart, starterStart, si, rc, ovector);
		}

		span_sub.flags |= cxSPAN_HAS_STARTER;
		span_sub.matches.push_back(spanstart);
	}

	unsigned int spanEnd = starterEnd;

	// Check if we should apply a contentName
	const wxString& contentName = span_sub.subMatcher->GetContentName();
	if (!contentName.empty()) {
		const unsigned int contentStart = span_sub.matches.empty() ? 0 : span_sub.matches[0]->end;

		// Create content span
		auto_ptr<stxmatch> contentIv(new stxmatch(contentName, NULL, contentStart, contentStart, NULL, NULL, scope));
		contentIv->subMatch = auto_ptr<submatch>(new submatch);
		contentIv->subMatch->subMatcher = span_sub.subMatcher; // same as parent
		contentIv->subMatch->flags |= cxSPAN_IS_CONTENT;
		contentIv->st = GetStyle(*contentIv);

		// Search inside content span
		si.pos = Search(*contentIv->subMatch, si, starterEnd, starterEnd, contentIv.get());
		contentIv->end = si.pos - starterStart;

		// Update parent
		span_sub.flags |= cxSPAN_HAS_CONTENT;
		span_sub.matches.push_back(contentIv);

		if (si.hitLimit) return;

		spanEnd = si.pos;
	}

	// Search inside span
	// (newly created span can never be endScope)
	// if we have a content span, this search starts from it's end to find ender
	si.pos = Search(span_sub, si, starterStart, spanEnd, scope);

	// scope->end has to be set in calling functions (to adj for scope start)
}

void Styler_Syntax::ReInitSpan(span_matcher& sm, unsigned int start, const SearchInfo& si, int rc, int* ovector) {
	if (!sm.HasEndCaptures()) return;

	match_matcher& spanstarter = *sm.GetStartMatcher();

	unsigned int lineStart;
	unsigned int lineEnd;
	unsigned int lineLen;
	vector<char> line; // TODO: make member variable
	const char* ptrLine = NULL;
	bool usingSi;

	// Check if we can reuse current line info
	if (start >= si.lineStart && start < si.lineEnd) {
		usingSi = true;
		lineStart = si.lineStart;
		lineEnd = si.lineEnd;
		lineLen = si.lineLen;
		ptrLine = &*si.line.begin();
	}
	else {
		usingSi = false;
		lineStart = start; // TODO: set to start-of-line
		cxLOCKDOC_READ(m_doc)
			lineEnd = doc.GetLine(lineStart, line);
		cxENDLOCK
		lineLen = lineEnd - lineStart;
		ptrLine = &*line.begin();
	}

	if (!ovector) {
		// Re-search for the starter to get captures
		// (only needed if we are not in a current search)
		pcre* subRe = spanstarter.GetMatchPattern();
		const int OVECCOUNT = 30;
		int ov[OVECCOUNT];
		ovector = ov;
		const int search_options = PCRE_NO_UTF8_CHECK;
		rc = pcre_exec(
			subRe,                // the compiled pattern
			NULL,                 // extra data - if we study the pattern
			ptrLine,              // the subject string
			lineLen,              // the length of the subject
			start - lineStart,    // start at offset in the subject
			search_options,       // options
			ovector,              // output vector for substring information
			OVECCOUNT);           // number of elements in the output vector
	}

	// ReInit span_matcher to get an updated ender
	if (rc > 0) {
		const vector<char>& lineref = (usingSi ? si.line : line);
		sm.ReInit(lineref, ovector, rc);
	}
}

void Styler_Syntax::Insert(unsigned int pos, unsigned int length) {
#ifdef __WXDEBUG__
	Verify();

	const unsigned int docLen = m_doc.GetLength();
	wxASSERT(pos >= 0 && pos < docLen);
	wxASSERT(length >= 0 && pos+length <= docLen);
#endif

	// Adjust end
	if (m_syntax_end > pos)	m_syntax_end += length;
	//else return; // Change outside search area

	unsigned int change_start;
	cxLOCKDOC_READ(m_doc)
		change_start = doc.GetLineStart(pos);
	cxENDLOCK

	const unsigned int adj_end = AdjustForInsertion(pos, length, m_topMatches, 0, change_start);

	unsigned int change_end = wxMax(adj_end, pos + length);
	if (!m_lines.IsLineEnd(change_end) || change_end == change_start) {
		change_end = m_lines.GetLineEndFromPos(change_end);
	}

	// After changes we want the lines to be updated with the new height
	m_updateLineHeight = true;

	// In case the change invalidates the whole rest of the doc
	// we don't want to parse it all. There can also come other
	// changes before next redraw, so we set the limit to change_end.
	DoSearch(change_start, change_end, change_end);
	m_updateLineHeight = false;
}

void Styler_Syntax::Delete(unsigned int start_pos, unsigned int end_pos) {
	const unsigned int docLen = m_doc.GetLength();
	wxASSERT(start_pos >= 0 && start_pos <= docLen);

	if (start_pos == end_pos) return;
	wxASSERT(end_pos > start_pos);

	// Check if we have deleted the entire document
	if (docLen == 0) {
		Invalidate();
		return;
	}

	// Adjust end
	unsigned int length = end_pos - start_pos;
	if (m_syntax_end > start_pos) {
		if (m_syntax_end > end_pos) m_syntax_end -= length;
		else m_syntax_end = start_pos;
	}
	else return; // Change after search area, no need to re-search

	unsigned int change_start;
	cxLOCKDOC_READ(m_doc)
		change_start = doc.GetLineStart(start_pos);
	cxENDLOCK

	unsigned int change_end = AdjustForDeletion(start_pos, end_pos, m_topMatches, 0, change_start);

	if (!m_lines.IsLineEnd(change_end) || change_end == change_start) {
		change_end = m_lines.GetLineEndFromPos(change_end);
	}

	// Do a new search starting from end of first match before pos
	if (change_start != m_syntax_end) {
		wxASSERT(change_start < m_syntax_end);

		// After changes we want the lines to be updated with the new height
		m_updateLineHeight = true;

		// In case the change invalidates the whole rest of the doc
		// we don't want to parse it all. There can also come other
		// changes before next redraw, so we set the limit to change_end.
		DoSearch(change_start, change_end, change_end);
		m_updateLineHeight = false;
	}
}

void Styler_Syntax::ApplyDiff(const vector<cxLineChange>& linechanges) {
	if (m_lines.GetLength() == 0) {
		Invalidate();
		return;
	}

	m_updateLineHeight = true;
#ifdef __WXDEBUG__
	// Syntax after change is not valid during application of diff
	m_verifyEnabled = false;
#endif

	for (vector<cxLineChange>::const_iterator l = linechanges.begin(); l != linechanges.end(); ++l) {
		// Adjust syntax end
		if (m_syntax_end > l->start) {
			if (m_syntax_end > l->end) m_syntax_end += l->diff;
			else m_syntax_end = l->start;
		}
		else goto cleanup_and_return; // Change after search area, no need to re-search

		const unsigned int old_line_end = l->end - l->diff;
		const unsigned int new_length = l->end - l->start;
		unsigned int change_end = l->end;

		// Adjust matches
		if (l->start != old_line_end) change_end = wxMax(AdjustForDeletion(l->start, old_line_end, m_topMatches, 0, l->start), change_end);;
		if (new_length) change_end = wxMax(AdjustForInsertion(l->start, new_length, m_topMatches, 0, l->start), change_end);

		// limit parsing upto next changed line
		unsigned int limit;
		vector<cxLineChange>::const_iterator nextchange = l+1;
		if (nextchange != linechanges.end()) limit = nextchange->start;
		else limit = m_lines.GetLineEndFromPos(change_end);
		
		if (l->start < limit) DoSearch(l->start, l->end, limit); 	
	}

cleanup_and_return:
	m_updateLineHeight = false;

#ifdef __WXDEBUG__
	m_verifyEnabled = true;
	Verify();
#endif
}

unsigned int Styler_Syntax::AdjustForInsertion(unsigned int pos, unsigned int length, submatch& submatches, unsigned int o, unsigned int lineStart) {
	unsigned int change_end = pos + length;
	auto_vector<stxmatch>& matches = submatches.matches;
	if (matches.empty()) return change_end;

	// Find first match containing or bigger than pos
	const stxmatch target(wxEmptyString, NULL, 0, pos, NULL, NULL, NULL);
	auto_vector<stxmatch>::iterator p = lower_bound(matches.begin(), matches.end(), &target, stxmatch_end_less());
	if (p == matches.end()) return change_end;

	// Check if pos is inside match
	stxmatch& m = *(*p);
	wxLogDebug(wxT("m %d %d"), m.start, m.end);

	if (m.start < pos && m.end > pos) {
		// Check if match is a span and we can move inside
		if (m.subMatch.get() && m.subMatch->subMatcher) {
			submatch& sm = *m.subMatch;
			const unsigned int offset = m.start;

			// Check if we are in first match (start delimiter).
			// If first match extends from before lineStart then we know
			// that it is not the start delimiter (match must have zero
			// length start delimiter)
			if (sm.flags & cxSPAN_HAS_STARTER && pos <= (offset + sm.matches[0]->end)) {
				// If so we have to remove the span
				change_end = m.end + length;
				p = matches.erase(p);
			}
			else {
				change_end = offset + AdjustForInsertion(pos - offset, length, *m.subMatch, o + offset, lineStart);

				// Adjust match to new size
				// This is the only case where the match is not deleted and thus need adjusting
				m.end += length;

				++p; // Go to next match
			}
		}
		else {
			// Else Delete and continue
			change_end = m.end + length;
			p = matches.erase(p);

			// If we have deleted the last match in a span we have to re-open it
			if (p == matches.end()) submatches.flags &= ~cxSPAN_IS_CLOSED;
		}
	}
	else if (m.end == pos) ++p;

	// Move all following matches to correct position
	while (p != matches.end()) {
		(*p)->start += length;
		(*p)->end += length;
		++p;
	}

#ifdef __WXDEBUG__
	if (change_end > m_lines.GetLength()) {
		Print();
		wxASSERT(false);
	}
#endif

	return change_end;
}

unsigned int Styler_Syntax::AdjustForDeletion(unsigned int start, unsigned int end, submatch& submatches, unsigned int o, unsigned int lineStart) {
	auto_vector<stxmatch>& matches = submatches.matches;
	if (matches.empty()) return start;

	// Find first match containing or bigger than start
	const stxmatch target(wxEmptyString, NULL, 0, start, NULL, NULL, NULL);
	auto_vector<stxmatch>::iterator p = lower_bound(matches.begin(), matches.end(), &target, stxmatch_end_less());
	if (p == matches.end()) return start; // search from end of last match

	// Ignore matches bordering start
	if ((*p)->end == start) {
		if (++p == matches.end()) return start;
	}

	// Check if pos is inside match
	unsigned int change_end = start;
	do {
		stxmatch& m = *(*p);
		if (m.start >= end) break;

		const bool isContained = (start <= m.start && m.end <= end);

		// If match not fully contained and is a span we can move inside
		// But we will only go in if the first part of the span still exists
		if (!isContained && m.subMatch.get() && m.subMatch->subMatcher && m.start <= start) {
			submatch& sm = *m.subMatch;
			const unsigned int offset = m.start;

			// Check if we are in first match (start delimiter).
			// If first match extends from before lineStart then we know
			// that it is not the start delimiter (match must have zero
			// length start delimiter)
			if (sm.flags & cxSPAN_HAS_STARTER && start <= (offset + sm.matches[0]->end)) {
				// If so we have to remove the span
				if (m.end > end) change_end = m.end - (end - start);
				p = matches.erase(p);
			}
			else {
				// We have to enter the span and continue search inside
				const unsigned int seg_start = offset > start ? 0 : start - offset;
				const unsigned int seg_end = m.end < end ? m.end - offset : end - offset;
				change_end = offset + AdjustForDeletion(seg_start, seg_end, *m.subMatch, o + offset, lineStart);

				// Adjust match to new position and size
				// This is the only case where the match is not deleted and thus need adjusting
				if (m.start > start) {
					m.end -= m.start - start;
					m.start = start;
				}
				m.end -= seg_end - seg_start;

				++p; // Go to next match
			}
		}
		else {
			// Else Delete and continue
			if (m.end > end) change_end = m.end - (end - start);
			p = matches.erase(p);

			// If we have deleted the last match in a span we have to re-open it
			if (p == matches.end()) submatches.flags &= ~cxSPAN_IS_CLOSED;
		}
	} while (p != matches.end());

	// Move all following matches to correct position
	const unsigned int length = end - start;
	while (p != matches.end()) {
		(*p)->start -= length;
		(*p)->end -= length;
		++p;
	}

	return change_end;
}

void Styler_Syntax::ParseAll() {
	const unsigned int len = m_doc.GetLength();
	if (m_syntax_end < len) {
		DoSearch(m_syntax_end, len, len);
	}
}

void Styler_Syntax::ParseTo(unsigned int pos) {
	wxASSERT(pos <= m_doc.GetLength());

	if (m_syntax_end < pos) {
		const unsigned int sr_end = m_lines.GetLineEndFromPos(pos); // always parse to end-of-line
		DoSearch(m_syntax_end, sr_end, sr_end);
	}
}

bool Styler_Syntax::OnIdle() {
	if (!HaveActiveSyntax()) return false;

	// Extend syntax a bit longer
	if (m_syntax_end < m_doc.GetLength()) {
		// Make sure the extended position is valid and extends to end-of-line
		unsigned int ext = wxMin(m_syntax_end+EXTSIZE, m_doc.GetLength());
		cxLOCKDOC_READ(m_doc)
			ext = doc.GetLineEnd(ext);
		cxENDLOCK

		DoSearch(m_syntax_end, ext, ext);
	}

	return m_syntax_end != m_doc.GetLength(); // true if we want more idle events
}


#ifdef __WXDEBUG__
void Styler_Syntax::Print() const {
	//const vector<stxmatch*>& matches = m_topMatches.matches;
	/*wxFFile file(wxT("matches.debug"), wxT("w"));

	file.Write(wxString::Format(wxT("matches: %d\n"), matches.size()));
	for (unsigned int i = 0; i < matches.size(); ++i) {
		stxmatch& m = *matches[i];
		file.Write(wxString::Format(wxT("%d: %d-%d %s\n"), i, m.start, m.end, m.st->name));
	}
	file.Write(wxString::Format(wxT(" end: %d\n"), i));*/

	wxLogDebug(wxT("%d-%d %s"), 0, m_syntax_end, m_topMatches.subMatcher->GetName().c_str());
	PrintMatches(1, m_topMatches);
}

void Styler_Syntax::PrintMatches(unsigned int level, const submatch& submatches) const{
	const auto_vector<stxmatch>& matches = submatches.matches;

	for (unsigned int i = 0; i < matches.size(); ++i) {
		const stxmatch& m = *matches[i];
		const wxString indent(wxT(' '), level*2);

		wxLogDebug(wxT("%s%d: %d-%d %s %d"),indent.c_str(), i, m.start, m.end, m.m_name.c_str(), m.subMatch.get() != NULL);

		if (m.subMatch.get()) {
			PrintMatches(level+1, *m.subMatch);
		}
	}
}


void Styler_Syntax::Verify() const {
	if (!m_verifyEnabled) return;

	const auto_vector<stxmatch>& matches = m_topMatches.matches;

	// Check that matches are in sequence
	for (unsigned int i = 1; i < matches.size(); ++i) {
		const stxmatch& m1 = *matches[i-1];
		const stxmatch& m2 = *matches[i];

		wxASSERT(m1.start <= m1.end);
		wxASSERT(m1.end <= m2.start);
		wxASSERT(m2.start <= m2.end);
	}

	// Check that top match is within bounds
	if (m_syntax_end > m_doc.GetLength()) {
		wxLogDebug(wxT("DocLen: %d LinesLen: %d"), m_doc.GetLength(), m_lines.GetLength());
		Print();
		wxASSERT(false);
	}
	
	// Check that last match is within bounds
	if (!matches.empty()) {
		if (matches.back()->end > m_doc.GetLength()) {
			Print();
			wxASSERT(false);
		}
	}

	// Check all subMatches
	for (unsigned int i2 = 0; i2 < matches.size(); ++i2) {
		VerifyMatch(matches[i2]);
	}
}

void Styler_Syntax::VerifyMatch(const stxmatch* m) const {
	if (!m->subMatch.get()) return;
	const auto_vector<stxmatch>& matches = m->subMatch->matches;

	// Check that matches are in sequence
	for (unsigned int i = 1; i < matches.size(); ++i) {
		const stxmatch& m1 = *matches[i-1];
		const stxmatch& m2 = *matches[i];

		wxASSERT(m1.start <= m1.end);
		wxASSERT(m1.end <= m2.start);
		wxASSERT(m2.start <= m2.end);
	}

	// Check that last match is within bonds
	if (!matches.empty()) {
		wxASSERT(matches.back()->end <= (m->end - m->start));
	}

	// Check all subMatches
	for (unsigned int i2 = 0; i2 < matches.size(); ++i2) {
		VerifyMatch(matches[i2]);
	}
}

#endif  //__WXDEBUG__

Styler_Syntax::stxmatch::stxmatch(const wxString& name, const matcher* m, unsigned int start, unsigned int end, style *st, submatch* subMatch, stxmatch* parent)
: m_name(name), m_matcher(m), start(start), end(end), st(st), subMatch(subMatch), parent(parent) {
}

Styler_Syntax::stxmatch::~stxmatch() {
/*	if (subMatch.get()) {
		static unsigned int count = subMatch->matches.size(); // debug
		static int flags = subMatch->flags; // debug
	}*/
}
