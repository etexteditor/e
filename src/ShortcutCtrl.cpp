#include "ShortcutCtrl.h"
#include "tmKey.h"

ShortcutCtrl::ShortcutCtrl(wxWindow* parent, wxWindowID id) {
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE);
    RegisterKeyHook(); // TextCtrl isn't automatically hooked, hook now
}

void ShortcutCtrl::Clear() {
	m_binding.clear();
	wxTextCtrl::Clear();
}

bool ShortcutCtrl::IsEmpty() const {
	return m_binding.empty();
}

const wxString& ShortcutCtrl::GetBinding() const {
	return m_binding;
}

void ShortcutCtrl::SetKey(const wxString& binding) {
	m_binding = binding;
	const tmKey key(binding);
	ChangeValue(key.shortcut);
}

bool ShortcutCtrl::OnPreKeyDown(wxKeyEvent& event) {
	int id = event.GetKeyCode();

	// Ignore initial presses on modifier keys
	if (id == WXK_SHIFT || id == WXK_ALT || id == WXK_CONTROL ||
		id == WXK_WINDOWS_LEFT || id == WXK_WINDOWS_RIGHT)
	{
		return false;
	}

	int modifiers = event.GetModifiers();

	// Adam V: Should this next line be "__WXMSW__" or should it be removed from the code?
#if __WXMSW
	if (HIWORD(event.m_rawFlags) & KF_EXTENDED) modifiers |= 0x0010; // Extended (usually keypad)
#endif
	tmKey key(id, modifiers);
	m_binding = key.getBinding(); // Set binding before changing the text value!

	SetValue(key.shortcut);

	// wxLogDebug(wxT("Binding: %s"), m_binding.c_str());
	return true;
}
