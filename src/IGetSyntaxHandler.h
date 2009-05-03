#ifndef __IGETSYNTAXHANDLER_H__
#define __IGETSYNTAXHANDLER_H__

class TmSyntaxHandler;

class IGetSyntaxHandler {
public:
	virtual TmSyntaxHandler& GetSyntaxHandler() const = 0;
};

#endif // __IGETSYNTAXHANDLER_H__
