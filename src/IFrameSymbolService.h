#ifndef __IFRAMESYMBOLSERVICE_H__
#define __IFRAMESYMBOLSERVICE_H__

#include "IFrameEditorService.h"

class IFrameSymbolService : public IFrameEditorService {
public:
	virtual void CloseSymbolList() = 0;
};

#endif //__IFRAMESYMBOLSERVICE_H__
