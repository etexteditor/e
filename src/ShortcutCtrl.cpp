#include "ShortcutCtrl.h"

// For tmKey:
#include "tm_syntaxhandler.h"

ShortcutCtrl::ShortcutCtrl(wxWindow* parent, wxWindowID id) {
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE);
    RegisterKeyHook(); // TextCtrl isn't automatically hooked, hook now
}

void ShortcutCtrl::Clear() {
	m_binding.clear();
	wxTextCtrl::Clear();
}

void ShortcutCtrl::SetKey(const wxString& binding) {
	m_binding = binding;

	const tmKey key(binding);
	ChangeValue(key.shortcut);
}

bool ShortcutCtrl::OnPreKeyDown(wxKeyEvent& event) {
	int id = event.GetKeyCode();

	// Ignore initial presses on modifier key
	if (id != WXK_SHIFT &&
		id != WXK_ALT &&
		id != WXK_CONTROL &&
		id != WXK_WINDOWS_LEFT &&
		id != WXK_WINDOWS_RIGHT)
	{	
		int modifiers = event.GetModifiers();
#if __WXMSW
		if (HIWORD(event.m_rawFlags) & KF_EXTENDED) modifiers |= 0x0010; // Extended (usually keypad)
#endif
		tmKey key(id, modifiers);

		SetValue(key.shortcut);

		m_binding = key.getBinding();
		wxLogDebug(wxT("Binding: %s"), m_binding.c_str());
		return true;
	}

	return false;
}
