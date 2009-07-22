#ifndef __SEPARATORLINE_H__
#define __SEPARATORLINE_H__

#include <wx/control.h>

class SeperatorLine : public wxControl {
public:
	SeperatorLine(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition);

private:
	// overriden base class virtuals
	virtual bool AcceptsFocus() const;

	// Event handlers
	void OnPaint(wxPaintEvent& evt);
	void OnEraseBackground(wxEraseEvent& event);
	DECLARE_EVENT_TABLE();
};

#endif
