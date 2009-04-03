#ifdef FEAT_COLLABORATION

#include "SyncThread.h"
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

// SyncEvent
DEFINE_EVENT_TYPE(syncEVT_USERLIST_RECEIVED)
DEFINE_EVENT_TYPE(syncEVT_CONNECT_FAILED)
IMPLEMENT_DYNAMIC_CLASS(SyncEvent, wxEvent)

SyncThread::SyncThread(cxSyncType type, CatalystWrapper cat, wxSocketBase* socket, wxEvtHandler& evtH)
: m_type(cxSYNC_SERVER), m_catalyst(cat), m_socket(socket), m_evtHandler(evtH), 
  m_userId(0), m_isConnected(false), m_isDone(false), m_isRecieverDone(false) {
	wxASSERT(type == cxSYNC_SERVER);

	Create();
    Run();
}

// Share documents with user on remote server
SyncThread::SyncThread(cxSyncType type, CatalystWrapper cat,  const wxIPV4address& addr, wxEvtHandler& evtH, unsigned int userId)
: m_type(cxSYNC_UPDATE), m_catalyst(cat), m_addr(addr), m_socket(NULL), m_evtHandler(evtH),
  m_userId(userId), m_isConnected(false), m_isDone(false), m_isRecieverDone(false) {
	wxASSERT(type == cxSYNC_UPDATE);
#ifdef __WXDEBUG__
	cxLOCK_READ(m_catalyst)
		wxASSERT(catalyst.IsValidUser(m_userId));
	cxENDLOCK
#endif // __WXDEBUG__

	Create();
    Run();
}

// Get list of users on remote server and returns
SyncThread::SyncThread(cxSyncType type, CatalystWrapper cat, const wxIPV4address& addr, wxEvtHandler& evtH)
: m_type(cxSYNC_GET_USERS), m_catalyst(cat), m_addr(addr), m_evtHandler(evtH),
  m_userId(0), m_socket(NULL), m_isConnected(false), m_isDone(false), m_isRecieverDone(false) {
	wxASSERT(type == cxSYNC_GET_USERS);

	Create();
    Run();
}

void* SyncThread::Entry()
{
	// If we have not been supplied a socket,
	// we have to make the connection
	if (m_type == cxSYNC_GET_USERS || m_type == cxSYNC_UPDATE) {
		wxLogDebug(wxT("cxSYNC_CONNECT:"));
		wxLogDebug(wxT("  cxHost: %s"), m_addr.Hostname());
		wxLogDebug(wxT("  cxPort: %u"), m_addr.Service());
		wxLogDebug(wxT("  cxIP: %s"), m_addr.IPAddress());

		wxSocketClient* sock = new wxSocketClient();
		sock->Connect(m_addr);

		if (sock->IsConnected()) m_socket = sock;
		else {
			wxLogDebug(wxT("Connect Failed: %s:%d"), m_addr.IPAddress(), m_addr.Service());
			
			// Notify caller that the connection failed
			SyncEvent event(syncEVT_CONNECT_FAILED, 1);
			event.SetUserId(m_userId);
			event.SetAddress(m_addr);
			event.SetError(sock->LastError());
			wxPostEvent(&m_evtHandler, event); // send in a thread-safe way

			// Clean up
			sock->Destroy();
			return NULL;
		}
	}
	else {
		wxASSERT(m_socket && m_socket->IsConnected());
		m_socket->GetPeer(m_addr);
	}
	
	// If we can't write anything for 10 seconds, assume a timeout
    m_socket->SetTimeout(10);

    // Wait for all the data to write, blocking on the socket calls
    m_socket->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);

	m_out = new wxSocketOutputStream(*m_socket);

	switch (m_type) {
	case cxSYNC_SERVER:
		wxLogDebug(wxT("server started"));
		Receive();
		wxLogDebug(wxT("server stopped"));
		break;

	case cxSYNC_UPDATE:
		{
			m_out->Write("CONNECT 1 ", 10);
			cxLOCK_READ(m_catalyst)
				catalyst.WriteUserId(*m_out, 0, true); // local user (+ modDate)
				m_out->Write(" ", 1);
				catalyst.WriteUserId(*m_out, m_userId); // remote user
			cxENDLOCK
			m_out->Write("\n", 1);

			// First line recieved has to be acknowledgement 
			wxString buf;
			if (!ReadLine(buf)) goto error;
			wxStringTokenizer tokens(buf ,wxT(' '));

			const wxString command = tokens.GetNextToken();
			if (command != wxT("ACK")) goto error;

			// Let catalyst know that we are connected to user
			m_isConnected = true;
#ifdef CATALYST_OLD_CODE
			cxLOCK_WRITE(m_catalyst)
				catalyst.AddConnection(m_userId);
			cxENDLOCK
#endif
			// Handle any requests from server
			if (!Process(false)) {
				wxLogDebug(wxT("  ERROR: Process1 failed"));
				goto error;
			}

			// Send the documents
			if (!Update()) {
				wxLogDebug(wxT("  ERROR: Update failed"));
				goto error;
			}

			// Let the client know that we are done
			m_out->Write("DONE\n", 5);
			m_isDone = true;

			// Handle any requests from server
			if (!Process()) {
				wxLogDebug(wxT("  ERROR: Process2 failed"));
				goto error;
			}

			// TODO: Check if more updates for user has arrived
		}
		break;

	case cxSYNC_GET_USERS:
		{
			m_out->Write("CONNECT_USERS\n", 14);

			// First line recieved has to be acknowledgement 
			// and count of users
			wxString buf;
			if (!ReadLine(buf)) break;
			wxStringTokenizer tokens(buf ,wxT(' '));

			wxString command = tokens.GetNextToken();
			if (command != wxT("ACK_USERS")) break;
			
			const wxString users_count_str = tokens.GetNextToken();
			if (users_count_str.empty()) break;
			const int users_count = wxAtoi(users_count_str);

			c4_View vUserList;
			for (int i = 0; i < users_count; ++i) {
				if (!ReadLine(buf)) break;
				tokens.SetString(buf ,wxT(' '));

				command = tokens.GetNextToken();
				if (command != wxT("USER")) break;

				const wxString user = tokens.GetNextToken();
				c4_Bytes userBytes;
				if (!ParseHex(user, userBytes)) break;

				c4_Row rUser;
				pUserID(rUser) = userBytes;
				if (!GetUser(rUser)) break;

				// Add to user list
				vUserList.Add(rUser);
			}

			// Notify caller that we have recieved the list
			SyncEvent event(syncEVT_USERLIST_RECEIVED, 1);
			event.SetAddress(m_addr);
			event.SetUsers(vUserList);
			wxPostEvent(&m_evtHandler, event); // send in a thread-safe way
		}
		break;
	
	default:
		wxASSERT(false); // Unknown type
	}

#ifdef CATALYST_OLD_CODE
	// Let catalyst know that the connection has ended
	if (m_isConnected) {
		cxLOCK_WRITE(m_catalyst)
			catalyst.RemoveConnection(m_userId);
		cxENDLOCK
	}
#endif

	wxLogDebug(wxT("  connection finished!"));

    // We can clean up & destroy the socket now
    delete m_out;
	m_socket->Destroy();

    return NULL;

error:
#ifdef CATALYST_OLD_CODE
	// Let catalyst know that the connection has ended
	if (m_isConnected) {
		cxLOCK_WRITE(m_catalyst)
			catalyst.RemoveConnection(m_userId);
		cxENDLOCK
	}
#endif

	// Notify caller that the connectio failed
	SyncEvent event(syncEVT_CONNECT_FAILED, 1);
	event.SetUserId(m_userId);
	event.SetAddress(m_addr);
	event.SetError(m_socket->LastError());
	wxPostEvent(&m_evtHandler, event); // send in a thread-safe way

	// We can clean up & destroy the socket now
    delete m_out;
	m_socket->Destroy();

    return NULL;
}

bool SyncThread::Receive() {
	wxString buf;

	// First line recieved has to be a connect request
	if (!ReadLine(buf)) return false;
	wxStringTokenizer tokens(buf ,wxT(' '));
	
	const wxString command = tokens.GetNextToken();

	if (command == wxT("CONNECT")) {
		// Check protocol version
		if (tokens.GetNextToken() != wxT("1")) return false;

		wxString clientId = tokens.GetNextToken();
		wxString serverId = tokens.GetNextToken();
		if (tokens.HasMoreTokens()) return false;

		// Check that it is us they want to connect to
		c4_Bytes serverBytes;
		if (!ParseHex(serverId, serverBytes)) return false;

		int userId;
		bool result;
		cxLOCK_READ(m_catalyst)
			if (!catalyst.IsLocalUser(serverBytes)) return false;

			// Notify client that the connection is accepted
			m_out->Write("ACK ", 4);
			catalyst.WriteUserId(*m_out);
			m_out->Write("\n", 1);

			// Check if we know the client
			// (ParseUserId will request profile if unknown)
			if (!ParseUserId(clientId, m_userBytes)) return false;
			result = catalyst.FindUser(m_userBytes, userId);
		cxENDLOCK

		if (result) {
			m_userId = userId;

			// Let catalyst know that we are connected to user
			m_isConnected = true;
			cxLOCK_WRITE(m_catalyst)
				catalyst.SetUserLastIP(m_userId, m_addr);
#ifdef CATALYST_OLD_CODE
				catalyst.AddConnection(m_userId);
#endif
			cxENDLOCK

			// Send updates to all documents the client is subscribed to
			if (!Update()) return false;
		}

		// Let the client know that we are done
		m_out->Write("DONE\n", 5);
		m_isDone = true;

		// Handle requests from client
		return Process();

		// TODO: Check if more updates for user has arrived
	}
	else if (command == wxT("CONNECT_USERS")) {
		// Acknowledge the user request (we are only one user)
		m_out->Write("ACK_USERS 1\n", 12);

		// Transmit full user profile (profile zero is our profile)
		m_out->Write("USER ", 5);
		
		cxLOCK_READ(m_catalyst)
			catalyst.WriteUserId(*m_out, 0);
			m_out->Write("\n", 1);
			catalyst.WriteUserProfile(*m_out, 0);
		cxENDLOCK

		return true;
	}
	else {
		// Unknown command
		wxASSERT(false);
		return false;
	}
}


bool SyncThread::Process(bool doWait, cxProcessMode mode) {
	// Handle qued requests
	if (mode == cxACTIVE) {
		while(m_requestQue.GetCount()) {
			const wxString request = m_requestQue[0];
			m_requestQue.RemoveAt(0);

			if (!DoProcess(request, cxACTIVE)) return false;
		}
	}

	// Never wait for timeout if we are both done
	if (m_isDone && m_isRecieverDone) doWait = false;

	// if we are not instructed to wait for timeout,
	// we return as soon as there are no more data
	const long ws = doWait ? -1 : 0;
	const long wm = doWait ?  0 : 1;
	
	// Handle incomming requests
	wxString request;
	while (m_socket->WaitForRead(ws, wm)) {
		if (TestDestroy()) break;

		if (m_socket->IsDisconnected()) {
			wxLogDebug(wxT("Lost connection"));
			// do not return yet, there might be data in the buffer
			//return false; 
		}

		if (!ReadLine(request)) return false;

		if(!DoProcess(request, mode)) return false;

		// If we are both done we can end the connection
		if (m_isDone && m_isRecieverDone) return true;
	}

	return true;
}

bool SyncThread::DoProcess(const wxString& request, cxProcessMode mode) {
	// In passive mode we cannot write to the socket.
	// We can only read, and que requests for later.

	wxStringTokenizer tokens(request ,wxT(' '));
	const wxString command = tokens.GetNextToken();

	if (command.empty()) return true;
	else if (command == wxT("UPDATE")) {
		const wxString doc = tokens.GetNextToken();
		c4_Bytes docBytes;
		if (!ParseHex(doc, docBytes)) return false;

		// Check if we already know the document
		int docId = -1;
		bool result;
		cxLOCK_READ(m_catalyst)
			result = catalyst.FindDocument(docBytes, docId);
		cxENDLOCK

		if(!result) {
			m_out->Write("DOC_IS_NEW ", 11);
			Catalyst::PrintString(*m_out, doc); // static does not need lock

			m_out->Write("\n", 1);

			// We can ignore rest of update request since we have
			// replied that we don't have any revisions.
			wxString line;
			do {
				if (!ReadLine(line)) return false;
			}
			while (line != wxT("END_UPDATE"));
		}
		else HandleUpdate(docId, mode);
	}
	else if (command == wxT("REV")) {
		const wxString doc_rev = tokens.GetNextToken();

		c4_Bytes docBytes;
		c4_Bytes revBytes;
		if (!ParseDocRev(doc_rev, docBytes, revBytes)) return false;

		c4_Row rRev;
		pDocBytes(rRev) = docBytes;
		pRevSig(rRev) = revBytes;

		const int err = GetRevision(rRev, mode);
		if (err) {
			wxLogDebug(wxT("ERROR: GetRevision failed %d"), err);
			//wxASSERT(false);
			return false;
		}

		// Check if we know the author
		cxLOCK_WRITE(m_catalyst)
			int authorId = -1;
			const bool knowAuthor = catalyst.FindUser(pAuthorBytes(rRev), authorId);
			if (knowAuthor && m_userId > 0) {
				pSource(rRev) = m_userId;
				pAuthor(rRev) = authorId;

				if (!catalyst.AddRevision(rRev)) {
					wxLogDebug(wxT("ERROR: AddRevision failed"));
				}
			}
			else {
				// GetRevision() will have requested the user profile
				// Que the revision until we recieve author and/or source info
				m_vQue.Add(rRev);
			}
		cxENDLOCK
	}
	else if (command == wxT("USER")) {
		const wxString user = tokens.GetNextToken();
		c4_Bytes userBytes;
		if (!ParseHex(user, userBytes)) return false;

		c4_Row rUser;
		pUserID(rUser) = userBytes;
		if (!GetUser(rUser)) return false;

		int userId;
		cxLOCK_WRITE(m_catalyst)
			userId = catalyst.AddUser(rUser);
		cxENDLOCK
		if (userId == -1) return false;

		// Check if we have received the client user profile
		if (m_userId == 0 && userBytes == m_userBytes) {
			m_userId = userId;

			// Let catalyst know that we are connected to user
			m_isConnected = true;
			cxLOCK_WRITE(m_catalyst)
				catalyst.SetUserLastIP(m_userId, m_addr);
#ifdef CATALYST_OLD_CODE
				catalyst.AddConnection(m_userId);
#endif
			cxENDLOCK
		}

		// Check if we have any qued revisions waiting for this
		// user profile.
		CheckQue(userBytes, userId);
	}
	else if (command == wxT("GET_USER")) {
		if (mode == cxACTIVE) {
			const wxString author = tokens.GetNextToken();
			c4_Bytes authorBytes;
			if (!ParseHex(author, authorBytes)) return false;

			cxLOCK_READ(m_catalyst)
				int user_id = -1;
				if (catalyst.FindUser(authorBytes, user_id)) {
					// Transmit full user profile
					m_out->Write("USER ", 5);
					catalyst.PrintBin(*m_out, authorBytes);
					m_out->Write("\n", 1);

					catalyst.WriteUserProfile(*m_out, user_id);
				}
				else return false; // Error: unknown user
			cxENDLOCK
		}
		else m_requestQue.Add(request);
	}
	else if (command == wxT("QUE_REQ_USER")) {
		wxASSERT(mode == cxACTIVE); // replaying qued request

		const wxString author = tokens.GetNextToken();

		// Request a user profile (qued request)
		m_out->Write("GET_USER ", 9);
		Catalyst::PrintString(*m_out, author);
		m_out->Write("\n", 1);
	}
	else if (command == wxT("QUE_ACK_REV")) {
		wxASSERT(mode == cxACTIVE); // replaying qued request

		const wxString doc = tokens.GetNextToken();
		const wxString rev = tokens.GetNextToken();

		const doc_id di(DOCUMENT, wxAtoi(doc), wxAtoi(rev));

		m_out->Write("HAVE_REV ", 9);
		cxLOCK_READ(m_catalyst)
			catalyst.WriteDocRevId(*m_out, di);
		cxENDLOCK
		m_out->Write("\n", 1);
	}
	else if (command == wxT("GET_DOC_LIST")) {
		if (mode == cxACTIVE) {
			m_out->Write("DOC_LIST\n", 9);

			// Send all documents
			cxLOCK_READ(m_catalyst)
				const c4_View& vDocuments = catalyst.GetDocView();

				for (int d = 0; d < vDocuments.GetSize(); ++d) {
					catalyst.WriteDocEntry(*m_out, d);
				}
			cxENDLOCK

			m_out->Write("END_DOC_LIST\n", 13);
		}
		else m_requestQue.Add(request);
	}
	else if (command == wxT("HAVE_REV")) {
		const wxString doc_rev = tokens.GetNextToken();
		c4_Bytes docBytes;
		c4_Bytes revBytes;
		if (!ParseDocRev(doc_rev, docBytes, revBytes)) return false;

		cxLOCK_READ(m_catalyst)
			doc_id di;
			if (!catalyst.FindRevision(docBytes, revBytes, di)) return false;

			// When we know that reciever has one revision
			// then we also know that it has all it's parents
			set<unsigned int>& revSet = m_stateRevs[di.document_id];
			while (di.IsOk() && revSet.find(di.version_id) == revSet.end()) {
				revSet.insert(di.version_id);
				di = catalyst.GetDocParent(di);
			}
		cxENDLOCK
	}
	else if (command == wxT("DOC_IS_NEW")) {
		const wxString doc = tokens.GetNextToken();
		c4_Bytes docBytes;
		if (!ParseHex(doc, docBytes)) return false;

		int docId;
		cxLOCK_READ(m_catalyst)
			if(!catalyst.FindDocument(docBytes, docId)) return false;
		cxENDLOCK

		m_stateDocs[docId] = cxSHARE_NEW;
	}
	else if (command == wxT("UNSUB_DOC")) {
		wxASSERT(m_userId != 0);

		const wxString doc = tokens.GetNextToken();
		c4_Bytes docBytes;
		if (!ParseHex(doc, docBytes)) return false;

		cxLOCK_WRITE(m_catalyst)
			int docId;
			if(!catalyst.FindDocument(docBytes, docId)) return false;

			catalyst.RemoveUserSubscription(m_userId, docId);
		cxENDLOCK
	}
	else if (command == wxT("DONE")) {
		m_isRecieverDone = true;
	}
	else return false;

	return true;
}

void SyncThread::CheckQue(const c4_Bytes& authorSig, unsigned int authorId) {
	if (m_userId == 0) return; // client source id must be valid

	int ndx = 0;
	while (1) {
		ndx = m_vQue.Find(pAuthorBytes[authorSig], ndx);
		if (ndx == -1) return;

		// TODO: Verify that we have parent (or is root)

		c4_RowRef rRevision = m_vQue[ndx];
		pSource(rRevision) = m_userId;
		pAuthor(rRevision) = authorId;

		cxLOCK_WRITE(m_catalyst)
			if (catalyst.AddRevision(rRevision)) {
				m_vQue.RemoveAt(ndx);
			}
			else ++ndx;
		cxENDLOCK
	}
}

bool SyncThread::HandleUpdate(unsigned int docId, cxProcessMode mode) {
#ifdef __WXDEBUG__
	cxLOCK_READ(m_catalyst)
		wxASSERT(catalyst.IsValidDoc(docId));
	cxENDLOCK
#endif //__WXDEBUG__

	wxString buf;
	wxStringTokenizer tokens;

	while(1) {
		if (!ReadLine(buf)) return false;
		if (buf == wxT("END_UPDATE")) return true;

		c4_Bytes revBytes;
		if (!ParseHex(buf, revBytes)) return false;

		// Check if we have the revision
		int revId;
		bool haveRev;
		cxLOCK_READ(m_catalyst)
			haveRev = catalyst.FindRevision(docId, revBytes, revId);
		cxENDLOCK

		if (haveRev) {
			const doc_id di(DOCUMENT, docId, revId);

			// Reply that we have the revision
			if (mode == cxACTIVE) {
				wxLogDebug(wxT("  -> HAVE_REV %d %d"), docId, revId);
				
				m_out->Write("HAVE_REV ", 9);
				cxLOCK_READ(m_catalyst)
					catalyst.WriteDocRevId(*m_out, di);
				cxENDLOCK
				m_out->Write("\n", 1);
			}
			else {
				wxLogDebug(wxT("  -> QUE_ACK_REV %d %d"), docId, revId);
				const wxString req = wxString::Format(wxT("QUE_ACK_REV %u %d"), docId, revId);
				m_requestQue.Add(req);
			}

			// Since we already have the revision, we have to update
			// it's share status.to reflect that this user also has it
			cxLOCK_WRITE(m_catalyst)
				if (m_userId) catalyst.UpdateShareState(di, m_userId);
			cxENDLOCK
		}
	}

	return false; // should newer end here
}

bool SyncThread::Update() {
	wxASSERT(m_userId != 0);

	// Send updates for each subscribed document
	vector<unsigned int> subsList;
	cxLOCK_READ(m_catalyst)
		catalyst.GetUserSubscriptions(m_userId, subsList);
	cxENDLOCK
	for (vector<unsigned int>::const_iterator i = subsList.begin(); i != subsList.end(); ++i) {
		// Check if there are any incomming requests from client
		if (!Process(false)) return false;

		if (!UpdateDoc(*i)) return false;
	}

	return true;
}

bool SyncThread::UpdateDoc(unsigned int docId) {
	//wxASSERT(m_catalyst.IsValidDoc(docId));

	const doc_id root(DOCUMENT, docId, 0);

	// First we have to establish which revisions (if any) the
	// receiver already has.
	// 
	// To do this we do a depth-first traversal of the history
	// tree, sending the id's of the revision as we back out of
	// them. Whenever we receive a confirmation that the
	// receiver has a revision we also know that he has all
	// of it's parent revisions.
	{
		m_out->Write("UPDATE ", 7);
		cxLOCK_READ(m_catalyst)
			catalyst.WriteDocId(*m_out, docId);
		cxENDLOCK
		m_out->Write("\n", 1);

		SendRevIdTraversal(root);

		m_out->Write("END_UPDATE\n", 11);
	}

	// Send the missing revisions
	return SendRevisionTraversal(root);
}

bool SyncThread::SendRevIdTraversal(const doc_id& di) {
	wxASSERT(di.IsDocument());
	//wxASSERT(m_catalyst.IsOk(di));

	// Check if we need to proceed with the inquery.
	// if any state flags are set it means that we either should
	// send the entire doc or none at all
	//   o cxSTATE_NEW: Reciever does not have any part of doc.
	//   o cxSTATE_UNSUBSCRIBED: Reciever no longer wants the doc.
	if (m_stateDocs.find(di.document_id) != m_stateDocs.end()) return true;
	
	// Recurse into children
	vector<unsigned int> revList;
	cxLOCK_READ(m_catalyst)
		catalyst.GetShareableChildList(di, revList);
	cxENDLOCK

	wxLogDebug(wxT("  IdTraversal. %d/%d childs: %d"), di.document_id, di.version_id, revList.size());

	bool haveRev = false;
	for (unsigned int i = 0; i < revList.size(); ++i) {
		const doc_id childId(DOCUMENT, di.document_id, revList[i]);

		if (SendRevIdTraversal(childId)) {
			// If reciever has any of the children, then he also has all parents
			haveRev = true;
		}
	}

	if (!haveRev) {
		// Check if there is any feedback (async)
		// Has to be passive because we are in the middle of an Update
		if (!Process(false, cxPASSIVE)) {
			wxLogDebug(wxT("  (process error)"));
			return true;
		}

		// Check if the receiver have indicated that he has this rev
		const set<unsigned int>& revSet = m_stateRevs[di.document_id];
		if (revSet.find(di.version_id) != revSet.end()) {
			wxLogDebug(wxT("  (haverev %d/%d)"), di.document_id, di.version_id);
			return true; // haveRev
		}

		// Send the revision signature
		wxLogDebug(wxT("  (revsig %d/%d)"), di.document_id, di.version_id);
		cxLOCK_READ(m_catalyst)
			catalyst.WriteRevId(*m_out, di);
		cxENDLOCK
		m_out->Write("\n", 1);

		return false;
	}
	else return true;
}

bool SyncThread::SendRevisionTraversal(const doc_id& di) {
	wxASSERT(di.IsDocument());
	//wxASSERT(m_catalyst.IsOk(di));

	// Check if there is any feedback (async)
	if (!Process(false)) return false;
	
	// Check if we need to proceed with the transfer
	cxDocStateMap::const_iterator p = m_stateDocs.find(di.document_id);
	if (p != m_stateDocs.end() && p->second == cxSHARE_UNSUBSCRIBED) return true;
		
	// Only send revision if receiver does not already have it
	const set<unsigned int>& revSet = m_stateRevs[di.document_id];
	if (revSet.find(di.version_id) == revSet.end()) {
		wxLogDebug(wxT("Sending: %d"), di.version_id);
		SendRevision(di);
	}

	// Send child revisions
	vector<unsigned int> revList;
	cxLOCK_READ(m_catalyst)
		catalyst.GetShareableChildList(di, revList);
	cxENDLOCK
	for (unsigned int i = 0; i < revList.size(); ++i) {
		const doc_id childId(DOCUMENT, di.document_id, revList[i]);

		if (!SendRevisionTraversal(childId)) return false;
	}

	return true;
}

bool SyncThread::SendRevision(const doc_id& di) const {
	wxASSERT(di.IsDocument());
	//wxASSERT(m_catalyst.IsOk(di));

	// DEBUG
	/*{
		wxFFileOutputStream out_file(wxT("out_stream"), wxT("a+b"));
		out_file.Write("REV ", 4);
		cxLOCK_READ(m_catalyst)
			catalyst.WriteDocRevId(out_file, di);
			out_file.Write("\n", 1);
			catalyst.WriteRevision(out_file, di);
		cxENDLOCK
		
		out_file.Write("END_REV\n", 8);
	}*/
	
	m_out->Write("REV ", 4);
	cxLOCK_READ(m_catalyst)
		catalyst.WriteDocRevId(*m_out, di);
		m_out->Write("\n", 1);
		catalyst.WriteRevision(*m_out, di);
	cxENDLOCK
	
	m_out->Write("END_REV\n", 8);

	return true;
}

bool SyncThread::GetUser(c4_Row& rUser) {
	wxString buf;
	if (!ReadLine(buf)) return false;
	wxStringTokenizer tokens(buf ,wxT(' '));

	wxString type = tokens.GetNextToken();

	// Date
	if (type == wxT("D")) {
		const wxString date = tokens.GetNextToken();
		wxLongLong llDate;
		if (!ParseHex64(date, llDate)) return false;

		pDate(rUser) = llDate.GetValue();
		
		if (!ReadLine(buf)) return false;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else return false; // mandatory
	
	// Name
	if (type == wxT("N")) {
		// The name is everything after the first space
		// it can itself contain spaces
		const wxString name = buf.AfterFirst(wxT(' '));
		if (name.empty()) return false;

		pUserName(rUser) = name.mb_str(wxConvUTF8);

		if (!ReadLine(buf)) return false;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else return false; // mandatory

	// Picture (optional)
	if (type == wxT("P")) {
		// Get the length
		wxString lenStr = tokens.GetNextToken();
		unsigned int len = wxAtoi(lenStr);

		c4_Bytes picBytes;
		if (!ReadBytes(picBytes, len)) return false;
		pUserPic(rUser) = picBytes;

		// End of picture data is marked by a single newline
		if (!ReadLine(buf) || !buf.empty()) return false;

		// End of profile
		if (!ReadLine(buf)) return false;
	}

	// End of user profile
	if (!buf.empty()) return false;

	return true;
}

int SyncThread::GetRevision(c4_Row& rRev, cxProcessMode mode) {
	wxString buf;
	if (!ReadLine(buf)) return 1;
	wxStringTokenizer tokens(buf ,wxT(' '));

	wxString type = tokens.GetNextToken();
	
	// DocumentID (often ommited since it is known from context)
	if (type == wxT("D")) {
		const wxString doc = tokens.GetNextToken();
		c4_Bytes docBytes;
		if (!ParseHex(doc, docBytes)) {
			wxASSERT(false);
			return 2;
		}

		if (pDocBytes(rRev) != docBytes) return 3;
		
		if (!ReadLine(buf)) return 4;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	}

	// Author
	if (type == wxT("A")) {
		const wxString author = tokens.GetNextToken();
		c4_Bytes authorBytes;
		if (!ParseUserId(author, authorBytes, mode)) return 5;

		// Archive author
		pAuthorBytes(rRev) = authorBytes;

		if (!ReadLine(buf)) return 6;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else return 7; // mandatory

	// Date
	if (type == wxT("T")) {
		const wxString date = tokens.GetNextToken();
		wxLongLong llDate;
		if (!ParseHex64(date, llDate)) return 8;

		pDate(rRev) = llDate.GetValue();
		
		if (!ReadLine(buf)) return 9;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else return 10; // mandatory

	// Timezone
	if (type == wxT("Z")) {
		const wxString timezone = tokens.GetNextToken();
		pTimezone(rRev) = wxAtol(timezone);
		
		if (!ReadLine(buf)) return 11;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else return 12; // mandatory

	// Parent
	if (type == wxT("P")) {
		const wxString parent = tokens.GetNextToken();
		c4_Bytes parentBytes;
		if (!ParseHex(parent, parentBytes)) return 13;

		// Archive parent
		pParentBytes(rRev) = parentBytes;
		
		if (!ReadLine(buf)) return 14;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	}

	// Label
	if (type == wxT("L")) {
		const wxString labelNode = tokens.GetNextToken();
		pLabelNode(rRev) = wxAtoi(labelNode);
		
		if (!ReadLine(buf)) return 15;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else pLabelNode(rRev) = -1;

	// Comment/Description
	if (type == wxT("C")) {
		const wxString descNode = tokens.GetNextToken();
		pDescNode(rRev) = wxAtoi(descNode);
		
		if (!ReadLine(buf)) return 16;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else pDescNode(rRev) = -1;

	// Properties
	if (type == wxT("R")) {
		const wxString str_propNode = tokens.GetNextToken();
		
		c4_Bytes revBytes;
		int node_id;
		if (!ParseNodeid(str_propNode, revBytes, node_id)) return 17;
		pPropBytes(rRev) = revBytes;
		pPropnode(rRev) = node_id;
		
		if (!ReadLine(buf)) return 18;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else {
		pPropVer(rRev) = -1;
		pPropnode(rRev) = -1;
	}

	// Data
	if (type == wxT("X")) {
		const wxString str_headNode = tokens.GetNextToken();
		
		c4_Bytes revBytes;
		int node_id;
		if (!ParseNodeid(str_headNode, revBytes, node_id)) return 19;
		pHeadBytes(rRev) = revBytes;
		pHeadnode(rRev) = node_id;
		
		if (!ReadLine(buf)) return 20;
		tokens.SetString(buf ,wxT(' '));
		type = tokens.GetNextToken();
	} else return 21; // mandatory

	// End of header
	if (!type.empty() || tokens.HasMoreTokens()) return 22;

	// Get nodes (they are in sequential order starting from 0)
	wxString id;
	c4_View vNodes;
	for (;;) {
		if (!ReadLine(buf)) return 23;
		tokens.SetString(buf ,wxT(' '));
		id = tokens.GetNextToken();

		// Check if we have recieved the last node
		if (id == wxT("END_REV")) break;

		// Verify the id
		int node_id = wxAtoi(id);
		if (node_id != vNodes.GetSize()) return 24;
		vNodes.SetSize(node_id+1);
		c4_RowRef rNode = vNodes[node_id];

		// Parse the type/datatype field
		type = tokens.GetNextToken();
		if (type.size() != 2) return 25;

		cxNodeType nodeType;
		switch (type[0]) {
		case wxChar('N'):
			nodeType = TYPE_NODE;
			break;
		case wxChar('D'):
			nodeType = TYPE_DATA;
			break;
		case wxChar('P'):
			nodeType = TYPE_PART;
			break;
		default:
			return 26;
		}
		pType(rNode) = nodeType;

		data_type dataType;
		switch (type[1]) {
		case wxChar('T'):
			dataType = DATA_TEXT;
			break;
		case wxChar('L'):
			dataType = DATA_LIST;
			break;
		case wxChar('B'):
			if (nodeType != TYPE_DATA) return 27;
			dataType = DATA_BOOLEAN;
			break;
		case wxChar('N'):
			if (nodeType != TYPE_DATA) return 28;
			dataType = DATA_NUMBER;
			break;
		default:
			return 29;
		}
		pDatatype(rNode) = dataType;

		// Get the length
		wxString lenStr = tokens.GetNextToken();
		unsigned int len;
		if (dataType == DATA_BOOLEAN) {
			if (lenStr == wxT("T")) len = 1;
			else if (lenStr == wxT("F")) len = 0;
			else return 30;
		}
		else {
			len = wxAtoi(lenStr);
		}
		pLength(rNode) = len;

		// Parse node contents
		if (nodeType == TYPE_NODE || (dataType == DATA_LIST && nodeType == TYPE_DATA)) {
			c4_View vNoderefs;
			vNoderefs.SetSize(len);
			for (unsigned int i = 0; i < len; ++i) {
				if (!ReadLine(buf)) return 31;
				tokens.SetString(buf ,wxT(' '));
				const wxString nodeRef = tokens.GetNextToken();

				c4_Bytes revBytes;
				int node_id;
				if (!ParseNodeid(nodeRef, revBytes, node_id)) return 32;
				
				c4_RowRef rNref = vNoderefs[i];
				pVerBytes(rNref) = revBytes;
				pNodeid(rNref) = node_id;

				if (nodeType == TYPE_NODE) {
					const wxString maxStr = tokens.GetNextToken();
					const int max = wxAtoi(maxStr);
				
					pMax(rNref) = max;
				}
			}
			pNoderefs(rNode) = vNoderefs;
		}
		else if (nodeType == TYPE_PART) {
			if (!ReadLine(buf)) return 33;
			tokens.SetString(buf ,wxT(' '));
			const wxString nodeRef = tokens.GetNextToken();

			c4_Row rNref;
			c4_Bytes revBytes;
			int node_id;
			if (!ParseNodeid(nodeRef, revBytes, node_id)) return 34;
			if (revBytes.Size() == 0) {
				wxASSERT(false);
				return 35; // Part always refs other revision
			}
			pVerBytes(rNref) = revBytes;
			pNodeid(rNref) = node_id;

			const wxString offsetStr = tokens.GetNextToken();
			pMax(rNref) = wxAtoi(offsetStr);

			c4_View vNoderefs;
			wxASSERT(vNoderefs.GetSize() == 0);
			vNoderefs.Add(rNref);
			pNoderefs(rNode) = vNoderefs;
		}
		else if (nodeType == TYPE_DATA) {
			c4_Bytes bData;
			if (!ReadBytes(bData, len)) return 36;
			pData(rNode) = bData;
		}
		else return 37;

		// End of node
		if (!ReadLine(buf) || !buf.empty()) {
			wxASSERT(false);
			return 38;
		}
	}
	pNodes(rRev) = vNodes;

	return 0; // ok
}

bool SyncThread::ReadBytes(c4_Bytes& bytes, unsigned int len) {
	unsigned char* data = bytes.SetBuffer(len);
	unsigned int count = 0;

	while (count < len && m_socket->WaitForRead()) {
		m_socket->Read(data, len-count);
		if (m_socket->Error()) {
			const wxSocketError err =  m_socket->LastError();
			
			if (err != wxSOCKET_WOULDBLOCK) {
				wxLogDebug(wxT("ReadBytes error: %d\n"), err);
				return false;
			}
		}

		const unsigned int lastCount = m_socket->LastCount(); 
		count += lastCount;
		data += lastCount;
	}
	
	return (count == len);
}

bool SyncThread::ReadLine(wxString& result)
{
    static const int LINE_BUF = 4095;

    result.clear();

    unsigned int len = 0;
	wxCharBuffer buf(LINE_BUF);
    char * const pBuf = buf.data();
    while ( m_socket->WaitForRead() )
    {
 		// peek at the socket to see if there is a LF
        const wxSocketFlags flagbuf = m_socket->GetFlags();
		m_socket->SetFlags(wxSOCKET_NONE | wxSOCKET_BLOCK);
		m_socket->Peek(pBuf, LINE_BUF);
		m_socket->SetFlags(flagbuf);

        size_t nRead = m_socket->LastCount();
        if ( !nRead && m_socket->Error() ) {
			const wxSocketError err =  m_socket->LastError();
			
			if (err != wxSOCKET_WOULDBLOCK) {
				wxLogDebug(wxT("Readline error: %d\n"), err);
				return false;
			}
		}

        pBuf[nRead] = '\0';
        const char *eol = strchr(pBuf, '\n');

        if ( eol )
        {
            // read everything up to and including '\n'
            nRead = eol - pBuf + 1;
        }

        m_socket->Read(pBuf, nRead);
        if ( m_socket->LastCount() != nRead )
            return false;
		len += nRead;

        pBuf[nRead] = '\0';
        result += wxString(pBuf, wxConvUTF8);

        if ( eol )
        {
			// remove trailing "\n"
            result.RemoveLast(1);

            wxLogDebug(result);
			return true;
        }
    }

    return false;
}

bool SyncThread::ParseUserId(const wxString& in, c4_Bytes& userBytes, cxProcessMode mode) {
	// Parse the user sig
	const wxString user = in.BeforeFirst(wxT('/'));
	if (!ParseHex(user, userBytes)) {
		wxASSERT(false);
		return false;
	}

	// Check if we know the author
	cxLOCK_READ(m_catalyst)
		int userId = -1;
		if (catalyst.FindUser(userBytes, userId)) {
			// Parse the (optional) last-modified date
			const wxString dateStr = in.AfterFirst(wxT('/'));
			if (!dateStr.empty()) {
				wxLongLong llDate;
				if (!ParseHex64(dateStr, llDate)) return false;

				if (!catalyst.IsUserModified(userId, llDate)) return true;
			}
			else return true;
		}
	cxENDLOCK
	
	// If the user is unknown or has been modified we have
	// to get the full profile
	if (mode == cxACTIVE) {
		// We have to request the author info
		m_out->Write("GET_USER ", 9);
		Catalyst::PrintBin(*m_out, userBytes);
		m_out->Write("\n", 1);
	}
	else {
		// Que for retrieval when leaving passive mode
		const wxString req = wxT("QUE_REQ_USER ") + user;
		m_requestQue.Add(req);
	}

	return true;
}

bool SyncThread::ParseDocRev(const wxString& in, c4_Bytes& docBytes, c4_Bytes& revBytes) { // static
	if (in.empty()) return false;

	// Parse the (optional) doc part
	const wxString doc = in.BeforeLast(wxT('/'));
	if (!doc.empty()) {
		if (!ParseHex(doc, docBytes)) return false;

		// Parse the node id 
		const wxString rev = in.AfterLast(wxT('/'));
		if (!ParseHex(rev, revBytes)) return false;
	}
	else if (!ParseHex(in, revBytes)) return false;

	return true;
}

bool SyncThread::ParseNodeid(const wxString& in, c4_Bytes& revBytes, int& node_id) { // static
	if (in.empty()) return false;

	// Parse the (optional) revision part of the node_id
	const wxString rev = in.BeforeLast(wxT('/'));
	if (!rev.empty()) {
		if (!ParseHex(rev, revBytes)) return false;

		// Parse the node id 
		const wxString nodeid = in.AfterLast(wxT('/'));
		node_id = wxAtoi(nodeid);
	}
	else node_id = wxAtoi(in);

	return true;
}

bool SyncThread::ParseHex(const wxString& in, c4_Bytes& bytes) { // static
	const unsigned int len = in.size();
	if (len % 2 != 0) {
		wxASSERT(false);
		return false; 
	}

	unsigned char* buf = bytes.SetBuffer(len / 2);

	unsigned int in_ndx = 0;
	unsigned int out_ndx = 0;
	while (in_ndx < len) {
		const int n1 = HexToNumber(in[in_ndx++]);
		const int n2 = HexToNumber(in[in_ndx++]);
		if (n1 < 0 || n2 < 0) {
			wxASSERT(false);
			return false;
		}

		const unsigned char c = (n1 << 4) | n2;
		buf[out_ndx++] = c;
	}

	return true;
}

bool SyncThread::ParseHex64(wxString in, wxLongLong& ll) { // static
	if (in.size() > 16) return false;
	if (in.size() < 16) in.Pad(16 - in.size(), wxChar('0'), false);

	wxLongLong_t ll_t;
	unsigned char* pll_t = (unsigned char*)&ll_t; 

	unsigned int in_ndx = 0;
	while (in_ndx < 16) {
		const int n1 = HexToNumber(in[in_ndx++]);
		const int n2 = HexToNumber(in[in_ndx++]);
		if (n1 < 0 || n2 < 0) return false;

		const unsigned char c = (n1 << 4) | n2;
		*(pll_t++) = c;
	}

	// 64bit integers are always serialized in big-endian order
	ll = wxUINT64_SWAP_ON_LE(ll_t);

	return true;
}


int SyncThread::HexToNumber(wxChar hex) { // static
	if (hex >= wxChar('0') && hex <= wxChar('9')) return hex - wxChar('0');
	if (hex >= wxChar('a') && hex <= wxChar('f')) return hex - wxChar('a') + 10;

	return -1; // invalid hex character
}

#endif // FEAT_COLLABORATION
