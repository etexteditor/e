#ifndef __IEDITORDOACTION_H__
#define __IEDITORDOACTION_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

class IEditorDoAction {
public:
	virtual void DoAction(const tmAction& action, const map<wxString, wxString>* envVars, bool isRaw) = 0;
};

#endif // __IEDITORDOACTION_H__
