#include "wxListCtrlEx.h"

wxListCtrlEx::wxListCtrlEx(): wxListCtrl(){}

wxListCtrlEx::wxListCtrlEx(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name):
	wxListCtrl(parent, id, pos, size, style, validator, name) {}

wxListCtrlEx::~wxListCtrlEx(void){}

int wxListCtrlEx::GetSelectedRow() const {
	long itemIndex = this->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	return itemIndex;
}

void wxListCtrlEx::SetSelectedRow(int selectedRow, bool ensure_visible) {
	int count = this->GetItemCount();
	for (int i = 0; i < count; i++) {
		long flags = (i==selectedRow)?wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED : 0;
		this->SetItemState(i, flags, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
	}

	if (ensure_visible)
		this->EnsureVisible(selectedRow);
}
