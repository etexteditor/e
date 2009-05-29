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

#include "RevTooltip.h"
#include <wx/statline.h>
#include "Catalyst.h"

RevTooltip::RevTooltip(const CatalystWrapper& catalyst): 
	m_catalyst(catalyst) {}

RevTooltip::RevTooltip(wxWindow *parent, const CatalystWrapper& catalyst):
	m_catalyst(catalyst)
{
	Create(parent);
}

void RevTooltip::Create(wxWindow *parent) {
	wxPopupWindow::Create(parent, wxSIMPLE_BORDER);
	Hide();
	SetBackgroundColour(*wxWHITE);

	m_mainPanel = new wxPanel(this);

	m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	{
		wxBoxSizer* userSizer = new wxBoxSizer(wxVERTICAL);
		{
			wxBitmap bmUserPic;
			m_userPic = new wxStaticBitmap(m_mainPanel, wxID_ANY, bmUserPic);
			userSizer->Add(m_userPic, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5);

			m_userName = new wxStaticText(m_mainPanel, wxID_ANY, wxT(""));
			userSizer->Add(m_userName, 0 , wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

			m_userColor = new ColorBox(m_mainPanel, wxID_ANY);
			userSizer->Add(m_userColor, 0 , wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

			m_mainSizer->Add(userSizer, 0, wxEXPAND);
		}

		wxBoxSizer* infoSizer = new wxBoxSizer(wxVERTICAL);
		{
			// Create the label ctrl
			m_labelCtrl = new wxStaticText(m_mainPanel, wxID_ANY, _(""));
			wxFont labelFont = m_labelCtrl->GetFont();
			labelFont.SetWeight(wxFONTWEIGHT_BOLD);
			m_labelCtrl->SetFont(labelFont);

			// Label & date
			wxBoxSizer* labelSizer = new wxBoxSizer(wxHORIZONTAL);
			{
				labelSizer->Add(m_labelCtrl, 0, wxRIGHT, 5);
				m_dateCtrl = new wxStaticText(m_mainPanel, wxID_ANY, _(""));
				labelSizer->Add(m_dateCtrl, 0);
				infoSizer->Add(labelSizer, 0 , wxEXPAND|wxALL, 5);
			}

			infoSizer->Add(new wxStaticLine(m_mainPanel), 0, wxEXPAND|wxLEFT|wxRIGHT, 5);

			// Description
			m_descCtrl = new wxStaticText(m_mainPanel, wxID_ANY, wxT(""));
			infoSizer->Add(m_descCtrl, 1, wxALL, 5);

			// Mirror status
			m_mirrorstatCtrl = new wxStaticText(m_mainPanel, wxID_ANY, _("Saved to disk as:"));

			m_mirrorSizer = new wxBoxSizer(wxVERTICAL);
			{
				m_mirrorSizer->Add(new wxStaticLine(m_mainPanel), 0, wxEXPAND|wxTOP|wxLEFT|wxRIGHT, 5);
				m_mirrorSizer->Add(m_mirrorstatCtrl, 0, wxLEFT|wxRIGHT, 5);
				m_mirrorlistCtrl = new wxStaticText(m_mainPanel, wxID_ANY, _(""));
				m_mirrorSizer->Add(m_mirrorlistCtrl, 0, wxLEFT, 20);
				m_mirrorSizer->AddSpacer(5);
				infoSizer->Add(m_mirrorSizer, 0, wxEXPAND);
				infoSizer->Hide(m_mirrorSizer);
			}

			m_mainSizer->Add(infoSizer, 0, wxEXPAND);
		}
	}

	m_mainPanel->SetSizerAndFit(m_mainSizer);
}

void RevTooltip::SetDocument(const doc_id& di, wxPoint pos) {
	Hide();

	cxLOCK_READ(m_catalyst)
		wxASSERT(catalyst.IsOk(di));

		doc_id m_docId = di;

		// Set the user info
		const unsigned int userId = catalyst.GetDocAuthor(m_docId);
		m_userPic->SetBitmap(catalyst.GetUserPic(userId));
		m_userName->SetLabel(catalyst.GetUserName(userId));
		if (userId == 0) m_userColor->Hide();
		else {
			m_userColor->SetBackgroundColour(catalyst.GetUserColor(userId));
			m_userColor->Show();
		}

		// Set the Label
		wxString label = catalyst.GetLabel(m_docId);
		if (!label.empty()) label += wxT(":");
		m_labelCtrl->SetLabel(label);

		// Set the Date
		const wxDateTime date = catalyst.GetDocDate(m_docId);
		const wxString age = catalyst.GetDocAge(m_docId);
		const wxString longdate = date.Format(wxT("%A, %B %d, %Y - %X ")) + wxString::Format(wxT("(%s)"),age.c_str());
		m_dateCtrl->SetLabel(longdate);

		// Set the Comment
		m_descCtrl->SetLabel(catalyst.GetDescription(m_docId));

		// Show paths to mirrors (if available)
		if (catalyst.IsMirrored(di)) {
			m_mainSizer->Show(m_mirrorSizer, true, true);

			wxArrayString paths;
			if (di.IsDraft()) catalyst.GetMirrorPathsForDraft(di, paths); // gets mirrors for all draft revisions
			else catalyst.GetMirrorPaths(di, paths);

			if (!paths.IsEmpty()) {
				wxString tip = paths[0];
				for (size_t i = 1; i < paths.GetCount(); ++i) {
					tip += wxT("\n");
					tip += paths[i];
				}
				m_mirrorlistCtrl->SetLabel(tip);
			}
			else wxASSERT(false);
		}
		else m_mainSizer->Show(m_mirrorSizer, false, true);
	cxENDLOCK

	m_mainSizer->Layout();
	m_mainSizer->SetSizeHints(m_mainPanel);
	m_mainSizer->SetSizeHints(this);

	// Get the size of the screen
	const int screen_x = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	const int screen_y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

	// Get the size of the mouse cursor
	// BUG WORKAROUND: GetMetric gives how large it _can_ be (32*32) not
	// how large the actual visible image is.
	const int cursor_x = 16; // wxSystemSettings::GetMetric(wxSYS_CURSOR_X);
	const int cursor_y = 17; // wxSystemSettings::GetMetric(wxSYS_CURSOR_Y);

	const wxSize size = GetSize();

	// Calculate the correct placement
	// (pos is assumed to be upper left corner of cursor)
	if (pos.x + cursor_x + size.x > screen_x) pos.x -= size.x + 3;
	else pos.x += cursor_x;

	if (pos.y + cursor_y + size.y > screen_y) pos.y -= size.y + 3;
	else pos.y += cursor_y;

	Move(pos);
	Show();
}

RevTooltip::ColorBox::ColorBox(wxWindow* parent, wxWindowID id)
: wxWindow(parent, id, wxDefaultPosition, wxSize(35, 12), wxSIMPLE_BORDER)
{
	// The size is fixed
	const wxSize size = GetSize();
	SetSizeHints(size, size);
}
