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

#ifndef __GUTTERCTRL_H__
#define __GUTTERCTRL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
        #include <wx/control.h>
        #include <wx/dc.h>
        #include <wx/dcclient.h>
        #include <wx/dcmemory.h>
#endif

struct tmTheme;
struct cxFold;
class EditorCtrl;

class GutterCtrl : public wxControl {
public:
	GutterCtrl(EditorCtrl& parent, wxWindowID id);
	void UpdateTheme(bool forceRecalculateDigitWidth=false);

	void SetGutterRight(bool doMove=true);

	unsigned int CalcLayout(unsigned int height);
	void DrawGutter() {wxClientDC dc(this);DrawGutter(dc);};
	void DrawGutter(wxDC& dc);

private:
	void ClickOnFold(unsigned int y);

	// Event Handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseLeftDClick(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnCaptureLost(wxMouseCaptureLostEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	EditorCtrl& m_editorCtrl;
	wxMemoryDC m_mdc;
	wxBitmap m_bitmap;
	int m_max_digits;
	int m_digit_width;
	unsigned int m_width;
	unsigned int m_numberX;
	unsigned int m_foldStartX;
	bool m_gutterLeft;

	bool m_showBookmarks;
	bool m_showFolds;
	wxBitmap m_bmBookmark;
	wxBitmap m_bmFoldOpen;
	wxBitmap m_bmFoldClosed;
	const cxFold* m_currentFold;
	int m_posBeforeFoldClick;

	const tmTheme& m_theme;
	wxColour m_numbercolor;
	wxColour m_edgecolor;
	wxColour m_hlightcolor;

	int m_currentSel;
	bool m_sel_startoutside;
	unsigned int m_sel_startline;
	unsigned int m_sel_endline;
};

#endif // __GUTTERCTRL_H__
