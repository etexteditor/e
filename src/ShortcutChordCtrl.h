#ifndef __SHORTCUTCHORDCTRL_H__
#define __SHORTCUTCHORDCTRL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "key_hook.h"

class ShortcutChordCtrl : public KeyHookable<wxTextCtrl> {
public:
	ShortcutChordCtrl(wxWindow* parent, wxWindowID id=wxID_ANY);
	void Clear();
	bool IsEmpty() const;

	void SetKey(const wxString& binding);
	const wxString& GetBinding() const;

	virtual bool OnPreKeyUp(wxKeyEvent& event);

private:
	wxString m_binding;
};


#endif // __SHORTCUTCHORDCTRL_H__
