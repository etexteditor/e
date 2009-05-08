#include "EasyPlistWriter.h"

#include "wx/platform.h"
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include "tinyxml.h" // tinyxml includes unused vars so it can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

#include <wx/filename.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/dir.h>

EasyPlistWriter::EasyPlistWriter(void){}
EasyPlistWriter::~EasyPlistWriter(void){}

static void plist_add_key(TiXmlElement* parent, const char* key) {
	TiXmlElement* keytag = new TiXmlElement("key");
	parent->LinkEndChild(keytag);
	TiXmlText* text = new TiXmlText(key);
	keytag->LinkEndChild(text);
}

static void plist_add_string(TiXmlElement* parent, const char* key, const char* value){
	plist_add_key(parent, key);

	TiXmlElement* strtag = new TiXmlElement("string");
	parent->LinkEndChild(strtag);
	TiXmlText* text = new TiXmlText(value);
	strtag->LinkEndChild(text);
}

static void plist_add_list(TiXmlElement* parent, const char* key, const wxArrayString& list) {
	plist_add_key(parent, key);

	TiXmlElement* array = new TiXmlElement("array");
	parent->LinkEndChild(array);

	for (unsigned int i = 0; i < list.GetCount(); ++i) {
		TiXmlElement* strtag = new TiXmlElement("string");
		array->LinkEndChild(strtag);
		TiXmlText* text = new TiXmlText(list[i].mb_str(wxConvUTF8));
		strtag->LinkEndChild(text);
	}
}

static TiXmlElement* plist_add_dict(TiXmlElement* parent, const char* key) {
	plist_add_key(parent, key);

	TiXmlElement* dict = new TiXmlElement("dict");
	parent->LinkEndChild(dict);
	return dict;
}
