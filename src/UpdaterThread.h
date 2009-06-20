#ifndef __UPDATERTHREAD_H__
#define __UPDATERTHREAD_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/thread.h>
#endif

class eSettings;
void CheckForUpdates(eSettings& settings, bool forceCheck=false);

#endif
