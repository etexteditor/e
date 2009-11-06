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

#include "EditorBundlePanel.h"
#include <wx/gbsizer.h>
#include <wx/statline.h>
#include "BundleItemEditorCtrl.h"
#include "ShortcutCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(EditorBundlePanel, wxPanel)

// Ctrl id's
enum {
	CTRL_ENVCHOICE,
	CTRL_SAVECHOICE,
	CTRL_INPUTCHOICE,
	CTRL_OUTPUTCHOICE,
	CTRL_FALLBACKCHOICE,
	CTRL_TRIGGERCHOICE,
	CTRL_CLEARSHORTCUT,
	CTRL_EXTTEXT,
	CTRL_TABTEXT,
	CTRL_SCOPETEXT,
	CTRL_SHORTCUT
};

BEGIN_EVENT_TABLE(EditorBundlePanel, wxPanel)
	EVT_CHOICE(CTRL_ENVCHOICE, EditorBundlePanel::OnChoice)
	EVT_CHOICE(CTRL_SAVECHOICE, EditorBundlePanel::OnChoice)
	EVT_CHOICE(CTRL_INPUTCHOICE, EditorBundlePanel::OnChoice)
	EVT_CHOICE(CTRL_OUTPUTCHOICE, EditorBundlePanel::OnChoice)
	EVT_CHOICE(CTRL_FALLBACKCHOICE, EditorBundlePanel::OnChoice)
	EVT_CHOICE(CTRL_TRIGGERCHOICE, EditorBundlePanel::OnTriggerChoice)
	EVT_BUTTON(CTRL_CLEARSHORTCUT, EditorBundlePanel::OnClearShortcut)
	EVT_TEXT(CTRL_EXTTEXT, EditorBundlePanel::OnTextChanged)
	EVT_TEXT(CTRL_TABTEXT, EditorBundlePanel::OnTextChanged)
	EVT_TEXT(CTRL_SCOPETEXT, EditorBundlePanel::OnTextChanged)
	EVT_TEXT(CTRL_SHORTCUT, EditorBundlePanel::OnTextChanged)
	EVT_SIZE(EditorBundlePanel::OnSize)
	EVT_CHILD_FOCUS(EditorBundlePanel::OnChildFocus)
END_EVENT_TABLE()

EditorBundlePanel::EditorBundlePanel(wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap): 
	wxPanel(parent, wxID_ANY, wxPoint(-100,-100)), 
	m_editorCtrl(new BundleItemEditorCtrl(cw, bitmap, this, parentFrame)), 
	m_currentType(BUNDLE_NONE) 
{
	Init();
}

EditorBundlePanel::EditorBundlePanel(int page_id, wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap): 
	wxPanel(parent, wxID_ANY, wxPoint(-100,-100)), 
	m_editorCtrl(new BundleItemEditorCtrl(page_id, cw, bitmap, this, parentFrame)), 
	m_currentType(BUNDLE_NONE) 
{
	Init();
	UpdatePanel();
}

void EditorBundlePanel::Init() {
	// Choices for 'save before command'
	wxArrayString saveChoices;
	saveChoices.Add(_("Nothing"));
	saveChoices.Add(_("Current File"));
	saveChoices.Add(_("All Files"));

	// Choices for Environment
	wxArrayString envChoices;
	envChoices.Add(_("Cygwin (generic)"));
	envChoices.Add(_("Cygwin (windows)"));
	envChoices.Add(_("Windows native"));

	// Choices for Input
	wxArrayString inputChoices;
	inputChoices.Add(_("None"));
	inputChoices.Add(_("Selected Text"));
	inputChoices.Add(_("Entire Document"));

	// Choices for Fallback
	wxArrayString fallbackChoices;
	fallbackChoices.Add(_("Document"));
	fallbackChoices.Add(_("Line"));
	fallbackChoices.Add(_("Word"));
	fallbackChoices.Add(_("Character"));
	fallbackChoices.Add(_("Scope"));
	fallbackChoices.Add(_("Nothing"));

	// Choices for Output
	wxArrayString outputChoices;
	outputChoices.Add(_("Discard"));
	outputChoices.Add(_("Replace Selected Text"));
	outputChoices.Add(_("Replace Document"));
	outputChoices.Add(_("Insert as Text"));
	outputChoices.Add(_("Insert as Snippet"));
	outputChoices.Add(_("Show as HTML"));
	outputChoices.Add(_("Show as Tool Tip"));
	outputChoices.Add(_("Create New Document"));

	// Choices for trigger choice
	wxArrayString triggerChoices;
	triggerChoices.Add(_("Tab Trigger"));
	triggerChoices.Add(_("Key Equivalent"));

	// Create controls
	m_saveStatic = new wxStaticText(this, wxID_ANY, _("Save:"));
	m_envStatic = new wxStaticText(this, wxID_ANY, _("Environment:"));
	m_inputStatic = new wxStaticText(this, wxID_ANY, _("Input:"));
	m_orStatic = new wxStaticText(this, wxID_ANY, _("or"));
	m_outputStatic = new wxStaticText(this, wxID_ANY, _("Output:"));
	m_extStatic = new wxStaticText(this, wxID_ANY, _("File Types:"));
	m_envChoice = new wxChoice(this, CTRL_ENVCHOICE, wxDefaultPosition, wxDefaultSize, envChoices);
	m_saveChoice = new wxChoice(this, CTRL_SAVECHOICE, wxDefaultPosition, wxDefaultSize, saveChoices);
	m_inputChoice = new wxChoice(this, CTRL_INPUTCHOICE, wxDefaultPosition, wxDefaultSize, inputChoices);
	m_fallbackChoice = new wxChoice(this, CTRL_FALLBACKCHOICE, wxDefaultPosition, wxDefaultSize, fallbackChoices);
	m_outputChoice = new wxChoice(this, CTRL_OUTPUTCHOICE, wxDefaultPosition, wxDefaultSize, outputChoices);
	m_extText = new wxTextCtrl(this, CTRL_EXTTEXT);

	m_staticLine = new wxStaticLine(this);

	m_activationStatic = new wxStaticText(this, wxID_ANY, _("Activation:"));
	m_scopeStatic = new wxStaticText(this, wxID_ANY, _("Scope Selector:"));
	m_triggerChoice = new wxChoice(this, CTRL_TRIGGERCHOICE, wxDefaultPosition, wxDefaultSize, triggerChoices);
	m_tabText = new wxTextCtrl(this, CTRL_TABTEXT);
	m_shortcutCtrl = new ShortcutCtrl(this, CTRL_SHORTCUT);
	m_clearButton = new wxButton(this, CTRL_CLEARSHORTCUT, _("Clear"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_scopeText = new wxTextCtrl(this, CTRL_SCOPETEXT);
	
	// Layout sizers
	m_envSizer = new wxBoxSizer(wxHORIZONTAL);
		m_envSizer->Add(m_envStatic, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT, 5);
		m_envSizer->Add(m_envChoice, 0, wxALIGN_RIGHT);
	m_inputSizer = new wxBoxSizer(wxHORIZONTAL);
		m_inputSizer->Add(m_inputChoice, 0);
		m_inputSizer->Add(m_orStatic, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
		m_inputSizer->Add(m_fallbackChoice, 0);
	m_triggerSizer = new wxBoxSizer(wxHORIZONTAL);
		m_triggerSizer->Add(m_triggerChoice, 0, wxRIGHT, 5);
		m_triggerSizer->Add(m_tabText, 1);
		m_triggerSizer->Add(m_shortcutCtrl, 1);
		m_triggerSizer->Add(m_clearButton, 0);

	m_editorCtrl->Show(true); // hidden by default

	m_mainSizer = new wxBoxSizer(wxVERTICAL);
		m_mainSizer->Add(m_editorCtrl, 1, wxEXPAND);
	
	SetSizer(m_mainSizer);

	// We want to get notified when text ctrls loses focus so we can freeze any changes
	m_extText->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(EditorBundlePanel::OnKillTextFocus), NULL, this);
	m_tabText->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(EditorBundlePanel::OnKillTextFocus), NULL, this);
	m_scopeText->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(EditorBundlePanel::OnKillTextFocus), NULL, this);

	// Notify editorCtrl that it is embedded in a bundle panel
	m_editorCtrl->SetParentPanel(this);
}

EditorCtrl* EditorBundlePanel::GetActiveEditor() {
	return m_editorCtrl;
}

const char** EditorBundlePanel::RecommendedIcon() const {
	return m_editorCtrl->RecommendedIcon();
}

void EditorBundlePanel::SaveSettings(unsigned int i, eFrameSettings& settings) {
	m_editorCtrl->SaveSettings(i, settings);
}

void EditorBundlePanel::CommandModeEnded() {
	m_editorCtrl->CommandModeEnded();
}

bool EditorBundlePanel::Show(bool show) {
	// When hiding, we want to hide panel first to avoid flicker
	bool result = false;
	if (!show) result = wxPanel::Show(false);

	// We want the editorCtrl to be shown/hidden like it's parent
	m_editorCtrl->Show(show);

	if (show) {
		Layout(); // dimensions may have changed while hidden
		result = wxPanel::Show(true);
	}

	return result;
}

void EditorBundlePanel::LayoutCtrls() {
	const BundleItemType type = m_editorCtrl->GetBundleType();
	if (m_currentType == type) return;

	// Clear previous panel layout
	if (m_currentType != BUNDLE_NONE) m_mainSizer->Remove(1);

	m_currentType = type;

	// Hide all ctrls
	const bool doShow = (type == BUNDLE_COMMAND || type == BUNDLE_SNIPPET);
	m_envChoice->Show(doShow);
	m_inputChoice->Show(doShow);
	m_outputChoice->Show(doShow);
	m_fallbackChoice->Show(doShow);
	m_triggerChoice->Show(doShow);
	m_tabText->Show(doShow);
	m_scopeText->Show(doShow);
	m_clearButton->Show(doShow);
	m_envStatic->Show(doShow);
	m_inputStatic->Show(doShow);
	m_outputStatic->Show(doShow);
	m_orStatic->Show(doShow);
	m_activationStatic->Show(doShow);
	m_scopeStatic->Show(doShow);
	m_staticLine->Show(doShow);
	m_shortcutCtrl->Show(doShow);

	m_saveStatic->Hide();
	m_saveChoice->Hide();
	m_extStatic->Hide();
	m_extText->Hide();

	switch(type) {
	case BUNDLE_COMMAND:
		{
			m_saveStatic->Show();
			m_saveChoice->Show();

			wxGridBagSizer* gridSizer = new wxGridBagSizer(5, 5);
				gridSizer->Add(m_saveStatic, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_saveChoice, wxGBPosition(0,1));
				gridSizer->Add(m_envStatic, wxGBPosition(0,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_envChoice, wxGBPosition(0,3), wxDefaultSpan, wxALIGN_RIGHT);
				gridSizer->Add(m_inputStatic, wxGBPosition(1,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_inputSizer, wxGBPosition(1,1));
				gridSizer->Add(m_outputStatic, wxGBPosition(1,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_outputChoice, wxGBPosition(1,3), wxDefaultSpan, wxALIGN_RIGHT);

				gridSizer->Add(m_staticLine,  wxGBPosition(2,0), wxGBSpan(1,4), wxEXPAND);

				gridSizer->Add(m_activationStatic, wxGBPosition(3,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_triggerSizer, wxGBPosition(3,1), wxGBSpan(1,3), wxEXPAND);
				gridSizer->Add(m_scopeStatic, wxGBPosition(4,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_scopeText, wxGBPosition(4,1), wxGBSpan(1,3), wxEXPAND);

				gridSizer->AddGrowableCol(1);
				m_mainSizer->Add(gridSizer, 0, wxEXPAND|wxALL, 5);
		}
		break;

	case BUNDLE_DRAGCMD:
		{
			m_extStatic->Show();
			m_extText->Show();
			m_envStatic->Show();
			m_envChoice->Show();
			m_scopeStatic->Show();
			m_scopeText->Show();

			wxGridBagSizer* gridSizer = new wxGridBagSizer(5, 5);
				gridSizer->Add(m_extStatic, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_extText, wxGBPosition(0,1), wxDefaultSpan, wxEXPAND);
				gridSizer->Add(m_envStatic, wxGBPosition(0,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_envChoice, wxGBPosition(0,3), wxDefaultSpan, wxALIGN_RIGHT);
				gridSizer->Add(m_scopeStatic, wxGBPosition(1,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_scopeText, wxGBPosition(1,1), wxGBSpan(1,3), wxEXPAND);
				gridSizer->AddGrowableCol(1);
				m_mainSizer->Add(gridSizer, 0, wxEXPAND|wxALL, 5);
		}
		break;

	case BUNDLE_SNIPPET:
		{
			m_inputStatic->Hide();
			m_inputSizer->Show(false);
			m_outputStatic->Hide();
			m_outputChoice->Hide();
			m_envStatic->Hide();
			m_envChoice->Hide();
			
			wxGridBagSizer* gridSizer = new wxGridBagSizer(5, 5);
				gridSizer->Add(m_activationStatic, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_triggerSizer, wxGBPosition(0,1), wxGBSpan(1,3), wxEXPAND);
				gridSizer->Add(m_scopeStatic, wxGBPosition(1,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
				gridSizer->Add(m_scopeText, wxGBPosition(1,1), wxGBSpan(1,3), wxEXPAND);

				gridSizer->AddGrowableCol(1);
				m_mainSizer->Add(gridSizer, 0, wxEXPAND|wxALL, 5);
		}
		break;

	case BUNDLE_PREF:
		{
			m_scopeStatic->Show();
			m_scopeText->Show();
			
			wxBoxSizer* scopeSizer = new wxBoxSizer(wxHORIZONTAL);
				scopeSizer->Add(m_scopeStatic, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
				scopeSizer->Add(m_scopeText, 1);
				m_mainSizer->Add(scopeSizer, 0, wxEXPAND|wxALL, 5);

		}
		break;

	case BUNDLE_LANGUAGE:
		{
			m_activationStatic->Show();
			m_shortcutCtrl->Show();
			m_clearButton->Show();

			wxBoxSizer* activationSizer = new wxBoxSizer(wxHORIZONTAL);
				activationSizer->Add(m_activationStatic, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);
				activationSizer->Add(m_triggerSizer, 1);
				m_mainSizer->Add(activationSizer, 0, wxEXPAND|wxALL, 5);

		}
		break;

	default:
		wxASSERT(false);
	}

	m_mainSizer->Layout();
}

void EditorBundlePanel::UpdatePanel() {
	LayoutCtrls();

	const DocumentWrapper& dw = m_editorCtrl->GetDocument();
	const BundleItemType type = m_editorCtrl->GetBundleType();

	switch(type) {
	case BUNDLE_COMMAND:
		{
			wxString runEnvironment;
			wxString beforeRunningCommand;
			wxString input;
			wxString fallbackInput;
			wxString output;
			cxLOCKDOC_READ(dw)
				doc.GetProperty(wxT("bundle:runEnvironment"), runEnvironment);
				doc.GetProperty(wxT("bundle:beforeRunningCommand"), beforeRunningCommand);
				doc.GetProperty(wxT("bundle:input"), input);
				doc.GetProperty(wxT("bundle:fallbackInput"), fallbackInput);
				doc.GetProperty(wxT("bundle:output"), output);
			cxENDLOCK

			// Set run environment
			int envSel = 0;
			if (!runEnvironment.empty()) {
				if (runEnvironment == wxT("cygwin")) envSel = 1;
				else if (runEnvironment == wxT("windows")) envSel = 2;
			}
			m_envChoice->SetSelection(envSel);

			// Action to do before command
			int saveSel = 0;
			if (!beforeRunningCommand.empty()) {
				if (beforeRunningCommand == wxT("nop")) saveSel = 0;
				else if (beforeRunningCommand == wxT("saveActiveFile")) saveSel = 1;
				else if (beforeRunningCommand == wxT("saveModifiedFiles")) saveSel = 2;
			}
			m_saveChoice->SetSelection(saveSel);

			// Input source
			int inputSel = 0;
			if (!input.empty()) {
				if (input == wxT("none")) inputSel = 0;
				else if (input == wxT("selection")) inputSel = 1;
				else if (input == wxT("document")) inputSel = 2;
			}
			m_inputChoice->SetSelection(inputSel);

			// Fallback is only valid if input is selection
			if (inputSel == 1) {
				// Fallback input source
				int fallbackSel = 5;
				if (!fallbackInput.empty()) {
					if (fallbackInput == wxT("document"))       fallbackSel = 0;
					else if (fallbackInput == wxT("line"))      fallbackSel = 1;
					else if (fallbackInput == wxT("word"))      fallbackSel = 2;
					else if (fallbackInput == wxT("character")) fallbackSel = 3;
					else if (fallbackInput == wxT("scope"))     fallbackSel = 4;
					else if (fallbackInput == wxT("none"))      fallbackSel = 5;
				}
				m_fallbackChoice->SetSelection(fallbackSel);

				m_inputSizer->Show(m_orStatic, true);
				m_inputSizer->Show(m_fallbackChoice, true);
			}
			else {
				m_inputSizer->Show(m_orStatic, false);
				m_inputSizer->Show(m_fallbackChoice, false);
			}
			m_inputSizer->Layout();

			// Output destination
			int outputSel = 0;
			if (!output.empty()) {
				if (output == wxT("discard"))                  outputSel = 0;
				else if (output == wxT("replaceSelectedText")) outputSel = 1;
				else if (output == wxT("replaceDocument"))     outputSel = 2;
				else if (output == wxT("afterSelectedText"))   outputSel = 3;
				else if (output == wxT("insertAsSnippet"))     outputSel = 4;
				else if (output == wxT("showAsHTML"))          outputSel = 5;
				else if (output == wxT("showAsTooltip"))       outputSel = 6;
				else if (output == wxT("openAsNewDocument"))   outputSel = 7;
			}
			m_outputChoice->SetSelection(outputSel);

			UpdateSelector(type, dw);
		}
		break;

	case BUNDLE_DRAGCMD:
		{
			wxString extText;
			wxString runEnvironment;
			wxString scope;
			cxLOCKDOC_READ(dw)
				doc.GetProperty(wxT("bundle:draggedFileExtensions"), extText);
				doc.GetProperty(wxT("bundle:runEnvironment"), runEnvironment);
				doc.GetProperty(wxT("bundle:scope"), scope);
			cxENDLOCK

			// Set run environment
			int envSel = 0;
			if (!runEnvironment.empty()) {
				if (runEnvironment == wxT("cygwin")) envSel = 1;
				else if (runEnvironment == wxT("windows")) envSel = 2;
			}
			m_envChoice->SetSelection(envSel);

			m_extText->ChangeValue(extText);
			m_scopeText->ChangeValue(scope);
		}
		break;

	case BUNDLE_SNIPPET:
		UpdateSelector(type, dw);
		break;

	case BUNDLE_PREF:
		{
			wxString scope;
			cxLOCKDOC_READ(dw)
				doc.GetProperty(wxT("bundle:scope"), scope);
			cxENDLOCK
			m_scopeText->ChangeValue(scope);
		}
		break;

	case BUNDLE_LANGUAGE:
		UpdateSelector(type, dw);
		break;

	default:
		wxASSERT(false);
	}
	m_mainSizer->Layout();
}

void EditorBundlePanel::UpdateSelector(BundleItemType type, const DocumentWrapper& dw) {
	// Get Properties
	wxString keyEquivalent;
	wxString tabTrigger;
	wxString scope;
	cxLOCKDOC_READ(dw)
		doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
		doc.GetProperty(wxT("bundle:tabTrigger"), tabTrigger);
		doc.GetProperty(wxT("bundle:scope"), scope);
	cxENDLOCK

	bool showTabTrigger = (type == BUNDLE_SNIPPET); // default
	if (!keyEquivalent.empty()) showTabTrigger = false;
	else if (!tabTrigger.empty()) showTabTrigger = true;

	// Update Ctrls
	if (showTabTrigger) {
		m_triggerChoice->SetSelection(0);
		m_tabText->SetValue(tabTrigger);

		m_triggerSizer->Show(m_tabText, true);
		m_triggerSizer->Show(m_shortcutCtrl, false);
		m_triggerSizer->Show(m_clearButton, false);
		m_triggerSizer->Layout();
	}
	else {
		m_triggerChoice->SetSelection(1); // keyEquiv is default
		m_triggerSizer->Show(m_tabText, false);
		m_triggerSizer->Show(m_shortcutCtrl, true);
		m_triggerSizer->Show(m_clearButton, true);
		m_triggerSizer->Layout();

		m_shortcutCtrl->SetKey(keyEquivalent);
	}
	m_scopeText->ChangeValue(scope);
}

void EditorBundlePanel::OnChoice(wxCommandEvent& event) {
	const wxObject* eventObject = event.GetEventObject();
	cxLOCKDOC_WRITE(m_editorCtrl->GetDocument())
		doc.Freeze();
		if (eventObject == m_envChoice) {
			switch (event.GetSelection()) {
				case 0: doc.DeleteProperty(wxT("bundle:runEnvironment")); break;
				case 1: doc.SetProperty(wxT("bundle:runEnvironment"), wxT("cygwin")); break;
				case 2: doc.SetProperty(wxT("bundle:runEnvironment"), wxT("windows")); break;
				default: wxASSERT(false);
			}			
		}
		else if (eventObject == m_saveChoice) {
			switch (event.GetSelection()) {
				case 0: doc.SetProperty(wxT("bundle:beforeRunningCommand"), wxT("nop")); break;
				case 1: doc.SetProperty(wxT("bundle:beforeRunningCommand"), wxT("saveActiveFile")); break;
				case 2: doc.SetProperty(wxT("bundle:beforeRunningCommand"), wxT("saveModifiedFiles")); break;
				default: wxASSERT(false);
			}
		}
		else if (eventObject == m_inputChoice) {
			switch (event.GetSelection()) {
				case 0: doc.SetProperty(wxT("bundle:input"), wxT("none")); break;
				case 1: doc.SetProperty(wxT("bundle:input"), wxT("selection")); break;
				case 2: doc.SetProperty(wxT("bundle:input"), wxT("document")); break;
				default: wxASSERT(false);
			}

			if (event.GetSelection() == 1) {
				m_inputSizer->Show(m_orStatic, true);
				m_inputSizer->Show(m_fallbackChoice, true);
			}
			else {
				doc.DeleteProperty(wxT("bundle:fallbackInput")); // fallback only valid if input is selection
				m_inputSizer->Show(m_orStatic, false);
				m_inputSizer->Show(m_fallbackChoice, false);
			}
			m_inputSizer->Layout();
		}
		else if (eventObject == m_fallbackChoice) {
			wxASSERT(m_inputChoice->GetSelection() == 1);
			switch (event.GetSelection()) {
				case 0: doc.SetProperty(wxT("bundle:fallbackInput"), wxT("document")); break;
				case 1: doc.SetProperty(wxT("bundle:fallbackInput"), wxT("line")); break;
				case 2: doc.SetProperty(wxT("bundle:fallbackInput"), wxT("word")); break;
				case 3: doc.SetProperty(wxT("bundle:fallbackInput"), wxT("character")); break;
				case 4: doc.SetProperty(wxT("bundle:fallbackInput"), wxT("scope")); break;
				case 5: doc.SetProperty(wxT("bundle:fallbackInput"), wxT("none")); break;
				default: wxASSERT(false);
			}
		}
		else if (eventObject == m_outputChoice) {
			switch (event.GetSelection()) {
				case 0: doc.SetProperty(wxT("bundle:output"), wxT("discard")); break;
				case 1: doc.SetProperty(wxT("bundle:output"), wxT("replaceSelectedText")); break;
				case 2: doc.SetProperty(wxT("bundle:output"), wxT("replaceDocument")); break;
				case 3: doc.SetProperty(wxT("bundle:output"), wxT("afterSelectedText")); break;
				case 4: doc.SetProperty(wxT("bundle:output"), wxT("insertAsSnippet")); break;
				case 5: doc.SetProperty(wxT("bundle:output"), wxT("showAsHTML")); break;
				case 6: doc.SetProperty(wxT("bundle:output"), wxT("showAsTooltip")); break;
				case 7: doc.SetProperty(wxT("bundle:output"), wxT("openAsNewDocument")); break;
				default: wxASSERT(false);
			}
		}
		else wxASSERT(false);
		doc.Freeze();
	cxENDLOCK
}
 
void EditorBundlePanel::OnTriggerChoice(wxCommandEvent& event) {
	if (event.GetSelection() == 0) { // Tab Trigger
		m_triggerSizer->Show(m_tabText, true);
		m_triggerSizer->Show(m_shortcutCtrl, false);
		m_triggerSizer->Show(m_clearButton, false);

		cxLOCKDOC_WRITE(m_editorCtrl->GetDocument())
			bool modified = false;
			if (doc.HasProperty(wxT("bundle:keyEquivalent"))) {
				doc.DeleteProperty(wxT("bundle:keyEquivalent"));
				modified = true;
			}
			if (!m_tabText->IsEmpty()) {
				doc.SetProperty(wxT("bundle:tabTrigger"), m_tabText->GetValue());
				modified = true;
			}
			if (modified) doc.Freeze();
		cxENDLOCK
	}
	else { // Key Equivalent
		m_triggerSizer->Show(m_tabText, false);
		m_triggerSizer->Show(m_shortcutCtrl, true);
		m_triggerSizer->Show(m_clearButton, true);

		cxLOCKDOC_WRITE(m_editorCtrl->GetDocument())
			bool modified = false;
			if (doc.HasProperty(wxT("bundle:tabTrigger"))) {
				doc.DeleteProperty(wxT("bundle:tabTrigger"));
				modified = true;
			}
			bool shortCutIsEmpty = m_shortcutCtrl->IsEmpty();
			if (!shortCutIsEmpty) {
				const wxString& binding = m_shortcutCtrl->GetBinding();
				doc.SetProperty(wxT("bundle:keyEquivalent"), binding);
				modified = true;
			}
			if (modified) doc.Freeze();
		cxENDLOCK
	}

	m_triggerSizer->Layout();
}

void EditorBundlePanel::OnClearShortcut(wxCommandEvent& WXUNUSED(event)) {
	m_shortcutCtrl->Clear();

	cxLOCKDOC_WRITE(m_editorCtrl->GetDocument())
		if (doc.HasProperty(wxT("bundle:keyEquivalent"))) {
			doc.DeleteProperty(wxT("bundle:keyEquivalent"));
			doc.Freeze();
		}
	cxENDLOCK
}

void EditorBundlePanel::OnTextChanged(wxCommandEvent& event) {
	const wxObject* eventObject = event.GetEventObject();
	cxLOCKDOC_WRITE(m_editorCtrl->GetDocument())
		if (eventObject == m_extText) {
			doc.SetProperty(wxT("bundle:draggedFileExtensions"), m_extText->GetValue());
		}
		else if (eventObject == m_shortcutCtrl) {
			const wxString& binding = m_shortcutCtrl->GetBinding();
			doc.SetProperty(wxT("bundle:keyEquivalent"), binding);
		}
		else if (eventObject == m_tabText) {
			doc.SetProperty(wxT("bundle:tabTrigger"), m_tabText->GetValue());
		}
		else if (eventObject == m_scopeText) {
			doc.SetProperty(wxT("bundle:scope"), m_scopeText->GetValue());
		}
		else wxASSERT(false);
	cxENDLOCK
}

void EditorBundlePanel::OnKillTextFocus(wxFocusEvent& WXUNUSED(event)) {
	cxLOCKDOC_WRITE(m_editorCtrl->GetDocument())
		doc.Freeze();
	cxENDLOCK
}

void EditorBundlePanel::OnSize(wxSizeEvent& event) {
	event.Skip();
}

void EditorBundlePanel::OnChildFocus(wxChildFocusEvent& event) {
	// When hidden we want to eat ChildFocusEvents. Otherwise when hiding the
	// editorCtrl, focus will shift to another ctrl and the event propagate
	// to AUI Notebook (switching selection back to the previous tab).
	if (IsShown()) wxPanel::OnChildFocus(event);
}
