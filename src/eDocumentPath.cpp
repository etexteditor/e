#include "eDocumentPath.h"

#ifdef __WXMSW__
    #include "CygwinDlg.h"
	#include <wx/msw/registry.h>
#endif

#include "eApp.h"

eDocumentPath::eDocumentPath(void){}
eDocumentPath::~eDocumentPath(void){}

wxString eDocumentPath::WinPathToCygwin(const wxFileName& path) { 
	wxASSERT(path.IsOk() && path.IsAbsolute());

#ifndef __WXMSW__
    return path.GetFullPath();
#else
    wxString fullpath = path.GetFullPath();

	// Check if we have a foward-slash unc path; cygwin can handle these directly.
	if (fullpath.StartsWith(wxT("//"))) {
		return fullpath;
	}
	
	// Check if we have a backslash unc path; convert to forward slash and pass on.
	if (fullpath.StartsWith(wxT("\\\\"))) {
		for (unsigned int i = 0; i < fullpath.size(); ++i) {
			if (fullpath[i] == wxT('\\')) fullpath[i] = wxT('/');
		}
		return fullpath;
	}

	// Convert C:\... to /cygdrive/c/...
	wxString unixPath = eDocumentPath::s_cygdrivePrefix + path.GetVolume().Lower();

	// Convert slashs in path segments
	const wxArrayString& dirs = path.GetDirs();
	for (unsigned int i = 0; i < dirs.GetCount(); ++i) {
		unixPath += wxT('/') + dirs[i];
	}

	// Add the filename, if there is one
	if (path.HasName()) {
		unixPath += wxT('/') + path.GetFullName();
	}

	return unixPath;
#endif
}

#ifdef __WXMSW__

// Initialize static data.
bool eDocumentPath::s_isCygwinInitialized = false;
wxString eDocumentPath::s_cygPath;
wxString eDocumentPath::s_cygdrivePrefix = wxT("/cygdrive/");

wxString eDocumentPath::GetCygwinDir() { 
	wxString cygPath;

	// Check if we have a cygwin installation
	wxRegKey cygKey(wxT("HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/"));
	if( cygKey.Exists() && cygKey.HasValue(wxT("native"))) {
		cygKey.QueryValue(wxT("native"), cygPath);
	}

	if (!cygPath.empty())
		return cygPath;

	// Also check "current user" (might be needed if user did not have admin rights during install)
	wxLogDebug(wxT("CygPath: No key in HKEY_LOCAL_MACHINE"));

	wxRegKey cygKey2(wxT("HKEY_CURRENT_USER\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\/"));
	if( cygKey2.Exists() ) {
		wxLogDebug(wxT("CygPath: key exits in HKEY_CURRENT_USER"));

		if (cygKey2.HasValue(wxT("native"))) {
			wxLogDebug(wxT("CygPath: native exits in HKEY_CURRENT_USER"));
			cygKey2.QueryValue(wxT("native"), cygPath);
		}
	}

	return cygPath;
}

wxString eDocumentPath::GetCygdrivePrefix() { 
	wxString cygdrivePrefix;

	// Check if we have a cygwin installation
	wxRegKey cygKey(wxT("HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\"));
	if( cygKey.Exists() && cygKey.HasValue(wxT("cygdrive prefix"))) {
		cygKey.QueryValue(wxT("cygdrive prefix"), cygdrivePrefix);
	}

	if (!cygdrivePrefix.empty())
		return cygdrivePrefix;

	// Also check "current user" (might be needed if user did not have admin rights during install)
	wxLogDebug(wxT("CygPath: No key in HKEY_LOCAL_MACHINE"));

	wxRegKey cygKey2(wxT("HKEY_CURRENT_USER\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2\\"));
	if( cygKey2.Exists() ) {
		wxLogDebug(wxT("CygPath: key exits in HKEY_CURRENT_USER"));

		if (cygKey2.HasValue(wxT("cygdrive prefix"))) {
			wxLogDebug(wxT("CygPath: native exits in HKEY_CURRENT_USER"));
			cygKey2.QueryValue(wxT("cygdrive prefix"), cygdrivePrefix);
		}
	}

	return cygdrivePrefix;
}

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

// This function was taken from EditorFrame, but is not called from anywhere.
void eDocumentPath::ConvertPathToWin(wxString& path) {
	if (!path.StartsWith(wxT("/cygdrive/"))) return;

	// Get drive letter
	const wxChar drive = wxToupper(path[10]);
	if (drive < wxT('A') || drive > wxT('Z')) return;

	// Build new path
	wxString newpath(drive);
	newpath += wxT(':');
	if (path.size() > 11) newpath += path.substr(11);
	else newpath += wxT('\\');
	path = newpath;

	// Convert path seperators
	for (unsigned int i = 0; i < path.size(); ++i) {
		if (path[i] == wxT('/')) path[i] = wxT('\\');
	}
}

void eDocumentPath::InitCygwinOnce(CatalystWrapper& cw, wxWindow *parentWindow) {
	bool shouldPromptUserForCygUpdate = true;
	cxLOCK_READ(cw)
		catalyst.GetSettingBool(wxT("cygupdate"), shouldPromptUserForCygUpdate);
	cxENDLOCK

	// If user has previously chosen not to install/update cygwin, then
	// we will not bother him on startup (it will still show
	// up later if using a command that need cygwin).

	if (shouldPromptUserForCygUpdate && parentWindow)
		eDocumentPath::InitCygwin(cw, parentWindow);
}


bool eDocumentPath_shouldUpdateCygwin(wxDateTime &stampTime, const wxFileName &supportFile){
	// Check if we should update cygwin
	if (!stampTime.IsValid())
		return true; // first time

	wxDateTime updateTime = supportFile.GetModificationTime();

	// Windows does not really handle the minor parts of file dates
	updateTime.SetMillisecond(0);
	updateTime.SetSecond(0);

	stampTime.SetMillisecond(0);
	stampTime.SetSecond(0);

	if (updateTime == stampTime)
		return false;

	wxLogDebug(wxT("InitCygwin: Diff dates"));
	wxLogDebug(wxT("  e-postinstall: %s"), updateTime.FormatTime());
	wxLogDebug(wxT("  last-e-update: %s"), stampTime.FormatTime());
	return true;
}

void run_cygwin_dlg(CatalystWrapper& cw, wxWindow *parentWindow, cxCygwinDlgMode mode){
	// Notify user to install cygwin
	CygwinDlg dlg(parentWindow, cw, mode);

	const bool cygUpdate = (dlg.ShowModal() == wxID_OK);

	cxLOCK_WRITE(cw)
		catalyst.SetSettingBool(wxT("cygupdate"), cygUpdate);
	cxENDLOCK
}

//
// Checks to see if Cygwin is initalized, and prompts user to do it if not.
//
// Returns true if Cygwin is initialized, otherwise false.
//
bool eDocumentPath::InitCygwin(CatalystWrapper& cw, wxWindow *parentWindow, bool silent) {
	if (eDocumentPath::s_isCygwinInitialized)
		return true;

	// Check if we have a cygwin installation
	eDocumentPath::s_cygPath = eDocumentPath::GetCygwinDir();
	eDocumentPath::s_cygdrivePrefix = eDocumentPath::GetCygdrivePrefix();

	if (eDocumentPath::s_cygPath.empty()) {
		if (!silent) run_cygwin_dlg(cw, parentWindow, cxCYGWIN_INSTALL);
		return false;
	}

	const wxString supportPath = ((eApp*)wxTheApp)->GetAppPath() + wxT("support\\bin\\cygwin-post-install.sh");
	const wxFileName supportFile(supportPath);

	// Get last updatetime
	wxDateTime stampTime;
	cxLOCK_READ(cw)
		wxLongLong dateVal;
		if (catalyst.GetSettingLong(wxT("cyg_date"), dateVal)) stampTime = wxDateTime(dateVal);
	cxENDLOCK

	// In older versions it could be saved as filestamp
	if (!stampTime.IsValid()) {
		const wxFileName timestamp(eDocumentPath::s_cygPath + wxT("\\etc\\setup\\last-e-update"));
		if (timestamp.FileExists()) {
			stampTime = timestamp.GetModificationTime();

			// Save in new location
			cxLOCK_WRITE(cw)
				catalyst.SetSettingLong(wxT("cyg_date"), stampTime.GetValue());
			cxENDLOCK
		}
	}

	if (eDocumentPath_shouldUpdateCygwin(stampTime, supportFile)) {
		if (!silent) {
			run_cygwin_dlg(cw, parentWindow, cxCYGWIN_INSTALL);

			// Cancel the command that needed cygwin support, 
			// but let the user try again without getting this dialog
			eDocumentPath::s_isCygwinInitialized = true;
		}
		return false;
	}

	eDocumentPath::s_isCygwinInitialized = true;
	return true;
}

#endif // __WXMSW__

wxString eDocumentPath::ConvertPathToUncFileUrl(const wxString& path) {
	if (path.empty()) return wxEmptyString;

	wxString uncPath = path;

	uncPath.Replace(wxT(" "), wxT("%20"));
	uncPath.Replace(wxT("\\"), wxT("/"));
	uncPath.Prepend(wxT("file:///"));

	return uncPath;
}
