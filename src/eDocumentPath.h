#ifndef __EDOCUMENTPATH_H__
#define __EDOCUMENTPATH_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/filename.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif // __WXMSW__

class eDocumentPath
{
public:
	eDocumentPath(void);
	~eDocumentPath(void);

#ifdef __WXMSW__
	static wxString GetCygwinDir();
	static wxString CygwinPathToWin(const wxString& path);
#endif // __WXMSW__

	static wxString WinPathToCygwin(const wxFileName& path);

	// Todo: these are public so change the naming style.
#ifdef __WXMSW__
	static bool s_isCygwinInitialized;
	static wxString s_cygPath;
#endif // __WXMSW__

};

#endif // __EDOCUMENTPATH_H__
