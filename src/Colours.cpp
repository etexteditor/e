#include "Colours.h"
#include <wx/colour.h>

bool ParseColourAlpha(const char* text, wxColour& colour, unsigned int& alpha) {
	if (!text) return false;
	if (strlen(text) < 7) return false;
	if (text[0] != '#') return false;

	int red;
	int green;
	int blue;
	alpha = 0;
	sscanf(text, "#%2x%2x%2x%2x", &red, &green, &blue, &alpha);

	colour.Set(red, green, blue);
	return true;
}
