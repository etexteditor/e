#ifndef __IGETPLISTHANDLERREF_H__
#define __IGETPLISTHANDLERREF_H__

class PListHandler;

class IGetPListHandlerRef {
public:
	virtual PListHandler& GetPListHandler() = 0;
};

#endif // __IGETPLISTHANDLERREF_H__
