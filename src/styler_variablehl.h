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
#include "styler_searchhl.h"
#include "eSettings.h"
#include "EditorChangeState.h"

#include <vector>
#include "time.h"


class EditorCtrl;
class DocumentWrapper;
class StyleRun;
struct tmTheme;
class Lines;

class Styler_VariableHL : public Styler_SearchHL {
public:
	Styler_VariableHL(const DocumentWrapper& rev, const Lines& lines, const std::vector<interval>& ranges, const std::vector<unsigned int>& cursors, const tmTheme& theme, eSettings& settings, EditorCtrl& editorCtrl);
	virtual ~Styler_VariableHL() {};

	void Clear();

	bool OnIdle();
	void Style(StyleRun& sr);

	bool ShouldStyle();
	bool IsCurrentWord(unsigned int start, unsigned int end);

	// Handle document changes
	void Insert(unsigned int pos, unsigned int length);
	void Delete(unsigned int start_pos, unsigned int end_pos);
	
	void ApplyStyle(StyleRun& sr, unsigned int start, unsigned int pos);
	bool FilterMatch(search_result& result, const Document& doc);

private:
	eSettings& m_settings;
	EditorCtrl& m_editorCtrl;

	unsigned int m_cursorPosition;
	wxLongLong m_lastUpdateTime;
	EditorChangeState m_lastEditorState;

	const wxColour& m_searchHighlightColor;
	const wxColour& m_selectionHighlightColor;
};

#endif // __STYLER_VARIABLEHL_H__
