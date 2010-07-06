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

#include "CommandHandler.h"
#include "EditorCtrl.h"
#include "EditorFrame.h"

template <class F>
class TextObjectTraverser0 : public CommandHandler::TextObjectTraverser {
public:
	TextObjectTraverser0(F function) : m_function(function) {}
	bool operator()(EditorCtrl& editor, size_t pos, interval& iv, interval& iv_inner) const {
		return (editor.*m_function)(pos, iv, iv_inner);
	}
private:
	F m_function;
};

template <class F, class V>
class TextObjectTraverser1 : public CommandHandler::TextObjectTraverser {
public:
	TextObjectTraverser1(F function, const V& arg1) : m_function(function), m_arg1(arg1) {}
	bool operator()(EditorCtrl& editor, size_t pos, interval& iv, interval& iv_inner) const {
		return (editor.*m_function)(m_arg1, pos, iv, iv_inner);
	}
private:
	F m_function;
	const V& m_arg1;
};

template<class F> TextObjectTraverser0<F> bindtraverser(F op) {
	return TextObjectTraverser0<F>(op);
}
template<class F, class V> TextObjectTraverser1<F,V> bindtraverser(F op, const V& v) {
	return TextObjectTraverser1<F,V>(op, v);
}

CommandHandler::CommandHandler(EditorFrame& parentFrame, EditorCtrl& editor)
: m_parentFrame(parentFrame), m_editor(editor), m_isRecording(false), m_isReplaying(false) {
	Clear();
}

bool CommandHandler::IsSearching() const {
	return (m_state == state_search || m_state == state_search_reverse);
}

bool CommandHandler::InCommand() const {
	return m_cmd.empty();
}

const wxString& CommandHandler::GetCommandString() const {
	return m_cmd;
}

void CommandHandler::Clear(bool cancel) {
	if (m_isRecording && !m_cmd.empty() && !cancel) {
		eMacro& macro = m_editor.GetMacro();
		eMacroCmd& cmd = macro.Add(wxT("RunCommandMode"));
		cmd.SetArg(0, wxT("command"), m_cmd);
	}

	m_state = state_normal;
	m_endState = state_normal;
	m_count = 0;
	m_count2 = 0;
	m_select = false;
	m_cmd.clear();
	m_reverse = false;
}

void CommandHandler::PlayCommand(const wxString& cmd) {
	wxKeyEvent evt(wxEVT_CHAR);

	m_isReplaying = true;
	for (wxString::const_iterator p = cmd.begin(); p != cmd.end(); ++p) {
		evt.m_keyCode = *p;
		evt.m_uniChar = *p;
		ProcessCommand(evt);
	}
	m_isReplaying = false;
}

bool CommandHandler::ProcessCommand(const wxKeyEvent& evt, bool record) {
	m_isRecording = record;
	const int c = evt.GetKeyCode();
	const wxChar uc = evt.GetUnicodeKey();
	
	// Build command string
	switch (c) {
	case WXK_LEFT:  m_cmd += 'h'; break;
	case WXK_RIGHT: m_cmd += 'l'; break;
	case WXK_UP:    m_cmd += 'k'; break;
	case WXK_DOWN:  m_cmd += 'j'; break;
	default:
		if (wxIsprint(uc)) m_cmd += uc;
	}

	// TODO: clear visual state on manual selection
	const bool selected = m_editor.IsSelected();

	//if (selected && m_state != state_visual)

	// Counts
	switch (m_state) {
	case state_normal:
	case state_visual:
	case state_delete:
	case state_copy:
	case state_range:
		{
			switch (c) {
			case '0':
				if (m_count) {
					m_count *= 10;
					m_parentFrame.ShowCommand(m_cmd);
					return true;
				}
				else break;
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
				m_count *= 10;
				m_count += c - '0';
				m_parentFrame.ShowCommand(m_cmd);
				return true;
			}
		}
	}

	// count should be product of counts or at least 1
	const size_t count = (m_count || m_count2)
							? ((m_count && m_count2)
								? m_count * m_count2
								: m_count + m_count2)
							: 1;

	// Movements
	switch (m_state) {
	case state_normal:
	case state_visual:
	case state_delete:
	case state_copy:
	case state_range:
	case state_change:
		{	
			switch (c) {
			// Movements
			case 'h':
			case WXK_LEFT:
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorLeft), EditorCtrl::SEL_IGNORE));
				break;
			case 'j':
			case WXK_DOWN:
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorDown), EditorCtrl::SEL_IGNORE));
				break;
			case 'k':
			case WXK_UP:
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorUp), EditorCtrl::SEL_IGNORE));
				break;
			case 'l':
			case WXK_RIGHT:
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorRight), EditorCtrl::SEL_IGNORE));
				break;
			case '0':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToLineStart), false));
				break;
			case '^':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToLineStart), true));
				break;
			case '$':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToLineEnd));
				break;
			case 'G':
				if (count) DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToLine), count));
				else DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToEnd), EditorCtrl::SEL_IGNORE));
				break;
			case '|':
				if (count) DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToColumn), count));
				else DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToLineStart), false));
				break;
			case 'f':
				m_state = state_findchar;
				break;
			case 'F':
				m_state = state_findchar_reverse;
				break;
			case 'w':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToWordStart), false));
				break;
			case 'W':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToWordStart), true));
				break;
			case 'e':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToWordEnd), false));
				break;
			case 'E':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToWordEnd), true));
				break;
			case 'b':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToPrevWordStart), false));
				break;
			case 'B':
				DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToPrevWordStart), true));
				break;
			case '+':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToNextLine));
				break;
			case '-':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToPrevLine));
				break;
			case '(':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToPrevSentence));
				break;
			case ')':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToNextSentence));
				break;
			case '{':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToParagraphStart));
				break;
			case '}':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToNextParagraph));
				break;
			case '[':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToPrevSymbol));
				break;
			case ']':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToNextSymbol));
				break;
			case '*':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToNextCurrent));
				break;
			case '#':
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToPrevCurrent));
				break;
			case '%':
				DoMovement(count, mem_fun_ref(&EditorCtrl::GotoMatchingBracket));
				break;
			case 'n':
				NextMatch(count, true);
				EndMovement();
				break;
			case 'N':
				NextMatch(count, false);
				EndMovement();
				break;

			// Selection
			case ',':
				SelectRangeStart();
				break;
			case '\\':
				if (selected) EnterSelections(count);
				break;
			case 'Y':
				m_reverse = true;
				//fall-through
			case 'V':
				if (selected) m_state = state_filter_sels;
				else m_state = state_select_all;
				break;
			case 'a':
				m_state = state_object;
				break;
			case 't':
				m_state = state_object_inner;
				break;

			// Commands
			case 'd': // delete
				// handle 'dd' shortcut
				if (m_state == state_delete) {
					m_editor.SelectCurrentLine();
					m_editor.Delete();
					m_editor.ReDraw();
					Clear();
					break;
				}

				if (selected) {
					m_editor.Delete();
					m_editor.ReDraw();
					Clear();
				}
				else {
					m_count2 = m_count; m_count = 0; // prepare for secondary count
					m_state = m_endState = state_delete;
					m_select = true;
				}
				break;
			case 'p': // paste
				for (size_t i = 0; i < count; ++i) {
					m_editor.OnPaste();
				}
				m_editor.ReDraw();
				Clear();
				break;
			case 'y': // copy
				// handle 'yy' shortcut
				if (m_state == state_copy) {
					m_editor.SelectCurrentLine();
					m_editor.OnCopy();
					m_editor.RemoveAllSelections();
					Clear();
					break;
				}

				if (selected) m_editor.OnCopy();
				else {
					m_count2 = m_count; m_count = 0; // prepare for secondary count
					m_state = m_endState =state_copy;
					m_select = true;
				}
				break;
			case 'x':
				if (selected) {
					SelectLikeMatch();
					m_editor.ReDraw();
				}
				else {
					m_select = true;
					DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorRight), EditorCtrl::SEL_IGNORE));
					if (m_editor.IsSelected()) {
						m_editor.Delete();
						m_editor.ReDraw();
					}
				}
				Clear();
				break;
			case 'X':
				if (selected) {
					SelectLikeMatch();
					m_editor.ReverseSelections();
					m_editor.ReDraw();
				}
				else {
					m_select = true;
					DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorLeft), EditorCtrl::SEL_IGNORE));
					if (m_editor.IsSelected()) {
						m_editor.Delete();
						m_editor.ReDraw();
					}
				}
				Clear();
				break;
			case 'o':
				if (selected) {
					CursorToOpposite();
					m_editor.ReDraw();
					Clear();
					break;
				}
				else if (!m_editor.HasSearchRange()) {
					// Insert line below current
					m_editor.CursorToLineEnd();
					m_editor.InsertNewline();
					m_parentFrame.ShowCommandMode(false);
					Clear();
					return true;
				}
			case 'O':
				if (!selected && !m_editor.HasSearchRange()) {
					// Insert line above current
					m_editor.CursorToLineStart();
					m_editor.InsertNewline();
					m_editor.CursorLeft(EditorCtrl::SEL_IGNORE);
					m_parentFrame.ShowCommandMode(false);
					Clear();
					return true;
				}
			case 'r':
				if (selected) {
					m_buffer.clear();
					m_state = state_replace;
				}
				else m_state = state_replacechar;
				break;

			// Modes
			case WXK_ESCAPE:
				m_state = state_normal;
				m_editor.ClearSearchRange();
				if (m_cmd.empty()) m_editor.RemoveAllSelections();
				EndMovement();
				break;
			case WXK_RETURN:
				if (m_state == state_range) SelectRangeEnd();
				m_state = state_normal;
				EndMovement();
				break;
			case 'i':
				m_editor.ClearSearchRange(true);
				m_parentFrame.ShowCommandMode(false);
				Clear();
				return true;
			case 'I':
				DoMovement(1, bind2nd(mem_fun_ref(&EditorCtrl::CursorToLineStart), true));
				m_editor.ClearSearchRange(true);
				m_parentFrame.ShowCommandMode(false);
				Clear();
				return true;
			case 'A':
				DoMovement(1, mem_fun_ref(&EditorCtrl::CursorToLineEnd));
				m_editor.ClearSearchRange(true);
				m_parentFrame.ShowCommandMode(false);
				Clear();
				return true;
			case 'c':
				if (m_state == state_change) {
					// Change current line
					m_select = false;
					DoMovement(1, bind2nd(mem_fun_ref(&EditorCtrl::CursorToLineStart), true), false);
					m_select = true;
					DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToLineEnd), false);
					if (m_editor.IsSelected()) m_editor.Delete();
					m_editor.ClearSearchRange(true);
					m_parentFrame.ShowCommandMode(false);
					Clear();
					m_editor.ReDraw();
					return true;
				}

				m_state = m_endState = state_change;
				m_select = true;
				break;
			case 'C':
				m_select = true;
				DoMovement(count, mem_fun_ref(&EditorCtrl::CursorToLineEnd));
				if (m_editor.IsSelected()) m_editor.Delete();
				m_editor.ClearSearchRange(true);
				m_parentFrame.ShowCommandMode(false);
				Clear();
				return true;
			case 'v':
				m_state = state_visual;
				m_select = true;
				break;
			case '/':
				m_state = state_search;
				m_search.clear();

				// Cache start positions
				m_searchPos = m_editor.GetPos();
				m_cursors = m_editor.GetSearchRangeCursors();
				break;
			case '?':
				m_state = state_search_reverse;
				m_search.clear();

				// Cache start positions
				m_searchPos = m_editor.GetPos();
				m_cursors = m_editor.GetSearchRangeCursors();
				break;

			default:
				return false; // unbound key
			}
		}
		break;

	case state_findchar:
		{
			m_search = wxChar(uc);
			DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToNextChar), uc));
		}
		break;
	case state_findchar_reverse:
		{
			m_search = wxChar(uc);
			DoMovement(count, bind2nd(mem_fun_ref(&EditorCtrl::CursorToPrevChar), uc));
		}
		break;
	case state_search:
		{
			DoSearch(count, c, uc);
			EndMovement();
		}
		break;
	case state_search_all:
		DoSearchAll(c, uc);
		m_editor.ReDraw();
		break;
	case state_search_lines:
		DoSearchLines(c, uc);
		m_editor.ReDraw();
		break;
	case state_object:
		SelectObject(c, count, true);
		EndMovement();
		break;
	case state_object_inner:
		SelectObject(c, count, false);
		EndMovement();
		break;
	case state_select_all:
		switch (c) {
		case 'a':
			m_state = state_select_all_object;
			break;
		case 't':
			m_state = state_select_all_object_inner;
			break;
		case '/':
			// find pattern
			m_search.clear();
			m_searchPos = m_editor.GetPos();
			m_state = state_search_all;
			break;
		case '|':
			// find lines
			m_search.clear();
			m_searchPos = m_editor.GetPos();
			m_state = state_search_lines;
			break;
		case '{':
			// find scope
			m_buffer.clear();
			m_state = state_scopesearch;
			break;
		case WXK_ESCAPE:
			Clear();
			break;
		default: break;
		}
		break;
	case state_filter_sels:
		FilterSelections(c, uc);
		break;
	case state_scopesearch:
		DoScopeSearch(c, uc);
		break;
	case state_select_all_object:
		SelectObject(c, count, true, true);
		if (m_reverse) m_editor.ReverseSelections();
		EndMovement();
		break;
	case state_select_all_object_inner:
		SelectObject(c, count, false, true);
		if (m_reverse) m_editor.ReverseSelections();
		EndMovement();
		break;
	case state_replace:
		if (c == WXK_ESCAPE) Clear();
		else Replace(uc);
		break;
	case state_replacechar:
		if (c == WXK_ESCAPE) Clear();
		else ReplaceChar(uc);
		break;
		

	default:
		wxASSERT(false); // invalid state
	}

	m_parentFrame.ShowCommand(m_cmd);
	return true;
}

void CommandHandler::EndMovement(bool redraw) {
	bool clearState = true;

	// Complex movements keep some state until done
	switch (m_state) {
	case state_visual:
	case state_search:
	case state_search_all:
	case state_range:
	case state_change:
		m_count = m_count2 = 0;
		clearState = false;
		break;
	default:
		break; // do nothing
	}

	// Commands need to wait till all movements are complete
	if (clearState && m_endState != state_normal) {
		if (m_editor.IsSelected()) {
			switch (m_endState) {
			case state_delete:
				m_editor.Delete();
				break;
			case state_copy:
				m_editor.OnCopy();
				m_editor.RemoveAllSelections();
				break;
			case state_change:
				if (m_editor.IsSelected()) m_editor.Delete();
				m_editor.ClearSearchRange(true);
				m_parentFrame.ShowCommandMode(false);
				break;
			default:
				break; // do nothing
			}
		}
	}

	m_editor.MakeCaretVisible();
	if (redraw) m_editor.ReDraw();
	if (clearState) Clear();
}

void CommandHandler::DoSearch(size_t count, int keyCode, wxChar c) {
	switch (keyCode) {
	case WXK_RETURN:
		if (m_endState == state_normal) {
			m_editor.RemoveAllSelections();
		}
		m_state = state_normal;
		return;
	case WXK_ESCAPE:
		m_editor.RemoveAllSelections();
		Clear();
		return;
	case WXK_BACK:
		if (!m_search.empty()) {
			m_search.erase(m_search.length()-1);
			m_cmd.erase(m_cmd.length()-1);
		}
		break;
	case '/':
		// keep selection but move caret to end
		if (m_editor.IsSelected()) {
			m_lastSearchPos = m_editor.GetSelections()[0].end;
			m_editor.SetPos(m_lastSearchPos);
		}
		Clear();
		return;
	default:
		m_search += c;
	}

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& lastCursors = m_editor.GetSearchRangeCursors();
		m_editor.RemoveAllSelections();

		// Search in each range
		for (size_t r = 0; r < ranges.size(); ++r) {
			const interval& iv = ranges[r];
			const size_t searchstart = m_cursors[r];
			
			search_result sr = {-1, searchstart, searchstart};
			for (size_t i = 0; i < count; ++i) {
				sr = m_editor.SearchDirect(m_search, FIND_MATCHCASE|FIND_USE_REGEX, sr.end, iv.end);
				if (sr.error_code < 0) break;
			}
			if (sr.error_code < 0) continue; // no matches

			// Move cursor
			const size_t cpos = m_editor.GetPos();
			if (lastCursors[r] == cpos) m_editor.SetPos(sr.start);
			m_editor.SetSearchRangeCursor(r, sr.start);

			// If movement is part of a command we want to highlight the
			// movement rather than the match
			if (m_endState == state_normal) m_editor.AddSelection(sr.start, sr.end, true);
			else m_editor.AddSelection(searchstart, sr.start, true);
		}
	}
	else {
		search_result sr = {-1, m_searchPos, m_searchPos};
		for (size_t i = 0; i < count; ++i) {
			sr = m_editor.RegExFind(m_search, sr.end, true);
			if (sr.error_code < 0) break;
		}
		if (sr.error_code < 0) return; // no matches

		m_lastSearchPos = sr.start;
		m_editor.SetPos(sr.start);

		// If movement is part of a command we want to highlight the
		// movement rather than the match
		if (m_endState == state_normal) m_editor.Select(sr.start, sr.end);
		else m_editor.Select(m_searchPos, sr.start);
	}

	m_editor.SetSearchHighlight(m_search, FIND_MATCHCASE|FIND_USE_REGEX);
}

void CommandHandler::DoSearchAll(int keyCode, wxChar c) {
	switch (keyCode) {
	case WXK_RETURN:
	case '/':
		Clear();
		return;
	case WXK_ESCAPE:
		m_editor.RemoveAllSelections();
		Clear();
		return;
	case WXK_BACK:
		if (!m_search.empty()) {
			m_search.erase(m_search.length()-1);
			m_cmd.erase(m_cmd.length()-1);

			if (m_search.empty()) {
				m_editor.SetPos(m_searchPos);
				m_editor.MakeCaretVisible();
				m_editor.ReDraw();
			}
		}
		break;
	default:
		m_search += c;
	}

	m_editor.RemoveAllSelections();
	if (m_search.empty()) return;

	const int options = FIND_MATCHCASE|FIND_USE_REGEX;
	const unsigned int pos = m_editor.GetPos();

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();

		// Select all matches in each range
		for (size_t i = 0; i < ranges.size(); ++i) {
			const interval& iv = ranges[i];
			const unsigned int cpos = cursors[i];

			// Do the search
			size_t searchpos = iv.start;
			for (;;) {
				const search_result sr = m_editor.SearchDirect(m_search, options, searchpos, iv.end);
				if (sr.error_code < 0) break; // no more matches

				m_editor.AddSelection(sr.start, sr.end);
				m_editor.SetSearchRangeCursor(i, sr.end);
				if (pos == cpos) {
					m_editor.SetPos(sr.end);
					m_lastSearchPos = sr.end;
				}

				searchpos = sr.end;
				if (sr.start == sr.end) ++searchpos; // avoid loop on zero-len matches
				if (searchpos > iv.end) break; // moved outside range
			}
		}
	}
	else {
		size_t searchpos = 0;
		for (;;) {
			const search_result sr = m_editor.SearchDirect(m_search, options, searchpos);
			if (sr.error_code < 0) break; // no more matches

			m_editor.AddSelection(sr.start, sr.end);
			m_editor.SetPos(sr.end);
			m_lastSearchPos = sr.end;

			searchpos = sr.end;
			if (sr.start == sr.end) ++searchpos; // avoid loop on zero-len matches
		}
	}

	if (m_reverse) m_editor.ReverseSelections();
}

void CommandHandler::DoSearchLines(int keyCode, wxChar c) {
	switch (keyCode) {
	case WXK_RETURN:
	case '|':
		Clear();
		return;
	case WXK_ESCAPE:
		m_editor.RemoveAllSelections();
		Clear();
		return;
	case WXK_BACK:
		if (!m_search.empty()) {
			m_search.erase(m_search.length()-1);
			m_cmd.erase(m_cmd.length()-1);

			if (m_search.empty()) {
				m_editor.SetPos(m_searchPos);
				m_editor.MakeCaretVisible();
				m_editor.ReDraw();
			}
		}
		break;
	default:
		m_search += c;
	}

	m_editor.RemoveAllSelections();
	if (m_search.empty()) return;

	const int options = FIND_MATCHCASE|FIND_USE_REGEX;
	const unsigned int pos = m_editor.GetPos();

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();

		// Select all matches in each range
		for (size_t r = 0; r < ranges.size(); ++r) {
			const interval& iv = ranges[r];
			const unsigned int cpos = cursors[r];

			// Do the search
			const size_t firstline = m_editor.GetLineFromPos(iv.start);
			const size_t lastline = m_editor.GetLineFromPos(iv.end)+1;
			for (size_t i = firstline; i < lastline; ++i) {
				interval l = m_editor.GetLineExtent(i);
				if (l.start < iv.start) l.start = iv.start;
				if (l.end > iv.end) l.end = iv.end;

				const search_result sr = m_editor.SearchDirect(m_search, options, l.start, l.end);
				if (sr.error_code < 0) continue; // no match in line

				m_editor.AddSelection(l.start, l.end);
				m_editor.SetSearchRangeCursor(r, l.end);
				if (pos == cpos) m_editor.SetPos(l.end);
			}
		}
	}
	else {
		const size_t linecount = m_editor.GetLineCount();
		for (size_t i = 0; i < linecount; ++i) {
			const interval iv = m_editor.GetLineExtent(i);

			const search_result sr = m_editor.SearchDirect(m_search, options, iv.start, iv.end);
			if (sr.error_code < 0) continue; // no match in line

			m_editor.AddSelection(iv.start, iv.end);
			m_editor.SetPos(iv.end);
		}
	}

	if (m_reverse) m_editor.ReverseSelections();
}

void CommandHandler::DoScopeSearch(int keyCode, wxChar c) {
	switch (keyCode) {
	case WXK_RETURN:
	case '}':
		DoSelectObject(true, true, bindtraverser(&EditorCtrl::GetNextObjectScope, m_buffer));
		if (m_reverse) m_editor.ReverseSelections();
		m_editor.ReDraw();
		Clear();
		return;
	case WXK_ESCAPE:
		m_editor.RemoveAllSelections();
		Clear();
		return;
	case WXK_BACK:
		if (!m_buffer.empty()) {
			m_buffer.erase(m_buffer.length()-1);
			m_cmd.erase(m_cmd.length()-1);
		}
		break;
	default:
		m_buffer += c;
	}
}

void CommandHandler::FilterSelections(int keyCode, wxChar c) {
	if (!m_editor.IsSelected()) return;

	// Get search pattern
	switch (keyCode) {
	case WXK_RETURN:
		break;
	case WXK_ESCAPE:
		Clear();
		return;
	case WXK_BACK:
		if (!m_search.empty()) {
			m_search.erase(m_search.length()-1);
			m_cmd.erase(m_cmd.length()-1);
		}
		return;
	default:
		m_search += c;
		return;
	}
	if (m_search.empty()) return;

	// Copy old selections
	const vector<interval> selections = m_editor.GetSelections();
	m_editor.RemoveAllSelections();

	const int options = FIND_MATCHCASE|FIND_USE_REGEX;

	for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
		const search_result sr = m_editor.SearchDirect(m_search, options, p->start, p->end);
		const bool hasmatch = (sr.error_code >= 0);
		if (hasmatch == m_reverse) continue;

		m_editor.AddSelection(p->start, p->end);
		m_editor.SetPos(p->end);
	}

	m_editor.DrawLayout();
	Clear();
}

void CommandHandler::NextMatch(size_t count, bool forward) {
	wxString pattern;
	bool regex = true;
	bool select = true;
	bool usesel = false;
	size_t pos = m_editor.GetPos();

	// Get pattern and startpos
	if (m_editor.IsSelected()) {
		if (!m_search.empty() && m_lastSearchPos == pos) {
			pattern = m_search;
		}
		else {
			// search for selection content 
			pattern = m_editor.GetFirstSelection();
			usesel = true;
			regex = false;
		}
		pos = m_editor.GetSelections()[0].end;
	}
	else if (!m_search.empty()) {
		pattern = m_search;
		select = false;
	}
	else return; // nothing to search after

	// Set options
	int options = FIND_MATCHCASE;
	if (regex) options |= FIND_USE_REGEX;
	if (!forward) options |= FIND_REVERSE;

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();

		// Copy old selections before removing them
		const vector<interval> selections = m_editor.GetSelections();
		m_editor.RemoveAllSelections();
	
		// Go to next match in each range
		for (size_t i = 0; i < ranges.size(); ++i) {
			const interval& iv = ranges[i];
			const size_t cpos = cursors[i];

			// Get pattern from selection if no search pattern
			if (usesel) {
				bool haspattern = false;
				for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
					if (p->start == cpos || p->end == cpos) {
						pattern = m_editor.GetText(p->start, p->end);
						haspattern = true;
						break;
					}
				}
				if (!haspattern) continue;
			}

			// Do the search
			size_t startpos = cpos;
			size_t endpos = cpos;
			const size_t limit = forward ? iv.end : iv.start;
			for (size_t n = 0; n < count; ++n) {
				const size_t searchpos = forward ? endpos : startpos;
				const search_result sr = m_editor.SearchDirect(pattern, options, searchpos, limit);
				if (sr.error_code < 0) return; // no matches
				if (!select && n == 0 && sr.start == pos) ++count; // avoid reselect
				
				startpos = sr.start;
				endpos = sr.end;
			}

			// Show result
			if (select)	m_editor.AddSelection(startpos, endpos);
			const size_t newcpos = select ? endpos : startpos;
			if (cpos == pos) {
				m_editor.SetPos(newcpos);
				m_lastSearchPos = newcpos;
			}
			m_editor.SetSearchRangeCursor(i, newcpos);
		}
	}
	else {
		// Do the search
		size_t startpos = pos;
		size_t endpos = pos;
		for (size_t i = 0; i < count; ++i) {
			const size_t searchpos = forward ? endpos : startpos;
			const search_result sr = m_editor.SearchDirect(pattern, options, searchpos);
			if (sr.error_code < 0) return; // no matches
			if (!select && i == 0 && sr.start == pos) ++count; // avoid reselect
			
			startpos = sr.start;
			endpos = sr.end;
		}

		// Show result
		if (select) m_editor.Select(startpos, endpos);
		const size_t newcpos = select ? endpos : startpos;
		m_editor.SetPos(newcpos);
		m_lastSearchPos = newcpos;
	}

	m_editor.SetSearchHighlight(pattern, regex ? FIND_USE_REGEX : 0);
}

template<class Op> void CommandHandler::DoMovement(size_t count, const Op& f, bool redraw) {
	const unsigned int pos = m_editor.GetPos();

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();
	
		// Do movement in each range
		unsigned int caretPos = 0;
		for (size_t i = 0; i < ranges.size(); ++i) {
			const interval& iv = ranges[i];
			const unsigned int startpos = cursors[i];
			
			m_editor.SetPos(startpos);
			for (size_t n = 0; n < count; ++n) f(m_editor);

			// Limit movement to range
			unsigned int endpos = m_editor.GetPos();
			if (endpos < iv.start) endpos = iv.start;
			if (endpos > iv.end) endpos = iv.end;
			m_editor.SetSearchRangeCursor(i, endpos);

			if (m_state == state_range) {
				// TODO: sel_id in range
				m_editor.ExtendSelectionToLine();
			}
			else if (m_select) m_editor.SelectFromMovement(startpos, endpos, false, true);
			else m_editor.RemoveAllSelections();

			if (startpos == pos) caretPos = endpos;
		}

		// Make sure main caret ends up the right spot
		m_editor.SetPos(caretPos);
	}
	else {
		for (size_t i = 0; i < count; ++i) {
			f(m_editor);
		}

		if (m_state == state_range && m_editor.IsSelected()) {
			m_editor.ExtendSelectionToLine();
		}
		else if (m_select) m_editor.SelectFromMovement(pos, m_editor.GetPos());
		else m_editor.RemoveAllSelections();
	}

	EndMovement(redraw);
}

void CommandHandler::DoSelectObject(bool inclusive, bool all, const TextObjectTraverser& getnextobject) {
	interval iv;
	interval iv_inner;
	const interval& v = inclusive ? iv : iv_inner;

	// We need to be aware of ranges during object searches
	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();
		const unsigned int caretpos = m_editor.GetPos();

		for (size_t i = 0; i < ranges.size(); ++i) {
			const interval& range = ranges[i];
			const unsigned int startpos = all ? range.start : cursors[i];
			unsigned int pos = startpos;

			while (getnextobject(m_editor, pos, iv, iv_inner)) {
				if (iv.start < range.start) {pos = iv.end; continue;}
				if (iv.end > range.end) break;

				m_editor.AddSelection(v.start, v.end, true);
				pos = iv.end;  // continue search from outer end
				if (!all) break;
			}

			// Set cursors
			if (pos != startpos) {
				m_editor.SetSearchRangeCursor(i, v.end);
				if (startpos == caretpos) m_editor.SetPos(v.end); // real caret follow
			}
		}
	}
	else {
		if (all) {	
			unsigned int pos = 0;
			while (getnextobject(m_editor, pos, iv, iv_inner)) {
				const interval& v = inclusive ? iv : iv_inner;
				m_editor.AddSelection(v.start, v.end, true);
				pos = iv.end; // continue search from outer end
			}
			if (pos != 0) m_editor.SetPos(v.end);
		}
		else if (getnextobject(m_editor, m_editor.GetPos(), iv, iv_inner)) {
			m_editor.AddSelection(v.start, v.end, true);
			m_editor.SetPos(v.end);
		}
	}
}



void CommandHandler::SelectRangeStart() {
	m_state = state_range;

	if (m_editor.IsSelected()) {
		// Select selections
	}
	else if (m_editor.HasSearchRange()) {
		// Select lines in ranges (relative to range top)
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();
		const unsigned int caretpos = m_editor.GetPos();

		for (unsigned i = 0; i < ranges.size(); ++i) {
			const interval& range = ranges[i];
			const unsigned int topline = m_editor.GetLineFromPos(range.start);
			const unsigned int selline = topline + (m_count ? m_count-1 : 0);
			if (selline >= m_editor.GetLineCount()) break;

			interval iv = m_editor.GetLineExtent(selline);
			if (iv.start >= range.end) continue;

			// Select the line
			iv.start = wxMax(range.start, iv.start);
			iv.end = wxMin(range.end, iv.end);
			m_editor.AddSelection(iv.start, iv.end);

			// Set cursors
			if (cursors[i] == caretpos) m_editor.SetPos(iv.end); // real caret follow
			m_editor.SetSearchRangeCursor(i, iv.end);
		}
		EndMovement();
	}
	else {
		// Select lines
		if (m_count) m_editor.SelectLine(m_count-1);
		else m_editor.SelectCurrentLine();
		EndMovement();
	}
}

void CommandHandler::SelectRangeEnd() {
	if (!m_editor.IsSelected()) return;
	if (m_count == 0) return;

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();
		const unsigned int caretpos = m_editor.GetPos();

		for (unsigned i = 0; i < ranges.size(); ++i) {
			const interval& range = ranges[i];
			const unsigned int topline = m_editor.GetLineFromPos(range.start);
			const unsigned int selline = topline + (m_count-1);
			if (selline >= m_editor.GetLineCount()) break;

			interval iv = m_editor.GetLineExtent(selline);
			if (iv.start >= range.end) continue;

			// Select the lines
			iv.start = wxMax(range.start, iv.start);
			iv.end = wxMin(range.end, iv.end);
			m_editor.AddSelection(iv.start, iv.end);

			// Set cursors
			if (cursors[i] == caretpos) m_editor.SetPos(iv.end); // real caret follow
			m_editor.SetSearchRangeCursor(i, iv.end);
		}
	}
	else {
		m_editor.CursorToLine(m_count);
		m_editor.ExtendSelectionToLine();
	}
}

void CommandHandler::SelectObject(wxChar c, size_t count, bool inclusive, bool all) {
	switch (c) {
	case 'c':
		DoSelectObject(inclusive, all, bindtraverser(&EditorCtrl::GetNextObjectScope, wxT("comment")));
		break;
	case 'w':
		DoSelectObject(inclusive, all, bindtraverser(&EditorCtrl::GetNextObjectWords, count));
		//m_editor.SelectWords(all ? 0 : count, inclusive);
		break;
	case '(':
	case '{':
	case '[':
	case '<':
		if (all) {
			DoSelectObject(inclusive, true, bindtraverser(&EditorCtrl::GetNextObjectBlock, c));
		}
		else {
			DoSelectObject(inclusive, false, bindtraverser(&EditorCtrl::GetContainingObjectBlock, c));
		}
		break;
	case '"':
	case '\'':
		if (all) {
			DoSelectObject(inclusive, true, bindtraverser(&EditorCtrl::GetNextObjectBlock, c));
		}
		else {
			DoSelectObject(inclusive, false, bindtraverser(&EditorCtrl::GetContainingObjectString, c));
		}
		break;
	case 's':
		DoSelectObject(inclusive, all, bindtraverser(&EditorCtrl::GetNextObjectSentence));
		break;
	case 'p':
		DoSelectObject(inclusive, all, bindtraverser(&EditorCtrl::GetNextObjectParagraph));
		break;
	case WXK_ESCAPE:
		Clear();
		break;

	default:
		break;
	}
}

void CommandHandler::SelectLikeMatch() {
	if (!m_editor.IsSelected()) return;
	const size_t pos = m_editor.GetPos();

	// Get the pattern to search for
	wxString pattern = m_search;
	bool regex = true;
	const bool issearch = (m_search.empty() || m_lastSearchPos != pos);
	if (issearch) {
		// search for selection content 
		pattern = m_editor.GetFirstSelection();
		regex = false;
	}
	if (pattern.empty()) return;

	m_editor.RemoveAllSelections();

	// Set options
	int options = FIND_MATCHCASE;
	if (regex) options |= FIND_USE_REGEX;

	if (m_editor.HasSearchRange()) {
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();

		// Select all matches in each range
		for (size_t i = 0; i < ranges.size(); ++i) {
			const interval& iv = ranges[i];
			const unsigned int cpos = cursors[i];

			// Do the search
			size_t searchpos = iv.start;
			for (;;) {
				const search_result sr = m_editor.SearchDirect(pattern, options, searchpos, iv.end);
				if (sr.error_code < 0) break; // no more matches

				m_editor.AddSelection(sr.start, sr.end);
				m_editor.SetSearchRangeCursor(i, sr.end);
				if (pos == cpos) {
					m_editor.SetPos(sr.end);
					if (issearch) m_lastSearchPos = sr.end;
				}

				searchpos = sr.end;
				if (sr.start == sr.end) ++searchpos; // avoid loop on zero-len matches
			}
		}
	}
	else {
		size_t searchpos = 0;
		for (;;) {
			const search_result sr = m_editor.SearchDirect(pattern, options, searchpos);
			if (sr.error_code < 0) break; // no more matches

			m_editor.AddSelection(sr.start, sr.end);
			m_editor.SetPos(sr.end);
			if (issearch) m_lastSearchPos = sr.end;

			searchpos = sr.end;
			if (sr.start == sr.end) ++searchpos; // avoid loop on zero-len matches
		}
	}
}

void CommandHandler::CursorToOpposite() {
	if (!m_editor.IsSelected()) return;

	const size_t pos = m_editor.GetPos();
	const vector<interval>& selections = m_editor.GetSelections();

	for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
		if (pos == p->start) {
			m_editor.SetPos(p->end);
			break;
		}
		if (pos == p->end) {
			m_editor.SetPos(p->start);
			break;
		}
	}
}

void CommandHandler::ReplaceChar(wxChar c) {
	if (m_editor.IsSelected()) return;
	
	if (m_editor.HasSearchRange()) {
		const unsigned int pos = m_editor.GetPos();
		const vector<interval>& ranges = m_editor.GetSearchRange();
		const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();

		int offset = 0;
		unsigned int caretpos = 0;
		for (unsigned i = 0; i < ranges.size(); ++i) {
			const interval& range = ranges[i];
			const unsigned int cpos = cursors[i] + offset;
			const bool iscaret = (cursors[i] == pos);

			// Get character extent
			m_editor.SetPos(cpos);
			m_editor.CursorRight(EditorCtrl::SEL_IGNORE);
			unsigned int endpos = m_editor.GetPos();
			if (endpos > range.end) endpos = range.end;
			
			// Replace
			if (cpos < endpos) offset -= m_editor.RawDelete(cpos, endpos);
			const unsigned int bytelen = m_editor.RawInsert(cpos, c);
			offset += bytelen;

			// Adjust carets
			const unsigned int newpos = cpos + bytelen;
			m_editor.SetSearchRangeCursor(i, newpos);
			if (iscaret) caretpos = newpos;

			// Adjust ranges if their sizes has changes
			const int diff = bytelen - (endpos - cpos);
			if (diff) m_editor.AdjustSearchRangeInsert(i, diff);
		}
		m_editor.SetPos(caretpos);
	}
	else {
		m_editor.Delete();
		m_editor.InsertChar(c);
	}

	m_editor.ReDraw();
	Clear();
}

bool CommandHandler::IsSelectionFromSearch() const {
	return (!m_search.empty() && m_lastSearchPos == m_editor.GetPos());
}

void CommandHandler::Replace(const wxChar c) {
	if (!m_editor.IsSelected()) return;

	// Get replacement pattern
	switch (c) {
	case WXK_RETURN:
		break;
	case WXK_ESCAPE:
		Clear();
		return;
	case WXK_BACK:
		if (!m_buffer.empty()) {
			m_buffer.erase(m_buffer.length()-1);
			m_cmd.erase(m_cmd.length()-1);
		}
		return;
	default:
		m_buffer += c;
		return;
	}

	// Did the selection come from a search
	const bool fromsearch = IsSelectionFromSearch(); 
	const vector<interval>& selections = m_editor.GetSelections();
	map<unsigned int,interval> captures;
	const bool hasranges = m_editor.HasSearchRange();
	const unsigned int pos = m_editor.GetPos();

	int offset = 0;
	for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
		const unsigned int start = p->start + offset;
		const unsigned int end = p->end + offset;
		const bool hascaret = (p->start == pos || p->end == pos);

		wxString rep;
		if (fromsearch) {
			// Redo the search to get captures
			captures.clear();
			const search_result sr = m_editor.RegExFind(m_search, start, true, &captures, end);
			if (sr.error_code < 0) continue;

			rep = m_editor.ParseReplaceString(m_buffer, captures);
		}
		const wxString& sub = fromsearch ? rep : m_buffer;

		m_editor.RawDelete(start, end);
		const unsigned int bytelen = m_editor.RawInsert(start, sub);
		const int diff = bytelen - (end - start);
		offset += diff;

		if (hasranges && diff) {
			const vector<interval>& ranges = m_editor.GetSearchRange();
			const vector<unsigned int>& cursors = m_editor.GetSearchRangeCursors();

			// Find range containing selection
			for (size_t i = 0; i < ranges.size(); ++i) {
				const interval& range = ranges[i];
				if (start >= range.start && end <= range.end) {
					m_editor.AdjustSearchRangeInsert(i, diff);

					const unsigned int cpos = cursors[i];
					if (p->start == cpos || p->end == cpos) {
						m_editor.SetSearchRangeCursor(i, start + bytelen);
					}
					break;
				}
			}
		}

		if (hascaret) m_editor.SetPos(start + bytelen);
	}
	
	m_editor.RemoveAllSelections();
	m_editor.ReDraw();
	Clear();
}

void CommandHandler::EnterSelections(size_t count) {
	// count indicates that we should enter capture
	if (count && IsSelectionFromSearch()) {
		const vector<interval> selections = m_editor.GetSelections(); // cache selections
		m_editor.RemoveAllSelections();

		map<unsigned int,interval> captures;
		const unsigned int pos = m_editor.GetPos();

		int offset = 0;
		for (vector<interval>::const_iterator p = selections.begin(); p != selections.end(); ++p) {
			const unsigned int start = p->start + offset;
			const unsigned int end = p->end + offset;
			const bool hascaret = (p->start == pos || p->end == pos);

			// Redo the search to get captures
			captures.clear();
			const search_result sr = m_editor.RegExFind(m_search, start, true, &captures, end);
			if (sr.error_code < 0) continue;

			map<unsigned int,interval>::const_iterator cap = captures.find(count);
			if (cap != captures.end()) {
				const interval& iv = cap->second;
				m_editor.AddSelection(iv.start, iv.end, true);
				if (hascaret) m_editor.SetPos(iv.start);
			}
		}
	}

	m_editor.SetSearchRange();
	Clear();
}