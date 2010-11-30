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

#ifndef __DIFFBAR_H__
#define __DIFFBAR_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Catalyst.h"
#include "styler.h"

#include <vector>

// pre-definitions
class EditorFrame;
class EditorCtrl;
class TmSyntaxHandler;

class DiffBar : public wxControl {
public:
	DiffBar(wxWindow* parent, CatalystWrapper& cw, EditorCtrl* leftEditor, EditorCtrl* rightEditor, TmSyntaxHandler& syntax_handler);
	void Init();
	void SetCallbacks();

	void SetDiff();
	void Swap();

	class LineMatch {
	public:
		unsigned int left_start;
		unsigned int left_end;
		unsigned int right_start;
		unsigned int right_end;
		cxChangeType left_type;
		cxChangeType right_type;
	};

	const std::vector<LineMatch>& GetLineMatches() const {return m_lineMatches;};

private:
	class Change {
	public:
		Change(cxChangeType type, unsigned int start, unsigned int end, unsigned int pos)
			: type(type), start_pos(start), end_pos(end), rev_pos(pos) {};
		cxChangeType type;
		unsigned int start_pos;
		unsigned int end_pos;
		unsigned int rev_pos;
	};

	class DiffStyler : public Styler {
	public:
		DiffStyler(const vector<Change>& diffs, bool isLeft, TmSyntaxHandler& syntax_handler);
		void SwapSide() {m_isLeft = !m_isLeft;};
		void Style(StyleRun& sr);
	private:
		bool m_isLeft;
		const std::vector<Change>& m_diffs;
		wxColor m_delColor;
		wxColor m_insColor;
	};

	void TransformMatchlist();
	void DrawLayout(wxDC& dc);

	std::vector<LineMatch>::const_iterator OnLeftBracket(int y);
	std::vector<LineMatch>::const_iterator OnRightBracket(int y);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	DECLARE_EVENT_TABLE();

	// Callback handlers
	static void OnBeforeEditorRedraw(void* data);
	static void OnLeftEditorRedrawn(void* data);
	static void OnRightEditorRedrawn(void* data);
	static void OnLeftEditorScroll(void* data);
	static void OnRightEditorScroll(void* data);
	static void OnLeftDocumentChanged(cxChangeType type, unsigned int pos, unsigned int len, void* data);
	static void OnRightDocumentChanged(cxChangeType type, unsigned int pos, unsigned int len, void* data);

	// Member variables
	CatalystWrapper m_catalyst;
	EditorCtrl* m_leftEditor;
	EditorCtrl* m_rightEditor;
	list<cxMatch> m_matchlist;
	std::vector<Change> m_diffs;
	std::vector<LineMatch> m_lineMatches;
	DiffStyler m_leftStyler;
	DiffStyler m_rightStyler;
	bool m_needRedraw;
	bool m_needTransform;
	int m_highlight;

	static const unsigned int s_bracketWidth;
};

#endif // __DIFFBAR_H__
