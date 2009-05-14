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

#include "SnippetHandler.h"
#include "tm_syntaxhandler.h"
#include "ShellRunner.h"
#include "EditorCtrl.h"
#include "Env.h"
#include "matchers.h"

void SnippetHandler::StartSnippet(EditorCtrl* editor, const vector<char>& snippet, cxEnv& env, const tmBundle* bundle) {
	wxASSERT(editor);
	m_env = &env;
	m_bundle = bundle;
	Clear();
	if (snippet.empty()) return;

	m_editor = editor;
	m_snippet = &snippet;

	// initialize parser
	//m_len = m_snippet->content.size();

	// Convert indents in snippet to match current tab settings
	const wxString& indentUnit = m_editor->GetIndentUnit();
	if (indentUnit == wxT('\t')) m_indentUnit.clear();
	else {
		const wxCharBuffer utfIndent = indentUnit.mb_str();
		const unsigned int indentLen = strlen(utfIndent.data());
		m_indentUnit.assign(utfIndent.data(), utfIndent.data()+indentLen);
	}

	if (Parse()) {
		m_offset = m_editor->GetPos();
		AdjustIndent();

		if (!m_snipText.empty()) {
			const wxString startSnip(&*m_snipText.begin(), wxConvUTF8, m_snipText.size());

			m_editor->RawInsert(m_offset, startSnip);
		}
		// Initial update
		UpdateVarTransforms();
		for (map<unsigned int,TabStop>::const_iterator p = m_tabstops.begin(); p != m_tabstops.end(); ++p) {
			ChangedTabstop(p->first, 0, true);
		}
		NextTab();
	}

	// TODO: Notify user if there are parse errors
}

void SnippetHandler::GotoEndAndClear() {
	m_editor->SetPos(m_offset + m_endpos);
	Clear();
}

void SnippetHandler::Clear() {
	if (m_editor) m_editor->Freeze();
	m_editor = NULL;
	m_snippet = NULL;

	m_offset = 0;
	m_tabstops.clear();
	m_intervals.clear();
	m_transforms.clear();
	m_varTransforms.clear();
	m_endpos = 0;
	m_curTab = 0;

	// parser state
	m_pos = 0;
	//m_len = 0;
	m_parentTabStop = 0;
	m_parentIv = -1;
	m_endposSet = false;
	m_snipText.clear();
}

bool SnippetHandler::Validate() {
	wxASSERT(m_editor && m_snippet);
	//wxASSERT(m_curTab != 0);

	const unsigned int pos = m_editor->GetPos();
	if (pos < m_offset) return false;

	// Verify that carret is inside or bordering current tabstop
	const TabInterval& iv = m_intervals[m_tabstops[m_curTab].iv];
	const unsigned alignStart = m_offset + iv.start;
	const unsigned alignEnd = m_offset + iv.end;
	if (pos < alignStart || pos > alignEnd) {
		Clear();
		return false;
	}
	else return true;
}

void SnippetHandler::NextTab() {
	wxASSERT(m_editor && m_snippet);

	// Get current tabstop
	map<unsigned int,TabStop>::iterator p = m_tabstops.find(m_curTab);

	// Find next tabstop
	if (m_curTab == 0) {
		if(!m_tabstops.empty()) {
			p = m_tabstops.begin();

			// don't start with the ender tabstop
			if (p->first == 0) ++p;
		}
	}
	else {
		if (p != m_tabstops.end()) {
			// Check if we need to pipe content through a shell command
			// before jumping to next tabstop
			if (p != m_tabstops.end() && !p->second.pipeCmd.empty()) {	
				DoPipe(p->second);
			}

			++p; // advance to next tabstop
		}
	}

	m_editor->RemoveAllSelections();

	if (p != m_tabstops.end()) {
		wxASSERT(p->second.isValid);
		const TabInterval& iv = m_intervals[p->second.iv];
		const bool isPipe = !p->second.pipeCmd.empty();

		// Go to the next tab
		m_curTab = p->first;
		m_editor->SetPos(m_offset + iv.start);
		m_editor->Select(m_offset+iv.start, m_offset+iv.end);
		if (iv.end == iv.start && ++p == m_tabstops.end() && iv.start == m_endpos) {
			// Last tabpos overlaps endpos. If it does not need piping
			// De-activate snippet (and freeze doc)
			if (!isPipe) Clear();
		}
	}
	else {
		// We are finished with snippet
		p = m_tabstops.find(0);
		if (p == m_tabstops.end()) {
			m_editor->SetPos(m_offset + m_endpos); // Set endpos
		}
		else {
			if (!p->second.pipeCmd.empty()) {
				m_curTab = 0;
				DoPipe(p->second);
			}
			else {
				const TabInterval& iv = m_intervals[p->second.iv];
				if (iv.start < iv.end) {
					m_editor->Select(m_offset + iv.start, m_offset + iv.end);
				}
				m_editor->SetPos(m_offset + iv.end);
			}
		}

		// De-activate snippet (and freeze doc)
		Clear();
	}
}

void SnippetHandler::PrevTab() {
	wxASSERT(m_editor && m_snippet);
	wxASSERT(m_curTab != 0); // ender tabstop should have deactivated snippet

	// You cannot go beyond first tabstop
	if (m_curTab <= 1) return;

	map<unsigned int,TabStop>::iterator p = m_tabstops.find(m_curTab);
	wxASSERT(p != m_tabstops.end());
	wxASSERT(p != m_tabstops.begin());

	m_editor->RemoveAllSelections();

	// Go to previous tabstop
	--p;
	const TabInterval& iv = m_intervals[p->second.iv];
	m_curTab = p->first;
	m_editor->SetPos(m_offset + iv.start);
	m_editor->Select(m_offset + iv.start, m_offset + iv.end);
}


void SnippetHandler::Insert(const wxString& text) {
	wxASSERT(m_editor && m_snippet);
	//wxASSERT(m_curTab != 0);

	unsigned int pos = m_editor->GetPos();

	// Verify that insertion is inside or bordering current tabstop
	map<unsigned int,TabStop>::iterator p = m_tabstops.find(m_curTab);
	if (p != m_tabstops.end()) {
		TabInterval& iv = m_intervals[p->second.iv];
		const unsigned alignPos = pos - m_offset;
		if (alignPos >= iv.start && alignPos <= iv.end) {
			int diff = 0;

			// Do we have to remove a selection
			if (m_editor->IsSelected()) {
				m_editor->RemoveAllSelections();
				m_editor->RawDelete(m_offset+iv.start, m_offset+iv.end);
				diff = iv.start - iv.end;
				pos = m_offset+iv.start;
			}

			// Insert the text (newline needs special handling for indentation)
			unsigned int len = 0;
			if (text == wxT("\n")) len = m_editor->InsertNewline();
			else len = m_editor->RawInsert(pos, text, true);

			diff += len;

			// Extend tabstop
			//iv.end += diff;

			// Update intervals & mirrors
			ChangedTabstop(m_curTab, diff);
			return;
		}
	}
	else wxASSERT(false); // m_curTab invalid

	// If we get to here we de-activate the snippet
	m_editor->RawInsert(pos, text, true);
	Clear(); // after insert to keep m_editor valid
}

void SnippetHandler::Delete(unsigned int start, unsigned int end) {
	wxASSERT(m_editor && m_snippet);
	wxASSERT(start <= end);
	wxASSERT(m_curTab != 0);

	const unsigned int alignStart = start - m_offset;
	const unsigned int alignEnd = end - m_offset;

	// Verify that deletion is inside current tabstop
	map<unsigned int,TabStop>::iterator p = m_tabstops.find(m_curTab);
	if (p != m_tabstops.end()) {
		TabInterval& iv = m_intervals[p->second.iv];
		if (alignStart >= iv.start && alignEnd <= iv.end) {
			// Remove any selections
			m_editor->RemoveAllSelections();

			if (start < end) {
				// Delete the text
				m_editor->RawDelete(start, end);
				const int diff = start - end;

				// Resize tabstop
				//iv.end += diff;

				// Update intervals & mirrors
				ChangedTabstop(m_curTab, diff);
			}
			return;
		}
	}
	else wxASSERT(false); // m_curTab invalid

	// If we get to here we de-activate the snippet
	m_editor->RawDelete(start, end);
	Clear(); // after delete to keep m_editor valid
}

void SnippetHandler::ChangedTabstop(unsigned int tabstop, int diff, bool init) {
	TabStop& ts = m_tabstops[tabstop];
	TabInterval& iv = m_intervals[ts.iv];

	wxASSERT(iv.start <= iv.end);

	// Remove any contained tabstops
	if (!init) {
		RemoveChildren(ts);
	}

	// move all intervals following (or containing) tabstop
	UpdateIntervals(ts.iv, diff);
	const unsigned int contentLen = iv.end - iv.start;

	// Apply change to mirrors
	if (!ts.mirrors.empty()) {
		const wxString content = m_editor->GetText(m_offset+iv.start, m_offset+iv.end);
		vector<unsigned int>& mirrors = ts.mirrors;

		for (vector<unsigned int>::iterator m = mirrors.begin(); m != mirrors.end(); ++m) {
			TabInterval& mIv = m_intervals[*m];
			const unsigned int ivLen = mIv.end - mIv.start;

			int mDiff = contentLen;
			if (ivLen) {
				m_editor->RawDelete(m_offset + mIv.start, m_offset + mIv.end);
				mDiff -= ivLen;
			}
			m_editor->RawInsert(m_offset + mIv.start, content);

			//mIv.end = mIv.start + contentLen;

			// move all intervals following (or containing) mirror
			UpdateIntervals(*m, mDiff);
		}
	}

	// Apply change to transformations
	if (!ts.transforms.empty()) {
		for (vector<unsigned int>::const_iterator t = ts.transforms.begin(); t != ts.transforms.end(); ++t) {
			Transformation& tr = m_transforms[*t];

			// Delete old text
			TabInterval& mIv = m_intervals[tr.iv];
			const unsigned int insPos = m_offset + mIv.start;
			m_editor->RawDelete(insPos, m_offset + mIv.end);
			int mDiff = mIv.start - mIv.end;

			// Get positions of source interval
			unsigned int start = m_offset + iv.start;
			unsigned int end = m_offset + iv.end;
			const bool beforeSource = start > insPos;
			if (beforeSource) {
				start += mDiff;
				end += mDiff;
			}

			// Insert the transformed replacetext
			const wxString replacetext = DoTransform(start, end, tr);
			const unsigned int len = m_editor->RawInsert(insPos, replacetext);

			// Move all intervals following (or containing) transform
			mDiff += len;
			UpdateIntervals(tr.iv, mDiff);
		}
	}
}

void SnippetHandler::UpdateVarTransforms() {
	wxASSERT(m_editor && m_snippet);

	for (vector<Transformation>::const_iterator t = m_varTransforms.begin(); t != m_varTransforms.end(); ++t) {
		TabInterval& iv = m_intervals[t->iv];

		// Search
		const unsigned int start = m_offset + iv.start;
		const unsigned int end = m_offset + iv.end;
		const wxString replacetext = DoTransform(start, end, *t);

		// Replace the text
		m_editor->RawDelete(start, end);
		const unsigned int len = m_editor->RawInsert(start, replacetext);

		// Move all intervals following (or containing) transform
		const int mDiff = len - (end - start);
		UpdateIntervals(t->iv, mDiff);
	}
}

wxString SnippetHandler::DoTransform(unsigned int start, unsigned int end, const Transformation& tr) const {
	wxASSERT(start <= end);

	wxString output;
	map<unsigned int,interval> captures;
	unsigned int pos = start;

	for (;;) {
		captures.clear();
		const search_result res = m_editor->RawRegexSearch(&*tr.regexp.begin(), start, end, pos, &captures);
		if (res.error_code < 0) break; // no match or invalid regex

		// Insert un-transformed text
		if (res.start > pos) output += m_editor->GetText(pos, res.start);

		// Insert transformed text
		output += m_editor->ParseReplaceString(tr.format, captures);

		// Adjust positions
		pos = res.end;
		if (res.start == res.end) {
			++pos; // avoid infinite loop on zero-length match
			if (pos <= end) output += m_editor->GetText(res.end, pos);
		}

		if (pos >= end || !tr.isGlobal) break;
	}

	// Insert last un-tranformed text
	if (pos < end) output += m_editor->GetText(pos, end);

	IndentString(output);
	return output;
}

void SnippetHandler::DoPipe(const TabStop& ts) {
	if (ts.pipeCmd.empty()) return;

	wxBusyCursor wait; // Set a busy cursor (will be reset when leaving scope)

	// Get up-to-date env
	cxEnv env;
	m_editor->SetEnv(env, true, m_bundle);

	// Set snippet and tabstops as env vars
	const wxString snippet = m_editor->GetText(m_offset, m_offset + m_endpos);
	env.SetEnv(wxT("TM_SNIPPET"), snippet);
	for (map<unsigned int,TabStop>::const_iterator p = m_tabstops.begin(); p != m_tabstops.end(); ++p) {
		const TabInterval& iv = m_intervals[p->second.iv];
		const wxString tabstop = m_editor->GetText(m_offset+iv.start, m_offset+iv.end);
		const wxString key = wxString::Format(wxT("TM_TABSTOP_%d"), p->first);
		env.SetEnv(key, tabstop);
	}

	// Get Input
	const TabInterval& iv = m_intervals[ts.iv];
	vector<char> input;
	m_editor->GetTextPart(m_offset+iv.start, m_offset+iv.end, input);

	vector<char> output;
	const int pid = ShellRunner::RawShell(ts.pipeCmd, input, &output, NULL, env);
	if (pid == 0 && !output.empty()) {		
		if (output.back() == '\n') output.pop_back(); // Strip the ending newline

		// Re-select tabstop
		m_editor->RemoveAllSelections();
		m_editor->Select(m_offset+iv.start, m_offset+iv.end);

		// Overwrite with output from cmd
		const wxString cmd_out = wxString(&*output.begin(), wxConvUTF8, output.size());
		Insert(cmd_out);

		//m_editor->SetPos(m_offset + iv.end);
	}
}

void SnippetHandler::AdjustIndentUnit() {
	// Check if we should convert indents in snippet to soft tabs.
	// This function should be called before Parse(), since it
	// does not adjust intervals
	const wxString& indentUnit = m_editor->GetIndentUnit();
	if (indentUnit == wxT('\t')) return;

	// Convert indent to utf-8
	const wxCharBuffer utfIndent = indentUnit.mb_str();
	const unsigned int indentLen = strlen(utfIndent.data());

	// Replace all tabs
	for (unsigned int i = 0; i < m_snipText.size(); ++i) {
		if (m_snipText[i] == '\t') {
			// Replace the tab
			m_snipText.erase(m_snipText.begin()+i, m_snipText.begin()+i+1);
			m_snipText.insert(m_snipText.begin()+i, utfIndent.data(), utfIndent.data()+indentLen);

			i += indentLen;
		}
	}
}

void SnippetHandler::AdjustIndent() {
	m_indent = m_editor->GetLineIndentFromPos(m_offset);
	if (m_indent.empty()) return;

	// Convert indent to utf-8
	const wxCharBuffer utfIndent = m_indent.mb_str();
	const unsigned int indentLen = strlen(utfIndent.data());

	// Insert indent after all newlines
	for (unsigned int i = 0; i < m_snipText.size(); ++i) {
		if (m_snipText[i] == '\n') {
			// move all intervals following (or containing) insertion
			UpdateIntervalsFromPos(i, indentLen);

			// Insert the indent
			++i;
			m_snipText.insert(m_snipText.begin()+i, utfIndent.data(), utfIndent.data()+indentLen);

			i += indentLen;
		}
	}
}

void SnippetHandler::IndentString(wxString& text) const {
	if (m_indent.empty()) return; // is set in AdjustIndent()

	for (unsigned int i = 0; i < text.size(); ++i) {
		if (text[i] != wxT('\n')) continue;

		// Insert indent after newline
		++i;
		text.insert(i, m_indent);
		i += m_indent.size();
	}
}

void SnippetHandler::UpdateIntervals(unsigned int id, int diff) {
	wxASSERT(id < m_intervals.size());

	if (diff == 0) return;

	// Update the changed interval
	TabInterval& iv = m_intervals[id];
	iv.end += diff;

	// Resize parents
	int parent = iv.parent;
	while (parent != -1) {
		TabInterval& tp = m_intervals[parent];
		tp.end += diff;
		parent = tp.parent;
	}

	// Move intervals following pos
	for (vector<TabInterval>::iterator vi = m_intervals.begin()+id+1; vi != m_intervals.end(); ++vi) {
		// children are resized rather than moved
		if (vi->start != iv.start || vi->parent < (int)id) {
			vi->start += diff;
		}
		vi->end += diff;
	}

	// Move endpos
	if (m_endpos >= iv.start) m_endpos += diff;
}

void SnippetHandler::UpdateIntervalsFromPos(unsigned int pos, int diff) {
	if (diff == 0) return;

	// Move invervals containing or beyond pos
	vector<TabInterval>::iterator vi = m_intervals.begin();
	for (; vi != m_intervals.end(); ++vi) {
		if (pos < vi->end) {

			if (pos < vi->start) vi->start += diff; // If contained, only resize
			vi->end += diff;
		}
	}

	// Move endpos
	if (m_endpos > pos) m_endpos += diff;
}

void SnippetHandler::RemoveChildren(TabStop& ts) {
	// Remove contained tabstops
	for (vector<unsigned int>::const_iterator c = ts.children.begin(); c != ts.children.end(); ++c) {
		RemoveChildren(m_tabstops[*c]);
		m_tabstops.erase(*c);
	}
	ts.children.clear();

	// Remove refs to contained mirrors
	for (vector<unsigned int>::const_iterator m = ts.childrenM.begin(); m != ts.childrenM.end(); ++m) {
		for (map<unsigned int,TabStop>::iterator t = m_tabstops.begin(); t != m_tabstops.end(); ++t) {
			TabStop& rts = t->second;

			vector<unsigned int>::iterator p = find(rts.mirrors.begin(), rts.mirrors.end(), *m);
			if (p != rts.mirrors.end()) {
				rts.mirrors.erase(p);
			}
		}
	}

	// Remove refs to contained transformations
	for (vector<unsigned int>::const_iterator r = ts.childrenTr.begin(); r != ts.childrenTr.end(); ++r) {
		for (map<unsigned int,TabStop>::iterator t = m_tabstops.begin(); t != m_tabstops.end(); ++t) {
			TabStop& rts = t->second;

			vector<unsigned int>::iterator p = find(rts.transforms.begin(), rts.transforms.end(), *r);
			if (p != rts.transforms.end()) {
				rts.transforms.erase(p);
			}
		}
	}
}

bool SnippetHandler::Parse(bool isWrapped) {
	wxASSERT(m_editor && m_snippet);

	const vector<char>& content = *m_snippet;
	const unsigned int len = content.size();

	while (m_pos < len) {
		char c = content[m_pos];

		switch(c) {
		case '\\':
			{
				const unsigned int nextpos = m_pos + 1;
				if (nextpos < len) {
					const char c2 = content[nextpos];
					if (c2 == '$' ||
						c2 == '`' ||
						c2 == '}' ||
						c2 == '|' ||
						c2 == '\\') {
						m_snipText.push_back(c2);
						m_pos += 2;
						continue;
					}
				}

				m_snipText.push_back(c);
				++m_pos;
			}
			break;
		case '$':
			++m_pos;
			ParseReplacement();
			break;
		case '`':
			// Parse Shell code
			{
				bool isCmd = false;

				++m_pos;
				unsigned int end = m_pos;
				while (end < len) {
					c = content[end];
					if (c == '\\') ++end;
					else if (c == '`') {
						isCmd = true;

						if (m_pos < end) {
							vector<char> cmd(&content[m_pos], &content[end]);
							vector<char> output;

							// Set a busy cursor (will be reset when leaving scope)
							wxBusyCursor wait;

							wxASSERT(m_env);
							const int pid = ShellRunner::RawShell(cmd, vector<char>(), &output, NULL, *m_env);
							if (pid == 0 && !output.empty()) {
								// Strip the ending newline
								if (output.back() == '\n') output.pop_back();

								m_snipText.insert(m_snipText.end(), output.begin(), output.end());
							}
						}
						m_pos = end+1;
						break;
					}
					++end;
				}

				// If we get to here just add it as a normal char
				if (!isCmd) m_snipText.push_back('`');
			}
			break;

		case '\t':
			// Convert tabs to match current settings
			if (m_indentUnit.empty()) m_snipText.push_back('\t'); // Just add the tab
			else m_snipText.insert(m_snipText.end(), m_indentUnit.begin(), m_indentUnit.end());
			++m_pos;
			break;

		case '|':
			if (isWrapped) {
				TabStop& ts = m_tabstops[m_parentTabStop];
				if (ts.isValid) {
					// Parse pipe to shellcmd
					unsigned int end = ++m_pos;
					while (end < len) {
						c = content[end];
						if (c == '\\') ++end;
						else if (c == '}') {
							if (m_pos < end) {
								ts.pipeCmd.assign(&content[m_pos], &content[end]);
							}
							m_pos = end+1;
							break;
						}
						++end;
					}
					return true;
				}
			}
			
			// Default: just handle it as normal char
			m_snipText.push_back(c);
			++m_pos;
			break;

		case '}':
			if (isWrapped) {
				++m_pos;
				return true;
			}
			// fall-through if un-wrapped
		default:
			m_snipText.push_back(c);
			++m_pos;
		}
	}

	if (!m_endposSet) {
		m_endpos = m_snipText.size();
		m_endposSet = true;
	}

	// Validate all tabstops
	for (map<unsigned int,TabStop>::const_iterator p = m_tabstops.begin(); p != m_tabstops.end(); ++p) {
		if (!p->second.isValid) {
			wxFAIL_MSG(wxString::Format(wxT("Invalid TabStop %d"), p->first));
			return false;
		}
	}

	return true;
}

bool SnippetHandler::ParseReplacement() {
	const vector<char>& content = *m_snippet;
	const unsigned int len = content.size();

	char c = content[m_pos];
	if (c == '{') {
		++m_pos;
		c = content[m_pos];
		if (isdigit(c)) {
			if (!ParseTabStop()) return false;
		}
		else if(isalpha(c)) {
			if (!ParseVariable()) return false;
		}
		else return false;
	}
	else if (wxIsdigit(c)) {
		unsigned int end = m_pos + 1;
		while (end < len && isdigit(content[end])) ++end;

		const unsigned int tabstop = atoi(&content[m_pos]);

		if (tabstop == 0) {
			m_endpos = m_snipText.size();
			m_endposSet = true;
		}
		else {
			// add empty interval
			const TabInterval iv = {m_parentIv, m_snipText.size(), m_snipText.size()};
			const unsigned int currentIv = m_intervals.size();
			m_intervals.push_back(iv);

			// add tabstop
			TabStop& ts = m_tabstops[tabstop];
			if (!ts.isValid) {
				ts.iv = currentIv;
				if (m_parentTabStop > 0) {
					m_tabstops[m_parentTabStop].children.push_back(tabstop);
				}
				ts.isValid = true;
			}
			else ts.mirrors.push_back(currentIv);
		}

		m_pos = end;
	}
	else if (isalpha(c)) {
		unsigned int end = m_pos + 1;
		while (end < len && (isalnum(content[end]) || content[end] == '_')) ++end;
		const wxString var(&content[m_pos], wxConvUTF8, end-m_pos);

		// Retrieve env variable (from tm or app env)
		wxString value;
		bool hasEnv = m_env->GetEnv(var, value);
		if (!hasEnv) hasEnv = wxGetEnv(var, &value);

		// Insert env value
		if (hasEnv) {
			const wxCharBuffer utfVal = value.mb_str(wxConvUTF8);
			m_snipText.insert(m_snipText.end(), utfVal.data(), utfVal.data()+strlen(utfVal));
		}

		m_pos = end;
	}
	else m_snipText.push_back('$');

	return true;
}

bool SnippetHandler::ParseTabStop() {
	const vector<char>& content = *m_snippet;
	const unsigned int len = content.size();
	const unsigned int start = m_snipText.size();

	// Get the tabstop
	unsigned int end = m_pos + 1;
	while (end < len && isdigit(content[end])) ++end;
	const unsigned int tabstop = atoi(&content[m_pos]);
	m_pos = end;

	// Add the interval
	const TabInterval iv = {m_parentIv, start,start};
	const unsigned int currentIv = m_intervals.size();
	m_intervals.push_back(iv);
	m_parentIv = currentIv;

	char c = content[m_pos];
	if (c == '}' || c == ':' || c == '|') {
		++m_pos;

		// Add tabstop
		TabStop& ts = m_tabstops[tabstop];
		bool isTabstop = false; // Could be mirror or transformation 
		if (!ts.isValid) {
			ts.iv = currentIv;
			if (m_parentTabStop > 0) {
				m_tabstops[m_parentTabStop].children.push_back(tabstop);
			}
			ts.isValid = true;
			isTabstop = true;
		}
		else {
			ts.mirrors.push_back(currentIv);
			if (m_parentTabStop > 0) {
				m_tabstops[m_parentTabStop].childrenM.push_back(currentIv);
			}
		}
	
		if (c == ':') {
			// Parse default value
			unsigned int oldStop = m_parentTabStop;
			m_parentTabStop = tabstop;
			if (!Parse(true)) return false;
			m_parentTabStop = oldStop;

			m_intervals[currentIv].end = m_snipText.size();
		}
		else if (c == '|') {
			// Parse pipe to shellcmd
			unsigned int end = m_pos;
			while (end < len) {
				c = content[end];
				if (c == '\\') ++end;
				else if (c == '}') {
					if (m_pos < end) {
						ts.pipeCmd.assign(&content[m_pos], &content[end]);
					}
					m_pos = end+1;
					break;
				}
				++end;
			}
		}
	}
	else if (c == '/') {
		++m_pos;
		Transformation tr;
		tr.iv = currentIv;

		if (!ParseTransform(tr)) return false;

		const unsigned int trId = m_transforms.size();
		m_transforms.push_back(tr);

		// Add as transform
		TabStop& ts = m_tabstops[tabstop];
		ts.transforms.push_back(trId);

		// add back ref
		if (m_parentTabStop > 0) {
			m_tabstops[m_parentTabStop].childrenTr.push_back(trId);
		}
	}

	m_parentIv = iv.parent;
	return true;
}

bool SnippetHandler::ParseVariable() {
	const vector<char>& content = *m_snippet;
	const unsigned int len = content.size();
	const unsigned int start = m_snipText.size();
	unsigned int end = start;

	// Get the env variable
	unsigned int eend = m_pos + 1;
	while (eend < len && (isalnum(content[eend]) || content[eend] == '_')) ++eend;
	const wxString var(&content[m_pos], wxConvUTF8, eend-m_pos);
	m_pos = eend;

	// Retrieve env variable (from tm or app env)
	wxString value;
	bool hasEnv = m_env->GetEnv(var, value);
	if (!hasEnv) hasEnv = wxGetEnv(var, &value);

	// Insert env value
	if (hasEnv) {
		const wxCharBuffer utfVal = value.mb_str(wxConvUTF8);
		m_snipText.insert(m_snipText.end(), utfVal.data(), utfVal.data()+strlen(utfVal));
		end = m_snipText.size();
	}

	char c = content[m_pos];
	if (c == ':') {
		++m_pos;
		if (!hasEnv) {
			// Insert alternative
			if (!Parse(true)) return false;
		}
		else {
			// ignore alt
			while (m_pos < len) {
				c = content[m_pos];
				if (c == '\\') ++m_pos;
				else if (c == '}') {
					++m_pos;
					break;
				}
				++m_pos;
			}
		}
	}
	else if (c == '/') {
		++m_pos;

		Transformation tr;
		if (!ParseTransform(tr)) return false;

		// Add interval
		const TabInterval iv = {m_parentIv, start, end};
		tr.iv = m_intervals.size();
		m_intervals.push_back(iv);

		m_varTransforms.push_back(tr);
	}
	else if (c == '}') {
		++m_pos; // no alt
	}

	return true;
}

bool SnippetHandler::ParseTransform(Transformation& tr) {
	const vector<char>& content = *m_snippet;
	const unsigned int len = content.size();

	// Init transformation
	tr.isGlobal = false;

	// Get regex
	unsigned int regex_end = m_pos;
	bool isSlashed = false;
	while (regex_end < len && !(content[regex_end] == '/' && !isSlashed)) {
		if (isSlashed) isSlashed = false;
		else if (content[regex_end] == '\\') isSlashed = true;
		++regex_end;
	}
	if (regex_end == len) return false;

	// Covert regex from Onigoruma to PCRE syntax
	wxString regex(&content[m_pos], wxConvUTF8, regex_end-m_pos);
	matcher::RegExConvert(regex);
	wxCharBuffer buf = regex.mb_str(wxConvUTF8);

	tr.regexp.assign(buf.data(), buf.data()+strlen(buf.data()));
	tr.regexp.push_back('\0');
	//tr.regexp = wxString(&content[m_pos], wxConvUTF8, regex_end-m_pos);
	m_pos = regex_end + 1; // eat slash

	// Get format
	// (tabs will be converted to current settings in ParseReplaceString)
	unsigned int format_end = m_pos;
	isSlashed = false;
	while (format_end < len && !(content[format_end] == '/' && !isSlashed)) {
		if (isSlashed) isSlashed = false;
		else if (content[format_end] == '\\') isSlashed = true;
		++format_end;
	}
	if (format_end == len) return false;
	tr.format = wxString(&content[m_pos], wxConvUTF8, format_end-m_pos);
	m_pos = format_end + 1; // eat slash


	// ignore options for now
	if (content[m_pos] == 'g') {
		tr.isGlobal = true;
		++m_pos;
	}
	if (content[m_pos] == 'm') ++m_pos;

	if (content[m_pos] != '}') return false;
	++m_pos;

	return true;
}

void SnippetHandler::Print() {
	wxLogDebug(wxT("currentTab: %u"), m_curTab);
	wxLogDebug(wxT("Tabstops:"));
	for (map<unsigned int,TabStop>::const_iterator t = m_tabstops.begin(); t != m_tabstops.end(); ++t) {
		wxLogDebug(wxT("  %u: iv=%u"), t->first, t->second.iv);
	}
	wxLogDebug(wxT("Intervals:"));
	for (unsigned int i = 0; i < m_intervals.size(); ++i) {
		const TabInterval& iv = m_intervals[i];
		wxLogDebug(wxT("  %u: %u-%u (%d)"), i, iv.start, iv.end, iv.parent);
	}
	wxLogDebug(wxT(""));
}
