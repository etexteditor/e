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

#ifndef __DIFFMARKBAR_H__
#define __DIFFMARKBAR_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "DiffBar.h"

// STL can't compile with Level 4
#pragma warning(push, 1)
#include <vector>
#pragma warning(pop)
using namespace std;

// pre-definitions
class EditorCtrl;

class DiffMarkBar : public wxControl {
public:
	DiffMarkBar(wxWindow* parent, const vector<DiffBar::LineMatch>& m_lineMatches, EditorCtrl* editor, bool isLeft);
	
	void SetEditor(EditorCtrl* editor) {m_editor = editor;};

private:
	void DrawLayout(wxDC& dc);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	const vector<DiffBar::LineMatch>& m_lineMatches;
	EditorCtrl* m_editor;
	bool m_isLeft;
	wxColour m_insColor;
	wxColour m_delColor;
	wxColour m_insBorder;
	wxColour m_delBorder;
};


#endif // __DIFFMARKBAR_H__