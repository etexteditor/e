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

#ifdef __WXMSW__
    #pragma warning(disable: 4786)
#endif

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
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

private:
	const tmAction& m_action;
	wxFont m_unifont;
};

class PopupMenuItem : public wxMenuItem {
public:
	PopupMenuItem(wxMenu* parentMenu, int id, const wxString& text);
};

#endif // __BUNDLEMENU_H__
