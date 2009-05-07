#ifdef __WXMSW__
    #pragma warning(disable: 4786)
#endif

#ifndef __MATCHERS_H__
#define __MATCHERS_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#include <map>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

// pre-declarations
class wxRegEx;
class interval;

struct real_pcre;                 // This double pre-definition is needed
typedef struct real_pcre pcre;    // because of the way it is defined in pcre.h
struct pcre_extra;
class match_matcher;


// Definitions
struct style {
	wxString name;
	wxString scope;
	wxColour foregroundcolor;
	wxColour backgroundcolor;
	int fontflags;
};

class matcher {
public:
	matcher()
	: m_isEnabled(true), m_isInitialized(false) {};
	virtual ~matcher() {};

	void SetName(const wxString& name) {m_name =  name;};
	const wxString& GetName() const {return m_name;};
	virtual bool Init(bool deep=false) = 0;
	
	bool IsInitialized() const {return m_isInitialized;};
	bool IsEnabled() const {return m_isEnabled;};
	void Disable() {m_isEnabled = false;};

	virtual const wxString& GetCaptureName(unsigned int WXUNUSED(capkey)) const {wxASSERT(false); return s_emptyString;};
	virtual const wxString& GetContentName() const {return s_emptyString;};

	virtual matcher& GetCallout(unsigned int callout_id) = 0;

	virtual bool HasCaptures() const {return false;};
	virtual bool IsSpanStart(unsigned int) {return false;};
	virtual bool IsSpanEnd(unsigned int) {return false;};
	virtual bool IsGroup() {return false;};
	virtual bool IsSpan() const {return false;};
	virtual matcher* GetMember(unsigned int) {return this;};
	
	virtual unsigned int GetSubId(unsigned int) {return 0;};

	// Matching
	virtual int Match(char* line, unsigned int start, unsigned int len, unsigned int& callout_id, int *ovector, int ovecsize, int zeromatch) = 0;

	struct calloutref {
		matcher* matchptr;
		match_matcher* realMatchptr;
		unsigned int callout_id;
	};

	// Only for other matchers
	virtual size_t GetMembers(vector<calloutref>& refs) = 0;
	virtual matcher& SubGetCallout(unsigned int) {return *this;};
	virtual bool SubIsSpanStart(unsigned int) {return false;};
	virtual unsigned int SubGetId(unsigned int id) {return id;};

	// Regex support functions
	static void RegExConvert(wxString& pattern);

protected:
#ifdef __WXDEBUG__
	static bool RegExVerify(const wxString& pattern, bool matchcase=true);
#endif

	// Regex optimization
	typedef map<wxChar, void*> NodeMap;
	static void OptimizeRegex(wxString& pattern);
	static void RebuildPattern(wxString& pattern, const NodeMap& node);
	static void DeleteNode(NodeMap* node);

	// Member variables
	bool m_isEnabled;
	wxString m_name;
	bool m_isInitialized;
	static const wxString s_emptyString;

	// static regexes
	static wxRegEx s_alternatives;
	static wxRegEx s_tabspattern;
	static wxRegEx s_repfromzero;
	static wxRegEx s_backref;
};

class group_matcher : public matcher {
public:
	group_matcher()
	: matcher(), m_initializing(false) {};
	~group_matcher() {};
	void AddMember(matcher* m);

	bool IsValid() const {return !m_members.empty();};

	// Generic class functions
	bool Init(bool deep=false);
	bool IsGroup() {return true;};
	bool IsSpanStart(unsigned int callout_id);
	pcre* GetMatchPattern();
	matcher& GetCallout(unsigned int callout_id);
	matcher* GetMember(unsigned int callout_id);

	unsigned int GetSubId(unsigned int id);
	size_t GetMembers(vector<calloutref>& refs);

	// Matching
	int Match(char* line, unsigned int start, unsigned int len, unsigned int& callout_id, int *ovector, int ovecsize, int zeromatch);

	bool SubIsSpanStart(unsigned int callout_id) {return IsSpanStart(callout_id);};
	matcher& SubGetCallout(unsigned int callout_id) {return GetCallout(callout_id);};
	unsigned int SubGetId(unsigned int id) {return GetSubId(id);};

protected:
	vector<matcher*> m_members;
	vector<calloutref> m_refs;

	bool m_initializing;
};

class match_matcher : public matcher {
public:
	match_matcher() : matcher(), m_hasCaptures(false), m_compiledPattern(NULL), m_patternStudy(NULL) {};
	~match_matcher();
	bool Init(bool) {return true;};

	void SetPattern(const wxString& pattern);
	void SetPattern(const char* pattern);
	const wxString& GetPattern() {return m_pattern;};

	pcre* GetMatchPattern();
	pcre_extra* GetPatternStudy() {return m_patternStudy;};

	// Matching
	int Match(char* line, unsigned int start, unsigned int len, unsigned int& callout_id, int *ovector, int ovecsize, int zeromatch);

	// Captures
	bool HasCaptures() const {return m_hasCaptures;};
	void AddCapture(unsigned int capkey, const wxString& name);
	const wxString& GetCaptureName(unsigned int capkey) const;

	// Generic class functions
	matcher& GetCallout(unsigned int callout_id);

	size_t GetMembers(vector<calloutref>& refs);

#ifdef __WXDEBUG__
	void Print() const {wxLogDebug(wxT("match_matcher: \"%s\""), m_pattern.c_str());};
#endif  //__WXDEBUG__

private:
	bool RegExCompile(const wxString& pattern, bool matchcase=true);

	// Member variables
	wxString m_pattern;
	wxString m_convPattern;
	bool m_hasCaptures;
	pcre* m_compiledPattern;
	pcre_extra* m_patternStudy;
	map<unsigned int,wxString> m_captures;
};

class span_matcher : public group_matcher {
public:
	span_matcher()
	: group_matcher(), m_startMatcher(NULL), m_endMatcher(NULL), m_hasEndCaptures(false) {};
	~span_matcher() {};

	bool Init(bool deep=false);
	void ReInit(const vector<char>& text, const int* captures, unsigned int capcount);

	void SetStartMatcher(match_matcher* m) {m_startMatcher = m;};
	void SetEndMatcher(match_matcher* m) {m_endMatcher = m;};
	void SetEndPattern(const wxString& pattern) {m_endPattern = pattern;};
	bool HasEndCaptures() const {return m_hasEndCaptures;};

	void SetContentName(const wxString& name) {m_contentName = name;};
	const wxString& GetContentName() const {return m_contentName;};

	bool IsSpan() const {return true;};
	bool IsSpanEnd(unsigned int callout_id);

	match_matcher* GetStartMatcher() const {return m_startMatcher;};
	matcher* GetStartMember(unsigned int callout_id) {return m_startMatcher->GetMember(callout_id);};

	bool SubIsSpanStart(unsigned int) {return true;};
	matcher& SubGetCallout(unsigned int) {return *this;};
	unsigned int SubGetId(unsigned int id) {return id;};

	size_t GetMembers(vector<calloutref>& refs);

private:
	match_matcher* m_startMatcher;
	match_matcher* m_endMatcher; // only used for captures
	wxString m_endPattern;
	wxString m_groupPattern;
	bool m_hasEndCaptures;
	wxString m_contentName;

	static wxRegEx s_refToCapture;
};

#endif // __MATCHERS_H__
