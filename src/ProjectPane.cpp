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

#include "ProjectPane.h"

#include <wx/mimetype.h>
#include <wx/progdlg.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/tokenzr.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>

#include "IFrameProjectService.h"
#include "ProjectSettings.h"
#include "FileActionThread.h"
#include "DirWatcher.h"
#include "RemoteThread.h"
#include "eDocumentPath.h"

#include "images/NewFolder.xpm"
#include "images/NewDocument.xpm"

#ifdef __WXMSW__
    #include "ShellContextMenu.h"

    #pragma warning(push, 1)
#endif
#include "tinyxml.h" // tinyxml includes unused vars so it can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

// ctrl ids
enum {
	ID_PRJTREE,
	ID_NEWDIR,
	ID_NEWDOC,
	ID_PRJ_SETTINGS,
	MENU_PROJECT_ITEM_RENAME,
	MENU_PROJECT_REFRESH,
	MENU_OPEN_TREEITEMS
};

BEGIN_EVENT_TABLE(ProjectPane, wxPanel)
  EVT_TREE_ITEM_EXPANDING(ID_PRJTREE, ProjectPane::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSING(ID_PRJTREE, ProjectPane::OnCollapseItem)
  EVT_TREE_BEGIN_LABEL_EDIT(ID_PRJTREE, ProjectPane::OnBeginEditItem)
  EVT_TREE_END_LABEL_EDIT(ID_PRJTREE, ProjectPane::OnEndEditItem)
  EVT_TREE_ITEM_ACTIVATED(ID_PRJTREE, ProjectPane::OnItemActivated)
  EVT_TREE_KEY_DOWN(ID_PRJTREE, ProjectPane::OnTreeKeyDown)
  EVT_TREE_ITEM_MENU(ID_PRJTREE, ProjectPane::OnTreeContextMenu)
  EVT_TREE_BEGIN_DRAG(ID_PRJTREE, ProjectPane::OnTreeBeginDrag)
  EVT_REMOTELIST_RECEIVED(ProjectPane::OnRemoteListReceived)
  EVT_REMOTEACTION(ProjectPane::OnRemoteAction)
  EVT_BUTTON(ID_NEWDIR, ProjectPane::OnNewDir)
  EVT_BUTTON(ID_NEWDOC, ProjectPane::OnNewDoc)
  EVT_BUTTON(ID_PRJ_SETTINGS, ProjectPane::OnButtonSettings)
  EVT_MENU(MENU_PROJECT_ITEM_RENAME, ProjectPane::OnItemRename)
  EVT_MENU(MENU_PROJECT_REFRESH, ProjectPane::OnRefresh)
  EVT_MENU(MENU_OPEN_TREEITEMS, ProjectPane::OnMenuOpenTreeItems)
  EVT_DIRWATCHER(ProjectPane::OnDirChanged)
  EVT_IDLE(ProjectPane::OnIdle)
END_EVENT_TABLE()

bool projectpane_is_dir_empty(const wxString& path) {
	if (path.empty()) return true;

    wxDir dir;
	if (!dir.Open(path)) return true;
    if (!dir.HasSubDirs() && !dir.HasFiles()) return true;
	return false;
}

ProjectPane::ProjectPane(IFrameProjectService& projectServce, wxWindow*parent, wxWindowID id):
	wxPanel(parent, id), 
	m_projectService(projectServce), 
	m_imageList(16,16), m_dirWatchHandle(NULL),
	m_isRemote(false), m_isDestroying(false),  m_remoteThread(m_projectService.GetRemoteThread()),
	m_remoteProfile(NULL), m_busyCount(0), m_newIconsCond(m_iconMutex)
{
#ifdef __WXMSW__
    m_contextMenu = NULL;
#endif

	// Set drop target so files can be dragged from explorer (and self)
	DropTarget *dropTarget = new DropTarget(*this);
	SetDropTarget(dropTarget);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

	// Add tree control
	m_prjTree = new wxTreeCtrl(this, ID_PRJTREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_MULTIPLE|wxTR_EDIT_LABELS);
	m_prjTree->SetImageList(&m_imageList);
	mainSizer->Add(m_prjTree, 1, wxEXPAND);

	const wxBitmap newDirBitmap(newfolder_xpm);
	const wxBitmap newDocBitmap(newdocument_xpm);
	m_newDirButton = new wxBitmapButton(this, ID_NEWDIR, newDirBitmap);
	m_newDocButton = new wxBitmapButton(this, ID_NEWDOC, newDocBitmap);
	m_settingsButton = new wxButton(this, ID_PRJ_SETTINGS, _("Settings"));

	// Set tooltips
	m_newDirButton->SetToolTip(_("Create new folder"));
	m_newDocButton->SetToolTip(_("Create new file"));
	m_settingsButton->SetToolTip(_("Edit project settings"));

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->Add(m_newDirButton, 0, wxALIGN_LEFT|wxALL, 2);
		buttonSizer->Add(m_newDocButton, 0, wxALIGN_LEFT|wxALL, 2);
		buttonSizer->AddStretchSpacer(1);
		buttonSizer->Add(m_settingsButton, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 2);
	mainSizer->Add(buttonSizer, 0, wxEXPAND);

	SetSizer(mainSizer);

	// Set up accelerator table
	const unsigned int accelcount = 2;
	wxAcceleratorEntry entries[accelcount];
	entries[0].Set(wxACCEL_NORMAL, WXK_F2, MENU_PROJECT_ITEM_RENAME);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5, MENU_PROJECT_REFRESH);
	wxAcceleratorTable accel(accelcount, entries);
	SetAcceleratorTable(accel);

	//const wxFileName path(wxT("C:\\Temp\\"));
	//SetProject(path);
#ifdef __WXGTK__
	isDirWatched = false;
#endif
	// Start icon retrieval thread
	wxThreadHelper::Create();
	GetThread()->Run();
}

ProjectPane::~ProjectPane() {
	Clear();

	m_isDestroying = true;
	// Wake up thread
	m_iconMutex.Lock();
	m_newIconsCond.Signal();
	m_iconMutex.Unlock();
	GetThread()->Wait();
}


bool ProjectPane::IsFocused() const {
	return (FindFocus() == m_prjTree);
}

bool ProjectPane::SetProject(const wxFileName& path) {
	if (!path.IsDir() || !path.DirExists()) return false;

	// Clear remote info
	if (m_isRemote) {
		m_isRemote = false;
		m_remoteProfile = NULL;
		m_prjUrl.clear();
		ResetBusy(); // may still be waiting for remote event

		m_newDirButton->Enable();
		m_newDocButton->Enable();
		m_settingsButton->Enable();
	}

	if (!m_prjPath.IsOk() || m_prjPath != path) {
		m_prjPath = path;
		Init();
	}

	return true;
}

bool ProjectPane::SetRemoteProject(const RemoteProfile* rp) {
	wxASSERT(rp && rp->IsValid());

	m_remoteProfile = rp;
	m_prjUrl = rp->GetUrl();
	m_isRemote = true;
	m_prjPath.Clear();

	m_newDirButton->Enable(true);
	m_newDocButton->Enable(true);
	m_settingsButton->Enable(false);

	Init();
	return true;
}

void ProjectPane::CloseProject(){
	m_prjPath = wxFileName();
	Clear();
}

void ProjectPane::Clear() {
	m_prjTree->DeleteAllItems();
	m_imageList.RemoveAll();
	m_freeImages.clear();
	m_newFolder.clear();
	if (m_dirWatchHandle) {
		m_projectService.GetDirWatcher().UnwatchDirectory(m_dirWatchHandle);
		m_dirWatchHandle = NULL;
	}
	ResetBusy();
}

void ProjectPane::Init() {
	Clear();
	AddFolderIcon(m_imageList); // 0 is always folder

	// Get the name of the project dir
	wxString projectName;
	if (m_isRemote) {
		projectName = m_remoteProfile->m_name;
	}
	else {
		const wxArrayString& path = m_prjPath.GetDirs();
		if (path.empty() && m_prjPath.HasVolume()) {
			// Get label if it is a drive
			const wxString vol = m_prjPath.GetFullPath();
			wxString label;
#ifdef __WXMSW__
			TCHAR volname[MAX_PATH+1];
			GetVolumeInformation(vol.c_str(), volname, MAX_PATH, NULL, NULL, NULL, NULL, 0);
			projectName = volname;
#endif //__WXMSW__
			projectName += wxT(" (") + m_prjPath.GetVolume() + wxFileName::GetVolumeSeparator() + wxT(")");
		}
		else {
			projectName = path.Last();
		}
	}

	const wxString strpath = m_isRemote ? m_prjUrl : m_prjPath.GetPath();

	// The dir may have been deleted/renamed again
	// so only add it if we can get icon
	const int image_id = AddFileIcon(strpath, true/*isDir*/);
	if (image_id == wxNOT_FOUND) return;

	// Add the root
	DirItemData* const rootData = new DirItemData(strpath, projectName, true, image_id, m_freeImages);
	const wxTreeItemId rootId = m_prjTree->AddRoot(projectName, image_id, -1, rootData);

	// Adding the root can fail (cause unknown)
	if (!rootId) {
		m_prjPath = wxFileName(); // invalidate
		return;
	}

	// Load project info (if available)
	if (!m_isRemote) m_infoHandler.SetRoot(m_prjPath);

	// Always start with root expanded
	Freeze();
	ExpandDir(rootId); // probably not needed when next is handled
	m_prjTree->Expand(rootId);
	Thaw();

	// Watch for changes to the dir
	if (!m_isRemote) {
		wxASSERT(m_dirWatchHandle == NULL);
#ifdef __WXMSW__
		m_dirWatchHandle = m_projectService.GetDirWatcher().WatchDirectory(m_prjPath.GetPath(), *this, true);
#else
		if (false == isDirWatched) {
			m_dirWatchHandle = m_projectService.GetDirWatcher().WatchDirectory(m_prjPath.GetPath(),
				*this, true);
			WatchTree(m_prjPath.GetPath());
			isDirWatched = true;
		}
#endif
	}
}

wxArrayString ProjectPane::GetSelections() const {
	wxArrayTreeItemIds items;
	if (!m_prjTree->GetSelections(items)) return wxArrayString();

	wxArrayString selections;
	for (unsigned int i = 0; i < items.GetCount(); ++i) {
		const DirItemData *data = (DirItemData*)m_prjTree->GetItemData( items[i] );
		selections.Add(data->m_path);
	}

	return selections;
}

void ProjectPane::OnRemoteListReceived(cxRemoteListEvent& event) {
	if (event.GetUrl() == m_waitingForDir) m_waitingForDir.clear(); // end wait
	SetBusy(false); // set in ExpandDir()
	if (!m_isRemote) return;

	// If project has changed, ignore event
	const wxString& url = event.GetUrl();
	if (!url.StartsWith(m_prjUrl)) return;

	// Handle errors
	if (!event.Succeded() && event.GetErrorCode() != CURLE_LOGIN_DENIED) {
		wxString msg = _T("Could not open remote folder: ") + event.GetUrl();
		msg += wxT("\n") + event.GetError();
		wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
		return;
	}

	// Check if the url is a visible dir
	const wxTreeItemId item = GetItemFromUrl(url);
	if (!item.IsOk()) return; // dir not in tree

	// No need to expand dir if it is already expanded
	if (m_prjTree->IsExpanded(item)) return;

	// Handle failed login (need item)
	if (event.GetErrorCode() == CURLE_LOGIN_DENIED) {
		if (m_projectService.AskRemoteLogin(m_remoteProfile)) {
			ExpandDir(item); // Try expanding again
			return;
		}
	}

	DirItemData *data = (DirItemData *) m_prjTree->GetItemData(item);
	wxASSERT(data && data->m_isDir);

	// Split in dirs and files
	std::vector<cxFileInfo>& fiList = event.GetFileList();
	wxArrayString dirs;
	wxArrayString files;
	for (std::vector<cxFileInfo>::const_iterator p = fiList.begin(); p != fiList.end(); ++p) {
		if (p->m_isDir) dirs.Add(p->m_name);
		else files.Add(p->m_name);
	}

	ExpandDir(item, data, dirs, files);
	m_prjTree->Expand(item);
}

void ProjectPane::SetBusy(bool busy) {
	if (busy) {
		if (m_busyCount == 0) SetCursor(wxCURSOR_WAIT);
		++m_busyCount;
		//wxBeginBusyCursor();
	}
	else {
		if (m_busyCount == 0) return; // not busy
		else if (m_busyCount == 1) SetCursor(wxCURSOR_ARROW);
		--m_busyCount;
	}

	/*if (wxIsBusy()) {
		wxEndBusyCursor();
	}*/
}

void ProjectPane::ResetBusy() {
	if (m_busyCount) SetCursor(wxCURSOR_ARROW);
	m_busyCount = 0;
}

void ProjectPane::OnRemoteAction(cxRemoteAction& event) {
	SetBusy(false); // set in ExpandDir()
	if (!m_isRemote) return;

	// If project has changed, ignore event
	const wxString& url = event.GetUrl();
	if (!url.StartsWith(m_prjUrl)) return;

	// Handle errors
	if (!event.Succeded()) {
		wxString msg = _T("Could not ");
		switch (event.GetActionType()) {
		case cxRA_DELETE:
			msg += _("delete: ");
			break;
		case cxRA_CREATEFILE:
		case cxRA_MKDIR:
			msg += _("create: ");
			break;
		case cxRA_RENAME:
			msg += _("rename: ");
			break;
		case cxRA_UPLOAD:
			msg += _("upload to: ");
			break;
        default:
            return; // not an event to be handled
		}
		msg += event.GetUrl();
		msg += wxT("\n") + event.GetError();
		wxMessageBox(msg, _T("e Error"), wxICON_ERROR);
		return;
	}

	switch (event.GetActionType()) {
	case cxRA_DELETE:
		{
			// Get the deleted item
			const wxTreeItemId item = GetItemFromUrl(url);
			if (!item.IsOk()) return; // not in tree

			m_prjTree->Delete(item);
		}
		break;

	case cxRA_CREATEFILE:
	case cxRA_UPLOAD:
		{
			wxString path = url;
			bool isDir = false;
			if (path.Last() == wxT('/')) {
				isDir = true;
				path.RemoveLast();
			}

			// Get filename and parent folder
			const wxString name = path.AfterLast(wxT('/'));
			path = path.BeforeLast(wxT('/'));
			const wxTreeItemId item = GetItemFromUrl(path);
			if (!item.IsOk()) return; // not in tree

			// Make sure parent folder is expanded
			if (m_prjTree->IsExpanded(item)) {
				// Add the new file to tree
				wxTreeItemId id;
				if (isDir) {
					DirItemData *dir_item = new DirItemData(url, name, true, 0, m_freeImages);
					id = m_prjTree->AppendItem(item, name, 0, -1, dir_item);
					m_prjTree->SetItemHasChildren(id); // make it expandable
				}
				else {
					const int image_id = AddFileIcon(name, false);
					if (image_id == wxNOT_FOUND) return;
					DirItemData *file_item = new DirItemData(url, name, false, image_id, m_freeImages);
					id = m_prjTree->AppendItem(item, name, image_id, -1, file_item);
				}
				m_prjTree->UnselectAll();
				m_prjTree->SelectItem(id);
			}
			else {
				ExpandDirWait(item);
				const wxTreeItemId file_item = GetItemFromUrl(url);
				if (!file_item.IsOk()) return; // not in tree
				m_prjTree->UnselectAll();
				m_prjTree->SelectItem(file_item);
			}

			// Open in editor
			if (event.GetActionType() == cxRA_CREATEFILE) {
				m_projectService.OpenRemoteFile(url, m_remoteProfile);
			}
		}
		break;

	case cxRA_MKDIR:
		{
			// Get parent folder
			const wxString path = url.BeforeLast(wxT('/'));
			const wxTreeItemId item = GetItemFromUrl(path);
			if (!item.IsOk()) return; // not in tree

			// Make sure parent folder is expanded
			if (m_prjTree->IsExpanded(item)) {
				// Add the new dir to tree
				const wxString fileName = url.AfterLast(wxT('/'));
				const wxString fullpath = url + wxT('/'); // url in mkdir event is without trailing slash
				DirItemData *dir_item = new DirItemData(fullpath, fileName, true, 0, m_freeImages);
				wxTreeItemId id = m_prjTree->AppendItem(item, fileName, 0, -1, dir_item);
				m_prjTree->SetItemHasChildren(id); // make it expandable
				m_prjTree->UnselectAll();
				m_prjTree->SelectItem(id);
			}
			else {
				ExpandDirWait(item);
				const wxTreeItemId dir = GetItemFromUrl(url);
				if (!dir.IsOk()) return; // not in tree
				m_prjTree->UnselectAll();
				m_prjTree->SelectItem(dir);
			}
		}
		break;

	case cxRA_RENAME:
		{
			// Get the renamed item
			const wxTreeItemId item = GetItemFromUrl(url);
			if (!item.IsOk()) return; // not in tree
			DirItemData *data = (DirItemData *) m_prjTree->GetItemData(item);

			// Create new path
			wxString path = url;
			if (path.Last() == wxT('/')) path.RemoveLast();
			path = path.BeforeLast(wxT('/'));
			const wxString& newname = event.GetTarget();
			path += wxT('/') + newname;
			if (data->m_isDir) path += wxT('/');

			// Update data
			data->m_name = newname;
			data->m_path = path;
			if (data->m_isDir) RefreshSubItemPaths(item);
			else data->SetImage(AddFileIcon(newname, false));

			// Update tree item
			m_prjTree->SetItemText(item, newname);
			m_prjTree->SetItemImage(item, data->m_imageId);
		}
		break;

    default:
        return; // not an event to be handled
	}
}

wxTreeItemId ProjectPane::GetItemFromPath(const wxString& path) const {
	// Make path relative to project
	wxString relativePath;
	const wxString prjPath = m_prjPath.GetPath();
	if (!path.StartsWith(prjPath, &relativePath)) return wxTreeItemId();
	if (relativePath.empty()) return m_prjTree->GetRootItem();
	
	//const wxFileName changedFile(relativePath);
	//if (!changedFile.IsOk()) return wxTreeItemId();

	const wxArrayString dirs = wxStringTokenize(relativePath, wxFileName::GetPathSeparators(), wxTOKEN_STRTOK);

	return GetItemFromNames(dirs);
}


wxTreeItemId ProjectPane::GetItemFromUrl(const wxString& url) const {
	wxASSERT(url.StartsWith(m_prjUrl));

	// Get subdirs
	const wxString subDirs = url.substr(m_prjUrl.size());
	const wxArrayString dirs = wxStringTokenize(subDirs, wxT("/"));

	return GetItemFromNames(dirs);
}

wxTreeItemId ProjectPane::GetItemFromNames(const wxArrayString& dirs) const {
	// Check if the url is a visible dir
	wxTreeItemId item = m_prjTree->GetRootItem();
	for (unsigned int i = 0; i < dirs.GetCount(); ++i) {
		wxTreeItemIdValue cookie;
		wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
		const wxString& dir = dirs[i];

		while(subItem.IsOk()) {
			const wxString& itemText = m_prjTree->GetItemText(subItem);
			if (itemText == dir) {
				item = subItem;
				break;
			}
			subItem = m_prjTree->GetNextChild(item, cookie);
		}

		if (!subItem.IsOk()) return wxTreeItemId(); // dir not in tree
	}

	return item;
}

void ProjectPane::ExpandDirWait(wxTreeItemId parentId) {
	wxASSERT(parentId);
	wxASSERT(m_isRemote);

	DirItemData *data = (DirItemData *) m_prjTree->GetItemData(parentId);
	wxASSERT(data && data->m_isDir);

    if (data->m_isExpanded) return;

	SetBusy();
	m_remoteThread.GetRemoteList(data->m_path, *m_remoteProfile, *this);

	// Wait for event to return
	m_waitingForDir = data->m_path;
	while (!m_waitingForDir.empty()) {
        wxMilliSleep(50); // don't eat 100% of the CPU

#ifdef __WXMSW__
		// WORKAROUND: Yield resets busy cursor
		// so we have to buffer it and reset it after yielding
		HCURSOR currentCursor = ::GetCursor();
#endif //__WXMSW__

		wxSafeYield(); // we must process messages or we'd never get the cxRemoteAction event

#ifdef __WXMSW__
		::SetCursor(currentCursor);
		//wxSetCursor(*wxHOURGLASS_CURSOR); // WORKAROUND: Yield resets busy cursor
#endif //__WXMSW__
	}
}

void ProjectPane::ExpandDir(wxTreeItemId parentId) {
	wxASSERT(parentId);

	DirItemData *data = (DirItemData *) m_prjTree->GetItemData(parentId);
	wxASSERT(data && data->m_isDir);

    if (data->m_isExpanded)
        return;

	wxLogDebug(wxT("Expanding: %s"), data->m_path.c_str());

	if (m_isRemote) {
		SetBusy();
		m_remoteThread.GetRemoteList(data->m_path, *m_remoteProfile, *this);
	}
	else {
		wxString dirName(data->m_path);
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
		if (dirName.Last() == ':') dirName += wxFILE_SEP_PATH;
#endif

		wxArrayString dirs;
		wxArrayString filenames;
		m_infoHandler.GetDirAndFileLists(dirName, dirs, filenames);

		ExpandDir(parentId, data, dirs, filenames);
	}
}

void ProjectPane::ExpandDir(wxTreeItemId parentId, DirItemData *data, const wxArrayString& dirs, const wxArrayString& filenames) {
	wxASSERT(data && data->m_isDir);

	wxString dirName(data->m_path);
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
    if (dirName.Last() == wxT(':')) dirName += wxFILE_SEP_PATH;
#endif

	// Prepare path for adding file/dirname
	if (!m_isRemote) {
		if(!wxEndsWithPathSeparator(dirName)) dirName += wxFILE_SEP_PATH;
	}
	else if (dirName.Last() != wxT('/')) dirName += wxT('/');


	// Add the sorted dirs
	wxString eachFilename;
	wxString path;
    size_t i;
    for (i = 0; i < dirs.Count(); i++)
    {
        eachFilename = dirs[i];
        path = dirName;
        path += eachFilename;
		if (m_isRemote) path += wxT('/'); // otherwise curl wont know it's a folder

		// The dir may have been deleted/renamed again
		// so only add it if we can get icon
		const int image_id = AddFileIcon(path, true); // zero is always dir icon
		if (image_id == wxNOT_FOUND) continue;

        DirItemData *dir_item = new DirItemData(path,eachFilename,true,image_id, m_freeImages);
        wxTreeItemId id = m_prjTree->AppendItem( parentId, eachFilename,
                                      image_id, -1, dir_item);
        //m_treeCtrl->SetItemImage( id, wxFileIconsTable::folder_open,
        //                          wxTreeItemIcon_Expanded );

        // Get filters for subdir
		//wxArrayString includeDirs;
		//wxArrayString excludeDirs;
		//wxArrayString includeFiles;
		//wxArrayString excludeFiles;
		//(path, includeDirs, excludeDirs, includeFiles, excludeFiles);

		// Has this got any children? If so, make it expandable.
        // (There are two situations when a dir has children: either it
        // has subdirectories or it contains files that weren't filtered
        // out. The latter only applies to dirctrl with files.)
        if (m_isRemote || !projectpane_is_dir_empty(path)) {
            m_prjTree->SetItemHasChildren(id);
        }
    }

    // Add the sorted filenames
    for (i = 0; i < filenames.Count(); i++)
    {
        eachFilename = filenames[i];
        path = dirName;
        path += eachFilename;

		// The file may have been deleted/renamed again
		// so only add it if we can get icon
		const int image_id = AddFileIcon(path, false);
		if (image_id == wxNOT_FOUND) continue;

        DirItemData *dir_item = new DirItemData(path,eachFilename,false,image_id,m_freeImages);
        m_prjTree->AppendItem( parentId, eachFilename, image_id, -1, dir_item);
    }

	// If the dir turns up empty after applying filters, remove expandability
	if (m_prjTree->GetChildrenCount(parentId, false) == 0) {
		data->m_isExpanded = false;
		m_prjTree->SetItemHasChildren(parentId, false);
	}
	else data->m_isExpanded = true;
}

void ProjectPane::RefreshDirs() {
	wxLogDebug(wxT("Refreshing project pane"));
	const wxTreeItemId rootId = m_prjTree->GetRootItem();
	if (!rootId) return; // empty

	// Build a list of all expanded dirs
	wxArrayString expandedDirs;
	const DirItemData *rootdata = (DirItemData *) m_prjTree->GetItemData(rootId);
	if (rootdata->m_isExpanded) {
		expandedDirs.Add(rootdata->m_path);
		GetExpandedDirs(rootId, expandedDirs);
	}

	// Build a list of all selections
	wxArrayString selections = GetSelections();

	// Get the last visible item
	const int scrollPos = m_prjTree->GetScrollPos(wxVERTICAL);

	Freeze();
	{
		// Collapse and reload
		m_prjTree->DeleteAllItems();
		Init();

		// Get new root
		const wxTreeItemId newRootId = m_prjTree->GetRootItem();
		if (newRootId) {
			DirItemData *rootdata = (DirItemData *) m_prjTree->GetItemData(newRootId);

			// Should root be selected
			const int ndx = selections.Index(rootdata->m_path);
			if (ndx != wxNOT_FOUND) {
				m_prjTree->SelectItem(newRootId);
				selections.RemoveAt(ndx);
			}

			// Expand and select subItems like previously
			ExpandAndSelect(newRootId, expandedDirs, selections);

			// Move to same position
			m_prjTree->SetScrollPos(wxVERTICAL, scrollPos);
		}
	}
	Thaw();
}

void ProjectPane::ExpandAndSelect(wxTreeItemId item, wxArrayString& expandedDirs, wxArrayString& selections) {
	wxASSERT(item);

	wxTreeItemIdValue cookie;
	wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
	while(subItem.IsOk()) {
		const DirItemData *subdata = (DirItemData *) m_prjTree->GetItemData(subItem);

		// Expand dirs
		if (subdata->m_isDir) {
			const int ndx = expandedDirs.Index(subdata->m_path);
			if (ndx != wxNOT_FOUND) {
				ExpandDir(subItem);
				m_prjTree->Expand(subItem);
				expandedDirs.RemoveAt(ndx);

				ExpandAndSelect(subItem, expandedDirs, selections);
			}
		}

		// Select items
		const int ndx = selections.Index(subdata->m_path);
		if (ndx != wxNOT_FOUND) {
			m_prjTree->SelectItem(subItem);
			selections.RemoveAt(ndx);
		}

		subItem = m_prjTree->GetNextChild(item, cookie);
	}
}


void ProjectPane::CollapseDir(wxTreeItemId parentId) {
    DirItemData *data = (DirItemData *) m_prjTree->GetItemData(parentId);

	wxLogDebug(wxT("Collapsing: %s"), data->m_path.c_str());

    if (!data->m_isExpanded)
        return;

    data->m_isExpanded = false;

	m_prjTree->Collapse(parentId);
	m_prjTree->DeleteChildren(parentId);
	//m_prjTree->CollapseAndReset(parentId);
}

bool ProjectPane::IsDirEmpty(const wxString& path) const {
	if (path.empty()) return true;

    wxDir dir;
	if (!dir.Open(path)) return true;
    if (!dir.HasSubDirs() && !dir.HasFiles()) return true;

	return false;

	/*// Get filters
	wxArrayString includeDirs;
	wxArrayString excludeDirs;
	wxArrayString includeFiles;
	wxArrayString excludeFiles;
	GetFilters(path, includeDirs, excludeDirs, includeFiles, excludeFiles);

	// Filter all subdirs
	wxString eachFilename;
 	int style = wxDIR_DIRS;
    if (dir.GetFirst(&eachFilename, wxEmptyString, style)) {
        do {
            if ((eachFilename != wxT(".")) && (eachFilename != wxT(".."))) {
                if (MatchFilter(eachFilename, includeDirs, excludeDirs)) {
					// There just have to be one matching dir
					return false;
				}
            }
        }
        while (dir.GetNext(&eachFilename));
    }

	// Filter all files in dir
	style = wxDIR_FILES;
	if (dir.GetFirst(&eachFilename, wxEmptyString, style)) {
        do {
            if ((eachFilename != wxT(".")) && (eachFilename != wxT(".."))) {
                if (MatchFilter(eachFilename, includeFiles, excludeFiles)) {
					// There just have to be one matching file
					return false;
				}
            }
        }
        while (dir.GetNext(&eachFilename));
    }

	// No matching dirs
	return true;*/
}

void ProjectPane::GetExpandedDirs(wxTreeItemId item, wxArrayString& dirs) {
	wxASSERT(item);

	wxTreeItemIdValue cookie;
	wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
	while(subItem.IsOk()) {
		DirItemData *subdata = (DirItemData *) m_prjTree->GetItemData(subItem);

		if (subdata->m_isDir && subdata->m_isExpanded) {
			dirs.Add(subdata->m_path);

			GetExpandedDirs(subItem, dirs);
		}

		subItem = m_prjTree->GetNextChild(item, cookie);
	}
}

void ProjectPane::RefreshSubItemPaths(const wxTreeItemId& item) {
	wxASSERT(item);

	DirItemData *data = (DirItemData *) m_prjTree->GetItemData(item);
	wxASSERT(data && data->m_isDir);

    if (!data->m_isExpanded) return;

	wxTreeItemIdValue cookie;
	wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
	while(subItem.IsOk()) {
		DirItemData *subdata = (DirItemData *) m_prjTree->GetItemData(subItem);

		if (m_isRemote) {
			subdata->m_path = data->m_path + subdata->m_name;
			if (subdata->m_isDir) subdata->m_path += wxT('/');
		}
		else subdata->m_path = data->m_path + wxString(wxFILE_SEP_PATH) + subdata->m_name;

		if (subdata->m_isDir && subdata->m_isExpanded) {
			RefreshSubItemPaths(subItem);
		}

		subItem = m_prjTree->GetNextChild(item, cookie);
	}
}

void ProjectPane::RefreshIcon(const wxTreeItemId& item) {
	wxASSERT(item);

	DirItemData *data = (DirItemData*)m_prjTree->GetItemData(item);
	wxASSERT(data);

	// Update the icon
	const int image_id = AddFileIcon(data->m_path, data->m_isDir);
	if (image_id != wxNOT_FOUND) {
		data->SetImage(image_id);
		m_prjTree->SetItemImage(item, image_id);
	}

	// Update parent icons as well
	if (item != m_prjTree->GetRootItem()) {
		RefreshIcon(m_prjTree->GetItemParent(item));
	}
}

wxTreeItemId ProjectPane::FindSubItem(const wxTreeItemId& item, const wxString& label) const {
	wxASSERT(item);

	wxTreeItemIdValue cookie;
	wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
	while (subItem.IsOk()) {
		if (m_prjTree->GetItemText(subItem) == label) return subItem;
		subItem = m_prjTree->GetNextChild(item, cookie);
	}

	return subItem;
}

void ProjectPane::OnExpandItem(wxTreeEvent &event)
{
    const wxTreeItemId parentId = event.GetItem();

    Freeze();
	ExpandDir(parentId);
	Thaw();
}

void ProjectPane::OnCollapseItem(wxTreeEvent &event )
{
    // Root item cannot be closed
	const wxTreeItemId itemId = event.GetItem();
	if (itemId == m_prjTree->GetRootItem()) {
		event.Veto();
		return;
	}

	CollapseDir(event.GetItem());
}

void ProjectPane::OnBeginEditItem(wxTreeEvent &event) {
	// We do not allow editing of volume labels
	wxTreeItemId item = event.GetItem();
	if (item == m_prjTree->GetRootItem()) {
		DirItemData *data = (DirItemData*)m_prjTree->GetItemData( item );
		const wxFileName fn = data->m_path;

		if (fn.GetDirs().empty() && fn.HasVolume()) {
			event.Veto();
		}
	}
}

static bool projectpane_illegal_filename(const wxString& filename) {
	return filename == _(".") || filename == _("..") ||
        (filename.Find(wxT('/')) != wxNOT_FOUND) ||
        (filename.Find(wxT('\\')) != wxNOT_FOUND) ||
        (filename.Find(wxT('|')) != wxNOT_FOUND);
}

void ProjectPane::OnEndEditItem(wxTreeEvent &event)
{
	// No change sometimes give empty label in event
	if (event.IsEditCancelled() || event.GetLabel().empty()) {
		event.Veto();
		return;
	}

	if (projectpane_illegal_filename(event.GetLabel())) {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

    wxTreeItemId id = event.GetItem();
    DirItemData *data = (DirItemData*)m_prjTree->GetItemData( id );
    wxASSERT( data );

    if (m_isRemote) {
		SetBusy();
		m_remoteThread.Rename(data->m_path, event.GetLabel(), *m_remoteProfile, *this);
		event.Veto(); // rename will happen when event returns
	}
	else {
		const wxString new_name = event.GetLabel();
		wxString new_path( wxPathOnly( data->m_path ) );
		new_path += wxString(wxFILE_SEP_PATH);
		new_path += new_name;

		if (new_name.CmpNoCase(data->m_name) != 0 && wxFileExists(new_path)) {
			wxMessageDialog dialog(this, _("There already exist a file with this name."), _("Error"), wxOK | wxICON_ERROR );
			dialog.ShowModal();
			event.Veto();
			return;
		}
		else if (wxDirExists(new_path)) {
			wxMessageDialog dialog(this, _("There already exist a folder with this name."), _("Error"), wxOK | wxICON_ERROR );
			dialog.ShowModal();
			event.Veto();
			return;
		}

		if (wxRenameFile(data->m_path, new_path))
		{
			wxLogDebug(wxT("rename done, setting new path"));
			wxString oldPath = data->m_path;
			data->SetNewPath( new_path );

			if (data->m_isDir && data->m_isExpanded) {
				RefreshSubItemPaths(id);
			}

			// chequeo si el archivo estaba abierto, lo cierro y vuelvo a abrir el nuevo
			// EditorCtrl -> SetPath(pathStr);

			// Open file in editor
			if (!data->m_isDir) {
				//m_projectService.OpenFile(new_path);
				//m_projectService.CloseTab(1, true);
				//wxAuiNotebook tabBar = m_projectService.GetTabBar();
				m_projectService.UpdateRenamedFile(oldPath, new_path);

			}
		}
		else
		{
			wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
			dialog.ShowModal();
			event.Veto();
		}
	}
}

void ProjectPane::OnItemActivated(wxTreeEvent& WXUNUSED(event)) {
	// Activate event restores focus to treeCtrl so we want to open items
	// in a subsequent event (otherwise it will steal focus from editorCtrl)
	wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, MENU_OPEN_TREEITEMS);
	AddPendingEvent(openEvent);
}

static bool projectpane_is_binary_file(const wxString& path) {
	// Read the first few bytes to see if this is a text file
	wxFFile file(path);
	char buffer[1024];
	const size_t bufLen = file.Read(buffer, 1024);
	for(size_t i = 0; i < bufLen; ++i) {
		if (buffer[i] == '\0') return true;
	}

	return false;
}

// Tries to execute a binary file.
// Returns true if we decided to execute it, false if the caller should try to open it in the editor anyway.
static bool projectpane_execute_binary(const wxString& name, const wxString& path) {
	const wxString ext = name.AfterLast(wxT('.'));
	if (ext.empty()) return false;

	// TODO: Check if this file ext is forced to open in e

	// .exe files are just run directly
	if (ext == wxT("exe")) {
		const wxString cmd = wxT("\"") + path + wxT("\"");
		wxExecute(cmd);
		return true;
	}

	// Check if another app is assigned to this file type
	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
	if (ft) {
		wxString mime;
		ft->GetMimeType(&mime);
		const wxString cmd = ft->GetOpenCommand(path);
		delete ft;

		if (mime != wxT("text/plain") && !cmd.empty()) {
			wxExecute(cmd);
			return true;
		}
	}

	return false;
}

void ProjectPane::OnMenuOpenTreeItems(wxCommandEvent& WXUNUSED(event)) {
	const bool shiftDown = wxGetKeyState(WXK_SHIFT);

	// Open all selected files
	wxArrayTreeItemIds items;
	if (!m_prjTree->GetSelections(items)) return;
	for (unsigned int i = 0; i < items.GetCount(); ++i) {
		const wxTreeItemId item = items[i];
		const DirItemData *data = (DirItemData*)m_prjTree->GetItemData(item);

		if (data->m_isDir) {
			if (data->m_isExpanded) CollapseDir(item);
			else {
				ExpandDir(item);
				m_prjTree->Expand(item);
			}
			continue;
		}

		if (data->m_isDir) continue;

		if (m_isRemote) {
			m_projectService.OpenRemoteFile(data->m_path, m_remoteProfile);
			continue;
		}

		// Always open file in e if shift is held down
		if (shiftDown) {
			m_projectService.OpenFile(data->m_path);
			continue;
		}

		bool isBinary = projectpane_is_binary_file(data->m_path);
		if (isBinary && projectpane_execute_binary(data->m_name, data->m_path)) continue;

		// If we get to here we should just open the file in e
		m_projectService.OpenFile(data->m_path);
	}
}

void ProjectPane::RenameItem() {
	wxArrayTreeItemIds items;
	m_prjTree->GetSelections(items);
	if (!items.IsEmpty()) {
		m_prjTree->EditLabel(items.Last());
	}
}

void ProjectPane::Upload(const wxString& url, const wxString& path) {
	wxASSERT(m_isRemote);

	SetBusy();
	m_remoteThread.UploadAsync(url, path, *m_remoteProfile, *this);
}

void ProjectPane::DeleteItems(bool allowUndo) {
	wxArrayTreeItemIds items;
	if (!m_prjTree->GetSelections(items) || items.GetCount() == 0) return;
	const size_t count = items.GetCount();

	if (m_isRemote) {
		// Get confirmation from user
		wxString msg;
		if (count == 1 ) msg = _("Are you sure you want to delete this item?");
		else msg = wxString::Format(_("Are you sure you want to delete these %d items?"), count);
		if (wxMessageBox(msg, _T("Deleting"), wxICON_WARNING|wxOK|wxCANCEL) != wxOK) return;

		SetBusy(); // reset in OnRemoteAction()
		for (size_t i = 0; i < count; ++i) {
			const DirItemData *data = (DirItemData*)m_prjTree->GetItemData( items[i] );
			m_remoteThread.Delete(data->m_path, *m_remoteProfile, *this);
		}
	}
	else {
		wxArrayString paths;
		for (size_t i = 0; i < count; ++i) {
			const DirItemData *data = (DirItemData*)m_prjTree->GetItemData( items[i] );
			paths.Add(data->m_path);
		}

		// Delete the files/dirs (thread will delete itself when done)
		new FileActionThread(FileActionThread::FILEACTION_DELETE, paths, allowUndo);
	}
}

void ProjectPane::OnTreeKeyDown(wxTreeEvent& event) {
	const int key = event.GetKeyCode();
	if (key == WXK_DELETE || key == WXK_BACK) {
		const bool allowUndo = !wxGetKeyState(WXK_SHIFT);
		DeleteItems(allowUndo);
	}
	
	// F2 (rename) & F5 (refresh) are caught as accelerators
	// to avoid conflicts with menu item shortcuts.
}

void ProjectPane::OnItemRename(wxCommandEvent& WXUNUSED(event)) {
	if (m_isRemote) return;
	RenameItem();
}

void ProjectPane::OnRefresh(wxCommandEvent& WXUNUSED(event)) {
	RefreshDirs();
}

void ProjectPane::OnTreeContextMenu(wxTreeEvent& event) {
	if (m_isRemote) return;

	wxTreeItemId item = event.GetItem();
	if (!item) return;

	bool showExtendedItems = wxGetKeyState(WXK_SHIFT);

	// Get the path of the item clicked
	const DirItemData *data = (DirItemData*)m_prjTree->GetItemData(item);
	const wxString& path = data->m_path;

	// Get the root dir that all items have to be contained in
	wxString rootdir;
	if (data->m_isDir) {
		wxFileName fn = wxFileName::DirName(path);

		if (fn.GetDirCount() == 0 && fn.HasVolume()) {
			// A drive can not have any siblings
			m_prjTree->UnselectAll();
		}
		else fn.RemoveLastDir();

		rootdir = fn.GetPath();
	}
	else {
		const wxFileName fn(path);
		rootdir = fn.GetPath();
	}

	wxArrayString paths;

	wxArrayTreeItemIds items;
	m_prjTree->GetSelections(items);

	m_prjTree->Freeze();

	bool clickOnSel = false;
	if (items.GetCount()) {
		// Add all selected items
		for (unsigned int i = 0; i < items.GetCount(); ++i) {
			const DirItemData *data = (DirItemData*)m_prjTree->GetItemData( items[i] );
			if (data->m_path == path) clickOnSel = true;

			// Get the containing path
			wxString dir;
			if (data->m_isDir) {
				wxFileName fn = wxFileName::DirName(data->m_path);
				fn.RemoveLastDir();
				dir = fn.GetPath();
			}
			else {
				const wxFileName fn(data->m_path);
				dir = fn.GetPath();
			}

			// Check if item is in the current dir
			// context menu can only deal with items all in same dir
			if (dir == rootdir) {
				paths.Add(data->m_path);
			}
			else m_prjTree->UnselectItem(items[i]);
		}
	}

	// if we clicked outside selection, we have to
	// deselect all but selection
	if (!clickOnSel) {
		m_prjTree->UnselectAll();
		paths.Clear();
	}

	// No selection, add item clicked
	if (paths.IsEmpty()) {
		m_prjTree->SelectItem(item);
		paths.Add(path);
	}

	m_prjTree->Thaw();

	const wxPoint screenPoint = ClientToScreen(event.GetPoint());

	// Show context menu
#ifdef __WXMSW__
	m_contextMenu = new ShellContextMenu();
	m_contextMenu->SetObjects(paths);
	m_contextMenu->ShowContextMenu(this, screenPoint, showExtendedItems);
	delete m_contextMenu;
	m_contextMenu = NULL;
#endif
}

void ProjectPane::OnTreeBeginDrag(wxTreeEvent& WXUNUSED(event)) {
	if (m_isRemote) return;

	wxArrayTreeItemIds items;
	if (!m_prjTree->GetSelections(items) || items.GetCount() == 0) return;

	wxFileDataObject fileData;
	for (unsigned int i = 0; i < items.GetCount(); ++i) {
		const DirItemData *data = (DirItemData*)m_prjTree->GetItemData( items[i] );
		fileData.AddFile(data->m_path);
	}

	wxLogDebug(wxT("Drag start (%d,%d)"), m_dragStartPos.x, m_dragStartPos.y);

	// Start drag
	wxDropSource dragSource(this);
	dragSource.SetData(fileData);
	dragSource.DoDragDrop(wxDrag_AllowMove);

	wxLogDebug(wxT("Drag end"));
}

void ProjectPane::OnDirChanged(wxDirWatcherEvent& event) {
	wxString path = event.GetChangedFile();
	const wxString prjPath = m_prjPath.GetPath();
	const int changeType = event.GetChangeType();
	
	wxLogDebug(wxT("%s Changed (%d)"), path.c_str(), changeType);

	//const wxString msg = wxString::Format(wxT("%s Changed (%d)\n"), path.c_str(), changeType);
	//OutputDebugString(msg);
	
	// On atomic saves we just ignore any changes
	if (path.EndsWith(wxT(".etmp"))) {
		if (changeType == DIRWATCHER_FILE_ADDED) m_atomicPath = path.substr(0, path.size()-5);
		else if (changeType == DIRWATCHER_FILE_RENAMED) m_atomicPath.clear(); // atomic save done
		return;
	}
	if (changeType == DIRWATCHER_FILE_REMOVED && path == m_atomicPath) return;
	
	// Make path relative to project
	wxString relativePath;
	if (!path.StartsWith(prjPath, &relativePath)) return;
	if (relativePath.empty()) return;

	//OutputDebugString(relativePath);

	wxArrayString dirs = wxStringTokenize(relativePath, wxFileName::GetPathSeparators(), wxTOKEN_STRTOK);
	if (dirs.IsEmpty()) return;

	// We need the taget file- or dir-name separate from the dirs
	wxString fileName = dirs.Last();
	dirs.RemoveAt(dirs.GetCount()-1);
	if (fileName == wxT(".e_preview_temp.css")) return; // don't show temp preview files

	//OutputDebugString(wxT(" c1\n"));

	// Check if the changed file is in a visible dir
	wxTreeItemId item = m_prjTree->GetRootItem();
	for (unsigned int i = 0; i < dirs.GetCount(); ++i) {
		//const wxString m = wxString::Format(wxT("   f%d\n"), i);
		//OutputDebugString(m);

		wxTreeItemIdValue cookie;
		wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
		const wxString& dir = dirs[i];

		while(subItem.IsOk()) {
			const wxString& itemText = m_prjTree->GetItemText(subItem);
			if (itemText == dir) {
				if (m_prjTree->IsExpanded(subItem)) {
					item = subItem;
					break;
				}
#ifdef __WXMSW__
				else if (path == m_newFolder || path == m_newFile) {
#else
				/* At Linux when new subdir is created by coping dir structure
				   we should process it this way
				*/
				else if ((path == m_newFolder || path == m_newFile) ||
					(true == event.IsDir())) {
#endif
					// If we have just created a new file/dir in this folder
					// (or one of it's children) we have to expand it.
					ExpandDir(subItem);
					m_prjTree->Expand(subItem);
					item = subItem;
					break;
				}
				else {
					// Make the dir expandable
					m_prjTree->SetItemHasChildren(subItem);

					return; // changed file not visible
				}
			}

			subItem = m_prjTree->GetNextChild(item, cookie);
		}
		if (!subItem.IsOk()) return; // item not found in subdir
	}

	//const wxString msg = wxString::Format(wxT("DEBUG: %s Changed (%d) - icons=%d\n"), path.c_str(), changeType, m_imageList.GetImageCount());
	//OutputDebugString(msg);
	//OutputDebugString(wxT(" c2\n"));

	switch (changeType) {
	case DIRWATCHER_FILE_MODIFIED:
		{
			wxTreeItemId id = FindSubItem(item, fileName);
			if (id.IsOk()) {
				RefreshIcon(id);
			}
		}
		break;

	case DIRWATCHER_FILE_REMOVED:
	case DIRWATCHER_FILE_RENAMED:
		{
#ifdef __WXGTK__
			if (event.IsDir()) {
				wxLogDebug(wxT("ProjectPane::%s() remove directory %s"),
					 wxString(__FUNCTION__,wxConvUTF8).c_str(), path.c_str());
				// need to remove dir watcher for this dir and all subdirs
				m_projectService.GetDirWatcher().UnwatchDirByName(path, true);
				// and for all subdirs
			}
#endif
			// Find the file
			wxTreeItemIdValue cookie;
			wxTreeItemId subItem = m_prjTree->GetFirstChild(item, cookie);
			while(subItem.IsOk()) {
				const wxString treeItem = m_prjTree->GetItemText(subItem);
				if (treeItem == fileName) {
					if (changeType == DIRWATCHER_FILE_REMOVED) {
						//OutputDebugString(wxT(" del_1\n"));
						m_prjTree->Delete(subItem);
						//OutputDebugString(wxT(" del_2\n"));
						return;
					}
					else break;
				}

				subItem = m_prjTree->GetNextChild(item, cookie);
			}
			const bool itemFound = subItem.IsOk();

			if (changeType == DIRWATCHER_FILE_RENAMED) {
				wxFileName newFile(event.GetNewFile());

				// If the new name does not match filter, remove it
				wxArrayString includeDirs;
				wxArrayString excludeDirs;
				wxArrayString includeFiles;
				wxArrayString excludeFiles;
				const wxFileName parentPath(path);
				m_infoHandler.GetFilters(parentPath.GetPath(), includeDirs, excludeDirs, includeFiles, excludeFiles);
				if (!m_infoHandler.MatchFilter(newFile.GetFullName(), includeDirs, excludeDirs)) {
					if (itemFound) m_prjTree->Delete(subItem);
					return;
				}
#ifdef __WXGTK__
				if (event.IsDir()) {
					// need to watch directory
					m_projectService.GetDirWatcher().WatchDirectory(event.GetNewFile(), *this, true);
					wxLogDebug(wxT("ProjectPane::%s() changed directory directory %s"),
					 	wxString(__FUNCTION__,wxConvUTF8).c_str(), path.c_str());
					// watch subdirs if exists
					WatchTree(event.GetNewFile());
				}
#endif
				if (itemFound) {
					// Rename the file
					m_prjTree->SetItemText(subItem, newFile.GetFullName());

					// We also have to update the DirInfo
					DirItemData *data = (DirItemData*)m_prjTree->GetItemData(subItem);
					data->SetNewPath(event.GetNewFile());

					// Refresh paths in all subitems
					if (data->m_isDir && data->m_isExpanded) {
						RefreshSubItemPaths(subItem);
					}
					break;
				}

				// fall-through to DIRWATCHER_FILE_ADDED
				path = event.GetNewFile();
				fileName = wxFileName(path).GetFullName();
			}
			else break;
		}
	case DIRWATCHER_FILE_ADDED:
		{
#ifdef __WXGTK__
			if (event.IsDir()) {
				// need to watch directory
				m_projectService.GetDirWatcher().WatchDirectory(path, *this, true);
				wxLogDebug(wxT("ProjectPane::%s() new directory %s"),
					 wxString(__FUNCTION__,wxConvUTF8).c_str(), path.c_str());
				// watch subdirs if exists
				WatchTree(path);
			}
#endif
#ifdef __WXMSW__
			// Ignore hidden files
			const DWORD dwAttrs = ::GetFileAttributes(path.c_str());
			if (dwAttrs & FILE_ATTRIBUTE_HIDDEN) return;
#endif //__WXMSW__

			// Get filters for parent dir
			wxArrayString includeDirs;
			wxArrayString excludeDirs;
			wxArrayString includeFiles;
			wxArrayString excludeFiles;
			const wxFileName parentPath(path);
			m_infoHandler.GetFilters(parentPath.GetPath(), includeDirs, excludeDirs, includeFiles, excludeFiles);

			if (wxDir::Exists(path)) {
				if (!m_infoHandler.MatchFilter(fileName, includeDirs, excludeDirs)) return;
				wxTreeItemId id = FindSubItem(item, fileName);
				if (!id.IsOk()) {
					// The dir may have been deleted/renamed again
					// so only add it if we can get icon
					const int image_id = AddFileIcon(path, true);
					if (image_id == wxNOT_FOUND) return;

					DirItemData *dir_item = new DirItemData(path, fileName, true, image_id, m_freeImages);
					id = m_prjTree->AppendItem(item, fileName, image_id, -1, dir_item);

					// Get filters for subdir
					includeDirs.Empty();
					excludeDirs.Empty();
					includeFiles.Empty();
					excludeFiles.Empty();
					m_infoHandler.GetFilters(path, includeDirs, excludeDirs, includeFiles, excludeFiles);

					if (!projectpane_is_dir_empty(path))	{
						m_prjTree->SetItemHasChildren(id);
					}
				}

				// If this is a folder we have just created, we give the user a
				// chance to rename it.
				if (path == m_newFolder) {
					m_prjTree->EditLabel(id);
					m_prjTree->UnselectAll();
					m_prjTree->SelectItem(id);
					m_newFolder.clear();
				}
			}
			else {
				if (!m_infoHandler.MatchFilter(fileName, includeFiles, excludeFiles)) return;

				wxTreeItemId id = FindSubItem(item, fileName);
				if (!id.IsOk()) {
					// The file may have been deleted/renamed again
					// so only add it if we can get icon
					const int image_id = AddFileIcon(path, false);
					if (image_id == wxNOT_FOUND) return;

					DirItemData *dir_item = new DirItemData(path, fileName, false, image_id, m_freeImages);
					id = m_prjTree->AppendItem(item, fileName, image_id, -1, dir_item);
				}

				if (path == m_newFile) {
					m_prjTree->UnselectAll();
					m_prjTree->SelectItem(id);
					m_prjTree->EnsureVisible(id);
					m_newFile.clear();
				}
			}
		}
		break;

	default:
		wxASSERT(false); // invalid change type
	}
	//OutputDebugString(wxT(" c3\n"));
}

void ProjectPane::OnNewDir(wxCommandEvent& WXUNUSED(event)) {
	if (m_prjTree->GetCount() == 0) return;

	if (m_isRemote) {
		// Ask for dirname
		wxTextEntryDialog dlg(this, _("Please enter name of new folder:"), _("New Folder"));
		if (dlg.ShowModal() != wxID_OK) return;
		if (dlg.GetValue().empty()) return;

		// Get the parent path
		wxString path;
		wxArrayTreeItemIds items;
		if (m_prjTree->GetSelections(items) && !items.empty()) {
			// Get the first selection
			DirItemData *data = (DirItemData*)m_prjTree->GetItemData(items[0]);
			if (data->m_isDir) path = data->m_path;
			else path = data->m_path.BeforeLast(wxT('/'));
		}
		else path = m_prjUrl; // Create dir in root

		// Add dirname
		if (path.Last() != wxT('/')) path += wxT('/');
		path += dlg.GetValue();

		// Create the file
		SetBusy();
		m_remoteThread.MkDir(path, *m_remoteProfile, *this);
	}
	else {
		// Get the parent path
		wxFileName path;
		wxArrayTreeItemIds items;
		if (m_prjTree->GetSelections(items) && !items.empty()) {
			// Get the first selection
			DirItemData *data = (DirItemData*)m_prjTree->GetItemData(items[0]);
			if (data->m_isDir) 	path = wxFileName(data->m_path, wxEmptyString);
			else path = data->m_path;
		}
		else {
			// Create dir in root
			path = m_prjPath;
		}

		// Create the dir
		wxFileName newDir(path);
		newDir.AppendDir(_("New Folder"));
		unsigned int count = 1;
		while (newDir.DirExists()) {
			newDir = path;
			newDir.AppendDir(wxString::Format(_("New Folder (%u)"), ++count));
		}

		// Remember path so we can edit name when notified of creation
		m_newFolder = newDir.GetPath();

		wxMkdir(m_newFolder);
	}
}

void ProjectPane::OnNewDoc(wxCommandEvent& WXUNUSED(event)) {
	if (m_prjTree->GetCount() == 0) return;

	if (m_isRemote) {
		// Ask for filename
		wxTextEntryDialog dlg(this, _("Please enter new filename:"), _("New File"));
		if (dlg.ShowModal() != wxID_OK) return;
		if (dlg.GetValue().empty()) return;

		// Get the parent path
		wxString path;
		wxArrayTreeItemIds items;
		if (m_prjTree->GetSelections(items) && !items.empty()) {
			// Get the first selection
			DirItemData *data = (DirItemData*)m_prjTree->GetItemData(items[0]);
			if (data->m_isDir) path = data->m_path;
			else path = data->m_path.BeforeLast(wxT('/'));
		}
		else path = m_prjUrl; // Create file in root

		// Add filename
		if (path.Last() != wxT('/')) path += wxT('/');
		path += dlg.GetValue();

		// Create the file
		SetBusy();
		m_remoteThread.CreateFile(path, *m_remoteProfile, *this);
	}
	else {
		// Get the parent path
		wxFileName path;
		wxArrayTreeItemIds items;
		if (m_prjTree->GetSelections(items) && !items.empty()) {
			// Get the first selection
			DirItemData *data = (DirItemData*)m_prjTree->GetItemData(items[0]);
			if (data->m_isDir) 	path = wxFileName(data->m_path, wxEmptyString);
			else path = data->m_path;
		}
		else path = m_prjPath; // Create file in root

		// Ask for filename
		wxTextEntryDialog dlg(this, _("Please enter new filename:"), _("New File"));
		if (dlg.ShowModal() != wxID_OK) return;
		path.SetFullName(dlg.GetValue());

		if (path.FileExists()) {
			wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
			dialog.ShowModal();
			return;
		}

		// Create the file
		m_newFile = path.GetFullPath();
		wxFile newfile(m_newFile, wxFile::write);
		if (!newfile.IsOpened()) {
			wxMessageDialog dialog(this, _("Could not create file."), _("Error"), wxOK | wxICON_ERROR );
			dialog.ShowModal();
			return;
		}
		newfile.Close();

		// Open file in editor
		m_projectService.OpenFile(path);
	}
}

void ProjectPane::OnButtonSettings(wxCommandEvent& WXUNUSED(event)) {
	// Get the current directory
	wxFileName path;
	wxArrayTreeItemIds items;
	if (m_prjTree->GetSelections(items) && !items.empty()) {
		// Get the first selection
		DirItemData *data = (DirItemData*)m_prjTree->GetItemData(items[0]);
		if (data->m_isDir) path.AssignDir(data->m_path);
		else {
			path = data->m_path;
			path.SetFullName(wxEmptyString);
		}
	}
	else {
		// Create file in root
		path = m_prjPath;
	}

	// Load project settings for current dir
	cxProjectInfo currentInfo;
	currentInfo.Load(m_prjPath, path.GetPath(), false);

	// Load inherited filters
	cxProjectInfo pInfo;
	if (!currentInfo.HasFilters() && path != m_prjPath) {
		path.RemoveLastDir();
		m_infoHandler.GetFilters(path.GetPath(), pInfo.includeDirs, pInfo.excludeDirs, pInfo.includeFiles, pInfo.excludeFiles);
	}

	// Show dialog
	ProjectSettings dlg(this, currentInfo, pInfo);
	if (dlg.ShowModal() == wxID_OK && dlg.IsModified()) {
		wxLogDebug(wxT("projectInfo ok and modified"));
		dlg.GetSettings(currentInfo);
		currentInfo.Save(m_prjPath.GetPath());
		RefreshDirs();
	}
}

void ProjectPane::AddFolderIcon(wxImageList& imageList) { // static
	wxASSERT(imageList.GetImageCount() == 0);

#ifdef __WXMSW__
	SHFILEINFO shfi;
	memset(&shfi,0,sizeof(shfi));
	LPCTSTR pszPath = (LPCTSTR)"";

	SHGetFileInfo(pszPath,
    FILE_ATTRIBUTE_DIRECTORY,
    &shfi, sizeof(shfi),
    SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON);

	wxIcon icon;
	icon.SetHICON(shfi.hIcon);
	imageList.Add(icon);
	//DestroyIcon(shfi.hIcon);  // not needed, destroyed by icon destuctor
#else
	wxIcon icon;
	if (false == GetIconFromFilePath(wxT("/"), icon)) {
		wxLogDebug(wxT("ProjectPane::%s() icon not found for /"), wxString(__FUNCTION__,
			 wxConvUTF8).c_str());
	} else {
		imageList.Add(icon);
	}
#endif
}

int ProjectPane::AddFileIcon(const wxString& path, bool isDir) {

	// Add a default icon (quick to retrieve)
	unsigned int index = 0; // zero is default dir icon
	if (!isDir) {
		wxIcon icon;
#ifdef __WXMSW__
		//OutputDebugString(wxT("DEBUG:   AddFileIcon\n")); // DEBUG
		SHFILEINFO shfi;
		memset(&shfi,0,sizeof(shfi));

		const int result = SHGetFileInfoW(path.c_str(),
		FILE_ATTRIBUTE_NORMAL,
		&shfi, sizeof(shfi),
		SHGFI_ICON|SHGFI_SMALLICON|SHGFI_USEFILEATTRIBUTES);

		if (result == 0) return wxNOT_FOUND;
		
		icon.SetHICON(shfi.hIcon);
#else
		if (false == GetIconFromFilePath(path, icon)) {
			wxLogDebug(wxT("ProjectPane::%s() icon not found for <%s>"), wxString(__FUNCTION__,
				 wxConvUTF8).c_str(), path.c_str());
			return wxNOT_FOUND;
		}
#endif
		// Add to imagelist
		if (m_freeImages.empty()) {
			index = m_imageList.Add(icon);
		}
		else {
			index = m_freeImages.back();
			m_freeImages.pop_back();
			m_imageList.Replace(index, icon);
		}
		//DestroyIcon(shfi.hIcon); // not needed, destroyed by icon destuctor
	}

	if (!m_isRemote) {
		// Retrieving a correct icon with overlays can be slow
		// especially on network drives. So we do it in a separate thread
		// (tree will be updated in idle time)
		m_iconPathsCrit.Enter();
			m_iconPathsForRetrieval.push_front(path);
		m_iconPathsCrit.Leave();

		// Signal thread that we have new icon paths
		wxMutexLocker lock(m_iconMutex);
		m_newIconsCond.Signal();
	}

	return index;
}

#ifdef __WXGTK__
bool ProjectPane::GetDefaultIcon(wxIcon &icon) {
	icon = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	return icon.IsOk();
}

bool ProjectPane::GetIconFromFilePath(const wxString& path, wxIcon &icon) {

	wxFileType* FileType = NULL;
	wxIconLocation IconLoc;
 	wxDir dir;

	wxLogDebug(wxT("ProjectPane::%s() path=%s"), wxString(__FUNCTION__, wxConvUTF8).c_str(), path.c_str());

//	if (wxFileName(path).IsDir()) {
	if (wxFileName::DirExists(path)) {
//	if (dir.Open(path)) {
		// get icon for directory 
		icon = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
		return icon.IsOk();
	}
	wxString FileExt = wxFileName(path).GetExt();
	if (wxT("") == FileExt) {
		// file without extension will have default icon
		return GetDefaultIcon(icon);
	}
	if (NULL == (FileType = wxTheMimeTypesManager->GetFileTypeFromExtension(FileExt))) {
		wxLogDebug(wxT("ProjectPane::%s() Can't get file type from ext=%s"),
			wxString(__FUNCTION__, wxConvUTF8).c_str(), FileExt.c_str());
		return GetDefaultIcon(icon);
	}
	if ((!FileType->GetIcon(&IconLoc)) || !IconLoc.IsOk()) {
		wxLogDebug(wxT("ProjectPane::%s() Can't get icon location"),
			wxString(__FUNCTION__, wxConvUTF8).c_str());
		return GetDefaultIcon(icon);
	}
	wxLogDebug(wxT("icon location <%s>"), IconLoc.GetFileName().c_str());
	if (!wxFile::Exists(IconLoc.GetFileName())) {
		wxLogDebug(wxT("ProjectPane::%s() Can't find icon <%s>."),
			wxString(__FUNCTION__, wxConvUTF8).c_str(), IconLoc.GetFileName().c_str());
		return GetDefaultIcon(icon);
	}
	wxIcon newIcon(IconLoc);
	if (!newIcon.IsOk()) {
		wxLogDebug(wxT("ProjectPane::%s() Icon is not OK"), wxString(__FUNCTION__, wxConvUTF8).c_str());
		return GetDefaultIcon(icon);
	}
	
	icon.CopyFromBitmap(newIcon.ConvertToImage().Rescale(16, 16, wxIMAGE_QUALITY_HIGH));
	return true;
}

/*
	Watch all subdirs except itself
*/
void ProjectPane::WatchTree(const wxString &path) {
	wxDir d;
	wxLogDebug(wxT("ProjectPane::%s() path=%s"), wxString(__FUNCTION__, wxConvUTF8).c_str(), path.c_str());
	d.Open(path);
	if (!d.IsOpened()) {
		return;
	}
	// Check for project settings
	wxArrayString includeDirs;
	wxArrayString excludeDirs;
	wxArrayString includeFiles;
	wxArrayString excludeFiles;
	m_infoHandler.GetFilters(path, includeDirs, excludeDirs, includeFiles, excludeFiles);

	// Get all subdirs
	wxString CurrentDir;
 	int style = wxDIR_DIRS;
	if (d.GetFirst(&CurrentDir, wxEmptyString, style)) {
		do {
			if (eDocumentPath::IsDotDirectory(CurrentDir)) continue;

			if (ProjectInfoHandler::MatchFilter(CurrentDir, includeDirs, excludeDirs)) {
				// watch dir and all subdirs
				wxString FullPath = path + wxT("/") + CurrentDir;
				m_projectService.GetDirWatcher().WatchDirectory(FullPath, *this, true);
				WatchTree(FullPath);
			}
		} while (d.GetNext(&CurrentDir));
	}
}
#endif

void* ProjectPane::Entry() {
	while (1) {
		while (!m_isDestroying && !m_iconPathsForRetrieval.empty()) {
			// Get path for which we should get icon
			m_iconPathsCrit.Enter();
				const wxString path = m_iconPathsForRetrieval.back();
				m_iconPathsForRetrieval.pop_back();
			m_iconPathsCrit.Leave();

			// Get the icon
			wxIcon icon;
#ifdef __WXMSW__
			SHFILEINFO shfi;
			memset(&shfi,0,sizeof(shfi));

			const int result = SHGetFileInfoW(path.c_str(),
			FILE_ATTRIBUTE_NORMAL,
			&shfi, sizeof(shfi),
			SHGFI_ICON|SHGFI_SMALLICON|SHGFI_ADDOVERLAYS);

			if (result == 0) continue;

			icon.SetHICON(shfi.hIcon); // icon destructor calls DestroyIcon for us
#else
			if (false == GetIconFromFilePath(path, icon)) {
				wxLogDebug(wxT("ProjectPane::%s() path=%s"),
					wxString(__FUNCTION__, wxConvUTF8).c_str(), path.c_str());
				continue;
			}
#endif

			// Shedule it for update during idle
			m_newIconsCrit.Enter();
				m_newIcons.push_back(PathIcon(path, icon));
			m_newIconsCrit.Leave();
		}
		if (m_isDestroying)
		    break;

		// Wait for new actions being pushed to the list
		wxMutexLocker lock(m_iconMutex);
		m_newIconsCond.Wait();
	}
	return NULL;
}

void ProjectPane::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	// Check if any corrected icons with overlays have been retrieved
	while(!m_newIcons.empty()) {
		m_newIconsCrit.Enter();
		const PathIcon pi = m_newIcons.back();
		m_newIcons.pop_back();
		m_newIconsCrit.Leave();

		const wxTreeItemId item = GetItemFromPath(pi.path);
		if (!item.IsOk()) continue;

		//OutputDebugString(wxT("DEBUG:   OnIdle\n")); // DEBUG

		// Add new icon to imagelist
		int index;
		if (m_freeImages.empty()) {
			index = m_imageList.Add(pi.icon);
		}
		else {
			index = m_freeImages.back();
			m_freeImages.pop_back();
			m_imageList.Replace(index, pi.icon);
		}

		// Update icon and itemdata
		m_prjTree->SetItemImage(item, index);
		DirItemData *data = (DirItemData*)m_prjTree->GetItemData(item);
		data->SetImage(index);
	}
}

#ifdef __WXMSW__
WXLRESULT ProjectPane::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) {

	// Contextmenu needs to intercept event sent to parent window
	if (m_contextMenu) {
		WXLRESULT res;
		if (m_contextMenu->HookWndProc(nMsg, wParam, lParam, res)) {
			return res;
		}
	}

	// Let the window do it's own event handling
	return wxPanel::MSWWindowProc(nMsg, wParam, lParam);
}
#endif //__WXMSW__


//-----------------------------------------------------------------------------
// DirItemData
//-----------------------------------------------------------------------------

ProjectPane::DirItemData::DirItemData(const wxString& path, const wxString& name, bool isDir, unsigned int image_id, std::vector<unsigned int>& fi)
: m_path(path), m_name(name), m_isDir(isDir), m_isHidden(false), m_isExpanded(false),
  m_imageId(image_id), m_freeImages(fi) {
}

ProjectPane::DirItemData::~DirItemData() {
	//const wxString msg = wxString::Format(wxT("delItem: %s (%d)\n"), m_path.c_str(), m_imageId);
	//OutputDebugString(msg);

	if (m_imageId != -1 && m_imageId != 0) m_freeImages.push_back(m_imageId); // zero is always generic folder
}

void ProjectPane::DirItemData::SetImage(unsigned int image_id) {
	//const wxString msg = wxString::Format(wxT("setImage: %s (%d -> %d)\n"), m_path.c_str(), m_imageId, image_id);
	//OutputDebugString(msg);

    if (m_imageId != -1 && m_imageId != 0) m_freeImages.push_back(m_imageId); // zero is always generic folder
    m_imageId = image_id;
}

void ProjectPane::DirItemData::SetNewPath(const wxString& path)
{
    m_path = path;
    m_name = wxFileNameFromPath(path);
}

// -- DropTarget -----------------------------------------------------------------

ProjectPane::DropTarget::DropTarget(ProjectPane& parent) : m_parent(parent) {
	SetDataObject(new wxFileDataObject);
}

wxDragResult ProjectPane::DropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def) {
	// Remove highlight
	if (m_lastOverItem.IsOk()) m_parent.m_prjTree->SetItemDropHighlight(m_lastOverItem, false);
	m_lastOverItem = wxTreeItemId(); // invalidate

	if (!TestDrop(x, y)) return wxDragNone;
	const bool allowUndo = !wxGetKeyState(WXK_SHIFT);

	// Which item are we over
	int flag;
	wxTreeItemId item = m_parent.m_prjTree->HitTest(wxPoint(x, y), flag);
	if (!item.IsOk()) return wxDragNone;
	const DirItemData *data = (DirItemData*)m_parent.m_prjTree->GetItemData(item);
	const wxString& path = data->m_path;

	// Get the file paths
	wxFileDataObject *dobj = (wxFileDataObject *)m_dataObject;
	const wxArrayString& filenames = dobj->GetFilenames();

	if (m_parent.m_isRemote) {
		// Get url of target dir
		const wxString parenturl = data->m_isDir ? data->m_path : data->m_path.BeforeLast(wxT('/')) + wxT('/');

		// Upload
		for (unsigned int i = 0; i < filenames.GetCount(); ++i) {
			const wxString& path = filenames[i];
			const wxString url = parenturl + wxFileNameFromPath(path);
			m_parent.Upload(url, path);
		}
	}
	else {
		// Get the target dir
		wxFileName target;
		if (data->m_isDir) target.AssignDir(path);
		else target = path;

		if (def == wxDragCopy) {
			// Copy the files/dirs (thread will delete itself when done)
			new FileActionThread(FileActionThread::FILEACTION_COPY, filenames, target.GetPath(), allowUndo);
		}
		else if (def == wxDragMove) {
			// Dragging in tree get easily triggered, so we want confirmation
			const wxString msg = (filenames.GetCount() == 1) ? wxT("Are you sure you want to move this item?")
				: wxString::Format(wxT("Are you sure you want to move these %d items?"), filenames.GetCount());

			if (!allowUndo || wxMessageBox(msg, wxT("Moving files"), wxYES_NO|wxICON_QUESTION, &m_parent) == wxYES) {
				// Move the files/dirs (thread will delete itself when done)
				new FileActionThread(FileActionThread::FILEACTION_MOVE, filenames, target.GetPath(), true);
			}
		}
	}

	return def;
}

wxDragResult ProjectPane::DropTarget::OnEnter(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def) {
	// Make sure last highlight is set invalid
	m_lastOverItem = wxTreeItemId();

	//return TestDrop(x, y) ? def : wxDragNone;
	return def;
}

wxDragResult ProjectPane::DropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
	// Which item are we over
	int flag;
	wxTreeItemId item = m_parent.m_prjTree->HitTest(wxPoint(x, y), flag);
	if (!item.IsOk()) return wxDragNone;

	// If we are over a dir we want to highlight it
	const DirItemData *data = (DirItemData*)m_parent.m_prjTree->GetItemData(item);
	if (data->m_isDir) {
		if (item != m_lastOverItem) {
			if (m_lastOverItem.IsOk()) m_parent.m_prjTree->SetItemDropHighlight(m_lastOverItem, false);

			m_parent.m_prjTree->SetItemDropHighlight(item);
			m_lastOverItem = item;
		}
	}
	else {
		if (m_lastOverItem.IsOk()) m_parent.m_prjTree->SetItemDropHighlight(m_lastOverItem, false);
		m_lastOverItem = wxTreeItemId(); // invalidate
	}

	return def;
}

bool ProjectPane::DropTarget::TestDrop(wxCoord x, wxCoord y) {
	if ( !GetData() )
        return wxDragNone;

	// Which item are we over
	int flag;
	wxTreeItemId item = m_parent.m_prjTree->HitTest(wxPoint(x, y), flag);
	if (!item.IsOk()) return false;
	const DirItemData *data = (DirItemData*)m_parent.m_prjTree->GetItemData(item);
	const wxString& path = data->m_path;

	// Get the file paths
	wxFileDataObject *dobj = (wxFileDataObject *)m_dataObject;
	const wxArrayString& filenames = dobj->GetFilenames();

	// Verify that we are not trying to copy a dir into itself
	for (unsigned int i = 0; i < filenames.GetCount(); ++i) {
		const wxString& dragPath = filenames[i];
		if (path.StartsWith(dragPath)) {
			return false;
		}
	}

	return true;
}

#ifdef CUSTOM_DRAGTREE
// -- DragTree -----------------------------------------------------------------

ProjectPane::DragTree::DragTree(wxWindow* parent, wxWindowID id)
: wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_MULTIPLE|wxTR_EDIT_LABELS),
  m_dragStartPos(wxDefaultPosition) {
}

#ifdef __WXMSW__

WXLRESULT ProjectPane::DragTree::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) {

	const int x = GET_X_LPARAM(lParam);
    const int y = GET_Y_LPARAM(lParam);
	const wxPoint pos(x, y);

	// wxTreeCtrls own drag handling seems to be over sensitive
	// so we use our own where we have a bit more control
	if (nMsg == WM_LBUTTONDOWN) {
		wxLogDebug(wxT("Drag LeftDown"));

		int flags;
		wxTreeItemId item = HitTest(pos, flags);

		if (item.IsOk()) {
			wxLogDebug(wxT("Drag LeftDown on item"));
			m_dragStartPos = pos;
		}
	}
	else if (nMsg == WM_MOUSEMOVE && m_dragStartPos != wxDefaultPosition) {
		// we also have to check against m_idEdited as edit mode eats LeftUp events
		if (m_idEdited) m_dragStartPos = wxDefaultPosition; // Restore state
		else {
			// No drag of remote items
			if (((ProjectPane*)m_parent)->IsRemote()) return 0;

			// We will start drag if we have moved beyond a couple of pixels
			// (we use double the threshold to minimize accidental drags)
			const int drag_x_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_X)*2;
			const int drag_y_threshold = wxSystemSettings::GetMetric(wxSYS_DRAG_Y)*2;

			if (abs(pos.x - m_dragStartPos.x) > drag_x_threshold ||
				abs(pos.y - m_dragStartPos.y) > drag_y_threshold)
			{
				// Create the file object of selected items
				RECT rect;
				wxArrayTreeItemIds items;
				if (!GetSelections(items) || items.GetCount() == 0) return 0;

				// Unselect all items
				UnselectAll();
				const wxColour unfocusedColour(214,211,206);

				wxFileDataObject fileData;
				for (unsigned int i = 0; i < items.GetCount(); ++i) {
					const DirItemData *data = (DirItemData*)GetItemData( items[i] );
					fileData.AddFile(data->m_path);

					// Re-select without focus
					SetItemBackgroundColour(items[i], unfocusedColour);
				}

				wxLogDebug(wxT("Drag start (%d,%d)"), m_dragStartPos.x, m_dragStartPos.y);

				// Start drag
				wxDropSource dragSource(this);
				dragSource.SetData(fileData);
				dragSource.DoDragDrop(wxDrag_AllowMove);

				wxLogDebug(wxT("Drag end"));

				// Restore selections
				for (unsigned int i = 0; i < items.GetCount(); ++i) {
					SetItemBackgroundColour(items[i], wxNullColour);
					SelectItem(items[i]);
				}

				// Restore state
				m_dragStartPos = wxDefaultPosition;
				return 0;
			}
		}
	}
	else if (nMsg == WM_LBUTTONUP && m_dragStartPos != wxDefaultPosition) {
		wxLogDebug(wxT("Drag LeftUp"));
		// Restore state
		m_dragStartPos = wxDefaultPosition;
	}
	else if (nMsg == WM_KILLFOCUS) {
		// cancel dnd
		wxLogDebug(wxT("KillFocus (cancel dnd)"));
		m_dragStartPos = wxDefaultPosition;
	}

	// Let the treectrl do it's own mouse handling
	return wxTreeCtrl::MSWWindowProc(nMsg, wParam, lParam);
}

// process WM_NOTIFY Windows message
bool ProjectPane::DragTree::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
	NMHDR *hdr = (NMHDR *)lParam;

    switch ( hdr->code )
    {
		case TVN_BEGINLABELEDIT:
		case TVN_ENDLABELEDIT:
			// Cancel any drag'n'drop
			wxLogDebug(wxT("LabelEdit (cancel dnd)"));
			m_dragStartPos = wxDefaultPosition;
			break;

		default:
			break;

	}

	// Let the treectrl do it's own event handling
	return wxTreeCtrl::MSWOnNotify(idCtrl, lParam, result);
}

#endif //__WXMSW__
#endif //CUSTOM_DRAGTREE
