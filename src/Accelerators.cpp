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

wxString normalize(wxString str) {
	str.Replace(wxT("&"), wxT(""));
	return str.Trim().MakeLower();
}

bool IsChord(wxString& accel) {
	return accel.Trim().Find(' ') != wxNOT_FOUND;
}

int makeHash(wxString& accel) {
	int code, flags;
	wxAcceleratorEntry::ParseAccel(wxT("\t")+accel, &flags, &code);
	
#ifdef __WXMSW__
	// ParseAccel does not support windows key
	if(accel.Lower().Contains(wxT("win"))) flags |= 0x0008;
#endif

	wxLogDebug(wxT("Hash for %s: %d %d %d"), accel, ((flags << 24) | code), flags, code);
	return (flags << 24) | code;
}

KeyBinding::KeyBinding(wxMenuItem* menuItem) : menuItem(menuItem) {
	label = menuItem->GetLabel();

	const wxString& text = menuItem->GetText();
	accel = text.Mid(text.Index('\t')+1);

	id = menuItem->GetId();
}

KeyChord::KeyChord(wxString chord) : key(chord) { }

Accelerators::Accelerators(EditorFrame* editorFrame) : 
	m_editorFrame(editorFrame), m_activeChord(NULL) {
	ReadCustomShortcuts();

	m_chordActivated = false;
	m_actionReturned = false;
}

void Accelerators::ParseMenu() {
	m_chords.clear();
	m_bindings.clear();

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

	KeyBinding* binding = new KeyBinding(item);

	// now parse the accel for a chord
	int pos = accel.Find(wxT(" "));
	if(accel.Len() > 0 && pos != wxNOT_FOUND) {

		wxString chordAccel = accel.Left(pos);
		binding->finalKey = accel.Mid(pos+1).Trim();

		std::map<int, KeyChord*>::iterator iterator;
		iterator = m_chords.find(makeHash(chordAccel));
		KeyChord* chord;
		if(iterator != m_chords.end()) {
			chord = iterator->second;
		} else {
			chord = new KeyChord(chordAccel);
			m_chords.insert(pair<int, KeyChord*>(makeHash(chordAccel), chord));
		}
		chord->bindings[makeHash(binding->finalKey)] = binding;
	} else {
		m_bindings[makeHash(accel)] = binding;
	}
}

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
		//InsertBinding(item, accel);
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

	wxArrayString keys = jsonRoot.GetMemberNames();
	for(unsigned int c = 0; c < keys.size(); c++) {
		wxString key = keys[c];
		m_customBindings[normalize(key)] = jsonRoot[key].AsString();
	}
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

	return HandleHash(hash);
}

/**
 * There are four cases:
 *  No custom binding
 *  Custom binding, non-chord
 *  Custom binding, chord
 *  Custom binding, chord, and chord was activated on previous keypress
 */
bool Accelerators::HandleBundle(int code, int flags, const tmAction* x) {
	if(shouldIgnore(code)) return false;
	int hash = (flags << 24) | code;
	int actionHash = (x->key.modifiers << 24) | x->key.keyCode;

	// now check if there is a custom shortcut
	std::map<wxString, wxString>::iterator iterator;
	iterator = m_customBindings.find(normalize(x->name));
	wxString customAccel;

	if(iterator != m_customBindings.end()) {
		customAccel = iterator->second.Trim();
	}

	if(customAccel.empty()) {
		// no way to match a non-chord if a chord is active
		if(m_activeChord != NULL) return false;

		std::map<int, KeyChord*>::iterator iterator;
		iterator = m_chords.find(actionHash);

		// If there is a chord that uses the same shortcut for the first keypress as this action, then the action will be ignored
		bool ret = iterator == m_chords.end() && hash == actionHash;
		m_actionReturned = ret;
		return ret;
	}

	int pos = customAccel.Find(wxT(" "));
	if(pos != wxNOT_FOUND) {
		// Custom shortcut is a chord

		// If we already activated the chord for this key press, then don't do it again
		// Otherwise the next bundle will think it should try to run the chord
		if(m_chordActivated) return false;

		// First, find or create the chord object
		wxString chordAccel = customAccel.Left(pos);
		wxString finalAccel = customAccel.Mid(pos+1);

		int chordHash = makeHash(chordAccel);
		int finalHash = makeHash(finalAccel);

		std::map<int, KeyChord*>::iterator iterator;
		iterator = m_chords.find(chordHash);
		KeyChord* chord;
		if(iterator != m_chords.end()) {
			chord = iterator->second;
		} else {
			chord = new KeyChord(chordAccel);
		}
		
		if(m_activeChord != NULL) {
			// Ctrl-X Ctrl-I and Ctrl-Z Ctrl-I can't match when you press Ctrl-I the second time
			if(makeHash(m_activeChord->key) != chordHash) return false;

			// chord is active, true if they are the same chord and second key press
			bool ret = hash == finalHash;
			m_actionReturned = m_actionReturned || ret;
			return ret;
		} else {
			if(chordHash != hash) return false;

			// chord is not active, so activate it
			m_activeChord = chord;
			m_chordActivated = true;
			return false;
		}
	} else {
		// no way to match a non-chord if a chord is active
		if(m_activeChord != NULL) return false;

		// Custom shortcut is just a regular key press
		// Trust that they will not use Ctrl-X and Ctrl-X Ctrl-C in the same file
		bool ret = hash == makeHash(customAccel);
		m_actionReturned = m_actionReturned || ret;
		return ret;
	}
}

bool Accelerators::WasChordActivated() {
	bool ret = m_chordActivated;

	// If we get into the bundles and we didn't activate a chord, then clear it out
	if(m_actionReturned) {
		m_activeChord = NULL;
	}

	m_chordActivated = false;
	m_actionReturned = false;

	return ret;
}

bool Accelerators::IsChord(int code, int flags) {
	int hash = (flags << 24) | code;
	std::map<int, KeyChord*>::iterator iterator;
	iterator = m_chords.find(hash);
	if(iterator != m_chords.end()) {
		m_activeChord = iterator->second;
		return true;
	}
	
	return false;
}

bool Accelerators::HandleHash(int hash) {
	if(m_activeChord) {
		std::map<int, KeyBinding*>::iterator iterator;
		iterator = m_activeChord->bindings.find(hash);
		if(iterator != m_activeChord->bindings.end()) {
			KeyBinding* binding = iterator->second;
			wxCommandEvent event2(wxEVT_COMMAND_MENU_SELECTED);
			event2.SetEventObject(m_editorFrame);
			event2.SetId(binding->id);
			event2.SetInt(binding->id);
			m_editorFrame->GetEventHandler()->ProcessEvent(event2);
			m_activeChord = NULL;
		}

		// If we have an active chord and it matches nothing or something, don't let it keep processing either way
		m_activeChord = NULL;
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
			wxCommandEvent event2(wxEVT_COMMAND_MENU_SELECTED);
			event2.SetEventObject(m_editorFrame);
			event2.SetId(binding->id);
			event2.SetInt(binding->id);
			m_editorFrame->GetEventHandler()->ProcessEvent(event2);
			m_activeChord = NULL;
			return true;
		}
	}

	m_activeChord = NULL;
	return false;
}

wxString Accelerators::StatusBarText() {
	if(m_activeChord) {
		return wxT("Chord ") + (m_activeChord->key) + wxT(" active");
	}

	return wxT("");
}