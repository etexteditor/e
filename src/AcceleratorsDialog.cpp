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

#include "AcceleratorsDialog.h"
#include "Accelerators.h"
#include "EditorFrame.h"
#include "BundleMenu.h"
#include "tmAction.h"
#include "tmBundle.h"

#include <wx/wx.h>
#include <wx/file.h>

enum {
	CTRL_ACCELERATORS_BROWSER
};


BEGIN_EVENT_TABLE(AcceleratorsDialog, wxDialog)
	EVT_BUTTON(wxID_OK, AcceleratorsDialog::OnSave)
END_EVENT_TABLE()

AcceleratorsDialog::AcceleratorsDialog(EditorFrame *parent):
	m_editorFrame(parent), m_accelerators(parent->GetAccelerators()),
	wxDialog ((wxWindow*)parent, wxID_ANY, wxEmptyString, wxDefaultPosition)
{
	SetTitle(_("Customize Keyboard Shortcuts"));

	m_treeView = new wxTreeCtrl(this, CTRL_ACCELERATORS_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_ROW_LINES|wxTR_SINGLE);

	// Create Layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_treeView, 1, wxEXPAND);
		mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	ParseMenu();

	SetSizer(mainSizer);
	SetSize(500, 580);
	Centre();
}

void AcceleratorsDialog::ParseMenu() {
	wxMenuBar* menuBar = m_editorFrame->GetMenuBar();
	if(!menuBar) return;

	wxTreeItemId root = m_treeView->AddRoot(wxT("Shortcuts"));
	
	for(unsigned int c = 0; c < menuBar->GetMenuCount(); c++) {
		ParseMenu(menuBar->GetMenu(c), menuBar->GetMenuLabel(c), root);
	}

	m_treeView->Expand(root);
}

void AcceleratorsDialog::ParseMenu(wxMenu* menu, wxString label, wxTreeItemId parent) {
	label.Replace(wxT("&"), wxT(""));
	if(label == wxT("Recent Files") || label == wxT("Recent Projects") || label == wxT("Tabs")) return;

	wxTreeItemId id = m_treeView->AppendItem(parent, label);

	wxMenuItemList& items = menu->GetMenuItems();
	for(unsigned int c = 0; c < items.size(); c++) {
		wxMenuItem* item = items[c];
		if(item->IsSeparator()) continue;

		if(item->IsSubMenu()) {
			ParseMenu(item->GetSubMenu(), item->GetLabel(), id);
		} else {
			ParseMenu(item, id);
		}
	}
}

void AcceleratorsDialog::ParseMenu(wxMenuItem* item, wxTreeItemId parent) {
	if(item->IsSeparator()) return;
	map<wxString, wxString>::iterator iterator;

	wxString label, accel = wxT(""), customAccel = wxT("");

	const type_info& typeInfo = typeid(*item);
	if(typeInfo == typeid(wxMenuItem)) {
		// read the data from the menu item
		wxString text = item->GetText();
		int tab = text.Find('\t');
		if(tab == wxNOT_FOUND) {
			label = text;
		} else {
			label = text.Mid(0, tab);
			accel = text.Mid(tab+1);
		}

		iterator = m_accelerators->m_defaultBindings.find(label);
		if(iterator != m_accelerators->m_defaultBindings.end()) {
			accel = iterator->second;
		}
	} else {
		BundleMenuItem* bItem = (BundleMenuItem*) item;

		label = bItem->GetLabel();
		accel = bItem->GetAction().key.shortcut;

		if(accel.empty()) return;
	}

	// now check if there is a custom shortcut
	iterator = m_accelerators->m_customBindings.find(normalize(label));
	if(iterator != m_accelerators->m_customBindings.end()) {
		customAccel = iterator->second;
	}

	label.Replace(wxT("&"), wxT(""));


	m_treeView->AppendItem(parent, label);
}

void AcceleratorsDialog::OnSave(wxCommandEvent& WXUNUSED(event)) {
	/*m_accelerators->SaveCustomShortcuts(jsonString);
	m_accelerators->ReadCustomShortcuts();
	m_accelerators->ParseMenu();*/

	this->Close();
}