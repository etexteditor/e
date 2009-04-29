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

#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "CloseButton.h"
#include "FindFlags.h"

// pre-definitions
class eSettings;

class SearchPanel : public wxPanel {
public:
	SearchPanel(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	~SearchPanel();

	// Member functions
	void InitSearch(const wxString& searchtext = wxT(""), bool replace=false);
	void Find();
	void FindNext();
	void FindPrevious();
	void Replace();
	void ReplaceAll();
	void HidePanel();

	bool HasSearchString() const {return !searchbox->GetValue().IsEmpty();};
	bool IsActive() const;

private:
	// Embedded class: SeperatorLine
	class SeperatorLine : public wxControl {
	public:
		SeperatorLine(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition);
	private:
		// overriden base class virtuals
		virtual bool AcceptsFocus() const { return FALSE; }
		// Event handlers
		void OnPaint(wxPaintEvent& evt);
		void OnEraseBackground(wxEraseEvent& event);
		DECLARE_EVENT_TABLE();
	};

	// Embedded class: SearchEvtHandler
	class SearchEvtHandler : public wxEvtHandler {
	public:
		SearchEvtHandler(wxWindow* parent);
	private:
		// Event handlers
		void OnChar(wxKeyEvent &evt);
		void OnFocusLost(wxFocusEvent& evt);
		DECLARE_EVENT_TABLE();
		// Member variables
		wxWindow* parent;
	};

	// Member functions
	void SetState(cxFindResult result);
	void RefreshSearchHistory();
	void UpdateSearchHistory();
	void RefreshReplaceHistory();
	void UpdateReplaceHistory();

	// Event handlers
	void OnSearchPopup(wxCommandEvent& evt);
	void OnSearchText(wxCommandEvent& evt);
	void OnSearchTextEnter(wxCommandEvent& evt);
	void OnSearchTextCombo(wxCommandEvent& evt);
	void OnNext(wxCommandEvent& evt);
	void OnPrevious(wxCommandEvent& evt);
	void OnCloseButton(wxCommandEvent& evt);
	void OnReplace(wxCommandEvent& evt);
	void OnReplaceAll(wxCommandEvent& evt);
	void OnChar(wxKeyEvent& evt);
	void OnSysColourChanged(wxSysColourChangedEvent& event);
	void OnMenuRegex(wxCommandEvent& evt);
	void OnMenuMatchCase(wxCommandEvent& evt);
	void OnMenuHighlight(wxCommandEvent& evt);
	void OnKillFocus(wxFocusEvent& event);
	DECLARE_EVENT_TABLE();

	// Member controls
	SearchEvtHandler* searchbox_evt_handler;
	SearchEvtHandler* replacebox_evt_handler;
	wxMenu m_popupMenu;
	wxBoxSizer* box;
	wxBoxSizer* vbox;
	wxComboBox* searchbox;
	SeperatorLine* sepline;
	wxButton* nextButton;
	wxButton* prevButton;
	wxButton* replaceButton;
	CloseButton* closeButton;
	wxComboBox* replaceBox;
	wxButton* allButton;
	wxBitmapButton* searchButton;
	wxBitmap m_searchBitmap;
	wxBitmap m_searchReBitmap;

	// Member variables (user settings)
	wxString m_searchText;
	bool m_use_regex;
	bool m_match_case;
	bool m_highlight;

	// Member variables (internal)
	bool restart_next_search;
	bool nosearch;
	eSettings& m_settings;
};

#endif // __SEARCHPANEL_H__
