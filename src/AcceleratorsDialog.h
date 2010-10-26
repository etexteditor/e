#ifndef __ACCELERATORSDLG_H__
#define __ACCELERATORSDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/treectrl.h>

class EditorFrame;
class Accelerators;

class AcceleratorsDialog : public wxDialog {
public:
	AcceleratorsDialog(EditorFrame *parent);

	void ParseMenu();
	void ParseMenu(wxMenu* menuItem, wxString label, wxTreeItemId parent);
	void ParseMenu(wxMenuItem* menuItem, wxTreeItemId parent);

private:
	void OnSave(wxCommandEvent& event);

	EditorFrame* m_editorFrame;
	Accelerators* m_accelerators;
	wxTreeCtrl* m_treeView;

	DECLARE_EVENT_TABLE();
};
#endif // __ACCELERATORSDLG_H__
