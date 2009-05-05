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

#ifndef __STATUSBAR_H__
#define __STATUSBAR_H__

#include "EditorChangeState.h"

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".

#ifndef WX_PRECOMP
        #include <wx/statusbr.h>
        #include <wx/dialog.h>
#endif

#include "SymbolRef.h"
// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;


// Pre-definitions
class EditorFrame;
class EditorCtrl;
class TmSyntaxHandler;

class StatusBar : public wxStatusBar {
public:
	StatusBar(EditorFrame& parent, wxWindowID id, TmSyntaxHandler& syntax_handler);

private:
	void UpdateBarFromActiveEditor();
	void UpdateTabs();

	void PopupSyntaxMenu(wxRect& menuPos);

	void SetPanelTextIfDifferent(const wxString& newText, const int panelIndex);

	void OnIdle(wxIdleEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMenuTabs2(wxCommandEvent& event);
	void OnMenuTabs3(wxCommandEvent& event);
	void OnMenuTabs4(wxCommandEvent& event);
	void OnMenuTabs8(wxCommandEvent& event);
	void OnMenuTabsOther(wxCommandEvent& event);
	void OnMenuTabsSoft(wxCommandEvent& event);
	void OnMenuGotoSymbol(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	class OtherTabDlg : public wxDialog {
	public:
		OtherTabDlg(EditorFrame& parent);
	private:
		void OnSlider(wxScrollEvent& event);
		DECLARE_EVENT_TABLE();
		EditorFrame& m_parentFrame;
	};

	// Member variables
	EditorFrame& m_parentFrame;
	TmSyntaxHandler& m_syntax_handler;

	unsigned int m_line;
	unsigned int m_column;

	unsigned int m_tabWidth;
	bool m_isSoftTabs;

	// Editor state
	EditorCtrl* m_editorCtrl;
	EditorChangeState m_editorChangeState;
	int m_editorCtrlId;
	unsigned int m_changeToken;
	unsigned int m_pos;

	vector<SymbolRef> m_symbols;
};

#endif // __STATUSBAR_H__
