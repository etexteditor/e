#include "Colours.h"
#include <wx/colour.h>

bool ParseColourAlpha(const char* text, wxColour& colour, unsigned int& alpha) {
	if (!text) return false;

	size_t digits = strlen(text);
	if (digits != 7 && digits != 9) return false;

	if (text[0] != '#') return false;

	int red, green, blue;
	alpha = 0;
	sscanf(text, "#%2x%2x%2x%2x", &red, &green, &blue, &alpha);

	colour.Set(red, green, blue);
	return true;
}
