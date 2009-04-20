#include "eDocumentPath.h"

eDocumentPath::eDocumentPath(void)
{
}

eDocumentPath::~eDocumentPath(void)
{
}

// Initialize statics
#ifdef __WXMSW__
bool eDocumentPath::s_isCygwinInitialized = false;
wxString eDocumentPath::s_cygPath;
#endif //__WXMSW__


// static
wxString eDocumentPath::WinPathToCygwin(const wxFileName& path) { 
	wxASSERT(path.IsOk() && path.IsAbsolute());
#ifdef __WXMSW__
    wxString fullpath = path.GetFullPath();

	// Check if we have an unc path
	if (fullpath.StartsWith(wxT("//"))) {
		return fullpath; // cygwin can handle unc paths directly
	}
	
	if (fullpath.StartsWith(wxT("\\\\"))) {
		// Convert path seperators
		for (unsigned int i = 0; i < fullpath.size(); ++i) {
			if (fullpath[i] == wxT('\\')) fullpath[i] = wxT('/');
		}
		return fullpath; // cygwin can handle unc paths directly
	}

	// Quick conversion
	wxString unixPath = wxT("/cygdrive/");
	unixPath += path.GetVolume().Lower(); // Drive

	// Dirs
	const wxArrayString& dirs = path.GetDirs();
	for (unsigned int i = 0; i < dirs.GetCount(); ++i) {
		unixPath += wxT('/') + dirs[i];
	}

	// Filename
	if (path.HasName()) {
		unixPath += wxT('/') + path.GetFullName();
	}

	return unixPath;
#else
    return path.GetFullPath();
#endif
}

#ifdef __WXMSW__
// static
wxString eDocumentPath::GetCygwinDir() { 
	wxString cygPath;

	// Check if we have a cygwin installation
	wxRegKey cygKey(wxT("HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/"));
	if( cygKey.Exists() && cygKey.HasValue(wxT("native"))) {
		cygKey.QueryValue(wxT("native"), cygPath);
	}

	// Also check "current user" (might be needed if user did not have admin rights during install)
	if (cygPath.empty()) {
		wxLogDebug(wxT("CygPath: No key in HKEY_LOCAL_MACHINE"));

		wxRegKey cygKey2(wxT("HKEY_CURRENT_USER\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/"));
		if( cygKey2.Exists() ) {
			wxLogDebug(wxT("CygPath: key exits in HKEY_CURRENT_USER"));

			if (cygKey2.HasValue(wxT("native"))) {
				wxLogDebug(wxT("CygPath: native exits in HKEY_CURRENT_USER"));
				cygKey2.QueryValue(wxT("native"), cygPath);
			}
		}
	}

	return cygPath;
}

// static
wxString eDocumentPath::CygwinPathToWin(const wxString& path) { 
	if (path.empty()) {
		wxASSERT(false);
		return wxEmptyString;
	}

	wxString newpath;

	if (path.StartsWith(wxT("/cygdrive/"))) {
		// Get drive letter
		const wxChar drive = wxToupper(path[10]);
		if (drive < wxT('A') || drive > wxT('Z')) {
			wxASSERT(false);
			return wxEmptyString;
		}

		// Build new path
		newpath += drive;
		newpath += wxT(':');
		if (path.size() > 11) newpath += path.substr(11);
		else newpath += wxT('\\');
	}
	else if (path.StartsWith(wxT("/usr/bin/"))) {
		newpath = s_cygPath + wxT("\\bin\\");
		newpath += path.substr(9);
	}
	else if (path.StartsWith(wxT("/usr/lib/"))) {
		newpath = s_cygPath + wxT("\\lib\\");
		newpath += path.substr(9);
	}
	else if (path.StartsWith(wxT("//"))) {
		newpath = path; // unc path
	}
	else if (path.GetChar(0) == wxT('/')) {
		newpath = s_cygPath;
		newpath += path;
	}
	else return path; // no conversion

	// Convert path seperators
	for (unsigned int i = 0; i < newpath.size(); ++i) {
		if (newpath[i] == wxT('/')) newpath[i] = wxT('\\');
	}

	return newpath;
}

#endif // __WXMSW__
