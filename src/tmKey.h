#ifndef __TMKEY_H__
#define __TMKEY_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

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
};

#endif // __TMKEY_H__
