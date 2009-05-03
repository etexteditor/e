#ifndef __IAPPPATHS_H__
#define __IAPPPATHS_H__

class IAppPaths {
public:
	virtual const wxString& GetAppPath() const = 0;
	virtual const wxString& GetAppDataPath() const = 0;
};

#endif // __IAPPPATHS_H__
