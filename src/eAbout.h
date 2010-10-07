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

#ifndef __EABOUT_H__
#define __EABOUT_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// Pre-declarations
class Catalyst;

class eAbout : public wxDialog {
public:
	eAbout(wxWindow *parent, const Catalyst& cat);

private:
	// Event handlers
	void OnBuy(wxCommandEvent& event);
	void OnRegister(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Embedded class: LogoPanel
	class LogoPanel : public wxPanel {
	public:
		LogoPanel(wxWindow *parent);

	private:
		// Event handlers
		void OnPaint(wxPaintEvent& event);
		DECLARE_EVENT_TABLE();

		// Member variables
		wxBitmap bitmap;
	};

	// Member variables
	bool m_needLicenseUpdate;
};

#endif // __EABOUT_H_
