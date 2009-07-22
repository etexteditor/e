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

#ifndef __CATALYST_H__
#define __CATALYST_H__

// wxString, wxImage, wxDatetime, wxArrayString
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include "mk4.h"
#include <wx/filename.h>
#include <wx/textbuf.h>
#include "MMapBuffer.h"
#include <wx/socket.h>
#include <wx/image.h>

#undef CATALYST_OLD_CODE

//#include <map>
#include <vector>
#include <list>
#include "auto_vector.h"

#include "Dispatcher.h"
//#include "zeroconf.h"
#include "RecursiveCriticalSection.h"
#include "Interval.h"

using namespace std;

// Defines (for synchronizations during multi-threading)
#define cxLOCK_READ(catalystWrapper) {RecursiveCriticalSectionLocker cx_lock(catalystWrapper.GetReadLock()); \
                                      const Catalyst& catalyst = catalystWrapper.GetCatalyst();
#define cxLOCK_WRITE(catalystWrapper){RecursiveCriticalSectionLocker cx_lock(catalystWrapper.GetWriteLock()); \
                                      Catalyst& catalyst = catalystWrapper.GetCatalyst();
#define cxLOCKDOC_READ(docWrapper) {RecursiveCriticalSectionLocker cx_lock(docWrapper.GetReadLock()); \
                                      const Document& doc = docWrapper.GetDoc();
#define cxLOCKDOC_WRITE(docWrapper){RecursiveCriticalSectionLocker cx_lock(docWrapper.GetWriteLock()); \
                                      Document& doc = docWrapper.GetDoc();
#define cxENDLOCK }

// pre-declarations
class Document;
class SyncEvent;
class RemoteProfile;
class cxInternal;
class eSettings;

// Type definitions
typedef int REVISION_ID; // DOCUMENT_ID
typedef int VERSION_ID;
typedef int NODE_ID;

// Constant variables
enum data_type {DATA_TEXT, DATA_LIST, DATA_BOOLEAN, DATA_NUMBER};
enum cxNodeType {TYPE_NODE, TYPE_DATA, TYPE_PART, TYPE_FILE};
enum {STATE_EDITABLE, STATE_FROZEN};
enum cxShareStatus {cxSHARE_FULL, cxSHARE_PENDING, cxSHARE_READ, cxSHARE_WRITE};
enum cxRevisionState {cxREVSTATE_DELETED, cxREVSTATE_NORMAL, cxREVSTATE_PENDING};
enum cxEncodeMode {cxENCODE_SIGN, cxENCODE_TRANSPORT};
enum doc_type {DRAFT=0, DOCUMENT=1};

// cxDEBUG2 is set if we want to run multible instances
// on the same computer for debugging
/*#ifdef cxDEBUG2
enum {
	cxCLIENT_PORT = 3000,
	cxSERVER_PORT = 4000
};
#else
enum {
	cxCLIENT_PORT = 4000,
	cxSERVER_PORT = 3000
};
#endif //cxDEBUG2 */
enum {
	cxCLIENT_PORT = 4000,
	cxSERVER_PORT = 4000
};

enum cxFileResult {
	cxFILE_OK,
	cxFILE_CONV_ERROR,
	cxFILE_OPEN_ERROR,
	cxFILE_DOWNLOAD_ERROR,
	cxFILE_WRITABLE_ERROR
};

enum cxChangeType {
	cxINSERTION,
	cxDELETION,
	cxVERSIONCHANGE
};

struct cxBookmark {
	unsigned int line_id;
	unsigned int start;
	unsigned int end;
};

struct match {
	unsigned int iv1_start_pos;
	unsigned int iv1_end_pos;
	unsigned int iv2_start_pos;
	unsigned int iv2_end_pos;
};
struct cxChange {
	cxChangeType type;
	unsigned int pos;
	unsigned int start;
	unsigned int end;
	unsigned int lines;
};
struct cxLineChange {
	unsigned int start;
	unsigned int end;
	int diff;
	int lines;
};
struct search_result {
	int error_code;
	unsigned int start;
	unsigned int end;
};
struct cxShare {
	unsigned int userId;
	cxShareStatus status;
	bool notify;
};

class doc_id {
public:
	doc_id(doc_type t, int docid, int verid) : type(t), document_id(docid), version_id(verid) {};
	doc_id() : type(DRAFT), document_id(-1), version_id(-1) {}; // default constructor
	bool operator==(const doc_id& di) const
		{return type == di.type && document_id == di.document_id && version_id == di.version_id;};
	bool operator!=(const doc_id& di) const
		{return type != di.type || document_id != di.document_id || version_id != di.version_id;};
	bool IsOk() const {return document_id != -1 && version_id != -1;};
	bool IsDraft() const {return type == DRAFT;};
	bool IsDocument() const {return type == DOCUMENT;};
	bool SameDoc(const doc_id& di) const {return di.type == type && di.document_id == document_id;};
	void Invalidate() {document_id = -1; version_id = -1;};
	doc_type Type() const {wxASSERT(version_id != -1); return type;};

	// Variables
	doc_type type;
	int document_id;
	int version_id;
};

class docid_pair {
public:
	docid_pair(const doc_id& d1, const doc_id& d2) : doc1(d1), doc2(d2) {};
	const doc_id doc1;
	const doc_id doc2;
};

class node_ref {
public:
	node_ref(int vid, int nid) : version_id(vid), node_id(nid) {};
	node_ref() : version_id(-1), node_id(-1) {}; // default constructor
	bool operator!=(const node_ref& nr) const
	{return version_id != nr.version_id || node_id != nr.node_id;};
	bool IsOk() const {return node_id != -1;};
	bool IsDraft() const {wxASSERT(node_id != -1); return version_id == -1;};
	bool IsDocument() const {wxASSERT(node_id != -1); return version_id != -1;};
	doc_type Type() const {wxASSERT(node_id != -1); return (version_id == -1) ? DRAFT : DOCUMENT;};

	// Variables
	int version_id;
	int node_id;
};

class cxNodeInfo {
public:
	cxNodeInfo() : start(0), end(0) {};
	cxNodeInfo(unsigned int s, unsigned int e, const doc_id& di) : start(s), end(e), source(di) {};

	// variables
	unsigned int start;
	unsigned int end;
	doc_id source;
};

struct cxDiffEntry {
	unsigned int version;
	unsigned int parent;
	interval range;
};

struct cxUserMsg {
	int flags;
	c4_Row rUser;
};

class cxMatch {
	public:
		cxMatch(size_t start1, size_t end1, size_t start2, size_t WXUNUSED(end2))
			: offset1(start1), offset2(start2), len(end1-start1) {};
		cxMatch(size_t start1, size_t start2, size_t len)
			: offset1(start1), offset2(start2), len(len) {};
		void ExtendUp() {--offset1; --offset2; ++len;};
		void ExtendDown() {++len;};
		size_t start1() const {return offset1;};
		size_t end1() const {return offset1+len;};
		size_t start2() const {return offset2;};
		size_t end2() const {return offset2+len;};
		size_t length() const {return len;};
		bool operator<(const cxMatch& m) const {return offset1 < m.offset1;};
	
		size_t offset1;
		size_t offset2;
		size_t len;
	};

// Constant db accessors
static const c4_IntProp    pAuthor("author");
static const c4_BytesProp  pAuthorBytes("author_bytes");
static const c4_IntProp    pBuffPath("buffpath");
static const c4_ViewProp   pChildren("children");
static const c4_IntProp    pChildref("childref");
static const c4_IntProp    pColor("color");
static const c4_BytesProp  pData("data");
static const c4_IntProp    pDatatype("datatype");
static const c4_LongProp   pDate("date");
static const c4_IntProp    pDescNode("desc");
static const c4_BytesProp  pDocBytes("doc_bytes");
static const c4_IntProp    pDocId("doc_id");
static const c4_BytesProp  pDocKey("key");
static const c4_BytesProp  pDocSig("id");
static const c4_IntProp    pDocType("doctype");
static const c4_ViewProp   pFreenodes("freenodes");
static const c4_IntProp    pHead("head");
static const c4_BytesProp  pHeadBytes("head_bytes");
static const c4_IntProp    pHeadnode("headnode");
static const c4_IntProp    pHeadType("headtype");
static const c4_IntProp    pHeadVer("headver");
static const c4_ViewProp   pHistory("history");
static const c4_StringProp pHostName("hostname");
static const c4_IntProp    pHostPort("port");
static const c4_ViewProp   pHosts("hosts");
static const c4_IntProp    pInterfaceIndex("ifc");
static const c4_StringProp pInterfaceName("name");
static const c4_ViewProp   pInterfaces("interfaces");
static const c4_IntProp    pLabelNode("label");
static const c4_StringProp pLastIP("lastip");
static const c4_IntProp    pLength("length");
static const c4_IntProp    pMax("max");
static const c4_IntProp    pNodeid("nodeid");
static const c4_ViewProp   pNoderefs("noderefs");
static const c4_ViewProp   pNodes("nodes");
static const c4_IntProp    pNotify("notify");
static const c4_IntProp    pOffset("offset");
static const c4_IntProp    pParent("parent");
static const c4_BytesProp  pParentBytes("parent_bytes");
static const c4_IntProp    pParentDoc("pdoc");
static const c4_IntProp    pParentVer("pver");
static const c4_StringProp pPath("path");
static const c4_BytesProp  pPropBytes("prop_bytes");
static const c4_IntProp    pPropnode("propnode");
static const c4_IntProp    pPropVer("propver");
static const c4_IntProp    pResolved("resolved");
static const c4_BytesProp  pRevSig("id");
static const c4_ViewProp   pShares("shares");
static const c4_IntProp    pState("state");
static const c4_IntProp    pStatus("status");
static const c4_IntProp    pSource("source");
static const c4_IntProp    pTimezone("tz");
static const c4_IntProp    pType("type");
static const c4_IntProp    pUnread("unread");
static const c4_IntProp    pUser("user");
static const c4_BytesProp  pUserID("id");
static const c4_BytesProp  pUserKey("key");
static const c4_StringProp pUserName("name");
static const c4_BytesProp  pUserPic("pic");
static const c4_BytesProp  pVerBytes("ver_bytes");
static const c4_IntProp    pVerId("ver_id");
static const c4_IntProp    pVersionid("versionid");

// ----- Catalyst ------------------------------------

class Catalyst {
public:
	explicit Catalyst(const wxString& path);
	~Catalyst();

	// Synchonization/transactions
	RecursiveCriticalSection& GetReadLock() const;
	RecursiveCriticalSection& GetWriteLock();
	void UnLock();
	void ReLock();
	void UnLock() const;
	void ReLock() const;

	// Identity
	bool CreateIdentity(const wxString& name);
	void SetProfile(const wxString& name, const wxImage& picture=wxImage());

	// Document handling
	doc_id NewDocument();
	Document GetDocument(const doc_id& di);
	int CreateDocument();
	int CreateDocument(const c4_Bytes& docSig);
	void DeleteDraft(const doc_id& di);
	void ClearDraftsWithoutParent();
	int NewDraft();

	// File mirrors
	bool GetFileMirror(const wxString& path, doc_id& di, wxDateTime& dt) const;
	int SetFileMirror(const wxString& path, const doc_id di, const wxDateTime& dt);
	int SetFileMirrorToModified(const wxString& path, const doc_id di);
	void DeleteFileMirror(const wxString& path);
	bool IsMirrored(const doc_id& di) const;
	bool IsMirroredSpecific(const doc_id& di) const;
	bool GetMirrorPaths(const doc_id& di, wxArrayString& paths) const;
	bool GetMirrorPathsForDraft(const doc_id& di, wxArrayString& paths) const;
	bool VerifyMirror(const wxString& path, const doc_id& di) const;

	Dispatcher& GetDispatcher();

	const c4_View& GetRevView() const;
	const c4_View& GetDocView() const;
	const wxString& GetPath() const;
	const wxFileName& GetDocPath() const;
	const wxFileName& GetDraftPath() const;

	void AllowCommit(bool allowCommit);
	void Commit();
	void CommitIdle();
	void ResetIdle();

	// Settings functions
	const wxLongLong& GetId() const;
	void MoveOldSettings(eSettings& settings);

	// List of documents
	void GetDocList(vector<doc_id>& doclist) const;

	// Document properties
	cxRevisionState GetDocState(const doc_id& di) const;
	bool IsDocUnread(const doc_id& di) const;
	unsigned int GetDocAuthor(const doc_id& di) const;
	wxString GetDocName(const doc_id& di) const;
	wxString GetDocHeadName(unsigned int docId) const;
	doc_id GetDocHead(unsigned int docId) const;
	wxDateTime GetDocDate(const doc_id& di) const;
	wxString GetDocAge(const doc_id& di) const;
	wxString GetLabel(const doc_id& di) const;
	wxString GetDescription(const doc_id& di) const;
	doc_id GetDocParent(const doc_id& di) const;
	int GetChildCount(const doc_id& di) const;
	doc_id GetChildID(const doc_id& di, int child_pos) const;
	void GetShareableChildList(const doc_id& di, vector<unsigned int>& revList) const;
	bool InSameHistory(const doc_id& d1, const doc_id& d2) const;
	bool IsEarlier(const doc_id& d1, const doc_id& d2) const;
	bool IsLater(const doc_id& d1, const doc_id& d2) const;
	bool IsValidDoc(unsigned int docId) const;
	bool IsOk(const doc_id& di) const;
	bool IsOk(const doc_id& di, const node_ref& nr) const;

	// Draft specific properties
	doc_id GetDraftHead(int draft_id) const;
	bool IsDraftDeletableOnExit(const doc_id& di) const;
	int GetDraftParent(const doc_id& di) const;

	bool FindDocument(const c4_Bytes& docSig, int &docId) const;
	bool FindRevision(const c4_Bytes& docSig, const c4_Bytes& revSig, doc_id& di) const;
	bool FindRevision(unsigned int doc_id, const c4_Bytes& revSig, int& rev_id) const;
	bool AddRevision(c4_RowRef& rSourceRev);
	void SignRevision(const doc_id& di);

	// Diff
	void GetDiff(const Document& doc1, const Document& doc2, list<cxMatch>& matchList);

	// Registration functions
	bool IsRegistered() const;
	bool IsExpired() const;
	int DaysLeftOfTrial() const;
	int TotalDays() const;
	const wxString& RegisteredUserName() const;
	const wxString& RegisteredUserEmail() const;
	void ShowRegisterDlg(wxWindow* parent);
	
	// Users
	wxString GetUserName(unsigned int user_id) const;
	wxBitmap GetUserPic(unsigned int user_id) const;
	wxColour GetUserColor(unsigned int userId) const;

	// Utility functions
	int GetRand() const;
	static int HexToNumber(wxChar hex);
	static wxString GetDateAge(const wxDateTime& date);

private:
	// Utility functions
	wxLongLong GetRand64() const;

	// Member variables
	mutable RecursiveCriticalSection m_critSec;
	cxInternal* m_i;

	// Settings
	wxLongLong m_id;

	friend class Document;
	friend class cxInternal;
};

class CatalystWrapper {
public:
	CatalystWrapper(Catalyst& cx) : m_catalyst(cx) {};

	RecursiveCriticalSection& GetReadLock() const {return m_catalyst.GetReadLock();};
	RecursiveCriticalSection& GetWriteLock() const {return m_catalyst.GetWriteLock();};

	Catalyst& GetCatalyst() {return m_catalyst;};
	const Catalyst& GetCatalyst() const {return m_catalyst;};
	Dispatcher& GetDispatcher() const {return m_catalyst.GetDispatcher();};

private:
	Catalyst& m_catalyst;

	friend class Document;
};

#endif // __CATALYST_H__
