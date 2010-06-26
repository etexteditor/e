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

#ifndef __STYLER_SELECTIONS_H__
#define __STYLER_SELECTIONS_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
	#include <wx/colour.h>
#endif

#include "Catalyst.h"
#include "styler.h"
#include <vector>


class EditorCtrl;
class DocumentWrapper;
class Document;
class StyleRun;
struct tmTheme;
class Lines;

class Styler_Selections : public Styler {
public:

	Styler_Selections(const DocumentWrapper& rev, const Lines& lines, const tmTheme& theme, EditorCtrl& editorCtrl);
	virtual ~Styler_Selections() {};

	void Invalidate();

	void Style(StyleRun& sr);
	void ApplyStyle(StyleRun& sr, unsigned int start, unsigned int end);

	// Handle document changes
	void Insert(unsigned int pos, unsigned int length);
	void Delete(unsigned int start_pos, unsigned int end_pos);
	void ApplyDiff(std::vector<cxChange>& changes);

	void EnableNavigation();
	void NextSelection();
	void PreviousSelection();
private:
	// Member variables
	const DocumentWrapper& m_doc;
	const Lines& m_lines;
	EditorCtrl& m_editorCtrl;

	bool m_enabled;
	int m_nextSelection;
	std::vector<interval> m_selections;

	// Theme variables
	const tmTheme& m_theme;
	const wxColour& m_searchHighlightColor;
	const wxColour& m_selectionHighlightColor;
};

#endif // __STYLER_SELECTIONS_H__
