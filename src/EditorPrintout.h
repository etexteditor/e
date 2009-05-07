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

#ifndef __EDITORPRINTOUT_H__
#define __EDITORPRINTOUT_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/print.h>

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

// Pre-definitions
class IPrintableDocument;
class FixedLine;
class LineListWrap;
struct tmTheme;
class interval;

class EditorPrintout: public wxPrintout {
public:
	EditorPrintout(const IPrintableDocument& printDoc, const tmTheme& theme);
	~EditorPrintout();

	void OnPreparePrinting();
	
	bool HasPage(int pageNum) {return (int)m_pages.size() >= pageNum;};
	void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);
	bool OnPrintPage(int pageNum);

private:
	// Member variables
	const IPrintableDocument& m_printDoc;
	const tmTheme& m_theme;
	FixedLine* m_line;
	LineListWrap* m_lineList;

	vector<unsigned int> m_pages;
	unsigned int m_page_height;
	unsigned int m_gutter_width;

	// Dummy vars for line
	static const vector<interval> s_sel;
	static const interval s_hlBracket;
	static const unsigned int s_lastpos;
	static const bool s_isShadow;
};

#endif //__EDITORPRINTOUT_H__
