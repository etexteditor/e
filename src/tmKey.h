#ifndef __TMKEY_H__
#define __TMKEY_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class tmKey {
public:
	tmKey()
		: modifiers(0), keyCode(0) {};
	tmKey(int key, int mod)
		: modifiers(mod), keyCode(key) {UpdateShortcut();};
	tmKey(const wxString& binding);

	wxString getBinding();

	static bool wxkToUni(int wxk, bool shifted, wxChar& uniCode);
	static bool uniToWxk(wxChar uniCode, int& wxk, int& mod);

	int modifiers;
	int keyCode;
	wxString shortcut;

protected:
	static void BuildMaps();

private:
	static wxChar CharToUpper(wxChar c);
	void UpdateShortcut();

	static map<wxChar, wxKeyCode> s_keyMap;
	static map<wxChar, wxKeyCode> s_numMap;
	static map<int, wxString> s_keyText;
	static map<int, wxChar> s_keyBind;
};



#endif // __TMKEY_H__
