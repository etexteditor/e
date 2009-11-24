#ifndef __CURRENTTABSPOPUP_H__
#define __CURRENTTABSPOPUP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

class wxListCtrlEx;
class wxListEvent;

struct OpenTabInfo {
	OpenTabInfo(const wxString& _filename, const wxString& _path): filename(_filename), path(_path) {};

	wxString filename;
	wxString path;
};

class CurrentTabsPopup: public wxDialog {
public:
	CurrentTabsPopup(wxWindow* parent, const std::vector<OpenTabInfo*>& tabInfo, int currrentTab=-1);
	~CurrentTabsPopup();

	int GetSelectedTabIndex() const;

	bool WrapToNextItem(bool full_service=false);
	bool WrapToPrevItem(bool full_service=false);
	void SelectRow(int row);

private:
	int GetSelectedRow() const;
	void SetSelectedRow(int selectedRow);

	DECLARE_EVENT_TABLE();
	void OnShow(wxShowEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	void OnItemActivated(wxListEvent& event);

	wxListCtrlEx* m_list;
	int m_selectedTabIndex;
};

#endif
