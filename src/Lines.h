#ifndef LINES_H
#define LINES_H

#include "wx/wxprec.h"
#include "Catalyst.h"
#include "FixedLine.h"
#include "LineListWrap.h"
#include "LineListNoWrap.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class wxFileName;
class IFoldingEditor;
struct tmTheme;
class Styler;

class Lines {
public:
	Lines(wxDC& dc, DocumentWrapper& dw, IFoldingEditor& editorCtrl, const tmTheme& theme);
	void Init();

	int GetHeight() const;
	int GetLineHeight() const;
	bool IsEmpty() const {return GetLength() == 0;};
	unsigned int GetLength() const;
	unsigned int GetLineCount(bool includeVirtual=true) const;
	unsigned int GetLastLine() const;
	void SetWidth(unsigned int newwidth, unsigned int index=0);
	int GetWidth();
	unsigned int GetDisplayWidth() const {return line.GetDisplayWidth();};
	unsigned int GetPos() const;
	unsigned int GetLastpos();
	void SetPos(unsigned int newpos, bool update_lastpos = true);
	void SetLastpos(unsigned int newpos);

	void UpdateParsedLine(unsigned int line_id);

	const vector<unsigned int>& GetOffsets() const {return ll->GetOffsets();};

	cxWrapMode GetWrapMode() const {return m_wrapMode;};
	void SetWordWrap(cxWrapMode wrapMode);
	void ShowIndent(bool showIndent);
	bool ShowMargin(unsigned int marginChars);
	unsigned int GetMarginPos() const {return m_marginPos;};

	void Invalidate() {ll->invalidate();};
	void UpdateFont();
	void SetTabWidth(unsigned int width);

	bool IsCaretInPreparedPos();
	wxPoint GetCaretPos() const;
	void UpdateCaretPos();
	wxPoint GetCharPos(unsigned int char_pos);

	int GetCurrentLine();
	int GetLineFromCharPos(unsigned int char_pos);
	unsigned int GetLineFromStartPos(unsigned int char_pos);
	unsigned int GetLineEndFromPos(unsigned int pos);
	unsigned int GetLineStartFromPos(unsigned int pos);
	unsigned int GetLineEndpos(unsigned int lineid, bool stripnewline = true);
	unsigned int GetLineStartpos(unsigned int lineid);
	bool isLineVirtual(unsigned int lineid);
	bool IsLineEnd(unsigned int pos);
	bool IsLineEnd(unsigned int line_id, unsigned int pos);
	bool IsLineStart(unsigned int line_id, unsigned int pos);
	bool IsBeforeNewline(unsigned int pos);
	bool IsLineEmpty(unsigned int lineid);

	int GetLineFromYPos(int ypos) const;
	int GetYPosFromLine(unsigned int lineid) const;
	int GetBottomYPosFromLine(unsigned int lineid) const;
	int PrepareYPos(int ypos);
	int PrepareAll() const {return ll->prepare_all();};
	int GetPosFromXY(int xpos, int ypos, bool allowOutbound=true);

	full_pos ClickOnLine(int xpos, int ypos, bool doSetpos=true);
	full_pos MovePosUp(int xpos = -1);
	full_pos MovePosDown(int xpos = -1);

	// Selection
	bool IsSelected() const;
	bool IsMultiSelected() const;
	int GetLastSelection() const {return m_lastSel;};
	bool IsSelectionShadow() const {return m_isSelShadow;};
	bool IsSelectionMultiline();
	int AddSelection(unsigned int start, unsigned int end);
	int UpdateSelection(unsigned int sel_id, unsigned int start, unsigned int end);
	const vector<interval>& GetSelections() const;
	void ShadowSelections(bool isShadow=true) {m_isSelShadow = isShadow;};
	void RemoveAllSelections(bool checkShadow=false, unsigned int pos=0);
	void RemoveSelection(unsigned int sel_id);

	void AddStyler(Styler& styler);

	void Clear();
	cxFileResult LoadText(const wxFileName& path, wxFontEncoding enc, const wxString& mirror=wxEmptyString);
	void ReLoadText();

	void InsertChar(unsigned int pos, const wxChar& newtext, unsigned int byte_len);
	void Insert(unsigned int pos, unsigned int byte_len);
	void Delete(unsigned int startpos, unsigned int endpos);
	void ApplyDiff(vector<cxChange>& changes);
	void Draw(int xoffset, int yoffset, wxRect& rect);

	// Tabs
	bool IsAtTabPoint();

	// Folding
	wxRect GetFoldIndicatorRect(unsigned int line_id);
	bool IsOverFoldIndicator(const wxPoint& point);

	vector<cxLineChange> ChangesToFullLines(const vector<cxChange>& changes);

	void Print();
	int OnIdle() {return ll->OnIdle();};
	bool NeedIdle() const {return ll->NeedIdle();};
	void Verify(bool deep=false);

private:
	void SetCaretPos(bool update=false);
	void SetLine(unsigned int lineId);

	unsigned int GetUnFoldedHeight() const;
	unsigned int UnFoldedYPos(unsigned int ypos) const;
	unsigned int FoldedYPos(unsigned int ypos) const;

	// Member variables
	wxDC& dc;
	DocumentWrapper& m_doc;
	IFoldingEditor& m_editorCtrl;
	bool NewlineTerminated;
	unsigned int pos;
	unsigned int lastpos;
	wxPoint caretpos;
	FixedLine line;
	const tmTheme& m_theme;
	int m_lastSel;

	unsigned int m_marginChars;
	unsigned int m_marginPos;

	// Selection variables
	vector<interval> selections;
	bool m_isSelShadow;

	// LineList vars
	cxWrapMode m_wrapMode;
	LineList* ll;
	LineListWrap llWrap;
	LineListNoWrap llNoWrap;

#ifdef __WXDEBUG__
	bool m_verifyEnabled;
#endif
};

#endif // LINES_H

