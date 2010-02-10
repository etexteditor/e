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

#include "RedoDlg.h"
#include "UndoHistory.h"
#include "EditorCtrl.h"
#include "EditorFrame.h"

RedoDlg::RedoDlg(EditorCtrl* parent, EditorFrame* frame, CatalystWrapper& cw, int editorId, const doc_id& di)
: wxDialog (parent, -1, _("Undo History"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {
	UndoHistory* undoHistory = new UndoHistory(cw, frame, editorId, this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	undoHistory->SetEditorCtrl(parent);
	undoHistory->Connect(wxEVT_CHAR, wxKeyEventHandler(RedoDlg::OnHistoryChar), NULL, this);
	undoHistory->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(RedoDlg::OnHistoryDClick), NULL, this);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(undoHistory, 1, wxEXPAND);

	SetSizer(mainSizer);
	SetSize(200, 300);
	Centre();

	// We have to set document after layout
	// to get correct position
	undoHistory->SetDocument(di, true);

	ShowModal();
}

void RedoDlg::OnHistoryChar(wxKeyEvent& event) {
	const int keycode = event.GetKeyCode();

	if (keycode == WXK_RETURN || keycode == WXK_ESCAPE) {
		Destroy(); // close dlg
		return;
	}

	event.Skip();
}

void RedoDlg::OnHistoryDClick(wxCommandEvent& WXUNUSED(event)) {
	Destroy(); // close dlg
}
