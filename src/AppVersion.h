#ifndef __APPVERSION_H__
#define __APPVERSION_H__

class wxString;

class AppVersion {
public:
	AppVersion(const wxString& _appid, const unsigned int _version, const bool _isregistered, const int _daysleftoftrial):
	  AppId(_appid), Version(_version), IsRegistered(_isregistered), DaysLeftOfTrial(_daysleftoftrial) {};

	const wxString AppId;
	const unsigned int Version;
	const bool IsRegistered;
	const int DaysLeftOfTrial;
};

AppVersion* GetAppVersion(void);

#endif
