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

#include "DiffPanel.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"
#include "DiffBar.h"
#include "DiffMarkBar.h"
#include <wx/gbsizer.h>

// Document Icons
#include "diff.xpm"

IMPLEMENT_DYNAMIC_CLASS(DiffPanel, wxPanel)

enum {
	ID_BUTTON_SWAP=100
};

BEGIN_EVENT_TABLE(DiffPanel, wxPanel)
	EVT_BUTTON(ID_BUTTON_SWAP, DiffPanel::OnButtonSwap)
	EVT_CHILD_FOCUS(DiffPanel::OnChildFocus)
END_EVENT_TABLE()

DiffPanel::DiffPanel(wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap):
	wxPanel(parent, wxID_ANY, wxPoint(-100,-100)), 
	m_parentFrame(&parentFrame), m_leftEditor(NULL), m_rightEditor(NULL), m_currentEditor(NULL) 
{
	Hide(); // Hidden during construction

	// Create ctrls
	m_leftEditor = new EditorCtrl(cw, bitmap, this, parentFrame);
	m_rightEditor = new EditorCtrl(cw, bitmap, this, parentFrame);
	m_leftEditor->SetScrollbarLeft();
	m_rightEditor->SetGutterRight();
	
	m_diffBar = new DiffBar(this, cw, m_leftEditor, m_rightEditor);
	m_leftMarkBar = new DiffMarkBar(this, m_diffBar->GetLineMatches(), m_leftEditor, true);
	m_rightMarkBar = new DiffMarkBar(this, m_diffBar->GetLineMatches(), m_rightEditor, false);

	m_leftTitle = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_rightTitle = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	wxButton* swapButton = new wxButton(this, ID_BUTTON_SWAP, wxT("<->"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	m_currentEditor = m_leftEditor; // default focus is left

	// Layout sizers
	m_mainSizer = new wxGridBagSizer();
	{
		m_mainSizer->AddGrowableRow(1);
		m_mainSizer->AddGrowableCol(1);
		m_mainSizer->AddGrowableCol(3);

		m_mainSizer->Add(m_leftTitle, wxGBPosition(0,0), wxGBSpan(1,2), wxEXPAND);
		m_mainSizer->Add(swapButton, wxGBPosition(0,2));
		m_mainSizer->Add(m_rightTitle, wxGBPosition(0,3), wxGBSpan(1,2), wxEXPAND);

		m_mainSizer->Add(m_leftMarkBar, wxGBPosition(1,0), wxGBSpan(1,1), wxEXPAND);
		m_mainSizer->Add(m_leftEditor, wxGBPosition(1,1), wxGBSpan(1,1), wxEXPAND);
		m_mainSizer->Add(m_diffBar, wxGBPosition(1,2), wxGBSpan(1,1), wxEXPAND);
		m_mainSizer->Add(m_rightEditor, wxGBPosition(1,3), wxGBSpan(1,1), wxEXPAND);
		m_mainSizer->Add(m_rightMarkBar, wxGBPosition(1,4), wxGBSpan(1,1), wxEXPAND);
	}

	SetSizer(m_mainSizer);
}

DiffPanel::DiffPanel(){}

DiffPanel::~DiffPanel() {
	// Make sure the editorCtrls get closed correctly
	// TODO: Better handling in EditorFrame
	m_leftEditor->Close();
	m_rightEditor->Close();
}

bool DiffPanel::Show(bool show) {
	const bool result = wxPanel::Show(show);

	if (m_leftEditor) {
		m_leftEditor->Show(show);
		m_leftEditor->EnableRedraw(show);
	}
	if (m_rightEditor) {
		m_rightEditor->Show(show);
		m_rightEditor->EnableRedraw(show);
	}

	if (show) {
		m_mainSizer->Layout();

		// Don't set callbacks until after initial display
		// otherwise it may jump to unintended position
		m_diffBar->SetCallbacks();
	}

	return result;
}

void DiffPanel::SetDiff(const wxString& leftPath, const wxString& rightPath) {
	m_leftEditor->OpenFile(leftPath, wxFONTENCODING_SYSTEM);
	m_rightEditor->OpenFile(rightPath, wxFONTENCODING_SYSTEM);

	m_leftTitle->SetValue(m_leftEditor->GetPath());
	m_rightTitle->SetValue(m_rightEditor->GetPath());

	m_diffBar->Init();
	m_diffBar->SetDiff();
}

bool DiffPanel::CmpPaths(const wxString& path1, const wxString& path2) const {
#ifdef __WXMSW__
	// paths on windows are case-insensitive
	if (path1.CmpNoCase(m_leftEditor->GetPath()) == 0 &&
		path2.CmpNoCase(m_rightEditor->GetPath()) == 0) return true;
	if (path2.CmpNoCase(m_leftEditor->GetPath()) == 0 &&
		path1.CmpNoCase(m_rightEditor->GetPath()) == 0) return true;
#else
	if (path1 == m_leftEditor->GetPath() &&
		path2 == m_rightEditor->GetPath()) return true;
	if (path2 == m_leftEditor->GetPath() &&
		path1 == m_rightEditor->GetPath()) return true;
#endif

	return false;
}

void DiffPanel::UpdateMarkBars() {
	m_leftMarkBar->Refresh();
	m_rightMarkBar->Refresh();
}

void DiffPanel::OnButtonSwap(wxCommandEvent& WXUNUSED(event)) {
	m_mainSizer->Detach(m_leftEditor);
	m_mainSizer->Detach(m_rightEditor);
	EditorCtrl* temp = m_leftEditor;
	m_leftEditor = m_rightEditor;
	m_rightEditor = temp;
	m_mainSizer->Add(m_leftEditor, wxGBPosition(1,1), wxGBSpan(1,1), wxEXPAND);
	m_mainSizer->Add(m_rightEditor, wxGBPosition(1,3), wxGBSpan(1,1), wxEXPAND);

	m_leftEditor->SetScrollbarLeft(true);
	m_rightEditor->SetScrollbarLeft(false);
	m_leftEditor->SetGutterRight(false);
	m_rightEditor->SetGutterRight(true);

	m_leftMarkBar->SetEditor(m_leftEditor);
	m_rightMarkBar->SetEditor(m_rightEditor);
	
	m_diffBar->Swap();
	
	Freeze();
	m_leftTitle->SetValue(m_leftEditor->GetPath());
	m_rightTitle->SetValue(m_rightEditor->GetPath());

	m_mainSizer->Layout();

	m_leftEditor->ReDraw();
	m_rightEditor->ReDraw();
	Thaw();
}

void DiffPanel::OnChildFocus(wxChildFocusEvent& event) {
	EditorCtrl* focused = (EditorCtrl*)event.GetWindow();
	if (focused != m_currentEditor) {
		if (focused == m_leftEditor) m_currentEditor = m_leftEditor;
		else if (focused == m_rightEditor) m_currentEditor = m_rightEditor;
		m_parentFrame->UpdateNotebook();
	}
}

void DiffPanel::SaveSettings(unsigned int i, eSettings& settings) {
	m_leftEditor->SaveSettings(i, settings, 1);
	m_rightEditor->SaveSettings(i, settings, 2);
}

void DiffPanel::RestoreSettings(unsigned int i, eSettings& settings) {
	m_leftEditor->RestoreSettings(i, settings, 1);
	m_rightEditor->RestoreSettings(i, settings, 2);

	m_leftTitle->SetValue(m_leftEditor->GetPath());
	m_rightTitle->SetValue(m_rightEditor->GetPath());

	m_diffBar->Init();
	m_diffBar->SetDiff();
}

const char** DiffPanel::RecommendedIcon() const {
	return diff_xpm;
}

EditorCtrl* DiffPanel::GetActiveEditor()
{
	return m_currentEditor;
}
