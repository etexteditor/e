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

#include "CloseButton.h"
#include "x.xpm"

#ifdef __WXMSW__
    #if wxUSE_UXTHEME
        #include "wx/msw/uxtheme.h" // XP theme handling
    #endif
#endif

BEGIN_EVENT_TABLE(CloseButton, wxControl)
	EVT_ERASE_BACKGROUND(CloseButton::OnEraseBackground)
	EVT_PAINT(CloseButton::OnPaint)
	EVT_BUTTON(5, CloseButton::OnBitmapButton)
	EVT_SET_FOCUS(CloseButton::OnSetFocus)
	EVT_MOUSE_EVENTS(CloseButton::OnMouseEvent)
	EVT_SYS_COLOUR_CHANGED(CloseButton::OnSysColourChanged)
END_EVENT_TABLE()

CloseButton::CloseButton(wxWindow* parent, wxWindowID id, const wxPoint& pos)
: wxControl(parent, id, pos, wxSize(-1, 24), wxNO_BORDER|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE),
  m_bitmapButton(NULL), m_state(1), m_acceptFocus(true)
{
	Init();
}

void CloseButton::Init() {
	bool usingTheme = false;

#ifdef __WXMSW__
#if wxUSE_UXTHEME
    if (wxUxThemeEngine::Get())
    {
		wxUxThemeHandle hTheme(this, L"WINDOW");
        if (hTheme)
        {
			usingTheme = true;

			// Theme may just have been activated
			if (m_bitmapButton) {
				m_bitmapButton->Destroy();
				m_bitmapButton = NULL;
			}

			wxClientDC dc(this);
			HDC hdc = GetHdcOf(dc);
			wxSize size(13, 15);
			SIZE sz;
			wxUxThemeEngine::Get()->GetThemePartSize(hTheme, hdc, 19 /*WP_SMALLCLOSEBUTTON*/,
				1 /* CBS_NORMAL */, NULL, TS_TRUE, &sz);
			SetSize(wxSize(sz.cx, sz.cy));
		}
	}
#endif // wxUSE_UXTHEME
#endif // __WXMSW__

	if (usingTheme == false && m_bitmapButton == NULL) {
		m_bitmapButton = new wxBitmapButton(this, 5, wxBitmap(x_xpm), wxPoint(0,0));
		wxSize size = m_bitmapButton->GetSize();
		SetSize(size);
	}

	// Make sure the sizers get correct resize info
	SetSizeHints(GetSize(), GetSize());
}


void CloseButton::SetAcceptFocus(bool do_accept) {
	m_acceptFocus = do_accept;
}

/* States:
CBS_NORMAL = 1,
CBS_HOT = 2,
CBS_PUSHED = 3,
CBS_DISABLED = 4
*/
void CloseButton::DrawButton(wxDC& dc) {
#ifdef __WXMSW__
#if wxUSE_UXTHEME
	bool usingTheme = false;
    if (wxUxThemeEngine::Get())
    {
        wxUxThemeHandle hTheme(this, L"WINDOW");
        if (hTheme)
        {
			usingTheme = true;

			// Draw the themed close button
			RECT rc;
			wxCopyRectToRECT(GetClientRect(), rc);

			wxUxThemeEngine::Get()->DrawThemeBackground(hTheme, GetHdcOf(dc), 19 /*WP_SMALLCLOSEBUTTON*/,
                	m_state, &rc, NULL);

		}
	}
#endif // wxUSE_UXTHEME
#endif // __WXMSW__
}

void CloseButton::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
}

void CloseButton::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	DrawButton(dc);
}

void CloseButton::OnBitmapButton(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_bitmapButton);

	wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED , GetId() );
	GetEventHandler()->ProcessEvent( event );
}

void CloseButton::OnSetFocus(wxFocusEvent &event) {
	if (m_acceptFocus) {
		if (m_bitmapButton) m_bitmapButton->SetFocus();
		event.Skip();
	}
	else {
		// Don't accept focus ( remember: no event.Skip() )
		if (event.GetWindow() != NULL) event.GetWindow()->SetFocus();
	}
}

void CloseButton::OnMouseEvent(wxMouseEvent& event) {
#ifdef __WXMSW__
#if wxUSE_UXTHEME
    bool usingTheme = false;
    if (wxUxThemeEngine::Get())
    {
		wxUxThemeHandle hTheme(this, L"WINDOW");
        if (hTheme)	usingTheme = true;
	}

	if (usingTheme) {
		wxClientDC dc(this);

		if (event.LeftDown()) {
			m_state = 3; // CBS_PUSHED
			DrawButton(dc);
		}
		else if (event.LeftUp()) {
			m_state = 1; // CBS_NORMAL

			wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED , GetId() );
			GetEventHandler()->ProcessEvent( event );
			DrawButton(dc);
		}
		else if (event.Entering()) {
			m_state = 2; // CBS_HOT
			DrawButton(dc);
		}
		else if (event.Leaving()) {
			m_state = 1; // CBS_NORMAL
			DrawButton(dc);
		}
	}
#endif // wxUSE_UXTHEME
#endif // __WXMSW__
}

void CloseButton::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event)) {
	// This is probably the user having changed the theme (under XP)
	Init();
}
