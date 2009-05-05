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

#include "matchers.h"
#include "pcre.h"
#include "Catalyst.h"
#include <wx/regex.h>

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <set>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

// Initialize statics
const wxString matcher::s_emptyString;
wxRegEx matcher::s_alternatives(wxT("(^|\\(|\\(\\?:)([[:alnum:]_]+(\\|[[:alnum:]_]+)+)($|\\))"));
wxRegEx matcher::s_tabspattern(wxT("^\\t+"));
wxRegEx matcher::s_repfromzero(wxT("{,([[:digit:]]+)}"));
wxRegEx matcher::s_backref(wxT("\\\\([[:digit:]]+)"));
wxRegEx span_matcher::s_refToCapture(wxT("\\\\([[:digit:]]+)"));

void matcher::OptimizeRegex(wxString& pattern) {
	if (!s_alternatives.Matches(pattern)) return;

	size_t start, len;
	s_alternatives.GetMatch(&start, &len, 2);
	size_t end = start+len;

	// Build the tree
	NodeMap* root = new NodeMap;
	NodeMap* node = root;
	NodeMap* prevNode = NULL;
	wxChar prevChar = '\0';
	for (size_t i = start; i < end; ++i) {
		const wxChar c = pattern[i];

		if (!node) {
			node = new map<wxChar, void*>;
			(*prevNode)[prevChar] = node;
		}
		prevNode = node;
		prevChar = c;
		node = (NodeMap*)(*node)[c];

		if (c == wxT('|')) node = root;
	}

	// Rebuild the pattern from tree
	wxString newpattern;
	RebuildPattern(newpattern, *root);

	pattern.replace(start, len, newpattern);
	//wxLogDebug(wxT("%s"), pattern);

	// Clean up
	DeleteNode(root);
}

void matcher::DeleteNode(NodeMap* node) {
	// Delete all subnodes
	for (NodeMap::iterator p = node->begin(); p != node->end(); ++p) {
		if (p->second) DeleteNode((NodeMap*)p->second);
	}

	delete node;
}

void matcher::RebuildPattern(wxString& pattern, const NodeMap& node) {
	wxASSERT(!node.empty());

	if (node.size() == 1) {
		if ((*node.begin()).first == wxT('|')) return; // ignore enders

		pattern.append((*node.begin()).first);
		if ((*node.begin()).second) {
			RebuildPattern(pattern, *(NodeMap*)(*node.begin()).second);
		}

	}
	else {
		const bool isFirst = pattern.empty();

		if (!isFirst) pattern.append(wxT("(?:"));

		bool ender = false;
		for (NodeMap::const_iterator p = node.begin(); p != node.end(); ++p) {
			if (p->first == wxT('|')) ender = true;
			else {
				if (p != node.begin()) pattern.append(wxT('|'));
				pattern.append(p->first);
				if (p->second) {
					RebuildPattern(pattern, *(NodeMap*)p->second);
				}
			}
		}

		if (!isFirst) {
			pattern.append(wxT(')'));
			if (ender) pattern.append(wxT('?'));
		}
	}
}

void matcher::RegExConvert(wxString& pattern) {
	// Remove leading tabs (left by TinyXML)
	s_tabspattern.ReplaceAll(&pattern, wxEmptyString);

	// Optimize the pattern
	OptimizeRegex(pattern);

	// Convert Oniguruma syntax to PCRE
	//s_namedpattern.ReplaceAll(&pattern, wxT("(?P<\\1"));
	//s_numbackref.ReplaceAll(&pattern, wxT("(?\\1)"));
	//s_namedbackref.ReplaceAll(&pattern, wxT("(?P>\\1)"));
	s_repfromzero.ReplaceAll(&pattern, wxT("{0,\\1}"));

	static wxRegEx beforenewline(wxT("\\\\Z\\(\\?!\\\\n\\)"));
	beforenewline.ReplaceAll(&pattern, wxT("\\z(?<!\\n)"));
}

#ifdef __WXDEBUG__
bool matcher::RegExVerify(const wxString& pattern, bool matchcase) {
	const char *error;
	int erroffset;

	int options = PCRE_UTF8; // We need multiline until we change to line basis
	if (!matchcase) options |= PCRE_CASELESS;

	// Compile the pattern
	pcre* compiledPattern = pcre_compile(
			pattern.mb_str(wxConvUTF8),   /* the pattern */
			options,              /* options */
			&error,               /* for error message */
			&erroffset,           /* for error offset */
			NULL);                /* use default character tables */

	if (compiledPattern) {
		free(compiledPattern);
		return true;
	}

	wxLogDebug(wxT("RegEx error: %s"), error);
	wxLogDebug(wxT("offset: %d\n%s"), erroffset, &pattern[erroffset]);

	wxFile file(wxT("pattern.debug"), wxFile::write);
	file.Write(pattern);
	file.Close();

	wxASSERT(false);
	return false;
}
#endif //__WXDEBUG__

matcher& match_matcher::GetCallout(unsigned int callout_id) {
	wxASSERT(callout_id == 0);
	return *this;
}

bool match_matcher::RegExCompile(const wxString& pattern, bool matchcase) {
	const char *error;
	int erroffset;

	int options = PCRE_UTF8; // We need multiline until we change to line basis
	if (!matchcase) options |= PCRE_CASELESS;

	// Compile the pattern
	m_compiledPattern = pcre_compile(
			pattern.mb_str(wxConvUTF8),   /* the pattern */
			options,              /* options */
			&error,               /* for error message */
			&erroffset,           /* for error offset */
			NULL);                /* use default character tables */

	if (m_compiledPattern) {
		m_patternStudy = pcre_study(m_compiledPattern, 0, &error);
		return true;
	}

	wxLogDebug(wxT("RegEx error: %s"), wxString(error, wxConvUTF8).c_str());
	wxLogDebug(wxT("Invalid pattern: %s"), pattern.c_str());
	wxString loc = pattern;
	loc.Remove(0, erroffset);
	wxLogDebug(wxT("Location: %s"), loc.c_str());

	// Notify user of error
	wxString msg = m_name + _(" contains invalid regex pattern!\n");
	msg += wxT("Error: ") + wxString(error, wxConvUTF8) + wxT("\n\n");
	//msg += pattern;
	wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK);
	return false;
}


// -------------------------------------------------------------------------------------

match_matcher::~match_matcher() {
	if (m_compiledPattern) free(m_compiledPattern);
	if (m_patternStudy) free(m_patternStudy);
}

void match_matcher::SetPattern(const char* pattern) {
	SetPattern(wxString(pattern, wxConvUTF8));
}

void match_matcher::SetPattern(const wxString& pattern) {
	// Clean up first (spans might reset endpatterns)
	if (m_compiledPattern) free(m_compiledPattern);
	if (m_patternStudy) free(m_patternStudy);
	m_compiledPattern = NULL;
	m_patternStudy = NULL;

	m_pattern = pattern;
	RegExConvert(m_pattern);
	wxASSERT(RegExVerify(m_pattern));

	// Convert backrefs to named refs
	/*m_convPattern = m_pattern;
	RegExNumToNamed(m_convPattern);
	if (m_convPattern == m_pattern) {
		// release unneeded memory
		wxString tmp;
		tmp.swap(m_convPattern);

		m_subs.push_back(&m_pattern);
	}
	else m_subs.push_back(&m_convPattern);*/

	m_isInitialized = true;
}

pcre* match_matcher::GetMatchPattern() {
	if (!m_compiledPattern) {
		RegExCompile(m_pattern);
	}
	return m_compiledPattern;
}

size_t match_matcher::GetMembers(vector<calloutref>& refs) {
	const calloutref cr = {this, this, 0};
	refs.push_back(cr);
	return 1;
}

void match_matcher::AddCapture(unsigned int capkey, const wxString& name) {
	wxASSERT(!name.empty());

	if (capkey == 0) {
		// If the capture cover the entire pattern we have to replace the name
		m_name = name;
	}
	else {
		m_hasCaptures = true;
		m_captures[capkey] = name;
	}
}

const wxString& match_matcher::GetCaptureName(unsigned int capkey) const {
	map<unsigned int,wxString>::const_iterator p = m_captures.find(capkey);
	if (p != m_captures.end()) return p->second;
	else return s_emptyString;
}

int match_matcher::Match(char* line, unsigned int start, unsigned int len, unsigned int& callout_id, int *ovector, int ovecsize, int WXUNUSED(zeromatch)) {
	wxASSERT(start < len);

	// Get search pattern from matcher
	const pcre* re = GetMatchPattern();
	const pcre_extra* study = GetPatternStudy();
	
	// Do the search
	const int rc = pcre_exec(
		re,                   // the compiled pattern
		study,                // extra data - if we study the pattern
		line,                 // the subject string
		len,                  // the length of the subject
		start,                // start at offset in the subject
		PCRE_NO_UTF8_CHECK,   // options
		ovector,              // output vector for substring information
		ovecsize);            // number of elements in the output vector


	// Do we have a match?
	callout_id = 0;
	return rc;
}

// -------------------------------------------------------------------------------------

bool span_matcher::Init(bool WXUNUSED(deep)) {
	if (m_initializing || m_isInitialized) return true;
	m_initializing = true; // avoid circular references

	wxASSERT(m_startMatcher);

	// If we are using a matcher as start delimiter we have to initialize it
	//if (!m_startMatcher->Init(deep)) wxASSERT(false); // init failed

	// Add the end pattern first (always ref 0)
	if (!m_endPattern.empty()) {
		RegExConvert(m_endPattern);

		// The end pattern is special in that it may contain references
		// to captures from the start matcher
		if (!s_refToCapture.Matches(m_endPattern)) {
			//m_pattern += wxT("(?:") + m_endPattern + wxT(")(?C0)");

			// the end matcher is only used for captures
			wxASSERT(m_endMatcher);
			m_endMatcher->SetPattern(m_endPattern);
		}
		else {
			// if it refs captures from starter, we have to wait for it
			m_hasEndCaptures = true;
		}
	}
	calloutref ref = {m_endMatcher, m_endMatcher, 0}; // m_endMatcher is only used for captures
	m_refs.push_back(ref);

	// Add contained members
	m_isInitialized = m_initializing = false; // will be set again by gm:Init()
	group_matcher::Init(true);

	m_isInitialized = true;
	m_initializing = false;
	return true;
}

void span_matcher::ReInit(const vector<char>& text, const int* captures, unsigned int capcount) {
	wxASSERT(m_isInitialized);

	if (!m_endMatcher) return;

	int diff = 0;
	wxString updatedPattern = m_endPattern;

	bool patternModified = false;
	const wxChar *start = m_endPattern.c_str();
	for ( const wxChar *p = start; *p; p++ )
    {
        if ( *p == _T('\\') )
        {
            if ( wxIsdigit(*++p) )
            {
                // back reference
                wxChar *end;
                const size_t index = (size_t)wxStrtoul(p, &end, 10);

				// do we have a back reference?
				if ( index != (size_t)-1 )
				{
					// adjusted with one for leading slash
					const unsigned int pos = (p-1 - start) + diff;
					const unsigned int refLen = end - p+1;

					// do we have a matching capture
					if (index < capcount && captures[2*index] != -1) {
						// get the capture
						const interval iv(captures[2*index], captures[2*index+1]);
						size_t capLen = iv.end - iv.start;
						const char* textPtr = text.empty() ? "" : &*text.begin();
						wxString rep(textPtr+iv.start, wxConvUTF8, capLen);
						capLen = rep.length();

						// escape any control chars in the replacement
						static wxRegEx ctrlChars(wxT("[^[:alnum:]]"));
						if (ctrlChars.Matches(rep)) {
							ctrlChars.ReplaceAll(&rep, wxT("\\\\\\0"));
							capLen = rep.size();
						}

						// replace the ref
						updatedPattern.replace(pos, refLen, rep);
						diff += (int)capLen - (int)refLen;
					}
					else {
						// eat the ref
						updatedPattern.erase(pos, refLen);
						diff -= refLen;
					}
					patternModified = true;
				}

				p = end - 1; // -1 to compensate for p++ in the loop
            }
            //else: backslash used as escape character
        }
    }

	// The end pattern is always first (ref 0)
	//m_pattern = wxT("(?:") + updatedPattern + wxT(")(?C0)");
	//if (!m_groupPattern.empty()) m_pattern += wxT("|") + m_groupPattern;

	// We also have to set the new pattern in the matcher used for captures
	if (patternModified) m_endMatcher->SetPattern(updatedPattern);
}

bool span_matcher::IsSpanEnd(unsigned int callout_id) {
	wxASSERT(m_isInitialized);
	wxASSERT(callout_id < m_refs.size());

	//if (callout_id >= m_endrefs_start) return true;
	//if (callout_id == m_endref) return true;
	return (callout_id == 0);
}

size_t span_matcher::GetMembers(vector<calloutref>& refs) {
	// We do not need to init span yet, as parent only need starter
	//if (!m_isInitialized) Init(true);

	const size_t count = m_startMatcher->GetMembers(refs);

	// Set span_matcher to be parent of refs
	for (size_t i = refs.size()-count; i < refs.size(); ++i) {
		refs[i].matchptr = this;
	}

	return count;
}

// -------------------------------------------------------------------------------------

void group_matcher::AddMember(matcher* m) {
	if (m->IsEnabled()) m_members.push_back(m);
}

bool group_matcher::Init(bool WXUNUSED(deep)) {
	if (m_initializing || m_isInitialized) return true;
	m_initializing = true;

	// Initialize all members and use their patterns
	for (vector<matcher*>::const_iterator m = m_members.begin(); m != m_members.end(); ++m) {
		matcher* const member = *m;
		
		// Add all member sub-patterns and map new callout id's to old
		member->GetMembers(m_refs);
	}

/*#ifdef __WXDEBUG__
		// Check for duplicates
		set<matcher*> matchrefs;
		vector<calloutref>::iterator p = m_refs.begin();
		while (p != m_refs.end()) {
			if (matchrefs.find(p->realMatchptr) == matchrefs.end()) {
				matchrefs.insert(p->realMatchptr);
				++p;
			else {
				wxASSERT(false);
				p = m_refs.erase(p);
			}
		}
#endif //__WXDEBUG__*/

	m_isInitialized = true;
	m_initializing = false;
	return true;
}

matcher& group_matcher::GetCallout(unsigned int callout_id) {
	wxASSERT(m_isInitialized);
	wxASSERT(callout_id < m_refs.size());

	// This returns the REAL member (deep search)
	const calloutref cf = m_refs[callout_id];
	return cf.matchptr->SubGetCallout(cf.callout_id);
}

bool group_matcher::IsSpanStart(unsigned int callout_id) {
	wxASSERT(m_isInitialized);
	wxASSERT(callout_id < m_refs.size());
	return m_refs[callout_id].matchptr->SubIsSpanStart(m_refs[callout_id].callout_id);
}

matcher* group_matcher::GetMember(unsigned int callout_id) {
	wxASSERT(m_isInitialized);
	wxASSERT(callout_id < m_refs.size());

	return m_refs[callout_id].matchptr;
}

size_t group_matcher::GetMembers(vector<calloutref>& refs) {
	if (!m_isInitialized) Init(true);

	size_t ndx = refs.size();
	const size_t end = ndx + m_refs.size();
	
	refs.resize(end);
	for (unsigned int i = 0; i < m_refs.size(); ++i) {
		const calloutref& r1 = m_refs[i];
		calloutref& r = refs[ndx];
		r.matchptr = this;
		r.realMatchptr = r1.realMatchptr;
		r.callout_id = i;

		++ndx;
	}

	return m_refs.size();
}

unsigned int group_matcher::GetSubId(unsigned int id) {
	wxASSERT(m_isInitialized);
	wxASSERT(id < m_refs.size());

	const calloutref& cf = m_refs[id];
	return cf.matchptr->SubGetId(cf.callout_id);
}

int group_matcher::Match(char* line, unsigned int start, unsigned int len, unsigned int& callout_id, int *ovector, int ovecsize, int zeromatch) {
	wxASSERT(m_isInitialized);
	wxASSERT(start < len);

	static const int search_options = PCRE_ANCHORED|PCRE_NO_UTF8_CHECK;
	const size_t ref_count = m_refs.size();

	for (unsigned int pos = start; pos < len; ++pos) {
		// Only start at valid utf8 chars
		if ((line[pos] & 0xC0) == 0x80) continue;

		for (unsigned int i = 0; i < ref_count; ++i) {
			match_matcher& m = *m_refs[i].realMatchptr;
			//wxLogDebug(wxT("%d: %s (%x)"), i, m_refs[i].matchptr->GetName().c_str(), m_refs[i].matchptr);
			//wxLogDebug(wxT("    (%x) %s"), m_refs[i].realMatchptr, m_refs[i].realMatchptr->GetPattern().c_str());

			// Get search pattern from matcher
			const pcre* re = m.GetMatchPattern();
			if (!re) return PCRE_ERROR_NULL;
			const pcre_extra* study = m.GetPatternStudy();
			
			// Do the search
			const int rc = pcre_exec(
				re,                   // the compiled pattern
				study,                // extra data - if we study the pattern
				line,                 // the subject string
				len,                  // the length of the subject
				pos,                  // start at offset in the subject
				search_options,       // options
				ovector,              // output vector for substring information
				ovecsize);            // number of elements in the output vector


			// Do we have a match?
			if (rc >= 0) {
				if (pos == start && (int)i == zeromatch) continue; // don't match same place twice

				callout_id = i;
				return rc;
			}
		}
	}

	// If we reach here, no match was found
	return PCRE_ERROR_NOMATCH;
}
