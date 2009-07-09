#ifndef __CURRENTTABSPOPUP_H__
#define __CURRENTTABSPOPUP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

class wxListCtrl;
class wxListEvent;

class CurrentTabsPopup: public wxDialog {
public:
	CurrentTabsPopup(wxWindow* parent, const wxPoint& pos);
	~CurrentTabsPopup();

private:
	DECLARE_EVENT_TABLE();

	class ListEventHandler : public wxEvtHandler {
	public:
		ListEventHandler(CurrentTabsPopup* parent): m_parent(parent){};

	private:
		DECLARE_EVENT_TABLE();
	
		void OnChar(wxKeyEvent& event);
		void OnItemActivated(wxListEvent& event);

		CurrentTabsPopup* m_parent;
	};

	void OnShow(wxShowEvent& event);
	wxListCtrl* m_list;
};

#endif
