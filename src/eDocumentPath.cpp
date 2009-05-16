#include "eDocumentPath.h"

#ifdef __WXMSW__
    #include "CygwinDlg.h"
	#include <wx/msw/registry.h>
#endif

// Needed to get app path, to find the post-cygwin install script
#include "IAppPaths.h"
#include "eSettings.h"

eDocumentPath::eDocumentPath(void){}
eDocumentPath::~eDocumentPath(void){}

bool eDocumentPath::MakeWritable(const wxString& path) {
#ifdef __WXMSW__
		DWORD dwAttrs = ::GetFileAttributes(path);
		if (dwAttrs & FILE_ATTRIBUTE_READONLY) {
			dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
			return SetFileAttributes(path, dwAttrs) != 0;
		}
		else return true;
#else
        // Get protection
		struct stat s;
		int res = stat(path.mb_str(wxConvUTF8), &s);
		if (res == 0) res = chmod(path.mb_str(wxConvUTF8), s.st_mode |= S_IWUSR);
		return res == 0;
#endif
}

bool eDocumentPath::IsDotDirectory(const wxString& path) {
	return path == wxT(".") || path == wxT("..");
}

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
		fullpath.Replace(wxT("\\"), wxT("/"));
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
	if (path.HasName()) unixPath += wxT('/') + path.GetFullName();

	return unixPath;
#endif
}

#ifdef __WXMSW__

// Initialize static data.
bool eDocumentPath::s_isCygwinInitialized = false;
wxString eDocumentPath::s_cygPath;
wxString eDocumentPath::s_cygdrivePrefix = wxT("/cygdrive/");

// Reads a value from the Cygwin registry key, trying both HKLM and HKCU.
wxString read_cygwin15_registry_key(const wxString &key_path, const wxString &key, const wxString &default_value = wxEmptyString) {
	wxString value;

	// Check "current user" ('just me').
	{
		wxRegKey cygKey(wxT("HKEY_CURRENT_USER\\SOFTWARE\\Cygnus Solutions\\Cygwin\\") + key_path);
		if( cygKey.Exists()  && cygKey.HasValue(key)) {
			cygKey.QueryValue(key, value);
		}
	}

	if (!value.empty())
		return value;
	
	// Also check in "local machine", the "install for everyone" location.
	{
		wxRegKey cygKey(wxT("HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\") + key_path);
		if( cygKey.Exists() && cygKey.HasValue(key)) {
			cygKey.QueryValue(key, value);
		}
	}

	if (!value.empty())
		return value;

	return default_value;
}

wxString eDocumentPath::GetCygwinDir() {
	return read_cygwin15_registry_key(wxT("mounts v2\\/"), wxT("native"));
}

wxString eDocumentPath::GetCygdrivePrefix() { 
	// Note that the registory value doesn't have '/' on the end, and we want one.
	return read_cygwin15_registry_key(wxT("mounts v2"), wxT("cygdrive prefix"), wxT("/cygdrive")) + wxT("/");
}

//
// This code assumes that we've already checked for special cases
// involving a cygdrive prefix of '/' and a path that isn't a drive path.
//
// The given path is assumed to already start with cygdrive prefix
//
wxString eDocumentPath::convert_cygdrive_path_to_windows(const wxString& path) {
	const size_t n = eDocumentPath::s_cygdrivePrefix.Len(); // Cygdrive prefix length

	// Get drive letter
	const wxChar drive = wxToupper(path[n]);
	if (drive < wxT('A') || wxT('Z') < drive) {
		// Invalid drive letter; can't do anything with this.
		wxASSERT(false);
		return wxEmptyString;
	}

	wxString newpath;
	newpath += drive;
	newpath += wxT(":\\");

	// Add stuff after the cygdrive+drive letter to the new path, else just add a slash.
	if (path.size() > n+2) newpath += path.substr(n+2);
	else newpath += wxT('\\');

	newpath.Replace(wxT("/"), wxT("\\"));
	return newpath;
}

wxString eDocumentPath::CygwinPathToWin(const wxString& path) { 
	if (path.empty()) {
		wxASSERT(false);
		return wxEmptyString;
	}

	wxString newpath;

	// Map cygdrive paths to standard Windows drive-letter.
	if (s_cygdrivePrefix == wxT("/")) {
		const size_t path_len = path.Len();

		// path looks like: /q or /q/ or /q/stuff
		if (path_len == 2 || ((path_len >= 3) && (path[2] == wxT('/'))))
			return convert_cygdrive_path_to_windows(path);

		// If we got here, then don't convert the path, and let the StartsWith cases
		// in the other top-level ifs happen.
	}
	else {
		if (path.StartsWith(s_cygdrivePrefix))
			return convert_cygdrive_path_to_windows(path);
	}

	// For /usr/bin and /usr/lib, techincally we should be mapping anything that
	// appears in the registry under cygwin mounts.
	//
	// Cygwin 1.7 will be changing this behavior, though, and storing mount information
	// in fstab.
	//
	// For now, keep hard coding these mounts and worry about playing will with existing
	// installations (that e didn't do) in the future.
	
	// Map /usr/bin/ paths to Cygwin bin folder
	if (path.StartsWith(wxT("/usr/bin/"))) {
		newpath = s_cygPath + wxT("\\bin\\") + path.substr(9);
		newpath.Replace(wxT("/"), wxT("\\"));
		return newpath;
	}
	
	// Map /usr/lib paths to Cygwin lib folder
	if (path.StartsWith(wxT("/usr/lib/"))) {
		newpath = s_cygPath + wxT("\\lib\\") + path.substr(9);
		newpath.Replace(wxT("/"), wxT("\\"));
		return newpath;
	}
	
	// Check for UNC paths
	if (path.StartsWith(wxT("//"))) {
		newpath = path;
		newpath.Replace(wxT("/"), wxT("\\"));
		return newpath;
	}
	
	// Cygwin paths that aren't cygdrive paths get mapped to cygwin's native install folder
	if (path.GetChar(0) == wxT('/')) {
		newpath = s_cygPath + path;
		newpath.Replace(wxT("/"), wxT("\\"));
		return newpath;
	}
	
	// If we got here, then don't convert the path.
	return path;
}

void eDocumentPath::InitCygwinOnce() {
	bool shouldPromptUserForCygUpdate = true;
	eGetSettings().GetSettingBool(wxT("cygupdate"), shouldPromptUserForCygUpdate);

	// If user has previously chosen not to install/update cygwin, then
	// we will not bother him on startup (it will still show
	// up later if using a command that need cygwin).

	if (shouldPromptUserForCygUpdate && wxTheApp->GetTopWindow())
		eDocumentPath::InitCygwin();
}

//
// Returns true if this Cygwin installation should be updated (or installed for the first time.)
// Returns false if we are up-to-date.
//
bool eDocumentPath_shouldUpdateCygwin(wxDateTime &stampTime, const wxFileName &supportFile){
	// E's support folder comes with a network installer for Cygwin.
	// Newer versions of E may come with newer Cygwin versions.
	// If the user already has Cygwin installed, we still check the
	// bundled installer to see if it is newer; if so, then we need to
	// re-install Cygwin at the newer version.

	if (!stampTime.IsValid())
		return true; // First time, so we need to update.

	wxDateTime updateTime = supportFile.GetModificationTime();

	// Windows doesn't store milliseconds; we clear out this part of the time.
	updateTime.SetMillisecond(0);
	updateTime.SetSecond(0);

	stampTime.SetMillisecond(0);
	stampTime.SetSecond(0);

	// If the times are the same, no update needed.
	if (updateTime == stampTime)
		return false;

	// ...else the dates differ and we need to update.
	wxLogDebug(wxT("InitCygwin: Diff dates"));
	wxLogDebug(wxT("  e-postinstall: %s"), updateTime.FormatTime());
	wxLogDebug(wxT("  last-e-update: %s"), stampTime.FormatTime());
	return true;
}

void run_cygwin_dlg(wxWindow *parentWindow, const cxCygwinDlgMode mode){
	// Notify user to install cygwin
	CygwinDlg dlg(parentWindow, mode);

	const bool cygUpdate = (dlg.ShowModal() == wxID_OK);
	eGetSettings().SetSettingBool(wxT("cygupdate"), cygUpdate);
}

// 
// Gets the cygwin last update time, migrating state form previous e versions if needed.
//
wxDateTime get_last_cygwin_update() {
	wxDateTime stampTime;
	wxLongLong dateVal;
	eSettings& settings = eGetSettings();
	if (settings.GetSettingLong(wxT("cyg_date"), dateVal)) stampTime = wxDateTime(dateVal);

	// In older versions it could be saved as filestamp
	if (!stampTime.IsValid()) {
		const wxFileName timestamp(eDocumentPath::CygwinPath() + wxT("\\etc\\setup\\last-e-update"));
		if (timestamp.FileExists()) {
			stampTime = timestamp.GetModificationTime();

			// Save in new location
			settings.SetSettingLong(wxT("cyg_date"), stampTime.GetValue());
		}
	}

	return stampTime;
}

//
// Checks to see if Cygwin is initalized, and prompts user to do it if not.
// Returns true if Cygwin is initialized, otherwise false.
//
bool eDocumentPath::InitCygwin(const bool silent) {
	if (eDocumentPath::s_isCygwinInitialized)
		return true;

	// Check if we have a cygwin installation and read out configuration
	eDocumentPath::s_cygPath = eDocumentPath::GetCygwinDir();
	eDocumentPath::s_cygdrivePrefix = eDocumentPath::GetCygdrivePrefix();

	const bool cygwin_is_installed = !s_cygPath.empty();

	if (!cygwin_is_installed) {
		if (!silent) run_cygwin_dlg(wxTheApp->GetTopWindow(), cxCYGWIN_INSTALL);
		return false;
	}

	// Get last cygwin update, and see if we need to reinstall
	const wxString supportPath = dynamic_cast<IAppPaths*>(wxTheApp)->GetAppPath() + wxT("support\\bin\\cygwin-post-install.sh");
	const wxFileName supportFile(supportPath);

	wxDateTime stampTime = get_last_cygwin_update();

	if (eDocumentPath_shouldUpdateCygwin(stampTime, supportFile)) {
		if (!silent) {
			run_cygwin_dlg(wxTheApp->GetTopWindow(), cxCYGWIN_UPDATE);

			// Cancel the command that needed cygwin support (return false below.)
			// But, since we have an older version of Cygwin installed,
			// claim that we have been initialized anyway.
			//
			// The user may retry the command, and it will work assuming it doesn't
			// rely on new Cygwin behavior.
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

bool eDocumentPath::IsRemotePath(const wxString& path) {
	return path.StartsWith(wxT("ftp://")) || path.StartsWith(wxT("http://"));
}

bool eDocumentPath::IsBundlePath(const wxString& path) {
	return (path.StartsWith(wxT("bundle://")));
}

wxString eDocumentPath::GetAppDataTempPath() {
	// Create temp buffer file
	const wxString tempPath = dynamic_cast<IAppPaths*>(wxTheApp)->GetAppDataPath() + wxT("temp") + wxFILE_SEP_PATH;
	if (!wxDirExists(tempPath)) wxMkdir(tempPath);
	return wxFileName::CreateTempFileName(tempPath);
}
