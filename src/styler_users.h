#ifndef __STYLER_USERS_H__
#define __STYLER_USERS_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include "Document.h"
#include "StyleRun.h"
#include "styler.h"

class Styler_Users : public Styler {
public:
	Styler_Users(const CatalystWrapper& cat, const DocumentWrapper& doc);

	void Style(StyleRun& sr);

	bool IsActive() const {return m_isActive;};
	void Enable() {m_isActive = true;};
	void Disable() {m_isActive = false;};

private:
	const CatalystWrapper& m_catalyst;
	const DocumentWrapper& m_doc;
	bool m_isActive;
};

#endif // __STYLER_USERS_H__
