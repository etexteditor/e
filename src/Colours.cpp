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

std::vector<char> WriteColourAlpha(const wxColour& colour, const unsigned int alpha) {
	wxASSERT(colour.Ok());
	wxASSERT(alpha <= 256);

	std::vector<char> str;
	if (alpha) {
		str.resize(10); // "#xxxxxxxx\0"
		sprintf(&*str.begin(), "#%02X%02X%02X%02X", colour.Red(), colour.Green(), colour.Blue(), alpha);
	}
	else {
		str.resize(8); // "#xxxxxx\0"
		sprintf(&*str.begin(), "#%02X%02X%02X", colour.Red(), colour.Green(), colour.Blue());
	}

	return str;
}
