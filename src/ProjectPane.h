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

#ifndef __PROJECTPANE_H__
#define __PROJECTPANE_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/treectrl.h>
#include <wx/filename.h>
#include <wx/dnd.h>
#include <wx/imaglist.h>

#include "ProjectInfoHandler.h"
#include "ProjectInfo.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <deque>
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;


// pre-definitions
class wxDirWatcherEvent;

#ifdef __WXMSW__
class ShellContextMenu;
#endif

class IFrameProjectService;
class RemoteThread;
class RemoteProfile;
class cxRemoteListEvent;
class cxRemoteAction;

class ProjectPane : public wxPanel, public wxThreadHelper {
public:
	ProjectPane(IFrameProjectService& parent, wxWindowID id = wxID_ANY);
	~ProjectPane();

	bool IsFocused() const;
	bool IsRemote() const {return m_isRemote;};
	void SetBusy(bool busy=true);
	void ResetBusy();

	bool SetProject(const wxFileName& path);
	bool SetRemoteProject(const RemoteProfile* rp);

	void Clear();
	virtual bool HasProject() const {return m_prjPath.IsOk();};
	virtual const wxFileName& GetRootPath() const {return m_prjPath;};
	wxString GetProjectString() const {if (IsRemote()) return m_prjUrl; else return m_prjPath.GetFullPath();};
	wxArrayString GetSelections() const;
	const map<wxString,wxString>& GetEnv() const {return m_infoHandler.GetRootInfo().env;};

	void RenameItem();
	void DeleteItems(bool allowUndo);
	void RefreshDirs();
	void Upload(const wxString& url, const wxString& path);

	// ProjectInfo
	ProjectInfoHandler& GetInfoHandler() {return m_infoHandler;};
	void SaveCurrentProjectInfo() const {m_infoHandler.SaveRootInfo();};
	
#ifdef __WXMSW__
	WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif

private:
	class DirItemData : public wxTreeItemData
	{
	public:
		DirItemData(const wxString& path, const wxString& name, bool isDir, unsigned int image_id, vector<unsigned int>& freeImages);
		~DirItemData();

		void SetNewPath(const wxString& path);
		void SetImage(unsigned int image_id);

		wxString m_path, m_name;
		bool m_isDir;
		bool m_isHidden;
		bool m_isExpanded;
		int m_imageId;
		vector<unsigned int>& m_freeImages;
	};

#ifdef __WXGTK__
	static bool GetIconFromFilePath(const wxString& path, wxIcon &icon);
	static bool GetDefaultIcon(wxIcon &icon);
	void WatchTree(const wxString &path);
#endif

	void Init();

	void ExpandDir(wxTreeItemId parentId);
	void ExpandDirWait(wxTreeItemId parentId);
	void ExpandDir(wxTreeItemId parentId, DirItemData *data, const wxArrayString& dirs, const wxArrayString& filenames);
	void CollapseDir(wxTreeItemId parentId);
	wxTreeItemId FindSubItem(const wxTreeItemId& item, const wxString& label) const;
	wxTreeItemId GetItemFromPath(const wxString& path) const;
	wxTreeItemId GetItemFromUrl(const wxString& url) const;
	wxTreeItemId GetItemFromNames(const wxArrayString& dirs) const;

	void RefreshIcon(const wxTreeItemId& item);
	void RefreshSubItemPaths(const wxTreeItemId& item);

	static void AddFolderIcon(wxImageList& imageList);
	int AddFileIcon(const wxString& path, bool isDir);

	void ExpandAndSelect(wxTreeItemId item, wxArrayString& expandedDirs, wxArrayString& selections);
	void GetExpandedDirs(wxTreeItemId item, wxArrayString& dirs);
	bool IsDirEmpty(const wxString& path) const;

	// Icon retrieval thread
	void* Entry();

	// Event handlers
	void OnExpandItem(wxTreeEvent& event);
	void OnCollapseItem(wxTreeEvent& event);
	void OnBeginEditItem(wxTreeEvent &event);
	void OnEndEditItem(wxTreeEvent& event);
	void OnItemActivated(wxTreeEvent& event);
	void OnTreeKeyDown(wxTreeEvent& event);
	void OnTreeContextMenu(wxTreeEvent& event);
	void OnTreeBeginDrag(wxTreeEvent& event);
	void OnMenuOpenTreeItems(wxCommandEvent& event);
	void OnDirChanged(wxDirWatcherEvent& event);
	void OnRemoteListReceived(cxRemoteListEvent& event);
	void OnRemoteAction(cxRemoteAction& event);
	void OnNewDir(wxCommandEvent& event);
	void OnNewDoc(wxCommandEvent& event);
	void OnButtonSettings(wxCommandEvent& event);
	void OnItemRename(wxCommandEvent& event);
	void OnRefresh(wxCommandEvent& event);
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE()

	class DropTarget : public wxDropTarget {
	public:
		DropTarget(ProjectPane& parent);
		wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
		wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
		wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
	private:
		bool TestDrop(wxCoord x, wxCoord y);
		ProjectPane& m_parent;
		wxTreeItemId m_lastOverItem;
	};

#ifdef CUSTOM_DRAGTREE
	class DragTree : public wxTreeCtrl {
	public:
		DragTree(wxWindow* parent, wxWindowID id);
#ifdef __WXMSW__
		WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
		bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);
#endif //__WXMSW__
	private:
		wxPoint m_dragStartPos;
	};
#endif //CUSTOM_DRAGTREE

	// Ctrl's
	wxBitmapButton* m_newDirButton;
	wxBitmapButton* m_newDocButton;
	wxButton* m_settingsButton;

	// Member variables
	IFrameProjectService& m_parentFrame;
	ProjectInfoHandler m_infoHandler;
	wxImageList m_imageList;
	void* m_dirWatchHandle;
	bool m_isRemote;
	bool m_isDestroying;
	RemoteThread& m_remoteThread;
	wxTreeCtrl* m_prjTree;

	wxFileName m_prjPath;

	const RemoteProfile* m_remoteProfile;
	wxString m_prjUrl;
	vector<unsigned int> m_freeImages;
	wxString m_newFolder; // for OnDirChanged
	wxString m_newFile;	  // for OnDirChanged
	wxString m_atomicPath; // to ignore atomic saves

#ifdef __WXMSW__
    ShellContextMenu* m_contextMenu;
#endif

	wxString m_waitingForDir;

	// Drag state
	wxPoint m_dragStartPos;

	// Busy state
	unsigned int m_busyCount;

	// Icon retrieval thread
	wxMutex m_iconMutex;
	wxCondition m_newIconsCond;
	wxCriticalSection m_iconPathsCrit;
	deque<wxString> m_iconPathsForRetrieval;
	wxCriticalSection m_newIconsCrit;

	class PathIcon {
	public:
		PathIcon(const wxString& path, const wxIcon& icon) : path(path), icon(icon) {};
		PathIcon(const PathIcon& pi) {path = pi.path; icon = pi.icon;};
		wxString path;
		wxIcon icon;
	};

	vector<PathIcon> m_newIcons;
#ifdef __WXGTK__
	bool isDirWatched;
#endif

	friend class DropTarget;
};

#endif // __PROJECTPANE_H__
