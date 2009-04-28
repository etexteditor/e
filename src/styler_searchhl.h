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

#ifndef __STYLER_SEARCHHL_H__
#define __STYLER_SEARCHHL_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include "StyleRun.h"
#include "styler.h"

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
class EditorCtrl;

class Styler_SearchHL : public Styler {
public:
	Styler_SearchHL(const DocumentWrapper& rev, const Lines& lines, const vector<interval>& ranges);
	virtual ~Styler_SearchHL() {};

	void Init(const EditorCtrl& editor);

	void Clear();
	void Invalidate();
	void SetSearch(const wxString& text, int options);
	void Style(StyleRun& sr);

	// Handle document changes
	void Insert(unsigned int pos, unsigned int length);
	void Delete(unsigned int start_pos, unsigned int end_pos);
	void ApplyDiff(const vector<cxChange>& changes);

private:
	void DoSearch(unsigned int start, unsigned int end, bool from_last=false);

	// Member variables
	const EditorCtrl* m_editor;
	const DocumentWrapper& m_doc;
	const Lines& m_lines;
	wxString m_text;
	int m_options;
	vector<interval> m_matches;
	const vector<interval>& m_searchRanges;

	// Theme variables
	const tmTheme& m_theme;
	const wxColour& m_hlcolor;
	const wxColour& m_rangeColor;

	unsigned int m_search_start;
	unsigned int m_search_end;
	static const unsigned int EXTSIZE;
};

#endif // __STYLER_SEARCHHL_H__
