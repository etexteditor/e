#ifndef __TMTHEME_H__
#define __TMTHEME_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
	#include <wx/colour.h>
	#include <wx/font.h>
#endif

struct tmTheme {
	wxString name;
	wxString uuid;
	wxColour backgroundColor;
	wxColour foregroundColor;
	wxColour selectionColor;
	wxColour shadowColor;
	wxColour invisiblesColor;
	wxColour lineHighlightColor;
	wxColour searchHighlightColor;
	wxColour bracketHighlightColor;
	wxColour gutterColor;
	wxFont font;
};

#endif
