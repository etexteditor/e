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
	}

    event.Skip();
}

void CurrentTabsPopup::ListEventHandler::OnItemActivated(wxListEvent& event) {
	m_parent->EndModal(wxID_OK);
//    event.Skip();
}

BEGIN_EVENT_TABLE(CurrentTabsPopup, wxDialog)
	EVT_SHOW(CurrentTabsPopup::OnShow)
END_EVENT_TABLE()

void CurrentTabsPopup::OnShow(wxShowEvent& event) {
	m_list->SetFocus();
	m_list->SetItemState(0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
}

CurrentTabsPopup::CurrentTabsPopup(wxWindow* parent, const wxPoint& pos):
	wxDialog(parent, wxID_ANY, wxEmptyString, pos, wxDefaultSize, wxNO_BORDER)
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	m_list = new wxListCtrl(this, wxID_ANY, pos, wxSize(500,300), wxLC_REPORT|wxLC_SINGLE_SEL);
	m_list->InsertColumn(0, wxT("Filename"));
	m_list->InsertColumn(1, wxT("Path"));

	m_list->InsertItem(0, wxT("Item 1"));
	m_list->SetItem(0, 1, wxT("..\\django\\list.py"));
	m_list->InsertItem(1, wxT("Item 2"));
	m_list->SetItem(1, 1, wxT("..\\..\\django\\urls\\views.py"));

	m_list->PushEventHandler(new ListEventHandler(this));

	mainSizer->Add(m_list, 1, wxEXPAND);

	SetSizerAndFit(mainSizer);
}

CurrentTabsPopup::~CurrentTabsPopup() {
	m_list->PopEventHandler(true);
}
