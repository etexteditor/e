#ifndef __ACCELERATORSDLG_H__
#define __ACCELERATORSDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

class EditorFrame;
class Accelerators;
class IHtmlWnd;
class IHtmlWndBeforeLoadEvent;

class AcceleratorsDialog : public wxDialog {
public:
	AcceleratorsDialog(EditorFrame *parent);

	wxString* GetHtml();
	void ParseMenu();
	void ParseMenu(wxMenu* menuItem, int index, wxString label, int level, int parent);
	void ParseMenu(wxMenuItem* menuItem, int index, int level, int parent);

private:
	void OnBeforeLoad(IHtmlWndBeforeLoadEvent& event);

	EditorFrame* m_editorFrame;
	IHtmlWnd* m_browser;
	wxArrayString m_htmlBits;
	Accelerators* m_accelerators;
	int m_id;

	DECLARE_EVENT_TABLE();
};
#endif // __ACCELERATORSDLG_H__
