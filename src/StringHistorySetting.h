#ifndef __STRINGHISTORYSETTING_H__
#define __STRINGHISTORYSETTING_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

class wxJSONValue;

class StringHistorySetting
{
public:
	StringHistorySetting(const wxString& key, size_t maxItems=20);

	size_t GetItemCount(const wxJSONValue& root) const;
	wxString GetItem(const wxJSONValue& root, size_t ndx) const;
	bool AddItem(wxJSONValue& root, const wxString& item);

private:
	const wxString m_key;
	const size_t m_maxItems;
};

#endif
