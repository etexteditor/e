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

#ifndef __STYLER_HTMLHL_H__
#define __STYLER_HTMLHL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
	#include <wx/colour.h>
#endif

#include "Catalyst.h"
#include "styler.h"
#include "eSettings.h"
class EditorCtrl;

#include <vector>


class DocumentWrapper;
class StyleRun;
struct tmTheme;
class Lines;

class Styler_HtmlHL : public Styler {
public:
	class TagInterval {
	public:
		unsigned int start, end, tagNameEnd;
		bool isClosingTag, isSelfClosingTag;
		
		TagInterval(unsigned int start, unsigned int end, const wxChar* data);
	};

	Styler_HtmlHL(const DocumentWrapper& rev, const Lines& lines, const tmTheme& theme, eSettings& settings, EditorCtrl& editorCtrl);
	virtual ~Styler_HtmlHL() {};

	void Clear();
	void Invalidate();
	void UpdateCursorPosition(unsigned int pos);
	void SelectParentTag();
	void Style(StyleRun& sr);
	
	bool ShouldStyle();
	void Reparse();
	bool IsValidTag(unsigned int start, unsigned int end, const wxChar* data);
	bool SameTag(TagInterval& openTag, TagInterval& closeTag, const wxChar* data);
	void FindBrackets(unsigned int start, unsigned int end, const wxChar* data);
	void FindAllBrackets(const wxChar* data);
	void FindTags(const wxChar* data);
	int FindMatchingTag(const wxChar* data, int tag);
	int FindCurrentTag();
	int FindParentClosingTag(unsigned int searchPosition);

	// Handle document changes
	void Insert(unsigned int pos, unsigned int length);
	void Delete(unsigned int start_pos, unsigned int end_pos);
	void ApplyDiff(const std::vector<cxChange>& changes);

private:
	// Member variables
	const DocumentWrapper& m_doc;
	const Lines& m_lines;
	eSettings& m_settings;
	EditorCtrl& m_editorCtrl;

	unsigned int m_cursorPosition;
	bool initialParse;
	int m_currentTag, m_matchingTag;
	std::vector<unsigned int> m_brackets;
	std::vector<TagInterval> m_tags;

	// Theme variables
	const tmTheme& m_theme;
	const wxColour& m_searchHighlightColor;
	const wxColour& m_selectionHighlightColor;
};

#endif // __STYLER_HTMLHL_H__
