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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/print.h>

#include <vector>

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

	std::vector<unsigned int> m_pages;
	unsigned int m_page_height;
	unsigned int m_gutter_width;
};

#endif //__EDITORPRINTOUT_H__
