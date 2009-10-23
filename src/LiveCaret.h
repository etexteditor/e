#ifndef __LIVECARET_H__
#define  __LIVECARET_H__

#include <wx/caret.h>

class LiveCaret : public wxCaret {
public:
	LiveCaret(wxWindow *window, int width, int height):
		wxCaret(window, width, height), m_keepAlive(false) {};

	void KeepAlive(bool keepAlive) {m_keepAlive = keepAlive;};

	virtual void OnKillFocus() {
#ifdef __WXDEBUG__
		// WORKAROUND: avoid assert in wxCaret::DoMove()
		wxCaret::OnKillFocus();
#else
		if (!m_keepAlive) wxCaret::OnKillFocus();
#endif // __WXDEBUG__
	};

private:
	bool m_keepAlive;
};

#endif