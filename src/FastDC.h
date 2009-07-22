/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __FASTDC_H__
#define __FASTDC_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <map>

class FastDC : public wxDC {
public:
	void FastDrawText(const wxString& text, wxCoord x, wxCoord y, wxCoord textwidth, wxCoord charheight) {
		// fontheight includes room for underline, so if the background
		// is solid, we have to first draw it manually to avoid missing bottom
		if (GetBackgroundMode() != wxTRANSPARENT) {
			SetPen(GetTextBackground());
			SetBrush(wxBrush(GetTextBackground(), wxSOLID));
			DrawRectangle(x, y, textwidth, charheight);
		}

#ifdef __WXMSW__
		// Windows only optimization, avoids GetTextExtend()
		DrawAnyText(text, x, y);
#else
        DrawText(text, x, y);
#endif
	};

	void SetFontStyles(int fontStyles, std::map<int,wxFont>& fontMap);

	// Cannot have any member variables, as it is cast from wxDC
};

#endif // Include guard
