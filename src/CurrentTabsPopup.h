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
	CurrentTabsPopup(wxWindow* parent, const std::vector<OpenTabInfo*>& tabInfo);
	~CurrentTabsPopup();

	int GetSelectedTabIndex() const;

private:
	enum Columns {
		Number = 0,
		Filename,
		Path,
		ColumnCount
	};

	int GetSelectedRow() const;
	void SetSelectedRow(int selectedRow);

	class ListEventHandler : public wxEvtHandler {
	public:
		ListEventHandler(CurrentTabsPopup* parent): m_parent(parent){};

	private:
		DECLARE_EVENT_TABLE();
		void OnChar(wxKeyEvent& event);
		void OnItemActivated(wxListEvent& event);

		CurrentTabsPopup* m_parent;
	};

	DECLARE_EVENT_TABLE();
	void OnShow(wxShowEvent& event);

	wxListCtrlEx* m_list;
	int m_selectedTabIndex;
};

#endif
