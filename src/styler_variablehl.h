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

#ifndef __STYLER_VARIABLEHL_H__
#define __STYLER_VARIABLEHL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
	#include <wx/colour.h>
#endif

#include "Catalyst.h"
#include "styler.h"

#include <vector>


class DocumentWrapper;
class StyleRun;
struct tmTheme;
class Lines;

class Styler_VariableHL : public Styler {
public:
	Styler_VariableHL(const DocumentWrapper& rev, const Lines& lines, const std::vector<interval>& ranges, const tmTheme& theme);
	virtual ~Styler_VariableHL() {};

	void Clear();
	void Invalidate();
	void SetCurrentWord(const wxString& text);
	void Style(StyleRun& sr);

	// Handle document changes
	void ApplyDiff(const std::vector<cxChange>& changes);

private:
	void DoSearch(unsigned int start, unsigned int end, bool from_last=false);

	// Member variables
	const DocumentWrapper& m_doc;
	const Lines& m_lines;
	wxString m_text;
	std::vector<interval> m_matches;

	// Theme variables
	const tmTheme& m_theme;
	const wxColour& m_hlcolor;
	const wxColour& m_rangeColor;

	unsigned int m_search_start;
	unsigned int m_search_end;
	static const unsigned int EXTSIZE;
};

#endif // __STYLER_VARIABLEHL_H__
