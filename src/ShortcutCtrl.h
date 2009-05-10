#ifndef __SHORTCUTCTRL_H__
#define __SHORTCUTCTRL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "key_hook.h"

class ShortcutCtrl : public KeyHookable<wxTextCtrl> {
public:
	ShortcutCtrl(wxWindow* parent, wxWindowID id=wxID_ANY);
	void Clear();
	bool IsEmpty() const {return m_binding.empty();};

	void SetKey(const wxString& binding);
	const wxString& GetBinding() const {return m_binding;};

	virtual bool OnPreKeyDown(wxKeyEvent& event);

private:
	wxString m_binding;
};


#endif // __SHORTCUTCTRL_H__
