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

#include "StatusBar.h"
#include "EditorFrame.h"
#include "BundleMenu.h"
#include <wx/fontmap.h>
#include "EditorCtrl.h"

// Menu id's
enum {
	MENU_TABS_2,
	MENU_TABS_3,
	MENU_TABS_4,
	MENU_TABS_8,
	MENU_TABS_OTHER,
	MENU_TABS_SOFT
};

BEGIN_EVENT_TABLE(StatusBar, wxStatusBar)
	EVT_IDLE(StatusBar::OnIdle)
	EVT_LEFT_DOWN(StatusBar::OnMouseLeftDown)
	EVT_MENU(MENU_TABS_2, StatusBar::OnMenuTabs2)
	EVT_MENU(MENU_TABS_3, StatusBar::OnMenuTabs3)
	EVT_MENU(MENU_TABS_4, StatusBar::OnMenuTabs4)
	EVT_MENU(MENU_TABS_8, StatusBar::OnMenuTabs8)
	EVT_MENU(MENU_TABS_OTHER, StatusBar::OnMenuTabsOther)
	EVT_MENU(MENU_TABS_SOFT, StatusBar::OnMenuTabsSoft)
	EVT_MENU_RANGE(5000, 6000, StatusBar::OnMenuGotoSymbol)
END_EVENT_TABLE()

StatusBar::StatusBar(EditorFrame& parent, wxWindowID id, TmSyntaxHandler& syntax_handler):
	wxStatusBar(&parent, id), 
	m_parentFrame(parent),
	m_syntax_handler(syntax_handler),
	m_editorCtrl(NULL) 
{
	const int widths[] = {160, 120, 100, -1, 150 };
	SetFieldsCount(WXSIZEOF(widths), widths);
}

void StatusBar::UpdateBarFromActiveEditor() {
/*

	Ask the parent frame, if the editor change state is not different, then leave.
	Get the active editor (a narrower interface to it, actually.)
	If NULL, then leave.
	
	Get the new change state.
	Proceed with work (updating the status bar panels.)

*/

	EditorCtrl* editorCtrl = m_parentFrame.GetEditorCtrl();
	if (!editorCtrl) return;

	const unsigned int changeToken = editorCtrl->GetChangeToken();
	const unsigned int pos = editorCtrl->GetPos();
	const int id = editorCtrl->GetId();

	// In rare cases a new editorCtrl may get same address as
	// a previous one, so we also track the window id.
	const bool newEditorCtrl = (editorCtrl != m_editorCtrl || id != m_editorCtrlId);

	// Only update if the editorCtrl has changed
	const bool updatePanels = newEditorCtrl || (m_changeToken != changeToken) || (m_pos != pos);
	if (!updatePanels) return;

	if (newEditorCtrl) m_changeToken = 0; // reset
	m_editorCtrl = editorCtrl;
	m_editorCtrlId = id;

	Freeze();
	if (editorCtrl) {
		// Get position info
		const unsigned int line = editorCtrl->GetCurrentLineNumber();
		const unsigned int column = editorCtrl->GetCurrentColumnNumber();

		// Caret position
		if (line != m_line || column != m_column) {
			SetStatusText(wxString::Format(wxT("Line: %u  Column: %u"), line, column), 0);

			m_line = line;
			m_column = column;
		}

		// Syntax
		if (GetStatusText(1) != editorCtrl->GetSyntaxName()) {
			SetStatusText(editorCtrl->GetSyntaxName(), 1);
		}

		// Only reload symbol list if doc has changed
		bool symbolsChanged = false;
		if (newEditorCtrl || m_changeToken != changeToken) {
			m_symbols.clear();
			if (editorCtrl->GetSymbols(m_symbols)) {
				m_changeToken = editorCtrl->GetChangeToken(); // Track change state (so we only update on change)
				symbolsChanged = true;
			}
		}

		// Symbols
		if (newEditorCtrl || symbolsChanged || m_pos != pos) {
			SetStatusText(wxEmptyString, 3);
			for (vector<SymbolRef>::reverse_iterator p = m_symbols.rbegin(); p != m_symbols.rend(); ++p) {
				if (m_pos >= p->start) {
					const SymbolRef& sr = *p;
					SetStatusText(editorCtrl->GetSymbolString(sr), 3);
					break;
				}
			}
		}

		// Encoding
		wxFontEncoding enc = editorCtrl->GetEncoding();
		if (enc == wxFONTENCODING_DEFAULT) enc = wxLocale::GetSystemEncoding();
		wxString encoding = wxFontMapper::GetEncodingName(enc).Lower();
		if (enc == wxFONTENCODING_UTF7 || enc == wxFONTENCODING_UTF8 || enc == wxFONTENCODING_UTF16LE ||
			enc == wxFONTENCODING_UTF16BE || enc == wxFONTENCODING_UTF32LE || enc == wxFONTENCODING_UTF32BE) {
			if (editorCtrl->GetBOM()) encoding += wxT("+bom");
		}
		wxTextFileType eol = editorCtrl->GetEOL();
		if (eol == wxTextFileType_None) eol = wxTextBuffer::typeDefault;
		if (eol == wxTextFileType_Dos) encoding += wxT(" crlf");
		else if (eol == wxTextFileType_Unix) encoding += wxT(" lf");
		else if (eol == wxTextFileType_Mac) encoding += wxT(" cr");

		SetStatusText(encoding, 4);

		m_pos = pos;
	}

	// Tabs
	const unsigned int tabWidth = m_parentFrame.GetTabWidth();
	const bool isSoftTabs = m_parentFrame.IsSoftTabs();
	if (tabWidth != m_tabWidth || isSoftTabs != m_isSoftTabs) {
		if (m_parentFrame.IsSoftTabs()) {
			SetStatusText(wxString::Format(wxT("Soft Tabs: %u"), tabWidth), 2);
		}
		else {
			SetStatusText(wxString::Format(wxT("Tab Size: %u"), tabWidth), 2);
		}

		m_tabWidth = tabWidth;
		m_isSoftTabs = isSoftTabs;
	}
	Thaw();
}

void StatusBar::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	UpdateBarFromActiveEditor();
}

void StatusBar::PopupSyntaxMenu(wxRect& menuPos) {
		const wxString& current = m_editorCtrl->GetSyntaxName();

		// Get syntaxes and sort
		vector<cxSyntaxInfo*> syntaxes = m_syntax_handler.GetSyntaxes();
		sort(syntaxes.begin(), syntaxes.end(), tmActionCmp());

		// Syntax submenu
		wxMenu syntaxMenu;
		for (unsigned int i = 0; i < syntaxes.size(); ++i) {
			const cxSyntaxInfo& si = *syntaxes[i];

			wxMenuItem* item = syntaxMenu.Append(new BundleMenuItem(&syntaxMenu, 1000+i, si, wxITEM_CHECK));
			if (si.name == current) item->Check();
		}

		PopupMenu(&syntaxMenu, menuPos.x, menuPos.y);
}

void StatusBar::OnMouseLeftDown(wxMouseEvent& event) {
	UpdateBarFromActiveEditor();
	wxASSERT(m_editorCtrl);

	const int x = event.GetX();
	const int y = event.GetY();
	wxRect syntaxRect;
	wxRect tabsRect;
	wxRect symbolsRect;
	wxRect encodingRect;
	GetFieldRect(1, syntaxRect);
	GetFieldRect(2, tabsRect);
	GetFieldRect(3, symbolsRect);
	GetFieldRect(4, encodingRect);

	if (syntaxRect.Contains(x, y)) {
		PopupSyntaxMenu(syntaxRect);
	}
	else if (tabsRect.Contains(x, y)) {
		// Create the tabs menu
		wxMenu tabsMenu;
		tabsMenu.Append(MENU_TABS_2, _("2"), wxEmptyString, wxITEM_CHECK);
		tabsMenu.Append(MENU_TABS_3, _("3"), wxEmptyString, wxITEM_CHECK);
		tabsMenu.Append(MENU_TABS_4, _("4"), wxEmptyString, wxITEM_CHECK);
		tabsMenu.Append(MENU_TABS_8, _("8"), wxEmptyString, wxITEM_CHECK);
		tabsMenu.Append(MENU_TABS_OTHER, _("Other..."), wxEmptyString, wxITEM_CHECK);
		tabsMenu.AppendSeparator();
		tabsMenu.Append(MENU_TABS_SOFT, _("Soft Tabs (Spaces)"), wxEmptyString, wxITEM_CHECK);

		const unsigned int tabWidth = m_parentFrame.GetTabWidth();
		if (tabWidth == 2) tabsMenu.Check(MENU_TABS_2, true);
		else if (tabWidth == 3) tabsMenu.Check(MENU_TABS_3, true);
		else if (tabWidth == 4) tabsMenu.Check(MENU_TABS_4, true);
		else if (tabWidth == 8) tabsMenu.Check(MENU_TABS_8, true);
		else {
			tabsMenu.SetLabel(MENU_TABS_OTHER, wxString::Format(wxT("Other (%u) ..."), tabWidth));
			tabsMenu.Check(MENU_TABS_OTHER, true);
		}

		if (m_parentFrame.IsSoftTabs()) tabsMenu.Check(MENU_TABS_SOFT, true);

		PopupMenu(&tabsMenu, tabsRect.x, tabsRect.y);
	}
	else if (symbolsRect.Contains(x, y) && m_editorCtrl) {
		wxMenu symbolsMenu;

		if (!m_symbols.empty()) {
			// Create the symbols menu
			unsigned int id = 5000; // menu range 5000-6000
			bool currentSet = false;
			for (vector<SymbolRef>::const_iterator p = m_symbols.begin(); p != m_symbols.end(); ++p) {
				const SymbolRef& sr = *p;
				wxString symbolString = m_editorCtrl->GetSymbolString(sr);
				if (symbolString.empty()) symbolString = wxT(" "); // menu name cannot be empty

				symbolsMenu.Append(id, symbolString, wxEmptyString, wxITEM_CHECK);

				// Select current
				if (!currentSet && m_pos < p->start) {
					if (p != m_symbols.begin()) {
						symbolsMenu.Check(id-1, true);
					}
					currentSet = true;
				}

				++id;
			}

			// Current symbol may be the last and therefore not checked above
			if (!currentSet && m_pos >= m_symbols.back().start) {
				symbolsMenu.Check(id-1, true);
			}
		}
		else {
			symbolsMenu.Append(0, _("No Symbols"), wxEmptyString);
			symbolsMenu.Enable(0, false);
		}

		PopupMenu(&symbolsMenu, symbolsRect.x, symbolsRect.y);
	}
	else if (encodingRect.Contains(x, y) && m_editorCtrl) {
		// Create the tabs menu
		wxMenu encMenu;
		m_parentFrame.CreateEncodingMenu(encMenu);
		m_parentFrame.UpdateEncodingMenu(encMenu);

		PopupMenu(&encMenu, encodingRect.x, encodingRect.y);
	}
	else event.Skip();
}

void StatusBar::OnMenuTabs2(wxCommandEvent& WXUNUSED(event)) {
	m_parentFrame.SetTabWidth(2);
}

void StatusBar::OnMenuTabs3(wxCommandEvent& WXUNUSED(event)) {
	m_parentFrame.SetTabWidth(3);
}

void StatusBar::OnMenuTabs4(wxCommandEvent& WXUNUSED(event)) {
	m_parentFrame.SetTabWidth(4);
}

void StatusBar::OnMenuTabs8(wxCommandEvent& WXUNUSED(event)) {
	m_parentFrame.SetTabWidth(8);
}

void StatusBar::OnMenuTabsOther(wxCommandEvent& WXUNUSED(event)) {
	OtherTabDlg dlg(m_parentFrame);
}

void StatusBar::OnMenuTabsSoft(wxCommandEvent& WXUNUSED(event)) {
	// WORKAROUND: event.IsChecked() gives wrong value
	const bool isSoftTabs = !m_parentFrame.IsSoftTabs();

	m_parentFrame.SetSoftTab(isSoftTabs);
}

void StatusBar::OnMenuGotoSymbol(wxCommandEvent& event) {
	const unsigned int ndx = event.GetId() - 5000;

	if (m_editorCtrl && ndx < m_symbols.size()) {
		m_editorCtrl->SetPos(m_symbols[ndx].start);
		m_editorCtrl->MakeCaretVisibleCenter();
		m_editorCtrl->ReDraw();
	}
}

// ---- OtherTabDlg --------------------------------------------------------------------------------

// Ctrl Ids
enum {
	CTRL_SLIDER
};

BEGIN_EVENT_TABLE(StatusBar::OtherTabDlg, wxDialog)
	EVT_COMMAND_SCROLL(CTRL_SLIDER, OtherTabDlg::OnSlider)
END_EVENT_TABLE()

StatusBar::OtherTabDlg::OtherTabDlg(EditorFrame& parent)
: wxDialog (&parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
  m_parentFrame(parent) {
	const unsigned int width = parent.GetTabWidth();

	SetTitle (_("Other Tab Size"));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxSlider* slider = new wxSlider(this, CTRL_SLIDER, width, 1, 32, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_AUTOTICKS);
	sizer->Add(slider, 1, wxEXPAND|wxALL, 10);

	sizer->Add(CreateButtonSizer(wxOK), 0, wxEXPAND|wxALL, 10);

	slider->SetFocus();
	SetSizerAndFit(sizer);
	SetSize(400, 50);
	Centre();

	ShowModal();
}

void StatusBar::OtherTabDlg::OnSlider(wxScrollEvent& event) {
	m_parentFrame.SetTabWidth(event.GetPosition());
}
