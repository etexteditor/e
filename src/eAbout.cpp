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

#include "eAbout.h"
#include "e.xpm"
#include <wx/statline.h>
#include "eApp.h"
#include "Catalyst.h"

BEGIN_EVENT_TABLE(eAbout, wxDialog)
	EVT_BUTTON(1, eAbout::OnBuy)
	EVT_BUTTON(2, eAbout::OnRegister)
END_EVENT_TABLE()

eAbout::eAbout(wxWindow *parent, const Catalyst& cat)
:  wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE) {
	const bool isregistered = cat.IsRegistered();

	SetTitle (_("About e"));

	// Logo panel
	LogoPanel *logopanel = new LogoPanel(this);

	// About text
	wxBoxSizer *infopane = new wxBoxSizer(wxVERTICAL);
	wxStaticText *appname;

	if (isregistered) appname = new wxStaticText (this, -1, _("Thanks for registering e"));
	else appname = new wxStaticText (this, -1, _("Thanks for evaluating e"));
	appname->SetFont(wxFont(20, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	infopane->Add(appname);
	infopane->Add(new wxStaticText(this, -1, _("Version: " + ((eApp*)wxTheApp)->m_version_name)));

	infopane->Add(0,10,1);

	wxString infotext;
	if (isregistered) infotext = _("e is a fast and elegant text editor with many innovative features.\n\nThanks for your registration of this unique tool. We hope you will have a lot of fun using it.\n\nWe are not a big corporation so we can assure you the fee you paid for the registration will go directly to the future software development.\n\nAgain thanks for your support.");
	else infotext = _("e is a fast and elegant text editor with many innovative features.\n\nThis is your 30 days evaluation of this unique tool. We hope you will have a lot of fun using it. You can decide to purchase a registration anytime, just press the 'Buy' button.\n\nWe are not a big corporation so we can assure you the fee you pay for the registration will go directly to the future software development.\n\nAgain thanks for your support.");

	wxTextCtrl* textctrl = new wxTextCtrl(this, -1, infotext, wxDefaultPosition, wxSize(350,150) , wxTE_MULTILINE|wxTE_READONLY);
	infopane->Add(textctrl);

	infopane->Add(0,10);
	infopane->Add(new wxStaticLine(this, -1), 0, wxEXPAND);
	infopane->Add(0,10);

	// Registration info
	infopane->Add(new wxStaticText(this, -1, _("This program is registered to:")));
	infopane->Add(0,10);
	if (isregistered) {
		infopane->Add(new wxStaticText(this, -1, cat.RegisteredUserName()), 0, wxALIGN_CENTER);
		infopane->Add(new wxStaticText(this, -1, cat.RegisteredUserEmail()), 0, wxALIGN_CENTER);
		infopane->Add(0,10);

		wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
		okButton->SetDefault();
		infopane->Add (okButton, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM|wxLEFT|wxBOTTOM, 10);
	}
	else {
		infopane->Add(new wxStaticText(this, -1, _("UNREGISTERED")), 0, wxALIGN_CENTER);

		wxString trialstate;
		int daysleft = cat.DaysLeftOfTrial();

		if (daysleft == 1) trialstate = _("1 day left of trial");
		else if (daysleft > 1) trialstate.Printf(_("%d days left of trial"), daysleft);
		else trialstate = _("* TRIAL EXPIRED *");

		wxStaticText* trialstatic = new wxStaticText(this, -1, trialstate);
		if (daysleft <= 0) {
			wxFont exp_font = trialstatic->GetFont();
			exp_font.SetWeight(wxBOLD);
			trialstatic->SetFont(exp_font);
			trialstatic->SetForegroundColour(*wxRED);
		}
		infopane->Add(trialstatic, 0, wxALIGN_CENTER);

		infopane->Add(0,10);

		wxGauge* daygauge = new wxGauge(this, -1, 30, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_PROGRESSBAR|wxGA_SMOOTH);
		daygauge->SetValue(30-daysleft);
		infopane->Add(daygauge, 0, wxEXPAND);
		infopane->Add(0,10);

		wxBoxSizer *buttons = new wxBoxSizer(wxHORIZONTAL);
		wxButton *buyButton = new wxButton (this, 1, _("BUY"));
		buyButton->SetDefault();
		buttons->Add(buyButton, 0, wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);
		buttons->Add(new wxButton(this, 2, _("Enter Reg. Code")), 0, wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);
		buttons->Add(new wxButton(this, wxID_OK, _("OK")), 0, wxALIGN_RIGHT|wxBOTTOM, 10);

		infopane->Add(buttons, 0, wxALIGN_RIGHT);
	}

	// Fit it all together
	wxBoxSizer *aboutpane = new wxBoxSizer(wxHORIZONTAL);
	aboutpane->Add(logopanel, 0, wxEXPAND);
	aboutpane->Add(10,0);
	aboutpane->Add(infopane, 1, wxEXPAND);
	aboutpane->Add(10,0);

	SetSizerAndFit(aboutpane);

	Centre();
}

void eAbout::OnBuy(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser(wxT("http://www.e-texteditor.com/order.html"));
}

void eAbout::OnRegister(wxCommandEvent& WXUNUSED(event)) {
	EndModal(11); // 11 for register
}

// ---- LogoPanel --------------------------------------------------------------

BEGIN_EVENT_TABLE(eAbout::LogoPanel, wxPanel)
	EVT_PAINT(eAbout::LogoPanel::OnPaint)
END_EVENT_TABLE()

eAbout::LogoPanel::LogoPanel(wxWindow *parent)
: wxPanel(parent), bitmap(e_xpm) {
	SetBackgroundColour(wxColour(138, 187, 222));
	SetSizeHints(bitmap.GetWidth(), bitmap.GetHeight());
	SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());
}

void eAbout::LogoPanel::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	dc.DrawBitmap(bitmap, 0, 0, true);
}
