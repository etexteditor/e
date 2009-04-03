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

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "EditorCtrl.h"

// Pre-definitions
class EditorFrame;

class StatusBar : public wxStatusBar {
public:
	StatusBar(EditorFrame& parent, wxWindowID id);

private:
	void UpdateEditorCtrl();

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

	unsigned int m_line;
	unsigned int m_column;
	unsigned int m_tabWidth;
	bool m_isSoftTabs;

	// Editor state
	EditorCtrl* m_editorCtrl;
	int m_editorCtrlId;
	unsigned int m_changeToken;
	unsigned int m_pos;

	vector<Styler_Syntax::SymbolRef> m_symbols;
};

#endif // __STATUSBAR_H__
