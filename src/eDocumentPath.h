#ifndef __EDOCUMENTPATH_H__
#define __EDOCUMENTPATH_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

#include <wx/filename.h>

#ifdef __WXMSW__
typedef DWORD FILE_PERMISSIONS;
#else
typedef mode_t FILE_PERMISSIONS;
#endif

class cxEnv;

class eDocumentPath
{
public:
	eDocumentPath(void);
	~eDocumentPath(void);

	static wxString WinPathToCygwin(const wxFileName& path);

	static wxString ConvertPathToUncFileUrl(const wxString& path);

	static bool IsBundlePath(const wxString& path);
	static bool IsRemotePath(const wxString& path);

	static bool IsDotDirectory(const wxString& path);

	static bool MakeWritable(const wxString& path);
	static FILE_PERMISSIONS GetPermissions(const wxString& path);
	static bool SetPermissions(const wxString& path, FILE_PERMISSIONS permissions);

#ifdef __WXMSW__
	static bool MakeHidden(const wxString& path);

	static wxString GetCygwinDir();
	static wxString GetCygdrivePrefix();

	static wxString CygwinPathToWin(const wxString& path);

	static bool InitCygwin(bool silent=false);
	static void InitCygwinOnce();

	static bool IsInitialized() { return s_isCygwinInitialized; }
	static wxString CygdrivePrefix() { return s_cygdrivePrefix; }
	static wxString CygwinPath() { return s_cygPath; }

private:
	static wxString convert_cygdrive_path_to_windows(const wxString& path);

	static bool s_isCygwinInitialized;
	static wxString s_cygdrivePrefix;
	static wxString s_cygPath;

#endif // __WXMSW__
};

#endif // __EDOCUMENTPATH_H__
