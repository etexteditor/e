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

	void Clear();
	bool IsSearching() const;

	bool ProcessCommand(const wxKeyEvent& evt);

	class TextObjectTraverser {
	public:
		virtual bool operator()(EditorCtrl& editor, size_t pos, interval& iv, interval& iv_inner) const = 0;
	};

private:
	void EndMovement();

	void DoSearch(size_t count, int keyCode, wxChar c);
	
	void SelectRangeStart();
	void SelectRangeEnd();
	void SelectObject(wxChar c, size_t count, bool inclusive=true, bool all=false);

	template<class Op> void DoMovement(size_t count, const Op& f);

	void DoSelectObject(bool inclusive, bool all, const TextObjectTraverser& getnextobject);

	enum State {
		state_normal,
		state_visual,
		state_delete,
		state_findchar,
		state_findchar_reverse,
		state_search,
		state_search_reverse,
		state_range,
		state_object,
		state_object_inner,
		state_select_all,
		state_select_all_object,
		state_select_all_object_inner
	};

	// Member variables
	EditorFrame& m_parentFrame;
	EditorCtrl& m_editor;

	// state
	State m_state;
	size_t m_count;
	size_t m_count2;
	bool m_select;
	wxString m_cmd;
	size_t m_searchPos;
	wxString m_search;
	std::vector<unsigned int> m_cursors;
	bool m_reverse;
};

#endif //__COMMANDHANDLER_H__