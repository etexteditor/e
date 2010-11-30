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
#include "ShortcutChordCtrl.h"
#include "jsonreader.h"
#include "jsonwriter.h"

#include <wx/wx.h>
#include <wx/file.h>
#include <map>

enum {
	CTRL_ACCELERATORS_BROWSER,
	CTRL_ACCELERATORS_CLEAR,
	CTRL_ACCELERATORS_KEY
};


BEGIN_EVENT_TABLE(AcceleratorsDialog, wxDialog)
	EVT_BUTTON(wxID_OK, AcceleratorsDialog::OnSave)
	EVT_TREE_ITEM_ACTIVATED(CTRL_ACCELERATORS_BROWSER, AcceleratorsDialog::OnClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(AcceleratorsDialog::AcceleratorDialog, wxDialog)
	EVT_BUTTON(wxID_OK, AcceleratorsDialog::AcceleratorDialog::OnSave)
	EVT_BUTTON(CTRL_ACCELERATORS_CLEAR, AcceleratorsDialog::AcceleratorDialog::OnClear)
END_EVENT_TABLE()

AcceleratorsDialog::AcceleratorsDialog(EditorFrame *parent):
	m_editorFrame(parent), m_accelerators(parent->GetAccelerators()),
	wxDialog ((wxWindow*)parent, wxID_ANY, wxEmptyString, wxDefaultPosition)
{
	SetTitle(_("Customize Keyboard Shortcuts"));

	m_treeView = new wxTreeCtrl(this, CTRL_ACCELERATORS_BROWSER, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_ROW_LINES|wxTR_SINGLE);

	// Create Layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Double click any node to edit its shortcut.  You may use chords such as Ctrl-X Ctrl-C.")), 0, wxALL, 5);
		mainSizer->Add(m_treeView, 1, wxEXPAND);
		mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	ParseMenu();

	SetSizer(mainSizer);
	SetSize(400, 500);
	Centre();
}

wxString AcceleratorsDialog::GetBinding(wxString& label) {
	map<wxString, wxString>::iterator iterator;
	iterator = m_accelerators->m_customBindings.find(normalize(label));
	if(iterator != m_accelerators->m_customBindings.end()) {
		return iterator->second;
	}

	return wxEmptyString;
}

wxString AcceleratorsDialog::GetDefaultBinding(wxString& label) {
	map<wxString, wxString>::iterator iterator;
	iterator = m_accelerators->m_defaultBindings.find(normalize(label));
	if(iterator != m_accelerators->m_defaultBindings.end()) {
		return iterator->second;
	}

	return wxEmptyString;
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

	wxString display = label;
	if(!customAccel.empty()) {
		display = wxString::Format(wxT("%s  ->  %s"), label, customAccel);
	} else if(!accel.empty()) {
		display = wxString::Format(wxT("%s  ->  %s"), label, accel);

	}

	m_treeView->AppendItem(parent, display);
	m_bindings.insert(pair<wxString, AcceleratorData>(label, AcceleratorData(label, accel, customAccel)));
}

void AcceleratorsDialog::OnSave(wxCommandEvent& WXUNUSED(event)) {
	std::map<wxString, wxString> existing;
	std::vector<wxString> conflicts;
	std::vector<wxTreeItemId> stack;
	stack.push_back(m_treeView->GetRootItem());
	while(stack.size() > 0) {
		wxTreeItemId id = stack[stack.size()-1];
		stack.pop_back();
		wxString label = m_treeView->GetItemText(id);
		if(label == wxT("Syntax") || label == wxT("Bundles")) continue;

		if(m_treeView->GetChildrenCount(id, false) > 0) {
			wxTreeItemIdValue cookie;
			wxTreeItemId first = m_treeView->GetFirstChild(id, cookie);
			while(first.IsOk()) {
				stack.push_back(first);
				first = m_treeView->GetNextChild(id, cookie);
			}
		} else {
			int separator = label.Find(wxT("  ->  "));
			if(separator <= 0) continue;

			wxString accel = label.Mid(separator+6);
			accel = accel.Trim();
			accel.Replace(wxT("+"), wxT("-"));
			
			if(existing.find(accel) != existing.end()) {
				wxString old = existing.find(accel)->second;
				if(!old.empty()) conflicts.push_back(old);
				conflicts.push_back(label);
				existing[accel] = wxT("");
			} else {
				existing[accel] = label;
			}
		}
	}

	if(conflicts.size() > 0) {
		wxString message;
		for(unsigned int c = 0; c < conflicts.size(); c++) {
			message << wxT("  ") << conflicts[c] << wxT("\n");
		}

		int result = wxMessageBox(wxString::Format(wxT("Some keystrokes are bound to multiple items:\n\n%s\nTo fix this, press Cancel.  To ignore, press Ok."), message), wxT("Warning"), wxOK|wxCANCEL);
		if(result == wxCANCEL) {
			return;
		}
	}


	wxJSONValue bindings;
	std::map<wxString, AcceleratorData>::iterator iterator;
	for(iterator = m_bindings.begin(); iterator != m_bindings.end(); iterator++) {
		wxString& customAccel = iterator->second.customAccel;
		if(!customAccel.empty()) {
			bindings[normalize(iterator->second.label)] = customAccel;
		}
	}

	m_accelerators->SaveCustomShortcuts(bindings);
	m_accelerators->ReadCustomShortcuts();
	m_accelerators->ParseMenu();

	this->Close();
}

void AcceleratorsDialog::OnClick(wxTreeEvent& event) {
	wxTreeItemId id = event.GetItem();
	wxString display = m_treeView->GetItemText(id);
	int separator = display.Find(wxT("  ->  "));
	wxString label = separator > 0 ? display.Left(separator) : display;
	std::map<wxString, AcceleratorData>::iterator iterator = m_bindings.find(label);
	if(iterator == m_bindings.end()) return;

	AcceleratorData& data = iterator->second;

	AcceleratorDialog dlg(data, this);
	dlg.ShowModal();
	
	if(dlg.save) {
		data.customAccel = dlg.GetBinding();
		
		wxString display = data.label;
		if(!data.customAccel.empty()) {
			display = wxString::Format(wxT("%s  ->  %s"), data.label, data.customAccel);
		} else if(!data.defaultAccel.empty()) {
			display = wxString::Format(wxT("%s  ->  %s"), data.label, data.defaultAccel);
		}

		m_treeView->SetItemText(id, display);
	}
}

AcceleratorsDialog::AcceleratorDialog::AcceleratorDialog(AcceleratorData& data, AcceleratorsDialog* parent) :
	wxDialog ((wxWindow*)parent, wxID_ANY, wxEmptyString, wxDefaultPosition), save(false)
{
	SetTitle(wxString::Format(wxT("Customize %s"), data.label));

	wxStaticText* defaultBinding = new wxStaticText(this, wxID_ANY, wxString::Format(wxT("Default Binding: %s"), data.defaultAccel));
	wxStaticText* customBinding = new wxStaticText(this, wxID_ANY, wxString::Format(wxT("Custom Binding: %s"), data.customAccel));
	m_chordCtrl = new ShortcutChordCtrl(this, CTRL_ACCELERATORS_KEY);
	m_chordCtrl->SetValue(data.customAccel);

	wxSizer* buttonSizer = CreateButtonSizer(wxOK|wxCANCEL);
	buttonSizer->Add(new wxButton(this, CTRL_ACCELERATORS_CLEAR, wxT("Clear")));

	// Create Layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(defaultBinding, 0, wxALL, 3);
		mainSizer->Add(customBinding, 0, wxALL, 3);
		mainSizer->Add(m_chordCtrl, 1, wxEXPAND|wxTOP|wxLEFT|wxRIGHT, 8);
		mainSizer->Add(buttonSizer, 0, wxEXPAND|wxALL, 5);

	SetSizerAndFit(mainSizer);
	Centre();

	m_chordCtrl->SetFocus();
}

wxString AcceleratorsDialog::AcceleratorDialog::GetBinding() {
	return m_chordCtrl->GetBinding();
}

void AcceleratorsDialog::AcceleratorDialog::OnSave(wxCommandEvent& WXUNUSED(event)) {
	save = true;
	this->Close();
}

void AcceleratorsDialog::AcceleratorDialog::OnClear(wxCommandEvent& WXUNUSED(event)) {
	m_chordCtrl->Clear();
}