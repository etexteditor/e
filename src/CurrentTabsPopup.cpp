#include "CurrentTabsPopup.h"
#include <wx/listctrl.h>

BEGIN_EVENT_TABLE(CurrentTabsPopup::ListEventHandler, wxEvtHandler)
	EVT_CHAR(CurrentTabsPopup::ListEventHandler::OnChar)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, CurrentTabsPopup::ListEventHandler::OnItemActivated)
END_EVENT_TABLE()

void CurrentTabsPopup::ListEventHandler::OnChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_ESCAPE:
		m_parent->EndModal(wxID_CANCEL);
		return;

	case WXK_UP: {
		int row = m_parent->GetSelectedRow();
		if (row == 0) {
			int max = m_parent->m_list->GetItemCount();
			m_parent->SetSelectedRow(max-1);
			return;
		}
		break;
	 }

	case WXK_DOWN: {
		int row = m_parent->GetSelectedRow();
		int max = m_parent->m_list->GetItemCount();
		if (row == max -1) {
			m_parent->SetSelectedRow(0);
			return;
		}
		break;
   }

	default:
		const wxChar c = event.GetUnicodeKey();
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
	m_list->SetItemState(0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);

	for (int i = 0; i < CurrentTabsPopup::ColumnCount; i++)
		m_list->SetColumnWidth(i, wxLIST_AUTOSIZE);
}

CurrentTabsPopup::CurrentTabsPopup(wxWindow* parent, const std::vector<OpenTabInfo*>& tabInfo):
	wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_selectedTabIndex(-1)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	m_list = new wxListCtrl(this, wxID_ANY, wxPoint(0,0), wxSize(500,300), wxLC_REPORT|wxLC_SINGLE_SEL);
	m_list->InsertColumn(CurrentTabsPopup::Number, wxT("#"));
	m_list->InsertColumn(CurrentTabsPopup::Filename, wxT("Filename"));
	m_list->InsertColumn(CurrentTabsPopup::Path, wxT("Path"));

	int index = 0;
	for (std::vector<OpenTabInfo*>::const_iterator p = tabInfo.begin(); p != tabInfo.end(); ++p) {
		if (index < 9)
			m_list->InsertItem(index, wxString::Format(wxT("%d"), index+1));
		else
			m_list->InsertItem(index, wxT("#"));

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

int CurrentTabsPopup::GetSelectedRow() const {
	long itemIndex = m_list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	return itemIndex;
}

/*
// Deselect item (wxLIST_STATE_FOCUSED - dotted border)
wxListCtrl->SetItemState(item, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
 
// Select item
wxListCtrl->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
*/

void CurrentTabsPopup::SetSelectedRow(int selectedRow) {
	int count = m_list->GetItemCount();
	for (int i = 0; i < count; i++) {
		long flags = (i==selectedRow)?wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED : 0;
		m_list->SetItemState(i, flags, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
	}
}
