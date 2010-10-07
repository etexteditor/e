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

#ifndef __BUNDLEMENU_H__
#define __BUNDLEMENU_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// pre-definitions
class tmAction;

class BundleMenuItem : public wxMenuItem {
public:
	BundleMenuItem(wxMenu* parentMenu, int id, const tmAction& action, wxItemKind kind = wxITEM_NORMAL);

#ifdef __WXMSW__
	bool OnMeasureItem(size_t *pwidth, size_t *pheight);
	bool OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat);
#endif

	void SetCustomAccel(wxString accel) { m_customAccel = accel; }
	const tmAction& GetAction() { return m_action; }

#ifdef __WXGTK__
	void AfterInsert();
#else
	void AfterInsert() {}
#endif

private:
	const tmAction& m_action;
#ifdef __WXMSW__
	wxFont m_unifont;
	wxString m_customAccel;
#endif
};

class PopupMenuItem : public wxMenuItem {
public:
	PopupMenuItem(wxMenu* parentMenu, int id, const wxString& text);
};

#endif // __BUNDLEMENU_H__
