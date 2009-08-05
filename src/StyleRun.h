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

#ifndef __STYLERUN_H__
#define __STYLERUN_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "tmTheme.h"
#include "FastDC.h"

#include <vector>
#include <map>

class StyleRun {
public:
	StyleRun(const tmTheme& theme, FastDC& dc);

	void SetPrintMode();
	void Init(unsigned int start, unsigned int end);
	void ClearCache() {s_fontCache.clear();};

	unsigned int GetRunStart() const {return m_styles[0].start;};
	unsigned int GetRunEnd() const {return m_styles.back().end;};
	size_t GetStyleCount() const {return m_styles.size();};
	unsigned int GetStyleAtPos(unsigned int pos) const;

	void EnableBold(bool enable) {m_enableBold = enable;};
	void EnableItalic(bool enable) {m_enableItalic = enable;};

	void SetForegroundColor(unsigned int start, unsigned int end, const wxColour& color);
	void SetBackgroundColor(unsigned int start, unsigned int end, const wxColour& color);
	void SetFontStyle(unsigned int start, unsigned int end, int fontStyle);
	void SetShowHidden(unsigned int start, unsigned int end, bool do_show);

	bool DoExtendBgAtPos(unsigned int pos) const;

	bool DoExtendBg() const {return m_extendBgColor.Ok() && m_extendBgColor != m_theme.backgroundColor;};
	void SetExtendBgColor(const wxColour& color) {m_extendBgColor = color;};
	const wxColour& GetExtendBgColor() const {return m_extendBgColor;};

	void ApplyStyle(unsigned int style_id) const;
	void ApplyFontStyle(int fontStyle) const {
		if (!m_enableBold) fontStyle &= ~wxFONTFLAG_BOLD;
		if (!m_enableItalic) fontStyle &= ~wxFONTFLAG_ITALIC;
		m_dc.SetFontStyles(fontStyle, s_fontCache);
	}
	void ResetStyle() const;
	bool ShowHidden(unsigned int style_id) const {
		wxASSERT(style_id >= 0 && style_id < m_styles.size());
		return m_styles[style_id].show_hidden;
	};

	unsigned int GetStyleEnd(unsigned int style_id) const {
		wxASSERT(style_id >= 0 && style_id < m_styles.size());
		return m_styles[style_id].end;
	};
	int GetFontStyle(unsigned int style_id) const {
		wxASSERT(style_id >= 0 && style_id < m_styles.size());
		return m_styles[style_id].fontStyle;
	}

	void Print() const; // DEBUG

private:
	// type definition
	struct StyleSR {
		unsigned int start;
		unsigned int end;
		const wxColour* foregroundcolor;
		const wxColour* backgroundcolor;
		int fontStyle;
		bool show_hidden;
	};

	// Member variables
	const tmTheme& m_theme;
	FastDC& m_dc;
	StyleSR m_default_style;
	std::vector<StyleSR> m_styles;
	wxColour m_extendBgColor;
	bool m_enableBold;
	bool m_enableItalic;
	bool m_printMode;

	// Font cache
	static std::map<int,wxFont> s_fontCache;
};


#endif // __STYLERUN_H__
