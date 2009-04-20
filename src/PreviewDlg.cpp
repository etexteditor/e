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

#include "PreviewDlg.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"
#include <wx/wfstream.h>

#if defined (__WXMSW__)
    #include "IEHtmlWin.h"
#elif defined (__WXGTK__)
    #include "WebKitHtmlWnd.h"
#endif

#include "images/left_arrow.xpm"
#include "images/left_arrow_gray.xpm"
//#include "images/left_arrow_over.xpm"
#include "images/left_arrow_click.xpm"
#include "images/right_arrow.xpm"
#include "images/right_arrow_gray.xpm"
//#include "images/right_arrow_over.xpm"
#include "images/right_arrow_click.xpm"
#include "images/pin1.xpm"
#include "images/pin1_over.xpm"
#include "images/pin2.xpm"
#include "images/pin2_over.xpm"

// control id's
enum
{
	ID_MSHTML,
	ID_RELOAD,
	ID_BACK,
	ID_FORWARD,
	ID_LOCATION,
	ID_PIN,
	ID_OPTIONS,
	ID_DOPIPE,
	ID_PIPECMD
};

BEGIN_EVENT_TABLE(PreviewDlg, wxPanel)
	EVT_IDLE(PreviewDlg::OnIdle)
	EVT_BUTTON(ID_RELOAD, PreviewDlg::OnButtonReload)
	EVT_BUTTON(ID_BACK, PreviewDlg::OnButtonBack)
	EVT_BUTTON(ID_FORWARD, PreviewDlg::OnButtonForward)
	EVT_BUTTON(ID_PIN, PreviewDlg::OnButtonPin)
	EVT_END_PROCESS(99, PreviewDlg::OnEndProcess)
	EVT_CHECKBOX(ID_OPTIONS, PreviewDlg::OnShowOptions)
	EVT_CHECKBOX(ID_DOPIPE, PreviewDlg::OnDoPipe)
	EVT_TEXT_ENTER(ID_LOCATION, PreviewDlg::OnLocationEnter)
	EVT_TEXT_ENTER(ID_PIPECMD, PreviewDlg::OnPipeCmdEnter)
#ifdef __WXMSW__
    EVT_ACTIVEX(ID_MSHTML, "TitleChange", PreviewDlg::OnMSHTMLTitleChange)
//	EVT_ACTIVEX(ID_MSHTML, "BeforeNavigate2", PreviewDlg::OnMSHTMLBeforeNavigate2X)
	EVT_ACTIVEX(ID_MSHTML, "CommandStateChange", PreviewDlg::OnMSHTMLStateChanged)
	EVT_ACTIVEX(ID_MSHTML, "DocumentComplete", PreviewDlg::OnMSHTMLDocumentComplete)
#endif
END_EVENT_TABLE()

PreviewDlg::PreviewDlg(EditorFrame& parent)
:  wxPanel (&parent),
   m_parent(parent), m_editorCtrl(NULL), m_thread(NULL), m_isOnPreview(true), m_isFirst(false),
   m_pinnedEditor(NULL), m_re_style(NULL), m_re_href(NULL) {

	// Adressbar ctrl
	m_backButton = new wxBitmapButton(this, ID_BACK, wxBitmap(left_arrow_xpm), wxDefaultPosition, wxDefaultSize, 0);
	m_backButton->SetBitmapDisabled(wxBitmap(left_arrow_gray_xpm));
	m_backButton->SetBitmapSelected(wxBitmap(left_arrow_click_xpm));
	m_backButton->SetToolTip(_("Go back one page"));
	//m_backButton->SetBitmapHover(wxBitmap(left_arrow_over_xpm));
	m_forwardButton = new wxBitmapButton(this, ID_FORWARD, wxBitmap(right_arrow_xpm), wxDefaultPosition, wxDefaultSize, 0);
	m_forwardButton->SetBitmapDisabled(wxBitmap(right_arrow_gray_xpm));
	m_forwardButton->SetBitmapSelected(wxBitmap(right_arrow_click_xpm));
	m_forwardButton->SetToolTip(_("Go forward one page"));
	//m_forwardButton->SetBitmapHover(wxBitmap(right_arrow_over_xpm));
	m_locationText = new wxTextCtrl(this, ID_LOCATION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_pinButton = new wxBitmapButton(this, ID_PIN, wxBitmap(pin1_xpm), wxDefaultPosition, wxDefaultSize, 0);
	m_pinButton->SetBitmapHover(wxBitmap(pin1_over_xpm));
	m_pinButton->SetToolTip(_("Pin preview to current page"));
	m_showOptions = new wxCheckBox(this, ID_OPTIONS, _("Options"));

	m_browser = NULL; // know to skip events sent during construction

#ifdef FEAT_BROWSER

#if defined (__WXMSW__)
	// IE Control
	m_browser = new wxIEHtmlWin(this, ID_MSHTML);
#elif defined (__WXGTK__)
	// WebKit control
	m_browser = new wxBrowser(this, ID_MSHTML);
#endif

#endif //FEAT_BROWSER

	// Option ctrls
	m_pipeCheck = new wxCheckBox(this, ID_DOPIPE, _("Pipe through command:"));
	m_cmdText = new wxTextCtrl(this, ID_PIPECMD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	wxButton* reloadButton = new wxButton(this, ID_RELOAD, _("Reload now"));

	// Layout
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* addressSizer = new wxBoxSizer(wxHORIZONTAL);
			addressSizer->Add(m_backButton, 0, wxALL, 5);
			addressSizer->Add(m_forwardButton, 0, wxALL, 5);
			addressSizer->Add(m_locationText, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			addressSizer->Add(m_pinButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			addressSizer->Add(m_showOptions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			m_mainSizer->Add(addressSizer, 0, wxEXPAND);
#ifdef FEAT_BROWSER
		m_mainSizer->Add(m_browser->GetWindow(), 1, wxEXPAND);
#endif
		m_optionSizer = new wxBoxSizer(wxHORIZONTAL);
			m_optionSizer->Add(m_pipeCheck, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			m_optionSizer->Add(m_cmdText, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);
			m_optionSizer->Add(reloadButton, 0, wxALL, 5);
			m_mainSizer->Add(m_optionSizer, 0, wxEXPAND);

	// Options are hidden as default
	m_mainSizer->Hide(m_optionSizer);

	SetSizer(m_mainSizer);

	m_idleTimer.Start();
}

PreviewDlg::~PreviewDlg() {
	//m_parent.PreviewClosed();

	// Terminate thread
	if (m_thread) m_thread->Terminate();

	// Delete temp files
	if (!m_tempPath.empty() && wxFileExists(m_tempPath)) wxRemoveFile(m_tempPath);
	if (!m_tempCssPath.empty() && wxFileExists(m_tempCssPath)) wxRemoveFile(m_tempCssPath);

	// Release compiled regex
	if (m_re_style) free(m_re_style);
	if (m_re_href) free(m_re_href);
}

void PreviewDlg::LoadSettings(const CatalystWrapper& cw) {
	// Get saved position
	bool showOptions = false;
	bool doPipe = false;
	wxString pipeCmd = wxT("markdown.pl");
	cxLOCK_READ(cw)
		catalyst.GetSettingBool(wxT("prvw_showoptions"), showOptions);
		catalyst.GetSettingBool(wxT("prvw_dopipe"), doPipe);
		catalyst.GetSettingString(wxT("prvw_pipecmd"), pipeCmd);
	cxENDLOCK

	m_showOptions->SetValue(showOptions);
	if (showOptions) m_mainSizer->Show(m_optionSizer);
	else m_mainSizer->Hide(m_optionSizer);
	m_mainSizer->Layout();

	m_pipeCheck->SetValue(doPipe);
	m_cmdText->SetValue(pipeCmd);
	if (doPipe) m_pipeCmd = pipeCmd;
}

void PreviewDlg::SaveSettings(CatalystWrapper& cw) const {
	cxLOCK_WRITE(cw)
		catalyst.SetSettingBool(wxT("prvw_showoptions"), m_showOptions->IsChecked());
		catalyst.SetSettingBool(wxT("prvw_dopipe"), m_pipeCheck->IsChecked());
		catalyst.SetSettingString(wxT("prvw_pipecmd"), m_cmdText->GetValue());
	cxENDLOCK
}

void PreviewDlg::UpdateBrowser(cxUpdateMode mode) {
	if (m_thread) return; // update in process

	// Do we have a temp file
	if (m_tempPath.empty()) {
		m_tempPath = wxFileName::CreateTempFileName(wxEmptyString);
		m_tempPath += wxT(".html");
		m_isFirst = true;
	}

	// Load the text of entire doc
	vector<char> text;
	if (m_pinnedEditor) {
		m_pinnedEditor->GetText(text);
		m_truePath = m_pinnedEditor->GetPath();
	}
	else {
		m_editorCtrl->GetText(text);
		m_truePath = m_editorCtrl->GetPath();
	}

	m_uncPath = ConvertPathToUNC(m_truePath);

	// Make sure we only update when the editor changes
	m_editorChangeToken = m_editorCtrl->GetChangeToken();
	m_editorCtrl->MarkPreviewDone();

	if (m_pipeCmd.empty()) {
		wxFile tempFile(m_tempPath, wxFile::write);
		if (!tempFile.IsOpened()) return;

		if (!text.empty()) {
			if (!m_truePath.empty()) {
				InsertBase(text, m_truePath);

				// Check if we should also save the current css file
				// (does not work with remote files as file has to be
				//  saved in same location as source)
				if (m_pinnedEditor && !m_editorCtrl->IsRemote()) {
					if (InsertStyle(text)) {
						wxFileOutputStream cssFile(m_tempCssPath);
						if (!cssFile.IsOk()) return;
						m_editorCtrl->WriteText(cssFile);
					}
				}
			}

			tempFile.Write(&*text.begin(), text.size());
		}

		if (m_isFirst) mode = cxUPDATE_RELOAD;
		RefreshBrowser(mode);
	}
	else {
		cxEnv env;
		m_editorCtrl->SetEnv(env, true);
		const wxString cmd = m_editorCtrl->GetBashCommand(m_pipeCmd, env);
		if (cmd.empty()) return;

		// Thread will send event and delete itself when done
		m_thread = new CommandThread(cmd, text, m_tempPath, m_truePath, *this, env);
	}

	m_isOnPreview = true;
}

void PreviewDlg::RefreshBrowser(cxUpdateMode mode) {

#ifdef FEAT_BROWSER
#ifdef __WXMSW__
	wxIEHtmlWin* ie = dynamic_cast<wxIEHtmlWin*>(m_browser);
	ie->UIDeactivate(); // otherwise ie may steal focus
#endif
	if (mode == cxUPDATE_RELOAD || m_isFirst) {
		m_browser->LoadUrl(m_tempPath);
	}
	else m_browser->Refresh(wxHTML_REFRESH_NORMAL);
#endif // FEAT_BROWSER

	m_isFirst = false;
	m_idleTimer.Start();
}

void PreviewDlg::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	if (!IsShown() || m_idleTimer.Time() < 100) return;

	// Check if we have changed source document
	EditorCtrl* activeEditor = m_parent.GetEditorCtrl();
	if (!activeEditor) return;

	if (m_editorCtrl != activeEditor) {
		m_editorCtrl = activeEditor;

		// Always reload on new source
		if (!m_pinnedEditor) {
			UpdateBrowser(cxUPDATE_RELOAD);
			return;
		}
	}

	// No need to refresh if user has clicked away from preview page
	if (!m_isOnPreview) return;

	// Check if document has changed
	if (m_pinnedEditor) {
		if (((m_pinnedEditor == m_editorCtrl || m_editorCtrl->GetName().EndsWith(wxT(".css")))
			&& m_editorCtrl->GetChangeToken() != m_editorChangeToken)
			|| m_editorCtrl->IsSavedForPreview() ) {

			UpdateBrowser(cxUPDATE_REFRESH);
		}
	}
	else if (m_editorCtrl->GetChangeToken() != m_editorChangeToken) {
		UpdateBrowser(cxUPDATE_REFRESH);
	}
}

bool PreviewDlg::InsertStyle(vector<char>& html) {
	// Check if we should replace style sheet ref
	if (!m_pinnedEditor) return false;
	if (html.empty()) return false;
	const wxString cssFilePath = m_editorCtrl->GetPath();
	if (!cssFilePath.EndsWith(wxT(".css"))) return false;

	// Compile patterns
	const char *error;
	int erroffset;
	if (!m_re_style) {
		m_re_style = pcre_compile(
			"<link[^>]+rel=\"stylesheet\"[^>]*>",   // the pattern
			PCRE_UTF8|PCRE_MULTILINE|PCRE_CASELESS,  // options
			&error,               // for error message
			&erroffset,           // for error offset
			NULL);                // use default character tables
	}
	if (!m_re_href) {
		m_re_href = pcre_compile(
			"href=\"(.*?.css).*?\"",   // the pattern
			PCRE_UTF8|PCRE_MULTILINE|PCRE_CASELESS,  // options
			&error,               // for error message
			&erroffset,           // for error offset
			NULL);                // use default character tables
	}

	// Do the search for style tag
	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	const int rc1 = pcre_exec(
		m_re_style,              // the compiled pattern
		NULL,                    // extra data - if we study the pattern
		&*html.begin(),            // the subject string
		wxMin(800, html.size()), // the length of the subject
		0,                       // start at offset in the subject
		PCRE_NO_UTF8_CHECK,      // options
		ovector,                 // output vector for substring information
		OVECCOUNT);              // number of elements in the output vector
	if (rc1 == 0) return false;

	const unsigned int tag_start_pos = ovector[0];
	const char* tag_start = &*html.begin() + tag_start_pos;
	const unsigned int tag_len = ovector[1] - ovector[0];

	// Do the search for href tag with css path
	const int rc2 = pcre_exec(
		m_re_href,               // the compiled pattern
		NULL,                    // extra data - if we study the pattern
		tag_start,               // the subject string
		tag_len,                 // the length of the subject
		0,                       // start at offset in the subject
		PCRE_NO_UTF8_CHECK,      // options
		ovector,                 // output vector for substring information
		OVECCOUNT);              // number of elements in the output vector

	if (rc2 == 2) {
		const unsigned int style_start = tag_start_pos + ovector[2];
		const unsigned int style_end = tag_start_pos + ovector[3];

		// Get match
		const wxString stylePath(&*html.begin()+style_start, wxConvUTF8, style_end-style_start);
		wxString localPath = cssFilePath;
		localPath.Replace(wxT("\\"), wxT("/"));

		if (localPath.EndsWith(stylePath)) {
			// Make sure we have temp file
			// Since css does not offer a base url, we have to place it in same location
			const wxString cssPath = wxPathOnly(cssFilePath);
			if (wxPathOnly(m_tempCssPath) != cssPath) {
				if (!m_tempCssPath.empty() && wxFileExists(m_tempCssPath)) wxRemoveFile(m_tempCssPath);
				m_tempCssPath = cssPath + wxFILE_SEP_PATH + wxT(".e_preview_temp.css");
				wxFile file(m_tempCssPath, wxFile::write); // create the temp file
				if (file.IsOpened()) {
					file.Close(); // allow file to be modified
#ifdef __WXMSW__
					// Make sure the file is marked as hidden and temporary
					//::SetFileAttributes(m_tempCssPath, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NOT_CONTENT_INDEXED|FILE_ATTRIBUTE_TEMPORARY); // Hiding seems to give problems on Vista
					::SetFileAttributes(m_tempCssPath, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED|FILE_ATTRIBUTE_TEMPORARY);
#endif
				}
			}

			const wxCharBuffer buff = m_tempCssPath.mb_str(wxConvUTF8);
			html.erase(html.begin()+style_start, html.begin()+style_end);
			html.insert(html.begin()+style_start, buff.data(), buff.data()+strlen(buff.data()));

			return true;
		}
	}

	return false;
}

void PreviewDlg::InsertBase(vector<char>& html, const wxString& path) { // static
	if (path.empty()) return;

	const char* basestart = "<base href=\"";
	const char* baseend = "\" />\n";
	vector<char> base;

	// Build base string
	base.insert(base.end(), basestart, basestart + strlen(basestart));

	const wxCharBuffer buf = path.mb_str(wxConvUTF8);
	const unsigned int len = strlen(buf.data());
	base.insert(base.end(), buf.data(), buf.data()+len);

	base.insert(base.end(), baseend, baseend + strlen(baseend));

	// Check if we have a head tag in the first 200 bytes
	vector<char>::iterator insertpos = html.begin();
	unsigned int end = wxMin(300, html.size());
	if (end > 6) {
		end -= 6;
		for (vector<char>::iterator c = html.begin(); c != html.begin() + end; ++c) {
			if (*c == '<' && *(c+1) == 'h' && *(c+2) == 'e' && *(c+3) == 'a' && *(c+4) == 'd' && *(c+5) == '>') {
				insertpos = c + 6;
				break;
			}
		}
	}

	html.insert(insertpos, base.begin(), base.end());
}

wxString PreviewDlg::ConvertPathToUNC(const wxString& path) { // static
	if (path.empty()) return wxEmptyString;

	wxString uncPath = path;

	uncPath.Replace(wxT(" "), wxT("%20"));
	uncPath.Replace(wxT("\\"), wxT("/"));
	uncPath.Prepend(wxT("file:///"));

	return uncPath;
}

void PreviewDlg::OnShowOptions(wxCommandEvent& event) {
	if (event.IsChecked()) m_mainSizer->Show(m_optionSizer);
	else m_mainSizer->Hide(m_optionSizer);
	m_mainSizer->Layout();
}

void PreviewDlg::OnDoPipe(wxCommandEvent& event) {
	if (event.IsChecked()) {
		m_pipeCmd = m_cmdText->GetValue();
	}
	else m_pipeCmd.clear();

	UpdateBrowser(cxUPDATE_REFRESH);
}

void PreviewDlg::OnPipeCmdEnter(wxCommandEvent& WXUNUSED(event)) {
	if (m_pipeCheck->IsChecked()) {
		m_pipeCmd = m_cmdText->GetValue();
	}
	else m_pipeCmd.clear();

	UpdateBrowser(cxUPDATE_REFRESH);
}

void PreviewDlg::OnButtonBack(wxCommandEvent& WXUNUSED(event)) {
#ifdef FEAT_BROWSER
	m_browser->GoBack();
#endif
}

void PreviewDlg::OnButtonForward(wxCommandEvent& WXUNUSED(event)) {
#ifdef FEAT_BROWSER
	m_browser->GoForward();
#endif
}

void PreviewDlg::OnButtonPin(wxCommandEvent& WXUNUSED(event)) {
	if (m_pinnedEditor) {
		m_pinnedEditor = NULL;
		m_pinButton->SetBitmapLabel(wxBitmap(pin1_xpm));
		m_pinButton->SetBitmapHover(wxBitmap(pin1_over_xpm));

		// Remove temp preview files
		if (!m_tempCssPath.empty() && wxFileExists(m_tempCssPath)) {
			wxRemoveFile(m_tempCssPath);
			m_tempCssPath.clear();
		}

		UpdateBrowser(cxUPDATE_REFRESH);
	}
	else {
		m_pinnedEditor = m_parent.GetEditorCtrl();
		m_pinButton->SetBitmapLabel(wxBitmap(pin2_xpm));
		m_pinButton->SetBitmapHover(wxBitmap(pin2_over_xpm));
	}
}

void PreviewDlg::PageClosed(const EditorCtrl* ec) {
	if (m_pinnedEditor && ec == m_pinnedEditor) {
		m_pinnedEditor = NULL;
		m_pinButton->SetBitmapLabel(wxBitmap(pin1_xpm));
		m_pinButton->SetBitmapHover(wxBitmap(pin1_over_xpm));

		// It will update on next idle
	}
}

void PreviewDlg::OnLocationEnter(wxCommandEvent& WXUNUSED(event)) {
	const wxString location = m_locationText->GetValue();
	if (location.empty()) return;

	if (location == m_uncPath) {
		UpdateBrowser(cxUPDATE_RELOAD);
	}
	else {
#ifdef FEAT_BROWSER
		m_browser->LoadUrl(location, wxEmptyString, true);
#endif
	}
}

void PreviewDlg::OnButtonReload(wxCommandEvent& WXUNUSED(event)) {
	if (m_pipeCheck->GetValue()) {
		m_pipeCmd = m_cmdText->GetValue();
	}
	else m_pipeCmd.clear();

	UpdateBrowser(cxUPDATE_RELOAD);
}

void PreviewDlg::OnEndProcess(wxProcessEvent& event) {
	m_thread = NULL;
	if (event.GetExitCode() == -1) return;

	RefreshBrowser(cxUPDATE_REFRESH);
}

#ifdef __WXMSW__
void PreviewDlg::OnMSHTMLTitleChange(wxActiveXEvent& event) {
	const wxString title = event[wxT("Text")];

	const wxString tempFile = m_tempPath.AfterLast(wxT('\\'));
	const wxString titleEnd = title.AfterLast(wxT('\\'));

	// This is a bit of a hack, but the easiest way to see if
	// it does not have a title and returns path is to compare
	// filenames (very litle chance for match with other)
	if (tempFile != titleEnd) {
		m_parent.SetWebPreviewTitle(wxT("Preview: ") + title);
	}
	else {
		m_parent.SetWebPreviewTitle(wxT("Preview: ") + m_truePath);
	}

}

void PreviewDlg::OnMSHTMLStateChanged(wxActiveXEvent& event) {
	long cmd = event[wxT("Command")];
	bool state = event[wxT("Enable")];
	if (cmd == -1) return;

	if (cmd & CSC_NAVIGATEBACK) {
		m_backButton->Enable(state);
	}

	if (cmd & CSC_NAVIGATEFORWARD) {
		m_forwardButton->Enable(state);
	}
}

void PreviewDlg::OnMSHTMLDocumentComplete(wxActiveXEvent& WXUNUSED(event)) {
	if (!m_browser) return; // event can be sent during construction

	const wxString location = m_browser->GetRealLocation();

	if (location != m_locationText->GetValue()) {
		const wxString tempFile = m_tempPath.AfterLast(wxT('\\'));
		const wxString locFile = location.AfterLast(wxT('/'));

		// This is a bit of a hack, but the easiest way to see if
		// we are in the preview file is to compare filenames
		if (tempFile != locFile) {
			m_locationText->SetValue(location);
			m_isOnPreview = false;
		}
		else {
			m_locationText->SetValue(m_uncPath);
			m_isOnPreview = true;
		}
	}
}
#endif // __WXMSW__

// ------ CommandThread -----------------------------------------------------

PreviewDlg::CommandThread::CommandThread(const wxString& command, vector<char>& input, const wxString& outputPath, const wxString& truePath, PreviewDlg& parent, const cxEnv& env)
: m_isTerminated(false), m_command(command), m_parent(parent), m_outputPath(outputPath),
  m_truePath(truePath), m_env(env) {
	m_input.swap(input); // copy as calling function can go out of scope

	// Start the thread
	Create();
	Run();
}

void* PreviewDlg::CommandThread::Entry() {
	cxExecute exec(m_env);
	//exec.SetDebugLogging(true);

	int resultCode = exec.Execute(m_command, m_input);

	// Write the output	to file
	if (!m_isTerminated && resultCode != -1) {
		wxFile tempFile(m_outputPath, wxFile::write);

		if (tempFile.IsOpened()) {
			vector<char> output;
			output.swap(exec.GetOutput());

			if (!output.empty()) {
				PreviewDlg::InsertBase(output, m_truePath);
				tempFile.Write(&*output.begin(), output.size());
			}
		}
	}

	if (m_isTerminated) return NULL;

	// Notify parent that the command is done
	wxProcessEvent event(99, 0, resultCode);
	m_parent.AddPendingEvent(event);

	return NULL;
}
