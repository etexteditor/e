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
#include "EditorFrame.h"
#include "images/search_png.h"
#include "images/search_re_png.h"
#include <wx/mstream.h>
#include "FindFlags.h"

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

SearchPanel::SearchPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE),
  m_use_regex(false), m_match_case(false), m_highlight(true), restart_next_search(false), nosearch(false) {

	// Create custom event handlers
	searchbox_evt_handler = new SearchEvtHandler(this);
	replacebox_evt_handler = new SearchEvtHandler(this);

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
	sepline = new SeperatorLine(this, -1);
	closeButton = new CloseButton(this, SEARCH_CLOSE);
	wxStaticText *searchlabel = new wxStaticText(this, -1, _("Search: "));

	wxPanel* searchBgr = new wxPanel(this, -1);
	searchBgr->SetWindowStyle(wxSUNKEN_BORDER);

	// Load bitmaps
	wxMemoryInputStream searchIn(search_png, SEARCH_PNG_LEN);
	m_searchBitmap = wxBitmap(wxImage(searchIn, wxBITMAP_TYPE_PNG));
	wxMemoryInputStream searchReIn(search_re_png, SEARCH_RE_PNG_LEN);
	m_searchReBitmap = wxBitmap(wxImage(searchReIn, wxBITMAP_TYPE_PNG));

	searchButton = new wxBitmapButton(searchBgr, SEARCH_BUTTON, m_searchBitmap);
	if (m_use_regex) searchButton->SetBitmapLabel(m_searchReBitmap);
	searchButton->SetWindowStyle(0);
	searchButton->SetBackgroundColour(*wxWHITE);
	searchButton->SetToolTip(_("Set search options (Shortcut Alt-Down)"));

	searchbox = new wxTextCtrl(searchBgr, SEARCH_BOX);
	searchbox->SetWindowStyle(wxNO_BORDER|wxTE_PROCESS_ENTER);
	searchbox->PushEventHandler(searchbox_evt_handler);

	wxBoxSizer *bgr_box = new wxBoxSizer(wxHORIZONTAL);
	bgr_box->Add(searchButton, 0, wxALIGN_CENTER);
	bgr_box->Add(searchbox, 1, wxALIGN_CENTER|wxEXPAND);
	searchBgr->SetSizer(bgr_box);
	wxSize brgsize = bgr_box->GetMinSize();
	searchBgr->SetSize(brgsize);
	searchBgr->SetSizeHints(brgsize.x, brgsize.y, -1, brgsize.y);
	//bgr_box->Layout();

	nextButton = new wxButton(this, SEARCH_NEXT, wxT("&Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	nextButton->SetToolTip(_("Find next match (Shortcut F3)"));
	//nextButton->PushEventHandler(evt_handler);
	prevButton = new wxButton(this, SEARCH_PREV, wxT("&Previous"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	prevButton->SetToolTip(_("Find previous match (Shortcut Shift-F3)"));
	replaceBox = new wxTextCtrl(this, REPLACE_BOX);
	replaceBox->PushEventHandler(replacebox_evt_handler);
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
	//box->Add(searchButton, 0, wxALIGN_CENTER);
	//box->Add(searchbox, 3, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
	box->Add(searchBgr, 3, wxALIGN_CENTER|wxEXPAND|wxTOP|wxBOTTOM, 2);
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
	// Make sure the SearchEventHandlers get deleted correctly
	searchbox->PopEventHandler(true);
	replaceBox->PopEventHandler(true);
}

bool SearchPanel::IsActive() const {
	if (!IsShown()) return false;

	// Check if any local ctrls has focus
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
	if (!searchtext.empty()) searchbox->SetValue(searchtext);
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
		searchButton->SetBackgroundColour(*wxWHITE);
		searchbox->SetBackgroundColour(*wxWHITE);
		searchbox->SetForegroundColour(*wxBLACK);
		restart_next_search = false;
	}
	else if (result == cxFOUND_AFTER_RESTART) {
		const wxColour restartColor(166,202,240);
		searchButton->SetBackgroundColour(restartColor);
		searchbox->SetBackgroundColour(restartColor);
		searchbox->SetForegroundColour(*wxBLACK);
		restart_next_search = false;
	}
	else {
		searchButton->SetBackgroundColour(*wxRED);
		searchbox->SetBackgroundColour(*wxRED);
		searchbox->SetForegroundColour(*wxWHITE);
		restart_next_search = true;
	}
	searchButton->Refresh();
	searchbox->Refresh();
}

void SearchPanel::Find() {
	// Get a pointer to the active editorctrl
	EditorCtrl* editorCtrl = ((EditorFrame*)GetGrandParent())->GetEditorCtrl();
	wxASSERT(editorCtrl);

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
	const cxFindResult result = editorCtrl->Find(searchtext, options);
	SetState(result);
}

void SearchPanel::FindNext() {
	// Get a pointer to the active editorctrl
	EditorCtrl* editorCtrl = ((EditorFrame*)GetGrandParent())->GetEditorCtrl();
	wxASSERT(editorCtrl);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;
	const cxFindResult result = editorCtrl->FindNext(searchbox->GetValue(), options);
	SetState(result);
}

void SearchPanel::FindPrevious() {
	// Get a pointer to the active editorctrl
	EditorCtrl* editorCtrl = ((EditorFrame*)GetGrandParent())->GetEditorCtrl();
	wxASSERT(editorCtrl);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;

	bool result = editorCtrl->FindPrevious(searchbox->GetValue(), options);
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::Replace() {
	// Get a pointer to the active editorctrl
	EditorCtrl* editorCtrl = ((EditorFrame*)GetGrandParent())->GetEditorCtrl();
	wxASSERT(editorCtrl);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;
	if (m_highlight) options |= FIND_HIGHLIGHT;
	if (restart_next_search) options |= FIND_RESTART;

	bool result = editorCtrl->Replace(searchbox->GetValue(), replaceBox->GetValue(), options);
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::ReplaceAll() {
	// Get a pointer to the active editorctrl
	EditorCtrl* editorCtrl = ((EditorFrame*)GetGrandParent())->GetEditorCtrl();
	wxASSERT(editorCtrl);

	int options = 0;
	if (m_match_case) options |= FIND_MATCHCASE;
	if (m_use_regex) options |= FIND_USE_REGEX;

	bool result = editorCtrl->ReplaceAll(searchbox->GetValue(), replaceBox->GetValue(), options);
	SetState(result ? cxFOUND : cxNOT_FOUND);
}

void SearchPanel::HidePanel() {
	((EditorFrame*)GetGrandParent())->ShowSearch(false);
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

		EditorCtrl* editorCtrl = ((EditorFrame*)GetGrandParent())->GetEditorCtrl();
		wxASSERT(editorCtrl);
		editorCtrl->ClearSearchHighlight();
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

void SearchPanel::OnNext(wxCommandEvent& WXUNUSED(evt)) {
	FindNext();
}

void SearchPanel::OnPrevious(wxCommandEvent& WXUNUSED(evt)) {
	FindPrevious();
}

void SearchPanel::OnReplace(wxCommandEvent& WXUNUSED(evt)) {
	Replace();
}

void SearchPanel::OnReplaceAll(wxCommandEvent& WXUNUSED(evt)) {
	ReplaceAll();
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

	((EditorFrame*)GetGrandParent())->SetSetting(wxT("search/highlight"), m_highlight);

	Find(); // Redo the search with the new settings
}

void SearchPanel::OnKillFocus(wxFocusEvent& WXUNUSED(evt)) {
	wxLogDebug(wxT("SearchPanel lost focus"));
}

// -- SeperatorLine -----------------------------------------------------------------

BEGIN_EVENT_TABLE(SearchPanel::SeperatorLine, wxControl)
	EVT_PAINT(SearchPanel::SeperatorLine::OnPaint)
	EVT_ERASE_BACKGROUND(SearchPanel::SeperatorLine::OnEraseBackground)
END_EVENT_TABLE()

SearchPanel::SeperatorLine::SeperatorLine(wxWindow* parent, wxWindowID id, const wxPoint& pos)
: wxControl(parent, id, pos, wxSize(10, 2)) {
	// Make sure sizers min/max height are fixed to 2 pixels
	SetSizeHints(-1, 2, -1, 2);
}

void SearchPanel::SeperatorLine::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
	wxPaintDC dc(this);
	// Get view dimensions
	wxSize size = GetClientSize();
	wxPen darkPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID);
    wxPen lightPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT), 1, wxSOLID);

	dc.SetPen(darkPen);
	dc.DrawLine(0, 0, size.x, 0);

	dc.SetPen(lightPen);
	dc.DrawLine(0, 1, size.x, 1);
}

void SearchPanel::SeperatorLine::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
	// no evt.Skip() as we don't want the background to be erased
}


// -- SearchEvtHandler -----------------------------------------------------------------

BEGIN_EVENT_TABLE(SearchPanel::SearchEvtHandler, wxEvtHandler)
	EVT_CHAR(SearchPanel::SearchEvtHandler::OnChar)
END_EVENT_TABLE()

SearchPanel::SearchEvtHandler::SearchEvtHandler(wxWindow* parent)
: parent(parent) {
}

void SearchPanel::SearchEvtHandler::OnChar(wxKeyEvent &evt) {
	if (evt.GetKeyCode() == WXK_ESCAPE) {
		((SearchPanel*)parent)->HidePanel();
		return;
	}
	/*else if (evt.GetKeyCode() == WXK_RETURN) {
		if (evt.ShiftDown()) ((SearchPanel*)parent)->FindPrevious();
		else ((SearchPanel*)parent)->FindNext();
	}*/

	evt.Skip();
}
