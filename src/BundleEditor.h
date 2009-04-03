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

#ifndef __BUNDLEEDITOR_H__
#define __BUNDLEEDITOR_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "tm_syntaxhandler.h"
#include "key_hook.h"

#include <wx/treectrl.h>
#include <wx/dnd.h>
#include <wx/dragimag.h>
#include <wx/imaglist.h>
#include <wx/gbsizer.h>

class BundleEditor : public wxDialog {
public:
	BundleEditor(wxWindow *parent);

	void LoadBundles();
	bool SaveChanges();

	bool IsModified() const;;

	enum SaveResult {SAVE_OK, SAVE_NOCHANGE, SAVE_CANCEL};

	void SelectItem(BundleItemType type, unsigned int bundleId, unsigned int itemId);
	void UpdateItemName(BundleItemType type, unsigned int bundleId, unsigned int itemId, const wxString& name);

private:
	bool SaveCurrentPanel();
	void UpdatePanelTitle(BundleItemType type, const wxString& itemName);
	void MarkAsModified() {m_isModified = true;};

	wxTreeItemId FindBundle(unsigned int bundleId) const;
	wxTreeItemId FindBundleItem(BundleItemType type, unsigned int bundleId, unsigned int itemId) const;

	void NewItem(BundleItemType type);

	// Event handlers
	void OnTreeItemChanging(wxTreeEvent& event);
	void OnTreeItemSelected(wxTreeEvent& event);
	void OnTreeEndLabelEdit(wxTreeEvent& event);
	void OnTreeBeginDrag(wxTreeEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnNewBundleItem(wxCommandEvent& event);
	void OnDeleteBundleItem(wxCommandEvent& event);
	void OnNewBundle(wxCommandEvent& event);
	void OnNewCommand(wxCommandEvent& event);
	void OnNewSnippet(wxCommandEvent& event);
	void OnNewDragCommand(wxCommandEvent& event);
	void OnNewPref(wxCommandEvent& event);
	void OnNewLanguage(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	DECLARE_EVENT_TABLE()

	class BundleItemData : public wxTreeItemData {
	public:
		BundleItemData(unsigned int bundleId, bool isSaved=true);
		BundleItemData(BundleItemType type, unsigned int bundleId, unsigned int itemId, bool isSaved=true);
		bool IsSaved() const {return m_isSaved;};
		void MarkAsSaved() {m_isSaved = true;};

		bool IsBundle() const {return m_type == BUNDLE_BUNDLE;};

		const BundleItemType m_type;
		const unsigned int m_bundleId;
		const unsigned int m_itemId;
		bool m_isSaved;
	};

	class ShortcutCtrl : public KeyHookable<wxTextCtrl> {
	public:
		ShortcutCtrl(wxWindow* parent);
		void Clear();

		void SetKey(const wxString& binding);
		const wxString& GetBinding() const {return m_binding;};

		virtual bool OnPreKeyDown(wxKeyEvent& event);

	private:
		wxString m_binding;
	};

	class SelectorPanel : public wxPanel {
	public:
		SelectorPanel(wxWindow* parent);
		void Update(const PListDict& dict);
		bool IsModified(const PListDict& dict) const;
		void Save(PListDict& dict);

		void ShowOnlyScope();
		void ShowOnlyKey();
	private:
		void OnClearShortcut(wxCommandEvent& event);
		void OnTriggerChoice(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()

		wxStaticText* m_activationStatic;
		wxChoice* m_triggerChoice;
		wxTextCtrl* m_tabText;
		ShortcutCtrl* m_shortcutCtrl;
		wxButton* m_clearButton;
		wxStaticText* m_scopeStatic;
		wxTextCtrl* m_scopeText;
		wxGridBagSizer* m_gridSizer;
		wxSizer* m_triggerSizer;
	};

	class CommandPanel : public wxPanel {
	public:
		CommandPanel(wxWindow* parent, PListHandler& plistHandler);
		void SetCommand(unsigned int bundleId, unsigned int cmdId);
		bool IsModified() const;
		bool Save();
	private:
		void OnInputChoice(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()

		unsigned int m_bundleId;
		unsigned int m_cmdId;
		PListHandler& m_plistHandler;
		wxTextCtrl* m_cmdCtrl;
		SelectorPanel* m_selectorPanel;
		wxChoice* m_saveChoice;
		wxChoice* m_envChoice;
		wxChoice* m_inputChoice;
		wxChoice* m_fallbackChoice;
		wxChoice* m_outputChoice;
		wxStaticText* m_orStatic;
		wxSizer* m_inputSizer;

		int m_saveSel;
		int m_envSel;
		int m_inputSel;
		int m_fallbackSel;
		int m_outputSel;
	};

	class SnippetPanel : public wxPanel {
	public:
		SnippetPanel(wxWindow* parent, PListHandler& plistHandler);
		void SetSnippet(unsigned int bundleId, unsigned int snippetId);
		bool IsModified() const;
		bool Save();
	private:
		unsigned int m_bundleId;
		unsigned int m_snippetId;
		PListHandler& m_plistHandler;
		wxChoice* m_envChoice;
		wxTextCtrl* m_snippetCtrl;
		SelectorPanel* m_selectorPanel;

		int m_envSel;
	};

	class DragPanel : public wxPanel {
	public:
		DragPanel(wxWindow* parent, PListHandler& plistHandler);
		void SetDragCommand(unsigned int bundleId, unsigned int cmdId);
		bool IsModified() const;
		bool Save();
	private:
		unsigned int m_bundleId;
		unsigned int m_cmdId;
		PListHandler& m_plistHandler;
		wxChoice* m_envChoice;
		wxTextCtrl* m_cmdCtrl;
		wxTextCtrl* m_scopeText;
		wxTextCtrl* m_extText;

		int m_envSel;
	};

	class PrefsPanel : public wxPanel {
	public:
		PrefsPanel(wxWindow* parent, PListHandler& plistHandler);
		void SetPref(unsigned int bundleId, unsigned int cmdId);
		void SetLanguage(unsigned int bundleId, unsigned int cmdId);
		bool IsModified() const;
		SaveResult Save();
	private:
		unsigned int m_bundleId;
		unsigned int m_prefId;
		BundleItemType m_mode;
		PListHandler& m_plistHandler;
		wxTextCtrl* m_cmdCtrl;
		SelectorPanel* m_selectorPanel;
	};

	class MenuPanel : public wxPanel {
	public:
		MenuPanel(BundleEditor& parent, PListHandler& plistHandler);
		void SetBundle(unsigned int bundleId);
		void UpdateBundleName(const wxString& name);
		unsigned int GetCurrentBundle() {return m_bundleId;};
		bool IsModified() const {return m_isModified;};
		bool Save();

		bool IsInMenu(BundleItemType type, unsigned int itemId) const;
		bool DnDHitTest(const wxPoint& point);
		bool DoDrop(const wxPoint& point, BundleItemType type, unsigned int itemId);

	private:
		class MenuItemData : public wxTreeItemData {
		public:
			MenuItemData(BundleItemType type, unsigned int itemId=0, const wxString& uuid=wxEmptyString);

			const BundleItemType m_type;
			const wxString m_uuid;
			const unsigned int m_itemId;
		};

		class BundleAction {
		public:
			BundleAction() {};
			BundleAction(BundleItemType type, unsigned int ndx)
				: m_type(type), m_index(ndx), m_inMenu(false) {};
			void SetInMenu(bool inMenu) {m_inMenu = inMenu;};
			BundleItemType m_type;
			unsigned int m_index;
			bool m_inMenu;
		};

		void ParseMenu(const PListArray& itemsArray, const PListDict& submenuDict, const wxTreeItemId& parentItem);
		bool IsParentOf(const wxTreeItemId parent, const wxTreeItemId child) const;

		// Item handling
		void RemoveItem(const wxTreeItemId item, bool deep);
		void DeleteSubDir(const char* uuid);
		wxTreeItemId InsertItem(const wxTreeItemId& item, const wxString& name, const char* uuid, unsigned int imageId, MenuItemData* menuData=NULL);

		PListDict GetEditablePlist();

		void OnTreeItemActivated(wxTreeEvent& event);
		void OnTreeItemSelected(wxTreeEvent& event);
		void OnTreeItemCollapsing(wxTreeEvent& event);
		void OnTreeItemBeginEdit(wxTreeEvent& event);
		void OnTreeItemEndEdit(wxTreeEvent& event);
		void OnTreeItemBeginDrag(wxTreeEvent& event);
		void OnTreeItemEndDrag(wxTreeEvent& event);
		void OnRemoveButton(wxCommandEvent& event);
		void OnSeparatorButton(wxCommandEvent& event);
		void OnSubDirButton(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()

		// Member variables
		BundleEditor& m_parentDlg;
		unsigned int m_bundleId;
		PListHandler& m_plistHandler;
		wxImageList m_imageList;
		map<wxString, BundleAction> m_actionMap;
		wxTreeItemId m_draggedItem;
		wxTreeItemId m_dropItem;
		bool m_isModified;
		bool m_itemsModified;

		// Controls
		wxTreeCtrl* m_menuTree;
		wxButton* m_removeButton;
	};

	// Member variables
	TmSyntaxHandler& m_syntaxHandler;
	PListHandler& m_plistHandler;
	wxImageList m_imageList;
	BundleItemType m_currentPanel;
	bool m_isModified;
	bool m_needFullReload;
	bool m_isActive;

	// DnD state
	wxDragImage* m_dragImage;
	wxTreeItemId m_draggedItem;
	bool m_canDragToBundle;

	// Member ctrls
	wxTreeCtrl* m_bundleTree;
	wxStaticBoxSizer* m_itemBorder;
	wxButton* m_bundlePlus;

	// Item Panels
	MenuPanel* m_bundlePanel;
	SnippetPanel* m_snippetPanel;
	CommandPanel* m_commandPanel;
	DragPanel* m_dragPanel;
	PrefsPanel* m_prefPanel;

	static const char* s_snippetsyntax;
	static const char* s_commandsyntax;
};

#endif // __BUNDLEEDITOR_H__
