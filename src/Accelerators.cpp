#include <vector>
#include <map>

#include "wx/wx.h"
#include <wx/wfstream.h>
#include <wx/regex.h>
#include "Strings.h"
#include "jsonreader.h"
#include "jsonwriter.h"

#include "Accelerators.h"
#include "EditorFrame.h"
#include "BundleMenu.h"
#include "tmAction.h"
#include "tmBundle.h"

#include "AcceleratorsDialog.h"

wxString normalize(wxString str) {
	str.Replace(wxT("&"), wxT(""));
	return str.Trim().MakeLower();
}

bool IsChord(wxString& accel) {
	return accel.Trim().Find(' ') != wxNOT_FOUND;
}

bool shouldIgnore(int code) {	
	switch(code) {
		case WXK_CONTROL:
		case WXK_ALT:
		case WXK_SHIFT:
		case WXK_WINDOWS_LEFT:
		case WXK_WINDOWS_RIGHT:
			return true;
	}

	return false;
}

/**
 * For constant time access, each keystroke is mapped to an int.
 * The first 8 bits are used to store the modifiers.
 * The remaining 24 bits are used to store the actual wxKeyCode of the key to press.
 */
int makeHash(wxString& accel) {
	wxAcceleratorEntry entry;
	entry.FromString(wxT("\t")+accel);
	int code = entry.GetKeyCode();
	int flags = entry.GetFlags();
	
#ifdef __WXMSW__
	// ParseAccel does not support windows key
	if(accel.Lower().Contains(wxT("win"))) flags |= 0x0008;
#endif

	//Win is not recognized as a modifier, so wxwidgets will not auto-capitalize the key codes for us
	code = wxToupper(code);

	wxLogDebug(wxT("Hash for %s: %d %d %d"), accel.c_str(), ((flags << 24) | code), flags, code);
	return (flags << 24) | code;
}

Accelerators::Accelerators(EditorFrame* editorFrame) : 
	m_editorFrame(editorFrame), m_activeChord(NULL), m_activeBundleChord(NULL) {
	ReadCustomShortcuts();
	Reset();
}

void Accelerators::DefineBinding(wxString accel, int id) {
	m_definedBindings.push_back(new KeyBinding(accel, id));
}

/**
 * Traverses the menubar.
 * For each menu item, it grabs the binding from the menu
 * and it checks if there is a custom binding for that menu item.
 */
void Accelerators::ParseMenu() {
	std::map<int, KeyChord*>::iterator iterator;
	for(iterator = m_chords.begin(); iterator != m_chords.end(); iterator++) {
		KeyChord* chord = iterator->second;
		std::map<int, KeyBinding*>::iterator iterator2;
		for(iterator2 = chord->bindings.begin(); iterator2 != chord->bindings.end(); iterator2++) {
			KeyBinding* binding = iterator2->second;
			delete binding;
		}
		delete chord;
	}
	
	std::map<int, KeyBinding*>::iterator iterator2;
	for(iterator2 = m_bindings.begin(); iterator2 != m_bindings.end(); iterator2++) {
		KeyBinding* binding = iterator2->second;
		delete binding;
	}

	m_chords.clear();
	m_bindings.clear();

	for(unsigned int c = 0; c < m_definedBindings.size(); c++) {
		// If the original binding is inserted, and it also appears on the menu, it will be deleted when the menu is reparsed and lead to a later segfault
		InsertBinding(new KeyBinding(m_definedBindings[c]->accel, m_definedBindings[c]->id));
	}

	m_needDefault = m_defaultBindings.size() == 0;

	wxMenuBar* menuBar = m_editorFrame->GetMenuBar();
	if(!menuBar) return;

	const unsigned int bundles = menuBar->FindMenu(_("&Bundles"));
	for(unsigned int c = 0; c < menuBar->GetMenuCount(); c++) {
		if(c == bundles) ParseBundlesMenu(menuBar->GetMenu(c));
		else ParseMenu(menuBar->GetMenu(c));
	}
}

void Accelerators::ParseMenu(wxMenu* menu) {
	wxMenuItemList& items = menu->GetMenuItems();
	for(unsigned int c = 0; c < items.size(); c++) {
		wxMenuItem* item = items[c];
		if(item->IsSubMenu()) {
			ParseMenu(item->GetSubMenu());
		} else {
			ParseMenu(item);
		}
	}
}

void Accelerators::ParseMenu(wxMenuItem* item) {
	if(item->IsSeparator()) return;

	// read the data from the menu item
	wxString text = item->GetText();
	int tab = text.Find('\t');
	wxString label, accel = wxT("");
	if(tab == wxNOT_FOUND) {
		label = text;
	} else {
		label = text.Mid(0, tab);
		accel = text.Mid(tab+1);

		if(m_needDefault) {
			m_defaultBindings[label] = accel;
		} else {
			// When reloading the accelerators, if the user removes a custom accelerator, it needs to reset to the original value
			map<wxString, wxString>::iterator iterator;
			iterator = m_defaultBindings.find(normalize(label));
			if(iterator != m_defaultBindings.end()) {
				accel = iterator->second;
			}

		}
	}

	// now check if there is a custom shortcut
	map<wxString, wxString>::iterator iterator;
	iterator = m_customBindings.find(normalize(label));
	if(iterator != m_customBindings.end()) {
		accel = iterator->second;

		text = label;
		if(accel.Len() > 0) {
			text += '\t';
			text += accel;
		}
		item->SetText(text);
	}
	accel = accel.Trim();

	InsertBinding(item, accel);
}

void Accelerators::InsertBinding(wxMenuItem* item, wxString& accel) {
	if(accel.Len() == 0) return;
	KeyBinding* binding = new KeyBinding(item, accel);
	InsertBinding(binding);
}

void Accelerators::InsertBinding(KeyBinding* binding) {
	wxString& accel = binding->accel;

	// now parse the accel for a chord
	int pos = accel.Find(wxT(" "));
	if(pos != wxNOT_FOUND) {

		wxString chordAccel = accel.Left(pos);
		int chordHash = makeHash(chordAccel);
		binding->finalKey = accel.Mid(pos+1).Trim();

		std::map<int, KeyChord*>::iterator iterator;
		iterator = m_chords.find(chordHash);
		KeyChord* chord;
		if(iterator != m_chords.end()) {
			chord = iterator->second;
		} else {
			chord = new KeyChord(chordAccel);
			m_chords.insert(pair<int, KeyChord*>(chordHash, chord));
		}

		// Prevent a memory leak if there are duplicate bindings
		int hash = makeHash(binding->finalKey);
		std::map<int, KeyBinding*>::iterator bindingIterator = chord->bindings.find(hash);
		if(bindingIterator != chord->bindings.end()) {
			delete bindingIterator->second;
		}
		chord->bindings[hash] = binding;
	} else {
		// Prevent a memory leak if there are duplicate bindings
		int hash = makeHash(accel);
		std::map<int, KeyBinding*>::iterator bindingIterator = m_bindings.find(hash);
		if(bindingIterator != m_bindings.end()) {
			delete bindingIterator->second;
		}

		m_bindings[hash] = binding;
	}
}

/**
 * Traverses the bundles menu.
 * For bundle menu items, their bindings are stored differently from other wxMenuItems
 */
void Accelerators::ParseBundlesMenu(wxMenu* menu) {
	wxMenuItemList& items = menu->GetMenuItems();
	for(unsigned int c = 0; c < items.size(); c++) {
		wxMenuItem* item = items[c];
		if(item->IsSubMenu()) {
			ParseBundlesMenu(item->GetSubMenu());
		} else {
			ParseBundlesMenu(item);
		}
	}
}

void Accelerators::ParseBundlesMenu(wxMenuItem* item) {
	// Non-bundle items can be processed like normal menu items
	const type_info& typeInfo = typeid(*item);
	if(typeInfo == typeid(wxMenuItem)) {
		ParseMenu(item);
		return;
	}

	BundleMenuItem* bItem = (BundleMenuItem*) item;

	// now check if there is a custom shortcut
	map<wxString, wxString>::iterator iterator;
	iterator = m_customBindings.find(normalize(bItem->GetLabel()));
	if(iterator != m_customBindings.end()) {
		wxString accel = iterator->second.Trim();
		bItem->SetCustomAccel(accel);
	} else {
		bItem->SetCustomAccel(wxT(""));
	}
}

void Accelerators::ReadCustomShortcuts() {
	wxString path = eGetSettings().GetSettingsDir() + wxT("accelerators.cfg");
	if (!wxFileExists(path)) return;

	wxFileInputStream fstream(path);
	if (!fstream.IsOk()) {
		wxLogDebug(wxT("Could not open keyboard settings file."));
		return;
	}

	// Parse the JSON contents
	wxJSONReader reader;
	wxJSONValue jsonRoot;
	const int numErrors = reader.Parse(fstream, &jsonRoot);
	if ( numErrors > 0 )  {
		// if there are errors in the JSON document, print the errors
		const wxArrayString& errors = reader.GetErrors();
		wxString msg = _("Invalid JSON in settings:\n\n") + wxJoin(errors, wxT('\n'), '\0');
		wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK);
		return;
	}

	if(!jsonRoot.HasMember(wxT("bindings"))) return;
	wxJSONValue bindings = jsonRoot[wxT("bindings")];

	m_customBindings.clear();
	if (bindings.IsObject()) {
		const wxArrayString keys = bindings.GetMemberNames();
		for(unsigned int c = 0; c < keys.size(); c++) {
			const wxString key = keys[c];
			m_customBindings[normalize(key)] = bindings[key].AsString();
		}
	}
}

void Accelerators::SaveCustomShortcuts(wxJSONValue& bindings) {
	wxJSONValue root;
	root[wxT("bindings")] = bindings;

	wxString path = eGetSettings().GetSettingsDir() + wxT("accelerators.cfg");
	wxFileOutputStream fstream(path);
	if (!fstream.IsOk()) {
		wxMessageBox(_("Could not open accelerators settings file."), _("File error"), wxICON_ERROR|wxOK);
		return;
	}

	// Write settings
	wxJSONWriter writer(wxJSONWRITER_STYLED);
	writer.Write(root, fstream);
}

bool Accelerators::HandleKeyEvent(wxKeyEvent& event) {
	int hash = event.GetModifiers();

#ifdef __WXMSW__
	// GetModifiers does not report the windows keys
	if (::GetKeyState(VK_LWIN) < 0) hash |= 0x0008; // wxMOD_META (Left Windows key)
	if (::GetKeyState(VK_RWIN) < 0) hash |= 0x0008; // wxMOD_META (Right Windows key)
#endif

	if(shouldIgnore(event.GetKeyCode())) return true;

	wxLogDebug(wxT("Hash: %d %d %d"), ((hash << 24) | event.GetKeyCode()), hash, event.GetKeyCode());
	hash = (hash << 24) | event.GetKeyCode();

	return MatchMenus(hash);
}

/**
 * Takes a bundle and finds the custom key binding for the bundle.
 * Returns the hash of the two key presses for the bundle.
 */
void Accelerators::ParseBundleForHash(const tmAction* x, int& outChordHash, int& outFinalHash, wxString& outChordString) {
	outChordHash = -1;
	outFinalHash = -1;

	int actionHash = (x->key.modifiers << 24) | x->key.keyCode;

	// now check if there is a custom shortcut
	std::map<wxString, wxString>::iterator iterator;
	iterator = m_customBindings.find(normalize(x->name));
	wxString customAccel;

	if(iterator != m_customBindings.end()) {
		customAccel = iterator->second.Trim();
	}

	if(customAccel.empty()) {
		outFinalHash = actionHash;
		return;
	}

	int pos = customAccel.Find(wxT(" "));
	if(pos != wxNOT_FOUND) {
		// Custom shortcut is a chord

		// First, find or create the chord object
		wxString chordAccel = customAccel.Left(pos);
		wxString finalAccel = customAccel.Mid(pos+1);

		outChordHash = makeHash(chordAccel);
		outFinalHash = makeHash(finalAccel);
		outChordString = chordAccel;
	} else {
		outFinalHash = makeHash(customAccel);
	}
}

/**
 * This builds a hashtable of all of the bundle key bindings.
 * This is done once to speed up and enhance the accuracy of the search for bundles that match later.
 * This method is called once for each bundle item.
 */
void Accelerators::ParseBundles(const tmAction* x) {
	int chordHash, finalHash;
	wxString chordString;
	ParseBundleForHash(x, chordHash, finalHash, chordString);

	if(chordHash > -1) {
		std::map<int, BundleKeyChord*>::iterator iterator;
		iterator = m_bundleChords.find(chordHash);
		BundleKeyChord* chord;
		if(iterator != m_bundleChords.end()) {
			chord = iterator->second;
		} else {
			chord = new BundleKeyChord(chordHash, chordString);
			m_bundleChords[chordHash] = chord;
		}

		chord->bindings[finalHash] = true;
	} else {
		m_bundleBindings[finalHash] = true;
	}
}

/**
 * This method is called when ParseBundles is finished with all the bundles.
 * It activates a chord / sees if any bundle items will match / determines what set of bindings to search later on.
 */
bool Accelerators::BundlesParsed(int code, int flags) {
	if(shouldIgnore(code)) return true;
	int hash = (flags << 24) | code;


	if(m_activeBundleChord) { // A chord for a bundle was previously activated
		m_searchBundleChords = true;
		bool ret = m_activeBundleChord->bindings.find(hash) != m_activeBundleChord->bindings.end();
		m_actionReturned = ret;
		return ret;
	} else if(m_activeChord) { // A chord was previously activated, but no bundle uses it
		return false;
	} else {
		// No chord is activated yet
		std::map<int, BundleKeyChord*>::iterator bundleChords = m_bundleChords.find(hash);
		std::map<int, KeyChord*>::iterator chords = m_chords.find(hash);
		std::map<int, bool>::iterator bundleBindings = m_bundleBindings.find(hash);

		if(bundleChords != m_bundleChords.end()) {
			m_activeBundleChord = bundleChords->second;
		}
		if(chords != m_chords.end()) {
			m_activeChord = chords->second;
		}

		if(m_activeBundleChord != NULL || m_activeChord != NULL) {
			m_chordActivated = true;
			return true;
		}

		if(bundleBindings != m_bundleBindings.end()) {
			m_searchBundleBindings = true;
			m_actionReturned = true;
			return true;
		}

		return false;
	}
}

/**
 * This method is called once for each bundle.
 * If it returns true, the bundle will be run.
 * It uses the results of BundlesParsed to check if this bundle matches the set of pressed keys.
 */
bool Accelerators::MatchBundle(int code, int flags, const tmAction* x) {
	if(shouldIgnore(code)) return false;
	int hash = (flags << 24) | code;

	int chordHash, finalHash;
	wxString chordString;
	ParseBundleForHash(x, chordHash, finalHash, chordString);

	if(chordHash > -1) {
		if(!m_searchBundleChords) return false;
		if(finalHash != hash) return false;

		std::map<int, BundleKeyChord*>::iterator iterator;
		iterator = m_bundleChords.find(chordHash);

		if(iterator == m_bundleChords.end()) return false;
		BundleKeyChord* chord = iterator->second;

		return chord->bindings.find(finalHash) != chord->bindings.end();
	} else {
		if(!m_searchBundleBindings) return false;
		return hash == finalHash;
	}
}

void Accelerators::Reset() {
	m_chordActivated = false;
	m_actionReturned = false;
	m_searchBundleBindings = false;
	m_searchBundleChords = false;

	std::map<int, BundleKeyChord*>::iterator iterator;
	for(iterator = m_bundleChords.begin(); iterator != m_bundleChords.end(); iterator++) {
		BundleKeyChord* b = iterator->second;
		delete b;
	}
	m_bundleChords.clear();
	m_bundleBindings.clear();
}

/**
 * This method causes execution to stop after the bundles IF a chord was activated.
 * This is important, because of how the code for handling bundles/menus is separated between two different areas.
 * If removed, 
 */
bool Accelerators::WasChordActivated() {
	bool ret = m_chordActivated;

	// If we get into the bundles and we didn't activate a chord, then clear it out
	if(m_actionReturned) {
		m_activeChord = NULL;
		m_activeBundleChord = NULL;
	}

	Reset();

	return ret;
}

void RunEvent(int id, EditorFrame* editorFrame) {
	wxCommandEvent event2(wxEVT_COMMAND_MENU_SELECTED);
	event2.SetEventObject(editorFrame);
	event2.SetId(id);
	event2.SetInt(id);
	editorFrame->GetEventHandler()->ProcessEvent(event2);
}

/**
 * Finds the menu item that matches the given key bidning hash and runs its event.
 */
bool Accelerators::MatchMenus(int hash) {
	if(m_activeChord) {
		std::map<int, KeyBinding*>::iterator iterator;
		iterator = m_activeChord->bindings.find(hash);
		if(iterator != m_activeChord->bindings.end()) {
			KeyBinding* binding = iterator->second;
			RunEvent(binding->id, m_editorFrame);
			ResetChords();
		}

		// If we have an active chord and it matches nothing or something, don't let it keep processing either way
		ResetChords();
		return true;
	} else {
		std::map<int, KeyChord*>::iterator iterator;
		iterator = m_chords.find(hash);
		if(iterator != m_chords.end()) {
			m_activeChord = iterator->second;
			return true;
		}
		
		std::map<int, KeyBinding*>::iterator iterator2;
		iterator2 = m_bindings.find(hash);
		if(iterator2 != m_bindings.end()) {
			KeyBinding* binding = iterator2->second;
			RunEvent(binding->id, m_editorFrame);
			ResetChords();
			return true;
		}
	}

	ResetChords();
	return false;
}

void Accelerators::ResetChords() {
	m_activeBundleChord = NULL;
	m_activeChord = NULL;
}

wxString Accelerators::StatusBarText() {
	if(m_activeChord) {
		return wxT("Chord ") + (m_activeChord->key) + wxT(" active");
	} else if(m_activeBundleChord) {
		return wxT("Chord ") + (m_activeBundleChord->key) + wxT(" active");
	}

	return wxT("");
}
