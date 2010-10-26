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

#include "eDockArt.h"

#include <wx/renderer.h>

#ifdef __WXMSW__
#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h" // XP theme handling
#endif
#endif

ModernDockArt::ModernDockArt(wxWindow* win) : wxAuiDefaultDockArt(), m_win(win)
{
	ModernInit();
}

void ModernDockArt::ModernInit()
{
	m_real_button_size = m_button_size;

	// Get the size of a small close button (themed)
#ifdef __WXMSW__
#if wxUSE_UXTHEME
    bool usingTheme = false;
    if (wxUxThemeEngine::Get())
    {
		wxUxThemeHandle hTheme(m_win, L"WINDOW");
        if (hTheme)
        {
			usingTheme = true;

			wxClientDC dc(m_win);
			HDC hdc = GetHdcOf(dc);
			wxSize size(13, 15);
			SIZE sz;
			wxUxThemeEngine::Get()->GetThemePartSize(hTheme, hdc, 19 /*WP_SMALLCLOSEBUTTON*/,
				1 /* CBS_NORMAL */, NULL, TS_TRUE, &sz);

			m_real_button_size = sz.cx;
			m_button_size = m_real_button_size + 2; // add room for a border
		}
	}
#endif // wxUSE_UXTHEME
#endif // __WXMSW__

	//m_button_border_size = 3;
	m_caption_text_indent = 6;
	m_caption_size = 22;

	// We only highlight the active pane with the caption text being in bold.
	// So we do not want a special colour for active elements.
	m_active_caption_colour = m_inactive_caption_colour;
	m_active_close_bitmap = m_inactive_close_bitmap;
};

void ModernDockArt::DrawCaption(wxDC& dc,
						   wxWindow* WXUNUSED(window),
                           const wxString& text,
                           const wxRect& rect,
                           wxAuiPaneInfo& pane)
{
    dc.SetPen(*wxTRANSPARENT_PEN);

    DrawCaptionBackground(dc, rect,
                          (pane.state & wxAuiPaneInfo::optionActive)?true:false);

	// Active captions are drawn with bold text
	if (pane.state & wxAuiPaneInfo::optionActive)
        m_caption_font.SetWeight(wxFONTWEIGHT_BOLD);
    else
        m_caption_font.SetWeight(wxFONTWEIGHT_NORMAL);
	dc.SetFont(m_caption_font);
	dc.SetTextForeground(m_inactive_caption_text_colour);

    wxCoord w,h;
    dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

    dc.SetClippingRegion(rect);
    dc.DrawText(text, rect.x+m_caption_text_indent, rect.y+(rect.height/2)-(h/2)-1);
    dc.DestroyClippingRegion();
}

void ModernDockArt::DrawCaptionBackground(wxDC& dc,
									 const wxRect& rect,
									 bool active)
{
    // Clear the background
	dc.SetBrush(m_background_brush);
	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

	if (active)
	{
		wxRendererNative::Get().DrawHeaderButton(m_win, dc, rect, wxCONTROL_FOCUSED);
	}
	else
	{
		wxRendererNative::Get().DrawHeaderButton(m_win, dc, rect);
	}
}


void ModernDockArt::DrawPaneButton(wxDC& dc,
							  wxWindow* window,
							  int button,
							  int button_state,
							  const wxRect& _rect,
							  wxAuiPaneInfo& pane)
{
	bool usingTheme = false;

#ifdef __WXMSW__
#if wxUSE_UXTHEME
    if (wxUxThemeEngine::Get())
    {
        wxUxThemeHandle hTheme(m_win, L"WINDOW");
        if (hTheme)
        {
			usingTheme = true;

			// Get the real button position (compensating for borders)
			//const wxRect rect(_rect.x, _rect.y+m_button_border_size, m_button_size, m_button_size);
			const unsigned int ypos = _rect.y + (_rect.height/2) - (m_button_size/2); // center vertically
			const wxRect rect(_rect.x, ypos, m_real_button_size, m_real_button_size);

			// Draw the themed close button
			RECT rc;
			wxCopyRectToRECT(rect, rc);

			int state = 0;
			switch (button_state) {
			case wxAUI_BUTTON_STATE_NORMAL:
				state = 1; // CBS_NORMAL
				break;
			case wxAUI_BUTTON_STATE_HOVER:
				state = 2; // CBS_HOT
				break;
			case wxAUI_BUTTON_STATE_PRESSED:
				state = 3; // CBS_PUSHED
				break;
			default:
				wxASSERT_MSG(false, wxT("Unknown state"));
			}

			wxUxThemeEngine::Get()->DrawThemeBackground(hTheme, GetHdcOf(dc), 19 /*WP_SMALLCLOSEBUTTON*/,
                	state, &rc, NULL);

		}
	}
#endif // wxUSE_UXTHEME
#endif // __WXMSW__

	// Fallback to default closebutton if themes are not enabled
	if (!usingTheme) wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, _rect, pane);
}

// ---- ModernTabArt -----------------------------------------------------------------------

#include "images/close.xpm"
#include "images/close_disabled.xpm"
#include "images/close_down.xpm"
#include "images/close_hot.xpm"

ModernTabArt::ModernTabArt() {
	m_active_close_bmp = wxBitmap(close_xpm);
	/* FIXME:
	The correct fix will be to modify the ModernTabArt class to also draw buttons on mouseover. 
	m_hot_close_bmp = wxBitmap(close_hot_xpm);*/
    m_disabled_close_bmp = wxBitmap(close_disabled_xpm);
}

wxAuiTabArt* ModernTabArt::Clone()
{
    return static_cast<wxAuiTabArt*>(new ModernTabArt());
}
