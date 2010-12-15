#ifndef __ACCELERATORS_H__
#define __ACCELERATORS_H__

#include <vector>
#include <map>
#include "wx/wx.h"

class EditorFrame;
class tmAction;
class wxJSONValue;
int makeHash(wxString& accel);
wxString normalize(wxString str);

class KeyBinding {
public:
	KeyBinding(wxMenuItem* menuItem, wxString& accel) :
	  accel(accel), id(menuItem->GetId()) {}

	KeyBinding(wxString accel, int id) : 
	  accel(accel), id(id) {}

	wxString accel, label, finalKey;
	int id;
};

class KeyChord {
public:
	KeyChord(wxString chord) : 
	  key(chord) {}

	wxString key;
	std::map<int, KeyBinding*> bindings;
};

class BundleKeyChord {
public:
	BundleKeyChord(int hash, wxString& chord) : 
	  key(chord), hash(hash) {};

	wxString key;
	std::map<int, bool> bindings;
	int hash;
};

class Accelerators {
public:
	Accelerators(EditorFrame* editorFrame);

	void DefineBinding(wxString accel, int id);

	void ReadCustomShortcuts();
	void SaveCustomShortcuts(wxJSONValue& jsonRoot);

	void ParseMenu();
	void ParseMenu(wxMenu* menu);
	void ParseMenu(wxMenuItem* menuItem);
	void InsertBinding(wxMenuItem* item, wxString& accel);
	void InsertBinding(KeyBinding* binding);

	void ParseBundlesMenu(wxMenu* menu);
	void ParseBundlesMenu(wxMenuItem* item);

	bool HandleKeyEvent(wxKeyEvent& event);
	bool MatchMenus(int hash);
	
	bool MatchBundle(int code, int flags, const tmAction* x);
	bool BundlesParsed(int code, int flags);
	void ParseBundles(const tmAction* x);
	void ParseBundleForHash(const tmAction* x, int& outChordHash, int& outFinalHash, wxString& outChordString);
	void Reset();
	void ResetChords();
	bool WasChordActivated();

	wxString StatusBarText();

	std::map<wxString, wxString> m_customBindings;
	std::map<wxString, wxString> m_defaultBindings;

private:
	EditorFrame* m_editorFrame;
	std::map<int, KeyChord*> m_chords;
	std::map<int, KeyBinding*> m_bindings;
	std::vector<KeyBinding*> m_definedBindings;

	std::map<int, BundleKeyChord*> m_bundleChords;
	std::map<int, bool> m_bundleBindings;

	KeyChord* m_activeChord;
	BundleKeyChord* m_activeBundleChord;

	bool m_chordActivated;
	bool m_actionReturned;
	bool m_searchBundleBindings;
	bool m_searchBundleChords;
	bool m_needDefault;
};

#endif
