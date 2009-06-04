#ifndef __ENVVARSPANEL_H__
#define __ENVVARSPANEL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/dialog.h>
#endif

class wxGrid;
class wxGridEvent;

class EnvVarsPanel : public wxPanel
{
public:
	EnvVarsPanel(wxWindow*parent, wxWindowID id = wxID_ANY);
	virtual ~EnvVarsPanel(void);

	bool VarsChanged(){return m_varsChanged;}

private:
	void OnButtonAddEnv(wxCommandEvent& event);
	void OnButtonDelEnv(wxCommandEvent& event);
	void OnGridChange(wxGridEvent& event);
	DECLARE_EVENT_TABLE()

	wxGrid* m_envList;
	bool m_varsChanged;
};

#endif
