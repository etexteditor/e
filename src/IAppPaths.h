#ifndef __IAPPPATHS_H__
#define __IAPPPATHS_H__

class IAppPaths {
public:
	virtual const wxString& AppPath() const = 0;
	virtual const wxString& AppDataPath() const = 0;
	virtual wxString CreateTempAppDataFile() = 0;
};

IAppPaths& GetAppPaths(void);

#endif // __IAPPPATHS_H__
