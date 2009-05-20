#include "tmKey.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

#ifndef WX_PRECOMP
	#include <wx/intl.h>
	#include <wx/log.h>
	#include <wx/msgdlg.h>
#endif

#ifdef __WXGTK__
  #include <gdk/gdk.h>
#endif


static std::map<wxChar, wxKeyCode> tmKey_s_keyMap;
static std::map<wxChar, wxKeyCode> tmKey_s_numMap;
static std::map<int, wxString> tmKey_s_keyText;
static std::map<int, wxChar> tmKey_s_keyBind;

// an instance of this class will be created at startup time, and initializer will take care of maps
static class tmKeyInitializer : private tmKey {
public:
    tmKeyInitializer() : tmKey() {
        tmKey::BuildMaps();
    }
} __key_maps_initializer;

void tmKey::BuildMaps() { // static
	tmKey_s_keyMap[wxT('\x0003')] = WXK_CANCEL;
	tmKey_s_keyMap[wxT('\x0009')] = WXK_TAB;
	tmKey_s_keyMap[wxT('\x000A')] = WXK_RETURN;
	tmKey_s_keyMap[wxT('\x000D')] = WXK_NUMPAD_ENTER;
	tmKey_s_keyMap[wxT('\x001B')] = WXK_ESCAPE;
	tmKey_s_keyMap[wxT('\x0020')] = WXK_SPACE;
	tmKey_s_keyMap[wxT('\x007F')] = WXK_BACK;
	tmKey_s_keyMap[wxT('\xF700')] = WXK_UP;
	tmKey_s_keyMap[wxT('\xF701')] = WXK_DOWN;
	tmKey_s_keyMap[wxT('\xF702')] = WXK_LEFT;
	tmKey_s_keyMap[wxT('\xF703')] = WXK_RIGHT;
	tmKey_s_keyMap[wxT('\xF704')] = WXK_F1;
	tmKey_s_keyMap[wxT('\xF705')] = WXK_F2;
	tmKey_s_keyMap[wxT('\xF706')] = WXK_F3;
	tmKey_s_keyMap[wxT('\xF707')] = WXK_F4;
	tmKey_s_keyMap[wxT('\xF708')] = WXK_F5;
	tmKey_s_keyMap[wxT('\xF709')] = WXK_F6;
	tmKey_s_keyMap[wxT('\xF70A')] = WXK_F7;
	tmKey_s_keyMap[wxT('\xF70B')] = WXK_F8;
	tmKey_s_keyMap[wxT('\xF70C')] = WXK_F9;
	tmKey_s_keyMap[wxT('\xF70D')] = WXK_F10;
	tmKey_s_keyMap[wxT('\xF70E')] = WXK_F11;
	tmKey_s_keyMap[wxT('\xF70F')] = WXK_F12;
	tmKey_s_keyMap[wxT('\xF710')] = WXK_F13;
	tmKey_s_keyMap[wxT('\xF711')] = WXK_F14;
	tmKey_s_keyMap[wxT('\xF712')] = WXK_F15;
	tmKey_s_keyMap[wxT('\xF713')] = WXK_F16;
	tmKey_s_keyMap[wxT('\xF714')] = WXK_F17;
	tmKey_s_keyMap[wxT('\xF715')] = WXK_F18;
	tmKey_s_keyMap[wxT('\xF716')] = WXK_F19;
	tmKey_s_keyMap[wxT('\xF717')] = WXK_F20;
	tmKey_s_keyMap[wxT('\xF718')] = WXK_F21;
	tmKey_s_keyMap[wxT('\xF719')] = WXK_F22;
	tmKey_s_keyMap[wxT('\xF71A')] = WXK_F23;
	tmKey_s_keyMap[wxT('\xF71B')] = WXK_F24;
	tmKey_s_keyMap[wxT('\xF727')] = WXK_INSERT;
	tmKey_s_keyMap[wxT('\xF728')] = WXK_DELETE;
	tmKey_s_keyMap[wxT('\xF729')] = WXK_HOME;
	tmKey_s_keyMap[wxT('\xF72B')] = WXK_END;
	tmKey_s_keyMap[wxT('\xF72C')] = WXK_PRIOR;    // Page up
	tmKey_s_keyMap[wxT('\xF72D')] = WXK_NEXT;     // Page down
	tmKey_s_keyMap[wxT('\xF72E')] = WXK_SNAPSHOT; // Print Screen
	tmKey_s_keyMap[wxT('\xF72F')] = WXK_SCROLL;   // Scroll Lock
	tmKey_s_keyMap[wxT('\xF730')] = WXK_PAUSE;
	tmKey_s_keyMap[wxT('\xF746')] = WXK_HELP;
	tmKey_s_keyMap[wxT('\xF735')] = WXK_MENU;
	tmKey_s_keyMap[wxT('\xF739')] = WXK_CLEAR;
	tmKey_s_keyMap[wxT('\xF732')] = WXK_CANCEL;   // Break

	tmKey_s_numMap[wxT('0')] = WXK_NUMPAD0;
	tmKey_s_numMap[wxT('1')] = WXK_NUMPAD1;
	tmKey_s_numMap[wxT('2')] = WXK_NUMPAD2;
	tmKey_s_numMap[wxT('3')] = WXK_NUMPAD3;
	tmKey_s_numMap[wxT('4')] = WXK_NUMPAD4;
	tmKey_s_numMap[wxT('5')] = WXK_NUMPAD5;
	tmKey_s_numMap[wxT('6')] = WXK_NUMPAD6;
	tmKey_s_numMap[wxT('7')] = WXK_NUMPAD7;
	tmKey_s_numMap[wxT('8')] = WXK_NUMPAD8;
	tmKey_s_numMap[wxT('9')] = WXK_NUMPAD9;
	tmKey_s_numMap[wxT(' ')] = WXK_NUMPAD_SPACE;
	tmKey_s_numMap[wxT('\x0009')] = WXK_NUMPAD_TAB;
	tmKey_s_numMap[wxT('\x000A')] = WXK_NUMPAD_ENTER;
	tmKey_s_numMap[wxT('\x000D')] = WXK_NUMPAD_ENTER;
	tmKey_s_numMap[wxT('\xF704')] = WXK_NUMPAD_F1;
	tmKey_s_numMap[wxT('\xF705')] = WXK_NUMPAD_F2;
	tmKey_s_numMap[wxT('\xF706')] = WXK_NUMPAD_F3;
	tmKey_s_numMap[wxT('\xF707')] = WXK_NUMPAD_F4;
	tmKey_s_numMap[wxT('\xF729')] = WXK_NUMPAD_HOME;
	tmKey_s_numMap[wxT('\xF702')] = WXK_NUMPAD_LEFT;
	tmKey_s_numMap[wxT('\xF700')] = WXK_NUMPAD_UP;
	tmKey_s_numMap[wxT('\xF703')] = WXK_NUMPAD_RIGHT;
	tmKey_s_numMap[wxT('\xF701')] = WXK_NUMPAD_DOWN;
	tmKey_s_numMap[wxT('\xF72C')] = WXK_NUMPAD_PRIOR;
	tmKey_s_numMap[wxT('\xF72C')] = WXK_NUMPAD_PAGEUP;
	tmKey_s_numMap[wxT('\xF72D')] = WXK_NUMPAD_NEXT;
	tmKey_s_numMap[wxT('\xF72D')] = WXK_NUMPAD_PAGEDOWN;
	tmKey_s_numMap[wxT('\xF72B')] = WXK_NUMPAD_END;
	tmKey_s_numMap[wxT('\xF72A')] = WXK_NUMPAD_BEGIN;
	tmKey_s_numMap[wxT('\xF727')] = WXK_NUMPAD_INSERT;
	tmKey_s_numMap[wxT('\xF728')] = WXK_NUMPAD_DELETE;
	tmKey_s_numMap[wxT('=')] = WXK_NUMPAD_EQUAL;
	tmKey_s_numMap[wxT('*')] = WXK_NUMPAD_MULTIPLY;
	tmKey_s_numMap[wxT('+')] = WXK_NUMPAD_ADD;
	tmKey_s_numMap[wxT(',')] = WXK_NUMPAD_SEPARATOR;
	tmKey_s_numMap[wxT('-')] = WXK_NUMPAD_SUBTRACT;
	tmKey_s_numMap[wxT('.')] = WXK_NUMPAD_DECIMAL;
	tmKey_s_numMap[wxT('/')] = WXK_NUMPAD_DIVIDE;

	tmKey_s_keyBind[WXK_SPACE] = wxT('\x0020');
	tmKey_s_keyBind[WXK_TAB] = wxT('\x0009');
	tmKey_s_keyBind[WXK_RETURN] = wxT('\x000A');
	tmKey_s_keyBind[WXK_NUMPAD_ENTER] = wxT('\x000D');
	tmKey_s_keyBind[WXK_ESCAPE] = wxT('\x001B');
	tmKey_s_keyBind[WXK_BACK] = wxT('\x007F');
	tmKey_s_keyBind[WXK_UP] = wxT('\xF700');
	tmKey_s_keyBind[WXK_DOWN] = wxT('\xF701');
	tmKey_s_keyBind[WXK_LEFT] = wxT('\xF702');
	tmKey_s_keyBind[WXK_RIGHT] = wxT('\xF703');
	tmKey_s_keyBind[WXK_F1] = wxT('\xF704');
	tmKey_s_keyBind[WXK_F2] = wxT('\xF705');
	tmKey_s_keyBind[WXK_F3] = wxT('\xF706');
	tmKey_s_keyBind[WXK_F4] = wxT('\xF707');
	tmKey_s_keyBind[WXK_F5] = wxT('\xF708');
	tmKey_s_keyBind[WXK_F6] = wxT('\xF709');
	tmKey_s_keyBind[WXK_F7] = wxT('\xF70A');
	tmKey_s_keyBind[WXK_F8] = wxT('\xF70B');
	tmKey_s_keyBind[WXK_F9] = wxT('\xF70C');
	tmKey_s_keyBind[WXK_F10] = wxT('\xF70D');
	tmKey_s_keyBind[WXK_F11] = wxT('\xF70E');
	tmKey_s_keyBind[WXK_F12] = wxT('\xF70F');
	tmKey_s_keyBind[WXK_F13] = wxT('\xF710');
	tmKey_s_keyBind[WXK_F14] = wxT('\xF711');
	tmKey_s_keyBind[WXK_F15] = wxT('\xF712');
	tmKey_s_keyBind[WXK_F16] = wxT('\xF713');
	tmKey_s_keyBind[WXK_F17] = wxT('\xF714');
	tmKey_s_keyBind[WXK_F18] = wxT('\xF715');
	tmKey_s_keyBind[WXK_F19] = wxT('\xF716');
	tmKey_s_keyBind[WXK_F20] = wxT('\xF717');
	tmKey_s_keyBind[WXK_F21] = wxT('\xF718');
	tmKey_s_keyBind[WXK_F22] = wxT('\xF719');
	tmKey_s_keyBind[WXK_F23] = wxT('\xF71A');
	tmKey_s_keyBind[WXK_F24] = wxT('\xF71B');
	tmKey_s_keyBind[WXK_INSERT] = wxT('\xF727');
	tmKey_s_keyBind[WXK_DELETE] = wxT('\xF728');
	tmKey_s_keyBind[WXK_HOME] = wxT('\xF729');
	tmKey_s_keyBind[WXK_END] = wxT('\xF72B');
	tmKey_s_keyBind[WXK_PRIOR] = wxT('\xF72C');
	tmKey_s_keyBind[WXK_NEXT] = wxT('\xF72D');
	tmKey_s_keyBind[WXK_SNAPSHOT] = wxT('\xF72E');
	tmKey_s_keyBind[WXK_SCROLL] = wxT('\xF72F');
	tmKey_s_keyBind[WXK_PAUSE] = wxT('\xF730');
	tmKey_s_keyBind[WXK_HELP] = wxT('\xF746');
	tmKey_s_keyBind[WXK_MENU] = wxT('\xF735');
	tmKey_s_keyBind[WXK_CLEAR] = wxT('\xF739');
	tmKey_s_keyBind[WXK_CANCEL] = wxT('\xF732'); // break

	tmKey_s_keyText[WXK_SPACE] = _("Space");
	tmKey_s_keyText[WXK_TAB] = _("Tab");
	tmKey_s_keyText[WXK_RETURN] = _("Return");
	tmKey_s_keyText[WXK_NUMPAD_ENTER] = _("Enter");
	tmKey_s_keyText[WXK_ESCAPE] = _("Escape");
	tmKey_s_keyText[WXK_BACK] = _("Back");
	tmKey_s_keyText[WXK_UP] = _("Up");
	tmKey_s_keyText[WXK_DOWN] = _("Down");
	tmKey_s_keyText[WXK_LEFT] = _("Left");
	tmKey_s_keyText[WXK_RIGHT] = _("Right");
	tmKey_s_keyText[WXK_F1] = _("F1");
	tmKey_s_keyText[WXK_F2] = _("F2");
	tmKey_s_keyText[WXK_F3] = _("F3");
	tmKey_s_keyText[WXK_F4] = _("F4");
	tmKey_s_keyText[WXK_F5] = _("F5");
	tmKey_s_keyText[WXK_F6] = _("F6");
	tmKey_s_keyText[WXK_F7] = _("F7");
	tmKey_s_keyText[WXK_F8] = _("F8");
	tmKey_s_keyText[WXK_F9] = _("F9");
	tmKey_s_keyText[WXK_F10] = _("F10");
	tmKey_s_keyText[WXK_F11] = _("F11");
	tmKey_s_keyText[WXK_F12] = _("F12");
	tmKey_s_keyText[WXK_F13] = _("F13");
	tmKey_s_keyText[WXK_F14] = _("F14");
	tmKey_s_keyText[WXK_F15] = _("F15");
	tmKey_s_keyText[WXK_F16] = _("F16");
	tmKey_s_keyText[WXK_F17] = _("F17");
	tmKey_s_keyText[WXK_F18] = _("F18");
	tmKey_s_keyText[WXK_F19] = _("F19");
	tmKey_s_keyText[WXK_F20] = _("F20");
	tmKey_s_keyText[WXK_F21] = _("F21");
	tmKey_s_keyText[WXK_F22] = _("F22");
	tmKey_s_keyText[WXK_F23] = _("F23");
	tmKey_s_keyText[WXK_F24] = _("F24");
	tmKey_s_keyText[WXK_INSERT] = _("Insert");
	tmKey_s_keyText[WXK_DELETE] = _("Delete");
	tmKey_s_keyText[WXK_HOME] = _("Home");
	tmKey_s_keyText[WXK_END] = _("End");
	tmKey_s_keyText[WXK_PRIOR] = _("PageUp");
	tmKey_s_keyText[WXK_NEXT] = _("PageDown");
	tmKey_s_keyText[WXK_SNAPSHOT] = _("PrintScn");
	tmKey_s_keyText[WXK_SCROLL] = _("ScrollLock");
	tmKey_s_keyText[WXK_PAUSE] = _("Pause");
	tmKey_s_keyText[WXK_HELP] = _("Help");
	tmKey_s_keyText[WXK_MENU] = _("Menu");
	tmKey_s_keyText[WXK_CLEAR] = _("Clear");
	tmKey_s_keyText[WXK_CANCEL] = _("Cancel");
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
					std::map<wxChar, wxKeyCode>::const_iterator p = tmKey_s_numMap.find(c);
					if (p != tmKey_s_numMap.end()) {
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
			std::map<wxChar, wxKeyCode>::const_iterator p = tmKey_s_keyMap.find(c);
			if (p != tmKey_s_keyMap.end()) {
				keyCode= p->second;
			}
			else {
				tmKey::uniToWxk(c, keyCode, modifiers);
			}
		}
	}
	
	UpdateShortcut();

#ifdef __WXDEBUG__
	//wxLogDebug(wxT("ParseKey('%s' to '%s')"), binding.c_str(), shortcut.c_str());

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
		std::map<int, wxString>::const_iterator k = tmKey_s_keyText.find(keyCode);
		if (k != tmKey_s_keyText.end()) {
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

	//wxLogDebug(wxT("UpdateShortcut(%d - %c,%d) to '%s'"), keyCode, keyCode>32 ? keyCode : ' ', modifiers, shortcut.c_str());
}

wxString tmKey::getBinding() {
	wxString binding;

	if (modifiers & 0x0002) binding = _("@"); // Ctrl
	if (modifiers & 0x0001) binding += _("^"); // Alt
	if (modifiers & 0x0008) binding += _("~"); // Win

	if (keyCode) {
		std::map<int, wxChar>::const_iterator k = tmKey_s_keyBind.find(keyCode);
		if (k != tmKey_s_keyBind.end()) binding += k->second;
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

	//wxLogDebug(wxT("getBinding(%d - %c,%d) to '%s'"), keyCode, keyCode, modifiers, binding.c_str());

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
        // Note that this is set to the virtual keycode (which does not necessarily
        // correspond to the ascii value). This is needed because keydown events
        // report it in this format.
        wxk = vk;
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
