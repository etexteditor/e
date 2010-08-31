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

#include "ClipboardHistoryPane.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"
#include "Strings.h"
#include "SearchListBox.h"

#include <algorithm>

#ifndef WX_PRECOMP
    #include <wx/sizer.h>
    #include <wx/dc.h>
#endif

using namespace std;

enum {
	CTRL_ACTION_LIST
};

BEGIN_EVENT_TABLE(ClipboardHistoryPane, wxPanel)
	EVT_LISTBOX_DCLICK(CTRL_ACTION_LIST, ClipboardHistoryPane::OnAction)
END_EVENT_TABLE()

ClipboardHistoryPane::ClipboardHistoryPane(EditorFrame& editorFrame, bool keepOpen):
	wxPanel(dynamic_cast<wxWindow*>(&editorFrame), wxID_ANY),
	m_parentFrame(editorFrame),
	m_keepOpen(keepOpen)
{
	// Create ctrls
	m_listBox = new ActionList(this, CTRL_ACTION_LIST, m_clipboardHistory);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_listBox, 1, wxEXPAND);

	SetSizer(mainSizer);
}

bool ClipboardHistoryPane::Destroy() {
	// delayed destruction: the panel will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

	return true;
}

void ClipboardHistoryPane::AddCopyText(wxString copytext) {
	if(m_clipboardHistory.size() > 0 && m_clipboardHistory[m_clipboardHistory.size()-1] == copytext) {
		return;
	}

	m_clipboardHistory.push_back(copytext);
	m_listBox->SetAllItems();
	m_listBox->SetSelection(m_clipboardHistory.size() - 1);
}

void ClipboardHistoryPane::OnAction(wxCommandEvent& WXUNUSED(event)) {
	if(m_listBox->GetSelectedCount() != 1) return;

	const int hit = m_listBox->GetSelection();
	if (hit != wxNOT_FOUND && hit < (int)m_clipboardHistory.size()) {
		m_parentFrame.GetEditorCtrl()->DoCopy(m_clipboardHistory[hit]);
	}
}


// --- ActionList --------------------------------------------------------

ClipboardHistoryPane::ActionList::ActionList(wxWindow* parent, wxWindowID id, std::vector<wxString>& clipboardHistory):
	SearchListBox(parent, id), 
	m_clipboardHistory(clipboardHistory)
{
	SetAllItems();
}

void ClipboardHistoryPane::ActionList::SetAllItems() {
	m_items.clear();

	Freeze();

	m_items.resize(m_clipboardHistory.size());
	for (unsigned int i = 0; i < m_clipboardHistory.size(); ++i) {
		m_items[i].id = i;
		m_items[i].text = &m_clipboardHistory[i];
	}

	SetItemCount(m_items.size());

	SetSelection(-1);
	RefreshAll();
	Thaw();
}

void ClipboardHistoryPane::ActionList::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const {
	const bool isCurrent = IsCurrent(n);

	if (isCurrent) dc.SetTextForeground(m_hlTextColor);
	else dc.SetTextForeground(m_textColor);

	const aItem& ai = m_items[n];
	const vector<unsigned int>& hl = ai.hlChars;
	const wxString& name = *ai.text;

	// Draw action name
	DrawItemText(dc, rect, name, hl, isCurrent);
}
