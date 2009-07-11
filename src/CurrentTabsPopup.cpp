#include "CurrentTabsPopup.h"
#include "wxListCtrlEx.h"

BEGIN_EVENT_TABLE(CurrentTabsPopup::ListEventHandler, wxEvtHandler)
	EVT_CHAR(CurrentTabsPopup::ListEventHandler::OnChar)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, CurrentTabsPopup::ListEventHandler::OnItemActivated)
END_EVENT_TABLE()

void CurrentTabsPopup::ListEventHandler::OnChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_ESCAPE:
		// Cancel tab selection.
		m_parent->EndModal(wxID_CANCEL);
		return;

	case WXK_UP: {
		// Wrap to bottom of the list if needed.
		int row = m_parent->m_list->GetSelectedRow();
		if (row == 0) {
			int max = m_parent->m_list->GetItemCount();
			m_parent->m_list->SetSelectedRow(max-1);
			return;
		}
		break;
	 }

	case WXK_DOWN: {
		// Wrap to top of the list if needed.
		int row = m_parent->m_list->GetSelectedRow();
		int max = m_parent->m_list->GetItemCount();
		if (row == max -1) {
			m_parent->m_list->SetSelectedRow(0);
			return;
		}
		break;
   }

	default:
		const wxChar c = event.GetUnicodeKey();
		// If a number 1-9 was pressed, and we have that many rows,
		// then choose that (-1) as the selected row index.
		if ((wxT('1') <= c) && ( c <= wxT('9'))) {
			int max = m_parent->m_list->GetItemCount();

			int choice = c-wxT('1');
			if (choice < max) {
				m_parent->m_selectedTabIndex = choice;
				m_parent->EndModal(wxID_OK);
			}
			return;
		}
	}

    event.Skip();
}

void CurrentTabsPopup::ListEventHandler::OnItemActivated(wxListEvent& WXUNUSED(event)) {
	long itemIndex = m_parent->m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (itemIndex == -1) {
		m_parent->EndModal(wxID_CANCEL);
	}
	else {
		m_parent->m_selectedTabIndex = itemIndex;
		m_parent->EndModal(wxID_OK);
	}
}

BEGIN_EVENT_TABLE(CurrentTabsPopup, wxDialog)
	EVT_SHOW(CurrentTabsPopup::OnShow)
END_EVENT_TABLE()

void CurrentTabsPopup::OnShow(wxShowEvent& WXUNUSED(event)) {
	m_list->SetFocus();
	m_list->SetSelectedRow(0);

	for (int i = 0; i < CurrentTabsPopup::ColumnCount; i++)
		m_list->SetColumnWidth(i, wxLIST_AUTOSIZE);
}

CurrentTabsPopup::CurrentTabsPopup(wxWindow* parent, const std::vector<OpenTabInfo*>& tabInfo):
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_selectedTabIndex(-1)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	m_list = new wxListCtrlEx(this, wxID_ANY, wxPoint(0,0), wxSize(500,300), wxLC_REPORT|wxLC_SINGLE_SEL);
	m_list->InsertColumn(CurrentTabsPopup::Number, wxT("#"));
	m_list->InsertColumn(CurrentTabsPopup::Filename, wxT("Filename"));
	m_list->InsertColumn(CurrentTabsPopup::Path, wxT("Path"));

	int index = 0;
	for (std::vector<OpenTabInfo*>::const_iterator p = tabInfo.begin(); p != tabInfo.end(); ++p) {
		if (index < 9)
			m_list->InsertItem(index, wxString::Format(wxT("%d"), index+1));
		else
			m_list->InsertItem(index, wxT(""));

		m_list->SetItem(index, CurrentTabsPopup::Filename, (*p)->filename);
		m_list->SetItem(index, CurrentTabsPopup::Path, (*p)->path);

		index++;
	}

	m_list->PushEventHandler(new ListEventHandler(this));

	mainSizer->Add(m_list, 1, wxEXPAND);
	SetSizerAndFit(mainSizer);
}

CurrentTabsPopup::~CurrentTabsPopup() {
	m_list->PopEventHandler(true);
}

int CurrentTabsPopup::GetSelectedTabIndex() const {
	return m_selectedTabIndex;
}
