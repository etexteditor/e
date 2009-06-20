#include "UpdaterThread.h"

#include <wx/dialup.h>
#include <wx/protocol/http.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>

#include "eApp.h"
#include "eSettings.h"


class UpdaterThread : public wxThread {
public:
	UpdaterThread(wxHTTP* http) : m_http(http) {};
	virtual void *Entry();

private:
	wxHTTP* m_http;
};


void CheckForUpdates(eSettings& settings, bool forceCheck) {
	if (!forceCheck) {
		// Check if it more than 7 days have gone since last update check
		wxLongLong lastup;
		if (settings.GetSettingLong(wxT("lastupdatecheck"), lastup)) {
			wxDateTime sevendaysago = wxDateTime::Now() - wxDateSpan(0, 0, 1, 0);
			wxDateTime lastupdated(lastup);

			if (lastupdated > sevendaysago) return;
		}
	}

	// Check if we have a internet connection
	wxDialUpManager* dup = wxDialUpManager::Create();
	if (dup->IsOnline()) {
		// We have to create the http protocol in main thread
		// it will be deleted in the updater thread
		wxHTTP* http = new wxHTTP;

		// Start the updater thread
		UpdaterThread *updater = new UpdaterThread(http);
		if (updater->Create() == wxTHREAD_NO_ERROR)	updater->Run();
	}
	delete dup; // Clean up
}


// Checking for updates is done in a seperate thread since the
// socket can block for some time and we don't want the main thread
// to lock up

void* UpdaterThread::Entry() {
	wxASSERT(m_http);

	const eApp& app = wxGetApp();
	const unsigned int thisVersion = app.VersionId();

	// Set a cookie so the server can count unique requests
	wxString cookie;
	if (app.IsRegistered()) cookie = wxString::Format(wxT("%s.%u.*"), app.GetId().ToString().c_str(), thisVersion);
	else cookie = wxString::Format(wxT("%s.%u.%d"), app.GetId().ToString().c_str(), thisVersion, app.DaysLeftOfTrial());
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
					if (id > thisVersion) {
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
					if (id > thisVersion) {
						newrelease = true;
						break;
					}
				}
#endif  //__WXDEBUG__
			}
			delete pStream; // Clean up

			// Remember this time as last update checked
			wxLongLong now = wxDateTime::Now().GetValue();
			eGetSettings().SetSettingLong(wxT("lastupdatecheck"), now);
		}
	}

	// Clean up
	delete m_http;

	if (newrelease) {
		// Notify main thread that there are new updates available
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, ID_UPDATES_AVAILABLE);
		wxGetApp().AddPendingEvent(event);
	}

	return NULL;
}
