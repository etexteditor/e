#ifndef __TMTHEME_H__
#define __TMTHEME_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
#include "wx/wx.h"
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