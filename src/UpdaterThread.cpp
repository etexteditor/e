#include "UpdaterThread.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/thread.h>
#endif

#include <wx/protocol/http.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>

#include "eSettings.h"
#include "AppVersion.h"

class UpdaterThread : public wxThread {
public:
	UpdaterThread(wxHTTP* http, AppVersion* info):
		m_http(http), m_info(info) {};
	virtual void *Entry();

private:
	wxHTTP* m_http;
	AppVersion* m_info;
};


void CheckForUpdates(ISettings& settings, AppVersion* info, bool forceCheck) {
	if (!forceCheck) {
		// Check if it more than 7 days have gone since last update check
		wxLongLong lastup;
		if (settings.GetSettingLong(wxT("lastupdatecheck"), lastup)) {
			wxDateTime sevendaysago = wxDateTime::Now() - wxDateSpan(0, 0, 1, 0);
			wxDateTime lastupdated(lastup);

			if (lastupdated > sevendaysago) return;
		}
	}

	// We have to create the http protocol in main thread
	// it will be deleted in the updater thread
	wxHTTP* http = new wxHTTP;

	// Start the updater thread
	UpdaterThread *updater = new UpdaterThread(http, info);
	if (updater->Create() == wxTHREAD_NO_ERROR)	updater->Run();
}

bool DoCheckForUpdates(wxHTTP* m_http, AppVersion* m_info) {
	wxASSERT(m_http);

	// Set a cookie so the server can count unique requests
	wxString cookie;
	if (m_info->IsRegistered)
		cookie = wxString::Format(wxT("%s.%u.*"), m_info->AppId.c_str(), m_info->Version);
	else
		cookie = wxString::Format(wxT("%s.%u.%d"), m_info->AppId.c_str(), m_info->Version, m_info->DaysLeftOfTrial);

	m_http->SetHeader(wxT("Cookie"), cookie);

	bool newrelease = false;
	if (m_http->Connect(wxT("www.e-texteditor.com"))) {
		wxInputStream* pStream = m_http->GetInputStream(wxT("/cgi-bin/status"));
		if (pStream) {
			wxTextInputStream text(*pStream);

			while (!pStream->Eof()) {
				wxString line = text.ReadLine();
				if (line.empty()) break;

				if (line.StartsWith(wxT("release"))) {
					wxString id_str = line.AfterFirst(wxT('='));
					id_str = id_str.BeforeFirst(wxT(' '));
					unsigned long id;
					id_str.ToULong(&id);
					if (id > m_info->Version) {
						newrelease = true;
						break;
					}
				}
#ifdef __WXDEBUG__
				else if (line.StartsWith(wxT("beta"))) {
					wxString id_str = line.AfterFirst(wxT('='));
					id_str = id_str.BeforeFirst(wxT(' '));
					unsigned long id;
					id_str.ToULong(&id);
					if (id > m_info->Version) {
						newrelease = true;
						break;
					}
				}
#endif  //__WXDEBUG__
			}
			delete pStream; // Clean up
		}
	}

	// Clean up
	delete m_http;
	delete m_info;

	return newrelease;
}


// Checking for updates is done in a seperate thread since the
// socket can block for some time and we don't want the main thread
// to lock up

void* UpdaterThread::Entry() {
	if(DoCheckForUpdates(m_http, m_info)) {
		// Notify main thread that there are new updates available
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UPDATES_AVAILABLE);
		wxTheApp->AddPendingEvent(event);
	}
	else {
		// Notify main thread that we have finished checking for updates
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UPDATES_CHECKED);
		wxTheApp->AddPendingEvent(event);
	}

	return NULL;
}
