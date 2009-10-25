#ifndef __REPLACESTRINGPARSER_H__
#define __REPLACESTRINGPARSER_H__

#include <map>
#include <vector>
#include <wx/string.h>
#include "Interval.h"

class DocumentWrapper;
struct ReplaceStringParserState;

class ReplaceStringParser {
public:
	ReplaceStringParser(const DocumentWrapper& doc, const wxString& indent, 
		const wxString& replacetext, const std::map<unsigned int, interval>& captures, const std::vector<char>* source=NULL);

	~ReplaceStringParser();

	wxString Parse();

private:
	void DoParse(const wxChar* start, const wxChar* end);

	const DocumentWrapper& m_doc;
	const wxString& m_indent;
	ReplaceStringParserState* state;
};

#endif
