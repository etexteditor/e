#ifndef _EAUINOTEBOOK_H_
#define _EAUINOTEBOOK_H_

#include "wx/aui/auibook.h"

class eAuiNotebook : public wxAuiNotebook {
public:
	eAuiNotebook(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE);

	int PageToTab(size_t page_idx);
    int TabToPage(size_t tab_idx);

	wxString SavePerspective();
	bool LoadPerspective(const wxString& layout);
};

#endif //_EAUINOTEBOOK_H_
