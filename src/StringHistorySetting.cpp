#include "StringHistorySetting.h"

StringHistorySetting::StringHistorySetting(void)
{
}

StringHistorySetting::~StringHistorySetting(void)
{
}

/*

size_t eSettings::GetReplaceCount() const {
	if (!m_jsonRoot.HasMember(wxT("replaceHistory"))) return 0;

	const wxJSONValue replacements = m_jsonRoot.ItemAt(wxT("replaceHistory"));
	return replacements.Size();
}

wxString eSettings::GetReplace(size_t ndx) const {
	const wxJSONValue replacements = m_jsonRoot.ItemAt(wxT("replaceHistory"));
	wxASSERT((int)ndx < replacements.Size());

	return replacements.ItemAt(ndx).AsString();
}

bool eSettings::AddReplace(const wxString& pattern) {
	wxJSONValue& replacements = m_jsonRoot[wxT("replaceHistory")];

	// Don't add duplicates
	if (replacements.Size() > 0) {
		const wxJSONValue last = replacements.ItemAt(0);
		if (last.AsString() == pattern) return false;
		
		// Check if there should be a duplicate lower down
		for (int i = 0; i < replacements.Size(); ++i) {
			if (replacements[i].AsString() == pattern) {
				replacements.Remove(i);
				break;
			}
		}
	}
	
	// Add the new item
	replacements.Insert(0, pattern);

	// Limit number of items to save
	if (replacements.Size() > 20) replacements.Remove(replacements.Size()-1);

	return true;
}

*/
