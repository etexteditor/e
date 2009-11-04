#include "CurrentTabsPopup.h"
#include "wxListCtrlEx.h"

enum CurrentTabsPopup_Columns {
	col_Number = 0,
	col_Filename,
	col_Path,
	col_ColumnCount
};

enum CurrentTabsPopup_IDs {
	ID_TAB_LIST = 100,
};

class ListEventHandler : public wxEvtHandler {
public:
	ListEventHandler(CurrentTabsPopup* parent);

private:
	DECLARE_EVENT_TABLE();
	void OnChar(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	CurrentTabsPopup* m_parent;
};


BEGIN_EVENT_TABLE(ListEventHandler, wxEvtHandler)
	EVT_CHAR(ListEventHandler::OnChar)
	EVT_KEY_DOWN(ListEventHandler::OnKeyDown)
END_EVENT_TABLE()

void ListEventHandler::OnKeyDown(wxKeyEvent& event) {
	const int key_code = event.GetKeyCode();
	const int modifiers = event.GetModifiers();

	if (! (modifiers == wxMOD_CONTROL || modifiers == (wxMOD_CONTROL|wxMOD_SHIFT)) ) {
		event.Skip();
		return;
	}

	if (key_code == 48) { // Zero key
		if (modifiers & wxMOD_SHIFT) // go up
			m_parent->WrapToPrevItem(true);
		else // go down
			m_parent->WrapToNextItem(true);

		return;
	}
	 
	event.Skip();
}

void ListEventHandler::OnChar(wxKeyEvent& event) {
	int key_code = event.GetKeyCode();
	switch ( key_code ) {
	case WXK_ESCAPE:
		// Cancel tab selection.
		m_parent->EndModal(wxID_CANCEL);
		return;

	case WXK_UP: {
		if (m_parent->WrapToPrevItem())
			return;

		break; // event.Skip() will handle normal up arrow handling
	 }

	case WXK_DOWN: {
		if (m_parent->WrapToNextItem())
			return;

		break; // event.Skip() will handle normal down arrow handling
   }

	default:
		const wxChar c = event.GetUnicodeKey();
		// If a number 1-9 was pressed, and we have that many rows,
		// then choose that (-1) as the selected row index.
		if ((wxT('1') <= c) && ( c <= wxT('9'))) {
			int choice = c-wxT('1');
			m_parent->SelectRow(choice);
			return;
		}
		else if (wxT('0') == c) { // Zero moves selection down
			m_parent->WrapToNextItem(true);
			return;
		}
		else if (wxT(')') == c) { // Shift-Zero moves selection up
			m_parent->WrapToPrevItem(true);
			return;
		}
		break;
	}

    event.Skip();
}

ListEventHandler::ListEventHandler( CurrentTabsPopup* parent ):
	m_parent(parent) {}

BEGIN_EVENT_TABLE(CurrentTabsPopup, wxDialog)
	EVT_SHOW(CurrentTabsPopup::OnShow)
	EVT_LEFT_DOWN(CurrentTabsPopup::OnMouseLeftDown)
	EVT_MOUSE_CAPTURE_LOST(CurrentTabsPopup::OnMouseCaptureLost)
END_EVENT_TABLE()

void CurrentTabsPopup::OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event)) {}

void CurrentTabsPopup::OnShow(wxShowEvent& WXUNUSED(event)) {
	m_list->SetFocus();
	m_list->SetSelectedRow(0);

	m_list->SetColumnWidth(col_Number, wxLIST_AUTOSIZE);
	m_list->SetColumnWidth(col_Filename, wxLIST_AUTOSIZE);
	m_list->SetColumnWidth(col_Path, wxLIST_AUTOSIZE_USEHEADER);

	this->CaptureMouse();
}

CurrentTabsPopup::CurrentTabsPopup(wxWindow* parent, const std::vector<OpenTabInfo*>& tabInfo):
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_selectedTabIndex(-1)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	m_list = new wxListCtrlEx(this, ID_TAB_LIST, wxPoint(0,0), wxSize(500,300), wxLC_REPORT|wxLC_SINGLE_SEL);
	m_list->InsertColumn(col_Number, wxT("#"));
	m_list->InsertColumn(col_Filename, wxT("Filename"));
	m_list->InsertColumn(col_Path, wxT("Path"));

	int index = 0;
	for (std::vector<OpenTabInfo*>::const_iterator p = tabInfo.begin(); p != tabInfo.end(); ++p) {
		if (index < 9)
			m_list->InsertItem(index, wxString::Format(wxT("%d"), index+1));
		else
			m_list->InsertItem(index, wxT(""));

		m_list->SetItem(index, col_Filename, (*p)->filename);
		m_list->SetItem(index, col_Path, (*p)->path);

		index++;
	}

	m_list->PushEventHandler(new ListEventHandler(this));

	mainSizer->Add(m_list, 1, wxEXPAND);
	SetSizerAndFit(mainSizer);
}

CurrentTabsPopup::~CurrentTabsPopup() {
	if (this->HasCapture())
		this->ReleaseMouse();

	m_list->PopEventHandler(true);
}

int CurrentTabsPopup::GetSelectedTabIndex() const {
	return m_selectedTabIndex;
}

bool CurrentTabsPopup::WrapToNextItem(bool full_service) {
	// Wrap to top of the list if needed.
	int row = m_list->GetSelectedRow();
	int max = m_list->GetItemCount();

	if (row == max-1) { // Wrap the selection.
		m_list->SetSelectedRow(0);
		return true;
	}

	if (full_service) {
		m_list->SetSelectedRow(row+1);
		return true;
	}

	return false;
}

bool CurrentTabsPopup::WrapToPrevItem(bool full_service) {
	// Wrap to bottom of the list if needed.
	int row = m_list->GetSelectedRow();

	if (row == 0) { // Wrap the selection.
		int max = m_list->GetItemCount();
		m_list->SetSelectedRow(max-1);
		return true;
	}

	if (full_service) {
		m_list->SetSelectedRow(row-1);
		return true;
	}

	return false;
}

void CurrentTabsPopup::OnMouseLeftDown(wxMouseEvent& event) {
	wxPoint where = event.GetPosition();
	wxLogDebug(_("%d,%d"), where.x, where.y);

	// If we have capture and click outside the window, then dismiss as a CANCEL
	if (HasCapture()) {
		if (where.x < 0 || where.y < 0 || this->GetSize().x < where.x || this->GetSize().y < where.y) {
			ReleaseMouse();
			EndModal(wxID_CANCEL);
		}
	}
}

void CurrentTabsPopup::SelectRow(int row) {
	int max = m_list->GetItemCount();
	if (row < max) {
		m_selectedTabIndex = row;
		EndModal(wxID_OK);
	}
}

void CurrentTabsPopup::OnItemActivated(wxListEvent& WXUNUSED(event)) {
	long itemIndex = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (itemIndex == -1) {
		EndModal(wxID_CANCEL);
	}
	else {
		m_selectedTabIndex = itemIndex;
		EndModal(wxID_OK);
	}
}
