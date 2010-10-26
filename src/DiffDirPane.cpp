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

#include "DiffDirPane.h"
#include <wx/dir.h>
#include <wx/filename.h>
#include "Catalyst.h"
#include "EditorFrame.h"

enum {
	ID_DIFFTREE,
	ID_MENU_COMPARE,
	ID_MENU_OPEN,
	ID_MENU_OPENLEFT,
	ID_MENU_OPENRIGHT,
	ID_MENU_COPYLEFT,
	ID_MENU_COPYRIGHT,
	ID_MENU_DELLEFT,
	ID_MENU_DELRIGHT
};

WX_DECLARE_STRING_HASH_MAP( wxTreeItemId, NameToTreeIdHash );

BEGIN_EVENT_TABLE(DiffDirPane, wxPanel)
	EVT_TREE_ITEM_GETTOOLTIP(ID_DIFFTREE, DiffDirPane::OnTreeGetToolTip)
	EVT_TREE_ITEM_MENU(ID_DIFFTREE, DiffDirPane::OnTreeMenu)
	EVT_TREE_ITEM_ACTIVATED(ID_DIFFTREE, DiffDirPane::OnTreeActivated)
	EVT_TREE_ITEM_EXPANDING(ID_DIFFTREE, DiffDirPane::OnTreeExpanding)
	EVT_MENU(ID_MENU_COMPARE, DiffDirPane::OnMenuCompare)
	EVT_MENU(ID_MENU_OPEN, DiffDirPane::OnMenuOpen)
	EVT_MENU(ID_MENU_OPENLEFT, DiffDirPane::OnMenuOpenLeft)
	EVT_MENU(ID_MENU_OPENRIGHT, DiffDirPane::OnMenuOpenRight)
	EVT_MENU(ID_MENU_COPYLEFT, DiffDirPane::OnMenuCopyToLeft)
	EVT_MENU(ID_MENU_COPYRIGHT, DiffDirPane::OnMenuCopyToRight)
	EVT_MENU(ID_MENU_DELLEFT, DiffDirPane::OnMenuDelLeft)
	EVT_MENU(ID_MENU_DELRIGHT, DiffDirPane::OnMenuDelRight)
END_EVENT_TABLE()

DiffDirPane::DiffDirPane(EditorFrame& parent)
: wxPanel(&parent), m_parentFrame(parent), m_imageList(16,16) {
	m_insColor.Set(192, 255, 192); // PASTEL GREEN
	m_delColor.Set(255, 192, 192); // PASTEL RED
	m_modColor.Set(185, 213, 255); // PASTEL BLUE
	AddFolderIcon();

	// Add tree control
	m_tree = new SortTreeCtrl(this, ID_DIFFTREE);
	m_tree->SetImageList(&m_imageList);

	// Create layout
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(m_tree, 1, wxEXPAND);

	SetSizer(sizer);
}

void DiffDirPane::SetDiff(const wxString& path1, const wxString& path2) {
	wxASSERT(wxDirExists(path1));
	wxASSERT(wxDirExists(path2));

	m_leftPath = path1;
	m_rightPath = path2;
	if (m_leftPath.Last() == wxFILE_SEP_PATH) m_leftPath.RemoveLast();
	if (m_rightPath.Last() == wxFILE_SEP_PATH) m_rightPath.RemoveLast();

	// Clean up
	m_tree->DeleteAllItems();

	// Add the root
	const wxString rootname = m_leftPath.AfterLast(wxFILE_SEP_PATH) + wxT(" <-> ") + m_rightPath.AfterLast(wxFILE_SEP_PATH);
	const wxTreeItemId root = m_tree->AddRoot(rootname, 0);

	// Do the comparison
	DoDirDiff(m_leftPath, m_rightPath, root);

	m_tree->Expand(root);
}

bool DiffDirPane::DoDirDiff(const wxString& path1, const wxString& path2, const wxTreeItemId& parent) {
	bool isModified = false;
	NameToTreeIdHash names;

	// Get subdirs in left path
	wxString name;
	wxDir leftDir(path1);
	bool cont = leftDir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
	while (cont) {
		const wxTreeItemId item = m_tree->AppendItem(parent, name, 0);
		m_tree->SetItemBackgroundColour(item, m_delColor); // Deletion is default
		m_tree->SetItemHasChildren(item);
		names[name] = item;

		cont = leftDir.GetNext(&name);
	}

	// Compare with subdirs in right path
	wxDir rightDir(path2);
	cont = rightDir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
	while (cont) {
		const wxString subpath2 = path2 + wxFILE_SEP_PATH + name;

		NameToTreeIdHash::iterator p = names.find(name);
		if (p != names.end()) {
			const wxTreeItemId item = p->second;
			const wxString subpath1 = path1 + wxFILE_SEP_PATH + name;

			if (DoDirDiff(subpath1, subpath2, item)) {
				m_tree->SetItemBackgroundColour(item, m_modColor); // modified
				isModified = true;
			}
			else m_tree->SetItemBackgroundColour(item, *wxWHITE); // unchanged
		}
		else {
			const wxTreeItemId item = m_tree->AppendItem(parent, name, 0);
			m_tree->SetItemBackgroundColour(item, m_insColor); // Addition
			m_tree->SetItemHasChildren(item);
		}

		cont = rightDir.GetNext(&name);
	}

	names.clear();

	// Get files in left path
	cont = leftDir.GetFirst(&name, wxEmptyString, wxDIR_FILES);
	while (cont) {
		const wxTreeItemId item = m_tree->AppendItem(parent, name, GetFileIcon(name));
		m_tree->SetItemBackgroundColour(item, m_delColor); // Deletion is default
		names[name] = item;

		cont = leftDir.GetNext(&name);
	}

	// Compare with files in right path
	cont = rightDir.GetFirst(&name, wxEmptyString, wxDIR_FILES);
	while (cont) {
		NameToTreeIdHash::iterator p = names.find(name);
		if (p != names.end()) {
			const wxTreeItemId item = p->second;
			const wxFileName file1(path1 + wxFILE_SEP_PATH + name);
			const wxFileName file2(path2 + wxFILE_SEP_PATH + name);

			if (file1.GetSize() != file2.GetSize() || file1.GetModificationTime() != file2.GetModificationTime()) {
				m_tree->SetItemBackgroundColour(item, m_modColor); // modified
				isModified = true;
			}
			else m_tree->SetItemBackgroundColour(item, *wxWHITE); // unchanged
		}
		else {
			const wxTreeItemId item = m_tree->AppendItem(parent, name, GetFileIcon(name));
			m_tree->SetItemBackgroundColour(item, m_insColor); // Addition
		}

		cont = rightDir.GetNext(&name);
	}

	m_tree->SortChildren(parent);

	return isModified;
}

void DiffDirPane::AddSubDir(const wxString& path, const wxTreeItemId& parent, const wxColour& color) {
	// Get subdirs
	wxString name;
	wxDir dir(path);
	bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
	while (cont) {
		const wxTreeItemId item = m_tree->AppendItem(parent, name, 0);
		m_tree->SetItemBackgroundColour(item, color);
		m_tree->SetItemHasChildren(item);

		cont = dir.GetNext(&name);
	}

	// Get files
	cont = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES);
	while (cont) {
		const wxTreeItemId item = m_tree->AppendItem(parent, name, GetFileIcon(name));
		m_tree->SetItemBackgroundColour(item, color);
		cont = dir.GetNext(&name);
	}
}


void DiffDirPane::AddFolderIcon() {
	// Folder image is always item zero
	wxASSERT(m_imageList.GetImageCount() == 0);

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
	m_imageList.Add(icon);
	//DestroyIcon(shfi.hIcon);  // not needed, destroyed by icon destuctor
#endif
}

int DiffDirPane::GetFileIcon(const wxString& path) {
	// Check if we have the icon cached
	const wxString ext = path.AfterLast(wxT('.'));
	IconHash::iterator p = m_iconHash.find(ext);
	if (p != m_iconHash.end()) return p->second;

	// Add a default icon (quick to retrieve)
	wxIcon icon;
#ifdef __WXMSW__
	SHFILEINFO shfi;
	memset(&shfi,0,sizeof(shfi));

	const int result = SHGetFileInfoW(path.c_str(),
	FILE_ATTRIBUTE_NORMAL,
	&shfi, sizeof(shfi),
	SHGFI_ICON|SHGFI_SMALLICON|SHGFI_USEFILEATTRIBUTES);

	if (result == 0) return wxNOT_FOUND;
	
	icon.SetHICON(shfi.hIcon);
#endif

	// Add to imagelist
	const int index = m_imageList.Add(icon);
	m_iconHash[ext] = index;

	return index;
}

wxString DiffDirPane::GetLeftPath(const wxTreeItemId& item) const {
	wxString path = m_leftPath;
	GetItemPath(item, path);
	return path;
}

wxString DiffDirPane::GetRightPath(const wxTreeItemId& item) const {
	wxString path = m_rightPath;
	GetItemPath(item, path);
	return path;
}

void DiffDirPane::GetItemPath(const wxTreeItemId& item, wxString& path) const {
	if (item == m_tree->GetRootItem()) return;
	
	GetItemPath(m_tree->GetItemParent(item), path);
	
	path += wxFILE_SEP_PATH;
	path += m_tree->GetItemText(item);
}

void DiffDirPane::OnTreeGetToolTip(wxTreeEvent& evt) {
	// Don't show tooltip for folders
	const wxTreeItemId item = evt.GetItem();
	if (m_tree->GetItemImage(item) == 0) return;

	const wxString leftPath = GetLeftPath(item);
	const wxString rightPath = GetRightPath(item);

	wxString tip = _("Left:\n  ");
	if (wxFileExists(leftPath)) {
		tip += leftPath + wxT("\n");
		const wxDateTime modDate = wxFileName(leftPath).GetModificationTime();
		const wxString age = Catalyst::GetDateAge(modDate);
		tip += modDate.Format(wxT("  %A, %B %d, %Y - %X (")) + age + wxT(")\n\n");
	}
	else tip += _("  not available\n\n");

	tip += _("Right:\n  ");
	if (wxFileExists(rightPath)) {
		tip += rightPath + wxT("\n");
		const wxDateTime modDate = wxFileName(rightPath).GetModificationTime();
		const wxString age = Catalyst::GetDateAge(modDate);
		tip += modDate.Format(wxT("  %A, %B %d, %Y - %X (")) + age + wxT(")");
	}
	else tip += _("  not available");

	evt.SetToolTip(tip);
}

void DiffDirPane::OnTreeMenu(wxTreeEvent& evt) {
	// Don't show context menu for folders
	const wxTreeItemId item = evt.GetItem();
	if (m_tree->GetItemImage(item) == 0) return;

	// Keep a ref to item for menu event handlers
	m_menuItem = item;

	// Use the color so we don't have to re-check for modification status
	const wxColour itemColor = m_tree->GetItemBackgroundColour(item);
	const bool isModified = (itemColor == m_modColor);

	wxMenu contextMenu;
	if (isModified) {
		contextMenu.Append(ID_MENU_COMPARE, _("&Compare"), _("Compare"));
		contextMenu.Append(ID_MENU_OPENLEFT, _("Open &Left version"), _("Open Left version"));
		contextMenu.Append(ID_MENU_OPENRIGHT, _("Open &Right version"), _("Open Right version"));
	}
	else contextMenu.Append(ID_MENU_OPEN, _("&Open"), _("Open"));
	contextMenu.AppendSeparator();

	const bool isDeleted = (itemColor == m_delColor);
	const bool isInserted = (itemColor == m_insColor);
	const bool isSame = (!isModified && !isDeleted && !isInserted);

	if (isModified || isInserted) {
		contextMenu.Append(ID_MENU_COPYLEFT, _("<-- Copy to Left"), _("<-- Copy to Left"));
	}
	if (isModified || isDeleted) {
		contextMenu.Append(ID_MENU_COPYRIGHT, _("--> Copy to Right"), _("--> Copy to Right"));
	}
	if (isDeleted || isSame) {
		contextMenu.Append(ID_MENU_DELLEFT, _("Delete from Left"), _("Delete from Left"));
	}
	if (isInserted || isSame) {
		contextMenu.Append(ID_MENU_DELRIGHT, _("Delete from Right"), _("Delete from Right"));
	}

	PopupMenu(&contextMenu);
}

void DiffDirPane::OnTreeActivated(wxTreeEvent& evt) {
	// Open folders on double-click
	const wxTreeItemId item = evt.GetItem();
	const int image_id = m_tree->GetItemImage(item);
	if (image_id == 0) {
		m_tree->Expand(item);
		return;
	}

	const wxString leftPath = GetLeftPath(item);
	const wxString rightPath = GetRightPath(item);

	if (m_tree->GetItemBackgroundColour(item) == m_modColor) {
		m_parentFrame.Compare(leftPath, rightPath);
		return;
	}

	if (wxFileExists(leftPath)) {
		m_parentFrame.Open(leftPath);
		return;
	}

	if (wxFileExists(rightPath)) {
		m_parentFrame.Open(rightPath);
		return;
	}
}

void DiffDirPane::OnTreeExpanding(wxTreeEvent& evt) {
	const wxTreeItemId item = evt.GetItem();

	// Nothing to do if folder has already been expanded
	if (m_tree->GetChildrenCount(item) > 0) return;

	const wxString leftPath = GetLeftPath(item);
	if (wxDirExists(leftPath)) {
		AddSubDir(leftPath, item, m_delColor);
		return;
	}

	const wxString rightPath = GetRightPath(item);
	if (wxDirExists(rightPath)) {
		AddSubDir(rightPath, item, m_insColor);
		return;
	}
}

void DiffDirPane::OnMenuCompare(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	const wxString rightPath = GetRightPath(m_menuItem);

	m_parentFrame.Compare(leftPath, rightPath);
}

void DiffDirPane::OnMenuOpen(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	if (wxFileExists(leftPath)) {
		m_parentFrame.Open(leftPath);
		return;
	}

	const wxString rightPath = GetRightPath(m_menuItem);
	if (wxFileExists(rightPath)) {
		m_parentFrame.Open(rightPath);
		return;
	}
}

void DiffDirPane::OnMenuOpenLeft(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	if (wxFileExists(leftPath)) {
		m_parentFrame.Open(leftPath);
	}

	const wxString rightPath = GetRightPath(m_menuItem);
	if (wxFileExists(rightPath)) {
		m_parentFrame.Open(rightPath);
	}
}

void DiffDirPane::OnMenuOpenRight(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString rightPath = GetRightPath(m_menuItem);
	if (wxFileExists(rightPath)) {
		m_parentFrame.Open(rightPath);
	}
}

void DiffDirPane::OnMenuCopyToLeft(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	const wxString rightPath = GetRightPath(m_menuItem);

	if (wxCopyFile(rightPath, leftPath)) {
		m_tree->SetItemBackgroundColour(m_menuItem, *wxWHITE); // unchanged
	}
}

void DiffDirPane::OnMenuCopyToRight(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	const wxString rightPath = GetRightPath(m_menuItem);

	if (wxCopyFile(leftPath, rightPath)) {
		m_tree->SetItemBackgroundColour(m_menuItem, *wxWHITE); // unchanged
	}
}

void DiffDirPane::OnMenuDelLeft(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	const wxString rightPath = GetRightPath(m_menuItem);

	if (!wxRemoveFile(leftPath)) return;

	if (wxFileExists(rightPath)) {
		m_tree->SetItemBackgroundColour(m_menuItem, m_insColor);
	}
	else m_tree->Delete(m_menuItem);
}

void DiffDirPane::OnMenuDelRight(wxCommandEvent& WXUNUSED(evt)) {
	if (!m_menuItem.IsOk()) return;

	const wxString leftPath = GetLeftPath(m_menuItem);
	const wxString rightPath = GetRightPath(m_menuItem);

	if (!wxRemoveFile(rightPath)) return;

	if (wxFileExists(leftPath)) {
		m_tree->SetItemBackgroundColour(m_menuItem, m_delColor);
	}
	else m_tree->Delete(m_menuItem);
}


// ---- SortTreeCtrl ----------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(DiffDirPane::SortTreeCtrl, wxTreeCtrl)

DiffDirPane::SortTreeCtrl::SortTreeCtrl(wxWindow* parent, wxWindowID id)
: wxTreeCtrl(parent, id,  wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS) {
}

int DiffDirPane::SortTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2) {
	const int image1 = GetItemImage(item1);
	const int image2 = GetItemImage(item2);

	// Folders always have image set to zero
	if (image1 == 0 && image2 > 0) return -1;
	if (image2 == 0 && image1 > 0) return 1;

	const wxString text1 = GetItemText(item1);
	const wxString text2 = GetItemText(item2);

	return text1.CmpNoCase(text2);
}
