#ifndef __ITMGETSYNTAXES_H__
#define __ITMGETSYNTAXES_H__

#include "SyntaxInfo.h"

class ITmGetSyntaxes {
public:
	virtual const std::vector<cxSyntaxInfo*>& GetSyntaxes() const = 0;
};

#endif // __ITMGETSYNTAXES_H__
