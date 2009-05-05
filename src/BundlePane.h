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

#ifndef __BUNDLEPANE_H__
#define __BUNDLEPANE_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/treectrl.h>
#include <wx/imaglist.h>
#include "plistHandler.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif


// Pre-definitions
class EditorFrame;
class TmSyntaxHandler;
class wxDragImage;

class BundlePane : public wxPanel {
public:
	BundlePane(EditorFrame& parent, TmSyntaxHandler& syntaxHandler);
	void LoadBundles();

private:
	class BundleItemData : public wxTreeItemData {
	public:
		BundleItemData(unsigned int bundleId)
			: m_type(BUNDLE_BUNDLE), m_bundleId(bundleId), m_itemId(0), m_isMenuItem(false) {};
		BundleItemData(BundleItemType type, unsigned int bundleId, const wxString uuid=wxEmptyString)
			: m_type(type), m_bundleId(bundleId), m_itemId(0),
			  m_uuid(type == BUNDLE_SEPARATOR ? wxT("------------------------------------") : uuid),
			  m_isMenuItem(true) {};
		BundleItemData(BundleItemType type, unsigned int bundleId, unsigned int itemId, const wxString uuid=wxEmptyString)
			: m_type(type), m_bundleId(bundleId), m_itemId(itemId), m_uuid(uuid), m_isMenuItem(false) {};
		BundleItemData(const BundleItemData& bid)
			: m_type(bid.m_type), m_bundleId(bid.m_bundleId), m_itemId(bid.m_itemId), m_uuid(bid.m_uuid), m_isMenuItem(bid.m_isMenuItem) {};

		bool IsBundle() const {return m_type == BUNDLE_BUNDLE;};
		bool IsMenuItem() const {return m_isMenuItem;};
		int GetImageId() const {
			switch (m_type) {
				case BUNDLE_COMMAND:   return 1;
				case BUNDLE_SNIPPET:   return 2;
				case BUNDLE_DRAGCMD:   return 3;
				case BUNDLE_PREF:      return 4;
				case BUNDLE_LANGUAGE:  return 5;
				case BUNDLE_SUBDIR:    return 6;
				case BUNDLE_NONE:      return 7;
				case BUNDLE_SEPARATOR: return 8;
				default: wxASSERT(false); return 0;
			}
		};

		const BundleItemType m_type;
		const unsigned int m_bundleId;
		const unsigned int m_itemId;
		const wxString m_uuid;
		bool m_isMenuItem;
	};
	class SortTreeCtrl : public wxTreeCtrl {
	public:
		SortTreeCtrl() {};
		SortTreeCtrl(wxWindow* parent, wxWindowID id, long style)
			: wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style) {};
		int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

		DECLARE_DYNAMIC_CLASS_NO_COPY(SortTreeCtrl)
	};

	void NewItem(BundleItemType type);
	void DeleteItem();

	// Menu Handling
	PListDict GetEditableMenuPlist(unsigned int bundleId);
	void ParseMenu(unsigned int bundleId, const PListArray& itemsArray, const PListDict& submenuDict, const wxTreeItemId& parentItem, const map<wxString, const BundleItemData*>& actionMap);
	void RemoveMenuItem(const wxTreeItemId item, bool deep, PListDict& infoDict);
	void DeleteSubDir(const char* uuid, PListDict& infoDict);
	wxTreeItemId InsertMenuItem(const wxTreeItemId& dstItem, const wxString& name, BundleItemData* menuData, PListDict& infoDict);
	void CopySubItems(wxTreeItemId srcItem, wxTreeItemId dstItem);
	wxTreeItemId FindMenuItem(BundleItemType type, unsigned int bundleId, unsigned int itemId) const;
	wxTreeItemId FindItemInMenu(wxTreeItemId menuItem, BundleItemType type, unsigned int itemId) const;

	bool IsTreeItemParentOf(const wxTreeItemId parent, const wxTreeItemId child) const;
	unsigned int GetTreeItemIndex(const wxTreeItemId parent, const wxTreeItemId child) const;

	// Event Handlers;
	void OnTreeItemActivated(wxTreeEvent& event);
	void OnTreeItemSelected(wxTreeEvent& event);
	void OnTreeBeginDrag(wxTreeEvent& event);
	void OnTreeEndDrag(wxTreeEvent& event);
	void OnTreeKeyDown(wxTreeEvent& event);
	void OnTreeMenu(wxTreeEvent& event);
	void OnButtonPlus(wxCommandEvent& event);
	void OnButtonMinus(wxCommandEvent& event);
	void OnButtonManage(wxCommandEvent& event);
	void OnMenuNew(wxCommandEvent& event);
	void OnMenuExport(wxCommandEvent& event);
	void OnMenuOpenItem(wxCommandEvent& event);
	//void OnMouseMotion(wxMouseEvent& event);
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE()

	// Member Ctrl's
	wxTreeCtrl* m_bundleTree;
	wxButton* m_bundlePlus;
	wxButton* m_bundleMinus;

	// Member variables
	EditorFrame& m_parentFrame;
	wxImageList m_imageList;
	TmSyntaxHandler& m_syntaxHandler;
	PListHandler& m_plistHandler;

	// DnD state
	wxTreeItemId m_draggedItem;

	// Statics
	static const char* s_snippetsyntax;
	static const char* s_commandsyntax;
};

#endif // __BUNDLEPANE_H__

