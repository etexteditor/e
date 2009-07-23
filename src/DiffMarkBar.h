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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "DiffBar.h"

#include <vector>

// pre-definitions
class EditorCtrl;

class DiffMarkBar : public wxControl {
public:
	DiffMarkBar(wxWindow* parent, const vector<DiffBar::LineMatch>& m_lineMatches, EditorCtrl* editor, bool isLeft);
	void SetEditor(EditorCtrl* editor);

private:
	void DrawLayout(wxDC& dc);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	const std::vector<DiffBar::LineMatch>& m_lineMatches;
	EditorCtrl* m_editor;
	bool m_isLeft;
	wxColour m_insColor;
	wxColour m_delColor;
	wxColour m_insBorder;
	wxColour m_delBorder;
};

#endif // __DIFFMARKBAR_H__
