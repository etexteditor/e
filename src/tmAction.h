#ifndef __TMACTION_H__
#define __TMACTION_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

#include <vector>
#include <functional>

#include "tmKey.h"

struct tmBundle;

class tmAction {
public:
	tmAction() :  isUnix(true), bundle(NULL), contentLoaded(false) {};
	virtual ~tmAction() {};

	virtual bool IsSnippet() const {return false;};
	virtual bool IsCommand() const {return false;};
	virtual bool IsDrag() const {return false;};
	virtual bool IsSyntax() const {return false;};
	virtual bool IsMacro() const {return false;};
	void SwapContent(std::vector<char>& c) {
		cmdContent.swap(c);
		contentLoaded = true;
	};
	void SetContent(const wxString& cmdStr) {
		const wxCharBuffer buf = cmdStr.mb_str(wxConvUTF8);
		const size_t len = strlen(buf.data());
		cmdContent.assign(buf.data(), buf.data()+len);
		contentLoaded = true;
	};

	// Member variables
	bool isUnix;
	wxString name;
	wxString uuid;
	wxString scope;
	wxString trigger;
	tmKey key;
	const tmBundle* bundle;

private:
	friend class TmSyntaxHandler;

	unsigned int plistRef;
	mutable bool contentLoaded;
	mutable std::vector<char> cmdContent;
};

class tmActionCmp : public std::binary_function<tmAction*, tmAction*, bool> {
public:
	bool operator()(const tmAction* x, const tmAction* y) const {return x->name.CmpNoCase(y->name) < 0;};
};

#endif // __TMACTION_H__
