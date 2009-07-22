#ifndef __IOPENTEXTMATEURL_H__
#define __IOPENTEXTMATEURL_H__

class wxString;

class IOpenTextmateURL {
public:
	virtual bool OpenTxmtUrl(const wxString& url) = 0;
};

#endif
