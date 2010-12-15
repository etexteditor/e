#include "ShortcutChordCtrl.h"
#include "tmKey.h"

static const struct wxKeyName
{
    wxKeyCode code;
    const wxChar *name;
} wxKeyNames[] =
{
    { WXK_DELETE, wxTRANSLATE("DEL") },
    { WXK_DELETE, wxTRANSLATE("DELETE") },
    { WXK_BACK, wxTRANSLATE("BACK") },
    { WXK_INSERT, wxTRANSLATE("INS") },
    { WXK_INSERT, wxTRANSLATE("INSERT") },
    { WXK_RETURN, wxTRANSLATE("ENTER") },
    { WXK_RETURN, wxTRANSLATE("RETURN") },
    { WXK_PAGEUP, wxTRANSLATE("PGUP") },
    { WXK_PAGEDOWN, wxTRANSLATE("PGDN") },
    { WXK_LEFT, wxTRANSLATE("LEFT") },
    { WXK_RIGHT, wxTRANSLATE("RIGHT") },
    { WXK_UP, wxTRANSLATE("UP") },
    { WXK_DOWN, wxTRANSLATE("DOWN") },
    { WXK_HOME, wxTRANSLATE("HOME") },
    { WXK_END, wxTRANSLATE("END") },
    { WXK_SPACE, wxTRANSLATE("SPACE") },
    { WXK_TAB, wxTRANSLATE("TAB") },
    { WXK_ESCAPE, wxTRANSLATE("ESC") },
    { WXK_ESCAPE, wxTRANSLATE("ESCAPE") },
    { WXK_CANCEL, wxTRANSLATE("CANCEL") },
    { WXK_CLEAR, wxTRANSLATE("CLEAR") },
    { WXK_MENU, wxTRANSLATE("MENU") },
    { WXK_PAUSE, wxTRANSLATE("PAUSE") },
    { WXK_CAPITAL, wxTRANSLATE("CAPITAL") },
    { WXK_SELECT, wxTRANSLATE("SELECT") },
    { WXK_PRINT, wxTRANSLATE("PRINT") },
    { WXK_EXECUTE, wxTRANSLATE("EXECUTE") },
    { WXK_SNAPSHOT, wxTRANSLATE("SNAPSHOT") },
    { WXK_HELP, wxTRANSLATE("HELP") },
    { WXK_ADD, wxTRANSLATE("ADD") },
    { WXK_SEPARATOR, wxTRANSLATE("SEPARATOR") },
    { WXK_SUBTRACT, wxTRANSLATE("SUBTRACT") },
    { WXK_DECIMAL, wxTRANSLATE("DECIMAL") },
    { WXK_DIVIDE, wxTRANSLATE("DIVIDE") },
    { WXK_NUMLOCK, wxTRANSLATE("NUM_LOCK") },
    { WXK_SCROLL, wxTRANSLATE("SCROLL_LOCK") },
    { WXK_PAGEUP, wxTRANSLATE("PAGEUP") },
    { WXK_PAGEDOWN, wxTRANSLATE("PAGEDOWN") },
    { WXK_NUMPAD_SPACE, wxTRANSLATE("KP_SPACE") },
    { WXK_NUMPAD_TAB, wxTRANSLATE("KP_TAB") },
    { WXK_NUMPAD_ENTER, wxTRANSLATE("KP_ENTER") },
    { WXK_NUMPAD_HOME, wxTRANSLATE("KP_HOME") },
    { WXK_NUMPAD_LEFT, wxTRANSLATE("KP_LEFT") },
    { WXK_NUMPAD_UP, wxTRANSLATE("KP_UP") },
    { WXK_NUMPAD_RIGHT, wxTRANSLATE("KP_RIGHT") },
    { WXK_NUMPAD_DOWN, wxTRANSLATE("KP_DOWN") },
    { WXK_NUMPAD_PAGEUP, wxTRANSLATE("KP_PRIOR") },
    { WXK_NUMPAD_PAGEUP, wxTRANSLATE("KP_PAGEUP") },
    { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("KP_NEXT") },
    { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("KP_PAGEDOWN") },
    { WXK_NUMPAD_END, wxTRANSLATE("KP_END") },
    { WXK_NUMPAD_BEGIN, wxTRANSLATE("KP_BEGIN") },
    { WXK_NUMPAD_INSERT, wxTRANSLATE("KP_INSERT") },
    { WXK_NUMPAD_DELETE, wxTRANSLATE("KP_DELETE") },
    { WXK_NUMPAD_EQUAL, wxTRANSLATE("KP_EQUAL") },
    { WXK_NUMPAD_MULTIPLY, wxTRANSLATE("KP_MULTIPLY") },
    { WXK_NUMPAD_ADD, wxTRANSLATE("KP_ADD") },
    { WXK_NUMPAD_SEPARATOR, wxTRANSLATE("KP_SEPARATOR") },
    { WXK_NUMPAD_SUBTRACT, wxTRANSLATE("KP_SUBTRACT") },
    { WXK_NUMPAD_DECIMAL, wxTRANSLATE("KP_DECIMAL") },
    { WXK_NUMPAD_DIVIDE, wxTRANSLATE("KP_DIVIDE") },
    { WXK_WINDOWS_LEFT, wxTRANSLATE("WINDOWS_LEFT") },
    { WXK_WINDOWS_RIGHT, wxTRANSLATE("WINDOWS_RIGHT") },
    { WXK_WINDOWS_MENU, wxTRANSLATE("WINDOWS_MENU") },
    { WXK_COMMAND, wxTRANSLATE("COMMAND") },
};

wxString CodeToString(int code, int flags) {
    wxString text;

    if(flags & wxACCEL_ALT) text += _("Alt-");
    if(flags & wxACCEL_CMD) text += _("Ctrl-");
    if(flags & wxACCEL_SHIFT) text += _("Shift-");
    if(flags & 0x0008) text += _("Win-");

    if(code >= WXK_F1 && code <= WXK_F12)
		text << _("F") << code - WXK_F1 + 1;
    else if(code >= WXK_NUMPAD0 && code <= WXK_NUMPAD9) 
		text << _("KP_") << code - WXK_NUMPAD0;
    else if(code >= WXK_SPECIAL1 && code <= WXK_SPECIAL20)
        text << _("SPECIAL") << code - WXK_SPECIAL1 + 1;
    else {
        for(size_t n = 0; n < WXSIZEOF(wxKeyNames); n++) {
            const wxKeyName& kn = wxKeyNames[n];
            if(code == kn.code) {
                text << wxGetTranslation(kn.name);
                return text;
            }
        }

		text << (wxChar)code;
    }

    return text;
}

ShortcutChordCtrl::ShortcutChordCtrl(wxWindow* parent, wxWindowID id) {
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE);
    RegisterKeyHook(); // TextCtrl isn't automatically hooked, hook now
}

void ShortcutChordCtrl::Clear() {
	m_binding.clear();
	wxTextCtrl::Clear();
}

bool ShortcutChordCtrl::IsEmpty() const {
	return m_binding.empty();
}

const wxString& ShortcutChordCtrl::GetBinding() const {
	return m_binding;
}

void ShortcutChordCtrl::SetKey(const wxString& binding) {
	m_binding = binding;
	SetValue(binding);
}

bool ShortcutChordCtrl::OnPreKeyUp(wxKeyEvent& event) {
	int id = event.GetKeyCode();

	// Ignore initial presses on modifier keys
	if (id == WXK_SHIFT || id == WXK_ALT || id == WXK_CONTROL ||
		id == WXK_WINDOWS_LEFT || id == WXK_WINDOWS_RIGHT)
	{
		return false;
	}

	int modifiers = event.GetModifiers();
	
#ifdef __WXMSW__
	// GetModifiers does not report the windows keys
	if (::GetKeyState(VK_LWIN) < 0) modifiers |= 0x0008; // wxMOD_META (Left Windows key)
	if (::GetKeyState(VK_RWIN) < 0) modifiers |= 0x0008; // wxMOD_META (Right Windows key)
#endif

	wxString binding = CodeToString(id, modifiers);

	wxString value = GetValue();
	if(value.Contains(wxT(" "))) {
		value = wxT("");
	}

	if(!value.empty()) {
		value = wxString::Format(wxT("%s %s"), value.c_str(), binding.c_str());
	} else {
		value = binding;
	}

	m_binding = value;
	SetValue(value);

	event.StopPropagation();

	// wxLogDebug(wxT("Binding: %s"), m_binding.c_str());
	return true;
}
