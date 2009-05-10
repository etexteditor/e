/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __EDITORBUNDLEPANEL_H__
#define __EDITORBUNDLEPANEL_H__

#include "BundleItemType.h"
#include "key_hook.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/panel.h>
	#include <wx/choice.h>
	#include <wx/stattext.h>
	#include <wx/sizer.h>
#endif

// Pre-definitions
class wxStaticLine;
class EditorFrame;
class EditorCtrl;
class BundleItemEditorCtrl;
class CatalystWrapper;
class DocumentWrapper;
class ShortcutCtrl;

class EditorBundlePanel : public wxPanel {
public:
	EditorBundlePanel() {}; // default const
	EditorBundlePanel(wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap);
	EditorBundlePanel(int page_id, wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap);

	EditorCtrl* GetEditor();
	
	void UpdatePanel();

	virtual bool Show(bool show=true);

private:
	void Init();
	void LayoutCtrls();

	void UpdateSelector(BundleItemType type, const DocumentWrapper& dw);

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

	// Event handlers
	void OnChoice(wxCommandEvent& event);
	void OnTriggerChoice(wxCommandEvent& event);
	void OnTextChanged(wxCommandEvent& event);
	void OnKillTextFocus(wxFocusEvent& event);
	void OnClearShortcut(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnChildFocus(wxChildFocusEvent& event);
	DECLARE_EVENT_TABLE()

	// Member ctrl's
	BundleItemEditorCtrl* m_editorCtrl;
	wxChoice* m_envChoice;
	wxChoice* m_saveChoice;
	wxChoice* m_inputChoice;
	wxChoice* m_outputChoice;
	wxChoice* m_fallbackChoice;
	wxChoice* m_triggerChoice;
	wxTextCtrl* m_tabText;
	wxTextCtrl* m_scopeText;
	wxTextCtrl* m_extText;
	wxButton* m_clearButton;
	wxStaticText* m_saveStatic;
	wxStaticText* m_envStatic;
	wxStaticText* m_inputStatic;
	wxStaticText* m_outputStatic;
	wxStaticText* m_orStatic;
	wxStaticText* m_activationStatic;
	wxStaticText* m_scopeStatic;
	wxStaticText* m_extStatic;
	wxStaticLine* m_staticLine;
	ShortcutCtrl* m_shortcutCtrl;

	// Member sizers
	wxBoxSizer* m_mainSizer;
	wxBoxSizer* m_envSizer;
	wxBoxSizer* m_inputSizer;
	wxBoxSizer* m_triggerSizer;

	// Member variables
	BundleItemType m_currentType;

	DECLARE_DYNAMIC_CLASS_NO_COPY(EditorBundlePanel)
};

#endif //__EDITORBUNDLEPANEL_H__

