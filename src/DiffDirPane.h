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

#ifndef __DIFFDIRPANE_H__
#define __DIFFDIRPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/treectrl.h>

WX_DECLARE_STRING_HASH_MAP( int, IconHash );

// Pre-declarations
class EditorFrame;

class DiffDirPane : public wxPanel {
public:
	DiffDirPane(EditorFrame& parent);

	void SetDiff(const wxString& path1, const wxString& path2);

private:
	bool DoDirDiff(const wxString& path1, const wxString& path2, const wxTreeItemId& parent);
	void AddSubDir(const wxString& path, const wxTreeItemId& parent, const wxColour& color);

	void AddFolderIcon();
	int GetFileIcon(const wxString& path);
	wxString GetLeftPath(const wxTreeItemId& item) const;
	wxString GetRightPath(const wxTreeItemId& item) const;
	void GetItemPath(const wxTreeItemId& item, wxString& path) const;

	// Event handlers
	void OnTreeGetToolTip(wxTreeEvent& evt);
	void OnTreeMenu(wxTreeEvent& evt);
	void OnTreeActivated(wxTreeEvent& evt);
	void OnTreeExpanding(wxTreeEvent& evt);
	void OnMenuCompare(wxCommandEvent& evt);
	void OnMenuOpen(wxCommandEvent& evt);
	void OnMenuOpenLeft(wxCommandEvent& evt);
	void OnMenuOpenRight(wxCommandEvent& evt);
	void OnMenuCopyToLeft(wxCommandEvent& evt);
	void OnMenuCopyToRight(wxCommandEvent& evt);
	void OnMenuDelLeft(wxCommandEvent& evt);
	void OnMenuDelRight(wxCommandEvent& evt);
	DECLARE_EVENT_TABLE();

	// Classes
	class SortTreeCtrl : public wxTreeCtrl {
		DECLARE_DYNAMIC_CLASS(wxTreeCtrl)
	public:
		SortTreeCtrl() {};
		SortTreeCtrl(wxWindow* parent, wxWindowID id);

		int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
	};

	// Ctrls
	SortTreeCtrl* m_tree;

	// Member variables
	wxString m_leftPath;
	wxString m_rightPath;
	wxColour m_insColor;
	wxColour m_delColor;
	wxColour m_modColor;
	EditorFrame& m_parentFrame;
	wxImageList m_imageList;
	IconHash m_iconHash;
	wxTreeItemId m_menuItem;
};

#endif //__DIFFDIRPANE_H__