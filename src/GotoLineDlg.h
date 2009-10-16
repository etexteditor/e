#ifndef __GOTOLINEDLG_H__
#define __GOTOLINEDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

class wxSpinCtrl;

class GotoLineDlg : public wxDialog {
public:
	GotoLineDlg(wxWindow *parent, unsigned int pos, unsigned int min, unsigned int max);
	unsigned int GetLine() const;

private:
	void OnEnter(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	wxSpinCtrl* m_lineCtrl;
};
#endif // __GOTOLINEDLG_H__
