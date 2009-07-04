#ifndef __EASYPLISTWRITER_H__
#define __EASYPLISTWRITER_H__

/* Easy Plist Writer

This class provides a wrapper around TinyXML for generating non-bundle-item PLists.
It is used to store .eprj (E Project) files, and was extracted from code in ProjectPane.

*/

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/arrstr.h>
#endif

class TiXmlElement;
class TiXmlDocument;

class EasyPlistWriter
{
public:
	EasyPlistWriter(void);
	~EasyPlistWriter(void);

	void AddString(TiXmlElement* parent, const char* key, const char* value);
	void AddList(TiXmlElement* parent, const char* key, const wxArrayString& list);
	TiXmlElement* AddDict(TiXmlElement* parent, const char* key);

	bool Save(const wxString& filepath) const;

private:
	void AddKey(TiXmlElement* parent, const char* key);

	TiXmlDocument* m_doc;
	TiXmlElement* m_rootDict;
};

#endif // __EASYPLISTWRITER_H__
