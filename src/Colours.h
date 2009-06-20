#ifndef __COLOURS_H__
#define __COLOURS_H__

#include <wx/string.h>

class wxColour;
bool ParseColourAlpha(const char* text, wxColour& colour, unsigned int& alpha);
wxString WriteColourAlpha(const wxColour& colour, const unsigned int alpha);
#endif
