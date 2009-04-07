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
#include "eApp.h"
#include <wx/statline.h>
#include "tm_syntaxhandler.h"
#include <wx/tokenzr.h>
#include "jsonreader.h"

#include "images/tmBundle.xpm"
#include "images/tmCommand.xpm"
#include "images/tmSnippet.xpm"
#include "images/tmDragCmd.xpm"
#include "images/tmPrefs.xpm"
#include "images/tmLanguage.xpm"

// Initialize statics
const char* BundleEditor::s_snippetsyntax = "Syntax Summary:\n"
"\n"
"   Variables        $TM_FILENAME, $TM_SELECTED_TEXT\n"
"   Fallback Values  ${TM_SELECTED_TEXT:$TM_CURRENT_WORD}\n"
"   Substitutions    ${TM_FILENAME/.*/\\U$0/}\n"
"\n"
"   Tab Stops        $1, $2, $3, ... $0 (optional)\n"
"   Placeholders     ${1:default value}\n"
"   Mirrors          <${2:tag}>...</$2>\n"
"   Transformations  <${3:tag}>...</${3/(\\w*).*/\\U$1/}>\n"
"\n"
"   Shell Code       `date`, `pwd`\n"
"\n"
"   Escape Codes     \\$ \\` \\\\";

const char* BundleEditor::s_commandsyntax = "# just to remind you of some useful environment variables\n"
"\n"
"echo File: \"$TM_FILEPATH\"\n"
"echo Word: \"$TM_CURRENT_WORD\"\n"
"echo Selection: \"$TM_SELECTED_TEXT\"";

enum {
	CTRL_BUNDLETREE,
	CTRL_NEWBUNDLEITEM,
	CTRL_DELBUNDLE,
	MENU_NEW_BUNDLE,
	MENU_NEW_COMMAND,
	MENU_NEW_SNIPPET,
	MENU_NEW_DRAGCMD,
	MENU_NEW_PREF,
	MENU_NEW_LANGUAGE
};

BEGIN_EVENT_TABLE(BundleEditor, wxDialog)
	EVT_TREE_SEL_CHANGING(CTRL_BUNDLETREE, BundleEditor::OnTreeItemChanging)
	EVT_TREE_SEL_CHANGED(CTRL_BUNDLETREE, BundleEditor::OnTreeItemSelected)
	EVT_TREE_END_LABEL_EDIT(CTRL_BUNDLETREE, BundleEditor::OnTreeEndLabelEdit)
	EVT_TREE_BEGIN_DRAG(CTRL_BUNDLETREE, BundleEditor::OnTreeBeginDrag)
	EVT_MOTION(BundleEditor::OnMouseMotion)
	EVT_LEFT_UP(BundleEditor::OnMouseLeftUp)
	EVT_CLOSE(BundleEditor::OnClose)
	EVT_BUTTON(CTRL_NEWBUNDLEITEM, BundleEditor::OnNewBundleItem)
	EVT_BUTTON(CTRL_DELBUNDLE, BundleEditor::OnDeleteBundleItem)
	EVT_MENU(MENU_NEW_BUNDLE, BundleEditor::OnNewBundle)
	EVT_MENU(MENU_NEW_COMMAND, BundleEditor::OnNewCommand)
	EVT_MENU(MENU_NEW_SNIPPET, BundleEditor::OnNewSnippet)
	EVT_MENU(MENU_NEW_DRAGCMD, BundleEditor::OnNewDragCommand)
	EVT_MENU(MENU_NEW_PREF, BundleEditor::OnNewPref)
	EVT_MENU(MENU_NEW_LANGUAGE, BundleEditor::OnNewLanguage)
END_EVENT_TABLE()

BundleEditor::BundleEditor(wxWindow *parent)
:  wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
   m_syntaxHandler(((eApp*)wxTheApp)->GetSyntaxHandler()), m_plistHandler(m_syntaxHandler.GetPListHandler()), m_imageList(16,16),
   m_currentPanel(BUNDLE_NONE), m_isModified(false), m_needFullReload(false), m_dragImage(NULL)
 {

	SetTitle (_("Edit Bundles"));

	m_imageList.Add(wxIcon(tmbundle_xpm));
	m_imageList.Add(wxIcon(tmcommand_xpm));
	m_imageList.Add(wxIcon(tmsnippet_xpm));
	m_imageList.Add(wxIcon(tmdragcmd_xpm));
	m_imageList.Add(wxIcon(tmprefs_xpm));
	m_imageList.Add(wxIcon(tmlanguage_xpm));

	// Create the controls
	m_bundleTree = new wxTreeCtrl(this, CTRL_BUNDLETREE, wxDefaultPosition, wxDefaultSize,
		wxTR_LINES_AT_ROOT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_EDIT_LABELS);
	m_bundleTree->SetMinSize(wxSize(50,50)); // ensure resizeability
	m_bundlePlus = new wxButton(this, CTRL_NEWBUNDLEITEM, wxT("+"));
	wxButton* bundleMinus = new wxButton(this, CTRL_DELBUNDLE, wxT("-"));
	m_bundleTree->SetImageList(&m_imageList);

	// Create the panels
	m_bundlePanel = new MenuPanel(*this, m_plistHandler);
	m_snippetPanel = new SnippetPanel(this, m_plistHandler);
	m_commandPanel = new CommandPanel(this, m_plistHandler);
	m_dragPanel = new DragPanel(this, m_plistHandler);
	m_prefPanel = new PrefsPanel(this, m_plistHandler);

	// Create the layout
	wxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
		wxSizer* bundleSizer = new wxBoxSizer(wxVERTICAL);
			bundleSizer->Add(m_bundleTree, 1, wxEXPAND);
			wxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
				buttonSizer->Add(m_bundlePlus, 0, wxRIGHT, 5);
				buttonSizer->Add(bundleMinus);
				bundleSizer->Add(buttonSizer, 0, wxALIGN_LEFT|wxTOP, 5);
			mainSizer->Add(bundleSizer, 1, wxEXPAND|wxALL, 5);
		m_itemBorder = new wxStaticBoxSizer(wxVERTICAL, this, _("No Item Selected"));
			m_itemBorder->Add(m_bundlePanel, 1, wxEXPAND);
			m_itemBorder->Add(m_snippetPanel, 1, wxEXPAND);
			m_itemBorder->Add(m_commandPanel, 1, wxEXPAND);
			m_itemBorder->Add(m_dragPanel, 1, wxEXPAND);
			m_itemBorder->Add(m_prefPanel, 1, wxEXPAND);
		mainSizer->Add(m_itemBorder, 2, wxEXPAND|wxALL, 5);

	// Hide all bundle panes except drag (which is the biggest) for initial sizing
	m_itemBorder->Show(m_bundlePanel, false);
	m_itemBorder->Show(m_snippetPanel, false);
	m_itemBorder->Show(m_commandPanel, false);
	m_itemBorder->Show(m_prefPanel, false);

	LoadBundles();

	SetSizerAndFit(mainSizer);
	SetSize(650, 600);
	Centre();
}

void BundleEditor::LoadBundles() {
	// Add an empty root node (will not be shown)
	m_bundleTree->DeleteAllItems();
	const wxTreeItemId rootItem = m_bundleTree->AddRoot(wxEmptyString);

	const vector<unsigned int> bundles = m_plistHandler.GetBundles();

	for (unsigned int i = 0; i < bundles.size(); ++i) {
		const unsigned int bundleId = bundles[i];

		// Get bundle info
		const PListDict infoDict = m_plistHandler.GetBundleInfo(bundleId);
		const wxString bundleName = infoDict.wxGetString("name");

		const wxTreeItemId bundleItem = m_bundleTree->AppendItem(rootItem, bundleName, 0, -1, new BundleItemData(bundleId));

		// Add commands
		const vector<unsigned int> commands =  m_plistHandler.GetList(BUNDLE_COMMAND, bundleId);
		for (unsigned int c = 0; c < commands.size(); ++c) {
			const unsigned int commandId = commands[c];

			const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, bundleId, commandId);
			const wxString cmdName = cmdDict.wxGetString("name");

			m_bundleTree->AppendItem(bundleItem, cmdName, 1, -1, new BundleItemData(BUNDLE_COMMAND, bundleId, commandId));
		}

		// Add snippets
		const vector<unsigned int> snippets =  m_plistHandler.GetList(BUNDLE_SNIPPET, bundleId);
		for (unsigned int n = 0; n < snippets.size(); ++n) {
			const unsigned int snippetId = snippets[n];

			const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, bundleId, snippetId);
			const wxString snipName = snipDict.wxGetString("name");

			m_bundleTree->AppendItem(bundleItem, snipName, 2, -1, new BundleItemData(BUNDLE_SNIPPET, bundleId, snippetId));
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
	}
}

bool BundleEditor::IsModified() const {
	switch (m_currentPanel) {
	case BUNDLE_BUNDLE:
		return m_bundlePanel->IsModified();
	case BUNDLE_SNIPPET:
		return m_snippetPanel->IsModified();
	case BUNDLE_COMMAND:
		return m_commandPanel->IsModified();
	case BUNDLE_DRAGCMD:
		return m_dragPanel->IsModified();
	case BUNDLE_PREF:
	case BUNDLE_LANGUAGE:
		return m_prefPanel->IsModified();
	default:
		break;
	}

	return false;
}

bool BundleEditor::SaveCurrentPanel() {
	switch (m_currentPanel) {
	case BUNDLE_BUNDLE:
		if (m_bundlePanel->Save()) MarkAsModified();
		break;

	case BUNDLE_SNIPPET:
		if (m_snippetPanel->Save()) MarkAsModified();
		break;

	case BUNDLE_COMMAND:
		if (m_commandPanel->Save()) MarkAsModified();
		break;

	case BUNDLE_DRAGCMD:
		if (m_dragPanel->Save()) MarkAsModified();
		break;

	case BUNDLE_PREF:
	case BUNDLE_LANGUAGE:
		{
			const SaveResult res = m_prefPanel->Save();
			if (res == SAVE_OK) {
				MarkAsModified();
				m_needFullReload = true; // prefs and languages need full reload
			}
			else if (res == SAVE_CANCEL) return false;
		}
		break;

	default:
		break;
	}

	return true;
}

void BundleEditor::UpdatePanelTitle(BundleItemType type, const wxString& itemName) {
	wxString title;

	switch (type) {
	case BUNDLE_BUNDLE:
		title = _("Edit Menu for Bundle \"") + itemName + _("\"");
		break;
	case BUNDLE_COMMAND:
		title = _("Edit Command \"") + itemName + _("\"");
		break;
	case BUNDLE_SNIPPET:
		title = _("Edit Snippet \"") + itemName + _("\"");
		break;
	case BUNDLE_DRAGCMD:
		title = _("Edit Drag Command \"") + itemName + _("\"");
		break;
	case BUNDLE_PREF:
		title = _("Edit Preference \"") + itemName + _("\"");
		break;
	case BUNDLE_LANGUAGE:
		title = _("Edit Language \"") + itemName + _("\"");
		break;
	default:
		title = _("No Item Selected");
	}

	m_itemBorder->GetStaticBox()->SetLabel(title);
}

void BundleEditor::OnTreeItemChanging(wxTreeEvent& event) {
	// Save any changes to previous bundle item
	if (!SaveCurrentPanel()) event.Veto();
}

void BundleEditor::OnTreeItemSelected(wxTreeEvent& WXUNUSED(event)) {
	wxTreeItemId selItem = m_bundleTree->GetSelection();

	// Hide all bundle panes
	m_itemBorder->Show(m_bundlePanel, false);
	m_itemBorder->Show(m_snippetPanel, false);
	m_itemBorder->Show(m_commandPanel, false);
	m_itemBorder->Show(m_dragPanel, false);
	m_itemBorder->Show(m_prefPanel, false);

	if (selItem.IsOk()) {
		const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);

		switch (data->m_type) {
		case BUNDLE_BUNDLE:
			{
				m_bundlePanel->SetBundle(data->m_bundleId);
				m_itemBorder->Show(m_bundlePanel, true);
			}
			break;

		case BUNDLE_SNIPPET:
			{
				m_snippetPanel->SetSnippet(data->m_bundleId, data->m_itemId);
				m_itemBorder->Show(m_snippetPanel, true);
			}
			break;

		case BUNDLE_COMMAND:
			{
				m_commandPanel->SetCommand(data->m_bundleId, data->m_itemId);
				m_itemBorder->Show(m_commandPanel, true);
			}
			break;

		case BUNDLE_DRAGCMD:
			{
				m_dragPanel->SetDragCommand(data->m_bundleId, data->m_itemId);
				m_itemBorder->Show(m_dragPanel, true);
			}
			break;

		case BUNDLE_PREF:
			{
				m_prefPanel->SetPref(data->m_bundleId, data->m_itemId);
				m_itemBorder->Show(m_prefPanel, true);
			}
			break;

		case BUNDLE_LANGUAGE:
			{
				m_prefPanel->SetLanguage(data->m_bundleId, data->m_itemId);
				m_itemBorder->Show(m_prefPanel, true);
			}
			break;

		default:
			break;
		}

		m_currentPanel = data->m_type;
		UpdatePanelTitle(data->m_type, m_bundleTree->GetItemText(selItem));
	}
	else {
		UpdatePanelTitle(BUNDLE_NONE, wxEmptyString);
	}

	m_itemBorder->Layout();
}

void BundleEditor::OnTreeEndLabelEdit(wxTreeEvent& event) {
	// WORKAROUND: We need to keep track of the controls edit status
	// to be able to stop accidental dnd during editing
	//m_isEditing = false;

	wxTreeItemId id = event.GetItem();
    BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData( id );
    wxASSERT( data );

	// No change sometimes give empty label in event
	wxString label = event.GetLabel();
	if (event.IsEditCancelled() || label.empty()) {
		// We still have to save the item if it has just been created
		if (!data->IsSaved()) {
			switch (data->m_type) {
			case BUNDLE_BUNDLE:
				m_plistHandler.SaveBundle(data->m_bundleId);
				break;
			case BUNDLE_COMMAND:
			case BUNDLE_SNIPPET:
			case BUNDLE_DRAGCMD:
			case BUNDLE_PREF:
			case BUNDLE_LANGUAGE:
				m_plistHandler.Save(data->m_type, data->m_bundleId, data->m_itemId);
				break;
			default:
				wxASSERT(false);
			}
			data->MarkAsSaved();
		}

		event.Veto();
		return;
	}

	// Set the new name and save
	switch (data->m_type) {
	case BUNDLE_BUNDLE:
		{
			PListDict bundleDict = m_plistHandler.GetEditableBundleInfo(data->m_bundleId);
			bundleDict.wxSetString("name", label);
			m_plistHandler.SaveBundle(data->m_bundleId);

			if (m_currentPanel == BUNDLE_BUNDLE) m_bundlePanel->UpdateBundleName(label);
		}
		break;
	case BUNDLE_COMMAND:
	case BUNDLE_SNIPPET:
	case BUNDLE_DRAGCMD:
	case BUNDLE_PREF:
	case BUNDLE_LANGUAGE:
		{
			PListDict itemDict = m_plistHandler.GetEditable(data->m_type, data->m_bundleId, data->m_itemId);
			itemDict.wxSetString("name", label);
			m_plistHandler.Save(data->m_type, data->m_bundleId, data->m_itemId);
		}
		break;
	default:
		wxASSERT(false);
	}

	data->MarkAsSaved();
	MarkAsModified();

	UpdatePanelTitle(data->m_type, label);
}

void BundleEditor::OnTreeBeginDrag(wxTreeEvent& event) {
	wxTreeItemId id = event.GetItem();
    BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData( id );
    wxASSERT( data );

	if (data->m_type == BUNDLE_BUNDLE) return; // bundles cannot be dragged

	m_draggedItem = id;
	m_dragImage = new wxDragImage(*m_bundleTree, id);
    m_dragImage->BeginDrag(wxPoint(0,0), this);
	m_dragImage->Show();

	m_bundleTree->SetItemDropHighlight(id, false);
	m_bundleTree->Update();

	// Check if item can be dragged to bundle menu
	m_canDragToBundle = false;
	if (m_currentPanel == BUNDLE_BUNDLE && data->m_bundleId == m_bundlePanel->GetCurrentBundle()) {
		if (data->m_type == BUNDLE_COMMAND || data->m_type == BUNDLE_SNIPPET) {
			if (!m_bundlePanel->IsInMenu(data->m_type, data->m_itemId)) m_canDragToBundle = true;
		}
	}
}

void BundleEditor::OnMouseMotion(wxMouseEvent& event) {
	if (!m_dragImage) return;

	m_dragImage->Hide();

	// HitTest in bundle menu
	if (m_canDragToBundle) {
		const wxPoint screenPoint = ClientToScreen(event.GetPosition());

		m_bundlePanel->DnDHitTest(screenPoint);
	}

	m_dragImage->Move(event.GetPosition());
	m_dragImage->Show();
}

void BundleEditor::OnMouseLeftUp(wxMouseEvent& event) {
	if (!m_dragImage) return;

	// End the drag
	m_dragImage->EndDrag();
	delete m_dragImage;
	m_dragImage = NULL;

	// Check if we can drop in bundle menu
	if (m_canDragToBundle) {
		BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(m_draggedItem);
		const wxPoint screenPoint = ClientToScreen(event.GetPosition());

		m_bundlePanel->DoDrop(screenPoint, data->m_type, data->m_itemId);
	}

	m_draggedItem = wxTreeItemId();
}

void BundleEditor::OnNewBundleItem(wxCommandEvent& WXUNUSED(event)) {
	// Create popup menu
	wxMenu popupMenu;
	popupMenu.Append(MENU_NEW_BUNDLE,   _("New Bundle"));
	popupMenu.AppendSeparator();
	popupMenu.Append(MENU_NEW_COMMAND,  _("New Command"));
	popupMenu.Append(MENU_NEW_SNIPPET,  _("New Snippet"));
	popupMenu.Append(MENU_NEW_DRAGCMD,  _("New DragCommand"));
	popupMenu.Append(MENU_NEW_PREF,     _("New Preference"));
	popupMenu.Append(MENU_NEW_LANGUAGE, _("New Language"));

	// Calc menu position
	wxPoint popupPos = m_bundlePlus->GetPosition();
	popupPos.y += m_bundlePlus->GetSize().y;

	PopupMenu(&popupMenu, popupPos);
}

void BundleEditor::OnDeleteBundleItem(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	if (!selItem.IsOk()) return; // Can't delete nothing

	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);

	switch (data->m_type) {
	case BUNDLE_BUNDLE:
		{
			const wxString msg = _("Are you sure you wish to delete this Bundle?");

			if (wxMessageBox(msg, _("Deleting Bundle"), wxICON_EXCLAMATION|wxOK|wxCANCEL) == wxOK) {
				m_plistHandler.DeleteBundle(data->m_bundleId);
				m_needFullReload = true;
			}
			else return;
		}
		break;
	case BUNDLE_COMMAND:
	case BUNDLE_SNIPPET:
	case BUNDLE_DRAGCMD:
		m_plistHandler.Delete(data->m_type, data->m_bundleId, data->m_itemId);
		break;
	case BUNDLE_PREF:
	case BUNDLE_LANGUAGE:
		m_plistHandler.Delete(data->m_type, data->m_bundleId, data->m_itemId);
		m_needFullReload = true;
		break;
	default:
		wxASSERT(false);
	}

	m_currentPanel = BUNDLE_NONE; // to avoid trying to save the deleted item
	m_bundleTree->Delete(selItem);
	MarkAsModified();
}

void BundleEditor::OnNewBundle(wxCommandEvent& WXUNUSED(event)) {
	const wxTreeItemId rootItem = m_bundleTree->GetRootItem();

	// Create a new (unsaved) bundle item
	const unsigned int bundleId = m_plistHandler.NewBundle(wxT("New Bundle"));
	const wxTreeItemId bundleItem = m_bundleTree->AppendItem(rootItem, wxT("New Bundle"), 0, -1, new BundleItemData(bundleId, false));

	// The new bundle will first be saved when user has edited the name
	m_bundleTree->SelectItem(bundleItem);
	m_bundleTree->EditLabel(bundleItem);
	MarkAsModified();
}

void BundleEditor::NewItem(BundleItemType type) {
	const wxTreeItemId selItem = m_bundleTree->GetSelection();
	if (!selItem.IsOk()) return; // Can't add item if no bundle

	const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(selItem);

	// Get the parent bundle
	const wxTreeItemId bundleItem = data->IsBundle() ? selItem : m_bundleTree->GetItemParent(selItem);

	wxString name;
	unsigned int icon_id = 0;
	switch (type) {
	case BUNDLE_COMMAND:
		name = _("New Command");
		icon_id = 1;
		break;
	case BUNDLE_SNIPPET:
		name = _("New Snippet");
		icon_id = 2;
		break;
	case BUNDLE_DRAGCMD:
		name = _("New DragCommand");
		icon_id = 3;
		break;
	case BUNDLE_PREF:
		name = _("New Preference");
		icon_id = 4;
		break;
	case BUNDLE_LANGUAGE:
		name = _("New Language");
		icon_id = 5;
		break;
	default:
		wxASSERT(false);
	}

	// Create a new (unsaved) command item
	const unsigned int bundleId = data->m_bundleId;
	const unsigned int itemId = m_plistHandler.New(type, bundleId, name);
	const wxTreeItemId treeItem = m_bundleTree->AppendItem(bundleItem, name, icon_id, -1, new BundleItemData(type, bundleId, itemId, false));

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

	// The new command will first be saved when user has edited the name
	m_bundleTree->SelectItem(treeItem);
	m_bundleTree->EditLabel(treeItem);
	MarkAsModified();
}

void BundleEditor::OnNewCommand(wxCommandEvent& WXUNUSED(event)) {
	NewItem(BUNDLE_COMMAND);
}

void BundleEditor::OnNewDragCommand(wxCommandEvent& WXUNUSED(event)) {
	NewItem(BUNDLE_DRAGCMD);
}

void BundleEditor::OnNewSnippet(wxCommandEvent& WXUNUSED(event)) {
	NewItem(BUNDLE_SNIPPET);
}

void BundleEditor::OnNewPref(wxCommandEvent& WXUNUSED(event)) {
	NewItem(BUNDLE_PREF);
}

void BundleEditor::OnNewLanguage(wxCommandEvent& WXUNUSED(event)) {
	NewItem(BUNDLE_LANGUAGE);
}

bool BundleEditor::SaveChanges() {
	if (!SaveCurrentPanel()) {
		return false;
	}
	
	if (m_isModified) {
		// Show user that we are reloading
		wxBusyCursor wait;

		if (m_needFullReload) {
			// if bundles are deleted or prefs or languages changed,
			// we have to call LoadBundles since all syntaxes will have to be reloaded
			m_syntaxHandler.LoadBundles(TmSyntaxHandler::cxRELOAD);
		}
		else {
			m_syntaxHandler.ReParseBundles();
		}
	}

	m_isModified = false;
	m_needFullReload = false;

	return true;
}

void BundleEditor::OnClose(wxCloseEvent& event) {
	if (!SaveCurrentPanel()) {
		event.Veto();
		return;
	}
	
	Hide();

	if (m_isModified) {
		// Show user that we are reloading
		wxBusyCursor wait;

		if (m_needFullReload) {
			// if bundles are deleted or prefs or languages changed,
			// we have to call LoadBundles since all syntaxes will have to be reloaded
			m_syntaxHandler.LoadBundles(TmSyntaxHandler::cxRELOAD);
		}
		else {
			m_syntaxHandler.ReParseBundles();
		}
	}

	m_isModified = false;
	m_needFullReload = false;
}

void BundleEditor::SelectItem(BundleItemType type, unsigned int bundleId, unsigned int itemId) {
	const wxTreeItemId item = FindBundleItem(type, bundleId, itemId);

	if (item.IsOk()) m_bundleTree->SelectItem(item);
}

void BundleEditor::UpdateItemName(BundleItemType type, unsigned int bundleId, unsigned int itemId, const wxString& name) {
	const wxTreeItemId item = FindBundleItem(type, bundleId, itemId);

	if (item.IsOk()) m_bundleTree->SetItemText(item, name);
}

wxTreeItemId BundleEditor::FindBundle(unsigned int bundleId) const {
	const wxTreeItemId rootItem = m_bundleTree->GetRootItem();

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_bundleTree->GetFirstChild(rootItem, cookie);

	while (item.IsOk()) {
		const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(item);
		if (data->m_bundleId == bundleId) {
			wxASSERT(data->m_type == BUNDLE_BUNDLE);
			return item;
		}

		item = m_bundleTree->GetNextChild(rootItem, cookie);
	}

	// bundle not found, return invalid id
	return wxTreeItemId();
}
wxTreeItemId BundleEditor::FindBundleItem(BundleItemType type, unsigned int bundleId, unsigned int itemId) const {
	const wxTreeItemId bundleItem = FindBundle(bundleId);
	if (!bundleItem.IsOk()) return wxTreeItemId(); // bundle not found, return invalid id

	if (type == BUNDLE_BUNDLE) return bundleItem;

	wxTreeItemIdValue cookie;
	wxTreeItemId item = m_bundleTree->GetFirstChild(bundleItem, cookie);

	while (item.IsOk()) {
		const BundleItemData* data = (BundleItemData*)m_bundleTree->GetItemData(item);
		if (data->m_type == type && data->m_itemId == itemId) {
			return item;
		}

		item = m_bundleTree->GetNextChild(bundleItem, cookie);
	}

	// item not found, return invalid id
	return wxTreeItemId();
}


// ----- BundleItemData -----------------------------------------------

BundleEditor::BundleItemData::BundleItemData(unsigned int bundleId, bool isSaved)
: m_type(BUNDLE_BUNDLE), m_bundleId(bundleId), m_itemId(0), m_isSaved(isSaved) {
}

BundleEditor::BundleItemData::BundleItemData(BundleItemType type, unsigned int bundleId, unsigned int itemId, bool isSaved)
: m_type(type), m_bundleId(bundleId), m_itemId(itemId), m_isSaved(isSaved) {
	wxASSERT(type != BUNDLE_BUNDLE);
}


// ----- ShortcutField -----------------------------------------------

BundleEditor::ShortcutCtrl::ShortcutCtrl(wxWindow* parent) {
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE);
}

void BundleEditor::ShortcutCtrl::Clear() {
	wxTextCtrl::Clear();
	m_binding.clear();
}

void BundleEditor::ShortcutCtrl::SetKey(const wxString& binding) {
	m_binding = binding;

	const tmKey key(binding);
	SetValue(key.shortcut);
}

bool BundleEditor::ShortcutCtrl::OnPreKeyDown(wxKeyEvent& event)
{
	int id = event.GetKeyCode();

	// Ignore initial presses on modifier key
	if (id != WXK_SHIFT &&
		id != WXK_ALT &&
		id != WXK_CONTROL &&
		id != WXK_WINDOWS_LEFT &&
		id != WXK_WINDOWS_RIGHT)
	{
		int modifiers = event.GetModifiers();
#ifdef __WXMSW__
		if (HIWORD(event.m_rawFlags) & KF_EXTENDED) modifiers |= 0x0010; // Extended (usually keypad)
#endif
		tmKey key(id, modifiers);

		SetValue(key.shortcut);

		m_binding = key.getBinding();
		wxLogDebug(wxT("Binding: %s"), m_binding.c_str());
		return true;
	}
	return false;
}

// ----- SelectorPanel -----------------------------------------------

// ctrl id's
enum {
	CTRL_CLEARSHORTCUT,
	CTRL_TRIGGERCHOICE
};

BEGIN_EVENT_TABLE(BundleEditor::SelectorPanel, wxPanel)
	EVT_BUTTON(CTRL_CLEARSHORTCUT, BundleEditor::SelectorPanel::OnClearShortcut)
	EVT_CHOICE(CTRL_TRIGGERCHOICE, BundleEditor::SelectorPanel::OnTriggerChoice)
END_EVENT_TABLE()

BundleEditor::SelectorPanel::SelectorPanel(wxWindow* parent)
: wxPanel(parent) {
	// Choices for trigger choice
	wxArrayString choices;
	choices.Add(_("Tab Trigger"));
	choices.Add(_("Key Equivalent"));

	// Create controls
	m_activationStatic = new wxStaticText(this, wxID_ANY, _("Activation:"));
	m_scopeStatic = new wxStaticText(this, wxID_ANY, _("Scope Selector:"));
	m_triggerChoice = new wxChoice(this, CTRL_TRIGGERCHOICE, wxDefaultPosition, wxDefaultSize, choices);
	m_tabText = new wxTextCtrl(this, wxID_ANY);
	m_shortcutCtrl = new ShortcutCtrl(this);
	m_clearButton = new wxButton(this, CTRL_CLEARSHORTCUT, _("Clear"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_scopeText = new wxTextCtrl(this, wxID_ANY);

	// We want the clear button to be same height as the textctrls
	const wxSize textSize = m_shortcutCtrl->GetSize();
	wxSize buttonSize = m_clearButton->GetSize();
	buttonSize.y = textSize.y;
	m_clearButton->SetSize(buttonSize);
	m_clearButton->SetSizeHints(buttonSize, buttonSize);

	m_triggerSizer = new wxBoxSizer(wxHORIZONTAL);
	m_triggerSizer->Add(m_tabText, 1);
	m_triggerSizer->Add(m_shortcutCtrl, 1);
	m_triggerSizer->Add(m_clearButton, 0);

	// Create the layout
	m_gridSizer = new wxGridBagSizer(5, 5);
		m_gridSizer->Add(m_activationStatic, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		m_gridSizer->Add(m_triggerChoice, wxGBPosition(0,1));
		m_gridSizer->Add(m_triggerSizer, wxGBPosition(0,2), wxDefaultSpan, wxEXPAND);
		m_gridSizer->Add(m_scopeStatic, wxGBPosition(1,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		m_gridSizer->Add(m_scopeText, wxGBPosition(1,1), wxGBSpan(1,2), wxEXPAND);
		m_gridSizer->AddGrowableCol(2);

	SetSizer(m_gridSizer);
}

void BundleEditor::SelectorPanel::ShowOnlyScope() {
	// workaround: hiding a top row leaves a gap, so for now we just disable
	m_gridSizer->Show(m_triggerChoice);
	m_triggerChoice->Disable();
	m_tabText->Disable();
	m_shortcutCtrl->Disable();
	m_clearButton->Disable();

	m_gridSizer->Show(m_scopeStatic);
	m_gridSizer->Show(m_scopeText);
	m_gridSizer->Layout();
}

void BundleEditor::SelectorPanel::ShowOnlyKey() {
	m_gridSizer->Hide(m_triggerChoice);
	m_triggerChoice->Enable();
	m_tabText->Enable();
	m_shortcutCtrl->Enable();
	m_clearButton->Enable();

	m_gridSizer->Hide(m_scopeStatic);
	m_gridSizer->Hide(m_scopeText);
	m_gridSizer->Layout();
}

void BundleEditor::SelectorPanel::Update(const PListDict& dict) {

	const char* tabTrigger = dict.GetString("tabTrigger");
	if (tabTrigger) {
		m_triggerChoice->SetSelection(0);
		m_tabText->SetValue(wxString(tabTrigger, wxConvUTF8));

		if (m_gridSizer->IsShown(m_triggerSizer)) {
			m_triggerSizer->Show(m_tabText, true);
			m_triggerSizer->Show(m_shortcutCtrl, false);
			m_triggerSizer->Show(m_clearButton, false);
			m_triggerSizer->Layout();
		}
	}
	else {
		m_triggerChoice->SetSelection(1); // keyEquiv is default
		if (m_gridSizer->IsShown(m_triggerSizer)) {
			m_triggerSizer->Show(m_tabText, false);
			m_triggerSizer->Show(m_shortcutCtrl, true);
			m_triggerSizer->Show(m_clearButton, true);
			m_triggerSizer->Layout();
		}

		const char* keyEquiv = dict.GetString("keyEquivalent");
		if (keyEquiv) {
			const wxString binding(keyEquiv, wxConvUTF8);
			m_shortcutCtrl->SetKey(binding);
		}
		else {
			m_shortcutCtrl->Clear();
		}
	}

	const wxString scope = dict.wxGetString("scope");
	m_scopeText->SetValue(scope);
}

bool BundleEditor::SelectorPanel::IsModified(const PListDict& dict) const {
	const char* tabTrigger = dict.GetString("tabTrigger");
	if (tabTrigger) {
		if (m_triggerChoice->GetSelection() != 0) return true;
		if (m_tabText->GetValue() != wxString(tabTrigger, wxConvUTF8)) return true;
	}
	else {
		if (m_triggerChoice->GetSelection() != 1) return true; // keyEquiv is default

		const char* keyEquiv = dict.GetString("keyEquivalent");
		if (keyEquiv) {
			const wxString key(keyEquiv, wxConvUTF8);
			if (m_shortcutCtrl->GetBinding() != key) return true;
		}
		else if (!m_shortcutCtrl->GetBinding().empty()) return true;
	}

	if (m_scopeText->GetValue() != dict.wxGetString("scope")) return true;

	return false;
}

void BundleEditor::SelectorPanel::Save(PListDict& dict) {
	if (m_triggerChoice->GetSelection() == 0) {
		dict.DeleteItem("keyEquivalent"); // if exists

		const char* tabTrigger = dict.GetString("tabTrigger");
		if (!tabTrigger || m_tabText->GetValue() != wxString(tabTrigger, wxConvUTF8)) {
			dict.wxSetString("tabTrigger", m_tabText->GetValue());
		}
	}
	else {
		dict.DeleteItem("tabTrigger"); // if exists

		const wxString& binding = m_shortcutCtrl->GetBinding();
		const char* keyEquiv = dict.GetString("keyEquivalent");
		if ((keyEquiv && binding != wxString(keyEquiv, wxConvUTF8))
			|| (!keyEquiv && !binding.empty())) {

			dict.wxSetString("keyEquivalent", binding);
		}
	}

	if (m_scopeText->GetValue() != dict.wxGetString("scope")) {
		dict.wxSetString("scope", m_scopeText->GetValue());
	}
}

void BundleEditor::SelectorPanel::OnTriggerChoice(wxCommandEvent& event) {
	if (event.GetSelection() == 0) {
		m_triggerSizer->Show(m_tabText, true);
		m_triggerSizer->Show(m_shortcutCtrl, false);
		m_triggerSizer->Show(m_clearButton, false);
	}
	else {
		m_triggerSizer->Show(m_tabText, false);
		m_triggerSizer->Show(m_shortcutCtrl, true);
		m_triggerSizer->Show(m_clearButton, true);
	}
	m_triggerSizer->Layout();
}

void BundleEditor::SelectorPanel::OnClearShortcut(wxCommandEvent& WXUNUSED(event)) {
	m_shortcutCtrl->Clear();
}

// ----- CommandPanel -----------------------------------------------


enum {
	CTRL_INPUTCHOICE
};

BEGIN_EVENT_TABLE(BundleEditor::CommandPanel, wxPanel)
	EVT_CHOICE(CTRL_INPUTCHOICE, BundleEditor::CommandPanel::OnInputChoice)
END_EVENT_TABLE()

BundleEditor::CommandPanel::CommandPanel(wxWindow* parent, PListHandler& plistHandler)
: wxPanel(parent), m_plistHandler(plistHandler) {
	// Choices for 'save before command'
	wxArrayString saveChoices;
	saveChoices.Add(_("Nothing"));
	saveChoices.Add(_("Current File"));
	saveChoices.Add(_("All Files"));

	// Choices for Environment
	wxArrayString envChoices;
	envChoices.Add(_("Cygwin (generic)"));
	envChoices.Add(_("Cygwin (windows)"));
	envChoices.Add(_("Windows native"));

	// Choices for Input
	wxArrayString inputChoices;
	inputChoices.Add(_("None"));
	inputChoices.Add(_("Selected Text"));
	inputChoices.Add(_("Entire Document"));

	// Choices for Fallback
	wxArrayString fallbackChoices;
	fallbackChoices.Add(_("Document"));
	fallbackChoices.Add(_("Line"));
	fallbackChoices.Add(_("Word"));
	fallbackChoices.Add(_("Character"));
	fallbackChoices.Add(_("Scope"));
	fallbackChoices.Add(_("Nothing"));

	// Choices for Output
	wxArrayString outputChoices;
	outputChoices.Add(_("Discard"));
	outputChoices.Add(_("Replace Selected Text"));
	outputChoices.Add(_("Replace Document"));
	outputChoices.Add(_("Insert as Text"));
	outputChoices.Add(_("Insert as Snippet"));
	outputChoices.Add(_("Show as HTML"));
	outputChoices.Add(_("Show as Tool Tip"));
	outputChoices.Add(_("Create New Document"));

	// Create controls
	wxStaticText* saveStatic = new wxStaticText(this, wxID_ANY, _("Save:"));
	wxStaticText* envStatic = new wxStaticText(this, wxID_ANY, _("Environment:"));
	wxStaticText* inputStatic = new wxStaticText(this, wxID_ANY, _("Input:"));
	m_orStatic = new wxStaticText(this, wxID_ANY, _("or"));
	wxStaticText* outputStatic = new wxStaticText(this, wxID_ANY, _("Output:"));
	m_cmdCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxTE_PROCESS_TAB);
	m_cmdCtrl->SetMinSize(wxSize(50,50)); // ensure resizeability
	m_selectorPanel = new SelectorPanel(this);
	m_saveChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, saveChoices);
	m_envChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, envChoices);
	m_inputChoice = new wxChoice(this, CTRL_INPUTCHOICE, wxDefaultPosition, wxDefaultSize, inputChoices);
	m_fallbackChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, fallbackChoices);
	m_outputChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, outputChoices);

	// Create the layout
	m_inputSizer = new wxBoxSizer(wxHORIZONTAL);
		m_inputSizer->Add(m_inputChoice, 0);
		m_inputSizer->Add(m_orStatic, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
		m_inputSizer->Add(m_fallbackChoice, 0);

	wxSizer* envSizer = new wxBoxSizer(wxHORIZONTAL);
			envSizer->Add(envStatic, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT, 5);
			envSizer->Add(m_envChoice, 0, wxALIGN_RIGHT);

	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		wxGridBagSizer* gridSizer = new wxGridBagSizer(5, 5);
			gridSizer->Add(saveStatic, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
			gridSizer->Add(m_saveChoice, wxGBPosition(0,1));
			gridSizer->Add(envSizer, wxGBPosition(0,2), wxDefaultSpan, wxALIGN_RIGHT);
			gridSizer->Add(m_cmdCtrl, wxGBPosition(1,0), wxGBSpan(1,3), wxEXPAND);
			gridSizer->Add(inputStatic, wxGBPosition(2,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
			gridSizer->Add(m_inputSizer, wxGBPosition(2,1), wxGBSpan(1,2));
			gridSizer->Add(outputStatic, wxGBPosition(3,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
			gridSizer->Add(m_outputChoice, wxGBPosition(3,1));
			gridSizer->AddGrowableRow(1);
			gridSizer->AddGrowableCol(2);
			mainSizer->Add(gridSizer, 1, wxEXPAND|wxALL, 5);
		mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND|wxALL, 5);
		mainSizer->Add(m_selectorPanel, 0, wxEXPAND|wxALL, 5);

	SetSizer(mainSizer);
}

void BundleEditor::CommandPanel::SetCommand(unsigned int bundleId, unsigned int cmdId) {
	m_bundleId = bundleId;
	m_cmdId = cmdId;

	const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, bundleId, cmdId);

	// Get snippet contents
	m_envSel = 0;
	const char* co = cmdDict.GetString("winCommand");
	if (!co) co = cmdDict.GetString("command");
	else m_envSel = 1;
	if (co) {
		const wxString content(co, wxConvUTF8);
		m_cmdCtrl->SetValue(content);
	}
	else m_cmdCtrl->Clear();

	// Check if it should run native
	const char* runEnv = cmdDict.GetString("runEnvironment");
	if (runEnv) {
		wxASSERT(m_envSel == 1); // command should be saved in winCommand
		m_envSel = 2;
	}
	m_envChoice->SetSelection(m_envSel);

	// Action to do before command
	const char* beforeCommand = cmdDict.GetString("beforeRunningCommand");
	m_saveSel = 0;
	if (beforeCommand) {
		if (strcmp(beforeCommand, "nop") == 0) m_saveSel = 0;
		else if (strcmp(beforeCommand, "saveActiveFile") == 0) m_saveSel = 1;
		else if (strcmp(beforeCommand, "saveModifiedFiles") == 0) m_saveSel = 2;
	}
	m_saveChoice->SetSelection(m_saveSel);

	// Input source
	const char* inputSource = cmdDict.GetString("input");
	m_inputSel = 0;
	if (inputSource) {
		if (strcmp(inputSource, "none") == 0) m_inputSel = 0;
		else if (strcmp(inputSource, "selection") == 0) m_inputSel = 1;
		else if (strcmp(inputSource, "document") == 0) m_inputSel = 2;
	}
	m_inputChoice->SetSelection(m_inputSel);

	// Fallback is only valid if input is selection
	if (m_inputSel == 1) {
		// Fallback input source
		const char* fallbackInput = cmdDict.GetString("fallbackInput");
		m_fallbackSel = 5;
		if (fallbackInput) {
			if (strcmp(fallbackInput, "document") == 0)       m_fallbackSel = 0;
			else if (strcmp(fallbackInput, "line") == 0)      m_fallbackSel = 1;
			else if (strcmp(fallbackInput, "word") == 0)      m_fallbackSel = 2;
			else if (strcmp(fallbackInput, "character") == 0) m_fallbackSel = 3;
			else if (strcmp(fallbackInput, "scope") == 0)     m_fallbackSel = 4;
			else if (strcmp(fallbackInput, "none") == 0)      m_fallbackSel = 5;
		}
		m_fallbackChoice->SetSelection(m_fallbackSel);

		m_inputSizer->Show(m_orStatic, true);
		m_inputSizer->Show(m_fallbackChoice, true);
	}
	else {
		m_fallbackSel = -1;
		m_inputSizer->Show(m_orStatic, false);
		m_inputSizer->Show(m_fallbackChoice, false);
	}
	m_inputSizer->Layout();

	// Output destination
	const char* outputDest = cmdDict.GetString("output");
	m_outputSel = 0;
	if (outputDest) {
		if (strcmp(outputDest, "discard") == 0)                  m_outputSel = 0;
		else if (strcmp(outputDest, "replaceSelectedText") == 0) m_outputSel = 1;
		else if (strcmp(outputDest, "replaceDocument") == 0)     m_outputSel = 2;
		else if (strcmp(outputDest, "afterSelectedText") == 0)   m_outputSel = 3;
		else if (strcmp(outputDest, "insertAsSnippet") == 0)     m_outputSel = 4;
		else if (strcmp(outputDest, "showAsHTML") == 0)          m_outputSel = 5;
		else if (strcmp(outputDest, "showAsTooltip") == 0)       m_outputSel = 6;
		else if (strcmp(outputDest, "openAsNewDocument") == 0)   m_outputSel = 7;
	}
	m_outputChoice->SetSelection(m_outputSel);

	m_selectorPanel->Update(cmdDict);
}

bool BundleEditor::CommandPanel::IsModified() const {
	if (m_cmdCtrl->IsModified()) return true;
	if (m_saveChoice->GetSelection() != m_saveSel) return true;
	if (m_envChoice->GetSelection() != m_envSel) return true;
	if (m_inputChoice->GetSelection() != m_inputSel) return true;
	if (m_fallbackChoice->IsShown() && m_fallbackChoice->GetSelection() != m_fallbackSel) return true;
	if (m_outputChoice->GetSelection() != m_outputSel) return true;

	const PListDict cmdDict = m_plistHandler.Get(BUNDLE_COMMAND, m_bundleId, m_cmdId);
	if (m_selectorPanel->IsModified(cmdDict)) return true;

	return false;
}

bool BundleEditor::CommandPanel::Save() {
	if (!IsModified()) return false;

	PListDict cmdDict = m_plistHandler.GetEditable(BUNDLE_COMMAND, m_bundleId, m_cmdId);

	const int envSel = m_envChoice->GetSelection();
	if (m_cmdCtrl->IsModified() || envSel != m_envSel) {
		const wxString content = m_cmdCtrl->GetValue();

		// Save command content
		if (envSel == 0) {
			cmdDict.DeleteItem("winCommand");
			cmdDict.wxSetString("command", content);
		}
		else {
			cmdDict.wxSetString("winCommand", content);
		}

		// Set run environment
		if (envSel == 2) {
			cmdDict.SetString("runEnvironment", "windows");
		}
		else {
			cmdDict.DeleteItem("runEnvironment");
		}

		m_envSel = envSel;
		m_cmdCtrl->DiscardEdits(); // reset internal modified flag
	}

	const int saveSel = m_saveChoice->GetSelection();
	if (saveSel != m_saveSel) {
		switch (saveSel) {
		case 0: cmdDict.SetString("beforeRunningCommand", "nop"); break;
		case 1: cmdDict.SetString("beforeRunningCommand", "saveActiveFile"); break;
		case 2: cmdDict.SetString("beforeRunningCommand", "saveModifiedFiles"); break;
		}
		m_saveSel = saveSel;
	}

	const int inputSel = m_inputChoice->GetSelection();
	if (inputSel != m_inputSel) {
		switch (inputSel) {
		case 0: cmdDict.SetString("input", "none"); break;
		case 1: cmdDict.SetString("input", "selection"); break;
		case 2: cmdDict.SetString("input", "document"); break;
		}
		m_inputSel = inputSel;
	}

	if (inputSel == 1) {
		const int fallbackSel = m_fallbackChoice->GetSelection();
		if (fallbackSel != m_fallbackSel) {
			switch(fallbackSel) {
			case 0: cmdDict.SetString("fallbackInput", "document"); break;
			case 1: cmdDict.SetString("fallbackInput", "line"); break;
			case 2: cmdDict.SetString("fallbackInput", "word"); break;
			case 3: cmdDict.SetString("fallbackInput", "character"); break;
			case 4: cmdDict.SetString("fallbackInput", "scope"); break;
			case 5: cmdDict.SetString("fallbackInput", "none"); break;
			}
			m_fallbackSel = fallbackSel;
		}
	}
	else cmdDict.DeleteItem("fallbackInput"); // fallback only valid if input is selection

	const int outputSel = m_outputChoice->GetSelection();
	if (outputSel != m_outputSel) {
		switch (outputSel) {
		case 0: cmdDict.SetString("output", "discard"); break;
		case 1: cmdDict.SetString("output", "replaceSelectedText"); break;
		case 2: cmdDict.SetString("output", "replaceDocument"); break;
		case 3: cmdDict.SetString("output", "afterSelectedText"); break;
		case 4: cmdDict.SetString("output", "insertAsSnippet"); break;
		case 5: cmdDict.SetString("output", "showAsHTML"); break;
		case 6: cmdDict.SetString("output", "showAsTooltip"); break;
		case 7: cmdDict.SetString("output", "openAsNewDocument"); break;
		}
		m_outputSel = outputSel;
	}

	m_selectorPanel->Save(cmdDict);

	m_plistHandler.Save(BUNDLE_COMMAND, m_bundleId, m_cmdId);
	return true;
}

void BundleEditor::CommandPanel::OnInputChoice(wxCommandEvent& event) {
	if (event.GetSelection() == 1) {
		m_inputSizer->Show(m_orStatic, true);
		m_inputSizer->Show(m_fallbackChoice, true);
	}
	else {
		m_fallbackSel = -1;
		m_inputSizer->Show(m_orStatic, false);
		m_inputSizer->Show(m_fallbackChoice, false);
	}
	m_inputSizer->Layout();
}

// ----- SnippetPanel -----------------------------------------------

BundleEditor::SnippetPanel::SnippetPanel(wxWindow* parent, PListHandler& plistHandler)
: wxPanel(parent), m_plistHandler(plistHandler) {
	// Choices for Environment
	wxArrayString choices;
	choices.Add(_("Cygwin (generic)"));
	choices.Add(_("Cygwin (windows)"));

	// Create controls
	wxStaticText* envStatic = new wxStaticText(this, wxID_ANY, _("Environment:"));
	m_envChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	m_snippetCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxTE_PROCESS_TAB);
	m_snippetCtrl->SetMinSize(wxSize(50,50)); // ensure resizeability

	m_selectorPanel = new SelectorPanel(this);

	// Create the layout
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		wxSizer* envSizer = new wxBoxSizer(wxHORIZONTAL);
			envSizer->Add(envStatic, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
			envSizer->Add(m_envChoice, 0, wxALIGN_RIGHT|wxALL, 5);
			mainSizer->Add(envSizer, 0, wxALIGN_RIGHT);
		mainSizer->Add(m_snippetCtrl, 1, wxEXPAND|wxALL, 5);
		mainSizer->Add(m_selectorPanel, 0, wxEXPAND|wxALL, 5);

	SetSizer(mainSizer);
}

void BundleEditor::SnippetPanel::SetSnippet(unsigned int bundleId, unsigned int snippetId) {
	m_bundleId = bundleId;
	m_snippetId = snippetId;

	const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, bundleId, snippetId);

	// Get snippet contents
	m_envSel = 0;
	const char* co = snipDict.GetString("winContent");
	if (!co) co = snipDict.GetString("content");
	else m_envSel = 1;
	if (co) {
		const wxString content(co, wxConvUTF8);
		m_snippetCtrl->SetValue(content);
	}
	else m_snippetCtrl->Clear();
	m_envChoice->SetSelection(m_envSel);

	m_selectorPanel->Update(snipDict);
}


bool BundleEditor::SnippetPanel::IsModified() const {
	if (m_envChoice->GetSelection() != m_envSel) return true;
	if (m_snippetCtrl->IsModified()) return true;

	const PListDict snipDict = m_plistHandler.Get(BUNDLE_SNIPPET, m_bundleId, m_snippetId);
	if (m_selectorPanel->IsModified(snipDict)) return true;

	return false;
}

bool BundleEditor::SnippetPanel::Save() {
	if (!IsModified()) return false;

	PListDict snipDict = m_plistHandler.GetEditable(BUNDLE_SNIPPET, m_bundleId, m_snippetId);

	const int envSel = m_envChoice->GetSelection();
	if (m_snippetCtrl->IsModified() || envSel != m_envSel) {
		const wxString content = m_snippetCtrl->GetValue();

		if (m_envChoice->GetSelection() == 0) {
			snipDict.DeleteItem("winContent");
			snipDict.wxSetString("content", content);
		}
		else {
			snipDict.wxSetString("winContent", content);
		}

		m_envSel = envSel;
		m_snippetCtrl->DiscardEdits(); // reset internal modified flag
	}

	m_selectorPanel->Save(snipDict);

	m_plistHandler.Save(BUNDLE_SNIPPET, m_bundleId, m_snippetId);
	return true;
}

// ----- PrefsPanel -----------------------------------------------

BundleEditor::PrefsPanel::PrefsPanel(wxWindow* parent, PListHandler& plistHandler)
: wxPanel(parent), m_plistHandler(plistHandler) {
	m_cmdCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxTE_PROCESS_TAB);
	m_cmdCtrl->SetMinSize(wxSize(50,50)); // ensure resizeability
	m_selectorPanel = new SelectorPanel(this);

	// Create the layout
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_cmdCtrl, 1, wxEXPAND|wxALL, 5);
		mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND|wxALL, 5);
		mainSizer->Add(m_selectorPanel, 0, wxEXPAND|wxALL, 5);

	SetSizer(mainSizer);
}

void BundleEditor::PrefsPanel::SetPref(unsigned int bundleId, unsigned int prefId) {
	m_bundleId = bundleId;
	m_prefId = prefId;
	m_mode = BUNDLE_PREF;

	const PListDict prefDict = m_plistHandler.Get(BUNDLE_PREF, bundleId, prefId);

	// Get the settings
	PListDict settingsDict;
	if (prefDict.GetDict("settings", settingsDict)) {
		const wxString jsonSettings = settingsDict.GetJSON();
		m_cmdCtrl->SetValue(jsonSettings);
	}
	else m_cmdCtrl->SetValue(wxT("{ }"));

	m_selectorPanel->ShowOnlyScope();
	m_selectorPanel->Update(prefDict);
	GetSizer()->Layout();
}

void BundleEditor::PrefsPanel::SetLanguage(unsigned int bundleId, unsigned int langId) {
	m_bundleId = bundleId;
	m_prefId = langId;
	m_mode = BUNDLE_LANGUAGE;

	const PListDict prefDict = m_plistHandler.Get(BUNDLE_LANGUAGE, bundleId, langId);

	// Get the settings
	const wxString jsonSettings = prefDict.GetJSON(true /*strip*/);
	m_cmdCtrl->SetValue(jsonSettings);

	m_selectorPanel->ShowOnlyKey();
	m_selectorPanel->Update(prefDict);
	GetSizer()->Layout();
}

bool BundleEditor::PrefsPanel::IsModified() const {
	if (m_cmdCtrl->IsModified()) return true;

	const PListDict prefDict = m_plistHandler.Get(m_mode, m_bundleId, m_prefId);
	if (m_selectorPanel->IsModified(prefDict)) return true;

	return false;
}

BundleEditor::SaveResult BundleEditor::PrefsPanel::Save() {
	if (!IsModified()) return BundleEditor::SAVE_NOCHANGE;

	wxJSONValue root;
	if (m_cmdCtrl->IsModified()) {
		const wxString jsonStr = m_cmdCtrl->GetValue();

		// Parse the JSON string
		wxJSONReader reader;
		const int numErrors = reader.Parse(jsonStr, &root);
		if ( numErrors > 0 )  {
			// if there are errors in the JSON document, print the
			// errors and return a non-ZERO value
			wxString msg = _("Invalid JSON syntax:\n");
			const wxArrayString& errors = reader.GetErrors();
			for ( int i = 0; i < numErrors; i++ )  {
				msg += wxT('\n');
				msg += errors[i];
			}
			msg += _("\n\nDo you want to discard changes?");

			const int res = wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK|wxCANCEL, GetParent());
			if (res == wxOK) {
				// Restore old settings
				if (m_mode == BUNDLE_PREF) SetPref(m_bundleId, m_prefId);
				else if (m_mode == BUNDLE_LANGUAGE) SetPref(m_bundleId, m_prefId);

				return BundleEditor::SAVE_NOCHANGE;
			}
			else {
				m_cmdCtrl->SetFocus();
				return BundleEditor::SAVE_CANCEL;
			}
		}
	}

	if (m_mode == BUNDLE_PREF) {
		PListDict prefDict = m_plistHandler.GetEditable(BUNDLE_PREF, m_bundleId, m_prefId);
		PListDict settingsDict;
		if (!prefDict.GetDict("settings", settingsDict)) settingsDict = prefDict.NewDict("settings");

		if (m_cmdCtrl->IsModified()) {
			settingsDict.SetJSON(root);
		}
		m_selectorPanel->Save(prefDict);

		m_plistHandler.Save(BUNDLE_PREF, m_bundleId, m_prefId);
	}
	else if (m_mode == BUNDLE_LANGUAGE) {
		PListDict langDict = m_plistHandler.GetEditable(BUNDLE_LANGUAGE, m_bundleId, m_prefId);
		
		if (m_cmdCtrl->IsModified()) {
			langDict.SetJSON(root, true /*strip*/);
		}
		m_selectorPanel->Save(langDict);

		m_plistHandler.Save(BUNDLE_LANGUAGE, m_bundleId, m_prefId);
	}
	else wxASSERT(false);

	m_cmdCtrl->DiscardEdits(); // reset internal modified flag
	return BundleEditor::SAVE_OK;
}


// ----- DragPanel -----------------------------------------------

BundleEditor::DragPanel::DragPanel(wxWindow* parent, PListHandler& plistHandler)
: wxPanel(parent), m_plistHandler(plistHandler) {
	// Choices for Environment
	wxArrayString choices;
	choices.Add(_("Cygwin (generic)"));
	choices.Add(_("Cygwin (windows)"));

	// Create controls
	wxStaticText* envStatic = new wxStaticText(this, wxID_ANY, _("Environment:"));
	m_envChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	wxStaticText* extStatic = new wxStaticText(this, wxID_ANY, _("File Types:"));
	m_extText = new wxTextCtrl(this, wxID_ANY);
	m_cmdCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxTE_PROCESS_TAB);
	m_cmdCtrl->SetMinSize(wxSize(50,50)); // ensure resizeability
	wxStaticText* scopeStatic = new wxStaticText(this, wxID_ANY, _("Scope Selector:"));
	m_scopeText = new wxTextCtrl(this, wxID_ANY);

	// Create the layout
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		wxSizer* envSizer = new wxBoxSizer(wxHORIZONTAL);
			envSizer->Add(envStatic, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
			envSizer->Add(m_envChoice, 0, wxALIGN_RIGHT|wxALL, 5);
			mainSizer->Add(envSizer, 0, wxALIGN_RIGHT);
		wxSizer* extSizer = new wxBoxSizer(wxHORIZONTAL);
			extSizer->Add(extStatic, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			extSizer->Add(m_extText, 1, wxEXPAND|wxALL, 5);
			mainSizer->Add(extSizer, 0, wxEXPAND);
		mainSizer->Add(m_cmdCtrl, 1, wxEXPAND|wxALL, 5);
		wxSizer* scopeSizer = new wxBoxSizer(wxHORIZONTAL);
			scopeSizer->Add(scopeStatic, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			scopeSizer->Add(m_scopeText, 1, wxEXPAND|wxALL, 5);
			mainSizer->Add(scopeSizer, 0, wxEXPAND);
		//mainSizer->Add(m_selectorPanel, 0, wxEXPAND|wxALL, 5);

	SetSizer(mainSizer);
}

void BundleEditor::DragPanel::SetDragCommand(unsigned int bundleId, unsigned int cmdId) {
	m_bundleId = bundleId;
	m_cmdId = cmdId;

	const PListDict cmdDict = m_plistHandler.Get(BUNDLE_DRAGCMD, bundleId, cmdId);

	// Get file extensions
	PListArray extArray;
	if (cmdDict.GetArray("draggedFileExtensions", extArray)) {
		wxString fileTypes;

		for (unsigned int i = 0; i < extArray.GetSize(); ++i) {
			if (i) fileTypes += wxT(", ");
			fileTypes += extArray.wxGetString(i);
		}

		m_extText->SetValue(fileTypes);
	}
	else m_extText->Clear();

	// Get command contents
	m_envSel = 0;
	const char* co = cmdDict.GetString("winCommand");
	if (!co) co = cmdDict.GetString("command");
	else m_envSel = 1;
	if (co) {
		const wxString content(co, wxConvUTF8);
		m_cmdCtrl->SetValue(content);
	}
	else m_cmdCtrl->Clear();
	m_envChoice->SetSelection(m_envSel);

	// Get Scope
	const wxString scope = cmdDict.wxGetString("scope");
	m_scopeText->SetValue(scope);
}

bool BundleEditor::DragPanel::IsModified() const {
	if (m_envChoice->GetSelection() != m_envSel) return true;
	if (m_extText->IsModified()) return true;
	if (m_cmdCtrl->IsModified()) return true;
	if (m_scopeText->IsModified()) return true;

	return false;
}

bool BundleEditor::DragPanel::Save() {
	if (!IsModified()) return false;

	PListDict cmdDict = m_plistHandler.GetEditable(BUNDLE_DRAGCMD, m_bundleId, m_cmdId);

	if (m_extText->IsModified()) {
		PListArray extArray = cmdDict.NewArray("draggedFileExtensions");
		extArray.Clear();

		wxStringTokenizer tokens(m_extText->GetValue(), wxT(" ,"), wxTOKEN_STRTOK);

		for (wxString tok = tokens.GetNextToken(); !tok.empty(); tok = tokens.GetNextToken()) {
			extArray.AddString(tok.mb_str(wxConvUTF8));
		}

		m_extText->DiscardEdits(); // reset internal modified flag
	}

	const int envSel = m_envChoice->GetSelection();
	if (m_cmdCtrl->IsModified() || envSel != m_envSel) {
		const wxString content = m_cmdCtrl->GetValue();

		if (m_envChoice->GetSelection() == 0) {
			cmdDict.DeleteItem("winCommand");
			cmdDict.wxSetString("command", content);
		}
		else {
			cmdDict.wxSetString("winCommand", content);
		}

		m_envSel = envSel;
		m_extText->DiscardEdits(); // reset internal modified flag
	}

	if (m_scopeText->IsModified()) {
		cmdDict.wxSetString("scope", m_scopeText->GetValue());
		m_scopeText->DiscardEdits(); // reset internal modified flag
	}

	m_plistHandler.Save(BUNDLE_DRAGCMD, m_bundleId, m_cmdId);
	return true;
}
