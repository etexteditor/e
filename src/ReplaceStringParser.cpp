#include "ReplaceStringParser.h"
#include "Document.h"

struct ReplaceStringParserState {
	ReplaceStringParserState(const wxString& reptext, const std::map<unsigned int,interval>& caps, const std::vector<char>* source):
		replacetext(reptext), captures(caps), source(source), upcase(false), lowcase(false), caseChar(false), caseText(false)
	{
		newtext.reserve(replacetext.size());
	}

	const wxString& replacetext;
	const map<unsigned int,interval>& captures;
	const vector<char>* source;
	wxString newtext;
	bool upcase;
	bool lowcase;
	bool caseChar;
	bool caseText;
};


ReplaceStringParser::ReplaceStringParser(const DocumentWrapper& doc, const wxString& indent, 
	const wxString& replacetext, const std::map<unsigned int,interval>& captures, const std::vector<char>* source):
		m_doc(doc), m_indent(indent)
{
	state = new ReplaceStringParserState(replacetext, captures, source);
}

ReplaceStringParser::~ReplaceStringParser() { delete state; }

static const size_t NO_INDEX = (size_t)-1;

void ReplaceStringParser::DoParse(const wxChar* start, const wxChar* end) {
	wxASSERT(start <= end);

	for ( const wxChar *p = start; p < end; ++p ) {
 		size_t index = NO_INDEX;

        if (*p == wxT('$')) {
            if (wxIsdigit(*++p)) {
                // back reference
                wxChar *end;
                index = (size_t)wxStrtoul(p, &end, 10);
                p = end - 1; // -1 to compensate for p++ in the loop
            }

			// do we have a back reference?
			if (index != NO_INDEX) {
				// yes, get its text
				map<unsigned int,interval>::const_iterator iv = state->captures.find(index);
				if ( iv != state->captures.end() ) {
					wxString reftext;
					if (state->source) {
						reftext = wxString(&*state->source->begin() + iv->second.start, wxConvUTF8, iv->second.end - iv->second.start);
					}
					else {
						cxLOCKDOC_READ(m_doc)
							reftext = doc.GetTextPart(iv->second.start, iv->second.end);
						cxENDLOCK
					}

					// Case foldings
					if (state->caseChar && !reftext.empty()) {
						if (state->lowcase) reftext[0] = wxTolower(reftext[0]);
						else if (state->upcase) reftext[0] = wxToupper(reftext[0]);
						else wxASSERT(false);

						state->caseChar = false;
						state->lowcase = false;
						state->upcase = false;
					}
					else if (state->caseText) {
						if (state->lowcase) reftext.MakeLower();
						else if (state->upcase) reftext.MakeUpper();
						else wxASSERT(false);
					}

					state->newtext += reftext;
				}
			}
        }
		else if (*p == wxT('\\')) {
			++p;
			if (p < end) {
				if (*p == wxT('n')) state->newtext += wxT('\n');
				else if (*p == wxT('t')) state->newtext += m_indent;
				else if (*p == wxT('l')) {
					state->caseChar = true;
					state->lowcase = true;
					state->upcase = false;
				}
				else if (*p == wxT('u')) {
					state->caseChar = true;
					state->lowcase = false;
					state->upcase = true;
				}
				else if (*p == wxT('L')) {
					state->caseText = true;
					state->lowcase = true;
					state->upcase = false;
				}
				else if (*p == wxT('U')) {
					state->caseText = true;
					state->lowcase = false;
					state->upcase = true;
				}
				else if (*p == wxT('E')) {
					state->caseText = false;
					state->caseChar = false;
					state->lowcase = false;
					state->upcase = false;
				}
				else state->newtext += *p;
			}
			else {
				state->newtext += '\\';
				break;
			}
		}
		else if (*p == wxT('(') && *(p+1) == wxT('?')) {
			// Parse condition
			const wxChar* p2 = p+2;
			const wxChar* inStart = NULL;
			const wxChar* inEnd = NULL;
			const wxChar* otStart = NULL;
			const wxChar* otEnd = NULL;
			bool success = false;

			if (wxIsdigit(*p2)) {
				// Get capture index
				wxChar *iend;
				unsigned int index = wxStrtoul(p2, &iend, 10);
				p2 = iend;

				if (*p2 != wxT(':')) goto error;
				++p2;

				inStart = p2;
				inEnd = p2;

				// find insertion end
				unsigned int pcount = 0;
				while(1) {
					if (!inEnd) goto error;
					else if (*inEnd == wxT('\\')) ++inEnd; // ignore escaped chars
					else if (*inEnd == wxT('(')) ++pcount; // count parens
					else if (*inEnd == wxT(')')) if (pcount) --pcount; else break;
					else if (*inEnd == wxT(':')) {
						// find 'otherwise' end
						otStart = otEnd = inEnd + 1;
						pcount = 0;
						while(1) {
							if (!otEnd) goto error;
							else if (*otEnd == wxT('\\')) ++otEnd; // ignore escaped chars
							else if (*otEnd == wxT('(')) ++pcount; // count parens
							else if (*otEnd == wxT(')')) {if (pcount) --pcount; else break;}
							++otEnd;
						}
						break;
					}
					++inEnd;
				}

				// parse the part that meets the condition
				map<unsigned int,interval>::const_iterator iv = state->captures.find(index);
				if (iv != state->captures.end())
					DoParse(inStart, inEnd);
				else if (otStart)
					DoParse(otStart, otEnd);

				success = true;
			}

error:
			if (success) p = otEnd ? otEnd : inEnd;
			else state->newtext += *p; // ordinary character

		}
        else {
			if (state->caseChar || state->caseText) {
				if (state->lowcase) state->newtext += wxTolower(*p);
				else if (state->upcase) state->newtext += wxToupper(*p);
				else wxASSERT(false);

				state->caseChar = false;
			}
			else state->newtext += *p; // ordinary character
		}
	}
}

wxString ReplaceStringParser::Parse() {
	const wxChar *start = state->replacetext.c_str();
	DoParse(start, start + state->replacetext.size());
	return state->newtext;
}
