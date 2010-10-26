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
#include "eBrowser.h"
#include "EditorFrame.h"
#include "BundleMenu.h"
#include "tmAction.h"
#include "tmBundle.h"
#include "AcceleratorsDialogHtml.h"

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/uri.h>

enum {
	CTRL_ACCELERATORS_BROWSER
};


BEGIN_EVENT_TABLE(AcceleratorsDialog, wxDialog)
	EVT_HTMLWND_BEFORE_LOAD(CTRL_ACCELERATORS_BROWSER, AcceleratorsDialog::OnBeforeLoad)
END_EVENT_TABLE()

/**
 * This class lists every single menu item in e and allows the user to change the key binding for almost all of them.
 * The gui was going to be too crazy to quickly create in wxwidgets, so I stuck with what I know.
 * The class creates an html page with all of the menu items, and displays it.  The user will hardly know they are using a website.
 * When they click the save button, the bindings are transfered via json in the url.
 */
AcceleratorsDialog::AcceleratorsDialog(EditorFrame *parent):
	m_editorFrame(parent), m_accelerators(parent->GetAccelerators()),
	wxDialog ((wxWindow*)parent, wxID_ANY, wxEmptyString, wxDefaultPosition)
{
	SetTitle(_("Customize Keyboard Shortcuts"));

	m_browser = NewBrowser(this, CTRL_ACCELERATORS_BROWSER);

	// Create Layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_browser->GetWindow(), 1, wxEXPAND);

	// The string has to be created on the heap.
	// Otherwise, there would be 4 copies of the string on the stack at one time and it causes a segfault
	wxString* html = GetHtml();
	//wxFile f(wxT("accel.html"), wxFile::write);
	//f.Write(*html);
	m_browser->LoadString(*html, false);
	delete html;

	SetSizer(mainSizer);
	SetSize(750, 580);
	Centre();
}

wxString* AcceleratorsDialog::GetHtml() {
	ParseMenu();

	int size = 0;
	for(unsigned int c = 0; c < m_htmlBits.GetCount(); c++) {
		size += m_htmlBits[c].Len();
	}

	wxString* output = new wxString();
	output->Alloc(size);

	for(unsigned int c = 0; c < m_htmlBits.GetCount(); c++) {
		output->Append(m_htmlBits[c]);
	}

	wxString* html = AcceleratorsDialogHtml();
	html->Replace(wxT("$1"), *output);

	delete output;
	return html;
}

void AcceleratorsDialog::ParseMenu() {
	m_htmlBits.clear();
	m_id = 0;
	
	m_htmlBits.Add(wxT("<table id='mainTable' cellspacing='0' cellpadding='0'> \
			<tr id='header'> \
				<th>Menu Item</th> \
				<th>Default Binding</th> \
				<th>Custom Binding</th> \
			</tr>"));

	wxMenuBar* menuBar = m_editorFrame->GetMenuBar();
	if(!menuBar) return;
	
	for(unsigned int c = 0; c < menuBar->GetMenuCount(); c++) {
		ParseMenu(menuBar->GetMenu(c), c, menuBar->GetMenuLabel(c), 0, 0);
	}

	m_htmlBits.Add(wxT("</table>"));
}

wxString evenOdd(int index) {
	return (index % 2 == 0 ? wxT("odd") : wxT("even"));
}

wxString& encode(wxString& str) {
	str.Replace(wxT(">"), wxT("&gt;"));
	str.Replace(wxT("<"), wxT("&lt;"));
	str.Replace(wxT("&"), wxT("&amp;"));
	return str;
}

void AcceleratorsDialog::ParseMenu(wxMenu* menu, int index, wxString label, int level, int parent) {
	label.Replace(wxT("&"), wxT(""));
	if(label == wxT("Recent Files") || label == wxT("Recent Projects") || label == wxT("Tabs")) return;

	int id = ++m_id;

	wxString menuClass = wxT(" menu");
	menuClass << parent;
	
	wxString menuId = wxT("menu");
	menuId << id;

	wxString levelClass = wxT(" level");
	levelClass << level;

	m_htmlBits.Add(wxT("<tr class='")+evenOdd(index)+menuClass+wxT("'>"));
		m_htmlBits.Add(wxT("<td colspan='3' class='menuLabel")+levelClass+wxT("'>"));
			m_htmlBits.Add(wxT("<a href='javascript:void(0);' class='expand' id='")+menuId+wxT("'>&rarr;</a>"));
			m_htmlBits.Add(encode(label));
		m_htmlBits.Add(wxT("</td>"));
	m_htmlBits.Add(wxT("</tr>"));

	wxMenuItemList& items = menu->GetMenuItems();
	int ix = 0;
	for(unsigned int c = 0; c < items.size(); c++) {
		wxMenuItem* item = items[c];
		if(item->IsSeparator()) continue;

		if(item->IsSubMenu()) {
			ParseMenu(item->GetSubMenu(), ix, item->GetLabel(), level+1, id);
		} else {
			ParseMenu(item, ix, level+1, id);
		}

		ix++;
	}
}

void AcceleratorsDialog::ParseMenu(wxMenuItem* item, int index, int level, int parent) {
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
	accel = encode(accel.Trim());
	if(accel.Len() == 0) accel = wxT("&nbsp;");
	customAccel = customAccel.Trim();


	wxString menuClass = wxT(" menu");
	menuClass << parent;

	wxString levelClass = wxT(" level");
	levelClass << level;

	m_htmlBits.Add(wxT("<tr class='")+evenOdd(index)+menuClass+wxT("'>"));
		m_htmlBits.Add(wxT("<td class='menuItemLabel")+levelClass+wxT("'>") + encode(label) + wxT("</td>"));
		m_htmlBits.Add(wxT("<td class='defaultBinding'>") + accel + wxT("</td>"));
		m_htmlBits.Add(wxT("<td class='newBinding'><input class='textfield' type='text' name='") + encode(label) + wxT("' value='") + encode(customAccel) + wxT("' size='25' /></td>"));
	m_htmlBits.Add(wxT("</tr>"));

}

void AcceleratorsDialog::OnBeforeLoad(IHtmlWndBeforeLoadEvent& event) {
	wxString url = event.GetURL();
	if(url.StartsWith(wxT("about")) || url.StartsWith(wxT("javascript"))) return;

	wxString jsonString = wxURI::Unescape(url);
	jsonString.Replace(wxT("http://localhost/"), wxT(""));

	m_accelerators->SaveCustomShortcuts(jsonString);
	m_accelerators->ReadCustomShortcuts();
	m_accelerators->ParseMenu();

	event.Cancel();
	this->Close();
}