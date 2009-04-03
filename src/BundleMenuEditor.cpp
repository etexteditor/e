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

#include "BundleEditor.h"

#include "images/tmBundle.xpm"
#include "images/tmCommand.xpm"
#include "images/tmSnippet.xpm"
#include "images/tmSubMenu.xpm"
#include "images/tmUnknown.xpm"
#include "images/tmSeparator.xpm"

enum {
	CTRL_MENUTREE = 100,
	CTRL_REMOVEBUTTON,
	CTRL_SUBDIRBUTTON,
	CTRL_SEPARATORBUTTON
};

BEGIN_EVENT_TABLE(BundleEditor::MenuPanel, wxPanel)
	EVT_TREE_ITEM_ACTIVATED(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemActivated)
	EVT_TREE_SEL_CHANGED(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemSelected)
	EVT_TREE_ITEM_COLLAPSING(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemCollapsing)
	EVT_TREE_BEGIN_LABEL_EDIT(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemBeginEdit)
	EVT_TREE_END_LABEL_EDIT(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemEndEdit)
	EVT_TREE_BEGIN_DRAG(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemBeginDrag)
	EVT_TREE_END_DRAG(CTRL_MENUTREE, BundleEditor::MenuPanel::OnTreeItemEndDrag)
	EVT_BUTTON(CTRL_REMOVEBUTTON, BundleEditor::MenuPanel::OnRemoveButton)
	EVT_BUTTON(CTRL_SEPARATORBUTTON, BundleEditor::MenuPanel::OnSeparatorButton)
	EVT_BUTTON(CTRL_SUBDIRBUTTON, BundleEditor::MenuPanel::OnSubDirButton)
END_EVENT_TABLE()

BundleEditor::MenuPanel::MenuPanel(BundleEditor& parent, PListHandler& plistHandler)
: wxPanel(&parent), m_parentDlg(parent), m_plistHandler(plistHandler), m_imageList(16,16), m_isModified(false), m_itemsModified(false) {
	// Build Imagelist
	m_imageList.Add(wxIcon(tmbundle_xpm));
	m_imageList.Add(wxIcon(tmcommand_xpm));
	m_imageList.Add(wxIcon(tmsnippet_xpm));
	m_imageList.Add(wxIcon(tmsubmenu_xpm));
	m_imageList.Add(wxIcon(tmunknown_xpm));
	m_imageList.Add(wxIcon(tmseparator_xpm));

	// Create controls
	m_menuTree = new wxTreeCtrl(this, CTRL_MENUTREE, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_EDIT_LABELS);
	m_menuTree->SetImageList(&m_imageList);
	m_menuTree->SetMinSize(wxSize(50,50)); // ensure resizeability
	wxButton* addSubMenu = new wxButton(this, CTRL_SUBDIRBUTTON, _("Add Submenu"));
	wxButton* addSeparator = new wxButton(this, CTRL_SEPARATORBUTTON, _("Add Separator"));
	m_removeButton = new wxButton(this, CTRL_REMOVEBUTTON, _("Remove Item"));
	m_removeButton->Disable();

	// Create the layout
	wxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
		mainSizer->Add(m_menuTree, 1, wxEXPAND|wxALL, 5);
		wxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
			buttonSizer->Add(addSubMenu, 0, wxEXPAND|wxBOTTOM, 5);
			buttonSizer->Add(addSeparator, 0, wxEXPAND|wxBOTTOM, 5);
			buttonSizer->Add(m_removeButton, 0, wxEXPAND);
			mainSizer->Add(buttonSizer, 0, wxALL, 5);

	SetSizer(mainSizer);
}

void BundleEditor::MenuPanel::SetBundle(unsigned int bundleId) {
	m_bundleId = bundleId;
	m_menuTree->DeleteAllItems();
	m_actionMap.clear();
	m_isModified = false;
	m_itemsModified = false;

	// Get bundle info
	const PListDict infoDict = m_plistHandler.GetBundleInfo(m_bundleId);
	const wxString bundleName = infoDict.wxGetString("name");

	// Add the root node (can not be edited)
	const wxTreeItemId rootItem = m_menuTree->AddRoot(bundleName, 0);


	// Build a map of id's of bundle actions
	const vector<unsigned int> commands =  m_plistHandler.GetList(BUNDLE_COMMAND, bundleId);
	for (unsigned int c = 0; c < commands.size(); ++c) {
		const unsigned int commandId = commands[c];

		const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, bundleId, commandId);
		const wxString uuid = cmdDict.wxGetString("uuid");

		m_actionMap[uuid] = BundleAction(BUNDLE_COMMAND, commandId);
	}
	const vector<unsigned int> snippets =  m_plistHandler.GetList(BUNDLE_SNIPPET, bundleId);
	for (unsigned int n = 0; n < snippets.size(); ++n) {
		const unsigned int snippetId = snippets[n];

		const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, bundleId, snippetId);
		const wxString uuid = snipDict.wxGetString("uuid");

		m_actionMap[uuid] = BundleAction(BUNDLE_SNIPPET, snippetId);
	}

	// Check if there is a menu
	PListDict menuDict;
	if (infoDict.GetDict("mainMenu", menuDict)) {
		PListArray itemsArray;
		if (!menuDict.GetArray("items", itemsArray)) return;

		PListDict submenuDict;
		menuDict.GetDict("submenus", submenuDict);

		// Parse the menu
		ParseMenu(itemsArray, submenuDict, rootItem);
	}
	else {
		// If there is no main menu structure, we use the ordering
		PListArray itemsArray;
		if (!infoDict.GetArray("ordering", itemsArray)) return;

		// Parse the menu
		ParseMenu(itemsArray, PListDict(), rootItem);
	}

	m_menuTree->Expand(rootItem);
}

void BundleEditor::MenuPanel::UpdateBundleName(const wxString& name) {
	const wxTreeItemId rootItem = m_menuTree->GetRootItem();
	m_menuTree->SetItemText(rootItem, name);
}

bool BundleEditor::MenuPanel::Save() {
	if (m_isModified) {
		m_plistHandler.SaveBundle(m_bundleId);
		m_isModified = false;
		m_itemsModified = false;
		return true;
	}

	if (m_itemsModified) {
		m_itemsModified = false;
		return true;
	}

	return false;
}

void BundleEditor::MenuPanel::ParseMenu(const PListArray& itemsArray, const PListDict& submenuDict, const wxTreeItemId& parentItem) {
	const bool hasSubmenus = submenuDict.IsOk();
	PListDict subDict;

	for (unsigned int i = 0; i < itemsArray.GetSize(); ++i) {
		const char* item = itemsArray.GetString(i);

		if (item[0] == '-') {
			m_menuTree->AppendItem(parentItem, wxT("---- separator ----"), 5);
		}
		else {
			// Look for submenu
			if (hasSubmenus && submenuDict.GetDict(item, subDict)) {
				const wxString name = subDict.wxGetString("name");
				const wxString uuid(item, wxConvUTF8);
				const wxTreeItemId subItem = m_menuTree->AppendItem(parentItem, name, 3, -1, new MenuItemData(BUNDLE_SUBDIR, 0, uuid));

				PListArray subArray;
				if (subDict.GetArray("items", subArray)) {
					ParseMenu(subArray, submenuDict, subItem);
				}

				continue;
			}

			const wxString uuid(item, wxConvUTF8);

			// Look for action
			map<wxString, BundleAction>::iterator s = m_actionMap.find(uuid);
			if (s != m_actionMap.end()) {
				BundleAction& ba = s->second;

				if (ba.m_type == BUNDLE_COMMAND) {
					const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, m_bundleId, ba.m_index);
					const wxString name = cmdDict.wxGetString("name");
					m_menuTree->AppendItem(parentItem, name, 1, -1, new MenuItemData(BUNDLE_COMMAND, ba.m_index, uuid));
				}
				else if (ba.m_type == BUNDLE_SNIPPET) {
					const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, m_bundleId, ba.m_index);
					const wxString name = snipDict.wxGetString("name");
					m_menuTree->AppendItem(parentItem, name, 2, -1, new MenuItemData(BUNDLE_SNIPPET, ba.m_index, uuid));
				}

				ba.SetInMenu(true);
				continue;
			}

			// Unknown item
			const wxTreeItemId unknownItem = m_menuTree->AppendItem(parentItem, _("unsupported action"), 4, -1, new MenuItemData(BUNDLE_NONE, 0, uuid));
			wxFont font = m_menuTree->GetItemFont(unknownItem);
			font.SetStyle(wxFONTSTYLE_ITALIC);
			m_menuTree->SetItemFont(unknownItem, font);
		}
	}
}

PListDict BundleEditor::MenuPanel::GetEditablePlist() {
	PListDict infoDict = m_plistHandler.GetEditableBundleInfo(m_bundleId);

	// Check if we have to create menu from ordering
	if (!infoDict.HasKey("mainMenu")) {
		// Create menu entry
		PListDict menuDict = infoDict.NewDict("mainMenu");
		PListArray itemsArray = menuDict.NewArray("items");

		// Copy entries from ordering
		PListArray orderingArray;
		if (infoDict.GetArray("ordering", orderingArray)) {
			for (unsigned int i = 0; i < orderingArray.GetSize(); ++i) {
				const char* uuid = orderingArray.GetString(i);
				if (!uuid) continue;

				/*// Only add commands and snippets
				const wxString uuidStr(uuid, wxConvUTF8);
				map<wxString, BundleAction>::const_iterator s = m_actionMap.find(uuidStr);
				if (s != m_actionMap.end()) {
					itemsArray.AddString(uuid);
				}*/

				itemsArray.AddString(uuid);
			}
		}
	}

	m_isModified = true;
	return infoDict;
}

void BundleEditor::MenuPanel::RemoveItem(const wxTreeItemId item, bool deep) {
	if (item == m_menuTree->GetRootItem()) return;

	// Get the parent
	const wxTreeItemId parent = m_menuTree->GetItemParent(item);

	// Count to get index of item
	unsigned int ndx = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_menuTree->GetFirstChild(parent, cookie);
	while (child.IsOk() && child != item) {
		++ndx;
		child = m_menuTree->GetNextChild(parent, cookie);
	}
	wxASSERT(child == item);

	// Remove item from treeCtrl
	m_menuTree->Delete(item);

	// Get uuid of parent
	wxString parentUuid;
	if (parent != m_menuTree->GetRootItem()) {
		const MenuItemData* data = (MenuItemData*)m_menuTree->GetItemData(parent);
		parentUuid = data->m_uuid;
	}

	PListDict infoDict = GetEditablePlist();
	PListDict menuDict;
	if (!infoDict.GetDict("mainMenu", menuDict)) {
		wxFAIL_MSG(wxT("No mainMenu in info.plist"));
		return;
	}
	PListDict submenuDict;
	menuDict.GetDict("submenus", submenuDict);

	PListArray itemsArray;
	if (parentUuid.empty()) {
		if (!menuDict.GetArray("items", itemsArray)) {
			wxFAIL_MSG(wxT("mainMenu does not contain an item array in info.plist"));
			return;
		}
	}
	else {
		if (!submenuDict.IsOk()) {
			wxFAIL_MSG(wxT("no submenus in info.plist"));
			return;
		}

		PListDict subDict;
		if (!submenuDict.GetDict(parentUuid.mb_str(wxConvUTF8), subDict)) {
			wxFAIL_MSG(wxT("could not find submenu in info.plist"));
			return;
		}

		if (!subDict.GetArray("items", itemsArray)) {
			wxFAIL_MSG(wxT("no items in submenu"));
			return;
		}
	}

	const char* uuid = itemsArray.GetString(ndx);
	map<wxString, BundleAction>::iterator s = m_actionMap.find(wxString(uuid, wxConvUTF8));
	if (s != m_actionMap.end()) {
		// Note that item has been removed from menu
		s->second.SetInMenu(false);
	}
	else if (deep) {
		// If it is a subdir, we have to recursively delete all subdirs
		if (submenuDict.IsOk() && submenuDict.HasKey(uuid)) {
			DeleteSubDir(uuid);
		}
	}

	// Remove the item
	itemsArray.DeleteItem(ndx);


}

void BundleEditor::MenuPanel::DeleteSubDir(const char* uuid) {
	wxASSERT(uuid);
	PListDict infoDict = GetEditablePlist();

	PListDict menuDict;
	if (!infoDict.GetDict("mainMenu", menuDict)) {
		wxFAIL_MSG(wxT("No mainMenu in info.plist"));
		return;
	}

	PListDict submenuDict;
	if (!menuDict.GetDict("submenus", submenuDict)) {
		wxFAIL_MSG(wxT("No submenus in info.plist"));
		return;
	}

	PListDict itemDict;
	if (!submenuDict.GetDict(uuid, itemDict)) {
		wxFAIL_MSG(wxT("Could not find submenu in info.plist"));
		return;
	}

	// Recursively delete all subdirs
	PListArray subArray;
	if (submenuDict.GetArray("items", subArray)) {
		for (unsigned int i = 0; i < subArray.GetSize(); ++i) {
			const char* itemId = subArray.GetString(i);
			if (submenuDict.HasKey(itemId)) {
				DeleteSubDir(itemId);
			}
			else {
				// Note that item has been removed from menu
				map<wxString, BundleAction>::iterator s = m_actionMap.find(wxString(itemId, wxConvUTF8));
				if (s != m_actionMap.end()) {
					s->second.SetInMenu(false);
				}
			}
		}
	}

	submenuDict.DeleteItem(uuid);
}

void BundleEditor::MenuPanel::OnTreeItemActivated(wxTreeEvent& event) {
	// The root item cannot be activated
	if (event.GetItem() == m_menuTree->GetRootItem()) return;

	const wxTreeItemId item = event.GetItem();
	const MenuItemData* data = (MenuItemData*)m_menuTree->GetItemData(item);
	if (data && (data->m_type == BUNDLE_COMMAND || data->m_type == BUNDLE_SNIPPET)) {
		m_parentDlg.SelectItem(data->m_type, m_bundleId, data->m_itemId);
	}
}

void BundleEditor::MenuPanel::OnTreeItemSelected(wxTreeEvent& event) {
	// The root item cannot be removed
	if (event.GetItem() == m_menuTree->GetRootItem()) {
		m_removeButton->Disable();
	}
	else m_removeButton->Enable();
}

void BundleEditor::MenuPanel::OnTreeItemCollapsing(wxTreeEvent& event) {
	// The root item cannot be collapsed
	if (event.GetItem() == m_menuTree->GetRootItem()) {
		event.Veto();
	}
}

void BundleEditor::MenuPanel::OnTreeItemBeginEdit(wxTreeEvent& event) {
	const wxTreeItemId item = event.GetItem();
	const MenuItemData* data = (MenuItemData*)m_menuTree->GetItemData(item);

	// root, separators and unknowns cannot be edited
	if (!data || data->m_type == BUNDLE_NONE) {
		event.Veto();
	}
}

void BundleEditor::MenuPanel::OnTreeItemEndEdit(wxTreeEvent& event) {
	const wxTreeItemId item = event.GetItem();
	const MenuItemData* data = (MenuItemData*)m_menuTree->GetItemData(item);
	const wxString& label = event.GetLabel();

	// root, separators and unknowns cannot be edited
	if (!data || data->m_type == BUNDLE_NONE || event.IsEditCancelled() || label.empty()) {
		event.Veto();
		return;
	}

	PListDict infoDict = GetEditablePlist();
	PListDict menuDict;
	if (!infoDict.GetDict("mainMenu", menuDict)) {
		wxFAIL_MSG(wxT("No mainMenu in info.plist"));
		return;
	}

	if (data->m_type == BUNDLE_SUBDIR) {
		PListDict submenuDict;
		if (!menuDict.GetDict("submenus", submenuDict)) {
			wxFAIL_MSG(wxT("No submenus in info.plist"));
			return;
		}

		PListDict itemDict;
		if (!submenuDict.GetDict(data->m_uuid.mb_str(wxConvUTF8), itemDict)) {
			wxFAIL_MSG(wxT("invalid submenu in info.plist"));
			return;
		}
		itemDict.wxSetString("name", label);
		m_isModified = true;
	}
	else if (data->m_type == BUNDLE_COMMAND) {
		PListDict cmdDict = m_plistHandler.GetEditable(BUNDLE_COMMAND, m_bundleId, data->m_itemId);
		cmdDict.wxSetString("name", label);
		m_plistHandler.Save(BUNDLE_COMMAND, m_bundleId, data->m_itemId);
		m_itemsModified = true;
	}
	else if (data->m_type == BUNDLE_SNIPPET) {
		PListDict snipDict = m_plistHandler.GetEditable(BUNDLE_SNIPPET, m_bundleId, data->m_itemId);
		snipDict.wxSetString("name", label);
		m_plistHandler.Save(BUNDLE_SNIPPET, m_bundleId, data->m_itemId);
		m_itemsModified = true;
	}
	else {
		wxFAIL_MSG(wxT("Invalid type"));
	}

	// Update the main bundle tree
	m_parentDlg.UpdateItemName(data->m_type, m_bundleId, data->m_itemId, label);
}

void BundleEditor::MenuPanel::OnTreeItemBeginDrag(wxTreeEvent& event) {
	// The root item cannot be dragged
	if (event.GetItem() == m_menuTree->GetRootItem()) {
		return;
	}

	m_draggedItem = event.GetItem();

	// We have to explicitly allow dragging
	event.Allow();
}

void BundleEditor::MenuPanel::OnTreeItemEndDrag(wxTreeEvent& event) {
	if (!m_draggedItem.IsOk()) return;

	wxTreeItemId itemSrc = m_draggedItem;
    wxTreeItemId itemDst = event.GetItem();
	m_draggedItem = wxTreeItemId(); // invalidate
	if (itemSrc == itemDst) return;
	if (itemDst.IsOk() && IsParentOf(itemSrc, itemDst)) return; // cannot drag into self

	// Get source
	const MenuItemData* data = (MenuItemData*)m_menuTree->GetItemData(itemSrc);

	wxString subUuid;
	wxTreeItemId insertedItem;
	if (data) {
		if (data->m_type == BUNDLE_SUBDIR) subUuid = data->m_uuid;

		const wxString name = m_menuTree->GetItemText(itemSrc);
		const unsigned int imageId = m_menuTree->GetItemImage(itemSrc);

		insertedItem = InsertItem(itemDst, name, data->m_uuid.mb_str(wxConvUTF8), imageId, new MenuItemData(*data));
	}
	else InsertItem(itemDst, wxT("---- separator ----"), "------------------------------------", 5);

	// Delete source ref
	const wxTreeItemId parent = m_menuTree->GetItemParent(itemSrc);
	RemoveItem(itemSrc, false);

	// Check if we have to rebuild submenu
	if (!subUuid.empty()) {
		const PListDict infoDict = m_plistHandler.GetBundleInfo(m_bundleId);
		PListDict menuDict;
		if (!infoDict.GetDict("mainMenu", menuDict)) return;
		PListDict submenuDict;
		if (!menuDict.GetDict("submenus", submenuDict)) return;
		PListDict subDict;
		if (!submenuDict.GetDict(subUuid.mb_str(wxConvUTF8), subDict)) return;
		PListArray subArray;
		if (!subDict.GetArray("items", subArray)) return;

		// Parse the menu
		ParseMenu(subArray, submenuDict, insertedItem);
	}
}

void BundleEditor::MenuPanel::OnRemoveButton(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId item = m_menuTree->GetSelection();
	if (!item.IsOk()) return;
	if (item == m_menuTree->GetRootItem()) return;

	RemoveItem(item, true);
}

void BundleEditor::MenuPanel::OnSeparatorButton(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId item = m_menuTree->GetSelection();
	InsertItem(item, wxT("---- separator ----"), "------------------------------------", 5);
}

void BundleEditor::MenuPanel::OnSubDirButton(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId item = m_menuTree->GetSelection();

	// Get a new uuid
	const wxString newUuid = PListHandler::GetNewUuid();
	const wxCharBuffer uuidBuf = newUuid.mb_str(wxConvUTF8);
	const char* uuid = uuidBuf.data();

	// Create the new group
	PListDict infoDict = GetEditablePlist();
	PListDict menuDict;
	if (!infoDict.GetDict("mainMenu", menuDict)) {
		wxFAIL_MSG(wxT("No mainMenu in info.plist"));
		return;
	}
	PListDict submenuDict = menuDict.NewDict("submenus");
	PListDict subDict = submenuDict.NewDict(uuid);
	subDict.NewArray("items");
	subDict.SetString("name", "New Group");

	// Insert ref to new group
	const wxTreeItemId newItem = InsertItem(item, wxT("New Group"), uuid, 3, new MenuItemData(BUNDLE_SUBDIR, 0, newUuid));
	if (newItem.IsOk()) m_menuTree->EditLabel(newItem);
}

wxTreeItemId BundleEditor::MenuPanel::InsertItem(const wxTreeItemId& item, const wxString& name, const char* uuid, unsigned int imageId, MenuItemData* menuData) {
	PListDict infoDict = GetEditablePlist();
	PListDict menuDict;
	if (!infoDict.GetDict("mainMenu", menuDict)) {
		wxFAIL_MSG(wxT("No mainMenu in info.plist"));
		return wxTreeItemId();
	}
	PListArray itemsArray;
	if (!menuDict.GetArray("items", itemsArray)) {
		wxFAIL_MSG(wxT("mainMenu does not contain an item array in info.plist"));
		return wxTreeItemId();
	}

	const wxTreeItemId rootItem = m_menuTree->GetRootItem();

	if (!item.IsOk()) {
		// No selection, just add to end of root
		const wxTreeItemId newItem = m_menuTree->AppendItem(rootItem, name, imageId, -1, menuData);
		m_menuTree->EnsureVisible(newItem);
		m_menuTree->SelectItem(newItem);
		itemsArray.AddString(uuid);

		return newItem;
	}
	else {
		unsigned int insPos = 0;
		wxTreeItemId parent;

		// Get parent and index
		const MenuItemData* data = (MenuItemData*)m_menuTree->GetItemData(item);
		if (item == rootItem || (data && data->m_type == BUNDLE_SUBDIR)) {
			parent = item;
		}
		else {
			parent = m_menuTree->GetItemParent(item);
			data = (MenuItemData*)m_menuTree->GetItemData(parent);

			// Count to get index of selected item
			wxTreeItemIdValue cookie;
			wxTreeItemId child = m_menuTree->GetFirstChild(parent, cookie);
			while (child.IsOk() && child != item) {
				++insPos;
				child = m_menuTree->GetNextChild(parent, cookie);
			}
			if (child != item) {
				wxFAIL_MSG(wxT("item not in tree"));
				return wxTreeItemId();
			}
			++insPos; // after selected item
		}

		// Insert in treeCtrl
		const wxTreeItemId newItem = m_menuTree->InsertItem(parent, insPos, name, imageId, -1, menuData);
		m_menuTree->EnsureVisible(newItem);
		m_menuTree->SelectItem(newItem);

		// Insert in plist
		if (parent == rootItem) {
			itemsArray.InsertString(insPos, uuid);
		}
		else {
			PListDict submenuDict;
			if (!menuDict.GetDict("submenus", submenuDict)) {
				wxFAIL_MSG(wxT("No submenus in info.plist"));
				return wxTreeItemId();
			}

			PListDict itemDict;
			if (!submenuDict.GetDict(data->m_uuid.mb_str(wxConvUTF8), itemDict)) {
				wxFAIL_MSG(wxT("invalid submenu in info.plist"));
				return wxTreeItemId();
			}

			PListArray subArray;
			if (itemDict.GetArray("items", subArray)) {
				subArray.InsertString(insPos, uuid);
			}
			else {
				wxFAIL_MSG(wxT("no items array in submenu"));
				return wxTreeItemId();
			}
		}

		return newItem;
	}
}

bool BundleEditor::MenuPanel::DnDHitTest(const wxPoint& point) {
	const wxPoint treePoint = m_menuTree->ScreenToClient(point);

	int resultFlag;
	const wxTreeItemId item = m_menuTree->HitTest(treePoint, resultFlag);

	if (item.IsOk()) {
		if (item != m_dropItem) {
			if (m_dropItem.IsOk()) m_menuTree->SetItemDropHighlight(m_dropItem, false);
			m_menuTree->SetItemDropHighlight(item);
			m_dropItem = item;

			m_menuTree->Update();
		}
		return true;
	}
	else {
		if (m_dropItem.IsOk()) {
			m_menuTree->SetItemDropHighlight(m_dropItem, false);
			m_dropItem = wxTreeItemId(); // invalidate
			m_menuTree->Update();
		}

		return (resultFlag == wxTREE_HITTEST_NOWHERE); // in ctrl, but outside items
	}
}

bool BundleEditor::MenuPanel::DoDrop(const wxPoint& point, BundleItemType type, unsigned int itemId) {
	// Clear drop highlighting
	if (m_dropItem.IsOk()) {
		m_menuTree->SetItemDropHighlight(m_dropItem, false);
		m_dropItem = wxTreeItemId(); // invalidate
	}

	if (IsInMenu(type, itemId)) return false;
	const wxPoint treePoint = m_menuTree->ScreenToClient(point);

	int resultFlag;
	const wxTreeItemId item = m_menuTree->HitTest(treePoint, resultFlag);
	if (!item.IsOk() && resultFlag != wxTREE_HITTEST_NOWHERE) return false;

	// Insert the item
	wxString uuidStr;
	if (type == BUNDLE_COMMAND) {
		const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, m_bundleId, itemId);
		const wxString name = cmdDict.wxGetString("name");
		const char* uuid = cmdDict.GetString("uuid");
		uuidStr = wxString(uuid, wxConvUTF8);
		InsertItem(item, name, uuid, 1, new MenuItemData(BUNDLE_COMMAND, itemId, uuidStr));
	}
	else if (type == BUNDLE_SNIPPET) {
		const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, m_bundleId, itemId);
		const wxString name = snipDict.wxGetString("name");
		const char* uuid = snipDict.GetString("uuid");
		uuidStr = wxString(uuid, wxConvUTF8);
		InsertItem(item, name, uuid, 2, new MenuItemData(BUNDLE_SNIPPET, itemId, uuidStr));
	}
	else return false;

	// Mark that the item have been added to the menu
	if (uuidStr) {
		map<wxString, BundleAction>::iterator s = m_actionMap.find(uuidStr);
		if (s != m_actionMap.end()) {
			s->second.SetInMenu(true);
		}
	}

	return true;
}

bool BundleEditor::MenuPanel::IsInMenu(BundleItemType type, unsigned int itemId) const {
	wxString uuid;
	if (type == BUNDLE_COMMAND) {
		const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, m_bundleId, itemId);
		uuid = cmdDict.wxGetString("uuid");
	}
	else if (type == BUNDLE_SNIPPET) {
		const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, m_bundleId, itemId);
		uuid = snipDict.wxGetString("uuid");
	}
	if (uuid.empty()) return false;

	// Check if item is in menu
	map<wxString, BundleAction>::const_iterator s = m_actionMap.find(uuid);
	if (s != m_actionMap.end()) {
		return s->second.m_inMenu;
	}
	else return false;
}

bool BundleEditor::MenuPanel::IsParentOf(const wxTreeItemId parent, const wxTreeItemId child) const {
	wxASSERT(parent.IsOk() && child.IsOk());

	wxTreeItemId item = m_menuTree->GetItemParent(child);

	while (item.IsOk()) {
		if (item == parent) return true;
		item = m_menuTree->GetItemParent(item);
	}

	return false;
}

// ----- MenuItemData -----------------------------------------------

BundleEditor::MenuPanel::MenuItemData::MenuItemData(BundleItemType type, unsigned int itemId, const wxString& uuid)
: m_type(type), m_uuid(uuid), m_itemId(itemId) {
}
