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

#ifndef __STYLER_SYNTAX_H__
#define __STYLER_SYNTAX_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Document.h"
#include "StyleRun.h"
#include "styler.h"
#include "pcre.h"
#include "matchers.h"
#include "tm_syntaxhandler.h"
#include "SymbolRef.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include "auto_vector.h"
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class Styler_Syntax : public Styler {
public:
	Styler_Syntax(const DocumentWrapper& dw, Lines& lines, TmSyntaxHandler& syntaxHandler);
	virtual ~Styler_Syntax() {Clear();};

	const wxString& GetName() const {return m_syntaxName;};

	bool IsOk() const {return m_topMatches.subMatcher != NULL;};
	bool IsParsed() const {return !IsOk() || m_syntax_end == m_doc.GetLength();};
	unsigned int GetLastParsedPos() const {return m_syntax_end;};

	bool UpdateSyntax();
	void SetSyntax(const wxString& syntaxName, const wxString& ext=wxEmptyString);
	void Style(StyleRun& sr);
	void ParseAll();
	void ParseTo(unsigned int pos);

	const deque<const wxString*> GetScope(unsigned int pos);
	void GetTextWithScopes(unsigned int start, unsigned int end, vector<char>& text);

	const deque<interval> GetScopeIntervals(unsigned int pos) const;

	void Clear();
	void Invalidate();
	void ReStyle();
	void Insert(unsigned int pos, unsigned int length);
	void Delete(unsigned int start_pos, unsigned int end_pos);
	void ApplyDiff(const vector<cxLineChange>& linechanges);

	bool OnIdle();

	void GetSymbols(vector<SymbolRef>& symbols) const;

private:
	// Definitions
	class submatch; // pre-def
	class stxmatch {
	public:
		stxmatch(const wxString& name, const matcher* m, unsigned int start, unsigned int end, style *st, submatch* submatch, stxmatch* parent);
		~stxmatch();
		const wxString& m_name;
		const matcher* m_matcher;
		unsigned int start;
		unsigned int end;
		const style *st;
		auto_ptr<submatch> subMatch;
		stxmatch* parent;
	};
	enum {
		cxSPAN_IS_CLOSED = 1,
		cxSPAN_HAS_STARTER = 2,
		cxSPAN_HAS_ENDER = 4,
		cxSPAN_HAS_CONTENT = 8,
		cxSPAN_IS_CONTENT = 16,
		cxIS_CAPTURES = 32
	};
	class submatch {
	public:
		submatch() : flags(0), subMatcher(NULL) {};
		int flags;
		auto_vector<stxmatch> matches;
		matcher* subMatcher;
	};
	struct SearchInfo {
		unsigned int pos;
		unsigned int line_id;
		unsigned int lineStart;
		unsigned int lineEnd;
		unsigned int lineLen;
		unsigned int changeEnd;
		unsigned int limit;
		vector<char> line;
		bool hitLimit;
		bool done;
	};
	class stxmatch_start_less : public binary_function<stxmatch*, stxmatch*, bool> {
	public:
		bool operator()(const stxmatch* x, const stxmatch* y) const {return x->start < y->start;};
	};
	class stxmatch_end_less : public binary_function<stxmatch*, stxmatch*, bool> {
	public:
		bool operator()(const stxmatch* x, const stxmatch* y) const {return x->end < y->end;};
	};

	unsigned int Search(submatch& submatches, SearchInfo& si, unsigned int scopeStart, unsigned int scopeEnd, stxmatch* scope);

	// Private methods
	void DoStyle(StyleRun& sr, unsigned int offset, const auto_vector<stxmatch>& matches);
	void DoSearch(unsigned int start, unsigned int end, unsigned int limit);
	unsigned int SubSearch(unsigned int offset, unsigned int start, unsigned int end, submatch& submatches, stxmatch* parent, bool doAdjust, bool& done);
	void CreateSpan(unsigned int starterStart, unsigned int starterEnd, matcher& subMatcher, unsigned int id, SearchInfo& si, stxmatch* scope, int rc, int* ovector);
	const style* GetStyle(stxmatch& m) const;
	void ReStyleSub(const submatch& sm);

	void GetSubScope(unsigned int pos, const submatch& sm, deque<const wxString*>& scopes) const;
	void GetSubScopeIntervals(unsigned int pos, unsigned int offset, const submatch& sm, deque<interval>& scopes) const;

	void AddCaptures(matcher& m, stxmatch& sm, unsigned int offset, const SearchInfo& si, int rc, int* ovector);
	void ReInitSpan(span_matcher& sm, unsigned int start, const SearchInfo& si, int rc=0, int* ovector=NULL);

	unsigned int AdjustForInsertion(unsigned int pos, unsigned int length, submatch& submatches, unsigned int o, unsigned int lineStart);
	unsigned int AdjustForDeletion(unsigned int start, unsigned int end, submatch& submatches, unsigned int o, unsigned int lineStart);

	void XmlText(unsigned int offset, const submatch& sm, unsigned int start, unsigned int end, vector<char>& text) const;

	void GetSubSymbols(unsigned int offset, const submatch& sm, deque<const wxString*>& scopes, vector<SymbolRef>& symbols) const;

	// Member variables
	const DocumentWrapper& m_doc;
	TmSyntaxHandler& m_syntaxHandler;
	Lines& m_lines;
	unsigned int m_syntax_end;
	static const unsigned int EXTSIZE;
	wxString m_syntaxName;
	bool m_updateLineHeight;

	submatch m_topMatches;
	const style* m_topStyle;

#ifdef __WXDEBUG__
	void Print() const;
	void PrintMatches(unsigned int level, const submatch& submatches) const;
	void Verify() const;
	void VerifyMatch(const stxmatch* m) const;

	bool m_verifyEnabled;
#endif  //__WXDEBUG__
};

#endif // __STYLER_SYNTAX_H__
