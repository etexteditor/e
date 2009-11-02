#ifndef __UPDATERTHREAD_H__
#define __UPDATERTHREAD_H__

// Constants
enum {
	ID_UPDATES_AVAILABLE=100,
	ID_UPDATES_CHECKED
};

class ISettings;
class AppVersion;
void CheckForUpdates(ISettings& settings, AppVersion* info, bool forceCheck=false);

#endif
