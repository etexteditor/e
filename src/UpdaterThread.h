#ifndef __UPDATERTHREAD_H__
#define __UPDATERTHREAD_H__

#include <wx/thread.h>

// Constants
enum {
	ID_UPDATES_AVAILABLE=100,
	ID_UPDATES_CHECKED
};

class ISettings;
class AppVersion;
class wxHTTP;
class AppVersion;
class SettingsDlg;

class UpdaterThread : public wxThread {
public:
	UpdaterThread(wxHTTP* http, AppVersion* info);
	virtual void *Entry();
	// set dialog to be notified after checking or NULL
	void SetWatcher(SettingsDlg *dlg);

private:
	wxHTTP* m_http;
	AppVersion* m_info;

	wxCriticalSection m_CritSectWatcher; // locker for m_Watcher ptr
	SettingsDlg *m_Watcher;
};

UpdaterThread* CheckForUpdates(ISettings& settings, AppVersion* info, bool forceCheck=false);

#endif
