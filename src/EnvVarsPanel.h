#ifndef __ENVVARSPANEL_H__
#define __ENVVARSPANEL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(disable:4786)
    #pragma warning(push, 1)
#endif
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class wxGrid;
class wxGridEvent;

class EnvVarsPanel : public wxPanel
{
public:
	EnvVarsPanel(wxWindow*parent, wxWindowID id = wxID_ANY);
	virtual ~EnvVarsPanel(void);

	void AddVars(const map<wxString,wxString>& vars);
	void GetVars(map<wxString,wxString>& vars);

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
