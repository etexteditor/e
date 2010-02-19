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

#ifndef FIXEDLINE_H
#define FIXEDLINE_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "Catalyst.h"
#include "StyleRun.h"
#include "WrapMode.h"

class FastDC;
struct tmTheme;
class Styler;
class DocumentWrapper;
class BracketHighlight;

struct full_pos {
	int pos;
	int caret_xpos;
	int caret_ypos;
	bool xy_outbound;
	bool on_newline;
};

class FixedLine {
public:
	FixedLine(wxDC& dc, const DocumentWrapper& dw, const vector<interval>& sel, const BracketHighlight& brackets, const unsigned int& lastpos, const bool& isShadow, const tmTheme& theme);
	void Init();

	bool IsValid() const {return width != 0;};
	void Invalidate() {width = 0; textstart = 0; textend = 0;};

	void SetPrintMode();
	void FlushCache(unsigned int pos=0);
	unsigned int SetLine(unsigned int startpos, unsigned int endpos, bool cache=true);
	int SetWidth(int newwidth);

	void DrawLine(int xoffset, int yoffset, const wxRect& rect, bool isFolded);
	void ResetStyle() {m_sr.ResetStyle();};

#ifdef __WXDEBUG__
	void ApplyStyle(unsigned int id) {m_sr.ApplyStyle(id);};
	FastDC& GetDC() {return dc;};
#endif

	void UpdateFont();
	void SetTabWidth(unsigned int width);
	void SetWordWrap(cxWrapMode wrapMode) {m_wrapMode = wrapMode; FlushCache();};
	void ShowIndentGuides(bool showIndent) {m_showIndent = showIndent;};

	unsigned int GetLineWidth() const;
	unsigned int GetDisplayWidth() const {return width;};
	unsigned int GetUnwrappedWidth() const {wxASSERT(m_wrapMode == cxWRAP_NONE); return m_lineWidth;};

	int GetHeight() const;
	int GetCharHeight() const {return charheight;};
	int GetCharWidth() const {return charwidth;}; // width of a whitespace char 
	wxPoint GetCaretPos(unsigned int pos, bool tryfront=false) const;
	wxRect GetFoldIndicatorRect() const;
	full_pos ClickOnLine(int xpos, int ypos) const;

	// Functions to get approximated stats without parsing line
	unsigned int GetQuickLineWidth(unsigned int startpos, unsigned int endpos);
	unsigned int GetQuickLineHeight(unsigned int startpos, unsigned int endpos);

	bool IsTabPos(unsigned int pos) const;
	bool IsOverFoldIndicator(const wxPoint& point) const;

	void AddStyler(Styler &styler);
	void StylersClear();
	void StylersInvalidate();
	void StylersInsert(unsigned int pos, unsigned int len);
	void StylersDelete(unsigned int start, unsigned int end);
	void StylersApplyDiff(vector<cxChange>& changes);

private:
	unsigned int DrawText(int xoffset, int x, int y, unsigned int start, unsigned int end);
	void BreakLine();
	int GetTabPoint(int xpos) const;

	// Member variables
	FastDC& dc;
	const DocumentWrapper& m_doc;
	unsigned int textstart;
	unsigned int textend;
	unsigned int m_lineLen;
	unsigned int m_lineBufferLen;
	int width;
	int old_width;
	unsigned int m_tabChars;
	const unsigned int& lastpos;
	const bool& m_isSelShadow;
	const BracketHighlight& m_brackets;
	const tmTheme& m_theme;
	StyleRun m_sr;
	unsigned int m_docLen;
	wxCharBuffer m_lineBuffer;
	unsigned int m_lineWidth; // full width when no word-wrap
	wxString m_textBuf;
	vector<unsigned int> m_extents;
	wxArrayInt m_extsBuf;
	int height;
	int charwidth;
	int m_nlwidth;
	int charheight;
	int tabwidth;
	const vector<interval>& selections;
	cxWrapMode m_wrapMode;
	bool m_showIndent;
	unsigned int m_indentWidth;
	wxBitmap bmFold;
	wxBitmap bmNewline;
	wxBitmap bmSpace;
	wxBitmap bmTab;

	// Above: set in constructors' intializer list
	// ----
	// Below: not set in initializer list

	vector<unsigned int> breakpoints;
	vector<Styler*> m_stylers;
	bool m_isFontFixedWidth;

	unsigned int m_foldWidth;
	unsigned int m_foldHeight;

	static wxString s_text;

#ifdef __WXDEBUG__
	bool m_inSetLine;
#endif
};

#endif // FIXEDLINE_H
