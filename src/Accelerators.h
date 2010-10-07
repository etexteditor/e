#ifndef __ACCELERATORS_H__
#define __ACCELERATORS_H__

#include <vector>
#include <map>
#include "wx/wx.h"

class EditorFrame;
class tmAction;

class KeyBinding {
public:
	KeyBinding(wxMenuItem* menuItem);

	wxMenuItem* menuItem;
	wxString accel, label, finalKey;
	int id;
};

class KeyChord {
public:
	KeyChord(wxString chord);

	wxString key;
	std::map<int, KeyBinding*> bindings;
};

class Accelerators {
public:
	Accelerators(EditorFrame* editorFrame);

	void ReadCustomShortcuts();

	void ParseMenu();
	void ParseMenu(wxMenu* menu);
	void ParseMenu(wxMenuItem* menuItem);
	void InsertBinding(wxMenuItem* item, wxString& accel);

	void ParseBundlesMenu(wxMenu* menu);
	void ParseBundlesMenu(wxMenuItem* item);

	bool HandleKeyEvent(wxKeyEvent& event);
	bool HandleBundle(int code, int flags, const tmAction* x);
	bool WasChordActivated();
	bool HandleHash(int hash);
	bool IsChord(int code, int flags);

	wxString StatusBarText();

private:
	EditorFrame* m_editorFrame;
	std::map<wxString, wxString> m_customBindings;
	std::map<int, KeyChord*> m_chords;
	std::map<int, KeyBinding*> m_bindings;
	KeyChord* m_activeChord;

	bool m_chordActivated;
	bool m_actionReturned;
};

#endif
