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

#include "BundleMenu.h"
#include "tmAction.h"

#ifdef __WXGTK__
#include <gtk/gtk.h>

void BundleMenuItem::AfterInsert(void) {
     GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
     gtk_widget_show(hbox);
     GtkWidget *menu = GetMenuItem();
     GtkWidget *old_label = gtk_bin_get_child(GTK_BIN(menu));
     if (old_label)
     {
         g_object_ref(old_label);
         gtk_container_remove(GTK_CONTAINER(menu), old_label);
         gtk_misc_set_alignment(GTK_MISC(old_label), 0.0, 0.5);
         gtk_container_add(GTK_CONTAINER(hbox), old_label);
         g_object_unref(old_label);
     }
     gtk_container_add(GTK_CONTAINER(menu), hbox);

	if ( !m_action.trigger.empty() )
    {
		const wxString trig =  m_action.trigger + wxT(" \x21E5");

        GtkWidget* label = gtk_label_new(trig.utf8_str());
        gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
        gtk_misc_set_padding(GTK_MISC(label), 8, 1);
        gtk_widget_show(label);
        gtk_container_add(GTK_CONTAINER(hbox), label);

	}
	else if (!m_action.key.shortcut.empty()) {
		const wxString& shortcut = m_action.key.shortcut;

        GtkWidget* label = gtk_label_new(shortcut.utf8_str());
        gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
        gtk_misc_set_padding(GTK_MISC(label), 5, 0);
        gtk_widget_show(label);
        gtk_container_add(GTK_CONTAINER(hbox), label);
    }
}
#endif

BundleMenuItem::BundleMenuItem(wxMenu* parentMenu, int id, const tmAction& action, wxItemKind kind):
	wxMenuItem(parentMenu, id, action.name, action.name, kind), 
	m_action(action) 
{
#ifdef __WXMSW__
	SetOwnerDrawn(true);

	// It is bad to hardcode this but default looks really bad
	SetMarginWidth(10);

	// We need a unicode font
	const unsigned int fontsize = GetFontToUse().GetPointSize() * 0.9;
	m_unifont = wxFont(fontsize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
		false, wxT("Lucida Sans Unicode"));
#endif

	// We need to escape any ambersands in the label
	// (otherwise they will be interpreted as shortcuts)
	wxString label = action.name;
	label.Replace(wxT("&"), wxT("&&"));
	SetItemLabel(label);
}

#ifdef __WXMSW__
bool BundleMenuItem::OnMeasureItem(size_t *pwidth, size_t *pheight) {
	wxMenuItem::OnMeasureItem(pwidth, pheight);

	// Increase width to make room for shortcut/trigger
	if (!m_action.trigger.empty()) {
		const wxString trig =  m_action.trigger + wxT("\x21E5");

		wxMemoryDC dc;
		dc.SetFont(m_unifont);
		wxCoord w; wxCoord h;
        dc.GetTextExtent(trig, &w, &h);

		*pwidth += w + 20;
	}
	else if (!m_action.key.shortcut.empty()) {
		const wxString& shortcut = m_customAccel.empty() ? m_action.key.shortcut : m_customAccel;

		wxMemoryDC dc;
		dc.SetFont(GetFontToUse());

		wxCoord w; wxCoord h;
        dc.GetTextExtent(shortcut, &w, &h);

		*pwidth += w + 10;
	}

	return true;
}

bool BundleMenuItem::OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat) {
	wxMenuItem::OnDrawItem(dc, rc, act, stat);

	// we do nothing on focus change
    if ( act == wxODFocusChanged ) return true;

	if ( !m_action.trigger.empty() )
    {
		const wxString trig =  m_action.trigger + wxT("\x21E5");

		dc.SetFont(m_unifont);
		int trig_width, trig_height;
        dc.GetTextExtent(trig, &trig_width, &trig_height);

		// Draw a grey rounded rect as background for trigger
		const unsigned int bg_height = trig_height;
		const unsigned int bg_width = trig_width + 10;
		const unsigned int bg_xpos = rc.GetWidth()-16-bg_width;
		const unsigned int bg_ypos = rc.y+ (int)((rc.GetHeight()-bg_height)/2.0);
		dc.SetPen(*wxLIGHT_GREY_PEN);
		dc.SetBrush(*wxLIGHT_GREY_BRUSH);
		dc.DrawRoundedRectangle(bg_xpos, bg_ypos, bg_width, bg_height, 2);

		// Draw the trigger text
		dc.DrawText(trig, bg_xpos + 5, bg_ypos - 1 );
	}
	else if (!m_action.key.shortcut.empty()) {
		const wxString& shortcut = m_customAccel.empty() ? m_action.key.shortcut : m_customAccel;
		
		dc.SetFont(GetFontToUse());
		int accel_width, accel_height;
        dc.GetTextExtent(shortcut, &accel_width, &accel_height);

		HDC hdc = GetHdcOf(dc);
		const int nPrevMode = SetBkMode(hdc, TRANSPARENT);

		// set the colors
		// --------------
		DWORD colText;
		if ( stat & wxODSelected )
		{
			colText = GetSysColor((stat & wxODDisabled) ? COLOR_GRAYTEXT : COLOR_HIGHLIGHTTEXT);
		}
		else
		{
			// fall back to default colors if none explicitly specified
			colText = GetSysColor(COLOR_MENUTEXT);
		}
		const COLORREF colOldText = ::SetTextColor(hdc, colText);

		// right align accel string with right edge of menu ( offset by the
        // margin width )
		::DrawState(hdc, NULL, NULL,
                (LPARAM)shortcut.c_str(), shortcut.length(),
                rc.GetWidth()-16-accel_width, rc.y+(int) ((rc.GetHeight()-accel_height)/2.0),
                0, 0,
                DST_TEXT |
                (((stat & wxODDisabled) && !(stat & wxODSelected)) ? DSS_DISABLED : 0));

		// Restore state
		(void)SetBkMode(hdc, nPrevMode);
		::SetTextColor(hdc, colOldText);
    }

	return true;
}
#endif


// ---- PopupMenuItem ---------------------------------------------------------------

PopupMenuItem::PopupMenuItem(wxMenu* parentMenu, int id, const wxString& text)
: wxMenuItem(parentMenu, id, text, text) {
#ifdef __WXMSW__
	SetOwnerDrawn(true);

	// It is bad to hardcode this but default looks really bac
	SetMarginWidth(10);

	// We need to strip '&' from shortcut text
	// This is done to allow the accelerator shortcuts to
	// be used as in menu shortcuts (usually with numbers).
	if (!m_strAccel.empty()) {
		m_strAccel.Replace(wxT("&"), wxEmptyString);
	}
#endif
}
