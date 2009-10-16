#ifndef __UPDATERTHREAD_H__
#define __UPDATERTHREAD_H__

// Constants
#define ID_UPDATES_AVAILABLE 100

class ISettings;
class AppVersion;
void CheckForUpdates(ISettings& settings, AppVersion* info, bool forceCheck=false);

#endif
