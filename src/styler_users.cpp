#include "styler_users.h"

Styler_Users::Styler_Users(const CatalystWrapper& cat, const DocumentWrapper& doc)
: m_catalyst(cat), m_doc(doc), m_isActive(true) {
}

void Styler_Users::Style(StyleRun& sr) {
	if (!m_isActive) return;

	const unsigned int rstart =  sr.GetRunStart();
	const unsigned int rend = sr.GetRunEnd();
	unsigned int pos = rstart;
	
	cxLOCKDOC_READ(m_doc)
		const Catalyst& catalyst = m_catalyst.GetCatalyst();
		
		while (pos < rend) {
			const cxNodeInfo ni = doc.GetNodeInfo(pos);

			if (ni.source.IsDocument()) {
				const unsigned int userId = catalyst.GetDocAuthor(ni.source);
				if (userId > 0) {
					const unsigned int end = wxMin(ni.end, rend);

					const wxColour userColor = catalyst.GetUserColor(userId);
					sr.SetBackgroundColor(pos, end, userColor);
					sr.SetShowHidden(pos, end, true);
				}
			}

			pos = wxMin(ni.end, rend);
		}
	cxENDLOCK
}
