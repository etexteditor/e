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

#include "styler_variablehl.h"
#include "styler_searchhl.h"

#include "StyleRun.h"
#include "Lines.h"
#include "Document.h"
#include "FindFlags.h"
#include "EditorCtrl.h"

Styler_VariableHL::Styler_VariableHL(const DocumentWrapper& rev, const Lines& lines, const vector<interval>& ranges, const tmTheme& theme, eSettings& settings, EditorCtrl& editorCtrl):
Styler_SearchHL(rev, lines, ranges, theme), m_settings(settings),
  m_selectionHighlightColor(m_theme.selectionColor),
  m_searchHighlightColor(m_theme.searchHighlightColor) ,
  m_cursorPosition(0), m_editorCtrl(editorCtrl)
{
	Clear(); // Make sure all variables are empty
}

void Styler_VariableHL::Clear() {
	Styler_SearchHL::Clear();

	//override default search options
	m_options = FIND_MATCHCASE;
}

void Styler_VariableHL::SetCurrentWord() {
	const wxString text = m_editorCtrl.GetCurrentWord();
	if (text != m_text || text.Len() == 0) {
		Clear();
		m_text = text;
	}

	m_cursorPosition = m_editorCtrl.GetPos();
}

bool Styler_VariableHL::IsCurrentWord(unsigned int start, unsigned int end) {
	return start <= m_cursorPosition && end >= m_cursorPosition;
}

bool Styler_VariableHL::ShouldStyle() {
	bool shouldStyle = false;
	m_settings.GetSettingBool(wxT("highlightVariables"), shouldStyle);
	return shouldStyle;
}

void Styler_VariableHL::ApplyStyle(StyleRun& sr, unsigned int start, unsigned int end) {
	//It's quite annoying if the styler highlights the current word in the document as you are typing.
	if(!IsCurrentWord(start, end)) {
		sr.SetBackgroundColor(start, end, m_searchHighlightColor);
		sr.SetShowHidden(start, end, true);
	}
}

void Styler_VariableHL::Style(StyleRun& sr) {
	if(!ShouldStyle()) {
		//If the user has the option turned off, then do no processing at all.
		//If they later turn it on, we could have junk, so make sure we do a new search at that point.
		Invalidate();
		return;
	}
	
	SetCurrentWord();
	Styler_SearchHL::Style(sr);
}

inline bool isAlphaNumeric(wxChar c) {
#ifdef __WXMSW__
	return ::IsCharAlphaNumeric(c) != 0;
#else
	return wxIsalnum(c);
#endif
}

/**
 * say we click on the variable var..
 * We want to filter out matches like these:
     variable
	 avar
	 a_var
 * But allow matches like these:
   var.method();
   function(var);
   var+2;
 */
bool Styler_VariableHL::FilterMatch(search_result& result, const Document& doc) {
	wxChar c;
	if(result.start > 0) {
		c = doc.GetChar(result.start-1);
		if(isAlphaNumeric(c) || c == '_') return false;
	}
	if(result.end < doc.GetLength()) {
		c = doc.GetChar(result.end);
		if(isAlphaNumeric(c) || c == '_') return false;
	}
	
	return true;
}

void Styler_VariableHL::Insert(unsigned int pos, unsigned int length) {
	if(!ShouldStyle()) {
		//If the user has the option turned off, then do no processing at all.
		//If they later turn it on, we could have junk, so make sure we do a new search at that point.
		Invalidate();
		return;
	}
	
	SetCurrentWord();
	Styler_SearchHL::Insert(pos, length);
}

void Styler_VariableHL::Delete(unsigned int start, unsigned int end) {
	if(!ShouldStyle()) {
		//If the user has the option turned off, then do no processing at all.
		//If they later turn it on, we could have junk, so make sure we do a new search at that point.
		Invalidate();
		return;
	}

	SetCurrentWord();
	Styler_SearchHL::Delete(start, end);
}