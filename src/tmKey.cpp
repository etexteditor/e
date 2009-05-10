#include "tmKey.h"

map<wxChar, wxKeyCode> tmKey::s_keyMap;
map<wxChar, wxKeyCode> tmKey::s_numMap;
map<int, wxString> tmKey::s_keyText;
map<int, wxChar> tmKey::s_keyBind;

// an instance of this class will be created at startup time, and initializer will take care of maps
static class tmKeyInitializer : private tmKey {
public:
    tmKeyInitializer() : tmKey() {
        tmKey::BuildMaps();
    }
} __key_maps_initializer;

void tmKey::BuildMaps() { // static
	s_keyMap[wxT('\x0003')] = WXK_CANCEL;
	s_keyMap[wxT('\x0009')] = WXK_TAB;
	s_keyMap[wxT('\x000A')] = WXK_RETURN;
	s_keyMap[wxT('\x000D')] = WXK_NUMPAD_ENTER;
	s_keyMap[wxT('\x001B')] = WXK_ESCAPE;
	s_keyMap[wxT('\x0020')] = WXK_SPACE;
	s_keyMap[wxT('\x007F')] = WXK_BACK;
	s_keyMap[wxT('\xF700')] = WXK_UP;
	s_keyMap[wxT('\xF701')] = WXK_DOWN;
	s_keyMap[wxT('\xF702')] = WXK_LEFT;
	s_keyMap[wxT('\xF703')] = WXK_RIGHT;
	s_keyMap[wxT('\xF704')] = WXK_F1;
	s_keyMap[wxT('\xF705')] = WXK_F2;
	s_keyMap[wxT('\xF706')] = WXK_F3;
	s_keyMap[wxT('\xF707')] = WXK_F4;
	s_keyMap[wxT('\xF708')] = WXK_F5;
	s_keyMap[wxT('\xF709')] = WXK_F6;
	s_keyMap[wxT('\xF70A')] = WXK_F7;
	s_keyMap[wxT('\xF70B')] = WXK_F8;
	s_keyMap[wxT('\xF70C')] = WXK_F9;
	s_keyMap[wxT('\xF70D')] = WXK_F10;
	s_keyMap[wxT('\xF70E')] = WXK_F11;
	s_keyMap[wxT('\xF70F')] = WXK_F12;
	s_keyMap[wxT('\xF710')] = WXK_F13;
	s_keyMap[wxT('\xF711')] = WXK_F14;
	s_keyMap[wxT('\xF712')] = WXK_F15;
	s_keyMap[wxT('\xF713')] = WXK_F16;
	s_keyMap[wxT('\xF714')] = WXK_F17;
	s_keyMap[wxT('\xF715')] = WXK_F18;
	s_keyMap[wxT('\xF716')] = WXK_F19;
	s_keyMap[wxT('\xF717')] = WXK_F20;
	s_keyMap[wxT('\xF718')] = WXK_F21;
	s_keyMap[wxT('\xF719')] = WXK_F22;
	s_keyMap[wxT('\xF71A')] = WXK_F23;
	s_keyMap[wxT('\xF71B')] = WXK_F24;
	s_keyMap[wxT('\xF727')] = WXK_INSERT;
	s_keyMap[wxT('\xF728')] = WXK_DELETE;
	s_keyMap[wxT('\xF729')] = WXK_HOME;
	s_keyMap[wxT('\xF72B')] = WXK_END;
	s_keyMap[wxT('\xF72C')] = WXK_PRIOR;    // Page up
	s_keyMap[wxT('\xF72D')] = WXK_NEXT;     // Page down
	s_keyMap[wxT('\xF72E')] = WXK_SNAPSHOT; // Print Screen
	s_keyMap[wxT('\xF72F')] = WXK_SCROLL;   // Scroll Lock
	s_keyMap[wxT('\xF730')] = WXK_PAUSE;
	s_keyMap[wxT('\xF746')] = WXK_HELP;
	s_keyMap[wxT('\xF735')] = WXK_MENU;
	s_keyMap[wxT('\xF739')] = WXK_CLEAR;
	s_keyMap[wxT('\xF732')] = WXK_CANCEL;   // Break

	s_numMap[wxT('0')] = WXK_NUMPAD0;
	s_numMap[wxT('1')] = WXK_NUMPAD1;
	s_numMap[wxT('2')] = WXK_NUMPAD2;
	s_numMap[wxT('3')] = WXK_NUMPAD3;
	s_numMap[wxT('4')] = WXK_NUMPAD4;
	s_numMap[wxT('5')] = WXK_NUMPAD5;
	s_numMap[wxT('6')] = WXK_NUMPAD6;
	s_numMap[wxT('7')] = WXK_NUMPAD7;
	s_numMap[wxT('8')] = WXK_NUMPAD8;
	s_numMap[wxT('9')] = WXK_NUMPAD9;
	s_numMap[wxT(' ')] = WXK_NUMPAD_SPACE;
	s_numMap[wxT('\x0009')] = WXK_NUMPAD_TAB;
	s_numMap[wxT('\x000A')] = WXK_NUMPAD_ENTER;
	s_numMap[wxT('\x000D')] = WXK_NUMPAD_ENTER;
	s_numMap[wxT('\xF704')] = WXK_NUMPAD_F1;
	s_numMap[wxT('\xF705')] = WXK_NUMPAD_F2;
	s_numMap[wxT('\xF706')] = WXK_NUMPAD_F3;
	s_numMap[wxT('\xF707')] = WXK_NUMPAD_F4;
	s_numMap[wxT('\xF729')] = WXK_NUMPAD_HOME;
	s_numMap[wxT('\xF702')] = WXK_NUMPAD_LEFT;
	s_numMap[wxT('\xF700')] = WXK_NUMPAD_UP;
	s_numMap[wxT('\xF703')] = WXK_NUMPAD_RIGHT;
	s_numMap[wxT('\xF701')] = WXK_NUMPAD_DOWN;
	s_numMap[wxT('\xF72C')] = WXK_NUMPAD_PRIOR;
	s_numMap[wxT('\xF72C')] = WXK_NUMPAD_PAGEUP;
	s_numMap[wxT('\xF72D')] = WXK_NUMPAD_NEXT;
	s_numMap[wxT('\xF72D')] = WXK_NUMPAD_PAGEDOWN;
	s_numMap[wxT('\xF72B')] = WXK_NUMPAD_END;
	s_numMap[wxT('\xF72A')] = WXK_NUMPAD_BEGIN;
	s_numMap[wxT('\xF727')] = WXK_NUMPAD_INSERT;
	s_numMap[wxT('\xF728')] = WXK_NUMPAD_DELETE;
	s_numMap[wxT('=')] = WXK_NUMPAD_EQUAL;
	s_numMap[wxT('*')] = WXK_NUMPAD_MULTIPLY;
	s_numMap[wxT('+')] = WXK_NUMPAD_ADD;
	s_numMap[wxT(',')] = WXK_NUMPAD_SEPARATOR;
	s_numMap[wxT('-')] = WXK_NUMPAD_SUBTRACT;
	s_numMap[wxT('.')] = WXK_NUMPAD_DECIMAL;
	s_numMap[wxT('/')] = WXK_NUMPAD_DIVIDE;

	s_keyBind[WXK_SPACE] = wxT('\x0020');
	s_keyBind[WXK_TAB] = wxT('\x0009');
	s_keyBind[WXK_RETURN] = wxT('\x000A');
	s_keyBind[WXK_NUMPAD_ENTER] = wxT('\x000D');
	s_keyBind[WXK_ESCAPE] = wxT('\x001B');
	s_keyBind[WXK_BACK] = wxT('\x007F');
	s_keyBind[WXK_UP] = wxT('\xF700');
	s_keyBind[WXK_DOWN] = wxT('\xF701');
	s_keyBind[WXK_LEFT] = wxT('\xF702');
	s_keyBind[WXK_RIGHT] = wxT('\xF703');
	s_keyBind[WXK_F1] = wxT('\xF704');
	s_keyBind[WXK_F2] = wxT('\xF705');
	s_keyBind[WXK_F3] = wxT('\xF706');
	s_keyBind[WXK_F4] = wxT('\xF707');
	s_keyBind[WXK_F5] = wxT('\xF708');
	s_keyBind[WXK_F6] = wxT('\xF709');
	s_keyBind[WXK_F7] = wxT('\xF70A');
	s_keyBind[WXK_F8] = wxT('\xF70B');
	s_keyBind[WXK_F9] = wxT('\xF70C');
	s_keyBind[WXK_F10] = wxT('\xF70D');
	s_keyBind[WXK_F11] = wxT('\xF70E');
	s_keyBind[WXK_F12] = wxT('\xF70F');
	s_keyBind[WXK_F13] = wxT('\xF710');
	s_keyBind[WXK_F14] = wxT('\xF711');
	s_keyBind[WXK_F15] = wxT('\xF712');
	s_keyBind[WXK_F16] = wxT('\xF713');
	s_keyBind[WXK_F17] = wxT('\xF714');
	s_keyBind[WXK_F18] = wxT('\xF715');
	s_keyBind[WXK_F19] = wxT('\xF716');
	s_keyBind[WXK_F20] = wxT('\xF717');
	s_keyBind[WXK_F21] = wxT('\xF718');
	s_keyBind[WXK_F22] = wxT('\xF719');
	s_keyBind[WXK_F23] = wxT('\xF71A');
	s_keyBind[WXK_F24] = wxT('\xF71B');
	s_keyBind[WXK_INSERT] = wxT('\xF727');
	s_keyBind[WXK_DELETE] = wxT('\xF728');
	s_keyBind[WXK_HOME] = wxT('\xF729');
	s_keyBind[WXK_END] = wxT('\xF72B');
	s_keyBind[WXK_PRIOR] = wxT('\xF72C');
	s_keyBind[WXK_NEXT] = wxT('\xF72D');
	s_keyBind[WXK_SNAPSHOT] = wxT('\xF72E');
	s_keyBind[WXK_SCROLL] = wxT('\xF72F');
	s_keyBind[WXK_PAUSE] = wxT('\xF730');
	s_keyBind[WXK_HELP] = wxT('\xF746');
	s_keyBind[WXK_MENU] = wxT('\xF735');
	s_keyBind[WXK_CLEAR] = wxT('\xF739');
	s_keyBind[WXK_CANCEL] = wxT('\xF732'); // break

	s_keyText[WXK_SPACE] = _("Space");
	s_keyText[WXK_TAB] = _("Tab");
	s_keyText[WXK_RETURN] = _("Return");
	s_keyText[WXK_NUMPAD_ENTER] = _("Enter");
	s_keyText[WXK_ESCAPE] = _("Escape");
	s_keyText[WXK_BACK] = _("Back");
	s_keyText[WXK_UP] = _("Up");
	s_keyText[WXK_DOWN] = _("Down");
	s_keyText[WXK_LEFT] = _("Left");
	s_keyText[WXK_RIGHT] = _("Right");
	s_keyText[WXK_F1] = _("F1");
	s_keyText[WXK_F2] = _("F2");
	s_keyText[WXK_F3] = _("F3");
	s_keyText[WXK_F4] = _("F4");
	s_keyText[WXK_F5] = _("F5");
	s_keyText[WXK_F6] = _("F6");
	s_keyText[WXK_F7] = _("F7");
	s_keyText[WXK_F8] = _("F8");
	s_keyText[WXK_F9] = _("F9");
	s_keyText[WXK_F10] = _("F10");
	s_keyText[WXK_F11] = _("F11");
	s_keyText[WXK_F12] = _("F12");
	s_keyText[WXK_F13] = _("F13");
	s_keyText[WXK_F14] = _("F14");
	s_keyText[WXK_F15] = _("F15");
	s_keyText[WXK_F16] = _("F16");
	s_keyText[WXK_F17] = _("F17");
	s_keyText[WXK_F18] = _("F18");
	s_keyText[WXK_F19] = _("F19");
	s_keyText[WXK_F20] = _("F20");
	s_keyText[WXK_F21] = _("F21");
	s_keyText[WXK_F22] = _("F22");
	s_keyText[WXK_F23] = _("F23");
	s_keyText[WXK_F24] = _("F24");
	s_keyText[WXK_INSERT] = _("Insert");
	s_keyText[WXK_DELETE] = _("Delete");
	s_keyText[WXK_HOME] = _("Home");
	s_keyText[WXK_END] = _("End");
	s_keyText[WXK_PRIOR] = _("PageUp");
	s_keyText[WXK_NEXT] = _("PageDown");
	s_keyText[WXK_SNAPSHOT] = _("PrintScn");
	s_keyText[WXK_SCROLL] = _("ScrollLock");
	s_keyText[WXK_PAUSE] = _("Pause");
	s_keyText[WXK_HELP] = _("Help");
	s_keyText[WXK_MENU] = _("Menu");
	s_keyText[WXK_CLEAR] = _("Clear");
	s_keyText[WXK_CANCEL] = _("Cancel");
}


tmKey::tmKey(const wxString& binding) : modifiers(0), keyCode(0) {
	bool numpad = false;

	for (unsigned int i = 0; i < binding.size(); ++i) {
		wxChar c = binding[i];

		// Everything before last char is modifier
		if (i+1 < binding.size()) {
			switch(c) {
			case wxT('$'):
				modifiers |= wxMOD_SHIFT;
				break;

			case wxT('@'):
				modifiers |= wxMOD_CONTROL;
				break;

			case wxT('^'):
				modifiers |= wxMOD_ALT;
				break;

			case wxT('~'):
				modifiers |= wxMOD_META;
				break;

			case wxT('#'):
				{
					// Numpad
					++i;
					numpad = true;
					c = binding[i];

					// Numpadded keycode
					map<wxChar, wxKeyCode>::const_iterator p = s_numMap.find(c);
					if (p != s_numMap.end()) {
						keyCode = p->second;
					}
					else goto error;
				}
				break;

			default:
				// ignore invalid modifiers
				break;
			}
		}
		else {
			// check keymap
			map<wxChar, wxKeyCode>::const_iterator p = s_keyMap.find(c);
			if (p != s_keyMap.end()) {
				keyCode= p->second;
			}
			else {
				tmKey::uniToWxk(c, keyCode, modifiers);
			}
		}
	}
	
	UpdateShortcut();

#ifdef __WXDEBUG__
	wxLogDebug(wxT("ParseKey('%s' to '%s')"), binding.c_str(), shortcut.c_str());

	if (!(keyCode != 0 || modifiers == 0)) {
		const wxString msg = wxString::Format(wxT("Invalid keycode in bundle: '%s' (%d,%d)"), binding.c_str(), keyCode, modifiers);
		wxMessageBox(msg, wxT("Bundle parse error"), wxICON_ERROR);
	}
#endif //__WXDEBUG__

	return;

error:
	keyCode = modifiers = 0;
}

void tmKey::UpdateShortcut() {
	shortcut.clear();
	
	// Build shortcut string
	if (modifiers & 0x0002) shortcut += _("Ctrl-");
	if (modifiers & 0x0001) shortcut += _("Alt-");
	if (modifiers & 0x0008) shortcut += _("Win-");
	if (modifiers & 0x0004) shortcut += _("Shift-");
	//if (numpad) key.shortcut += _("Numpad-");

	if (keyCode) {
		map<int, wxString>::const_iterator k = s_keyText.find(keyCode);
		if (k != s_keyText.end()) {
			if (keyCode == WXK_NUMPAD_ENTER) shortcut += _("Numpad-");
			shortcut += k->second;
		}
		else  {
			wxChar uniKey;

			if (tmKey::wxkToUni(keyCode, false, uniKey)) shortcut += CharToUpper(uniKey);
			else {
				// If we cannot get a valid shortcut (it might need a dead key combi
				// on this keyboard layout). We remove the shortcut
				shortcut.clear();
			}
		}
	}

#ifdef __WXDEBUG__
	wxLogDebug(wxT("UpdateShortcut(%d - %c,%d) to '%s'"), keyCode, keyCode>32 ? keyCode : ' ', modifiers, shortcut.c_str());
#endif //__WXDEBUG__
}

wxString tmKey::getBinding() {
	wxString binding;

	if (modifiers & 0x0002) binding = _("@"); // Ctrl
	if (modifiers & 0x0001) binding += _("^"); // Alt
	if (modifiers & 0x0008) binding += _("~"); // Win

	if (keyCode) {
		map<int, wxChar>::const_iterator k = s_keyBind.find(keyCode);
		if (k != s_keyBind.end()) binding += k->second;
		else {
			wxChar uniKey;
			if (!tmKey::wxkToUni(keyCode, false, uniKey)) return wxEmptyString;

			if (modifiers & wxMOD_SHIFT) {
				wxChar uniKey_shifted;
				if (tmKey::wxkToUni(keyCode, false, uniKey_shifted) && uniKey_shifted != uniKey) {
					binding += uniKey_shifted;
				}
				else {
					binding += wxT('$'); // shift
					binding += uniKey;
				}
			}
		}
	}

#ifdef __WXDEBUG__
	wxLogDebug(wxT("getBinding(%d - %c,%d) to '%s'"), keyCode, keyCode, modifiers, binding.c_str());
#endif //__WXDEBUG__

	return binding;
}

bool tmKey::wxkToUni(int wxk, bool shifted, wxChar& uniCode) // static
{
    if (!wxIsprint(wxk))
        return false;

#ifdef __WXMSW__
    // Convert key to scan code
    const SHORT scancode = ::VkKeyScan(wxk);
    const unsigned char vk = LOBYTE(scancode);     // Low eight bits = virtual-key code

    // Get the char on the key for this keycode
    unsigned char key_state[256];
    memset (key_state, 0, sizeof (key_state));
    const int BUFFER_SIZE = 10;
    if (shifted)
        key_state[VK_SHIFT] = 0x80;
    TCHAR buf[BUFFER_SIZE] = { 0 };
    if (::ToUnicode(vk, 0, key_state, buf, BUFFER_SIZE, 0) != 1)
        return false;

    uniCode = buf[0];
    return true;
#elif defined(__WXGTK__)
    // FIXME not too correct - this is wxKeyCode not wxChar
    guint keyval = gdk_unicode_to_keyval(wxk);
    if (keyval & 0x01000000)
        return false;
    keyval = shifted ? gdk_keyval_to_upper(keyval) : gdk_keyval_to_lower(keyval);
    uniCode = gdk_keyval_to_unicode(keyval);

#ifdef __WXDEBUG__
    wxLogDebug(wxT("wxkToUni(%x- %c,%d) to %x - %c"), wxk, wxk, shifted, uniCode, uniCode);
#endif //__WXDEBUG__
    return true;
#else
#error Unknown platform
#endif
}

bool tmKey::uniToWxk(wxChar uniCode, int& wxk, int& mod) // static
{
#ifdef __WXMSW__
    // Convert key to scan code
    const SHORT scancode = ::VkKeyScan(uniCode);
    const unsigned char vk = LOBYTE(scancode);     // Low eight bits = virtual-key code
    const int state = HIBYTE(scancode);   // The next eight bits = shift state
    if (state & 1) mod |= wxMOD_SHIFT;
    if (state & 2) mod |= wxMOD_CONTROL;
    if (state & 4) mod |= wxMOD_ALT;

    wxk = wxCharCodeMSWToWX(vk, 0);
    if (wxk == 0) {
        // Normal ascii char
        wxk = uniCode;
    }
    return true;
#elif defined(__WXGTK__)
    guint keyval = gdk_unicode_to_keyval(uniCode);
    if (keyval & 0x01000000)
        return false;
    if (gdk_keyval_is_upper(keyval))
    {
        mod |= wxMOD_SHIFT;
        keyval = gdk_keyval_to_lower(keyval);
#ifdef __WXDEBUG__
    wxLogDebug(wxT("uniToWxk(%x - %c) downshifting keycode is %x"), uniCode, uniCode, keyval);
#endif //__WXDEBUG__
    }
    // FIXME not too correct - this is wxChar not wxKeyCode
    wxk = gdk_keyval_to_unicode(keyval);
#ifdef __WXDEBUG__
    wxLogDebug(wxT("uniToWxk(%x - %c) to %x - %c"), uniCode, uniCode, wxk, wxk);
#endif //__WXDEBUG__
    return true;
#else
#error Unknown platform
#endif
}

wxChar tmKey::CharToUpper(wxChar c) { // static
#ifdef __WXMSW__
	const LONG inChar = MAKELONG(c, 0);
	const LONG outChar = (LONG)::CharUpper((LPTSTR)inChar);
	return (wxChar)LOWORD(outChar);
#else
	return wxToupper(c);
#endif // __WXMSW__
}
