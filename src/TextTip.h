#ifndef __TEXTTIP_H__
#define __TEXTTIP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/popupwin.h>
#include "key_hook.h"

class TextTip : public KeyHookable<wxPopupTransientWindow> {
public:
	TextTip(wxWindow *parent, const wxString& text, const wxPoint& pos, const wxSize& size, TextTip** windowPtr);
	~TextTip();

	void Close();

private:
	void OnDismiss();

	void OnKeyDown(wxKeyEvent& event);
	void OnMouseClick(wxMouseEvent& event);
	void OnCaptureLost(wxMouseCaptureLostEvent& event);
	DECLARE_EVENT_TABLE();

	virtual bool OnPreKeyDown(wxKeyEvent& event);

	TextTip** m_windowPtr;
};


#endif // __TEXTTIP_H__
