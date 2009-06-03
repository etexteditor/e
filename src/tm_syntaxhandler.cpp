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

#include "tm_syntaxhandler.h"

#include <wx/ffile.h>
#include <wx/dir.h>

#include "pcre.h"

#include "plistHandler.h"
#include "Document.h"
#include "eSettings.h"
#include "matchers.h"
#include "Dispatcher.h"
#include "BundleMenu.h"
#include "tmStyle.h"

#include "IAppPaths.h"
#include "IEditorDoAction.h"

// tinyxml includes unused vars so it can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include "tinyxml.h"
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

class DirTraverserSimple : public wxDirTraverser
{
public:
    DirTraverserSimple(wxArrayString& dirs) : m_dirs(dirs) { }
    virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
    {
        return wxDIR_IGNORE;
    }
    virtual wxDirTraverseResult OnDir(const wxString& dirname)
    {
        m_dirs.Add(dirname);
		return wxDIR_IGNORE;
    }
private:
    wxArrayString& m_dirs;
};


// Initialize static variables
const wxString TmSyntaxHandler::s_emptyString;

TmSyntaxHandler::TmSyntaxHandler(Dispatcher& disp, PListHandler& plistHandler)
: m_plistHandler(plistHandler),
  m_dispatcher(disp), m_styleNode(NULL), m_bundleMenu(NULL), m_nextMenuID(9000), m_nextFoldID(0), m_doUpdateBundles(true),
  m_nextBundle(0), m_currentSyntax(NULL), m_currentMatchers(NULL), m_currentParsedReps(NULL), m_repsInParsing(NULL) {
	// Initialize TinyXml
	TiXmlBase::SetCondenseWhiteSpace(false);

	// Initialize default theme
	m_defaultTheme.backgroundColor = *wxWHITE;
	m_defaultTheme.foregroundColor = *wxBLACK;
	m_defaultTheme.selectionColor = wxColour(138, 187, 222); // e Blue
	m_defaultTheme.shadowColor = wxColour(225, 225, 225);
	m_defaultTheme.invisiblesColor = wxColour(112, 112, 112); // medium gray
	m_defaultTheme.lineHighlightColor = *wxWHITE;
	m_defaultTheme.searchHighlightColor = wxColour(wxT("Yellow"));
	m_defaultTheme.gutterColor = wxColour(192, 192, 255); // Pastel purple

	// Get font
	wxString fontDesc;
	eSettings& settings = eGetSettings();
	if (settings.GetSettingString(wxT("font"), fontDesc)) {
		m_defaultTheme.font.SetNativeFontInfo(fontDesc);
	}
	if (!m_defaultTheme.font.Ok()) {
		m_defaultTheme.font = wxFont(9, wxMODERN, wxNORMAL, wxNORMAL, false);
	}

	m_currentTheme = m_defaultTheme;

	// Load Theme
	wxString themeUuid;
	if (settings.GetSettingString(wxT("theme_id"), themeUuid)) {
		SetTheme(themeUuid.mb_str(wxConvUTF8));
	}
	else SetTheme("71D40D9D-AE48-11D9-920A-000D93589AF6"); // Default Theme ("Mac Classic")

	// Load the bundles (actions are loaded in idle time)
	m_bundleList = m_plistHandler.GetBundles(); // Get a list of all bundles
	m_bundleMenu = new wxMenu;
	if (!m_bundleList.empty()) {
		m_bundleMenu->Append(m_nextMenuID, _("Loading Bundles..."));
		m_bundleMenu->Enable(m_nextMenuID, false);
	}

	// The syntaxes are the only parts initially updated
	// (commands, snippets.. are updated in next idle time)
	LoadSyntaxes(m_bundleList);
}

TmSyntaxHandler::~TmSyntaxHandler() {
	ClearBundleInfo();
}

bool TmSyntaxHandler::DoIdle() {
	// The plisthandler constructor only updated info.plist and syntaxes.
	// So we tell it to update the rest here (to get faster startup time).
	if (m_doUpdateBundles) {
		m_plistHandler.Update(PListHandler::UPDATE_REST);
		m_doUpdateBundles = false;
		return true;
	}

	// Load one bundle per idle event to distribute load
	if (m_nextBundle < m_bundleList.size()) {
		// Frame may already have retrieved menu
		if (!m_bundleMenu) m_bundleMenu = new wxMenu;

		// Load bundle
		const unsigned int bundleId = m_bundleList[m_nextBundle];
		LoadBundle(bundleId);

		++m_nextBundle;

		if (m_nextBundle == m_bundleList.size()) {
			// Notify that all bundle actions have been loaded
			m_dispatcher.Notify(wxT("BUNDLE_ACTIONS_RELOADED"), NULL, 0);
		}
		else return true;
	}

	return false; // need no more idle events
}

void TmSyntaxHandler::ClearBundleInfo() {
	// Release allocated syntaxes
	for (vector<cxSyntaxInfo*>::iterator x = m_syntaxes.begin(); x != m_syntaxes.end(); ++x) {
		delete *x;
	}
	m_syntaxes.clear();

	// Release allocated matchers
	for (vector<matcher*>::iterator mp = m_matchers.begin(); mp != m_matchers.end(); ++mp) {
		delete *mp;
	}
	m_matchers.clear();

	// Release allocated bundles
	for (vector<tmBundle*>::iterator b = m_bundles.begin(); b != m_bundles.end(); ++b) {
		delete *b;
	}
	m_bundles.clear();

	// Release allocated styles
	for (vector<style*>::iterator v = m_styles.begin(); v != m_styles.end(); ++v) {
		delete *v;
	}
	m_styles.clear();
	delete m_styleNode;
	m_styleNode = NULL;

	// Release allocated indentation preferences
	for (vector<tmPrefs*>::iterator p = m_prefs.begin(); p != m_prefs.end(); ++p) {
		delete *p;
	}
	m_prefs.clear();
	m_prefsNode.clear();

	// Release allocated shell var preferences
	for (vector<map<wxString, wxString>* >::iterator s = m_shellVarPrefs.begin(); s != m_shellVarPrefs.end(); ++s) {
		delete *s;
	}
	m_shellVarPrefs.clear();
	m_shellVarNode.clear();

	// Release allocated smart typing pairs
	for (vector<map<wxString, wxString>* >::iterator t = m_smartTypingPairs.begin(); t != m_smartTypingPairs.end(); ++t) {
		delete *t;
	}
	m_smartTypingPairs.clear();
	m_smartPairsNode.clear();

	// Release allocated completion lists
	for (vector<vector<wxString>*>::iterator c = m_completions.begin(); c != m_completions.end(); ++c) {
		delete *c;
	}
	m_completions.clear();
	m_completionsNode.clear();

	// Release allocated completion commands
	for (vector<tmCompletionCmd*>::iterator cc = m_completionCmds.begin(); cc != m_completionCmds.end(); ++cc) {
		delete *cc;
	}
	m_completionCmds.clear();
	m_completionCmdNode.clear();

	m_disableCompletionNode.clear();

	for (vector<wxString*>::iterator st = m_symbolTransforms.begin(); st != m_symbolTransforms.end(); ++st) {
		delete *st;
	}
	m_symbolTransforms.clear();
	m_symbolNode.clear();

	// Release allocated Fold Rules
	for (vector<cxFoldRule*>::iterator f = m_foldRules.begin(); f != m_foldRules.end(); ++f) {
		delete *f;
	}
	m_foldRules.clear();
	m_foldNode.clear();

	ClearBundleActions();
}

void TmSyntaxHandler::ClearBundleActions() {
	// Release allocated actions
	for (map<const wxString, tmAction*>::iterator s = m_actions.begin(); s != m_actions.end(); ++s) {
		delete s->second;
	}
	m_actions.clear();

	// Release allocated triggers
	for (map<const wxString, sNode<tmAction>*>::iterator t = m_actionTriggers.begin(); t != m_actionTriggers.end(); ++t) {
		delete t->second;
	}
	m_actionTriggers.clear();

	m_menuActions.clear();
	m_actionNode.clear();
	m_dragNode.clear();

	delete m_bundleMenu;
	m_bundleMenu = NULL;
	m_nextMenuID = 9000; // range is 9000-11999
}

void TmSyntaxHandler::LoadSyntaxes(const vector<unsigned int>& bundles) {
	for (unsigned int b = 0; b < bundles.size(); ++b) {
		const unsigned int bundleId = bundles[b];

		// Parse Syntaxes
		const vector<unsigned int> syntaxes = m_plistHandler.GetList(BUNDLE_LANGUAGE, bundleId);
		for (unsigned int s = 0; s < syntaxes.size(); ++s) {
			const unsigned int syntaxId = syntaxes[s];

			cxSyntaxInfo* si = GetSyntaxInfo(bundleId, syntaxId);
			if (si && si->IsOk()) {
				m_syntaxes.push_back(si);
			}
			else {
				// Notify user that the syntax could not be loaded
				wxString msg = _("Error parsing syntax file ");
				wxMessageBox(msg, _("Parse error"), wxICON_ERROR|wxOK);
			}
		}
	}
}

void TmSyntaxHandler::LoadBundle(unsigned int bundleId) {
	const wxFileName bPath = m_plistHandler.GetBundlePath(bundleId);

	tmBundle* bundle = new tmBundle;
	bundle->bundleRef = bundleId;
	bundle->path = bPath;

	wxLogDebug(wxT(" Loading bundle: %s"), bPath.GetFullPath().c_str());

	// Parse Commands
	const vector<unsigned int> commands = m_plistHandler.GetList(BUNDLE_COMMAND, bundleId);
	for (unsigned int c = 0; c < commands.size(); ++c) {
		const unsigned int commandId = commands[c];

		ParseCommand(*bundle, commandId);
	}

	// Parse Snippets
	const vector<unsigned int> snippets = m_plistHandler.GetList(BUNDLE_SNIPPET, bundleId);
	for (unsigned int n = 0; n < snippets.size(); ++n) {
		const unsigned int snippetId = snippets[n];

		ParseSnippet(*bundle, snippetId);
	}

	// Parse DragCommands
	const vector<unsigned int> dragCommands = m_plistHandler.GetList(BUNDLE_DRAGCMD, bundleId);
	for (unsigned int d = 0; d < dragCommands.size(); ++d) {
		const unsigned int commandId = dragCommands[d];

		ParseDragCommand(*bundle, commandId);
	}

	// Parse Preferences
	const vector<unsigned int> prefs = m_plistHandler.GetList(BUNDLE_PREF, bundleId);
	for (unsigned int p = 0; p < prefs.size(); ++p) {
		const unsigned int prefId = prefs[p];

		const PListDict prefDict = m_plistHandler.Get(BUNDLE_PREF, bundleId, prefId);
		ParsePreferences(prefDict, bundle);
	}

	// Parse Bundle info
	const PListDict infoDict = m_plistHandler.GetBundleInfo(bundleId);
	ParseInfo(infoDict, *bundle);

	// Add bundle to list
	m_bundles.push_back(bundle);
}

void TmSyntaxHandler::LoadBundles(cxBundleLoad mode) {
	if (mode == cxUPDATE || mode == cxRELOAD) {
		ClearBundleInfo();
		if (mode == cxUPDATE) m_plistHandler.Update();
	}

	m_bundleMenu = new wxMenu;

	// Get the list of all bundles
	m_bundleList = m_plistHandler.GetBundles();
	m_nextBundle = m_bundleList.size(); // make sure DoIdle() won't reparse them

	// Parse Syntaxes
	LoadSyntaxes(m_bundleList);

	// Parse Bundle Actions
	for (unsigned int b = 0; b < m_bundleList.size(); ++b) {
		const unsigned int bundleId = m_bundleList[b];
		LoadBundle(bundleId);
	}

	if (mode == cxUPDATE || mode == cxRELOAD) {
		// We also have to reload the current theme
		const wxCharBuffer themeUuid = m_currentTheme.uuid.mb_str(wxConvUTF8);
		if (!LoadTheme(themeUuid.data())) {
			m_currentTheme = m_defaultTheme;
		}

		// Notify that bundles have been reloaded
		m_dispatcher.Notify(wxT("BUNDLES_RELOADED"), NULL, 0);
	}
}

void TmSyntaxHandler::ReParseBundles(bool onlyMenu) {
	// WARNING: This function does not deal with deleted bundles.
	// No bundles should be deleted before calling this function.

	vector<unsigned int> bundles = m_plistHandler.GetBundles();

	// Build a map of the bundles that are already parsed
	map<unsigned int, tmBundle*> parsedBundles;
	for (unsigned int i = 0; i < m_bundles.size(); ++i) {
		const unsigned int bundleId = m_bundles[i]->bundleRef;
		parsedBundles[bundleId] = m_bundles[i];
	}

	// Clean up before parsing
	if (!onlyMenu) ClearBundleActions();
	else {
		delete m_bundleMenu;
		m_nextMenuID = 9000; // range is 9000-11999
	}
	m_bundleMenu = new wxMenu;
	

	// Parse Bundles
	for (unsigned int b = 0; b < bundles.size(); ++b) {
		const unsigned int bundleId = bundles[b];

		// Check if we already have this bundle
		tmBundle* bundle;
		map<unsigned int, tmBundle*>::const_iterator p = parsedBundles.find(bundleId);
		if (p != parsedBundles.end()) bundle = p->second;
		else {
			bundle = new tmBundle;
			bundle->bundleRef = bundleId;
			bundle->path = m_plistHandler.GetBundlePath(bundleId);

			// Add new bundle to list
			m_bundles.push_back(bundle);
		}

		if (!onlyMenu) {
			// Parse Commands
			const vector<unsigned int> commands = m_plistHandler.GetList(BUNDLE_COMMAND, bundleId);
			for (unsigned int c = 0; c < commands.size(); ++c) {
				const unsigned int commandId = commands[c];

				ParseCommand(*bundle, commandId);
			}

			// Parse Snippets
			const vector<unsigned int> snippets = m_plistHandler.GetList(BUNDLE_SNIPPET, bundleId);
			for (unsigned int n = 0; n < snippets.size(); ++n) {
				const unsigned int snippetId = snippets[n];

				ParseSnippet(*bundle, snippetId);
			}

			// Parse DragCommands
			const vector<unsigned int> dragCommands = m_plistHandler.GetList(BUNDLE_DRAGCMD, bundleId);
			for (unsigned int d = 0; d < dragCommands.size(); ++d) {
				const unsigned int commandId = dragCommands[d];

				ParseDragCommand(*bundle, commandId);
			}
		}

		// Parse Bundle info
		const PListDict infoDict = m_plistHandler.GetBundleInfo(bundleId);
		ParseInfo(infoDict, *bundle);
	}

	// Notify that bundle actions have been reloaded
	m_dispatcher.Notify(wxT("BUNDLE_ACTIONS_RELOADED"), NULL, 0);
}

wxMenu* TmSyntaxHandler::GetBundleMenu() {
	// The menu will be owned by reciever so we set
	// our ref to NULL to avoid double deletion
	wxMenu* m = m_bundleMenu;
	m_bundleMenu = NULL;
	return m;
}

wxFileName TmSyntaxHandler::GetBundleSupportPath(unsigned int bundleId) const {
	return m_plistHandler.GetBundleSupportPath(bundleId);
}

wxString TmSyntaxHandler::GetBundleItemUriFromMenu(unsigned int id) const {
	// Get uuid
	map<unsigned int, wxString>::const_iterator p = m_menuActions.find(id);
	if (p == m_menuActions.end()) {
		wxLogDebug(wxT("DoBundleAction: No matching menuAction"));
		return wxEmptyString;
	}
	const wxString& uuid = p->second;

	BundleItemType type;
	unsigned int bundleId;
	unsigned int itemId;
	if (!m_plistHandler.GetBundleItemFromUuid(uuid, type, bundleId, itemId)) return wxEmptyString;
	
	return m_plistHandler.GetBundleItemUri(type, bundleId, itemId);
}

void TmSyntaxHandler::DoBundleAction(unsigned int id, IEditorDoAction& editor) {
	// Get uuid
	map<unsigned int, wxString>::const_iterator p = m_menuActions.find(id);
	if (p == m_menuActions.end()) {
		wxLogDebug(wxT("DoBundleAction: No matching menuAction"));
		return;
	}
	const wxString& uuid = p->second;

	// Look for action
	map<const wxString, tmAction*>::iterator s = m_actions.find(uuid);
	if (s != m_actions.end()) {
		tmAction* action = s->second;

		editor.DoAction(*action, NULL, false);
	}
	else {
		wxLogDebug(wxT("DoBundleAction: No matching uuid"));
	}
}

bool TmSyntaxHandler::SetTheme(const char* uuid) {
	wxASSERT(uuid);

	if(!LoadTheme(uuid)) return false;

	eGetSettings().SetSettingString(wxT("theme_id"), wxString(uuid, wxConvUTF8));
	m_dispatcher.Notify(wxT("THEME_CHANGED"), NULL, 0);
	return true;
}

void TmSyntaxHandler::SetDefaultTheme() {
	m_currentTheme = m_defaultTheme;
	m_dispatcher.Notify(wxT("THEME_CHANGED"), NULL, 0);
}

void TmSyntaxHandler::SetFont(const wxFont& font) {
	eGetSettings().SetSettingString(wxT("font"), font.GetNativeFontInfoDesc());

	m_defaultTheme.font = font;
	m_currentTheme.font = font;
	m_dispatcher.Notify(wxT("THEME_CHANGED"), NULL, 0);
}

const style* TmSyntaxHandler::GetStyle(const deque<const wxString*>& scopes) const {
#ifdef __WXDEBUG__
	bool debug = false;
	if (debug) {
		for (unsigned int i = 0; i < scopes.size(); ++i) {
			wxLogDebug(*scopes[i]);
		}
		wxLogDebug(wxT(""));

		m_styleNode->Print();
	}
#endif //__WXDEBUG__

	if (m_styleNode) {
		const vector<const style*>* s = m_styleNode->GetMatch(scopes);
		if (s && !s->empty()) return (*s)[0];
	}

	return NULL;
}

const wxString& TmSyntaxHandler::GetIndentNonePattern(const deque<const wxString*>& scopes) const {
	// Find all matching indentation rules
	vector<const tmPrefs*> result;
	PrefsMatch pred(PrefsMatch::unIndentedLinePattern);
	m_prefsNode.GetMatches(scopes, result, pred);

	// Return the best match
	if (!result.empty()) return result[0]->unIndentedLinePattern;
	else return s_emptyString;
}

const wxString& TmSyntaxHandler::GetIndentNextPattern(const deque<const wxString*>& scopes) const {
	// Find all matching indentation rules
	vector<const tmPrefs*> result;
	PrefsMatch pred(PrefsMatch::indentNextLinePattern);
	m_prefsNode.GetMatches(scopes, result, pred);

	// Return the best match
	if (!result.empty()) return result[0]->indentNextLinePattern;
	else return s_emptyString;
}

const wxString& TmSyntaxHandler::GetIndentIncreasePattern(const deque<const wxString*>& scopes) const {
	// Find all matching indentation rules
	vector<const tmPrefs*> result;
	PrefsMatch pred(PrefsMatch::increaseIndentPattern);
	m_prefsNode.GetMatches(scopes, result, pred);

	// Return the best match
	if (!result.empty()) return result[0]->increaseIndentPattern;
	else return s_emptyString;
}

const wxString& TmSyntaxHandler::GetIndentDecreasePattern(const deque<const wxString*>& scopes) const {
	// Find all matching indentation rules
	vector<const tmPrefs*> result;
	PrefsMatch pred(PrefsMatch::decreaseIndentPattern);
	m_prefsNode.GetMatches(scopes, result, pred);

	// Return the best match
	if (!result.empty()) return result[0]->decreaseIndentPattern;
	else return s_emptyString;
}

map<wxString, wxString> TmSyntaxHandler::GetShellVariables(const deque<const wxString*>& scopes) const {
	m_shellVarNode.Print();

	// Get all shell variables
	const vector<const map<wxString, wxString>*>* result = m_shellVarNode.GetMatch(scopes);
	if (result && !result->empty()) {
		/*for (unsigned int i = 0; i < result->size(); ++i) {
			wxLogDebug(wxT("%d:"), i);
			const map<wxString, wxString>* m = (*result)[i];

			for (map<wxString, wxString>::const_iterator p = m->begin(); p != m->end(); ++p) {
				wxLogDebug(wxT("  %s: %s"), p->first, p->second);
			}
		}*/

		// Copy to a single map
		map<wxString, wxString> vars;
		for (unsigned int i = 0; i < result->size(); ++i) {
			const map<wxString, wxString>* res = (*result)[i];

			for (map<wxString, wxString>::const_iterator r = res->begin(); r != res->end(); ++r) {
				vars.insert(*r);
			}
		}

		return vars;
		//return *(*result)[0];
	}

	return map<wxString, wxString>();
}

map<wxString, wxString> TmSyntaxHandler::GetSmartTypingPairs(const deque<const wxString*>& scopes) const {
	const vector<const map<wxString, wxString>*>* result = m_smartPairsNode.GetMatch(scopes);
	if (result && !result->empty()) return *(*result)[0];
	return map<wxString, wxString>();
}

const vector<wxString>* TmSyntaxHandler::GetCompletionList(const deque<const wxString*>& scopes) const {
	const vector<const vector<wxString>*>* result = m_completionsNode.GetMatch(scopes);
	if (result && !result->empty()) return (*result)[0];
	return NULL;
}

const tmCompletionCmd* TmSyntaxHandler::GetCompletionCmd(const deque<const wxString*>& scopes) const {
	const vector<const tmCompletionCmd*>* result = m_completionCmdNode.GetMatch(scopes);
	if (result && !result->empty()) return (*result)[0];
	return NULL;
}

bool TmSyntaxHandler::DisableDefaultCompletion(const deque<const wxString*>& scopes) const {
	const vector<const void*>* result = m_disableCompletionNode.GetMatch(scopes);
	return result != NULL;
}

bool TmSyntaxHandler::ShowSymbol(const deque<const wxString*>& scopes, const wxString*& transform) const {
	const vector<const wxString*>* result = m_symbolNode.GetMatch(scopes);
	if (result && !result->empty()) {
		transform = (*result)[0];
		wxASSERT(transform->size() >= 0); // just to check it is a valid pointer
		return true;
	}
	return false;
}

const TmSyntaxHandler::cxFoldRule* TmSyntaxHandler::GetFoldRule(const deque<const wxString*>& scopes) const {
	const vector<const cxFoldRule*>* result = m_foldNode.GetMatch(scopes);
	if (result && !result->empty())  return (*result)[0];
	return NULL;
}

void TmSyntaxHandler::GetAllActions(const deque<const wxString*>& scopes, vector<const tmAction*>& result) const {
	m_actionNode.GetMatches(scopes, result);
}

void TmSyntaxHandler::GetActions(const deque<const wxString*>& scopes, vector<const tmAction*>& result, const ShortcutMatch& matchfun) const {
	// Find all matching snippets and commands
	m_actionNode.GetMatches(scopes, result, matchfun);

	// Find all matching syntaxes
	for (vector<cxSyntaxInfo*>::const_iterator x = m_syntaxes.begin(); x != m_syntaxes.end(); ++x) {
		if (matchfun(*x)) result.push_back(*x);
	}
}

void TmSyntaxHandler::GetDragActions(const deque<const wxString*>& scopes, vector<const tmDragCommand*>& result, const ExtMatch& matchfun) const {
	m_dragNode.GetMatches(scopes, result, matchfun);
}

const vector<const tmAction*> TmSyntaxHandler::GetActions(const wxString& trigger, const deque<const wxString*>& scopes) const {
	map<const wxString, sNode<tmAction>*>::const_iterator p = m_actionTriggers.find(trigger);

	if (p != m_actionTriggers.end()) {
		p->second->Print();
		const vector<const tmAction*>* s = (const vector<const tmAction*>*)p->second->GetMatch(scopes);
		if (s) return *s;
	}

	return vector<const tmAction*>();
}

const vector<char>& TmSyntaxHandler::GetActionContent(const tmAction& action) const {
	if (action.contentLoaded) return action.cmdContent;

	wxASSERT(action.bundle);

	// Load content into action
	if (action.IsCommand()) {
		const PListDict commandDict = action.IsDrag() ? m_plistHandler.Get(BUNDLE_DRAGCMD, action.bundle->bundleRef, action.plistRef)
													  : m_plistHandler.Get(BUNDLE_COMMAND, action.bundle->bundleRef, action.plistRef);

		const char* co = commandDict.GetString("winCommand");
		if (!co) co = commandDict.GetString("command");
		if (co) {
			action.cmdContent.assign(co, co + strlen(co));
			action.contentLoaded = true;
		}
	}
	else if (action.IsSnippet()) {
		const PListDict snippetDict = m_plistHandler.Get(BUNDLE_SNIPPET, action.bundle->bundleRef, action.plistRef);

		// Actual command
		const char* co = snippetDict.GetString("winContent");
		if (!co) co = snippetDict.GetString("content");
		if (co) {
			action.cmdContent.assign(co, co + strlen(co));
			action.contentLoaded = true;
		}
	}
	else wxASSERT(false);

	return action.cmdContent;
}

cxSyntaxInfo* TmSyntaxHandler::GetSyntaxInfo(unsigned int bundleId, unsigned int syntaxId) {
	const PListDict syntaxDict = m_plistHandler.Get(BUNDLE_LANGUAGE, bundleId, syntaxId);

	cxSyntaxInfo* si = new cxSyntaxInfo(bundleId, syntaxId);
	si->name = syntaxDict.wxGetString("name");
	si->scope = syntaxDict.wxGetString("scopeName");
	si->firstline = syntaxDict.wxGetString("firstLineMatch");

	// Key binding
	const char* binding = syntaxDict.GetString("keyEquivalent");
	if (binding) {
		si->key = tmKey(wxString(binding, wxConvUTF8));
	}

	// Get file types
	PListArray filetypesArray;
	if (syntaxDict.GetArray("fileTypes", filetypesArray)) {
		for (unsigned int i = 0; i < filetypesArray.GetSize(); ++i) {
			si->filewild.Add(filetypesArray.wxGetString(i));
		}
	}

	// Get Folding rules
	const char* foldingStartMarker = syntaxDict.GetString("foldingStartMarker");
	const char* foldingStopMarker = syntaxDict.GetString("foldingStopMarker");
	if (foldingStartMarker && foldingStopMarker) {
		cxFoldRule* rule = new cxFoldRule(m_nextFoldID++, foldingStartMarker, foldingStopMarker);

		// Add to action tree
		SelectorParser<cxFoldRule> parser(si->scope, rule);
		sNode<cxFoldRule>* n = parser.ParseExpr();
		if (n) m_foldNode.Merge(n);

		// Add to cleanup list
		m_foldRules.push_back(rule);
	}

	return si;
}

const cxSyntaxInfo* TmSyntaxHandler::InitSyntax(cxSyntaxInfo& si, bool WXUNUSED(isTop)) {
	if (si.topmatcher) return &si;
	if (ParseSyntax(si)) {
		//if (isTop && si.topmatcher) si.topmatcher->Init(true);
		/*if (isTop) {
			// There might be included matchers so we have to init all
			// top matchers before returning the info
			for (vector<cxSyntaxInfo*>::iterator p = m_syntaxes.begin(); p != m_syntaxes.end(); ++p) {
				if((*p)->topmatcher) (*p)->topmatcher->Init(true);
			}
		}*/

		return &si;
	}
	return NULL;
}

bool TmSyntaxHandler::ParseSyntax(cxSyntaxInfo& si) {
	wxASSERT(si.IsOk());

	const PListDict syntaxDict = m_plistHandler.Get(BUNDLE_LANGUAGE, si.bundleId, si.syntaxId);

	// Cache prev parser state
	cxSyntaxInfo* prevSyntax = m_currentSyntax;
	vector<matcher*>* prevMatchers = m_currentMatchers;
	const PListDict* prevRepo = m_currentRepo;
	map<const char*,matcher*,cp_less>* prevParsedReps = m_currentParsedReps;
	map<const char*,matcher**,cp_less>* prevRepsInParsing = m_repsInParsing;

	// Initialize parser state
	m_currentSyntax = &si;
	m_currentMatchers = new vector<matcher*>;

	group_matcher* topmatcher = NewGroup();
	si.topmatcher = topmatcher;
	m_currentRepo = NULL;
	m_currentParsedReps = new map<const char*,matcher*,cp_less>;
	m_repsInParsing = new map<const char*,matcher**,cp_less>;

	bool success = false;

	// We have to start by finding the repository since it might be ref'ed by others
	PListDict repositoryDict;
	if (syntaxDict.GetDict("repository", repositoryDict)) {
		m_currentRepo = &repositoryDict;
	}

	// Get name for top matcher
	si.topmatcher->SetName(syntaxDict.wxGetString("scopeName"));

	// Parse Patterns
	PListArray patternArray;
	if (syntaxDict.GetArray("patterns", patternArray)) {
		if (!ParseGroup(patternArray, *topmatcher)) goto error;
	}
	success = true;

	// copy allocted matchers to main list
	m_matchers.insert(m_matchers.end(), m_currentMatchers->begin(), m_currentMatchers->end());

error:
	if (!success) {
		// Release allocated matchers
		for (vector<matcher*>::iterator mp = m_currentMatchers->begin(); mp != m_currentMatchers->end(); ++mp) {
			delete *mp;
		}
	}
	delete m_currentMatchers;
	delete m_currentParsedReps;
	delete m_repsInParsing;

	// Restore state
	m_currentSyntax = prevSyntax;
	m_currentMatchers = prevMatchers;
	m_currentRepo = prevRepo;
	m_currentParsedReps = prevParsedReps;
	m_repsInParsing = prevRepsInParsing;

	return success;
}

bool TmSyntaxHandler::ParseGroup(const PListArray& groupArray, group_matcher& gm) {
	for (unsigned int i = 0; i < groupArray.GetSize(); ++i) {
		PListDict patternDict;
		if (groupArray.GetDict(i, patternDict)) {
			matcher* m = NULL;
			ParsePattern(patternDict, m);
			if (m) gm.AddMember(m);
		}
	}
	if (!gm.IsValid()) {
		wxLogDebug(wxT("WARNING: group_matcher %s is empty!"), gm.GetName().c_str());
	}

	return true;
}

bool TmSyntaxHandler::ParsePattern(const PListDict& patternDict, matcher*& m) {
	if (patternDict.HasKey("match")) {
		match_matcher* mm = NewMatcher();
		m = mm;
		return ParseMatch(mm, patternDict);
	}

	if (patternDict.HasKey("begin")) {
		span_matcher* sm = NewSpan();
		m = sm;
		return ParseSpan(sm, patternDict);
	}

	if (patternDict.HasKey("include")) {
		ParseInclude(patternDict, m);
		return true; // return true even if we don't find include
	}

	if (patternDict.HasKey("patterns")) {
		PListArray groupArray;
		if (!patternDict.GetArray("patterns", groupArray)) return false;

		group_matcher* gm = NewGroup();
		m = gm;
		return ParseGroup(groupArray, *gm);
	}

	// Empty pattern or comment
	return false;
}

bool TmSyntaxHandler::ParseInclude(const PListDict& patternDict, matcher*& m) {
	const char* inc = patternDict.GetString("include");

	if (inc) {
		if (inc[0] == '$') {
			// reference self
			m = m_currentSyntax->topmatcher;
		}
		else if (inc[0] == '#') {
			// reference matcher from repository
			m = GetFromRepo(inc+1);
		}
		else {
			m = GetMatcher(wxString(inc, wxConvUTF8));
		}
	}

	if (m) return true;
	else {
		// Notify user that the syntax could not be loaded
#ifdef __WXDEBUG__
		const wxString msg = _("Syntax ") + m_currentSyntax->name + _(" includes unknown syntax: ") + wxString(inc, wxConvUTF8);
		wxLogDebug(msg);
#endif //__WXDEBUG__

		//wxMessageBox(msg, _("Parse error"), wxICON_ERROR|wxOK);
		return false;
	}
}

bool TmSyntaxHandler::ParseMatch(match_matcher* mm, const PListDict& patternDict) {
	wxASSERT(mm);

	mm->SetName(patternDict.wxGetString("name"));
	mm->SetPattern(patternDict.GetString("match"));

	const char* disabled = patternDict.GetString("disabled");
	if (disabled && strcmp(disabled, "1") == 0) {
		mm->Disable();
	}

	PListDict capturesDict;
	if (patternDict.GetDict("captures", capturesDict)) {
		ParseCaptures(*mm, capturesDict);
	}

	return true;
}

bool TmSyntaxHandler::ParseSpan(span_matcher* sm, const PListDict& patternDict) {
	match_matcher* beginM = NULL;
	match_matcher* endM = NULL;

	sm->SetName(patternDict.wxGetString("name"));

	// Start matcher
	const char* begin = patternDict.GetString("begin");
	if (!begin) return false;  // need start matcher
	beginM = NewMatcher();
	beginM->SetPattern(begin);
	sm->SetStartMatcher(beginM);

	// End matcher
	const char* end = patternDict.GetString("end");
	if (!end) return false; // need end matcher

	const wxString endPattern(end, wxConvUTF8);
	sm->SetEndPattern(endPattern);

	// The end matcher is only used for captures
	endM = NewMatcher();
	sm->SetEndMatcher(endM);

	// Embedded patterns
	PListArray groupArray;
	if (patternDict.GetArray("patterns", groupArray) && !ParseGroup(groupArray, *sm)) return false;

	// Scope name for entire contents
	const char* contentName = patternDict.GetString("contentName");
	if (contentName) {
		sm->SetContentName(wxString(contentName, wxConvUTF8));
	}

	// Captures
	PListDict caps;
	if (patternDict.GetDict("captures", caps)) {
		// Same captures for start and end matcher
		ParseCaptures(*beginM, caps);
		ParseCaptures(*endM, caps);
	}
	else {
		// Parse individual captures
		if (patternDict.GetDict("beginCaptures", caps)) {
			ParseCaptures(*beginM, caps);
		}
		if (patternDict.GetDict("endCaptures", caps)) {
			ParseCaptures(*endM, caps);
		}
	}

	// Is disabled?
	const char* disabled = patternDict.GetString("disabled");
	if (disabled && strcmp(disabled, "1") == 0) {
		sm->Disable();
	}

	return true;
}

bool TmSyntaxHandler::ParseCaptures(match_matcher& m, const PListDict& captureDict) {

	for (unsigned int i = 0; i < captureDict.GetSize(); ++i) {
		const char* key = captureDict.GetKey(i);

		// Verify that the key is numeric
		unsigned int capkey;
		if (sscanf(key, "%u", &capkey) != 1) return false;

		PListDict capnameDict;
		if (!captureDict.GetDict(key, capnameDict)) return false;
		m.AddCapture(capkey, capnameDict.wxGetString("name"));
	}

	return true;
}

matcher* TmSyntaxHandler::GetFromRepo(const char* name) {
	if (!m_currentRepo) return NULL;

	// Check if we have already parsed the matcher
	map<const char*,matcher*,cp_less>::const_iterator p = m_currentParsedReps->find(name);
	if (p != m_currentParsedReps->end()) return p->second;

	// Check if it is currently being parsed
	map<const char*,matcher**,cp_less>::const_iterator p2 = m_repsInParsing->find(name);
	if (p2 != m_repsInParsing->end()) return *p2->second;

	PListDict patternDict;
	if (!m_currentRepo->GetDict(name, patternDict)) return NULL;

	matcher* m = NULL;
	(*m_repsInParsing)[name] = &m;

	ParsePattern(patternDict, m);

	if (m) (*m_currentParsedReps)[name] = m;
	m_repsInParsing->erase(name);
	return m;
}

void TmSyntaxHandler::GetSyntaxes(wxArrayString& nameArray) const {
	nameArray.Clear();
	for (vector<cxSyntaxInfo*>::const_iterator p = m_syntaxes.begin(); p != m_syntaxes.end(); ++p) {
		nameArray.Add((*p)->name);
	}
	nameArray.Sort();
}

matcher* TmSyntaxHandler::GetMatcher(const wxString& scope) {
	for (vector<cxSyntaxInfo*>::iterator p = m_syntaxes.begin(); p != m_syntaxes.end(); ++p) {
		if ((*p)->scope != scope) continue;

		const cxSyntaxInfo* si = InitSyntax(*(*p), false); // loaded but not necessarily initialized yet
		return si ? si->topmatcher : NULL;
	}

	// If we reached here, no syntaxes matched scope
	return NULL;
}

const cxSyntaxInfo* TmSyntaxHandler::GetSyntax(const wxString& syntaxName, const wxString& ext) {
	for (vector<cxSyntaxInfo*>::iterator p = m_syntaxes.begin(); p != m_syntaxes.end(); ++p) {
		if ((*p)->name == syntaxName) {
			// if this is a manual override, we save the ext-to-syntax association
			if (!ext.empty()) m_plistHandler.SetSyntaxAssoc(ext, (*p)->name);

			return InitSyntax(*(*p));
		}
	}

	// If we reached here, no syntaxes matched name
	return NULL;
}

const cxSyntaxInfo* TmSyntaxHandler::GetSyntax(const DocumentWrapper& document) {
	wxString filename;
	unsigned int firstline_end = 0;
	vector<char> line;
	cxLOCKDOC_READ(document)
		filename = doc.GetPropertyName();

		// Find end of first line
		firstline_end = doc.GetLine(0, line);
	cxENDLOCK

	// Check if we have a manual override
	// (Allow for extensions containing dots and extensions that cover entire filename)
	wxString ext = filename;
	while (!ext.empty()) {
		const wxString manSyn = m_plistHandler.GetSyntaxAssoc(ext);
		if (!manSyn.empty()) {
			const cxSyntaxInfo* si = GetSyntax(manSyn);
			if (si) return si;
		}
		ext = ext.AfterFirst(wxT('.'));
	}

	for (vector<cxSyntaxInfo*>::iterator p = m_syntaxes.begin(); p != m_syntaxes.end(); ++p) {
		// First check if filename matches wildcards
		// (Allow for extensions containing dots and extensions that cover entire filename)
		cxSyntaxInfo& si = *(*p);
		ext = filename;

		while (!ext.empty()) {
			for (unsigned int i = 0; i < si.filewild.GetCount(); ++i) {
				const wxString& filewild = si.filewild[i];
				if (ext == filewild) return InitSyntax(si);
			}
			ext = ext.AfterFirst(wxT('.'));
		}

		// Check if first line is matched by pattern
		if (firstline_end && !si.firstline.empty()) {
			// First we have to compile the pattern
			const char *error;
			int erroffset;
			pcre* re = pcre_compile(
				si.firstline.mb_str(wxConvUTF8),   // the pattern
				PCRE_UTF8,             // options
				&error,               // for error message
				&erroffset,           // for error offset
				NULL);                // use default character tables

			// Do the search
			if (re != NULL) {
				const int OVECCOUNT = 30;
				int ovector[OVECCOUNT];
				const int search_options = PCRE_NO_UTF8_CHECK;
				const int rc = pcre_exec(
					re,                   // the compiled pattern
					NULL,                // extra data - if we study the pattern
					&*line.begin(),         // the subject string
					firstline_end,        // the length of the subject
					0,                    // start at offset in the subject
					search_options,       // options
					ovector,              // output vector for substring information
					OVECCOUNT);           // number of elements in the output vector

				if (rc >= 0) return InitSyntax(si);
			}
			else {
				// Invalid firstline pattern
				wxASSERT(false);
			}
		}
	}

	// If we reached here, no syntaxes matched filename
	return NULL;
}

bool TmSyntaxHandler::GetThemeName(const wxFileName& path, wxString& name) {
	wxFFile tempfile(path.GetFullPath(), wxT("rb"));
	TiXmlDocument doc;
	if (!tempfile.IsOpened() || !doc.LoadFile(tempfile.fp())) return false;

	const TiXmlHandle docHandle(&doc);

	// TODO: Verify that this is a valid plist

	const TiXmlElement* child = docHandle.FirstChildElement("plist").FirstChildElement("dict").FirstChild().Element();
	if (!child) return false; // empty plist

	for(; child; child = child->NextSiblingElement() )
	{
		if (strcmp(child->Value(), "key") != 0) return false; // invalid dict
		const TiXmlElement* const value = child->NextSiblingElement();

		if (strcmp(child->GetText(), "name") == 0) {
			name = wxString(value->GetText(), wxConvUTF8);
			return true;
		}

		child = value;
	}

	return false;
}

bool TmSyntaxHandler::LoadTheme(const char* uuid) {
	PListDict theme;
	const int themeNdx = m_plistHandler.GetThemeFromUuid(uuid);
	if (themeNdx == -1 || !m_plistHandler.GetTheme(themeNdx, theme)) return false;

	vector<style*> styles;
	tmTheme themeSettings = m_defaultTheme;
	themeSettings.uuid = wxString(uuid, wxConvUTF8);

	// Get Theme name
	const char* name = theme.GetString("name");
	if (name) themeSettings.name = wxString(name, wxConvUTF8);

	// Parse Settings
	PListArray settings;
	if (theme.GetArray("settings", settings)) {
		for (unsigned int i = 0; i < settings.GetSize(); ++i) {
			PListDict set;
			if (!settings.GetDict(i, set)) goto error; // all settings are dicts

			if (set.HasKey("name")) {
				if (!ParseStyle(set, styles, themeSettings)) goto error;
			}
			else {
				PListDict general;
				if (!set.GetDict("settings", general)) goto error;
				if (!ParseSettings(general, themeSettings)) goto error;
			}
		}
	}

	// Seperate scope to avoid conflict with error label
	{
		// Parse the selectors
		sNode<style>* rootNode = new sNode<style>;
		for (vector<style*>::const_iterator p = styles.begin(); p != styles.end(); ++p) {
			if ((*p)->scope.empty()) continue;

			SelectorParser<style> parser((*p)->scope, *p);
			sNode<style>* n = parser.ParseExpr();
			if (n) rootNode->Merge(n);
			else {
				delete rootNode;
				goto error;
			}
		}

		// Set if we successfully loaded theme
		m_currentTheme = themeSettings;
		for (vector<style*>::iterator v = m_styles.begin(); v != m_styles.end(); ++v) {
			delete *v; // release old styles
		}
		m_styles = styles;
		delete m_styleNode;
		m_styleNode = rootNode;
	}
	return true;

error:
	// clean up
	for (vector<style*>::iterator v = styles.begin(); v != styles.end(); ++v) {
		delete *v;
	}
	return false;
}

bool TmSyntaxHandler::ParseSettings(const PListDict& dict, tmTheme& settings) {
	wxColour backgroundColor;
	wxColour foregroundColor;
	wxColour selectionColor;
	wxColour lineHighlightColor;
	wxColour searchHighlightColor;
	wxColour bracketHighlightColor;
	wxColour gutterColor;
	wxColour multiColor;

	const char* background = dict.GetString("background");
	if (background) backgroundColor = ParseColor(wxString(background, wxConvUTF8), wxNullColour);

	const char* foreground = dict.GetString("foreground");
	if (foreground) foregroundColor = ParseColor(wxString(foreground, wxConvUTF8), backgroundColor);

	const char* selection = dict.GetString("selection");
	if (selection) selectionColor = ParseColor(wxString(selection, wxConvUTF8), backgroundColor);

	const char* lineHighlight = dict.GetString("lineHighlight");
	if (lineHighlight) lineHighlightColor = ParseColor(wxString(lineHighlight, wxConvUTF8), backgroundColor);

	const char* searchHighlight = dict.GetString("searchHighlight");
	if (searchHighlight) searchHighlightColor = ParseColor(wxString(searchHighlight, wxConvUTF8), backgroundColor);

	const char* bracketHighlight = dict.GetString("bracketHighlight");
	if (bracketHighlight) bracketHighlightColor = ParseColor(wxString(bracketHighlight, wxConvUTF8), backgroundColor);

	const char* gutter = dict.GetString("gutter");
	if (gutter) gutterColor = ParseColor(wxString(gutter, wxConvUTF8), backgroundColor);

	const char* multi = dict.GetString("multiEditHighlight");
	if (multi) multiColor = ParseColor(wxString(multi, wxConvUTF8), multiColor);

	if (backgroundColor.Ok()) settings.backgroundColor = backgroundColor;
	if (foregroundColor.Ok()) settings.foregroundColor = foregroundColor;
	if (selectionColor.Ok()) settings.selectionColor = selectionColor;
	if (lineHighlightColor.Ok()) settings.lineHighlightColor = lineHighlightColor;

	if (searchHighlightColor.Ok()) settings.searchHighlightColor = searchHighlightColor;
	else settings.searchHighlightColor = wxColour(wxT("Yellow"));  // default

	if (bracketHighlightColor.Ok()) settings.bracketHighlightColor = bracketHighlightColor;
	else settings.bracketHighlightColor = wxColour(wxT("Yellow"));  // default

	if (gutterColor.Ok()) settings.gutterColor = gutterColor;
	else settings.gutterColor = wxColour(192, 192, 255);  // default for gutter is Pastel purple

	if (multiColor.Ok()) settings.shadowColor = multiColor;
	else settings.shadowColor = wxColour(225, 225, 225);  // default for gutter is Grey

	return true;
}

bool TmSyntaxHandler::ParseStyle(const PListDict& dict, vector<style*>& styles, tmTheme& settings) {
	style* st = new style;
	st->fontflags = wxFONTFLAG_DEFAULT;

	const char* name = dict.GetString("name");
	if (name) st->name = wxString(name, wxConvUTF8);

	const char* scope = dict.GetString("scope");
	if (scope) st->scope = wxString(scope, wxConvUTF8);

	PListDict set;
	if (dict.GetDict("settings", set)) {
		const char* foreground = set.GetString("foreground");
		if (foreground) st->foregroundcolor = ParseColor(wxString(foreground, wxConvUTF8), settings.backgroundColor);

		const char* background = set.GetString("background");
		if (background) st->backgroundcolor = ParseColor(wxString(background, wxConvUTF8), settings.backgroundColor);

		const char* fontStyle = set.GetString("fontStyle");
		if (fontStyle) {
			if (strstr(fontStyle, "italic") != NULL) st->fontflags |= wxFONTFLAG_ITALIC;
			if (strstr(fontStyle, "bold") != NULL) st->fontflags |= wxFONTFLAG_BOLD;
			if (strstr(fontStyle, "underline") != NULL) st->fontflags |= wxFONTFLAG_UNDERLINED;
		}
	}

	styles.push_back(st);

	return true;
}

wxColour TmSyntaxHandler::ParseColor(const wxString& color_hex, const wxColour& bgColor) {
	if (color_hex.size() < 7) return wxNullColour;
	if (color_hex[0] != wxT('#')) return wxNullColour;

	unsigned long red = 0;
	unsigned long green = 0;
	unsigned long blue = 0;
	unsigned long alpha = wxALPHA_OPAQUE;
	if (!color_hex.Mid(1, 2).ToULong(&red, 16)) return wxNullColour;
	if (!color_hex.Mid(3, 2).ToULong(&green, 16)) return wxNullColour;
	if (!color_hex.Mid(5, 2).ToULong(&blue, 16)) return wxNullColour;

	// Handle alpha
	if (color_hex.size() == 9 && bgColor.Ok()) {
		if (!color_hex.Mid(7, 2).ToULong(&alpha, 16)) return wxNullColour;

		// Manually apply alpha transparency
		// TODO: consider using wxGCDC for rendering so it can do nativa alpha
		red = (red * alpha + bgColor.Red() * (255 - alpha)) >> 8;
		green = (green * alpha + bgColor.Green() * (255 - alpha)) >> 8;
		blue = (blue * alpha + bgColor.Blue() * (255 - alpha))  >> 8;
	}

	return wxColour(red, green, blue); //, alpha);
}

match_matcher* TmSyntaxHandler::NewMatcher() {
	match_matcher* m = new match_matcher();
	m_currentMatchers->push_back(m);
	return m;
}

span_matcher* TmSyntaxHandler::NewSpan() {
	span_matcher* m = new span_matcher();
	m_currentMatchers->push_back(m);
	return m;
}

group_matcher* TmSyntaxHandler::NewGroup() {
	group_matcher* m = new group_matcher();
	m_currentMatchers->push_back(m);
	return m;
}

bool TmSyntaxHandler::ParseSnippet(const tmBundle& bundle, unsigned int snippetId) {
	const PListDict snippetDict = m_plistHandler.Get(BUNDLE_SNIPPET, bundle.bundleRef, snippetId);

	const char* name = snippetDict.GetString("name");
	const char* uuid = snippetDict.GetString("uuid");
	if (!name || !uuid) return false;

	// Create the new snippet
	tmSnippet* snip = new tmSnippet;

	snip->bundle = &bundle;
	snip->plistRef = snippetId;
	snip->name = wxString(name, wxConvUTF8);
	snip->uuid = wxString(uuid, wxConvUTF8);
	snip->scope = snippetDict.wxGetString("scope");
	snip->trigger = snippetDict.wxGetString("tabTrigger");

	// The actual snippet content does not get added before GetActionContent() get called

	// Run Environment
	const char* runEnv = snippetDict.GetString("runEnvironment");
	if (runEnv && strcmp(runEnv, "windows") == 0) {
		snip->isUnix = false;
	}

	// Key binding
	const char* binding = snippetDict.GetString("keyEquivalent");
	if (binding) {
		snip->key = tmKey(wxString(binding, wxConvUTF8));
	}

	// Add to action tree
	SelectorParser<tmAction> parser(snip->scope, snip);
	sNode<tmAction>* n = parser.ParseExpr();
	if (n) m_actionNode.Merge(n);

	// DELETE: If there is a tabTrigger we have to parse the selector
	if (!snip->trigger.empty()) {
		SelectorParser<tmAction> parser(snip->scope, snip);
		sNode<tmAction>* n = parser.ParseExpr();
		if (n) {
			sNode<tmAction>* rootNode = m_actionTriggers[snip->trigger];
			if (rootNode) rootNode->Merge(n);
			else m_actionTriggers[snip->trigger] = n;
		}
		else {
			delete snip;
			return false;
		}
	}

	// Add to snippet map
	m_actions[snip->uuid] = snip;

	return true;
}

bool TmSyntaxHandler::ParseCommand(const tmBundle& bundle, unsigned int commandId) {
	const PListDict commandDict = m_plistHandler.Get(BUNDLE_COMMAND, bundle.bundleRef, commandId);

	const char* name = commandDict.GetString("name");
	const char* uuid = commandDict.GetString("uuid");
	if (!name || !uuid) return false;

	// Create the command
	tmCommand* cmd = new tmCommand;

	cmd->bundle = &bundle;
	cmd->plistRef = commandId;
	cmd->name = wxString(name, wxConvUTF8);
	cmd->uuid = wxString(uuid, wxConvUTF8);
	cmd->scope = commandDict.wxGetString("scope");
	cmd->trigger = commandDict.wxGetString("tabTrigger");

	// The actual command content does not get added before GetActionContent() get called

	// Run Environment
	const char* runEnv = commandDict.GetString("runEnvironment");
	if (runEnv && strcmp(runEnv, "windows") == 0) {
		cmd->isUnix = false;
	}

	// Key binding
	const char* binding = commandDict.GetString("keyEquivalent");
	if (binding) {
		cmd->key = tmKey(wxString(binding, wxConvUTF8));
	}

	// Action to do before command
	cmd->save = tmCommand::csNONE;
	const char* beforeCommand = commandDict.GetString("beforeRunningCommand");
	if (beforeCommand) {
		if (strcmp(beforeCommand, "nop") == 0) cmd->save = tmCommand::csNONE;
		else if (strcmp(beforeCommand, "saveActiveFile") == 0) cmd->save = tmCommand::csDOC;
		else if (strcmp(beforeCommand, "saveModifiedFiles") == 0) cmd->save = tmCommand::csALL;
		else wxASSERT(false); // unknown action
	}

	// Input format
	cmd->inputXml = false;
	const char* inputFormat = commandDict.GetString("keyEquivalent");
	if (inputFormat) {
		if (strcmp(inputFormat, "xml") == 0) cmd->inputXml = true;
	}

	// Input source
	cmd->input = tmCommand::ciNONE;
	const char* inputSource = commandDict.GetString("input");
	if (inputSource) {
		if (strcmp(inputSource, "none") == 0) cmd->input = tmCommand::ciNONE;
		else if (strcmp(inputSource, "selection") == 0) cmd->input = tmCommand::ciSEL;
		else if (strcmp(inputSource, "line") == 0) cmd->input = tmCommand::ciLINE;
		else if (strcmp(inputSource, "character") == 0) cmd->input = tmCommand::ciCHAR;
		else if (strcmp(inputSource, "document") == 0) cmd->input = tmCommand::ciDOC;
		else if (strcmp(inputSource, "word") == 0) cmd->input = tmCommand::ciWORD;
		else if (strcmp(inputSource, "scope") == 0) cmd->input = tmCommand::ciSCOPE;
		else wxASSERT(false);
	}

	// Fallback input source
	cmd->fallbackInput = tmCommand::ciDOC; // default is full document
	const char* fallbackInput = commandDict.GetString("fallbackInput");
	if (fallbackInput) {
		if (strcmp(fallbackInput, "none") == 0) cmd->fallbackInput = tmCommand::ciNONE;
		else if (strcmp(fallbackInput, "selection") == 0) cmd->fallbackInput = tmCommand::ciSEL;
		else if (strcmp(fallbackInput, "line") == 0) cmd->fallbackInput = tmCommand::ciLINE;
		else if (strcmp(fallbackInput, "character") == 0) cmd->fallbackInput = tmCommand::ciCHAR;
		else if (strcmp(fallbackInput, "document") == 0) cmd->fallbackInput = tmCommand::ciDOC;
		else if (strcmp(fallbackInput, "word") == 0) cmd->fallbackInput = tmCommand::ciWORD;
		else if (strcmp(fallbackInput, "scope") == 0) cmd->fallbackInput = tmCommand::ciSCOPE;
		else wxASSERT(false);
	}

	// Output destination
	cmd->output = tmCommand::coNONE;
	const char* outputDest = commandDict.GetString("output");
	if (outputDest) {
		if (strcmp(outputDest, "discard") == 0) cmd->output = tmCommand::coNONE;
		else if (strcmp(outputDest, "replaceSelectedText") == 0) cmd->output = tmCommand::coSEL;
		else if (strcmp(outputDest, "showAsHTML") == 0) cmd->output = tmCommand::coHTML;
		else if (strcmp(outputDest, "afterSelectedText") == 0) cmd->output = tmCommand::coINSERT;
		else if (strcmp(outputDest, "insertAsSnippet") == 0) cmd->output = tmCommand::coSNIPPET;
		else if (strcmp(outputDest, "showAsTooltip") == 0) cmd->output = tmCommand::coTOOLTIP;
		else if (strcmp(outputDest, "openAsNewDocument") == 0) cmd->output = tmCommand::coNEWDOC;
		else if (strcmp(outputDest, "replaceDocument") == 0) cmd->output = tmCommand::coREPLACEDOC;
		else wxASSERT(false);
	}

	// Add to action tree
	SelectorParser<tmAction> parser(cmd->scope, cmd);
	sNode<tmAction>* n = parser.ParseExpr();
	if (n) m_actionNode.Merge(n);

	// If there is a tabTrigger we have to parse the selector
	if (!cmd->trigger.empty()) {
		SelectorParser<tmAction> parser(cmd->scope, cmd);
		sNode<tmAction>* n = parser.ParseExpr();
		if (n) {
			sNode<tmAction>* rootNode = m_actionTriggers[cmd->trigger];
			if (rootNode) rootNode->Merge(n);
			else m_actionTriggers[cmd->trigger] = n;
		}
		else {
			delete cmd;
			return false;
		}
	}

	// Add to snippet map
	m_actions[cmd->uuid] = cmd;

	return true;
}

bool TmSyntaxHandler::ParseDragCommand(const tmBundle& bundle, unsigned int commandId) {
	const PListDict dragDict = m_plistHandler.Get(BUNDLE_DRAGCMD, bundle.bundleRef, commandId);

	const char* name = dragDict.GetString("name");
	const char* uuid = dragDict.GetString("uuid");
	if (!name || !uuid) return false;

	// Create the command
	tmDragCommand* cmd = new tmDragCommand;

	cmd->bundle = &bundle;
	cmd->plistRef = commandId;
	cmd->name = wxString(name, wxConvUTF8);
	cmd->uuid = wxString(uuid, wxConvUTF8);
	cmd->scope = dragDict.wxGetString("scope");
	// The actual command content does not get added before GetActionContent() get called

	// Run Environment
	const char* runEnv = dragDict.GetString("runEnvironment");
	if (runEnv && strcmp(runEnv, "windows") == 0) {
		cmd->isUnix = false;
	}

	// DragCommands always output to snippet
	cmd->output = tmCommand::coSNIPPET;

	// Get the list of file extensions
	PListArray extArray;
	if (dragDict.GetArray("draggedFileExtensions", extArray)) {
		for (unsigned int i = 0; i < extArray.GetSize(); ++i) {
			cmd->extArray.Add(extArray.wxGetString(i));
		}
	}

	// Add to drag commands tree
	SelectorParser<tmDragCommand> parser(cmd->scope, cmd);
	sNode<tmDragCommand>* n = parser.ParseExpr();
	if (n) m_dragNode.Merge(n);

	// Add to action map
	m_actions[cmd->uuid] = cmd;

	return true;
}

bool TmSyntaxHandler::ParsePreferences(const PListDict& prefDict, tmBundle* bundle) {
	const char* scopeStr = prefDict.GetString("scope");
	const wxString scope = scopeStr ? wxString(scopeStr, wxConvUTF8) : *wxEmptyString;

	PListDict settingsDict;
	if (!prefDict.GetDict("settings", settingsDict)) return false;

	// Look for indent patterns
	const char* increaseIndentPattern = settingsDict.GetString("increaseIndentPattern");
	const char* decreaseIndentPattern = settingsDict.GetString("decreaseIndentPattern");
	const char* indentNextLinePattern = settingsDict.GetString("indentNextLinePattern");
	const char* unIndentedLinePattern = settingsDict.GetString("unIndentedLinePattern");
	if (increaseIndentPattern || decreaseIndentPattern ||
		indentNextLinePattern || unIndentedLinePattern) {

		tmPrefs* prefs = new tmPrefs();
		prefs->increaseIndentPattern = wxString(increaseIndentPattern, wxConvUTF8);
		prefs->decreaseIndentPattern = wxString(decreaseIndentPattern, wxConvUTF8);
		prefs->indentNextLinePattern = wxString(indentNextLinePattern, wxConvUTF8);
		prefs->unIndentedLinePattern = wxString(unIndentedLinePattern, wxConvUTF8);

		// Add to action tree
		SelectorParser<tmPrefs> parser(scope, prefs);
		sNode<tmPrefs>* n = parser.ParseExpr();
		if (n) m_prefsNode.Merge(n);

		// Add to prefs list (for later cleanup)
		m_prefs.push_back(prefs);
	}

	// Look for shell variables
	PListArray shellVariables;
	if (settingsDict.GetArray("shellVariables", shellVariables)) {
		map<wxString, wxString>* vars = new map<wxString, wxString>;

		for (unsigned int i = 0; i < shellVariables.GetSize(); ++i) {
			PListDict shellVar;
			if (shellVariables.GetDict(i, shellVar)) {
				const char* name = shellVar.GetString("name");
				const char* value = shellVar.GetString("value");
				if (name && value) {
					(*vars)[wxString(name, wxConvUTF8)] = wxString(value, wxConvUTF8);
				}
			}
		}

		if (vars->empty()) delete vars;
		else {

			// Add to action tree
			SelectorParser<map<wxString, wxString> > parser(scope, vars);
			sNode<map<wxString, wxString> >* n = parser.ParseExpr();
			if (n) m_shellVarNode.Merge(n);

			// Add to prefs list (for later cleanup)
			m_shellVarPrefs.push_back(vars);
		}
	}

	//  Look for Smart Typing Pairs
	PListArray smartTypingPairs;
	if (settingsDict.GetArray("smartTypingPairs", smartTypingPairs)) {
		map<wxString, wxString>* pairs = new map<wxString, wxString>;

		for (unsigned int i = 0; i < smartTypingPairs.GetSize(); ++i) {
			PListArray smartPair;
			if (smartTypingPairs.GetArray(i, smartPair) && smartPair.GetSize() >= 2) {
				const char* first = smartPair.GetString(0);
				const char* second = smartPair.GetString(1);
				if (first && second) {
					(*pairs)[wxString(first, wxConvUTF8)] = wxString(second, wxConvUTF8);
				}
			}
		}

		if (pairs->empty()) delete pairs;
		else {

			// Add to action tree
			SelectorParser<map<wxString, wxString> > parser(scope, pairs);
			sNode<map<wxString, wxString> >* n = parser.ParseExpr();
			if (n) m_smartPairsNode.Merge(n);

			// Add to prefs list (for later cleanup)
			m_smartTypingPairs.push_back(pairs);
		}
	}

	// Completion Lists
	PListArray completionsArray;
	if (settingsDict.GetArray("completions", completionsArray)) {
		vector<wxString>* completions = new vector<wxString>;
		completions->reserve(completionsArray.GetSize());

		for (unsigned int i = 0; i < completionsArray.GetSize(); ++i) {
			completions->push_back(completionsArray.wxGetString(i));
		}

		// Add to action tree
		SelectorParser<vector<wxString> > parser(scope, completions);
		sNode<vector<wxString> >* n = parser.ParseExpr();
		if (n) m_completionsNode.Merge(n);

		// Add to cleanup list
		m_completions.push_back(completions);
	}

	// Completion Commands
	const char* completionCommand = settingsDict.GetString("completionCommand");
	if (completionCommand) {
		tmCompletionCmd* ccmd = new tmCompletionCmd;
		ccmd->cmd.assign(completionCommand, completionCommand + strlen(completionCommand));
		ccmd->bundle = bundle;

		// Add to action tree
		SelectorParser<tmCompletionCmd> parser(scope, ccmd);
		sNode<tmCompletionCmd>* n = parser.ParseExpr();
		if (n) m_completionCmdNode.Merge(n);

		// Add to cleanup list
		m_completionCmds.push_back(ccmd);
	}

	// Default completion
	int disVal;
	if (settingsDict.GetInteger("disableDefaultCompletion", disVal) && disVal == 1) {
		// Add to action tree
		SelectorParser<void> parser(scope, (void*)1);
		sNode<void>* n = parser.ParseExpr();
		if (n) m_disableCompletionNode.Merge(n);
	}

	// Symbol lists
	if (settingsDict.HasKey("showInSymbolList")) {
		// we check if it was set to "1", but the bundles are not consistent
		// with whether it should be an integer or a string).
		bool showInSymbolList = false;
		int doShow;
		if (settingsDict.GetInteger("showInSymbolList", doShow)) {
			if (doShow == 1) showInSymbolList = true;
		}
		else if (settingsDict.wxGetString("showInSymbolList") == wxT("1")) showInSymbolList = true;

		if (showInSymbolList) {
			const char* transform = settingsDict.GetString("symbolTransformation");
			wxString* symTrans = (transform) ? new wxString(transform, wxConvUTF8) : new wxString();
			wxASSERT(symTrans->size() >= 0); // check that string is valid

			// Add to action tree
			SelectorParser<wxString> parser(scope, symTrans);
			sNode<wxString>* n = parser.ParseExpr();
			if (n) m_symbolNode.Merge(n);

			// Add to cleanup list
			m_symbolTransforms.push_back(symTrans);
		}
	}

	return true;
}

bool TmSyntaxHandler::ParseInfo(const PListDict& infoDict, tmBundle& bundle) {
	bundle.name = infoDict.wxGetString("name");
	bundle.uuid = infoDict.wxGetString("uuid");

	// Check if there is a menu
	wxMenu* mainMenu = NULL;
	PListDict menuDict;
	if (infoDict.GetDict("mainMenu", menuDict)) {
		PListArray itemsArray;
		if (!menuDict.GetArray("items", itemsArray)) return true;
		if (itemsArray.GetSize() == 0) return true; // don't add menu if no items

		PListDict submenuDict;
		menuDict.GetDict("submenus", submenuDict);

		// Parse the menu
		mainMenu = ParseMenu(itemsArray, submenuDict);
	}
	else {
		// If there is no main menu structure, we use the ordering
		PListArray itemsArray;
		if (!infoDict.GetArray("ordering", itemsArray)) return true;

		// Parse the menu
		mainMenu = ParseMenu(itemsArray, PListDict());
	}

	// Add to Bundle menu
	if (mainMenu && !bundle.name.empty()) {
		// We need to escape any ambersands in the label
		// (otherwise they will be interpreted as shortcuts)
		wxString label = bundle.name;
		label.Replace(wxT("&"), wxT("&&"));
		
		m_bundleMenu->Append(m_nextMenuID, label, mainMenu, bundle.name);
		++m_nextMenuID;
	}
	else delete mainMenu;

	return true;
}

wxMenu* TmSyntaxHandler::ParseMenu(const PListArray& itemsArray, const PListDict& submenuDict) {
	const bool hasSubmenus = submenuDict.IsOk();

	wxMenu* menu = new wxMenu();
	PListDict subDict;

	for (unsigned int i = 0; i < itemsArray.GetSize(); ++i) {
		const char* item = itemsArray.GetString(i);

		if (item[0] == '-') menu->AppendSeparator();
		else {
			// Look for submenu
			if (hasSubmenus && submenuDict.GetDict(item, subDict)) {
				const wxString name = subDict.wxGetString("name");

				PListArray subArray;
				wxMenu* sMenu = NULL;
				if (subDict.GetArray("items", subArray)) {
					sMenu = ParseMenu(subArray, submenuDict);
				}

				if (sMenu && !name.empty()) {
					menu->Append(m_nextMenuID, name, sMenu, name);
					++m_nextMenuID;
				}
				else delete sMenu;
				continue;
			}

			const wxString uuid(item, wxConvUTF8);

			// Look for action
			map<const wxString, tmAction*>::iterator s = m_actions.find(uuid);
			if (s != m_actions.end()) {
				tmAction* action = s->second;
				wxASSERT(action);

                BundleMenuItem *item = new BundleMenuItem(menu, m_nextMenuID, *action);
				menu->Append(item);
                item->AfterInsert();
				m_menuActions[m_nextMenuID] = action->uuid;
				++m_nextMenuID;
			}
		}

		wxASSERT(m_nextMenuID >= 9000 && m_nextMenuID < 12000);
	}

	return menu;
}

// ---- cxFoldRule ------------------------------------------------

TmSyntaxHandler::cxFoldRule::cxFoldRule(unsigned int id, const char* startMarker, const char* endMarker)
: ruleId(id), foldingStartMarker(startMarker, startMarker + strlen(startMarker)+1),
foldingEndMarker(endMarker, endMarker + strlen(endMarker)+1) {
}

// ---- SelectorParser ------------------------------------------------

template<class T> SelectorParser<T>::SelectorParser(const wxString& selector, const T* target)
: m_target(target), m_selector(selector), m_len(selector.size()), m_pos(0) {
	 m_currentNode = m_scopeNode = m_rootNode = new sNode<T>;
};


template<class T> sNode<T>* SelectorParser<T>::ParseExpr() {
	GetNextToken(); // Sets m_currentToken

	// Empty selector is ok. It matches everything
	if (m_currentToken == TOKEN_EOF) {
		m_rootNode->targets = new vector<const T*>;
		m_rootNode->targets->push_back(m_target);
		return m_rootNode;
	}

	while(1) {
		switch (m_currentToken) {
		case TOKEN_WORD:
			{
				sNode<T>* parentNode = NULL;
				sNode<T>* targetNode = NULL;

				do {
					if (!ParseScope()) {
						delete m_rootNode;
						return NULL; // TODO: Cleanup
					}

					// Target is the node that will be at the bottom of the
					// ancestor chain
					if (!targetNode) {
						targetNode = m_currentNode;
					}

					if (parentNode) {
						m_currentNode->AddAncestor(parentNode);
					}
					parentNode = m_scopeNode;
				}
				while (m_currentToken == TOKEN_WORD);

				// Add the style
				if (!targetNode->targets) {
					targetNode->targets = new vector<const T*>;
				}
				targetNode->targets->push_back(m_target);

				m_rootNode->AddOrNode(m_scopeNode);
			}
			break;
		case TOKEN_OR:
			GetNextToken();
			break;
		case TOKEN_EOF:
			// end of selector
			return m_rootNode;
		case TOKEN_PARAN_START:
		case TOKEN_PARAN_END:
			// ignore paranthesis
			GetNextToken();
			break;
		case TOKEN_MINUS:
			// ignore everthing after minus
			return m_rootNode;
		default:
			// Unexpected
			return NULL;
		}
	}

	return m_rootNode;
}

template<class T> typename SelectorParser<T>::selToken SelectorParser<T>::GetNextToken() {
	// ignore whitespace
	while (m_pos < m_len && m_selector[m_pos] == wxT(' ')) ++m_pos;

	if (m_pos < m_len) {
		wxChar c = m_selector[m_pos];
		if (wxIsalpha(c)) {
		    unsigned int i = m_pos+1;
			while (i < m_len) {
				c = m_selector[i];
				if (wxIsspace(c) || c == wxT('.') || c == wxT(',') || c == wxT('|') || c == wxT(')')) break;
                ++i;
			}
			m_tokenValue = m_selector.Mid(m_pos, i - m_pos);
			m_pos = i;

			m_currentToken = TOKEN_WORD;
			return TOKEN_WORD;
		}
		else if (c == wxT('.')) {
			++m_pos;

			m_currentToken = TOKEN_DOT;
			return TOKEN_DOT;
		}
		else if (c == wxT(',') || c == wxT('|')) {
			++m_pos;

			m_currentToken = TOKEN_OR;
			return TOKEN_OR;
		}
		else if (c == wxT('(')) {
			++m_pos;
			m_currentToken = TOKEN_PARAN_START;
			return TOKEN_PARAN_START;
		}
		else if (c == wxT(')')) {
			++m_pos;
			m_currentToken = TOKEN_PARAN_END;
			return TOKEN_PARAN_END;
		}
		else if (c == wxT('-')) {
			// minus is also legal in words so this is only if it is in front
			++m_pos;
			m_currentToken = TOKEN_MINUS;
			return TOKEN_MINUS;
		}
	}

	m_currentToken = TOKEN_EOF;
	return TOKEN_EOF;
}

template<class T> bool SelectorParser<T>::ParseScope() {
	// Parse first word (required)
	wxASSERT(m_currentToken == TOKEN_WORD);
	m_currentNode = m_scopeNode = new sNode<T>(m_tokenValue);

	while(GetNextToken() == TOKEN_DOT) {
		if (GetNextToken() == TOKEN_WORD) {
			// Add the new word to current scope
			sNode<T>* newNode = new sNode<T>(m_tokenValue);
			if (!m_currentNode->postfix) {
				m_currentNode->postfix = new map<wxString,sNode<T>*>;
			}
			(*m_currentNode->postfix)[m_tokenValue] = newNode;
			m_currentNode = newNode;
		}
		else {
			// Invalid scope
			return false;
		}
	}

	return true;
}

// ---- sNode ------------------------------------------------

template<class T> sNode<T>::sNode()
: word(wxEmptyString), postfix(NULL), orNodes(NULL), ancestors(NULL), targets(NULL) {};

template<class T> sNode<T>::sNode(const wxString& word)
: word(word), postfix(NULL), orNodes(NULL), ancestors(NULL), targets(NULL) {};

template<class T> sNode<T>::~sNode() {
	clear();
}

template<class T> void sNode<T>::clear() {
	// Delete all postfix nodes
	if (postfix) {
		for (typename NodeMap::iterator p = postfix->begin(); p != postfix->end(); ++p) {
			delete p->second;
		}
		delete postfix;
		postfix = NULL;
	}

	// Delete all "or" nodes
	if (orNodes) {
		for (typename NodeMap::iterator p = orNodes->begin(); p != orNodes->end(); ++p) {
			delete p->second;
		}
		delete orNodes;
		orNodes = NULL;
	}

	// Delete all ancestor nodes
	if (ancestors) {
		for (typename NodeMap::iterator p = ancestors->begin(); p != ancestors->end(); ++p) {
			delete p->second;
		}
		delete ancestors;
		ancestors = NULL;
	}

	// Delete targets vector (no need to delete contained pointers)
	delete targets;
	targets = NULL;

	word.clear();
}

template<class T> void sNode<T>::Tokenize(const wxString& scope, wxArrayString& words) const {
	const size_t len = scope.size();
	size_t wordstart = 0;
	size_t i = 0;

	while (i < len) {
		if (scope[i] == '.') {
			words.Add(scope.substr(wordstart, i - wordstart));
			wordstart = ++i;
		}
		else ++i;
	}

	if (wordstart < len) {
		words.Add(scope.substr(wordstart));
	}
}

static inline wxString NextWord(const wxString& scope, size_t& pos) {
	const size_t len = scope.size();
	const size_t wordstart = pos;
	size_t i = pos;

	while (i < len && scope[i] != '.') ++i;

	pos = i+1;
	return scope.substr(wordstart, i-wordstart);
}

template<class T> const vector<const T*>* sNode<T>::GetMatch(const deque<const wxString*>& scopes) const {
	if (scopes.empty()) return targets;

	if (orNodes) {
		// We start at the bottom of the scope and we keep going
		// a level up until we hit a match
		size_t ndx = scopes.size();
		while (ndx > 0) {
			--ndx;

			// Parse the current scope
			const wxString& scope = *scopes[ndx];
			size_t scopePos = 0;
			const wxString w = NextWord(scope, scopePos);

			typename NodeMap::const_iterator p = orNodes->find(w);
			if (p != orNodes->end()) {
				const vector<const T*>* res = p->second->Match(scope, scopePos, scopes, ndx);
				if (res) return res;
			}

			// If no match, go one level up in scope and try again
		}
	}


	return targets;
}


template<class T> void sNode<T>::GetMatches(const deque<const wxString*>& scopes, vector<const T*>& result) const {
	if (scopes.empty()) {
		if (targets) result = *targets;
		return;
	}

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
				p->second->Matches(words, 0, scopes, ndx, result);

				// TODO: Add a ref to the triggering scope level
			}
		}
	}

	if (targets) result.insert(result.end(), targets->begin(), targets->end());
}
/*
template<class T> template<class P> void sNode<T>::GetMatches(const deque<const wxString*>& scopes, vector<const T*>& result, P pred) const {
	if (!scopes.empty()) {
		if (orNodes) {
			// We start at the bottom of the scope and we keep going a level up
			for (int ndx = scopes.size()-1; ndx >= 0; --ndx) {
				// Parse the current scope
				wxArrayString words;
				Tokenize(*scopes[ndx], words);
				const wxString& w = words[0];

				map<wxString,sNode*>::const_iterator p = orNodes->find(w);
				if (p != orNodes->end()) {
					if (p->second->MatchPredicate(words, 0, scopes, ndx, result, pred)) return;

					// TODO: Add a ref to the triggering scope level
				}
			}
		}
	}

	// match (but there may be no targes here)
	if (targets) {
		vector<const T*>::const_iterator p = targets->begin();
		while (p != targets->end()) {
			const T* t = *p;

			if (pred(t)) {
				result.insert(result.end(), *p);
			}
		}
	}
}
*/
template<class T> const vector<const T*>* sNode<T>::Match(const wxString& scope, size_t scopePos, const deque<const wxString*>& scopes, size_t level) const {
	if (postfix && scopePos < scope.size()) {
		const wxString w = NextWord(scope, scopePos);
		typename NodeMap::const_iterator p = postfix->find(w);
		if (p != postfix->end()) {
			const vector<const T*>* s = p->second->Match(scope, scopePos, scopes, level);
			if (s) return s;
		}
	}

	// If we didn't find a match look for ancestors
	if (level && ancestors) {
		// Parse the ancestor scope
		const size_t ndx = level - 1;
		const wxString& scope2 = *scopes[ndx];
		scopePos = 0;
		const wxString w = NextWord(scope2, scopePos);

		typename NodeMap::const_iterator p = ancestors->find(w);
		if (p != ancestors->end()) {
			const vector<const T*>* s = p->second->Match(scope2, scopePos, scopes, ndx);
			if (s) return s;
		}
	}

	return targets; // match (but there may be no styles here)
}

template<class T> void sNode<T>::Matches(const wxArrayString& words, unsigned int pos, const deque<const wxString*>& scopes, size_t level, vector<const T*>& result) const {
	wxASSERT(word == words[pos]);

	const unsigned int nextPos = pos+1;
	if (postfix && nextPos < words.Count()) {
		typename NodeMap::const_iterator p = postfix->find(words[nextPos]);
		if (p != postfix->end()) {
			p->second->Matches(words, nextPos, scopes, level, result);
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
			p->second->Matches(words2, 0, scopes, ndx, result);
		}
	}

	// match (but there may be no targes here)
	if (targets) result.insert(result.end(), targets->begin(), targets->end());
}
/*
template<class T> bool sNode<T>::MatchPredicate(const wxArrayString& words, unsigned int pos, const deque<const wxString*>& scopes, unsigned int level, vector<const T*>& result, const unary_function<const T*, bool>& pred) const {
	wxASSERT(word == words[pos]);

	const unsigned int nextPos = pos+1;
	if (postfix && nextPos < words.Count()) {
		map<wxString,sNode*>::const_iterator p = postfix->find(words[nextPos]);
		if (p != postfix->end()) {
			if (p->second->MatchPredicate(words, nextPos, scopes, level, result, pred)) return true;
		}
	}

	// If we didn't find a match look for ancestors
	if (level && ancestors) {
		// Parse the ancestor scope
		const unsigned int ndx = level - 1;
		wxArrayString words2;
		Tokenize(*scopes[ndx], words2);
		const wxString& w = words2[0];

		map<wxString,sNode*>::const_iterator p = ancestors->find(w);
		if (p != ancestors->end()) {
			if (p->second->MatchPredicate(words2, 0, scopes, ndx, result, pred)) return true;
		}
	}

	// match (but there may be no targes here)
	bool isFound = false;
	if (targets) {
		vector<const T*>::const_iterator p = targets->begin();
		while (p != targets->end()) {
			if (pred(*p)) {
				result.insert(result.end(), p);
				isFound = true;
			}
		}
	}

	return isFound;
}
*/
template<class T> void sNode<T>::AddOrNode(sNode<T>* n) {
	if (orNodes) {
		typename NodeMap::iterator p = orNodes->find(n->word);
		if (p == orNodes->end()) {
			(*orNodes)[n->word] = n;
		}
		else {
			p->second->Merge(n);
		}
	}
	else {
		orNodes = new map<wxString,sNode*>;
		(*orNodes)[n->word] = n;
	}
}

template<class T> void sNode<T>::AddAncestor(sNode<T>* n) {
	if (ancestors) {
		typename NodeMap::iterator p = ancestors->find(n->word);
		if (p == ancestors->end()) {
			(*ancestors)[n->word] = n;
		}
		else {
			p->second->Merge(n);
		}
	}
	else {
		ancestors = new map<wxString,sNode*>;
		(*ancestors)[n->word] = n;
	}
}

template<class T> void sNode<T>::Merge(sNode<T>* n) {
	wxASSERT(word == n->word);

	// Merge all postfix nodes
	if (postfix) {
		if (n->postfix) {
			for (typename NodeMap::iterator p = n->postfix->begin(); p != n->postfix->end(); ++p) {
				const wxString& w = p->second->word;

				typename NodeMap::iterator s = postfix->find(w);
				if (s == postfix->end()) {
					(*postfix)[w] = p->second;
				}
				else {
					(s->second)->Merge(p->second);
				}
			}
		}
		delete n->postfix; // All nodes have been copied or merged
	}
	else postfix = n->postfix;
	n->postfix = NULL; // Avoid trouble when deleting node

	// Merge all "or" nodes
	if (orNodes) {
		if (n->orNodes) {
			for (typename NodeMap::iterator p = n->orNodes->begin(); p != n->orNodes->end(); ++p) {
				const wxString& w = p->second->word;

				typename NodeMap::iterator s = orNodes->find(w);
				if (s == orNodes->end()) {
					(*orNodes)[w] = p->second;
				}
				else {
					(s->second)->Merge(p->second);
				}
			}
		}
		delete n->orNodes; // All nodes have been copied or merged
	}
	else orNodes = n->orNodes;
	n->orNodes = NULL; // Avoid trouble when deleting node

	// Merge all ancestor nodes
	if (ancestors) {
		if (n->ancestors) {
			for (typename NodeMap::iterator p = n->ancestors->begin(); p != n->ancestors->end(); ++p) {
				const wxString& w = p->second->word;

				typename NodeMap::iterator s = ancestors->find(w);
				if (s == ancestors->end()) {
					(*ancestors)[w] = p->second;
				}
				else {
					(s->second)->Merge(p->second);
				}
			}
		}
		delete n->ancestors; // All nodes have been copied or merged
	}
	else ancestors = n->ancestors;
	n->ancestors = NULL; // Avoid trouble when deleting node

	// Merge styles
	if (targets) {
		if (n->targets) {
			// TODO: Avoid duplicates
			targets->insert(targets->end(), n->targets->begin(), n->targets->end());
			delete n->targets;
		}
	}
	else targets = n->targets;
	n->targets = NULL;

	// Delete the contributing node
	delete n;
}

template<class T> void sNode<T>::Print(size_t indent) const {
	const wxString pre(' ', indent);
	if (indent == 0) wxLogDebug(wxT("%s%s"), pre.c_str(), word.c_str());

	if (postfix) {
		wxLogDebug(wxT("%spostfix:"), pre.c_str());
		for (typename NodeMap::const_iterator p = postfix->begin(); p != postfix->end(); ++p) {
			wxLogDebug(wxT("%s  %s"), pre.c_str(), p->second->word.c_str());

			if (p->second->targets) {
				wxLogDebug(wxT("%s    -> %d"), pre.c_str(), p->second->targets->size());
			}

			p->second->Print(indent+5);
		}
	}

	if (orNodes) {
		wxLogDebug(wxT("%sor:"), pre.c_str());
		for (typename NodeMap::const_iterator p2 = orNodes->begin(); p2 != orNodes->end(); ++p2) {
			wxLogDebug(wxT("%s  %s"), pre.c_str(), p2->second->word.c_str());

			if (p2->second->targets) {
				wxLogDebug(wxT("%s    -> %d"), pre.c_str(), p2->second->targets->size());
			}

			p2->second->Print(indent+5);
		}
	}

	if (ancestors) {
		wxLogDebug(wxT("%sancestors:"), pre.c_str());
		for (typename NodeMap::const_iterator p3 = ancestors->begin(); p3 != ancestors->end(); ++p3) {
			wxLogDebug(wxT("%s  %s"), pre.c_str(), p3->second->word.c_str());

			if (p3->second->targets) {
				wxLogDebug(wxT("%s    -> %d"), pre.c_str(), p3->second->targets->size());
			}

			p3->second->Print(indent+5);
		}
	}
}
