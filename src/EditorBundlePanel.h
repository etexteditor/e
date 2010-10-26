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
#include "IUpdatePanel.h"
#include "ITabPage.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif


class wxStaticLine;

class EditorFrame;
class EditorCtrl;
class BundleItemEditorCtrl;
class CatalystWrapper;
class DocumentWrapper;
class ShortcutCtrl;

class EditorBundlePanel : public wxPanel, public IUpdatePanel, public ITabPage {
public:
	EditorBundlePanel() {}; // default const
	EditorBundlePanel(wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap);
	EditorBundlePanel(int page_id, wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap);

	virtual void UpdatePanel();

	virtual bool Show(bool show=true);

	virtual EditorCtrl* GetActiveEditor();
	virtual const char** RecommendedIcon() const;
	virtual void SaveSettings(unsigned int i, eFrameSettings& settings);
	virtual void CommandModeEnded();

private:
	void Init();
	void LayoutCtrls();

	void UpdateSelector(BundleItemType type, const DocumentWrapper& dw);

	// Event handlers
	void OnChoice(wxCommandEvent& event);
	void OnTriggerChoice(wxCommandEvent& event);
	void OnTextChanged(wxCommandEvent& event);
	void OnKillTextFocus(wxFocusEvent& event);
	void OnClearShortcut(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnChildFocus(wxChildFocusEvent& event);
	DECLARE_EVENT_TABLE()

	// Member data
	BundleItemEditorCtrl* m_editorCtrl;
	BundleItemType m_currentType;

	// --- ^ Initializer List ^ ---

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

	DECLARE_DYNAMIC_CLASS_NO_COPY(EditorBundlePanel)
};

#endif //__EDITORBUNDLEPANEL_H__

