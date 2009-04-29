/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#include "BundleManager.h"
#include "EditorFrame.h"
#include <wx/filename.h>
#include "eApp.h"
#include <wx/progdlg.h>
#include <wx/stdpaths.h>
#include <wx/ffile.h>
#include "urlencode.h"

#include "images/accept.xpm"
#include "images/exclamation.xpm"
#include "images/empty.xpm"

// tinyxml includes unused vars so it can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include "tinyxml.h"
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

// control id's
enum
{
	ID_HTML_DESC = 100,
	ID_BUNDLELIST,
	ID_INSTALLBUTTON,
	ID_DELETEBUTTON
};

// Init static vars
const wxString BundleManager::s_emptyString; // to avoid return temp warning

BEGIN_EVENT_TABLE(BundleManager, wxDialog)
	EVT_REMOTELIST_RECEIVED(BundleManager::OnRemoteListReceived)
	EVT_REMOTEACTION(BundleManager::OnRemoteAction)
	EVT_LIST_ITEM_SELECTED(ID_BUNDLELIST, BundleManager::OnItemSelected)
	EVT_BUTTON(ID_INSTALLBUTTON, BundleManager::OnInstallButton)
	EVT_BUTTON(ID_DELETEBUTTON, BundleManager::OnDeleteButton)
	EVT_CLOSE(BundleManager::OnClose)
	EVT_HTMLWND_BEFORE_LOAD(ID_HTML_DESC, BundleManager::OnBeforeLoad)
END_EVENT_TABLE()

BundleManager::BundleManager(EditorFrame& parent, TmSyntaxHandler& syntaxHandler)
: wxDialog (&parent, -1, _("Manage Bundles"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
  m_parentFrame(parent), m_remoteThread(parent.GetRemoteThread()), m_syntaxHandler(syntaxHandler), m_plistHandler(m_syntaxHandler.GetPListHandler()),
  m_allBundlesReceived(false), m_needBundleReload(false)
{
	m_repositories.push_back(RepoInfo(wxT("review mm"), wxT("http://macromates.com/svn/Bundles/trunk/Review/Bundles/")));
	m_repositories.push_back(RepoInfo(wxT("macromates"), wxT("http://macromates.com/svn/Bundles/trunk/Bundles/")));
	m_repositories.push_back(RepoInfo(wxT("ebundles"), wxT("http://ebundles.googlecode.com/svn/trunk/Bundles/")));

	wxImageList* imageList = new wxImageList(16, 16, true, 0);
	imageList->Add(wxIcon(empty_xpm));
	imageList->Add(wxIcon(accept_xpm));
	imageList->Add(wxIcon(exclamation_xpm));

	// Create the controls
	m_bundleList = new wxListCtrl(this, ID_BUNDLELIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL);
	m_bundleList->InsertColumn(0, _("Bundles"));
	m_bundleList->SetColumnWidth(0, 400);
	m_bundleList->InsertColumn(1, _("Last Updated"));
	m_bundleList->InsertColumn(2, _("Source"));
	m_bundleList->AssignImageList(imageList, wxIMAGE_LIST_SMALL);

#ifdef __WXMSW__
	m_browser = new wxIEHtmlWin(this, ID_HTML_DESC);
#endif

	wxStaticText* statusLabel = new wxStaticText(this, wxID_ANY, _("Status: "));
	m_statusText = new wxStaticText(this, wxID_ANY, wxT(""));
	m_installButton = new wxButton(this, ID_INSTALLBUTTON, _("Install"));
	m_installButton->Hide();
	m_deleteButton = new wxButton(this, ID_DELETEBUTTON, _("Delete"));
	m_deleteButton->Hide();

	// Let the user know that we are downloading bundle info
	m_bundleList->InsertItem(0, _("Retrieving Bundles..."));
	m_bundleList->SetItemTextColour(0, *wxLIGHT_GREY);

	// Create the layout
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
		m_mainSizer->Add(m_bundleList, 4, wxEXPAND|wxALL, 5);
		wxSizer* statusSizer = new wxBoxSizer(wxHORIZONTAL);
			statusSizer->Add(statusLabel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
			statusSizer->Add(m_statusText, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
			statusSizer->AddStretchSpacer();
			statusSizer->Add(m_installButton, 0, wxALIGN_RIGHT|wxLEFT, 5);
			statusSizer->Add(m_deleteButton, 0, wxALIGN_RIGHT|wxLEFT, 5);
			m_mainSizer->Add(statusSizer, 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
#ifdef __WXMSW__
		m_mainSizer->Add(m_browser, 1, wxEXPAND|wxALL, 5);
#endif

	SetSizerAndFit(m_mainSizer);
	SetSize(600, 600);
	Centre();

	LoadBundleLists();
}

BundleManager::~BundleManager() {
	// Make sure no events are sent to this win after deletion
	m_remoteThread.RemoveEventHandler(*this);
}

void BundleManager::LoadBundleLists() {
	m_bundleList->SetCursor(wxCURSOR_WAIT);

	// Get lists from online repositories
	RemoteProfile rp;
	for (vector<RepoInfo>::const_iterator p = m_repositories.begin(); p != m_repositories.end(); ++p) {
		m_remoteThread.GetRemoteList(p->url, rp, *this);
	}

	// Get list of installed bundles
	m_installedBundles = m_plistHandler.GetInstalledBundlesInfo();
}

int wxCALLBACK CompareNameFunction(long item1, long item2, long WXUNUSED(sortData))
{
    cxFileInfo* fi1 = (cxFileInfo*)item1;
	cxFileInfo* fi2 = (cxFileInfo*)item2;

	return fi1->m_name.CmpNoCase(fi2->m_name);
}

void BundleManager::UpdateBundleList() {
	Freeze();
	m_bundleList->DeleteAllItems();

	// Insert bundles in ListCtrl
	for (vector<RepoInfo>::const_iterator p = m_repositories.begin(); p != m_repositories.end(); ++p) {
		AddItems(p->name, p->bundleList);
	}

	m_bundleList->SortItems(CompareNameFunction, 0);

	Thaw();
} 

const wxString& BundleManager::GetCurrentRepoUrl() const {
	wxASSERT(!m_currentRepo.empty());
	
	for (vector<RepoInfo>::const_iterator p = m_repositories.begin(); p != m_repositories.end(); ++p) {
		if (p->name == m_currentRepo) {
			return p->url;
		}
	}

	return s_emptyString;
}

void BundleManager::AddItems(const wxString& repoName, const vector<cxFileInfo>& bundleList) {
	for (unsigned int i = 0; i < bundleList.size(); ++i) {
		const cxFileInfo& p = bundleList[i];
		const wxString bundleName = p.m_name.BeforeLast(wxT('.'));

		// Check if it has already been inserted
		long itemId = m_bundleList->FindItem(0, bundleName);
		if (itemId == -1) itemId = m_bundleList->InsertItem(m_bundleList->GetItemCount(), bundleName);

		const wxString age = Catalyst::GetDateAge(p.m_modDate);
		m_bundleList->SetItem(itemId, 1, age);
		m_bundleList->SetItem(itemId, 2, repoName);
		m_bundleList->SetItemData(itemId, (long)&p);

		// Check if bundle is installed
		for (vector<PListHandler::cxBundleInfo>::const_iterator b = m_installedBundles.begin(); b != m_installedBundles.end(); ++b) {
			if (b->dirName == p.m_name) {
				if (!b->isDisabled) {
					// We can't count on the filesystem preserving milliseconds
					wxDateTime installedDate = b->modDate;
					wxDateTime remoteDate = p.m_modDate;
					installedDate.SetSecond(0);
					installedDate.SetMillisecond(0);
					remoteDate.SetSecond(0);
					remoteDate.SetMillisecond(0);

					m_bundleList->SetItemImage(itemId, (installedDate == remoteDate) ? 1 : 2);
				}
				break;
			}
		}
	}
}

void BundleManager::OnClose(wxCloseEvent& WXUNUSED(event)) {
	Hide();

	// Show user that we are reloading
	wxBusyCursor wait;

	if (m_needBundleReload) {
		m_syntaxHandler.LoadBundles(TmSyntaxHandler::cxUPDATE);
	}
}

void BundleManager::OnRemoteListReceived(cxRemoteListEvent& event) {
	if (m_allBundlesReceived) return;

	// Handle errors
	if (event.GetErrorCode() != CURLE_OK) {
		m_allBundlesReceived = true; // avoid double errors

		wxMessageBox(event.GetUrl() + wxT("\n") + event.GetError(), _("Download Error"), wxICON_ERROR|wxOK, this);
		m_bundleList->DeleteAllItems();
		m_bundleList->SetCursor(wxCURSOR_ARROW); // reset busy
	}

	// Get the bundle list
	for (vector<RepoInfo>::iterator p = m_repositories.begin(); p != m_repositories.end(); ++p) {
		if (event.GetUrl() == p->url) {
			p->bundleList = event.GetFileList();
			p->received = true;
			break;
		}
	}
	
	// Check if we have received all bundles
	m_allBundlesReceived = true;
	for (vector<RepoInfo>::const_iterator p2 = m_repositories.begin(); p2 != m_repositories.end(); ++p2) {
		if (!p2->received) {
			m_allBundlesReceived = false;
			break;
		}
	}

	if (m_allBundlesReceived) {
		UpdateBundleList();
		m_bundleList->SetCursor(wxCURSOR_ARROW); // reset busy
	}
}

void BundleManager::OnItemSelected(wxListEvent& event) {
	// Get repository
	const long itemId = event.GetIndex();
	SelectItem(itemId);
}

void BundleManager::SelectItem(long itemId, bool update) {
	m_currentSel = itemId;
	wxListItem li;
	li.SetId(itemId);
	li.SetColumn(2);
	li.SetMask(wxLIST_MASK_TEXT);
	m_bundleList->GetItem(li);
	m_currentRepo = li.GetText();

	// Get the file info
	m_currentBundle = (cxFileInfo*)m_bundleList->GetItemData(itemId);
	const wxString& name = m_currentBundle->m_name;

	// Check if bundle is installed
	m_currentBundleState = BDL_NOT_INSTALLED;
	m_currentBundleInfo = NULL;
	for (vector<PListHandler::cxBundleInfo>::iterator b = m_installedBundles.begin(); b != m_installedBundles.end(); ++b) {
		if (b->dirName == name) {
			if (b->isDisabled) m_currentBundleState = BDL_DISABLED;
			else if (b->modDate == m_currentBundle->m_modDate) m_currentBundleState = BDL_INSTALLED_UPTODATE;
			else if (b->modDate < m_currentBundle->m_modDate) m_currentBundleState = BDL_INSTALLED_OLDER;
			else m_currentBundleState = BDL_INSTALLED_NEWER;

			m_currentBundleInfo = &*b;
			break;
		}
	}

	Freeze();

	// Update status and buttons
	m_installButton->Hide();
	m_deleteButton->Hide();
	switch (m_currentBundleState) {
	case BDL_NOT_INSTALLED:
		m_statusText->SetLabel(_("Not Installed"));
		m_installButton->SetLabel(_("&Install"));
		m_installButton->Show();
		break;
	case BDL_DISABLED:
		m_statusText->SetLabel(_("Not Installed"));
		m_installButton->SetLabel(_("&Restore"));
		m_installButton->Show();
		break;
	case BDL_INSTALLED_UPTODATE:
		m_statusText->SetLabel(_("Installed and up-to-date"));
		m_deleteButton->Show();
		break;
	case BDL_INSTALLED_OLDER:
		m_statusText->SetLabel(_("Installed version is older than in repository"));
		m_installButton->SetLabel(_("&Update"));
		m_installButton->Show();
		m_deleteButton->Show();
		break;
	case BDL_INSTALLED_NEWER:
		m_statusText->SetLabel(_("Installed version is newer than in repository"));
		m_installButton->SetLabel(_("&Update"));
		m_installButton->Show();
		m_deleteButton->Show();
		break;
	}
	m_mainSizer->Layout();

	Thaw();

	if (!update) {
		// Clear description
#ifdef __WXMSW__
		m_browser->LoadString(wxT(""));
#endif
		
		// See if we can download the info.plist
		RemoteProfile rp;
		const wxString& repoUrl = GetCurrentRepoUrl();
		const wxString url = repoUrl + name + wxT("/info.plist");
		if (!m_tempFile.empty() && wxFileExists(m_tempFile)) wxRemoveFile(m_tempFile);
		m_tempFile = wxFileName::CreateTempFileName(wxT("html"));
		m_remoteThread.DownloadAsync(url, m_tempFile, rp, *this);
	}
}

void BundleManager::OnRemoteAction(cxRemoteAction& event) {
	if (!event.Succeded()) return;
	if (!event.GetTarget() == m_tempFile) return;

	// Get the description
	wxFFile tempfile(m_tempFile, wxT("rb"));
	TiXmlDocument doc;
	if (!tempfile.IsOpened() || !doc.LoadFile(tempfile.fp())) return;
	TiXmlHandle docHandle( &doc );
	const TiXmlElement* child = docHandle.FirstChildElement("plist").FirstChildElement("dict").FirstChildElement().Element();
	while (child) {
		if (strcmp(child->Value(), "key") != 0) return; // invalid dict
		const TiXmlElement* const value = child->NextSiblingElement();
		if (!value) return; // invalid dict

		if (strcmp(child->GetText(), "description") == 0) {
			const char* desc = value->GetText(); // Get value
			if (desc) {
#ifdef __WXMSW__
				const wxString descStr(desc, wxConvUTF8);
				m_browser->LoadString(descStr);
#endif
			}

			return;
		}
		child = value->NextSiblingElement(); // advance past value
	}
}

void BundleManager::OnBeforeLoad(IHtmlWndBeforeLoadEvent& event) {
    const wxString url = event.GetURL();
	if (url == wxT("about:blank")) return;

	wxLaunchDefaultBrowser(url);

	// Don't try to open it in inline browser
	event.Cancel(true);
}

void BundleManager::OnInstallButton(wxCommandEvent& WXUNUSED(event)) {
	switch (m_currentBundleState) {
	case BDL_NOT_INSTALLED:
	case BDL_INSTALLED_OLDER:
	case BDL_INSTALLED_NEWER:
		InstallBundle();
		break;
	case BDL_DISABLED:
		RestoreBundle();
		break;
	case BDL_INSTALLED_UPTODATE:
		wxASSERT(false); // should never happen
		break;
	}
}

void BundleManager::OnDeleteButton(wxCommandEvent& WXUNUSED(event)) {
	wxASSERT(m_currentBundle);
	wxASSERT(m_currentBundleInfo);
	if (m_installedBundles.empty()) return;

	// convert pointer to iterator
	vector<PListHandler::cxBundleInfo>::iterator p = m_installedBundles.begin() + (m_currentBundleInfo - &*m_installedBundles.begin());

	if (m_currentBundleInfo->id == -1) {
		// We have just installed it, so just delete dir
		const wxString installPath = ((eApp*)wxTheApp)->GetAppDataPath() + wxT("InstalledBundles") + wxFILE_SEP_PATH + m_currentBundle->m_name;
		DelTree(installPath);

		m_installedBundles.erase(p);
	}
	else {
		if (m_plistHandler.DeleteBundle(m_currentBundleInfo->id)) m_installedBundles.erase(p);
		else m_currentBundleInfo->isDisabled = true;
	}

	// Update UI
	m_bundleList->SetItemImage(m_currentSel, 0); // empty image
	SelectItem(m_currentSel, true);

	m_needBundleReload = true;
}

void BundleManager::RestoreBundle() {
	wxASSERT(m_currentBundle);
	wxASSERT(m_currentBundleInfo);
	wxASSERT(m_currentBundleInfo->id != -1);

	if (!m_plistHandler.RestoreBundle(m_currentBundleInfo->id)) return;

	// Update bundle info
	m_currentBundleInfo->modDate = m_plistHandler.GetBundleModDate(m_currentBundleInfo->id);
	m_currentBundleInfo->isDisabled = false;

	// Update UI
	if (m_currentBundleInfo->modDate == m_currentBundle->m_modDate) m_bundleList->SetItemImage(m_currentSel, 1); // up-to-date image
	else m_bundleList->SetItemImage(m_currentSel, 1); // changed image
	SelectItem(m_currentSel, true);

	m_needBundleReload = true;
}

bool BundleManager::InstallBundle() {
	wxASSERT(m_currentBundle);

	// Create temp dir
	wxString tempDir = wxStandardPaths::Get().GetTempDir();
	tempDir += wxFILE_SEP_PATH + m_currentBundle->m_name;
	if (wxDirExists(tempDir)) {
		// Delete old first
		DelTree(tempDir);
	}
	wxMkdir(tempDir);

	// Show progress dialog
	wxProgressDialog dlg(_("Downloading..."), m_currentBundle->m_name, 200, this, wxPD_AUTO_HIDE|wxPD_APP_MODAL);

	// Download bundle
	const wxString& repoUrl = GetCurrentRepoUrl();
	const wxString url = repoUrl + m_currentBundle->m_name + wxT('/');
	wxFileName path(tempDir, wxEmptyString);
	if (!DownloadDir(url, path, dlg)) {
		DelTree(tempDir); // clean up
		return false;
	}

	// Set modDate to match repo
	// Windows does not support changing dates on directories under FAT so
	// to make sure it works we set the date on info.plist instead
	path.SetFullName(wxT("info.plist"));
	path.SetTimes(NULL, &m_currentBundle->m_modDate, NULL);

	// Delete installed version (if any)
	wxString installPath = ((eApp*)wxTheApp)->GetAppDataPath() + wxT("InstalledBundles") + wxFILE_SEP_PATH;
	if (!wxDirExists(installPath)) wxMkdir(installPath);
	installPath += m_currentBundle->m_name;
	if (wxDirExists(installPath)) DelTree(installPath);

	// Move bundle to install dir
	wxRenameFile(tempDir, installPath);

	// Update list
	bool inList = false;
	for (vector<PListHandler::cxBundleInfo>::iterator p = m_installedBundles.begin(); p != m_installedBundles.end(); ++p) {
		if (p->dirName == m_currentBundle->m_name) {
			p->modDate = m_currentBundle->m_modDate;
			inList = true;
			break;
		}
	}
	if (!inList) {
		PListHandler::cxBundleInfo bi(-1, m_currentBundle->m_name, m_currentBundle->m_modDate);
		m_installedBundles.push_back(bi);
	}

	// Update the UI
	m_bundleList->SetItemImage(m_currentSel, 1); // up-to-date image
	SelectItem(m_currentSel, true);

	m_needBundleReload = true;
	return true;
}

bool BundleManager::DownloadDir(const wxString& url, const wxFileName& path, wxProgressDialog& dlg) {
	// Get list of files and folders
	RemoteProfile rp;
	vector<cxFileInfo> fiList;
	CURLcode errorCode = m_remoteThread.GetRemoteListWait(url, rp, fiList);
	if (errorCode != CURLE_OK) goto error;

	// Download files and folders
	{
		unsigned int retryCount = 0;
		for (vector<cxFileInfo>::const_iterator p = fiList.begin(); p != fiList.end(); ++p) {
			if (p->m_isDir) {
				// Create subdir
				wxFileName dir = path;
				dir.AppendDir(p->m_name);
				wxMkdir(dir.GetPath());

				const wxString dirName = p->m_name + wxT('/');
				if (!dlg.Pulse(dirName)) return false;

				// Download contents
				const wxString dirUrl = url + dirName;
				if (!DownloadDir(dirUrl, dir, dlg)) return false;
			}
			else {
				wxString filename = p->m_name;
#ifdef __WXMSW__
				filename = UrlEncode::EncodeFilename(filename); // Make filename safe for win
#endif
				wxFileName filePath = path;
				filePath.SetFullName(filename);
				const wxString pathStr = filePath.GetFullPath();

				if (!dlg.Pulse(filename)) return false;

				// Download file
				const wxString fileUrl = url + p->m_name;
				errorCode = m_remoteThread.Download(fileUrl, pathStr, rp);
				
				// Check for errors
				if (errorCode != CURLE_OK) goto error;

				// There has been some cases where the download has ended
				// up with an empty file but no error. As a workaround
				// we give it one more try and then return an error.
				if (filePath.GetSize() == 0 && p->m_size != 0) {
					if (retryCount == 0) {
						wxLogDebug(wxT("Received empty file! Retrying download"));
						++retryCount;
						--p;
						continue;
					}
					else {
						errorCode = CURLE_PARTIAL_FILE;
						goto error;
					}
				}

				// Set modDate
				filePath.SetTimes(NULL, &p->m_modDate, NULL);

				retryCount = 0;
			}
		}
	}

	return true;

error:
	wxMessageBox(m_remoteThread.GetErrorText(errorCode), _("Download Error"), wxICON_ERROR|wxOK, this);
	return false;
}

#ifdef __WXMSW__
static void ConvertWxToFileTime(FILETIME *ft, const wxDateTime& dt)
{
    SYSTEMTIME st;
    st.wDay = dt.GetDay();
    st.wMonth = (WORD)(dt.GetMonth() + 1);
    st.wYear = (WORD)dt.GetYear();
    st.wHour = dt.GetHour();
    st.wMinute = dt.GetMinute();
    st.wSecond = dt.GetSecond();
    st.wMilliseconds = dt.GetMillisecond();

    FILETIME ftLocal;
    if ( !::SystemTimeToFileTime(&st, &ftLocal) )
    {
        wxLogLastError(_T("SystemTimeToFileTime"));
    }

    if ( !::LocalFileTimeToFileTime(&ftLocal, ft) )
    {
        wxLogLastError(_T("LocalFileTimeToFileTime"));
    }
}
#endif

void BundleManager::SetDirModDate(wxFileName& path, const wxDateTime& modDate) {
#ifdef __WXMSW__
	wxLogDebug(wxT("SetDirModDate: %s %s"), modDate.FormatDate(), modDate.FormatTime());
	HANDLE hDir = ::CreateFile (
		path.GetPath(),
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	FILETIME ftWrite;
	ConvertWxToFileTime(&ftWrite, modDate);

	::SetFileTime(hDir, &ftWrite, &ftWrite, &ftWrite);

	::CloseHandle(hDir);
#else
	path.SetTimes(NULL, &modDate, NULL);
#endif
}

void BundleManager::DelTree(const wxString& path) {
#ifdef __WXMSW__
	// Paths have to be double-null terminated
	wxString filePaths = path + wxT('\0');
	filePaths += wxT('\0');

	// The File Operation Structure
	SHFILEOPSTRUCT SHFileOp;
	ZeroMemory(&SHFileOp, sizeof(SHFILEOPSTRUCT));
	SHFileOp.hwnd = NULL;
	SHFileOp.pFrom = filePaths.c_str();
	SHFileOp.wFunc = FO_DELETE;
	SHFileOp.fFlags = FOF_NOCONFIRMMKDIR|FOF_SILENT|FOF_NOCONFIRMATION|FOF_NOERRORUI;

	// Do the file operation
	SHFileOperation(&SHFileOp);

#else
	wxASSERT(false); // not implemented
#endif
}
