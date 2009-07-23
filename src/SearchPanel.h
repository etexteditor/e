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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/panel.h>
	#include <wx/string.h>
#endif

#include "FindFlags.h"

class CloseButton;
class eSettings;
class IFrameSearchService;
class IEditorSearch;

class wxBoxSizer;
class wxButton;
class wxComboBox;
class wxCheckBox;
class wxStaticText;

class SearchPanel : public wxPanel {
public:
	SearchPanel(IFrameSearchService& searchService, wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	~SearchPanel();

	// Member functions
	void InitSearch(const wxString& searchtext = wxT(""), bool replace=false);
	void Find();
	void FindNext();
	void FindPrevious();
	void Replace();
	void ReplaceAll();
	void HidePanel();

	bool HasSearchString() const;
	bool IsActive() const;

	IEditorSearch* GetEditorSearch();

	// Member functions
	void SetState(cxFindResult result, int resultCount = -1);
	void RefreshSearchHistory();
	void UpdateSearchHistory();
	void RefreshReplaceHistory();
	void UpdateReplaceHistory();

private:
	void InitAcceleratorTable();

	// Event handlers
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

	IFrameSearchService& m_searchService;

	// Member controls
	wxBoxSizer* vbox;
	wxComboBox* searchbox;
	wxButton* nextButton;
	wxButton* prevButton;
	wxButton* replaceButton;
	CloseButton* closeButton;
	wxComboBox* replaceBox;
	wxButton* allButton;

	wxCheckBox* checkHighlight;
	wxCheckBox* checkRegex;
	wxCheckBox* checkMatchcase;
	wxStaticText* commandResults;


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
