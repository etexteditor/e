#include "stdafx.h"
#include <limits.h>
#include "Document.h"
#include "ISettings.h"
#include "Support.h"
#include <gtest/gtest.h>

class NoSettings: public ISettings {
public:
	virtual bool GetSettingBool(const wxString& name, bool& value) const { return false; };
	virtual bool GetSettingInt(const wxString& name, int& value) const { return false; };
	virtual bool GetSettingLong(const wxString& name, wxLongLong& value) const { return false; };
	virtual bool GetSettingString(const wxString& name, wxString& value) const { return false; };
};

TEST(documentNewlines, conversion) {
	wxString edb;
	if (!RequireEdb(edb)) {
		FAIL() << "Need to copy a registered e.db into this folder for this test to run.";
	}

	Catalyst* pCatalyst= new Catalyst(edb);
	CatalystWrapper* cw = new CatalystWrapper(*pCatalyst);

	{ // Put the document in a scope, so it falls out before Catalyst does.
		Document m_doc(*cw);
		const NoSettings settings;
		//RecursiveCriticalSectionLocker cx_lock(GetReadLock());
		m_doc.CreateNew(settings);

		//cxLOCKDOC_WRITE(m_doc)
		//	doc.Freeze();
		//cxENDLOCK

		//cxLOCKDOC_WRITE(m_doc)
		//	doc.Close();
		//cxENDLOCK
	}

	if (cw) delete cw;
	if (pCatalyst) delete pCatalyst;
}
