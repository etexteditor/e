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

#ifndef __TM_SYNTAXHANDLER_H__
#define __TM_SYNTAXHANDLER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/filename.h>

#include <vector>
#include <deque>
#include <map>

#include "tmBundle.h"
#include "tmAction.h"
#include "tmCommand.h"
#include "tmTheme.h"
#include "tmKey.h"
#include "SyntaxInfo.h"

#include "IGetPListHandlerRef.h"
#include "ITmThemeHandler.h"
#include "ITmGetSyntaxes.h"
#include "ITmLoadBundles.h"

// Pre-definitions
class PListHandler;
class PListDict;
class PListArray;

class IEditorDoAction;

class matcher;
class match_matcher;
class span_matcher;
class group_matcher;

class TiXmlElement;

class DocumentWrapper;
class Dispatcher;

struct style;

class tmSnippet : public tmAction {
public:
	bool IsSnippet() const {return true;};
	virtual ~tmSnippet() {};
};


class tmDragCommand : public tmCommand {
public:
    virtual ~tmDragCommand() {};

	bool IsDrag() const {return true;};

	wxArrayString extArray;
};

struct tmPrefs {
	wxString increaseIndentPattern;
	wxString decreaseIndentPattern;
	wxString indentNextLinePattern;
	wxString unIndentedLinePattern;
};

struct tmCompletionCmd {
	std::vector<char> cmd;
	tmBundle* bundle;
};

template <class T> class sNode {
public:
	sNode();
	sNode(const wxString& word);
	~sNode();
	void clear();

	typedef std::map<wxString,sNode<T>*> NodeMap;

	const std::vector<const T*>* GetMatch(const std::deque<const wxString*>& scopes) const;
	void GetMatches(const std::deque<const wxString*>& scopes, std::vector<const T*>& result) const;
	template<class P> void GetMatches(const std::deque<const wxString*>& scopes, std::vector<const T*>& result, P& pred) const {
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
			typename std::vector<const T*>::const_iterator p = targets->begin();
			while (p != targets->end()) {
				const T* t = *p;

				if (pred(t)) {
					result.insert(result.end(), *p);
				}
				++p;
			}
		}
	};

	const std::vector<const T*>* Match(const wxString& scope, size_t scopePos, const std::deque<const wxString*>& scopes, size_t level) const;
	void Matches(const wxArrayString& words, unsigned int pos, const std::deque<const wxString*>& scopes, size_t level, std::vector<const T*>& result) const;
	template<class P> bool MatchPredicate(const wxArrayString& words, unsigned int pos, const std::deque<const wxString*>& scopes, size_t level, std::vector<const T*>& result, P& pred) const {
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
			typename std::vector<const T*>::const_iterator p = targets->begin();
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
	std::vector<const T*>* targets;

	void Print(size_t indent=0) const;

private:
	void Tokenize(const wxString& scope, wxArrayString& words) const;
};

class TmSyntaxHandler:
	public ITmThemeHandler,
	public ITmLoadBundles,
	public ITmGetSyntaxes
{
public:
	TmSyntaxHandler(Dispatcher& disp, PListHandler& plistHandler);
	~TmSyntaxHandler();

	class ShortcutMatch : public std::unary_function<const tmAction*, bool> {
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

	class ExtMatch : public std::unary_function<const tmDragCommand*, bool> {
	public:
		ExtMatch(const wxString& ext) : m_ext(ext) {};
		bool operator()(const tmDragCommand* x) const {
			const int ndx = x->extArray.Index(m_ext, false);
			return (ndx != wxNOT_FOUND);
		};
	private:
		const wxString& m_ext;
	};

	class PrefsMatch : public std::unary_function<const tmPrefs*, bool> {
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
	virtual PListHandler& GetPListHandler() {return m_plistHandler;};
	bool DoIdle();

	// Bundle Parsing
	virtual void LoadBundles(cxBundleLoad mode);
	virtual void ReParseBundles(bool onlyMenu=false);
	void LoadSyntaxes(const std::vector<unsigned int>& bundles);
	void LoadBundle(unsigned int bundeId);
	bool AllBundlesLoaded() const {return m_nextBundle == m_bundleList.size();};

	wxMenu* GetBundleMenu();
	wxString GetBundleItemUriFromMenu(unsigned int id) const;
	void DoBundleAction(unsigned int id, IEditorDoAction& editor);

	// Paths for bundles
	wxFileName GetBundleSupportPath(unsigned int bundleId) const;

	// Themes
	virtual bool SetTheme(const char* uuid);
	virtual void SetDefaultTheme();
	virtual const tmTheme& GetTheme() const {return m_currentTheme;};
	virtual const wxString& GetCurrentThemeName() const {return m_currentTheme.name;};
	virtual const wxFont& GetFont() const {return m_currentTheme.font;};
	virtual void SetFont(const wxFont& font);

	// Syntax
	virtual const std::vector<cxSyntaxInfo*>& GetSyntaxes() const {return m_syntaxes;};
	void GetSyntaxes(wxArrayString& nameArray) const;
	const cxSyntaxInfo* GetSyntax(const wxString& syntaxName, const wxString& ext=wxEmptyString);
	const cxSyntaxInfo* GetSyntax(const DocumentWrapper& document);

	// Style
	const style* GetStyle(const std::deque<const wxString*>& scopes) const;

	// Actions
	void GetAllActions(const std::deque<const wxString*>& scopes, std::vector<const tmAction*>& result) const;
	void GetActions(const std::deque<const wxString*>& scopes, std::vector<const tmAction*>& result, const ShortcutMatch& matchfun) const;
	void GetDragActions(const std::deque<const wxString*>& scopes, std::vector<const tmDragCommand*>& result, const ExtMatch& matchfun) const;
	const std::vector<const tmAction*> GetActions(const wxString& trigger, const std::deque<const wxString*>& scopes) const;
	const std::vector<char>& GetActionContent(const tmAction& action) const;

	// Indentation
	const wxString& GetIndentNonePattern(const std::deque<const wxString*>& scopes) const;
	const wxString& GetIndentNextPattern(const std::deque<const wxString*>& scopes) const;
	const wxString& GetIndentIncreasePattern(const std::deque<const wxString*>& scopes) const;
	const wxString& GetIndentDecreasePattern(const std::deque<const wxString*>& scopes) const;

	// Shell variables
	std::map<wxString, wxString> GetShellVariables(const std::deque<const wxString*>& scopes) const;

	// Smart Typing Pairs
	std::map<wxString, wxString> GetSmartTypingPairs(const std::deque<const wxString*>& scopes) const;

	// Completion
	const std::vector<wxString>* GetCompletionList(const std::deque<const wxString*>& scopes) const;
	const tmCompletionCmd* GetCompletionCmd(const std::deque<const wxString*>& scopes) const;
	bool DisableDefaultCompletion(const std::deque<const wxString*>& scopes) const;

	// Symbol
	bool ShowSymbol(const std::deque<const wxString*>& scopes, const wxString*& transform) const;

	// Folding
	class cxFoldRule {
	public:
		cxFoldRule(unsigned int id, const char* startMarker, const char* endMarker);
		const unsigned int ruleId;
		const std::vector<char> foldingStartMarker;
		const std::vector<char> foldingEndMarker;
	};
	const cxFoldRule* GetFoldRule(const std::deque<const wxString*>& scopes) const;

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
	bool ParseStyle(const PListDict& dict, std::vector<style*>& styles, tmTheme& settings);
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


	class cp_less : public std::binary_function<const char*, const char*, bool> {
	public:
		bool operator()(const char* x, const char* y) const {return strcmp(x,y) < 0;};
	};

	// Member variables
	PListHandler& m_plistHandler;
	Dispatcher& m_dispatcher;
	tmTheme m_defaultTheme;
	tmTheme m_currentTheme;
	std::vector<tmBundle*> m_bundles;
	std::vector<cxSyntaxInfo*> m_syntaxes;
	std::vector<matcher*> m_matchers;
	std::vector<style*> m_styles;
	sNode<style>* m_styleNode;
	sNode<tmAction> m_actionNode;
	sNode<tmDragCommand> m_dragNode;
	std::map<const wxString, tmAction*> m_actions;
	std::map<const wxString, sNode<tmAction>*> m_actionTriggers;
	std::map<unsigned int, wxString> m_menuActions;
	wxMenu* m_bundleMenu;
	unsigned int m_nextMenuID;
	unsigned int m_nextFoldID;

	// Idle time bundle parsing
	std::vector<unsigned int> m_bundleList;
	bool m_doUpdateBundles;
	unsigned int m_nextBundle;

	// Preferences
	std::vector<tmPrefs*> m_prefs;
	std::vector<std::map<wxString, wxString>* > m_shellVarPrefs;
	std::vector<std::map<wxString, wxString>* > m_smartTypingPairs;
	std::vector<std::vector<wxString>* > m_completions;
	std::vector<tmCompletionCmd*> m_completionCmds;
	std::vector<wxString*> m_symbolTransforms;
	std::vector<cxFoldRule*> m_foldRules;
 	sNode<tmPrefs> m_prefsNode;
	sNode<std::map<wxString, wxString> > m_shellVarNode;
	sNode<std::map<wxString, wxString> > m_smartPairsNode;
	sNode<std::vector<wxString> > m_completionsNode;
	sNode<tmCompletionCmd> m_completionCmdNode;
	sNode<void> m_disableCompletionNode;
	sNode<wxString> m_symbolNode;
	sNode<cxFoldRule> m_foldNode;

	// Syntax parser state
	cxSyntaxInfo* m_currentSyntax;
	std::vector<matcher*> * m_currentMatchers;
	const PListDict* m_currentRepo;
	std::map<const char*,matcher*,cp_less> * m_currentParsedReps;
	std::map<const char*,matcher**,cp_less> * m_repsInParsing;

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
