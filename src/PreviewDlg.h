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

#ifndef __PREVIEWDLG_H__
#define __PREVIEWDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "Env.h"

#ifdef __WXMSW__
#include "IEHtmlWin.h"
#else
#include "IHtmlWnd.h"
#endif

using namespace std;

// pre-declarations
struct real_pcre;                 // This double pre-definition is needed
typedef struct real_pcre pcre;    // because of the way it is defined in pcre.h
class EditorFrame;
class EditorCtrl;
class eFrameSettings;
class wxProcessEvent;
class wxWebControl;
class wxWebEvent;

class PreviewDlg : public wxPanel {
public:
	PreviewDlg(EditorFrame& parent);
	~PreviewDlg();

	void LoadSettings(const eFrameSettings& settings);
	void SaveSettings(eFrameSettings& settings) const;
	void PageClosed(const EditorCtrl* ec);

	// Utility functions
	bool InsertStyle(vector<char>& html);
	static void InsertBase(vector<char>& html, const wxString& path);

private:
	class CommandThread : public wxThread {
	public:
		CommandThread(const wxString& command, vector<char>& input, const wxString& outputPath, const wxString& truePath, wxEvtHandler& parent, const cxEnv& env);
		void Terminate() {m_isTerminated = true;};
		virtual void *Entry();
	private:
		bool m_isTerminated;
		const wxString m_command;
		wxEvtHandler& m_parent;
		vector<char> m_input;
		const wxString& m_outputPath;
		const wxString& m_truePath;
		const cxEnv m_env;
	};

	enum cxUpdateMode {
		cxUPDATE_RELOAD,
		cxUPDATE_REFRESH
	};

	void UpdateBrowser(cxUpdateMode mode=cxUPDATE_REFRESH);
	void RefreshBrowser(cxUpdateMode mode);
	void SetBrowser(int sel);

	void OnTitleChange(const wxString& title);
	void OnDocumentComplete(const wxString& location);

	// Event handlers
	void OnIdle(wxIdleEvent& event);
	void OnButtonReload(wxCommandEvent& event);
	void OnButtonBack(wxCommandEvent& event);
	void OnButtonForward(wxCommandEvent& event);
	void OnButtonPin(wxCommandEvent& event);
	void OnLocationEnter(wxCommandEvent& event);
	void OnPipeCmdEnter(wxCommandEvent& event);
	void OnDoPipe(wxCommandEvent& event);
	void OnShowOptions(wxCommandEvent& event);
	void OnEndProcess(wxProcessEvent& event);
	void OnWebChoice(wxCommandEvent& event);
#ifdef __WXMSW__
	void OnMSHTMLTitleChange(wxActiveXEvent& event);
	void OnMSHTMLStateChanged(wxActiveXEvent& event);
	void OnMSHTMLDocumentComplete(wxActiveXEvent& event);
#endif
	void OnWebTitleChange(wxWebEvent& event);
	void OnWebDocumentComplete(wxWebEvent& event);
	DECLARE_EVENT_TABLE()


	// Member variables
	EditorFrame& m_parent;
	EditorCtrl* m_editorCtrl;
	wxStopWatch m_idleTimer;
	wxString m_tempPath;
	wxString m_tempCssPath;
	wxString m_truePath;
	wxString m_uncPath;
	CommandThread* m_thread;
	wxString m_pipeCmd;
	bool m_isOnPreview;
	bool m_isFirst;
	EditorCtrl* m_pinnedEditor;
	unsigned int m_editorChangeToken;

	pcre* m_re_style;
	pcre* m_re_href;

	// Member Ctrls
#ifdef __WXMSW__
	wxIEHtmlWin* m_browser;
#else
	IHtmlWnd* m_browser;
#endif
	wxWebControl* m_webcontrol;

	wxCheckBox* m_pipeCheck;
	wxTextCtrl* m_cmdText;
	wxBitmapButton* m_backButton;
	wxBitmapButton* m_forwardButton;
	wxTextCtrl* m_locationText;
	wxBitmapButton* m_pinButton;
	wxCheckBox* m_showOptions;
	wxChoice* m_webChoice;

	wxBoxSizer* m_mainSizer;
	wxBoxSizer* m_optionSizer;

};

#endif // __PREVIEWDLG_H__
