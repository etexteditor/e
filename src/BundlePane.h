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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/treectrl.h>
#include <wx/imaglist.h>
#include "plistHandler.h"

#include <map>


// Pre-definitions
class EditorFrame;
class ITmLoadBundles;
class wxDragImage;
class BundleItemData;

class BundlePane : public wxPanel {
public:
	BundlePane(EditorFrame& parent, ITmLoadBundles* syntaxHandler);
	void LoadBundles();

private:
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
	void ParseMenu(unsigned int bundleId, const PListArray& itemsArray, const PListDict& submenuDict, const wxTreeItemId& parentItem, const std::map<wxString, const BundleItemData*>& actionMap);
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
	ITmLoadBundles* m_syntaxHandler;
	PListHandler& m_plistHandler;

	// DnD state
	wxTreeItemId m_draggedItem;

	// Statics
	static const char* s_snippetsyntax;
	static const char* s_commandsyntax;
};

#endif // __BUNDLEPANE_H__
