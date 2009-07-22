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

#include "CompletionPopup.h"
#include "EditorCtrl.h"

class CompletionList : public wxListBox {
public:
	CompletionList(wxDialog& parent, EditorCtrl& editorCtrl, const wxString& target, const wxArrayString& completions);
	~CompletionList();

private:
	void Update();
	void SetCompletions(const wxArrayString& completions);
	void EndCompletion() {m_parentDlg.Destroy();};

	void OnKillFocus(wxFocusEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	DECLARE_EVENT_TABLE();

	wxDialog& m_parentDlg;
	EditorCtrl& m_editorCtrl;
	wxString m_target;
	wxArrayString m_completions;
	wxListBox* m_listBox;
};


BEGIN_EVENT_TABLE(CompletionList, wxListBox)
	EVT_KILL_FOCUS(CompletionList::OnKillFocus)
	EVT_CHAR(CompletionList::OnChar)
	EVT_LEFT_DOWN(CompletionList::OnLeftDown)
END_EVENT_TABLE()

CompletionList::CompletionList(wxDialog& parent, EditorCtrl& editorCtrl, const wxString& target, const wxArrayString& completions):
	wxListBox(&parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, completions, wxSIMPLE_BORDER|wxLB_SINGLE|wxWANTS_CHARS),
	m_parentDlg(parent), m_editorCtrl(editorCtrl), m_target(target), m_completions(completions) 
{
	// editorCtrl has text cursor as default
	SetCursor(wxCursor(wxCURSOR_ARROW));

	// We want the editorCtrl to keep showing the caret
	m_editorCtrl.KeepCaretAlive();

	Show();
	if (!completions.IsEmpty()) SetSelection(0);
}

CompletionList::~CompletionList() {
	// Restore editorCtrls caret handling
	m_editorCtrl.KeepCaretAlive(false);
}

void CompletionList::OnKillFocus(wxFocusEvent& WXUNUSED(event)) {
	EndCompletion();
}

void CompletionList::OnLeftDown(wxMouseEvent& event) {
	const int hit = HitTest(event.GetPosition());

	if (hit != wxNOT_FOUND) {
		const wxString& word = GetString(hit);
		m_editorCtrl.ReplaceCurrentWord(word);
		m_editorCtrl.ReDraw();
		EndCompletion();
	}
}

void CompletionList::OnChar(wxKeyEvent& event) {
	const int keyCode = event.GetKeyCode();

	switch (keyCode) {
	case WXK_UP:
	case WXK_DOWN:
	case WXK_PAGEUP:
	case WXK_PAGEDOWN:
		// Let the control itself handle up/down
		event.Skip();
		return;

	case WXK_SPACE:
		if (event.ControlDown()) {
			// Cycle through options
			unsigned int sel = GetSelection();
			++sel;
			if (sel == GetCount()) sel = 0;
			SetSelection(sel);
			return;
		}
		// Fallthrough

	case WXK_TAB:
	case WXK_RETURN:
		// Select
		{
			const int sel = GetSelection();
			if (sel != wxNOT_FOUND) {
				const wxString& word = GetString(sel);
				m_editorCtrl.ReplaceCurrentWord(word);

				if (keyCode == WXK_SPACE) {
					m_editorCtrl.InsertChar(wxT(' '));
				}

				m_editorCtrl.ReDraw();
			}
			EndCompletion();
		}
		return;

	case WXK_ESCAPE:
		EndCompletion();
		return;
	}

	// Pass event on to editorCtrl
	m_editorCtrl.ProcessEvent(event);

	// Adapt list to new target
	const wxChar key = event.GetUnicodeKey();
	if (wxIsalnum(key) || key == wxT('_') || keyCode == WXK_BACK) {
		Update();
	}
	else EndCompletion(); // end completion
}

void CompletionList::Update() {
	const wxString target = m_editorCtrl.GetCurrentWord();

	if (target.empty()) EndCompletion();
	else {
		if (target.StartsWith(m_target)) {
			// Shrink the list to match new word
			wxArrayString completions;
			for (unsigned int i = 0; i < m_completions.GetCount(); ++i) {
				const wxString& word = m_completions[i];
				if (word.StartsWith(target)) completions.Add(word);
			}
			SetCompletions(completions);
		}
		else {
			// Build new completion list
			m_target = target;
			m_completions = m_editorCtrl.GetCompletionList();
			SetCompletions(m_completions);
		}
	}
}

void CompletionList::SetCompletions(const wxArrayString& completions) {
	if (completions.IsEmpty()) EndCompletion();
	else {
		Clear();
		InsertItems(completions, 0);
		SetSelection(0);
	}
}


CompletionPopup::CompletionPopup(EditorCtrl& parent, const wxPoint& pos, const wxPoint& topPos, const wxString& target, const wxArrayString& completions):
	wxDialog(&parent, wxID_ANY, wxEmptyString, pos, wxDefaultSize, wxNO_BORDER)
{
	CompletionList* clist = new CompletionList(*this, parent, target, completions);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(clist, 1, wxEXPAND);

	SetSizerAndFit(mainSizer);

	// Make sure that there is room for dialog
	const int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	const wxSize size = GetSize();
	if (pos.y + size.y > screenHeight) {
		Move(pos.x, topPos.y - size.y);
	}

	Show();
	clist->SetFocus();
}
