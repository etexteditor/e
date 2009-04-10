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

#include "BundlePane.h"
#include "EditorFrame.h"
#include "eApp.h"

#include "images/tmBundle.xpm"
#include "images/tmCommand.xpm"
#include "images/tmSnippet.xpm"
#include "images/tmDragCmd.xpm"
#include "images/tmPrefs.xpm"
#include "images/tmLanguage.xpm"
#include "images/tmSubMenu.xpm"
#include "images/tmUnknown.xpm"
#include "images/tmSeparator.xpm"

enum {
	CTRL_BUNDLETREE,
	CTRL_NEWBUNDLEITEM,
	CTRL_DELBUNDLE,
	CTRL_MANAGE,

	MENU_NEW_BUNDLE,
	MENU_NEW_COMMAND,
	MENU_NEW_SNIPPET,
	MENU_NEW_DRAGCMD,
	MENU_NEW_PREF,
	MENU_NEW_LANGUAGE,
	MENU_NEW_SEPARATOR,
	MENU_NEW_SUBMENU,
	MENU_DELETE,
	MENU_EXPORT,
	MENU_OPEN_ITEM
};

BEGIN_EVENT_TABLE(BundlePane, wxPanel)
	EVT_TREE_ITEM_ACTIVATED(CTRL_BUNDLETREE, BundlePane::OnTreeItemActivated)
	EVT_TREE_SEL_CHANGED(CTRL_BUNDLETREE, BundlePane::OnTreeItemSelected)
	EVT_TREE_BEGIN_DRAG(CTRL_BUNDLETREE, BundlePane::OnTreeBeginDrag)
	EVT_TREE_END_DRAG(CTRL_BUNDLETREE, BundlePane::OnTreeEndDrag)
	EVT_TREE_KEY_DOWN(CTRL_BUNDLETREE, BundlePane::OnTreeKeyDown)
	EVT_TREE_ITEM_MENU(CTRL_BUNDLETREE, BundlePane::OnTreeMenu)
	EVT_BUTTON(CTRL_NEWBUNDLEITEM, BundlePane::OnButtonPlus)
	EVT_BUTTON(CTRL_DELBUNDLE, BundlePane::OnButtonMinus)
	EVT_BUTTON(CTRL_MANAGE, BundlePane::OnButtonManage)
	EVT_MENU(MENU_NEW_BUNDLE, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_COMMAND, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_SNIPPET, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_DRAGCMD, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_PREF, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_LANGUAGE, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_SEPARATOR, BundlePane::OnMenuNew)
	EVT_MENU(MENU_NEW_SUBMENU, BundlePane::OnMenuNew)
	EVT_MENU(MENU_DELETE, BundlePane::OnButtonMinus)
	EVT_MENU(MENU_EXPORT, BundlePane::OnMenuExport)
	EVT_MENU(MENU_OPEN_ITEM, BundlePane::OnMenuOpenItem)
	EVT_IDLE(BundlePane::OnIdle)
END_EVENT_TABLE()

// Initialize statics
const char* BundlePane::s_snippetsyntax = "Syntax Summary:\n"
"\n"
"   Variables        $TM_FILENAME, $TM_SELECTED_TEXT\n"
"   Fallback Values  ${TM_SELECTED_TEXT:$TM_CURRENT_WORD}\n"
"   Substitutions    ${TM_FILENAME/.*/\\U$0/}\n"
"\n"
"   Tab Stops        $1, $2, $3, ... $0 (optional)\n"
"   Placeholders     ${1:default value}\n"
"   Mirrors          <${2:tag}>...</$2>\n"
"   Transformations  <${3:tag}>...</${3/(\\w*).*/\\U$1/}>\n"
"   Pipes            ${1:ruby code|ruby -e \"print eval STDIN.read\"}\n"
"\n"
"   Shell Code       `date`, `pwd`\n"
"\n"
"   Escape Codes     \\$ \\` \\\\";

const char* BundlePane::s_commandsyntax = "# just to remind you of some useful environment variables\n"
"\n"
"echo File: \"$TM_FILEPATH\"\n"
"echo Word: \"$TM_CURRENT_WORD\"\n"
"echo Selection: \"$TM_SELECTED_TEXT\"";

BundlePane::BundlePane(EditorFrame& parent)
: wxPanel(&parent, wxID_ANY, wxPoint(-100,-100)), m_parentFrame(parent), m_imageList(16,16),
  m_syntaxHandler(((eApp*)wxTheApp)->GetSyntaxHandler()), m_plistHandler(m_syntaxHandler.GetPListHandler()) {
	// Build Imagelist
	m_imageList.Add(wxIcon(tmbundle_xpm));
	m_imageList.Add(wxIcon(tmcommand_xpm));
	m_imageList.Add(wxIcon(tmsnippet_xpm));
	m_imageList.Add(wxIcon(tmdragcmd_xpm));
	m_imageList.Add(wxIcon(tmprefs_xpm));
	m_imageList.Add(wxIcon(tmlanguage_xpm));
	m_imageList.Add(wxIcon(tmsubmenu_xpm));
	m_imageList.Add(wxIcon(tmunknown_xpm));
	m_imageList.Add(wxIcon(tmseparator_xpm));

	// Create the controls
	m_bundleTree = new SortTreeCtrl(this, CTRL_BUNDLETREE, wxTR_LINES_AT_ROOT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_EDIT_LABELS);
	m_bundleTree->SetMinSize(wxSize(50,50)); // ensure resizeability
	m_bundlePlus = new wxButton(this, CTRL_NEWBUNDLEITEM, wxT("+"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_bundleMinus = new wxButton(this, CTRL_DELBUNDLE, wxT("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	wxButton* bundleManage = new wxButton(this, CTRL_MANAGE, wxT("Manage.."));
	m_bundleTree->SetImageList(&m_imageList);

	// Set tooltips
	m_bundlePlus->SetToolTip(_("Add new item"));
	m_bundleMinus->SetToolTip(_("Delete item"));
	m_bundleMinus->Disable();

	// Create the layout
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_bundleTree, 1, wxEXPAND);
		wxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
			buttonSizer->Add(m_bundlePlus, 0, wxRIGHT, 2);
			buttonSizer->Add(m_bundleMinus, 0, wxRIGHT, 2);
			buttonSizer->AddStretchSpacer(1);
			buttonSizer->Add(bundleManage, 0, wxALIGN_RIGHT);
			mainSizer->Add(buttonSizer, 0, wxEXPAND|wxALL, 2);

	SetSizer(mainSizer);

	// Bundles will be loaded in idle time
}

void BundlePane::LoadBundles() {
	// Add an empty root node (will not be shown)
	m_bundleTree->DeleteAllItems();
	const wxTreeItemId rootItem = m_bundleTree->AddRoot(wxEmptyString);

	const vector<unsigned int> bundles = m_plistHandler.GetBundles();
	for (unsigned int i = 0; i < bundles.size(); ++i) {
		const unsigned int bundleId = bundles[i];

		// Add Bundle Root
		const PListDict infoDict = m_plistHandler.GetBundleInfo(bundleId);
		const wxString bundleName = infoDict.wxGetString("name");
		const wxTreeItemId bundleItem = m_bundleTree->AppendItem(rootItem, bundleName, 0, -1, new BundleItemData(bundleId));

		// Add the menu
		const wxTreeItemId menuItem = m_bundleTree->AppendItem(bundleItem, _("Menu"), 6, -1, new BundleItemData(BUNDLE_MENU, bundleId));
		map<wxString, const BundleItemData*> actionMap;

		// Add commands
		const vector<unsigned int> commands =  m_plistHandler.GetList(BUNDLE_COMMAND, bundleId);
		for (unsigned int c = 0; c < commands.size(); ++c) {
			const unsigned int commandId = commands[c];

			const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, bundleId, commandId);
			const wxString cmdName = cmdDict.wxGetString("name");
			const wxString uuid = cmdDict.wxGetString("uuid");

			BundleItemData* itemData = new BundleItemData(BUNDLE_COMMAND, bundleId, commandId, uuid);
			actionMap[uuid] = itemData;

			m_bundleTree->AppendItem(bundleItem, cmdName, 1, -1, itemData);
		}

		// Add snippets
		const vector<unsigned int> snippets =  m_plistHandler.GetList(BUNDLE_SNIPPET, bundleId);
		for (unsigned int n = 0; n < snippets.size(); ++n) {
			const unsigned int snippetId = snippets[n];

			const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, bundleId, snippetId);
			const wxString snipName = snipDict.wxGetString("name");
			const wxString uuid = snipDict.wxGetString("uuid");

			BundleItemData* itemData = new BundleItemData(BUNDLE_SNIPPET, bundleId, snippetId, uuid);
			actionMap[uuid] = itemData;

			m_bundleTree->AppendItem(bundleItem, snipName, 2, -1, itemData);
		}

		// Add DragCommands
		const vector<unsigned int> dragCommands =  m_plistHandler.GetList(BUNDLE_DRAGCMD, bundleId);
		for (unsigned int d = 0; d < dragCommands.size(); ++d) {
			const unsigned int commandId = dragCommands[d];

			const PListDict cmdDict = m_plistHandler.Get(BUNDLE_DRAGCMD, bundleId, commandId);
			const wxString cmdName = cmdDict.wxGetString("name");

			m_bundleTree->AppendItem(bundleItem, cmdName, 3, -1, new BundleItemData(BUNDLE_DRAGCMD, bundleId, commandId));
		}

		// Add Preferences
		const vector<unsigned int> preferences =  m_plistHandler.GetList(BUNDLE_PREF, bundleId);
		for (unsigned int p = 0; p < preferences.size(); ++p) {
			const unsigned int prefId = preferences[p];

			const PListDict prefDict = m_plistHandler.Get(BUNDLE_PREF, bundleId, prefId);
			const wxString name = prefDict.wxGetString("name");

			m_bundleTree->AppendItem(bundleItem, name, 4, -1, new BundleItemData(BUNDLE_PREF, bundleId, prefId));
		}

		// Add Languages
		const vector<unsigned int> languages =  m_plistHandler.GetList(BUNDLE_LANGUAGE, bundleId);
		for (unsigned int l = 0; l < languages.size(); ++l) {
			const unsigned int langId = languages[l];

			const PListDict langDict = m_plistHandler.Get(BUNDLE_LANGUAGE, bundleId, langId);
			const wxString name = langDict.wxGetString("name");

			m_bundleTree->AppendItem(bundleItem, name, 5, -1, new BundleItemData(BUNDLE_LANGUAGE, bundleId, langId));
		}

		// Check if there is a menu
		PListDict menuDict;
		if (infoDict.GetDict("mainMenu", menuDict)) {
			PListArray itemsArray;
			if (menuDict.GetArray("items", itemsArray)) {
				PListDict submenuDict;
				menuDict.GetDict("submenus", submenuDict);

				// Parse the menu
				ParseMenu(bundleId, itemsArray, submenuDict, menuItem, actionMap);
			}
		}
		else {
			// If there is no main menu structure, we use the ordering
			PListArray itemsArray;
			if (infoDict.GetArray("ordering", itemsArray)) {
				// Parse the menu
				ParseMenu(bundleId, itemsArray, PListDict(), menuItem, actionMap);
			}
		}

		m_bundleTree->SortChildren(bundleItem);
	}

	m_bundleTree->SortChildren(rootItem);
}

void BundlePane::ParseMenu(unsigned int bundleId, const PListArray& itemsArray, const PListDict& submenuDict, const wxTreeItemId& parentItem, const map<wxString, const BundleItemData*>& actionMap) {
	const bool hasSubmenus = submenuDict.IsOk();
	PListDict subDict;

	for (unsigned int i = 0; i < itemsArray.GetSize(); ++i) {
		const char* item = itemsArray.GetString(i);

		if (item[0] == '-') {
			m_bundleTree->AppendItem(parentItem, wxT("---- separator ----"), 8, -1, new BundleItemData(BUNDLE_SEPARATOR, bundleId));
		}
		else {
			// Look for submenu
			if (hasSubmenus && submenuDict.GetDict(item, subDict)) {
				const wxString name = subDict.wxGetString("name");
				const wxString uuid(item, wxConvUTF8);
				const wxTreeItemId subItem = m_bundleTree->AppendItem(parentItem, name, 6, -1, new BundleItemData(BUNDLE_SUBDIR, bundleId, uuid));

				PListArray subArray;
				if (subDict.GetArray("items", subArray)) {
					ParseMenu(bundleId, subArray, submenuDict, subItem, actionMap);
				}

				continue;
			}

			const wxString uuid(item, wxConvUTF8);

			// Look for action
			map<wxString, const BundleItemData*>::const_iterator s = actionMap.find(uuid);
			if (s != actionMap.end()) {
				const BundleItemData& ba = *s->second;

				if (ba.m_type == BUNDLE_COMMAND || ba.m_type == BUNDLE_SNIPPET) {
					const PListDict itemDict = m_plistHandler.Get(ba.m_type, ba.m_bundleId, ba.m_itemId);
					const wxString name = itemDict.wxGetString("name");
					BundleItemData* itemData = new BundleItemData(ba);
					itemData->m_isMenuItem = true;
					const int imageId = (ba.m_type == BUNDLE_COMMAND) ? 1 : 2;
					m_bundleTree->AppendItem(parentItem, name, imageId, -1, itemData);
				}
				continue;
			}

			// Unknown item
			const wxTreeItemId unknownItem = m_bundleTree->AppendItem(parentItem, _("unsupported action"), 7, -1, new BundleItemData(BUNDLE_NONE, bundleId, uuid));
			wxFont font = m_bundleTree->GetItemFont(unknownItem);
			font.SetStyle(wxFONTSTYLE_ITALIC);
			m_bundleTree->SetItemFont(unknownItem, font);
		}
	}
}

void BundlePane::OnTreeItemActivated(wxTreeEvent& WXUNUSED(event)) {
	// Activate event restores focus to treeCtrl so we want to open item
	// in a subsequent event (otherwise it will steal focus from editorCtrl)
	wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, MENU_OPEN_ITEM);
	AddPendingEvent(openEvent);
}

void BundlePane::OnMenuOpenItem(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	if (!selItem.IsOk()) return;

	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);
	if (!data || data->m_type == BUNDLE_NONE || data->m_type == BUNDLE_BUNDLE || data->m_type == BUNDLE_SUBDIR || data->m_type == BUNDLE_MENU) return;

	const wxString bundlePath = m_plistHandler.GetBundleItemUri(data->m_type, data->m_bundleId, data->m_itemId);
	if (bundlePath.empty()) return;

	m_parentFrame.Open(bundlePath);
}


void BundlePane::OnTreeItemSelected(wxTreeEvent& event) {
	const wxTreeItemId selItem = event.GetItem();
	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);

	// The menu root cannot be removed
	if (data->m_type == BUNDLE_MENU) m_bundleMinus->Disable();
	else m_bundleMinus->Enable();
}

void BundlePane::OnTreeBeginDrag(wxTreeEvent& event) {
	wxTreeItemId id = event.GetItem();
	if (!id.IsOk()) {
		wxLogDebug(wxT("Starting Drag: invalid item"));
		return;
	}
    const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData( id );
    
	if (!data || data->m_type == BUNDLE_BUNDLE) {
		wxLogDebug(wxT("Starting Drag: bundle or root menu"));
		return; // bundles and menu roots cannot be dragged
	}

	m_draggedItem = id;

	wxLogDebug(wxT("Starting Drag of item: %s"), m_bundleTree->GetItemText(id).c_str());

	// Need to explicitly allow drag
	event.Allow();
}


void BundlePane::OnTreeEndDrag(wxTreeEvent& event) {
	if (!m_draggedItem.IsOk()) return;

	const wxTreeItemId itemSrc = m_draggedItem;
	const wxTreeItemId itemDst = event.GetItem();
	m_draggedItem = wxTreeItemId();

	if (!itemDst.IsOk()) return; // Invalid destintion
	if (itemSrc == itemDst) return; // Can't drag to self
	if (IsTreeItemParentOf(itemSrc, itemDst)) return; // Can't drag to one of your own children

	wxLogDebug(wxT("Ending Drag over item: %s"), m_bundleTree->GetItemText(itemDst).c_str());

	const BundleItemData* srcData = (BundleItemData*)m_bundleTree->GetItemData(itemSrc);
	const BundleItemData* dstData = (BundleItemData*)m_bundleTree->GetItemData(itemDst);

	if (!dstData->IsMenuItem()) return; // You can only drag items to menu
	if (dstData->m_bundleId != srcData->m_bundleId) return; // Items can only be dragged within same bundle

	// We have to cache uuid of submenus
	const wxString subUuid = (srcData->m_type == BUNDLE_SUBDIR) ? srcData->m_uuid : wxString(wxEmptyString);

	const unsigned int bundleId = srcData->m_bundleId;
	PListDict infoDict = GetEditableMenuPlist(bundleId);
	
	// Insert the item
	Freeze();
	const wxString name = m_bundleTree->GetItemText(itemSrc);
	const wxTreeItemId insertedItem = InsertMenuItem(itemDst, name, new BundleItemData(*srcData), infoDict);

	if (srcData->m_type == BUNDLE_SUBDIR) {
		CopySubItems(itemSrc, insertedItem);
	}

	// Delete source ref
	if (srcData->IsMenuItem()) RemoveMenuItem(itemSrc, false, infoDict);
	Thaw();

	// Save the modified plist
	m_plistHandler.SaveBundle(bundleId);

	// Update menu in editorFrame
	m_syntaxHandler.ReParseBundles(true/*onlyMenu*/);
}

void BundlePane::OnTreeKeyDown(wxTreeEvent& event) {
	const int key = event.GetKeyCode();
	if (key == WXK_DELETE || key == WXK_BACK) {
		DeleteItem();
	}
}

bool BundlePane::IsTreeItemParentOf(const wxTreeItemId parent, const wxTreeItemId child) const {
	wxASSERT(parent.IsOk() && child.IsOk());

	wxTreeItemId item = m_bundleTree->GetItemParent(child);

	while (item.IsOk()) {
		if (item == parent) return true;
		item = m_bundleTree->GetItemParent(item);
	}

	return false;
}

unsigned int BundlePane::GetTreeItemIndex(const wxTreeItemId parent, const wxTreeItemId child) const {
	// Count to get index of item
	unsigned int ndx = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId c = m_bundleTree->GetFirstChild(parent, cookie);
	while (c.IsOk() && c != child) {
		++ndx;
		c = m_bundleTree->GetNextChild(parent, cookie);
	}
	wxASSERT(child == c);

	return ndx;
}

void BundlePane::OnIdle(wxIdleEvent& event) {
	if (!m_bundleTree->IsEmpty()) return; // already updated
	
	// Keep waiting until all bundles are updated
	if (!m_plistHandler.AllBundlesUpdated()) {
		event.RequestMore();
		return;
	}

	LoadBundles();
}

void BundlePane::OnTreeMenu(wxTreeEvent& event) {
	const wxTreeItemId item = event.GetItem();
	if (item.IsOk()) m_bundleTree->SelectItem(item);
	
	const bool inBundle = item.IsOk();
	bool inMenu = false;
	if (item.IsOk()) {
		const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(item);
		if (data->IsMenuItem()) inMenu = true;
	}

	// Create popup menu
	wxMenu popupMenu;
	popupMenu.Append(MENU_NEW_BUNDLE,   _("New &Bundle"));
	if (inBundle) {
		popupMenu.AppendSeparator();
		popupMenu.Append(MENU_NEW_COMMAND,  _("New &Command"));
		popupMenu.Append(MENU_NEW_SNIPPET,  _("New &Snippet"));
		popupMenu.Append(MENU_NEW_DRAGCMD,  _("New D&ragCommand"));
		popupMenu.Append(MENU_NEW_PREF,     _("New &Preference"));
		popupMenu.Append(MENU_NEW_LANGUAGE, _("New &Language"));
	}
	if (inMenu) {
		popupMenu.AppendSeparator();
		popupMenu.Append(MENU_NEW_SUBMENU,   _("New Sub&Menu"));
		popupMenu.Append(MENU_NEW_SEPARATOR, _("New S&eparator"));
	}

	if (inBundle && !inMenu) {
		popupMenu.AppendSeparator();
		popupMenu.Append(MENU_DELETE, _("&Delete"));
		popupMenu.Append(MENU_EXPORT, _("E&xport.."));
	}

	PopupMenu(&popupMenu);
}

void BundlePane::OnButtonPlus(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	const bool inBundle = selItem.IsOk();
	
	bool inMenu = false;
	if (selItem.IsOk()) {
		const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);
		if (!data || data->IsMenuItem()) inMenu = true;
	}

	const wxBitmap bundleBmp(tmbundle_xpm);
	const wxBitmap commandBmp(tmcommand_xpm);
	const wxBitmap snippetBmp(tmsnippet_xpm);
	const wxBitmap dragcmdBmp(tmdragcmd_xpm);
	const wxBitmap prefsBmp(tmprefs_xpm);
	const wxBitmap languageBmp(tmlanguage_xpm);
	const wxBitmap submenuBmp(tmsubmenu_xpm);
	const wxBitmap separatorBmp(tmseparator_xpm);

	// Create popup menu
	wxMenu popupMenu;
	popupMenu.Append(MENU_NEW_BUNDLE,   _("New &Bundle"))->SetBitmap(bundleBmp);
	if (inBundle) {
		popupMenu.AppendSeparator();
		popupMenu.Append(MENU_NEW_COMMAND,  _("New &Command"))->SetBitmap(commandBmp);
		popupMenu.Append(MENU_NEW_SNIPPET,  _("New &Snippet"))->SetBitmap(snippetBmp);
		popupMenu.Append(MENU_NEW_DRAGCMD,  _("New &DragCommand"))->SetBitmap(dragcmdBmp);
		popupMenu.Append(MENU_NEW_PREF,     _("New &Preference"))->SetBitmap(prefsBmp);
		popupMenu.Append(MENU_NEW_LANGUAGE, _("New &Language"))->SetBitmap(languageBmp);
	}
	if (inMenu) {
		popupMenu.AppendSeparator();
		popupMenu.Append(MENU_NEW_SUBMENU,   _("New Sub&Menu"))->SetBitmap(submenuBmp);
		popupMenu.Append(MENU_NEW_SEPARATOR, _("New S&eparator"))->SetBitmap(separatorBmp);
	}

	// Calc menu position
	wxPoint popupPos = m_bundlePlus->GetPosition();
	popupPos.y += m_bundlePlus->GetSize().y;

	PopupMenu(&popupMenu, popupPos);
};

void BundlePane::OnMenuExport(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	if (!selItem.IsOk()) return;
	
	// Get the item name
	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);
	wxString name;
	if (data->IsBundle()) {
		const wxFileName bundlePath = m_plistHandler.GetBundlePath(data->m_bundleId);
		name = bundlePath.GetDirs().Last();
	}
	else {
		const wxFileName bundlePath = m_plistHandler.GetBundleItemPath(data->m_type, data->m_bundleId, data->m_itemId);
		name = bundlePath.GetFullName();
	}

	// Get destination path from user
	const wxString msg = data->IsBundle() ? _("Export Bundle as..") : _("Export Bundle Item as");
	wxFileDialog dlg(this, msg, wxEmptyString, name, wxFileSelectorDefaultWildcardStr, wxFD_SAVE);
	if (dlg.ShowModal() != wxID_OK) return;
	const wxString path = dlg.GetPath();
	if (path.empty()) return;

	// Export the item
	if (data->IsBundle()) {
		wxFileName dstPath;
		dstPath.AssignDir(path);
		m_plistHandler.ExportBundle(dstPath, data->m_bundleId);
	}
	else m_plistHandler.ExportBundleItem(path, data->m_type, data->m_bundleId, data->m_itemId);
}

void BundlePane::OnMenuNew(wxCommandEvent& event) {
	const int menuId = event.GetId();

	switch (menuId) {
		case MENU_NEW_BUNDLE:
			{
				// Get name from user
				const wxString name = wxGetTextFromUser(_("Name of new item:"), _("New Item"), _("New Bundle"), this);
				if (name.empty()) return; // user cancel

				// Create a new  bundle item
				const unsigned int bundleId = m_plistHandler.NewBundle(name);
				m_plistHandler.SaveBundle(bundleId);

				// Add to tree
				const wxTreeItemId rootItem = m_bundleTree->GetRootItem();
				const wxTreeItemId bundleItem = m_bundleTree->AppendItem(rootItem, name, 0, -1, new BundleItemData(bundleId));
				const wxTreeItemId menuItem = m_bundleTree->AppendItem(bundleItem, _("Menu"), 6, -1, new BundleItemData(BUNDLE_MENU, bundleId));
				m_bundleTree->SortChildren(rootItem);
				m_bundleTree->SelectItem(bundleItem);
			}
			break;
		case MENU_NEW_COMMAND:
			NewItem(BUNDLE_COMMAND);
			break;
		case MENU_NEW_SNIPPET:
			NewItem(BUNDLE_SNIPPET);
			break;
		case MENU_NEW_DRAGCMD:
			NewItem(BUNDLE_DRAGCMD);
			break;
		case MENU_NEW_PREF:
			NewItem(BUNDLE_PREF);
			break;
		case MENU_NEW_LANGUAGE:
			NewItem(BUNDLE_LANGUAGE);
			break;
		case MENU_NEW_SEPARATOR:
			{
				const wxTreeItemId selItem = m_bundleTree->GetSelection();
				const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);

				PListDict infoDict = GetEditableMenuPlist(data->m_bundleId);
				InsertMenuItem(selItem, wxT("---- separator ----"), new BundleItemData(BUNDLE_SEPARATOR, data->m_bundleId), infoDict);
				m_plistHandler.SaveBundle(data->m_bundleId);

				// Update menu in editorFrame
				m_syntaxHandler.ReParseBundles(true/*onlyMenu*/);
			}
			break;
		case MENU_NEW_SUBMENU:
			{
				// Get name from user
				const wxString name = wxGetTextFromUser(_("Name of new item:"), _("New Item"), _("New SubMenu"), this);
				if (name.empty()) return; // user cancel

				const wxTreeItemId selItem = m_bundleTree->GetSelection();
				const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);
				
				// Get a new uuid
				const wxString newUuid = PListHandler::GetNewUuid();
				const wxCharBuffer uuidBuf = newUuid.ToUTF8();
				const char* uuid = uuidBuf.data();

				// Create the new group
				PListDict infoDict = GetEditableMenuPlist(data->m_bundleId);
				PListDict menuDict;
				if (!infoDict.GetDict("mainMenu", menuDict)) {
					wxFAIL_MSG(wxT("No mainMenu in info.plist"));
					return;
				}
				PListDict submenuDict = menuDict.NewDict("submenus");
				PListDict subDict = submenuDict.NewDict(uuid);
				subDict.NewArray("items");
				subDict.wxSetString("name", name);

				// Insert in menu
				InsertMenuItem(selItem, name, new BundleItemData(BUNDLE_SUBDIR, data->m_bundleId, newUuid), infoDict);
				m_plistHandler.SaveBundle(data->m_bundleId);

				// Update menu in editorFrame
				m_syntaxHandler.ReParseBundles(true/*onlyMenu*/);
			}
			break;
		default: wxASSERT(false);
	}
};

void BundlePane::OnButtonManage(wxCommandEvent& WXUNUSED(event)) {
	m_parentFrame.ShowBundleManager();
};

void BundlePane::RemoveMenuItem(const wxTreeItemId item, bool deep, PListDict& infoDict) {
	// Get the parent
	const wxTreeItemId parent = m_bundleTree->GetItemParent(item);
	const unsigned int ndx = GetTreeItemIndex(parent, item);

	// Remove item from treeCtrl
	m_bundleTree->Delete(item);

	// Get uuid of parent
	wxString parentUuid;
	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(parent);
	if (data->m_type != BUNDLE_MENU) parentUuid = data->m_uuid;

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

	if (deep) {
		// If it is a subdir, we have to recursively delete all subdirs
		const char* uuid = itemsArray.GetString(ndx);
		if (submenuDict.IsOk() && submenuDict.HasKey(uuid)) {
			DeleteSubDir(uuid, infoDict);
		}
	}

	// Remove the item
	itemsArray.DeleteItem(ndx);
}

void BundlePane::DeleteSubDir(const char* uuid, PListDict& infoDict) {
	wxASSERT(uuid);

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
				DeleteSubDir(itemId, infoDict);
			}
		}
	}

	submenuDict.DeleteItem(uuid);
}

wxTreeItemId BundlePane::InsertMenuItem(const wxTreeItemId& dstItem, const wxString& name, BundleItemData* menuData, PListDict& infoDict) {
	wxASSERT(dstItem.IsOk());

	// Make sure that the new item is marked as being in a menu
	menuData->m_isMenuItem = true;

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

	// Get parent and index
	wxTreeItemId parent;
	unsigned int insPos = 0;
	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(dstItem);
	if (data->m_type == BUNDLE_MENU || data->m_type == BUNDLE_SUBDIR) {
		parent = dstItem;
	}
	else {
		parent = m_bundleTree->GetItemParent(dstItem);
		data = (BundleItemData*)m_bundleTree->GetItemData(parent);

		// Count to get index of selected item
		insPos = GetTreeItemIndex(parent, dstItem);
		++insPos; // after selected item
	}

	// Insert in treeCtrl
	const wxTreeItemId newItem = m_bundleTree->InsertItem(parent, insPos, name, menuData->GetImageId(), -1, menuData);
	m_bundleTree->EnsureVisible(newItem);
	m_bundleTree->SelectItem(newItem);

	// Insert in plist
	if (data->m_type == BUNDLE_MENU) {
		itemsArray.InsertString(insPos, menuData->m_uuid.ToUTF8());
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
			subArray.InsertString(insPos, menuData->m_uuid.ToUTF8());
		}
		else {
			wxFAIL_MSG(wxT("no items array in submenu"));
			return wxTreeItemId();
		}
	}

	return newItem;
}

PListDict BundlePane::GetEditableMenuPlist(unsigned int bundleId) {
	PListDict infoDict = m_plistHandler.GetEditableBundleInfo(bundleId);

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

				itemsArray.AddString(uuid);
			}
		}
	}

	return infoDict;
}

void BundlePane::CopySubItems(wxTreeItemId srcItem, wxTreeItemId dstItem) {
	unsigned int ndx = 0;
	wxTreeItemIdValue cookie;
	wxTreeItemId c = m_bundleTree->GetFirstChild(srcItem, cookie);
	while (c.IsOk()) {
		const BundleItemData* childData = (BundleItemData*)m_bundleTree->GetItemData(c);
		const wxString subname = m_bundleTree->GetItemText(c);
		
		wxTreeItemId item = m_bundleTree->InsertItem(dstItem, ndx, subname, childData->GetImageId(), -1, new BundleItemData(*childData));

		if (childData->m_type == BUNDLE_SUBDIR) CopySubItems(c, item);

		++ndx;
		c = m_bundleTree->GetNextChild(srcItem, cookie);
	}
}

wxTreeItemId BundlePane::FindMenuItem(BundleItemType type, unsigned int bundleId, unsigned int itemId) const {
	wxASSERT(type == BUNDLE_COMMAND || type == BUNDLE_SNIPPET);

	// Find bundle
	wxTreeItemId rootItem = m_bundleTree->GetRootItem();
	wxTreeItemIdValue cookie;
	wxTreeItemId c = m_bundleTree->GetFirstChild(rootItem, cookie);
	while (c.IsOk()) {
		const BundleItemData* childData = (BundleItemData*)m_bundleTree->GetItemData(c);
		if (childData->m_bundleId == bundleId) break;

		c = m_bundleTree->GetNextChild(rootItem, cookie);
	}
	if (!c.IsOk()) return wxTreeItemId();

	// Menu is always first subitem
	wxTreeItemId menuItem = m_bundleTree->GetFirstChild(c, cookie);
	if (!menuItem.IsOk()) {wxASSERT(false); return wxTreeItemId();}

	// Find menu item
	return FindItemInMenu(menuItem, type, itemId);
}

wxTreeItemId BundlePane::FindItemInMenu(wxTreeItemId menuItem, BundleItemType type, unsigned int itemId) const {
	wxTreeItemIdValue cookie;
	wxTreeItemId c = m_bundleTree->GetFirstChild(menuItem, cookie);
	while (c.IsOk()) {
		const BundleItemData* childData = (BundleItemData*)m_bundleTree->GetItemData(c);
		if (childData->m_type == BUNDLE_SUBDIR) {
			wxTreeItemId res = FindItemInMenu(c, type, itemId);
			if (res.IsOk()) return res;
		}
		else if (childData->m_type == type && childData->m_itemId == itemId) return c;

		c = m_bundleTree->GetNextChild(menuItem, cookie);
	}
	
	return wxTreeItemId(); // Not in menu
}

void BundlePane::OnButtonMinus(wxCommandEvent& WXUNUSED(event)) {
	DeleteItem();
}

void BundlePane::DeleteItem() {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	if (!selItem.IsOk()) return; // Can't delete nothing

	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);
	if (data->m_type == BUNDLE_MENU) return; // can't delete menu root
	const unsigned int bundleId = data->m_bundleId;

	if (data->IsMenuItem()) {
		PListDict infoDict = GetEditableMenuPlist(bundleId);
		RemoveMenuItem(selItem, true, infoDict);
		m_plistHandler.SaveBundle(bundleId);

		// Update menu in editorFrame
		m_syntaxHandler.ReParseBundles(true/*onlyMenu*/);
		return;
	}

	bool needFullReload = false;

	switch (data->m_type) {
	case BUNDLE_BUNDLE:
		{
			const wxString msg = _("Are you sure you wish to delete this Bundle?");

			if (wxMessageBox(msg, _("Deleting Bundle"), wxICON_EXCLAMATION|wxOK|wxCANCEL) == wxOK) {
				m_plistHandler.DeleteBundle(data->m_bundleId);
				needFullReload = true;
			}
			else return;
		}
		break;
	case BUNDLE_COMMAND:
	case BUNDLE_SNIPPET:
		{
			m_plistHandler.Delete(data->m_type, data->m_bundleId, data->m_itemId);

			// Check if it should also be removed from menu
			wxTreeItemId menuItem = FindMenuItem(data->m_type, data->m_bundleId, data->m_itemId);
			if (menuItem.IsOk()) {
				PListDict infoDict = GetEditableMenuPlist(bundleId);
				RemoveMenuItem(menuItem, true, infoDict);
				m_plistHandler.SaveBundle(bundleId);
			}
		}
		break;
	case BUNDLE_DRAGCMD:
		m_plistHandler.Delete(data->m_type, data->m_bundleId, data->m_itemId);
		break;
	case BUNDLE_PREF:
	case BUNDLE_LANGUAGE:
		m_plistHandler.Delete(data->m_type, data->m_bundleId, data->m_itemId);
		needFullReload = true;
		break;
	default:
		wxASSERT(false);
	}

	m_bundleTree->Delete(selItem);

	wxBusyCursor wait; // Show user that we are reloading
	if (needFullReload) {
		// if bundles are deleted or prefs or languages changed,
		// we have to call LoadBundles since all syntaxes will have to be reloaded
		m_syntaxHandler.LoadBundles(TmSyntaxHandler::cxRELOAD);
	}
	else m_syntaxHandler.ReParseBundles();
}

void BundlePane::NewItem(BundleItemType type) {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	if (!selItem.IsOk()) return; // Can't add item if no bundle
	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);

	// Get the parent bundle
	const BundleItemData* bundleData = data;
	wxTreeItemId bundleItem = selItem;
	while (!bundleData->IsBundle()) {
		bundleItem = m_bundleTree->GetItemParent(bundleItem);
		bundleData = (BundleItemData*)m_bundleTree->GetItemData(bundleItem);
	}

	// Get name of new item
	wxString name;
	switch (type) {
	case BUNDLE_COMMAND:  name = _("New Command"); break;
	case BUNDLE_SNIPPET:  name = _("New Snippet"); break;
	case BUNDLE_DRAGCMD:  name = _("New DragCommand"); break;
	case BUNDLE_PREF:     name = _("New Preference"); break;
	case BUNDLE_LANGUAGE: name = _("New Language"); break;
	default: wxASSERT(false);
	}
	name = wxGetTextFromUser(_("Name of new item:"), _("New Item"), name, this);
	if (name.empty()) return; // user cancel

	// Create a new (unsaved) command item
	const unsigned int bundleId = data->m_bundleId;
	const unsigned int itemId = m_plistHandler.New(type, bundleId, name);
	
	// Set default values
	PListDict itemDict = m_plistHandler.GetEditable(type, bundleId, itemId);
	switch (type) {
	case BUNDLE_COMMAND:
		itemDict.SetString("input", "selection");
		itemDict.SetString("output", "replaceSelectedText");
		itemDict.SetString("command", s_commandsyntax);
		break;
	case BUNDLE_SNIPPET:
		itemDict.SetString("tabTrigger", "");
		itemDict.SetString("content", s_snippetsyntax);
		break;
	case BUNDLE_DRAGCMD:
		{
			PListArray extArray = itemDict.NewArray("draggedFileExtensions");
			extArray.AddString("png");
			extArray.AddString("jpg");
			itemDict.SetString("command", "echo \"$TM_DROPPED_FILE\"");
		}
		break;
	case BUNDLE_PREF:
		break;
	case BUNDLE_LANGUAGE:
		break;
	default:
		wxASSERT(false);
	}

	// Save the new item
	m_plistHandler.Save(type, bundleId, itemId);

	// Add to tree
	const wxString uuid = itemDict.wxGetString("uuid");
	BundleItemData* itemData = new BundleItemData(type, bundleId, itemId, uuid);
	const wxTreeItemId treeItem = m_bundleTree->AppendItem(bundleItem, name, itemData->GetImageId(), -1, itemData);
	m_bundleTree->SortChildren(bundleItem);
	m_bundleTree->SelectItem(treeItem);

	// Also add to menu if needed
	if (data->IsMenuItem()) {
		PListDict infoDict = GetEditableMenuPlist(bundleId);
		BundleItemData* menuData = new BundleItemData(*itemData);
		InsertMenuItem(selItem, name, menuData, infoDict);
		m_plistHandler.SaveBundle(bundleId);

		// Update menu in editorFrame
		m_syntaxHandler.ReParseBundles(true/*onlyMenu*/);
	}

	// Open in editor
	const wxString bundlePath = m_plistHandler.GetBundleItemUri(type, bundleId, itemId);
	if (bundlePath.empty()) return;
	m_parentFrame.Open(bundlePath);
}

// ---- SortTreeCtrl ----------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(BundlePane::SortTreeCtrl, wxTreeCtrl)

int BundlePane::SortTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2) {
	const BundleItemData* data1 = (BundleItemData*)GetItemData(item1);
	const BundleItemData* data2 = (BundleItemData*)GetItemData(item2);

	// Menu root is always top
	if (data1->m_type == BUNDLE_MENU) return -1;
	if (data2->m_type == BUNDLE_MENU) return 1;

	// First sort on type
	if (data1->m_type < data2->m_type) return -1;
	if (data1->m_type > data2->m_type) return 1;

	// Then sort on name
	const wxString name1 = GetItemText(item1);
	const wxString name2 = GetItemText(item2);
	return name1.CmpNoCase(name2);
}

