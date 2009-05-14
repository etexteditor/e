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

#ifdef __WXMSW__
    #pragma warning(disable: 4786)
#endif

#ifndef __TM_SYNTAXHANDLER_H__
#define __TM_SYNTAXHANDLER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#include <deque>
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

using namespace std;

#include "plistHandler.h"
#include "tmTheme.h"
#include "tmKey.h"


// Pre-definitions
class IEditorDoAction;

class matcher;
class match_matcher;
class span_matcher;
class group_matcher;

class TiXmlElement;

class DocumentWrapper;
class Dispatcher;

struct style;

struct tmBundle {
	unsigned int bundleRef;
	wxString name;
	wxString uuid;
	wxFileName path;
};

class tmAction {
public:
	tmAction() :  isUnix(true), bundle(NULL), contentLoaded(false) {};
	virtual ~tmAction() {};

	virtual bool IsSnippet() const {return false;};
	virtual bool IsCommand() const {return false;};
	virtual bool IsDrag() const {return false;};
	virtual bool IsSyntax() const {return false;};
	void SwapContent(vector<char>& c) {
		cmdContent.swap(c);
		contentLoaded = true;
	};

	// Member variables
	bool isUnix;
	wxString name;
	wxString uuid;
	wxString scope;
	wxString trigger;
	tmKey key;
	const tmBundle* bundle;

private:
	friend class TmSyntaxHandler;

	unsigned int plistRef;
	mutable bool contentLoaded;
	mutable vector<char> cmdContent;
};

class tmSnippet : public tmAction {
public:
	bool IsSnippet() const {return true;};
	virtual ~tmSnippet() {};
};

class tmCommand : public tmAction {
public:
	tmCommand()
	: save(csNONE), input(ciNONE), inputXml(false), fallbackInput(ciNONE), output(coNONE) {};
	virtual ~tmCommand() {};

	bool IsCommand() const {return true;};

	enum CmdSave {csNONE, csDOC, csALL};
	enum CmdInput {ciNONE, ciSEL, ciDOC, ciLINE, ciWORD, ciCHAR, ciSCOPE};
	enum CmdOutput {coNONE, coSEL, coDOC, coINSERT, coSNIPPET, coTOOLTIP, coHTML, coNEWDOC, coREPLACEDOC};

	CmdSave save;
	CmdInput input;
	bool inputXml;
	CmdInput fallbackInput;
	CmdOutput output;
};

class tmActionCmp : public binary_function<tmAction*, tmAction*, bool> {
public:
	bool operator()(const tmAction* x, const tmAction* y) const {return x->name.CmpNoCase(y->name) < 0;};
};

class tmDragCommand : public tmCommand {
public:
    virtual ~tmDragCommand() {};

	bool IsDrag() const {return true;};

	wxArrayString extArray;
};

class cxSyntaxInfo : public tmAction {
public:
	cxSyntaxInfo(unsigned int bundleid, unsigned int syntaxid)
		: bundleId(bundleid), syntaxId(syntaxid), topmatcher(NULL) {};
    virtual ~cxSyntaxInfo() {};

	bool IsOk() const {return !name.empty();};
	bool IsSyntax() const {return true;};

	wxArrayString filewild;
	wxString firstline;
	unsigned int bundleId;
	unsigned int syntaxId;
	matcher* topmatcher;
};


struct tmPrefs {
	wxString increaseIndentPattern;
	wxString decreaseIndentPattern;
	wxString indentNextLinePattern;
	wxString unIndentedLinePattern;
};

struct tmCompletionCmd {
	vector<char> cmd;
	tmBundle* bundle;
};

template <class T> class sNode {
public:
	sNode();
	sNode(const wxString& word);
	~sNode();
	void clear();

	typedef map<wxString,sNode<T>*> NodeMap;

	const vector<const T*>* GetMatch(const deque<const wxString*>& scopes) const;
	void GetMatches(const deque<const wxString*>& scopes, vector<const T*>& result) const;
	template<class P> void GetMatches(const deque<const wxString*>& scopes, vector<const T*>& result, P& pred) const {
		if (!scopes.empty()) {
			if (orNodes) {
				// We start at the bottom of the scope and we keep going a level up
				size_t ndx = scopes.size();
				while (ndx > 0) {
					--ndx;

					// Parse the current scope
					wxArrayString words;
					Tokenize(*scopes[ndx], words);
					const wxString& w = words[0];

					typename NodeMap::const_iterator p = orNodes->find(w);
					if (p != orNodes->end()) {
						if (p->second->MatchPredicate(words, 0, scopes, ndx, result, pred)) return;

						// TODO: Add a ref to the triggering scope level
					}
				}
			}
		}

		// match (but there may be no targets here)
		if (targets) {
			typename vector<const T*>::const_iterator p = targets->begin();
			while (p != targets->end()) {
				const T* t = *p;

				if (pred(t)) {
					result.insert(result.end(), *p);
				}
				++p;
			}
		}
	};

	const vector<const T*>* Match(const wxString& scope, size_t scopePos, const deque<const wxString*>& scopes, size_t level) const;
	void Matches(const wxArrayString& words, unsigned int pos, const deque<const wxString*>& scopes, size_t level, vector<const T*>& result) const;
	template<class P> bool MatchPredicate(const wxArrayString& words, unsigned int pos, const deque<const wxString*>& scopes, size_t level, vector<const T*>& result, P& pred) const {
		wxASSERT(word == words[pos]);

		const unsigned int nextPos = pos+1;
		if (postfix && nextPos < words.Count()) {
			typename NodeMap::const_iterator p = postfix->find(words[nextPos]);
			if (p != postfix->end()) {
				if (p->second->MatchPredicate(words, nextPos, scopes, level, result, pred)) return true;
			}
		}

		// If we didn't find a match look for ancestors
		if (level && ancestors) {
			// Parse the ancestor scope
			const size_t ndx = level - 1;
			wxArrayString words2;
			Tokenize(*scopes[ndx], words2);
			const wxString& w = words2[0];

			typename NodeMap::const_iterator p = ancestors->find(w);
			if (p != ancestors->end()) {
				if (p->second->MatchPredicate(words2, 0, scopes, ndx, result, pred)) return true;
			}
		}

		// match (but there may be no targes here)
		bool isFound = false;
		if (targets) {
			typename vector<const T*>::const_iterator p = targets->begin();
			while (p != targets->end()) {
				if (pred(*p)) {
					result.insert(result.end(), *p);
					isFound = true;
				}
				++p;
			}
		}

		return isFound;
	};

	void AddAncestor(sNode* n);
	void AddOrNode(sNode* n);
	void Merge(sNode* n);

	// Member variables
	wxString word;
	NodeMap* postfix;
	NodeMap* orNodes;
	NodeMap* ancestors;
	vector<const T*>* targets;

	void Print(size_t indent=0) const;

private:
	void Tokenize(const wxString& scope, wxArrayString& words) const;
};

class TmSyntaxHandler {
public:
	TmSyntaxHandler(Dispatcher& disp, bool clearCache);
	~TmSyntaxHandler();

	enum cxBundleLoad {
		cxINIT,
		cxUPDATE,
		cxRELOAD
	};

	class ShortcutMatch : public unary_function<const tmAction*, bool> {
	public:
		ShortcutMatch(int keyCode, int modifiers)
			: m_keyCode(keyCode), m_modifiers(modifiers) {};
		bool operator()(const tmAction* x) const {
			const tmKey& key = x->key;
			return (key.keyCode == m_keyCode && key.modifiers == m_modifiers);
		};
	private:
		const int m_keyCode;
		const int m_modifiers;
	};

	class ExtMatch : public unary_function<const tmDragCommand*, bool> {
	public:
		ExtMatch(const wxString& ext) : m_ext(ext) {};
		bool operator()(const tmDragCommand* x) const {
			const int ndx = x->extArray.Index(m_ext, false);
			return (ndx != wxNOT_FOUND);
		};
	private:
		const wxString& m_ext;
	};

	class PrefsMatch : public unary_function<const tmPrefs*, bool> {
	public:
		enum PrefType {
			increaseIndentPattern,
			decreaseIndentPattern,
			indentNextLinePattern,
			unIndentedLinePattern
		};

		PrefsMatch(PrefType target) : m_target(target) {};
		bool operator()(const tmPrefs* x) const {
			switch(m_target) {
			case increaseIndentPattern:
				if (!x->increaseIndentPattern.empty()) return true;
				break;
			case decreaseIndentPattern:
				if (!x->decreaseIndentPattern.empty()) return true;
				break;
			case indentNextLinePattern:
				if (!x->indentNextLinePattern.empty()) return true;
				break;
			case unIndentedLinePattern:
				if (!x->unIndentedLinePattern.empty()) return true;
				break;
			}
			return false;
		};
	private:
		const PrefType m_target;
	};

	// Plists
	PListHandler& GetPListHandler() {return m_plistHandler;};
	bool DoIdle();

	// Bundle Parsing
	void LoadBundles(cxBundleLoad mode);
	void ReParseBundles(bool onlyMenu=false);
	void LoadSyntaxes(const vector<unsigned int>& bundles);
	void LoadBundle(unsigned int bundeId);
	bool AllBundlesLoaded() const {return m_nextBundle == m_bundleList.size();};

	wxMenu* GetBundleMenu();
	wxString GetBundleItemUriFromMenu(unsigned int id) const;
	void DoBundleAction(unsigned int id, IEditorDoAction& editor);

	// Paths for bundles
	wxFileName GetBundleSupportPath(unsigned int bundleId) const;

	// Themes
	bool SetTheme(const char* uuid);
	void SetDefaultTheme();
	const tmTheme& GetTheme() const {return m_currentTheme;};
	const wxString& GetCurrentThemeName() const {return m_currentTheme.name;};
	const wxFont& GetFont() const {return m_currentTheme.font;};
	void SetFont(const wxFont& font);

	// Syntax
	const vector<cxSyntaxInfo*>& GetSyntaxes() const {return m_syntaxes;};
	void GetSyntaxes(wxArrayString& nameArray) const;
	const cxSyntaxInfo* GetSyntax(const wxString& syntaxName, const wxString& ext=wxEmptyString);
	const cxSyntaxInfo* GetSyntax(const DocumentWrapper& document);

	// Style
	const style* GetStyle(const deque<const wxString*>& scopes) const;

	// Actions
	void GetAllActions(const deque<const wxString*>& scopes, vector<const tmAction*>& result) const;
	void GetActions(const deque<const wxString*>& scopes, vector<const tmAction*>& result, const ShortcutMatch& matchfun) const;
	void GetDragActions(const deque<const wxString*>& scopes, vector<const tmDragCommand*>& result, const ExtMatch& matchfun) const;
	const vector<const tmAction*> GetActions(const wxString& trigger, const deque<const wxString*>& scopes) const;
	const vector<char>& GetActionContent(const tmAction& action) const;

	// Indentation
	const wxString& GetIndentNonePattern(const deque<const wxString*>& scopes) const;
	const wxString& GetIndentNextPattern(const deque<const wxString*>& scopes) const;
	const wxString& GetIndentIncreasePattern(const deque<const wxString*>& scopes) const;
	const wxString& GetIndentDecreasePattern(const deque<const wxString*>& scopes) const;

	// Shell variables
	map<wxString, wxString> GetShellVariables(const deque<const wxString*>& scopes) const;

	// Smart Typing Pairs
	map<wxString, wxString> GetSmartTypingPairs(const deque<const wxString*>& scopes) const;

	// Completion
	const vector<wxString>* GetCompletionList(const deque<const wxString*>& scopes) const;
	const tmCompletionCmd* GetCompletionCmd(const deque<const wxString*>& scopes) const;
	bool DisableDefaultCompletion(const deque<const wxString*>& scopes) const;

	// Symbol
	bool ShowSymbol(const deque<const wxString*>& scopes, const wxString*& transform) const;

	// Folding
	class cxFoldRule {
	public:
		cxFoldRule(unsigned int id, const char* startMarker, const char* endMarker);
		const unsigned int ruleId;
		const vector<char> foldingStartMarker;
		const vector<char> foldingEndMarker;
	};
	const cxFoldRule* GetFoldRule(const deque<const wxString*>& scopes) const;

private:
	void ClearBundleInfo();
	void ClearBundleActions();

	// Syntax parsing
	cxSyntaxInfo* GetSyntaxInfo(unsigned int bundleId, unsigned int syntaxId);
	const cxSyntaxInfo* InitSyntax(cxSyntaxInfo& si, bool isTop=true);
	bool ParseSyntax(cxSyntaxInfo& si);
	bool ParseGroup(const PListArray& groupArray, group_matcher& gm);
	bool ParsePattern(const PListDict& patternDict, matcher*& m);
	bool ParseInclude(const PListDict& patternDict, matcher*& m);
	bool ParseMatch(match_matcher* mm, const PListDict& patternDict);
	bool ParseSpan(span_matcher* sm, const PListDict& patternDict);
	bool ParseCaptures(match_matcher& m, const PListDict& captureDict);
	matcher* GetFromRepo(const char* name);
	matcher* GetMatcher(const wxString& scope);

	// Theme parsing
	bool GetThemeName(const wxFileName& path, wxString& name);
	bool LoadTheme(const char* uuid);
	bool ParseSettings(const PListDict& dict, tmTheme& settings);
	bool ParseStyle(const PListDict& dict, vector<style*>& styles, tmTheme& settings);
	wxColour ParseColor(const wxString& color_hex, const wxColour& bgColor);

	// Snippet parsing
	bool ParseSnippet(const tmBundle& bundle, unsigned int snippetId);

	// Command parsing
	bool ParseCommand(const tmBundle& bundle, unsigned int commandId);

	// DragCommand parsing
	bool ParseDragCommand(const tmBundle& bundle, unsigned int commandId);

	// Preference parsing
	bool ParsePreferences(const PListDict& prefDict, tmBundle* bundle);

	// Info parsing
	bool ParseInfo(const PListDict& infoDict, tmBundle& bundle);
	wxMenu* ParseMenu(const PListArray& itemsArray, const PListDict& submenuDict);

	// Memory managment
	match_matcher* NewMatcher();
	span_matcher* NewSpan();
	group_matcher* NewGroup();


	class cp_less : public binary_function<const char*, const char*, bool> {
	public:
		bool operator()(const char* x, const char* y) const {return strcmp(x,y) < 0;};
	};

	// Member variables
	PListHandler m_plistHandler;
	Dispatcher& m_dispatcher;
	tmTheme m_defaultTheme;
	tmTheme m_currentTheme;
	vector<tmBundle*> m_bundles;
	vector<cxSyntaxInfo*> m_syntaxes;
	vector<matcher*> m_matchers;
	vector<style*> m_styles;
	sNode<style>* m_styleNode;
	sNode<tmAction> m_actionNode;
	sNode<tmDragCommand> m_dragNode;
	map<const wxString, tmAction*> m_actions;
	map<const wxString, sNode<tmAction>*> m_actionTriggers;
	map<unsigned int, wxString> m_menuActions;
	wxMenu* m_bundleMenu;
	unsigned int m_nextMenuID;
	unsigned int m_nextFoldID;

	// Idle time bundle parsing
	vector<unsigned int> m_bundleList;
	bool m_doUpdateBundles;
	unsigned int m_nextBundle;

	// Preferences
	vector<tmPrefs*> m_prefs;
	vector<map<wxString, wxString>* > m_shellVarPrefs;
	vector<map<wxString, wxString>* > m_smartTypingPairs;
	vector<vector<wxString>* > m_completions;
	vector<tmCompletionCmd*> m_completionCmds;
	vector<wxString*> m_symbolTransforms;
	vector<cxFoldRule*> m_foldRules;
 	sNode<tmPrefs> m_prefsNode;
	sNode<map<wxString, wxString> > m_shellVarNode;
	sNode<map<wxString, wxString> > m_smartPairsNode;
	sNode<vector<wxString> > m_completionsNode;
	sNode<tmCompletionCmd> m_completionCmdNode;
	sNode<void> m_disableCompletionNode;
	sNode<wxString> m_symbolNode;
	sNode<cxFoldRule> m_foldNode;

	// Syntax parser state
	cxSyntaxInfo* m_currentSyntax;
	vector<matcher*> * m_currentMatchers;
	const PListDict* m_currentRepo;
	map<const char*,matcher*,cp_less> * m_currentParsedReps;
	map<const char*,matcher**,cp_less> * m_repsInParsing;

	static const wxString s_emptyString;
};

template <class T> class SelectorParser {
public:
	SelectorParser(const wxString& selector, const T* target);

	sNode<T>* ParseExpr();

private:
	enum selToken {TOKEN_EOF, TOKEN_WORD, TOKEN_DOT, TOKEN_OR, TOKEN_PARAN_START, TOKEN_PARAN_END, TOKEN_MINUS};

	// Lexer
	selToken GetNextToken();

	// Parser
	bool ParseScope();
	bool ParseWord();

	// Member variables
	const T* m_target;
	const wxString& m_selector;
	const size_t m_len;
	unsigned int m_pos;
	selToken m_currentToken;
	wxString m_tokenValue;
	unsigned int m_errorPos;
	wxString m_error;

	sNode<T>* m_rootNode;
	sNode<T>* m_scopeNode;
	sNode<T>* m_currentNode;
};

#endif // __TM_SYNTAXHANDLER_H__
