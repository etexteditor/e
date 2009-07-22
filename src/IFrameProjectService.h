#ifndef __IFRAMEPROJECTSERVICE_H__
#define __IFRAMEPROJECTSERVICE_H__

#include "IFrameRemoteThread.h"
#include <wx/fontenc.h>

class wxFileName;
class wxString;
class RemoteProfile;
class DirWatcher;

// EditorFrame services as used by the ProjectPane
class IFrameProjectService : public IFrameRemoteThread {
public:
	// Opening local and remote files.
	virtual bool OpenFile(const wxFileName& path, wxFontEncoding enc=wxFONTENCODING_SYSTEM, const wxString& mate=wxEmptyString) = 0;
	virtual bool OpenRemoteFile(const wxString& url, const RemoteProfile* rp=NULL) = 0;
	virtual bool CloseTab(unsigned int tab_id, bool removetab) = 0;
	virtual void UpdateRenamedFile(const wxFileName& path, const wxFileName& newPath) = 0;

	// Prompting for remote credentials.
	virtual bool AskRemoteLogin(const RemoteProfile* rp) = 0;

	// Directory monitoring.
	virtual DirWatcher& GetDirWatcher() = 0;
};

#endif // __IFRAMEPROJECTSERVICE_H__
