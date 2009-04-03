#ifndef __SYNCTHREAD_H__
#define __SYNCTHREAD_H__

#ifdef FEAT_COLLABORATION

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include <wx/socket.h>
#include <wx/sckstrm.h>

// STL can't compile with Level 4
// <map> is included int "Catalyst.h"
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <set>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

// Constant db accessors
static const c4_BytesProp pDocBytes("doc_bytes");
static const c4_BytesProp pAuthorBytes("author_bytes");
static const c4_BytesProp pHeadBytes("head_bytes");
static const c4_BytesProp pParentBytes("parent_bytes");
static const c4_BytesProp pPropBytes("prop_bytes");
static const c4_BytesProp pVerBytes("ver_bytes");

// CONNECTION TYPES
enum cxSyncType {
	cxSYNC_SERVER,
	cxSYNC_GET_USERS,
	cxSYNC_UPDATE
};

class SyncEvent : public wxEvent {
public:
	SyncEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
		: wxEvent(id, commandType), m_userId(0), m_error(wxSOCKET_NOERROR) {};
	SyncEvent(const SyncEvent& event) : wxEvent(event) {
		m_userId = event.m_userId;
		m_addr = event.m_addr;
		m_userList = event.m_userList;
		m_error = event.m_error;
	};

	virtual wxEvent* Clone() const {
		return new SyncEvent(*this);
	};

	void SetUserId(unsigned int userId) {m_userId = userId;};
	unsigned int GetUserId() const {return m_userId;};
	void SetAddress(const wxIPV4address& addr) {m_addr = addr;};
	void SetUsers(const c4_View& vUsers) {m_userList = vUsers;};
	const wxIPV4address& GetAddress() const {return m_addr;};
	const c4_View& GetUsers() const {return m_userList;};
	void SetError(wxSocketError error) {m_error = error;};
	wxSocketError GetError() const {return m_error;};

private:
	unsigned int m_userId;
	wxIPV4address m_addr;
	c4_View m_userList;
	wxSocketError m_error;

	DECLARE_DYNAMIC_CLASS(SyncEvent);
};
typedef void (wxEvtHandler::*SyncEventFunction) (SyncEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(syncEVT_USERLIST_RECEIVED, 801)
	DECLARE_EVENT_TYPE(syncEVT_CONNECT_FAILED, 801)
END_DECLARE_EVENT_TYPES()

#define SyncEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (SyncEventFunction) &func
#define EVT_SYNC_USERLIST_RECEIVED(func) wx__DECLARE_EVT0(syncEVT_USERLIST_RECEIVED, SyncEventHandler(func))
#define EVT_SYNC_CONNECT_FAILED(func) wx__DECLARE_EVT0(syncEVT_CONNECT_FAILED, SyncEventHandler(func))


class SyncThread : public wxThread {
public:
	SyncThread(cxSyncType type, CatalystWrapper cat, wxSocketBase* socket, wxEvtHandler& evtH); // cxSYNC_SERVER
	SyncThread(cxSyncType type, CatalystWrapper cat, const wxIPV4address& addr, wxEvtHandler& evtH, unsigned int user_id); // cxSYNC_UPDATE
	SyncThread(cxSyncType type, CatalystWrapper cat, const wxIPV4address& addr, wxEvtHandler& evtH);   // cxSYNC_GET_USERS
	virtual void *Entry();

private:
	bool Update();
	bool Receive();

	// Definitions
	enum cxProcessMode {cxACTIVE, cxPASSIVE};
	enum cxDocShareState {cxSHARE_NEW, cxSHARE_UNSUBSCRIBED};
	typedef map<unsigned int, set<unsigned int> > cxDocRevMap;
	typedef map<unsigned int, cxDocShareState> cxDocStateMap;

	bool UpdateDoc(unsigned int docId);
	bool SendRevision(const doc_id& di) const;
	bool SendRevIdTraversal(const doc_id& di);
	bool SendRevisionTraversal(const doc_id& di);
	bool GetUser(c4_Row& rRev);
	int GetRevision(c4_Row& rRev, cxProcessMode mode);

	void CheckQue(const c4_Bytes& authorSig, unsigned int authorId);

	bool Process(bool doWait=true, cxProcessMode mode=cxACTIVE);
	bool DoProcess(const wxString& request, cxProcessMode mode);
	bool HandleUpdate(unsigned int docId, cxProcessMode mode);

	bool ReadBytes(c4_Bytes& bytes, unsigned int len);
	bool ReadLine(wxString& result);

	bool ParseUserId(const wxString& in, c4_Bytes& userBytes, cxProcessMode mode=cxACTIVE);
	static bool ParseDocRev(const wxString& in, c4_Bytes& docBytes, c4_Bytes& revBytes);
	static bool ParseNodeid(const wxString& in, c4_Bytes& revBytes, int& node_id);
	static bool ParseHex(const wxString& in, c4_Bytes& bytes);
	static bool ParseHex64(wxString in, wxLongLong& ll);
	static int HexToNumber(wxChar hex);

	// Member variables
	cxSyncType m_type;
	CatalystWrapper m_catalyst;
	wxIPV4address m_addr;
	wxSocketBase* m_socket;
	wxEvtHandler& m_evtHandler;
	wxSocketOutputStream* m_out;
	unsigned int m_userId;
	c4_Bytes m_userBytes; // only valid if m_userId=0
	c4_View m_vQue; // Que for waiting revisions (needing author or parent)
	wxArrayString m_requestQue;
	bool m_isConnected; // true if the user known
	bool m_isDone; // true if we are done
	bool m_isRecieverDone; // true if reciever are done

	// Receiver state
	cxDocRevMap m_stateRevs;
	cxDocStateMap m_stateDocs;
};

#endif // FEAT_COLLABORATION
#endif // __SYNCTHREAD_H__
