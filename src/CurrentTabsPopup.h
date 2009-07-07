#ifndef __CURRENTTABSPOPUP_H__
#define __CURRENTTABSPOPUP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

class CurrentTabsPopup: public wxDialog {
public:
	CurrentTabsPopup(wxWindow* parent, const wxPoint& pos);
};

#endif
