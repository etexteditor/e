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
#include <wx/mstream.h>

#include "IFrameSearchService.h"
#include "IEditorSearch.h"

#include "eSettings.h"
#include "CloseButton.h"
#include "SeparatorLine.h"

#include "images/search_png.h"
#include "images/search_re_png.h"


class SearchEvtHandler : public wxEvtHandler {
public:
	SearchEvtHandler(SearchPanel* parent);
private:
	// Event handlers
	void OnKeyDown(wxKeyEvent &evt);
	void OnChar(wxKeyEvent &evt);
	void OnFocusLost(wxFocusEvent& evt);
	void OnMouseWheel(wxMouseEvent& evt);
	DECLARE_EVENT_TABLE();
	// Member variables
	SearchPanel* parent;
};


// control ids
enum
{
    SEARCH_CLOSE = 100,
    SEARCH_BUTTON,
	SEARCH_BOX,
	SEARCH_NEXT,
	SEARCH_PREV,
	REPLACE_BOX,
	REPLACE_REPLACE,
	REPLACE_ALL
};

// popup-menu ids
enum
{
    PMENU_REGEX = 300,
	PMENU_MCASE,
	PMENU_HIGHLIGHT
};

BEGIN_EVENT_TABLE(SearchPanel, wxPanel)
	EVT_TEXT(SEARCH_BOX, SearchPanel::OnSearchText)
	EVT_TEXT_ENTER(SEARCH_BOX, SearchPanel::OnSearchTextEnter)
	EVT_COMBOBOX(SEARCH_BOX, SearchPanel::OnSearchTextCombo)
	EVT_BUTTON(SEARCH_BUTTON, SearchPanel::OnSearchPopup)
	EVT_BUTTON(SEARCH_NEXT, SearchPanel::OnNext)
	EVT_BUTTON(SEARCH_PREV, SearchPanel::OnPrevious)
	EVT_BUTTON(SEARCH_CLOSE, SearchPanel::OnCloseButton)
	EVT_BUTTON(REPLACE_REPLACE, SearchPanel::OnReplace)
	EVT_BUTTON(REPLACE_ALL, SearchPanel::OnReplaceAll)
	EVT_SYS_COLOUR_CHANGED(SearchPanel::OnSysColourChanged)
	EVT_MENU(PMENU_REGEX, SearchPanel::OnMenuRegex)
	EVT_MENU(PMENU_MCASE, SearchPanel::OnMenuMatchCase)
	EVT_MENU(PMENU_HIGHLIGHT, SearchPanel::OnMenuHighlight)
	EVT_KILL_FOCUS(SearchPanel::OnKillFocus)
END_EVENT_TABLE()

SearchPanel::SearchPanel(IFrameSearchService& searchService, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size):
	wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE),
	m_searchService(searchService),
	m_use_regex(false), m_match_case(false), m_highlight(true), restart_next_search(false), nosearch(false),
	m_settings(eGetSettings())
{
	// Create custom event handlers
	wxEvtHandler* searchbox_evt_handler = new SearchEvtHandler(this);
	wxEvtHandler* replacebox_evt_handler = new SearchEvtHandler(this);

	// Set up accelerator table
	const unsigned int accelcount = 4;
	wxAcceleratorEntry entries[accelcount];
	entries[0].Set(wxACCEL_ALT, WXK_DOWN, SEARCH_BUTTON);
	entries[1].Set(wxACCEL_ALT, (int)'R', REPLACE_REPLACE);
	entries[2].Set(wxACCEL_ALT, (int)'A', REPLACE_ALL);
	entries[3].Set(wxACCEL_NORMAL, WXK_ESCAPE, SEARCH_CLOSE);
	wxAcceleratorTable accel(accelcount, entries);
	SetAcceleratorTable(accel);

	// Setup the popup menu
	m_popupMenu.Append(PMENU_REGEX, _("Use &Regular Expressions"), _("Use Regular Expressions"), wxITEM_CHECK);
	m_popupMenu.Append(PMENU_MCASE, _("&Match Case"), _("Match Case"), wxITEM_CHECK);
	m_popupMenu.AppendSeparator();
	m_popupMenu.Append(PMENU_HIGHLIGHT, _("&Highlight"), _("Highlight all matches"), wxITEM_CHECK);
	m_popupMenu.Check(PMENU_HIGHLIGHT, m_highlight);

	// Create the controls
	SeperatorLine* sepline = new SeperatorLine(this, -1);
	closeButton = new CloseButton(this, SEARCH_CLOSE);
	wxStaticText *searchlabel = new wxStaticText(this, -1, _("Search: "));

	// Load bitmaps
	wxMemoryInputStream searchIn(search_png, SEARCH_PNG_LEN);
	m_searchBitmap = wxBitmap(wxImage(searchIn, wxBITMAP_TYPE_PNG));
	wxMemoryInputStream searchReIn(search_re_png, SEARCH_RE_PNG_LEN);
	m_searchReBitmap = wxBitmap(wxImage(searchReIn, wxBITMAP_TYPE_PNG));

	searchButton = new wxBitmapButton(this, SEARCH_BUTTON, m_searchBitmap);
	if (m_use_regex) searchButton->SetBitmapLabel(m_searchReBitmap);
	searchButton->SetWindowStyle(0);
	searchButton->SetToolTip(_("Set search options (Shortcut Alt-Down)"));

	searchbox = new wxComboBox(this, SEARCH_BOX);
	searchbox->SetWindowStyle(wxTE_PROCESS_ENTER);
	searchbox->PushEventHandler(searchbox_evt_handler);
	RefreshSearchHistory();

	nextButton = new wxButton(this, SEARCH_NEXT, wxT("&Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	nextButton->SetToolTip(_("Find next match (Shortcut F3)"));
	//nextButton->PushEventHandler(evt_handler);
	prevButton = new wxButton(this, SEARCH_PREV, wxT("&Previous"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	prevButton->SetToolTip(_("Find previous match (Shortcut Shift-F3)"));
	replaceBox = new wxComboBox(this, REPLACE_BOX);
	replaceBox->PushEventHandler(replacebox_evt_handler);
	RefreshReplaceHistory();
	replaceButton = new wxButton(this, REPLACE_REPLACE, wxT("&Replace"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	replaceButton->SetToolTip(_("Replace current match (Shortcut Alt-R)"));
	allButton = new wxButton(this, REPLACE_ALL, wxT("&All"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	allButton->SetToolTip(_("Replace all matches (Shortcut Alt-A)"));

	// Disable the buttons (no search text yet)
	nextButton->Disable();
	prevButton->Disable();
	replaceButton->Disable();
	allButton->Disable();

	// Create the sizer layout
	vbox = new wxBoxSizer(wxVERTICAL);
	vbox->Add(sepline, 0, wxEXPAND);
	box = new wxBoxSizer(wxHORIZONTAL);
	box->Add(closeButton, 0, wxALIGN_CENTER|wxRIGHT, 2);
	box->Add(searchlabel, 0, wxALIGN_CENTER);
	box->Add(searchButton, 0, wxALIGN_CENTER);
	box->Add(searchbox, 3, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
	box->Add(nextButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
	box->Add(prevButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
	box->Add(replaceBox, 2, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
	box->Add(replaceButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
	box->Add(allButton, 0, wxALIGN_CENTER|wxTOP|wxBOTTOM, 2);
	vbox->Add(box, 0, wxEXPAND);
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
	if (focused == (wxWindow*)searchButton) return true;
	if (focused == (wxWindow*)closeButton) return true;

	return false; // no focus
}

IEditorSearch* SearchPanel::GetEditorSearch() {
	return m_searchService.GetSearch();
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

	nosearch = true;
	if (!searchtext.empty()) {
		searchbox->SetValue(searchtext);

		nextButton->Enable();
		prevButton->Enable();
		replaceButton->Enable();
		allButton->Enable();
	}
	nosearch = false;

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

void SearchPanel::SetState(cxFindResult result) {
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
	else {
		searchbox->SetBackgroundColour(*wxRED);
		searchbox->SetForegroundColour(*wxWHITE);
		restart_next_search = true;
	}
	searchButton->Refresh();
	searchbox->Refresh();
}

void SearchPanel::Find() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	wxASSERT(editorSearch);

	const wxString searchtext = searchbox->GetValue();
	if (searchtext.empty()) {
		SetState(cxFOUND); // reset state
		return;
	}

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;
	const cxFindResult result = editorSearch->Find(searchtext, options);
	SetState(result);
}

void SearchPanel::FindNext() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	wxASSERT(editorSearch);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;
	const cxFindResult result = editorSearch->FindNext(searchbox->GetValue(), options);
	SetState(result);
}

void SearchPanel::FindPrevious() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	wxASSERT(editorSearch);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;

	bool result = editorSearch->FindPrevious(searchbox->GetValue(), options);
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::Replace() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	wxASSERT(editorSearch);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;

	bool result = editorSearch->Replace(searchbox->GetValue(), replaceBox->GetValue(), options);
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::ReplaceAll() {
	IEditorSearch* editorSearch = m_searchService.GetSearch();
	wxASSERT(editorSearch);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;

	bool result = editorSearch->ReplaceAll(searchbox->GetValue(), replaceBox->GetValue(), options);
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::HidePanel() {
	m_searchService.ShowSearch(false);
}

void SearchPanel::OnCloseButton(wxCommandEvent& WXUNUSED(evt)) {
	HidePanel();
}

void SearchPanel::OnSearchPopup(wxCommandEvent& WXUNUSED(evt)) {
	// Update the menu to reflect settings
	m_popupMenu.Check(PMENU_REGEX, m_use_regex);
	m_popupMenu.Check(PMENU_MCASE, m_match_case);
	m_popupMenu.Check(PMENU_HIGHLIGHT, m_highlight);

	// Get popup position (middle of button)
	wxPoint pos = searchButton->GetScreenPosition();
	const wxSize size = searchButton->GetSize();
	pos.x += size.x / 2;
	pos.y += size.y / 2;
	pos = ScreenToClient(pos);

	// Show the popup menu
	PopupMenu(&m_popupMenu, pos);

	searchbox->SetFocus();
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

	// Don't do a search if the searchstring is allready selected
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
	searchButton->SetBitmapLabel(m_use_regex ? m_searchReBitmap : m_searchBitmap);
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

void SearchPanel::OnChar(wxKeyEvent& evt) {
	wxLogDebug(wxT("OnChar"));
	if (evt.GetKeyCode() == WXK_ESCAPE) {
		wxLogDebug(wxT("ESCAPE"));
	}
}

void SearchPanel::OnSysColourChanged(wxSysColourChangedEvent& event) {
	// This is probably the user having changed the theme (under XP)
	wxPanel::OnSysColourChanged(event); // propagate the event
	vbox->Layout();
}

void SearchPanel::OnMenuRegex(wxCommandEvent& WXUNUSED(evt)) {
	m_use_regex = !m_use_regex; // WORKAROUND: evt.IsChecked() returns wrong value

	searchButton->SetBitmapLabel(m_use_regex ? m_searchReBitmap : m_searchBitmap);

	Find(); // Redo the search with the new settings
}

void SearchPanel::OnMenuMatchCase(wxCommandEvent& WXUNUSED(evt)) {
	m_match_case = !m_match_case; // WORKAROUND: evt.IsChecked() returns wrong value
	Find(); // Redo the search with the new settings
}

void SearchPanel::OnMenuHighlight(wxCommandEvent& WXUNUSED(evt)) {
	m_highlight = !m_highlight; // WORKAROUND: evt.IsChecked() returns wrong value

	m_settings.SetSettingBool(wxT("search/highlight"), m_highlight);

	Find(); // Redo the search with the new settings
}

void SearchPanel::OnKillFocus(wxFocusEvent& WXUNUSED(evt)) {
	wxLogDebug(wxT("SearchPanel lost focus"));
}

void SearchPanel::RefreshSearchHistory() {
	// Clear() also deletes the text in search box, so we have to cache it
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

	if (m_settings.AddSearch(searchbox->GetValue(), m_use_regex, m_match_case)) {
		RefreshSearchHistory();
	}
}

void SearchPanel::RefreshReplaceHistory() {
	// Clear() also deletes the text in search box, so we have to cache it
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

	if (m_settings.AddReplace(replaceBox->GetValue())) {
		RefreshReplaceHistory();
	}
}

// -- SearchEvtHandler -----------------------------------------------------------------

BEGIN_EVENT_TABLE(SearchEvtHandler, wxEvtHandler)
	EVT_KEY_DOWN(SearchEvtHandler::OnKeyDown)
	EVT_CHAR(SearchEvtHandler::OnChar)
	EVT_KILL_FOCUS(SearchEvtHandler::OnFocusLost)
	EVT_MOUSEWHEEL(SearchEvtHandler::OnMouseWheel)
END_EVENT_TABLE()

SearchEvtHandler::SearchEvtHandler(SearchPanel* parent): parent(parent) {}

void SearchEvtHandler::OnKeyDown(wxKeyEvent &event) {
	// When wxTE_PROCESS_ENTER is set, we have to manually process tab
	if (event.GetKeyCode() == WXK_TAB) {
		const int direction = wxGetKeyState(WXK_SHIFT) ? 0 : wxNavigationKeyEvent::IsForward;
		((wxWindow*)event.GetEventObject())->Navigate(direction);
		return;
	}

	event.Skip();
}

void SearchEvtHandler::OnChar(wxKeyEvent &evt) {
	if (evt.GetKeyCode() == WXK_ESCAPE) {
		parent->HidePanel();
		return;
	}
	/*else if (evt.GetKeyCode() == WXK_RETURN) {
		if (evt.ShiftDown()) ((SearchPanel*)parent)->FindPrevious();
		else ((SearchPanel*)parent)->FindNext();
	}*/

	evt.Skip();
}

void SearchEvtHandler::OnFocusLost(wxFocusEvent& evt) {
	// We don't want to save partial searches during incremental search
	// so we only save when the ctrl loses focus. 
	parent->UpdateSearchHistory();
	evt.Skip();
}

void SearchEvtHandler::OnMouseWheel(wxMouseEvent& evt) {
	// We want mousewheel events to go to the active editor
	// (instead of just scrolling comboBox with search history)
	IEditorSearch* editorSearch = parent->GetEditorSearch();
	if (editorSearch) editorSearch->GetEventHandlerI()->ProcessEvent(evt);
}
