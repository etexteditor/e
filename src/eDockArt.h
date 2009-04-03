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

#ifndef __EDOCKART_H__
#define __EDOCKART_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/aui/aui.h>
#include <wx/renderer.h>

#ifdef __WXMSW__
#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h" // XP theme handling
#endif
#endif

class ModernDockArt : public wxAuiDefaultDockArt
{
public:
	ModernDockArt(wxWindow* win);

	void DrawPaneButton(wxDC& dc,
				  wxWindow *window,
                  int button,
                  int button_state,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);

	void DrawCaption(wxDC& dc,
				  wxWindow *window,
                  const wxString& text,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);

protected:
	void ModernInit();
    void DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active);

	unsigned int m_caption_text_indent;
	unsigned int m_real_button_size;

private:
	wxWindow* m_win;
};

class ModernTabArt : public wxAuiDefaultTabArt {
public:
	ModernTabArt();
	wxAuiTabArt* Clone();
};

#endif // __EDOCKART_H__
