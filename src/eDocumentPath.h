#ifndef __EDOCUMENTPATH_H__
#define __EDOCUMENTPATH_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include <wx/filename.h>

class eDocumentPath
{
public:
	eDocumentPath(void);
	~eDocumentPath(void);

	static wxString WinPathToCygwin(const wxFileName& path);
	static void ConvertPathToWin(wxString& path);

#ifdef __WXMSW__

	static wxString GetCygwinDir();
	static wxString CygwinPathToWin(const wxString& path);

	static bool InitCygwin(CatalystWrapper& cw, wxWindow *parentWindow, bool silent=false);
	static void InitCygwinOnce(CatalystWrapper& cw, wxWindow *parentWindow);

	// Todo: these are public so change the naming style.
	static wxString s_cygPath;

private:
	static bool s_isCygwinInitialized;

#endif // __WXMSW__
};

#endif // __EDOCUMENTPATH_H__
