#ifndef __ITMLOADBUNDLES_H__
#define __ITMLOADBUNDLES_H__

#include "IGetPListHandlerRef.h"

enum cxBundleLoad {
	cxINIT,
	cxUPDATE,
	cxRELOAD
};

class ITmLoadBundles: public virtual IGetPListHandlerRef {
public:
	virtual void LoadBundles(cxBundleLoad mode) = 0;
	virtual void ReParseBundles(bool onlyMenu=false) = 0;
};

#endif // __ITMLOADBUNDLES_H__
