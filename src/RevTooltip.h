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

#ifndef __REVTOOLTIP_H__
#define __REVTOOLTIP_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include "wx/popupwin.h"

class RevTooltip : public wxPopupWindow {
public:
	RevTooltip(const CatalystWrapper catalyst) : m_catalyst(catalyst) {};
	RevTooltip(wxWindow *parent, const CatalystWrapper catalyst);
	void Create(wxWindow *parent);

	void SetDocument(const doc_id& di, wxPoint pos);

private:
	class ColorBox : public wxWindow {
	public:
		ColorBox(wxWindow* parent, wxWindowID id);
	};

	const CatalystWrapper m_catalyst;
	doc_id m_docId;

	wxPanel* m_mainPanel;
	wxBoxSizer* m_mainSizer;
	wxBoxSizer* m_mirrorSizer;
	wxStaticBitmap* m_userPic;
	wxStaticText* m_userName;
	ColorBox* m_userColor;
	wxStaticText* m_dateCtrl;
	wxStaticText* m_descCtrl;
	wxStaticText* m_labelCtrl;
	wxStaticText* m_mirrorstatCtrl;
	wxStaticText* m_mirrorlistCtrl;
};

#endif // __REVTOOLTIP_H_
