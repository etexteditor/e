/*******************************************************************************
 *
 * Copyright (C) 2010, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __MACROPANE_H__
#define __MACROPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/grid.h>

class eMacro;
class EditorFrame;

class MacroPane : public wxPanel {
public:
	MacroPane(EditorFrame& frame, wxWindow* parent, eMacro& macro);

	void OnMacroChanged();

private:
	void UpdateButtons();
	void UpdateArgsGrid();

	// Event handlers
	void OnButtonRec(wxCommandEvent& evt);
	void OnButtonDel(wxCommandEvent& evt);
	void OnButtonUp(wxCommandEvent& evt);
	void OnButtonDown(wxCommandEvent& evt);
	void OnButtonSave(wxCommandEvent& evt);
	void OnCmdList(wxCommandEvent& evt);
	void OnGridChanged(wxGridEvent& evt);
	void OnIdle(wxIdleEvent& evt);
	DECLARE_EVENT_TABLE()

	// Ctrls
	wxBitmap m_bitmapStartRec;
	wxBitmap m_bitmapStopRec;
	wxBitmapButton* m_buttonRec;
	wxBitmapButton* m_buttonDel;
	wxBitmapButton* m_buttonUp;
	wxBitmapButton* m_buttonDown;
	wxBitmapButton* m_buttonSave;
	wxListBox* m_cmdList;
	wxGrid* m_argsGrid;

	// Member variabels
	EditorFrame& m_parentFrame;
	bool m_recState;
	eMacro& m_macro;
};

#endif //__MACROPANE_H__