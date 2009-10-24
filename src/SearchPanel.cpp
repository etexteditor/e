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

#include "SearchPanel.h"

#include <wx/combobox.h>

#include "IFrameSearchService.h"
#include "IEditorSearch.h"
#include "eSettings.h"
#include "CloseButton.h"
#include "SeparatorLine.h"


class SearchEvtHandler : public wxEvtHandler {
public:
	SearchEvtHandler(SearchPanel* parent): parent(parent) {};
private:
	void OnKeyDown(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnFocusLost(wxFocusEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	DECLARE_EVENT_TABLE();

	SearchPanel* parent;
};


enum SearchPanel_IDs {
    SEARCH_CLOSE = 100,
	SEARCH_BOX,
	SEARCH_NEXT,
	SEARCH_PREV,
	REPLACE_BOX,
	REPLACE_REPLACE,
	REPLACE_ALL,
	CHECK_REGEX,
	CHECK_MATCHCASE,
	CHECK_HIGHLIGHT,
};

BEGIN_EVENT_TABLE(SearchPanel, wxPanel)
	EVT_TEXT(SEARCH_BOX, SearchPanel::OnSearchText)
	EVT_TEXT_ENTER(SEARCH_BOX, SearchPanel::OnSearchTextEnter)
	EVT_COMBOBOX(SEARCH_BOX, SearchPanel::OnSearchTextCombo)

	EVT_BUTTON(SEARCH_NEXT, SearchPanel::OnNext)
	EVT_BUTTON(SEARCH_PREV, SearchPanel::OnPrevious)
	EVT_BUTTON(SEARCH_CLOSE, SearchPanel::OnCloseButton)
	EVT_BUTTON(REPLACE_REPLACE, SearchPanel::OnReplace)
	EVT_BUTTON(REPLACE_ALL, SearchPanel::OnReplaceAll)

	EVT_SYS_COLOUR_CHANGED(SearchPanel::OnSysColourChanged)

	EVT_CHECKBOX(CHECK_HIGHLIGHT, SearchPanel::OnMenuHighlight)
	EVT_CHECKBOX(CHECK_REGEX, SearchPanel::OnMenuRegex)
	EVT_CHECKBOX(CHECK_MATCHCASE, SearchPanel::OnMenuMatchCase)
END_EVENT_TABLE()

SearchPanel::SearchPanel(IFrameSearchService& searchService, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size):
	wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE),
	m_searchService(searchService),
	m_use_regex(false), m_match_case(false), m_highlight(true), restart_next_search(false), nosearch(false),
	m_settings(eGetSettings())
{
	InitAcceleratorTable();

	// Create the controls
	SeperatorLine* sepline = new SeperatorLine(this, -1);
	closeButton = new CloseButton(this, SEARCH_CLOSE);
	wxStaticText *searchlabel = new wxStaticText(this, -1, _("Search: "));

	searchbox = new wxComboBox(this, SEARCH_BOX);
	searchbox->SetWindowStyle(wxTE_PROCESS_ENTER);
	searchbox->PushEventHandler(new SearchEvtHandler(this));
	RefreshSearchHistory();

	nextButton = new wxButton(this, SEARCH_NEXT, wxT("&Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	nextButton->SetToolTip(_("Find next match (F3)"));

	prevButton = new wxButton(this, SEARCH_PREV, wxT("&Previous"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	prevButton->SetToolTip(_("Find previous match (Shift-F3)"));

	replaceBox = new wxComboBox(this, REPLACE_BOX);
	replaceBox->PushEventHandler(new SearchEvtHandler(this));
	RefreshReplaceHistory();

	replaceButton = new wxButton(this, REPLACE_REPLACE, wxT("&Replace"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	replaceButton->SetToolTip(_("Replace current match (Alt-R)"));

	allButton = new wxButton(this, REPLACE_ALL, wxT("&All"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	allButton->SetToolTip(_("Replace all matches (Alt-A)"));

	// Disable the buttons (no search text yet)
	nextButton->Disable();
	prevButton->Disable();
	replaceButton->Disable();
	allButton->Disable();

	checkHighlight = new wxCheckBox(this, CHECK_HIGHLIGHT, wxT("&Highlight"));
	checkHighlight->SetValue(true);
	checkRegex = new wxCheckBox(this, CHECK_REGEX, wxT("Rege&x"));
	checkMatchcase = new wxCheckBox(this, CHECK_MATCHCASE, wxT("Match &case"));

	commandResults = new wxStaticText(this, wxID_ANY, wxT(""));

	// Create the sizer layout
	vbox = new wxBoxSizer(wxVERTICAL);
	vbox->Add(sepline, 0, wxEXPAND);
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
		box->AddSpacer(5);
		box->Add(closeButton, 0, wxALIGN_CENTER|wxRIGHT, 2);
		box->Add(searchlabel, 0, wxALIGN_CENTER);
		box->Add(searchbox, 3, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
		box->Add(nextButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box->Add(prevButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box->Add(replaceBox, 2, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
		box->Add(replaceButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box->Add(allButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box->AddSpacer(5);
	vbox->Add(box, 0, wxEXPAND);

	wxBoxSizer* box2 = new wxBoxSizer(wxHORIZONTAL);
		box2->AddSpacer(5 + closeButton->GetSize().GetWidth() + 2);
		box2->Add(new wxStaticText(this, wxID_ANY, wxT("Options:")), 0, wxALIGN_CENTER);
		box2->Add(checkHighlight, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box2->AddSpacer(5);
		box2->Add(checkRegex, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box2->AddSpacer(5);
		box2->Add(checkMatchcase, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
		box2->AddSpacer(10);
		box2->Add(commandResults, 1, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
		box2->AddSpacer(5);
	vbox->Add(box2, 0, wxEXPAND);

	SetSizer(vbox);

	// Set the correct size
	wxSize minsize = vbox->GetMinSize();
	SetSize(minsize);

	// Make sure sizes get the right min/max sizes
	SetSizeHints(minsize.x, minsize.y, -1, minsize.y);
}

SearchPanel::~SearchPanel() {
	// Delete the SearchEventHandlers
	searchbox->PopEventHandler(true);
	replaceBox->PopEventHandler(true);
}

void SearchPanel::InitAcceleratorTable() {
	const unsigned int accelcount = 3;
	wxAcceleratorEntry entries[accelcount];
	entries[0].Set(wxACCEL_ALT, (int)'R', REPLACE_REPLACE);
	entries[1].Set(wxACCEL_ALT, (int)'A', REPLACE_ALL);
	entries[2].Set(wxACCEL_NORMAL, WXK_ESCAPE, SEARCH_CLOSE);
	wxAcceleratorTable accel(accelcount, entries);
	SetAcceleratorTable(accel);
}

bool SearchPanel::HasSearchString() const {
	return !searchbox->GetValue().IsEmpty();
}

bool SearchPanel::IsActive() const {
	if (!IsShown()) return false;

	// Check if any local control has focus
	wxWindow* focused = FindFocus();
	if (focused == (wxWindow*)searchbox) return true;
	if (focused == (wxWindow*)replaceBox) return true;
	if (focused == (wxWindow*)nextButton) return true;
	if (focused == (wxWindow*)prevButton) return true;
	if (focused == (wxWindow*)replaceButton) return true;
	if (focused == (wxWindow*)allButton) return true;
	if (focused == (wxWindow*)closeButton) return true;

	return false; // no focus
}

void SearchPanel::InitSearch(const wxString& searchtext, bool replace) {
	wxWindow* focus_win = FindFocus();
	if (focus_win == replaceBox) {
		if (replace) Replace();
		else searchbox->SetFocus();
		return;
	}
	
	if (!replace && focus_win == searchbox) {
		FindNext(); 
		return;
	}

	if (!searchtext.empty()) {
		// Don't trigger a search as we set the text value
		nosearch = true;
			searchbox->SetValue(searchtext);
		nosearch = false;

		nextButton->Enable();
		prevButton->Enable();
		replaceButton->Enable();
		allButton->Enable();
	}

	SetState(cxFOUND);
	if (replace) {
		replaceBox->SetSelection(-1, -1); // select all
		replaceBox->SetFocus();
	}
	else {
		searchbox->SetSelection(-1, -1); // select all
		searchbox->SetFocus();
	}
}

void SearchPanel::SetState(cxFindResult result, int resultCount) {
	// Give visual feedback in the search field
	if (result == cxFOUND) {
		searchbox->SetBackgroundColour(*wxWHITE);
		searchbox->SetForegroundColour(*wxBLACK);
		restart_next_search = false;
	}
	else if (result == cxFOUND_AFTER_RESTART) {
		const wxColour restartColor(166,202,240);
		searchbox->SetBackgroundColour(restartColor);
		searchbox->SetForegroundColour(*wxBLACK);
		restart_next_search = false;
	}
	else /* result == cxNOT_FOUND */ {
		searchbox->SetBackgroundColour(*wxRED);
		searchbox->SetForegroundColour(*wxWHITE);
		restart_next_search = true;
	}
	searchbox->Refresh();

	wxString results = (0 <= resultCount) ? wxString::Format(wxT("%d results"), resultCount) : wxEmptyString;
	commandResults->SetLabel(results);
}

int SearchPanel::GetOptionFlags() {
	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;
	return options;
}

void SearchPanel::Find() {
	const wxString searchtext = searchbox->GetValue();
	if (searchtext.empty()) {
		SetState(cxFOUND); // reset state
		return;
	}

	IEditorSearch* editorSearch = m_searchService.GetSearch();
	const cxFindResult result = editorSearch->Find(searchtext, GetOptionFlags());
	SetState(result);
}

void SearchPanel::FindNext() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	const cxFindResult result = editorSearch->FindNext(searchbox->GetValue(), GetOptionFlags());
	SetState(result);
}

void SearchPanel::FindPrevious() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	const cxFindResult result = editorSearch->FindPrevious(searchbox->GetValue(), GetOptionFlags());
	SetState(result);
}

void SearchPanel::Replace() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	bool result = editorSearch->Replace(searchbox->GetValue(), replaceBox->GetValue(), GetOptionFlags());
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::ReplaceAll() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	int resultCount = editorSearch->ReplaceAll(searchbox->GetValue(), replaceBox->GetValue(), GetOptionFlags());
	SetState(resultCount ? cxFOUND : cxNOT_FOUND, resultCount);
}

void SearchPanel::HidePanel() {
	m_searchService.ShowSearch(false);
}

void SearchPanel::OnCloseButton(wxCommandEvent& WXUNUSED(evt)) {
	HidePanel();
}

void SearchPanel::OnSearchText(wxCommandEvent& evt) {
	restart_next_search = false;

	if(evt.GetString().empty()) {
		nextButton->Disable();
		prevButton->Disable();
		replaceButton->Disable();
		allButton->Disable();

		IEditorSearch* editorSearch = m_searchService.GetSearch();
		wxASSERT(editorSearch);
		editorSearch->ClearSearchHighlight();
	}
	else {
		nextButton->Enable();
		prevButton->Enable();
		replaceButton->Enable();
		allButton->Enable();
	}

	// Don't do a search if the searchstring is already selected
	if (nosearch) return;

	Find();
}

void SearchPanel::OnSearchTextEnter(wxCommandEvent& WXUNUSED(evt)) {
	if (wxGetKeyState(WXK_SHIFT)) FindPrevious();
	else FindNext();
}

void SearchPanel::OnSearchTextCombo(wxCommandEvent& evt) {
	wxString pattern;
	m_settings.GetSearch(evt.GetSelection(), pattern, m_use_regex, m_match_case);
}

void SearchPanel::OnNext(wxCommandEvent& WXUNUSED(evt)) {
	FindNext();
}

void SearchPanel::OnPrevious(wxCommandEvent& WXUNUSED(evt)) {
	FindPrevious();
}

void SearchPanel::OnReplace(wxCommandEvent& WXUNUSED(evt)) {
	Replace();
	UpdateReplaceHistory();
}

void SearchPanel::OnReplaceAll(wxCommandEvent& WXUNUSED(evt)) {
	ReplaceAll();
	UpdateReplaceHistory();
}

void SearchPanel::OnSysColourChanged(wxSysColourChangedEvent& event) {
	// This is probably the user having changed the theme (under XP)
	wxPanel::OnSysColourChanged(event); // propagate the event
	vbox->Layout();
}

void SearchPanel::OnMenuRegex(wxCommandEvent& WXUNUSED(evt)) {
	m_use_regex = !m_use_regex;
	Find();
}

void SearchPanel::OnMenuMatchCase(wxCommandEvent& WXUNUSED(evt)) {
	m_match_case = !m_match_case;
	Find();
}

void SearchPanel::OnMenuHighlight(wxCommandEvent& WXUNUSED(evt)) {
	m_highlight = !m_highlight;
	m_settings.SetSettingBool(wxT("search/highlight"), m_highlight);
	Find();
}

void SearchPanel::RefreshSearchHistory() {
	// Clear() deletes history and text, so we have to cache it
	const wxString searchText = searchbox->GetValue();
	searchbox->Clear();
	searchbox->SetValue(searchText);

	const size_t sCount = m_settings.GetSearchCount();
	for (size_t i = 0; i < sCount; ++i) {
		wxString pattern;
		bool isRegex;
		bool matchCase;
		m_settings.GetSearch(i, pattern, isRegex, matchCase);
		searchbox->Append(pattern);
	}
}

void SearchPanel::UpdateSearchHistory() {
	if (searchbox->GetValue().IsEmpty()) return;

	if (m_settings.AddSearch(searchbox->GetValue(), m_use_regex, m_match_case))
		RefreshSearchHistory();
}

void SearchPanel::RefreshReplaceHistory() {
	// Clear() deletes history and text, so we have to cache it
	const wxString repText = replaceBox->GetValue();
	replaceBox->Clear();
	replaceBox->SetValue(repText);

	const size_t rCount = m_settings.GetReplaceCount();
	for (size_t i = 0; i < rCount; ++i) {
		const wxString pattern = m_settings.GetReplace(i);
		replaceBox->Append(pattern);
	}
}

void SearchPanel::UpdateReplaceHistory() {
	if (replaceBox->GetValue().IsEmpty()) return;

	if (m_settings.AddReplace(replaceBox->GetValue()))
		RefreshReplaceHistory();
}

// -- SearchEvtHandler -----------------------------------------------------------------

BEGIN_EVENT_TABLE(SearchEvtHandler, wxEvtHandler)
	EVT_KEY_DOWN(SearchEvtHandler::OnKeyDown)
	EVT_CHAR(SearchEvtHandler::OnChar)
	EVT_KILL_FOCUS(SearchEvtHandler::OnFocusLost)
	EVT_MOUSEWHEEL(SearchEvtHandler::OnMouseWheel)
END_EVENT_TABLE()

void SearchEvtHandler::OnKeyDown(wxKeyEvent &event) {
	// When wxTE_PROCESS_ENTER is set, we have to manually process tab
	if (event.GetKeyCode() == WXK_TAB) {
		const int direction = wxGetKeyState(WXK_SHIFT) ? 0 : wxNavigationKeyEvent::IsForward;
		((wxWindow*)event.GetEventObject())->Navigate(direction);
		return;
	}

	event.Skip(true);
}

void SearchEvtHandler::OnChar(wxKeyEvent& event) {
	if (event.GetKeyCode() == WXK_ESCAPE) {
		parent->HidePanel();
		return;
	}
	event.Skip(true);
}

void SearchEvtHandler::OnFocusLost(wxFocusEvent& event) {
	// We don't want to save partial searches during incremental search
	// so we only save when the ctrl loses focus. 
	parent->UpdateSearchHistory();
	event.Skip(true);
}

void SearchEvtHandler::OnMouseWheel(wxMouseEvent& event) {
	// Let EditorFrame handle the wheel event (which will delegate to the EditorCtrl.)
	wxTheApp->GetTopWindow()->ProcessEvent(event);
}
