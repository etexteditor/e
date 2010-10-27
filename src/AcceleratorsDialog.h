#ifndef __ACCELERATORSDLG_H__
#define __ACCELERATORSDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/treectrl.h>
#include <map>

class EditorFrame;
class Accelerators;
class ShortcutChordCtrl;

class AcceleratorsDialog : public wxDialog {
public:
	AcceleratorsDialog(EditorFrame *parent);

	void ParseMenu();
	void ParseMenu(wxMenu* menuItem, wxString label, wxTreeItemId parent);
	void ParseMenu(wxMenuItem* menuItem, wxTreeItemId parent);

	wxString GetBinding(wxString& label);
	wxString GetDefaultBinding(wxString& label);
	
	void OnSave(wxCommandEvent& event);
	void OnClick(wxTreeEvent& event);

private:
	class AcceleratorData {
	public:
		AcceleratorData(wxString label, wxString defaultAccel, wxString customAccel) :
		  label(label), defaultAccel(defaultAccel), customAccel(customAccel) {}

		wxString label, defaultAccel, customAccel;
	};

	class AcceleratorDialog : public wxDialog {
	public:
		AcceleratorDialog(AcceleratorData& data, AcceleratorsDialog* parent);
		wxString GetBinding();
		void OnSave(wxCommandEvent& event);
		void OnClear(wxCommandEvent& event);
		bool save;
	private:
		ShortcutChordCtrl* m_chordCtrl;
		DECLARE_EVENT_TABLE();
	};

	EditorFrame* m_editorFrame;
	Accelerators* m_accelerators;
	wxTreeCtrl* m_treeView;
	std::map<wxString, AcceleratorData> m_bindings;

	DECLARE_EVENT_TABLE();
};
#endif // __ACCELERATORSDLG_H__
