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

EasyPlistWriter::EasyPlistWriter(void): 
	m_doc(new TiXmlDocument()) 
{
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	m_doc->LinkEndChild(decl);

	TiXmlUnknown* dt = new TiXmlUnknown();
	dt->SetValue("!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
	m_doc->LinkEndChild(dt);

	TiXmlElement* plist = new TiXmlElement( "plist" );
	plist->SetAttribute("version", "1.0");
	m_doc->LinkEndChild(plist);

	m_rootDict = new TiXmlElement("dict");
	plist->LinkEndChild(m_rootDict);
}

EasyPlistWriter::~EasyPlistWriter(void){
	if (m_doc) delete m_doc;
}

void EasyPlistWriter::AddKey(TiXmlElement* parent, const char* key) {
	TiXmlElement* keytag = new TiXmlElement("key");
	parent->LinkEndChild(keytag);
	TiXmlText* text = new TiXmlText(key);
	keytag->LinkEndChild(text);
}

void EasyPlistWriter::AddString(TiXmlElement* parent, const char* key, const char* value){
	AddKey(parent, key);

	TiXmlElement* strtag = new TiXmlElement("string");
	parent->LinkEndChild(strtag);
	TiXmlText* text = new TiXmlText(value);
	strtag->LinkEndChild(text);
}

void EasyPlistWriter::AddList(TiXmlElement* parent, const char* key, const wxArrayString& list) {
	AddKey(parent, key);

	TiXmlElement* array = new TiXmlElement("array");
	parent->LinkEndChild(array);

	for (unsigned int i = 0; i < list.GetCount(); ++i) {
		TiXmlElement* strtag = new TiXmlElement("string");
		array->LinkEndChild(strtag);
		TiXmlText* text = new TiXmlText(list[i].mb_str(wxConvUTF8));
		strtag->LinkEndChild(text);
	}
}

TiXmlElement* EasyPlistWriter::AddDict(TiXmlElement* parent, const char* key) {
	AddKey(parent, key);

	TiXmlElement* dict = new TiXmlElement("dict");
	parent->LinkEndChild(dict);
	return dict;
}

bool EasyPlistWriter::Save(const wxString& filepath) const {
	wxLogDebug(wxT("Saving projectInfo %s"), filepath.c_str());
	wxFFile docffile(filepath, _T("wb"));
	bool result = docffile.IsOpened() && m_doc->SaveFile(docffile.fp());
	wxCHECK2_MSG(result, return result, wxT("  save failed"));

	return result;
}
