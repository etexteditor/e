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

#ifndef __CLOSEBUTTON_H__
#define __CLOSEBUTTON_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

class CloseButton : public wxControl {
public:
	CloseButton(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition);
	void SetAcceptFocus(bool do_accept);

private:
	void Init();
	void DrawButton(wxDC& dc);

	// event handlers
	void OnEraseBackground(wxEraseEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnBitmapButton(wxCommandEvent& evt);
	void OnSetFocus(wxFocusEvent &event);
	void OnMouseEvent(wxMouseEvent& event);
	void OnSysColourChanged(wxSysColourChangedEvent& event);
	DECLARE_EVENT_TABLE();

	// member variables
	wxBitmapButton *m_bitmapButton;
	int m_state;
	bool m_acceptFocus;
};

#endif // __CLOSEBUTTON_H__

