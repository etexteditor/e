#ifndef __WXLISTCTRLEX_H__
#define __WXLISTCTRLEX_H__
#include <wx/listctrl.h>

class wxListCtrlEx: public wxListCtrl {
public:
	wxListCtrlEx();
	wxListCtrlEx(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxListCtrlNameStr);
	virtual ~wxListCtrlEx(void);

	/* Returns the first selected row index, or -1 if no row is selected. */
	int GetSelectedRow() const;

	/* Sets the given row to be the only selected & focused row. */
	void SetSelectedRow(int selectedRow, bool ensure_visible=true);
};

#endif
