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

#ifndef __COMMANDHANDLER_H__
#define __COMMANDHANDLER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

// Pre-definitions
class EditorFrame;
class EditorCtrl;
class interval;

class CommandHandler {
public:
	CommandHandler(EditorFrame& parentFrame, EditorCtrl& editor);
	~CommandHandler() {};

	void Clear(bool cancel=false);
	bool IsSearching() const;
	bool InCommand() const;
	const wxString& GetCommandString() const;

	void PlayCommand(const wxString& cmd);
	bool ProcessCommand(const wxKeyEvent& evt, bool record=false);

	class TextObjectTraverser {
	public:
		virtual bool operator()(EditorCtrl& editor, size_t pos, interval& iv, interval& iv_inner) const = 0;
	};

private:
	void EndMovement(bool redraw=true);

	void DoSearch(size_t count, int keyCode, wxChar c);
	void DoSearchAll(int keyCode, wxChar c);
	void DoSearchLines(int keyCode, wxChar c);
	void DoScopeSearch(int keyCode, wxChar c);
	void NextMatch(size_t count, bool forward=true);
	
	
	bool IsSelectionFromSearch() const;
	void SelectRangeStart();
	void SelectRangeEnd();
	void SelectObject(wxChar c, size_t count, bool inclusive=true, bool all=false);
	void SelectLikeMatch();

	void CursorToOpposite();
	void FilterSelections(int keyCode, wxChar c);

	void EnterSelections(size_t count);

	void ReplaceChar(wxChar c);
	void Replace(wxChar c);

	template<class Op> void DoMovement(size_t count, const Op& f, bool redraw=true);

	void DoSelectObject(bool inclusive, bool all, const TextObjectTraverser& getnextobject);

	enum State {
		state_normal,
		state_visual,
		state_delete,
		state_change,
		state_copy,
		state_findchar,
		state_findchar_reverse,
		state_search,
		state_search_reverse,
		state_search_all,
		state_search_lines,
		state_range,
		state_object,
		state_object_inner,
		state_select_all,
		state_select_all_object,
		state_select_all_object_inner,
		state_filter_sels,
		state_scopesearch,
		state_replace,
		state_replacechar,
		state_extracmds,
	};

	// Member variables
	EditorFrame& m_parentFrame;
	EditorCtrl& m_editor;

	// state
	State m_state;
	State m_endState;
	size_t m_count;
	size_t m_count2;
	bool m_select;
	wxString m_cmd;
	size_t m_searchPos;
	size_t m_lastSearchPos;
	wxString m_search;
	wxString m_buffer;
	std::vector<unsigned int> m_cursors;
	bool m_reverse;
	bool m_isRecording;
	bool m_isReplaying;
};

#endif //__COMMANDHANDLER_H__