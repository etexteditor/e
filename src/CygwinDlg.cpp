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

#include "CygwinDlg.h"
#include "EditorCtrl.h"
#include "eApp.h"
#include "Execute.h"
#include "eDocumentPath.h"

BEGIN_EVENT_TABLE(CygwinDlg, wxDialog)
	EVT_BUTTON(wxID_OK, CygwinDlg::OnButtonOk)
END_EVENT_TABLE()

CygwinDlg::CygwinDlg(wxWindow *parent, CatalystWrapper& cw, cxCygwinDlgMode mode)
: wxDialog (parent, wxID_ANY, wxEmptyString, wxDefaultPosition), m_catalyst(cw), m_mode(mode) {
	if (m_mode == cxCYGWIN_INSTALL) SetTitle(_("Cygwin not installed!"));
	else SetTitle(_("Update Cygwin"));

	const wxString installMsg = _("e uses the Cygwin package to provide many of the powerful Unix-like commands not usually available on Windows. Without Cygwin, some of e's advanced features will be disabled.\n\nWould you like to install Cygwin now? (If you say no, e will ask you again when you try to use an advanced feature.)");
	//const wxString updateMsg = installMsg;
	const wxString updateMsg = _("To get full benefit of the bundle commands, your cygwin installation needs to be updated.");

	// Create controls
	wxStaticText* msg = new wxStaticText(this, wxID_ANY, (m_mode == cxCYGWIN_INSTALL) ? installMsg : updateMsg);
	msg->Wrap(300);
	wxStaticText* radioTitle = new wxStaticText(this, wxID_ANY, (m_mode == cxCYGWIN_INSTALL) ? _("Install Cygwin:") : _("Update Cygwin"));
	m_autoRadio = new wxRadioButton(this, wxID_ANY, _("Automatic"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_manualRadio = new wxRadioButton(this, wxID_ANY, _("Manual"));

	// Create Layout
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(msg, 0, wxALL, 5);
		mainSizer->Add(radioTitle, 0, wxLEFT|wxTOP, 5);
		wxBoxSizer* radioSizer = new wxBoxSizer(wxVERTICAL);
			radioSizer->Add(m_autoRadio, 0, wxLEFT, 20);
			radioSizer->Add(m_manualRadio, 0, wxLEFT, 20);
			mainSizer->Add(radioSizer, 0, wxALL, 5);
		mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	SetSizerAndFit(mainSizer);
	Centre();
}

void CygwinDlg::OnButtonOk(wxCommandEvent& WXUNUSED(event)) {
	const wxString appPath = ((eApp*)wxTheApp)->GetAppPath();
	const cxCygwinInstallMode install_mode = m_autoRadio->GetValue() ? cxCYGWIN_AUTO : cxCYGWIN_MANUAL;
	new CygwinInstallThread(m_catalyst, install_mode, appPath);

	EndModal(wxID_OK);
}

// ---- CygwinInstallThread ------------------------------------------------------------

CygwinDlg::CygwinInstallThread::CygwinInstallThread(CatalystWrapper& cw, cxCygwinInstallMode mode, const wxString& appPath)
: m_catalyst(cw), m_mode(mode), m_appPath(appPath) {
	Create();
    Run();
}

void* CygwinDlg::CygwinInstallThread::Entry() {
	wxString cygPath = eDocumentPath::GetCygwinDir();
	if (cygPath.empty()) {
		// Make sure it get eventual proxy settings from IE
		wxFileName::Mkdir(wxT("C:\\cygwin\\etc\\setup\\"), 0777, wxPATH_MKDIR_FULL);
		wxFile file(wxT("C:\\cygwin\\etc\\setup\\last-connection"), wxFile::write);
		file.Write(wxT("IE"));

		cygPath = wxT("C:\\cygwin");
	}
	const wxString supportPath = m_appPath + wxT("Support\\bin");
	const wxString packages = wxT("curl,wget,tidy,perl,python,ruby,file,libiconv");

	const wxString cmd = wxString::Format(wxT("%s\\cygwin-setup-p.exe"), supportPath);
	wxString options;
	if (m_mode == cxCYGWIN_AUTO) {
		options = wxString::Format(wxT("--quiet-mode --site http://mirrors.dotsrc.org/cygwin/ --package %s  --root \"%s\""), packages, cygPath);
	}
	else {
		options = wxString::Format(wxT("--package %s"), packages);
	}

	// Run Setup
	// We have to use ShellExecute to enable Vista UAC elevation
	SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOASYNC|SEE_MASK_NOCLOSEPROCESS;
    sei.nShow = SW_SHOWNORMAL;
    sei.lpFile = cmd.c_str();
	sei.lpParameters = options.c_str();
	if (!::ShellExecuteEx(&sei)) return NULL;

	// Wait for setup to complete
	if (sei.hProcess != 0) {
		::WaitForSingleObject(sei.hProcess, INFINITE);
		::CloseHandle(sei.hProcess);
	}

	// Path may have been changed during install
	cygPath = eDocumentPath::GetCygwinDir();
	if (cygPath.empty()) return NULL;

	// Setup environment
	cxEnv env;
	env.SetToCurrent();
	env.SetEnv(wxT("TM_SUPPORT_PATH"), eDocumentPath::WinPathToCygwin(m_appPath + wxT("Support"))); // needed by post-install

	// Run postinstall
	cxExecute exec(env);
	exec.SetUpdateWindow(false);
	exec.SetShowWindow(false);
	const wxString supportScript = supportPath + wxT("\\cygwin-post-install.sh");
	const wxString postCmd = wxString::Format(wxT("\"%s\\bin\\bash\" --login \"%s\""), cygPath, supportScript);
	const int postexit = exec.Execute(postCmd);
	if (postexit != 0) {
		wxLogDebug(wxT("post-install failed %d"), postexit);
		// return NULL; // don't return. May fail on Vista
	}

	// Mark this update as done
	const wxFileName supportFile(supportScript);
	const wxDateTime updateTime = supportFile.GetModificationTime();
	cxLOCK_WRITE(m_catalyst)
		catalyst.SetSettingLong(wxT("cyg_date"), updateTime.GetValue());
	cxENDLOCK

	return NULL;
}
