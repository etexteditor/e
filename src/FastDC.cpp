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

#include "FastDC.h"

void FastDC::SetFontStyles(int fontStyles, std::map<int,wxFont>& fontMap) {
	const int fontStyle = (fontStyles & wxFONTFLAG_ITALIC) ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL;
	const int fontWeight = (fontStyles & wxFONTFLAG_BOLD) ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL;
	const bool fontUnderline = (fontStyles & wxFONTFLAG_UNDERLINED) ? true : false;

	// Check if we need to change font style
	// (changing the font can be expensive, so we want to avoid it if possible)
	const bool styleChanged = (m_font.GetStyle() != fontStyle);
	const bool widthChanged = (m_font.GetWeight() != fontWeight);
	const bool underlineChanged = (m_font.GetUnderlined() != fontUnderline);
	if (!styleChanged && !widthChanged && !underlineChanged) return;

	// Check if we have font in cache
	std::map<int,wxFont>::const_iterator p = fontMap.find(fontStyles);
	if (p == fontMap.end()) {
		m_font.SetStyle(fontStyle);
		m_font.SetWeight(fontWeight);
		m_font.SetUnderlined(fontUnderline);

		fontMap[fontStyles] = m_font;
	}
	else m_font = p->second;

	/*m_font.SetStyle(fontStyle);
	m_font.SetWeight(fontWeight);
	m_font.SetUnderlined(fontUnderline);*/

#ifdef __WXMSW__
	HGDIOBJ hfont = ::SelectObject(GetHdc(), GetHfontOf(m_font));
    if ( hfont == HGDI_ERROR )
    {
        wxLogLastError(_T("SelectObject(font)"));
    }
    else // selected ok
    {
        if ( !m_oldFont )
            m_oldFont = (WXHPEN)hfont;
    }
#else
    SetFont(m_font);
#endif
}
