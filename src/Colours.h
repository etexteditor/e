#ifndef __COLOURS_H__
#define __COLOURS_H__

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

class wxColour;
bool ParseColourAlpha(const char* text, wxColour& colour, unsigned int& alpha);
std::vector<char> WriteColourAlpha(const wxColour& colour, const unsigned int alpha);
#endif
