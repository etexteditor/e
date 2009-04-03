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

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#ifdef __WXMSW__
    #pragma warning(disable:4786)
#endif

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif
#include "Catalyst.h"
#include "DataText.h"

// pre-declarations
class doc_byte_iter;
struct real_pcre;                 // This double pre-definition is needed
typedef struct real_pcre pcre;    // because of the way it is defined in pcre.h
struct pcre_extra;

class Document {
public:
	Document(const doc_id& di, CatalystWrapper cw);
	Document(CatalystWrapper cw); // Creates an uninitialized revision
	~Document();

	// Synchonization/transactions
	RecursiveCriticalSection& GetReadLock() const {return m_catalyst.GetReadLock();};
	RecursiveCriticalSection& GetWriteLock() {return m_catalyst.GetWriteLock();};

	bool IsOk() const;
	bool IsEmpty() const;
	bool IsDraft() const {return m_docId.IsDraft();};
	bool IsDocument() const {return m_docId.IsDocument();};
	void Clear(bool initialRevision=false);
	void Close();
	void Commit(const wxString& label, const wxString& desc);

	// Text Retrieval
	wxString GetText() const;
	wxString GetTextPart(int start_pos, int end_pos) const;
	void GetTextPart(int start_pos, int end_pos, wxString& text) const
		{wxASSERT(IsOk());m_catalyst.ResetIdle();m_textData.GetTextPart(start_pos, end_pos, text);};
	void GetTextPart(int start_pos, int end_pos, unsigned char* buffer) const
		{wxASSERT(IsOk());m_catalyst.ResetIdle();m_textData.GetTextPart(start_pos, end_pos, buffer);};
	void GetTextPart(int start_pos, int end_pos, vector<char>& buffer) const;
	unsigned int GetLine(unsigned int start_pos, vector<char>& buffer) const
		{wxASSERT(IsOk()); return m_textData.GetLine(start_pos, buffer);};
	wxString GetTextPart(const doc_id& version, int start_pos, int end_pos) const;
	wxChar GetChar(unsigned int pos) const;
	const DataText& GetData() const {return m_textData;};
	void WriteText(wxOutputStream& stream) const;

	// Length & Positions
	unsigned int GetLength() const;
	unsigned int GetLengthInChars(unsigned int start_pos, unsigned int end_pos) const;
	unsigned int GetCharPos(unsigned int offset, int char_count) const;
	unsigned int GetCharPos(const doc_id& version, unsigned int offset, int char_count) const;
	unsigned int GetCharLength(unsigned int pos) const;
	unsigned int GetNextCharPos(unsigned int pos) const;
	unsigned int GetPrevCharPos(unsigned int pos) const;
	unsigned int GetValidCharPos(unsigned int pos) const;
	unsigned int GetLineStart(unsigned int pos) const;
	unsigned int GetLineEnd(unsigned int pos) const;

	// Loading & Saving
	cxFileResult LoadText(const wxFileName& path, vector<unsigned int>& offsets, wxFontEncoding enc=wxFONTENCODING_SYSTEM, const wxString& mirror=wxEmptyString);
	cxFileResult SaveText(const wxFileName& path, bool forceNativeEOL=false, const wxString& realpath=wxEmptyString, bool keepMirrorDate=false);
	void GetLines(vector<unsigned int>& list) const;

	// Modification
	unsigned int Insert(int pos, const wxString& text);
	unsigned int Insert(int pos, const char* text);
	void Delete(int start_pos, int end_pos);
	int DeleteChar(unsigned int pos, bool nextchar=true);
	void DeleteAll();
	void Move(int source_startpos, int source_endpos, int dest_pos);
	unsigned int Replace(unsigned int start_pos, unsigned int end_pos, const wxString& text);
	void Freeze();

	// Searching
	search_result FindBackwards(const wxString& searchtext, int start_pos, bool matchcase) const;
	search_result Find(const wxString& searchtext, int start_pos, bool matchcase=false, int end_pos = 0) const;

	search_result RegExFind(const pcre* re, const pcre_extra* study, int start_pos, map<unsigned int,interval> *captures=NULL, int end_pos = 0, int search_options=0) const;
	search_result RegExFind(const wxString& searchtext, int start_pos, bool matchcase=false, map<unsigned int,interval> *captures=NULL, int end_pos = 0) const;
	search_result RegExFindBackwards(const wxString& searchtext, int start_pos, bool matchcase=false) const;
	//static pcre* RegExCompile(const wxString& pattern, bool matchcase=false);

	// Diff
	cxNodeInfo GetNodeInfo(unsigned int pos) const {return m_textData.GetNodeInfo(pos);};
	vector<match> Diff(const doc_id& version1, const doc_id& version2) const;
	void PartialDiff(const interval& range, vector<cxDiffEntry>& rangeHistory) const;
	vector<cxChange> GetChanges(const doc_id& version1, const doc_id& version2) const;
	bool TextChanged(const doc_id& version1, const doc_id& version2) const;
	bool PropertiesChanged(const doc_id& version1, const doc_id& version2) const;

	// Properties
	wxDateTime GetDate() const;
	wxString GetPropertyName() const;
	void SetPropertyName(const wxString& name);
	wxFontEncoding GetPropertyEncoding() const;
	void SetPropertyEncoding(wxFontEncoding encoding);
	wxTextFileType GetPropertyEOL() const;
	void SetPropertyEOL(wxTextFileType eol);
	bool GetPropertyBOM() const;
	void SetPropertyBOM(bool hasBOM);
	void SetDocRead(bool isRead=true);
	void SetDefaultEncoding();

	// Properties (generic)
	bool HasProperty(const wxString& name) const;
	bool GetProperty(const wxString& name, wxString& value) const;
	void SetProperty(const wxString& name, const wxString& value);
	void DeleteProperty(const wxString& name);

	// Document ID & meta
	void CreateNew();
	void SetDocument(const doc_id& di);
	doc_id GetDocument() const {return m_docId;};
	doc_id GetParent() const;
	doc_id GetDraftParent() const;
	doc_id GetDraftParent(int version_id) const;
	bool IsDraftAttached() const;
	int GetDocumentID() const;
	//void SetVersion(VERSION_ID vid);
	int GetVersionID() const;
	int GetVersionCount() const;
	int GetVersionLength(const doc_id& di) const;
	void GetVersionChildren(int version, vector<int>& childlist) const;
	void MakeHead();

	// Grouping of multible changes
	void StartChange();
	void EndChange();

	bool operator==(const doc_id& di) const;
	bool operator!=(const doc_id& di) const;

	// Debug functions
#ifdef __WXDEBUG__
	void Print() const;
	void PrintAll() const;
	int NodeCount() const;
#endif  //__WXDEBUG__

private:
	// data members
	Catalyst& m_catalyst;
	Dispatcher& dispatcher;
	c4_View vRevisions;
	//c4_View vDocuments;
	c4_View vNodes;
	c4_View vHistory;
	doc_id m_docId;
	bool do_notify;
	DataText m_textData;

	// Cache of last compiled regex
	mutable wxString m_regex_cache;
	mutable int m_options_cache;
	mutable pcre *m_re;

	// Support functions
	void PrepareForChange();
	node_ref GetHeadnode() const;
	node_ref GetHeadnode(const doc_id& di) const;
	node_ref GetPropnode() const;
	node_ref GetPropnode(const doc_id& di) const;
	void UpdateHeadnode();
	void UpdatePropnode(node_ref propnode);
	void NewRevision();

	static bool DetectEncoding(const char* buffer, size_t len, wxFontEncoding& encoding, unsigned int& BOM_len);

	// Notifier handlers
	static void OnDocDeleted(Document* self, void* data, int filter);

	friend class doc_byte_iter;
};

class DocumentWrapper {
public:
	DocumentWrapper(CatalystWrapper& cw) : m_doc(cw) {}; // unintialized doc
	DocumentWrapper(CatalystWrapper& cw, bool createNew) : m_doc(cw) { // new doc
		wxASSERT(createNew);
		if (createNew) {
			RecursiveCriticalSectionLocker cx_lock(GetReadLock());
			m_doc.CreateNew();
		}
	};
	DocumentWrapper(Document& doc) : m_doc(doc) {};
	DocumentWrapper(const doc_id& di, CatalystWrapper& cw) : m_doc(di, cw) {};

	bool operator==(const doc_id& di) const {return (m_doc == di);};
	bool operator!=(const doc_id& di) const {return (m_doc != di);};
	bool IsOk() const {return m_doc.IsOk();};

	// GetLength() is so used that we add direct access from the wrapper
	unsigned int GetLength() const {
		RecursiveCriticalSectionLocker cx_lock(m_doc.GetReadLock());
		return m_doc.GetLength();
	};

	RecursiveCriticalSection& GetReadLock() const {return m_doc.GetReadLock();};
	RecursiveCriticalSection& GetWriteLock() {return m_doc.GetWriteLock();};

	Document& GetDoc() {return m_doc;};
	const Document& GetDoc() const {return m_doc;};

private:
	Document m_doc;
};

#endif // __DOCUMENT_H__
