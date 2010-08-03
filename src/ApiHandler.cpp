/*******************************************************************************
 *
 * Copyright (C) 2010, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#include "ApiHandler.h"
#include "eApp.h"
#include "EditorFrame.h"
#include "EditorCtrl.h"
#include "IConnection.h"
#include "eIpcThread.h"
#include "WindowEnabler.h"
#include "tm_syntaxhandler.h"

BEGIN_EVENT_TABLE(ApiHandler, wxEvtHandler)
	EVT_IDLE(ApiHandler::OnIdle)
	EVT_COMMAND(wxID_ANY, wxEVT_IPC_CALL, ApiHandler::OnIpcCall)
	EVT_COMMAND(wxID_ANY, wxEVT_IPC_CLOSE, ApiHandler::OnIpcClosed)
END_EVENT_TABLE()


ApiHandler::ApiHandler(eApp& app)
: m_app(app), m_ipcThread(NULL), m_ipcNextNotifierId(0) {
	// Register method handlers
	m_ipcFunctions["GetActiveEditor"] = &ApiHandler::IpcGetActiveEditor;
	m_ipcFunctions["IsSoftTabs"] = &ApiHandler::IpcIsSoftTabs;
	m_ipcFunctions["GetTabWidth"] = &ApiHandler::IpcGetTabWidth;
	m_ipcFunctions["Log"] = &ApiHandler::IpcLog;
	m_ipcFunctions["ReloadBundles"] = &ApiHandler::IpcReloadBundles;

	// Register method handlers for editor
	m_ipcEditorFunctions["Prompt"] = &ApiHandler::IpcEditorPrompt;
	m_ipcEditorFunctions["InsertAt"] = &ApiHandler::IpcEditorInsertAt;
	m_ipcEditorFunctions["InsertTabStops"] = &ApiHandler::IpcEditorInsertTabStops;
	m_ipcEditorFunctions["DeleteRange"] = &ApiHandler::IpcEditorDeleteRange;
	m_ipcEditorFunctions["GetLength"] = &ApiHandler::IpcEditorGetLength;
	m_ipcEditorFunctions["GetText"] = &ApiHandler::IpcEditorGetText;
	m_ipcEditorFunctions["GetLineText"] = &ApiHandler::IpcEditorGetLineText;
	m_ipcEditorFunctions["GetLineRange"] = &ApiHandler::IpcEditorGetLineRange;
	m_ipcEditorFunctions["GetCurrentLine"] = &ApiHandler::IpcEditorGetCurrentLine;
	m_ipcEditorFunctions["GetScope"] = &ApiHandler::IpcEditorGetScope;
	m_ipcEditorFunctions["GetPos"] = &ApiHandler::IpcEditorGetPos;
	m_ipcEditorFunctions["GetVersionId"] = &ApiHandler::IpcEditorGetVersionId;
	m_ipcEditorFunctions["GetSelections"] = &ApiHandler::IpcEditorGetSelections;
	m_ipcEditorFunctions["Select"] = &ApiHandler::IpcEditorSelect;
	m_ipcEditorFunctions["ShowCompletions"] = &ApiHandler::IpcEditorShowCompletions;
	m_ipcEditorFunctions["GetChangesSince"] = &ApiHandler::IpcEditorGetChangesSince;
	m_ipcEditorFunctions["ShowInputLine"] = &ApiHandler::IpcEditorShowInputLine;
	m_ipcEditorFunctions["WatchChanges"] = &ApiHandler::IpcEditorWatchChanges;

	// Start the ipc server
	m_ipcThread = new eIpcThread(*this);
}

ApiHandler::~ApiHandler() {
	if (m_ipcThread) m_ipcThread->stop(); // deletes self on completion
}

void ApiHandler::OnIpcCall(wxCommandEvent& event) {
	IConnection* conn = (IConnection*)event.GetClientData();
	if (!conn) return;

	const hessian_ipc::Call* call = conn->get_call();
	hessian_ipc::Writer& writer = conn->get_reply_writer();
	if (!call) return;

	const string& m = call->GetMethod();
	const wxString method(m.c_str(), wxConvUTF8, m.size());

	wxLogDebug(wxT("IPC: %s"), method);

	// Call the function (if it exists)
	bool methodFound = true;
	if (call->IsObjectCall()) {
		const hessian_ipc::Call& call = *conn->get_call();
		const hessian_ipc::Value& v1 = call.GetParameter(0);
		const int editorId = -v1.GetInt();

		EditorCtrl* editor = m_app.GetEditorCtrl(editorId);
		if (!editor) {
			writer.write_fault(hessian_ipc::NoSuchObjectException, "Unknown object");
			conn->reply_done();
			return;
		}

		map<string, PIpcEdFun>::const_iterator p = m_ipcEditorFunctions.find(m.c_str());
		if (p != m_ipcEditorFunctions.end()) {
			(this->*p->second)(*editor, *conn);
		}
		else {
			eMacroCmd cmd(method);
			for (size_t i = 1; i < call.GetParameterCount(); ++i) {
				const hessian_ipc::Value& v = call.GetParameter(i);

				if (v.IsBoolean()) cmd.AddArg(v.GetBoolean());
				else if (v.IsInt()) cmd.AddArg(v.GetInt());
				else if (v.IsString()) {
					const string& arg = v.GetString();
					const wxString argstr(arg.c_str(), wxConvUTF8, arg.size());
					cmd.AddArg(argstr);
				}
				else wxASSERT(false);
			}

			const wxVariant reply = editor->PlayCommand(cmd);
			if (reply.GetName() == wxT("Unknown method")) methodFound = false;
			else editor->ReDraw();

			//TODO: write variant
			writer.write_reply_null();
		}
	}
	else {
		map<string, PIpcFun>::const_iterator p = m_ipcFunctions.find(m.c_str());
		if (p != m_ipcFunctions.end()) (this->*p->second)(*conn);
		else methodFound = false;
	}

	if (!methodFound) {
		writer.write_fault(hessian_ipc::NoSuchMethodException, "Unknown method");
	}

	// Notify connection that it can send the reply (threadsafe)
	conn->reply_done();
}

ApiHandler::ConnectionState& ApiHandler::GetConnState(IConnection& conn) {
	boost::ptr_map<IConnection*,ConnectionState>::iterator p = m_connStates.find(&conn);
	if (p == m_connStates.end()) {
		IConnection* c = &conn;
		m_connStates.insert(c, new ConnectionState);
		p = m_connStates.find(&conn);
	}
	
	return *p->second;
}

void ApiHandler::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	// Do we have ipc connections watching for editor changes
	vector<EditorWatch>::iterator p = m_editorWatchers.begin();
	while (p != m_editorWatchers.end()) {
		if (p->type == WATCH_EDITOR_CHANGE) {
			EditorCtrl* editor = m_app.GetEditorCtrl(p->editorId);
			if (!editor){
				// the editor has been closed
				OnEditorChanged(p->notifierId, false); // Send notification
				m_notifiers.erase(p->notifierId);
				p = m_editorWatchers.erase(p); // remove entry	
				continue; 
			}

			const unsigned int token = editor->GetChangeToken();
			if (token != p->changeToken) {
				wxLogDebug(wxT("token: %d"), token);
				OnEditorChanged(p->notifierId, true); // Send notification
				p->changeToken = token;
			}
		}
		++p;
	}
}


void ApiHandler::OnIpcClosed(wxCommandEvent& event) {
	IConnection* conn = (IConnection*)event.GetClientData();
	if (!conn) return;

	// Remove all notifiers from closed connection
	map<unsigned int, IConnection*>::iterator p = m_notifiers.begin();
	while (p != m_notifiers.end()) {
		if (p->second == conn) p = m_notifiers.erase(p);
		else ++p;
	}

	// Clear any associated state
	m_connStates.erase(conn);
}


void ApiHandler::IpcGetActiveEditor(IConnection& conn) {
	EditorCtrl* editor = m_app.GetActiveEditorCtrl();
	const int id = -editor->GetId();

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply_handle(id);
}

void ApiHandler::IpcIsSoftTabs(IConnection& conn) {
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(m_app.GetTopFrame()->IsSoftTabs());
}

void ApiHandler::IpcGetTabWidth(IConnection& conn) {
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(m_app.GetTopFrame()->GetTabWidth());
}

void ApiHandler::IpcLog(IConnection& conn) {
	// Get the text to insert
	const hessian_ipc::Call& call = *conn.get_call();
	const hessian_ipc::Value& v = call.GetParameter(0);
	const string& t = v.GetString();
	const wxString text(t.c_str(), wxConvUTF8, t.size());

	wxLogDebug(text);
}

void ApiHandler::IpcReloadBundles(IConnection& conn) {
	m_app.GetSyntaxHandler().LoadBundles(cxUPDATE);

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply_null();
}

void ApiHandler::IpcEditorPrompt(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();

	// Get the title
	const hessian_ipc::Value& v2 = call.GetParameter(1);
	const string& t = v2.GetString();
	const wxString title(t.c_str(), wxConvUTF8, t.size());

	// Show Prompt
	WindowEnabler we; // otherwise dlg wont be able to return focus
	const wxString text = wxGetTextFromUser(title, _("Input text"), wxEmptyString, &editor);
	
	const wxCharBuffer str = text.ToUTF8();

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(str.data());
}

void ApiHandler::IpcEditorGetVersionId(EditorCtrl& editor, IConnection& conn) {
	const doc_id id = editor.GetLastStableDocID();

	// create handle for doc_id
	ConnectionState& connState = GetConnState(conn);
	const size_t handle = connState.docHandles.size();
	connState.docHandles.push_back(id);

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(handle);
}

void ApiHandler::IpcEditorGetText(EditorCtrl& editor, IConnection& conn) {
	vector<char> text;
	editor.GetText(text);

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(text);
}

void ApiHandler::IpcEditorGetLineText(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();
	const unsigned int lineid = call.GetParameter(1).GetInt();
	if (lineid > editor.GetLineCount()) return; // fault

	vector<char> text;
	editor.GetLine(lineid, text);

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(text);
}

void ApiHandler::IpcEditorGetLineRange(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();

	const unsigned int lineid = call.GetParameter(1).GetInt();
	if (lineid > editor.GetLineCount()) return; // fault

	const interval iv = editor.GetLineExtent(lineid);

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(iv);
}

void ApiHandler::IpcEditorGetCurrentLine(EditorCtrl& editor, IConnection& conn) {
	const unsigned int line_id = editor.GetCurrentLineNumber()-1;

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(line_id);
}

void ApiHandler::IpcEditorGetPos(EditorCtrl& editor, IConnection& conn) {
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(editor.GetPos());
}

void ApiHandler::IpcEditorGetLength(EditorCtrl& editor, IConnection& conn) {
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(editor.GetLength());
}

void ApiHandler::IpcEditorGetScope(EditorCtrl& editor, IConnection& conn) {
	const deque<const wxString*> s = editor.GetScope();
	vector<string> scopes;
	for (deque<const wxString*>::const_iterator p = s.begin(); p != s.end(); ++p) {
		scopes.push_back((*p)->ToUTF8().data());
	}

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(scopes);
}

void ApiHandler::IpcEditorGetSelections(EditorCtrl& editor, IConnection& conn) {
	const vector<interval>& sel = editor.GetSelections();
	
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(sel);
}

void ApiHandler::IpcEditorSelect(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();
	const int v2 = call.GetParameter(1).GetInt();
	const int v3 = call.GetParameter(2).GetInt();

	if (v2 != v3) {
		editor.Select(v2, v3);
		editor.MakeSelectionVisible();
	}
	editor.SetPos(v3);
	editor.ReDraw();

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(true);
}

void ApiHandler::IpcEditorInsertTabStops(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();
	const hessian_ipc::Value& v2 = call.GetParameter(1);
	const hessian_ipc::List& tabstops = v2.AsList();

	if (!tabstops.empty()) {
		SnippetHandler& sh = editor.GetSnippetHandler();
		sh.Clear();
		for (size_t i = 0; i < tabstops.size(); ++i) {
			const hessian_ipc::List& t = tabstops.get(i).AsList();
			const interval iv(t.get(0).GetInt(), t.get(1).GetInt());
			const string& s = t.get(2).GetString();
			const wxString text(s.c_str(), wxConvUTF8, s.size());

			sh.AddTabStop(iv, text);
		}
		sh.NextTab();
		editor.ReDraw();
	}

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply_null();
}

void ApiHandler::IpcEditorShowCompletions(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();
	const hessian_ipc::Value& v2 = call.GetParameter(1);
	const hessian_ipc::List& completions = v2.AsList();

	if (!completions.empty()) {
		wxArrayString comp;
		for (size_t i = 0; i < completions.size(); ++i) {
			const string& s = completions.get(i).GetString();
			const wxString text(s.c_str(), wxConvUTF8, s.size());

			comp.push_back(text);
		}
		editor.ShowCompletionPopup(comp);
	}

	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply_null();
}

void ApiHandler::IpcEditorInsertAt(EditorCtrl& editor, IConnection& conn) {
	// Get the insert position
	const hessian_ipc::Call& call = *conn.get_call();
	const size_t pos = call.GetParameter(1).GetInt();
	if (pos > editor.GetLength()) return; // fault: invalid position

	// Get the text to insert
	const hessian_ipc::Value& v3 = call.GetParameter(2);
	const string& t = v3.GetString();
	const wxString text(t.c_str(), wxConvUTF8, t.size());
	
	// Insert the text
	// TODO: adjust selections
	const unsigned int cpos = editor.GetPos();
	const size_t byte_len = editor.RawInsert(pos, text);
	if (cpos >= pos) editor.SetPos(cpos + byte_len);
	editor.ReDraw();

	// Write the reply
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(byte_len);
}

void ApiHandler::IpcEditorDeleteRange(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();
	
	// Get the inset position
	const size_t start = call.GetParameter(1).GetInt();
	const size_t end = call.GetParameter(2).GetInt();
	if (end > editor.GetLength() || start > end) return; // fault: invalid positions

	// Delete the range
	const size_t byte_len = editor.RawDelete(start, end);

	// Write the reply
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(byte_len);
}

void ApiHandler::IpcEditorShowInputLine(EditorCtrl& , IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();

	// Get caption
	const hessian_ipc::Value& v2 = call.GetParameter(1);
	const string& t = v2.GetString();
	const wxString caption(t.c_str(), wxConvUTF8, t.size());

	// Register notifier
	const unsigned int notifier_id = GetNextNotifierId();
	m_notifiers[notifier_id] = &conn;

	// Show input line
	EditorFrame* frame = m_app.GetTopFrame();
	if (!frame) return;
	frame->ShowInputPanel(notifier_id, caption);

	// Return notifier id
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(notifier_id);
}

void ApiHandler::IpcEditorWatchTab(EditorCtrl& editor, IConnection& conn) {
	// Register notifier
	const unsigned int notifier_id = GetNextNotifierId();
	m_notifiers[notifier_id] = &conn;

	// Add to watch list
	const EditorWatch ew = {WATCH_EDITOR_TAB, editor.GetId(), 0, notifier_id};
	m_editorWatchers.push_back(ew);

	// Return notifier id
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(notifier_id);
}

void ApiHandler::IpcEditorWatchChanges(EditorCtrl& editor, IConnection& conn) {
	// Register notifier
	const unsigned int notifier_id = GetNextNotifierId();
	m_notifiers[notifier_id] = &conn;

	// Add to watch list
	const EditorWatch ew = {WATCH_EDITOR_CHANGE, editor.GetId(), editor.GetChangeToken(), notifier_id};
	m_editorWatchers.push_back(ew);

	// Return notifier id
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(notifier_id);
}

void ApiHandler::IpcEditorGetChangesSince(EditorCtrl& editor, IConnection& conn) {
	const hessian_ipc::Call& call = *conn.get_call();

	// Get the version handle
	const size_t versionHandle = call.GetParameter(1).GetInt();
	ConnectionState& connState = GetConnState(conn);
	if (versionHandle >= connState.docHandles.size()) return; // fault: invalid handle
	const doc_id& di = connState.docHandles[versionHandle];

	// Get diff
	vector<size_t> changedlines;
	editor.GetLinesChangedSince(di, changedlines);

	// Return changed lines
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_reply(changedlines);
}

void ApiHandler::OnInputLineChanged(unsigned int nid, const wxString& text) {
	// Look up notifier id
	map<unsigned int, IConnection*>::const_iterator p = m_notifiers.find(nid);
	if (p == m_notifiers.end()) return;
	IConnection& conn = *p->second;

	// Send notifier
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	const wxCharBuffer str = text.ToUTF8();
	writer.write_notifier(nid, str.data());

	conn.notifier_done();
}

void ApiHandler::OnInputLineClosed(unsigned int nid) {
	// Look up notifier id
	map<unsigned int, IConnection*>::const_iterator p = m_notifiers.find(nid);
	if (p == m_notifiers.end()) return;
	IConnection& conn = *p->second;

	// Remove notifier
	m_notifiers.erase(nid);

	// Notifier listener that the notifier has ended
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	writer.write_notifier_ended(nid);
	conn.notifier_done();
}

void ApiHandler::OnEditorTab(int) {
	// find watchers
	for (vector<EditorWatch>::iterator p = m_editorWatchers.begin(); p != m_editorWatchers.end(); ++p) {
		if (p->type != WATCH_EDITOR_TAB) continue;

		// Look up notifier id
		map<unsigned int, IConnection*>::const_iterator n = m_notifiers.find(p->notifierId);
		if (n == m_notifiers.end()) return;
		IConnection& conn = *n->second;

		// Send notifier
		hessian_ipc::Writer& writer = conn.get_reply_writer();
		writer.write_notifier(p->notifierId, true);  // true for change, false for close
		conn.notifier_done();
	}
}

void ApiHandler::OnEditorChanged(unsigned int nid, bool state) {
	// Look up notifier id
	map<unsigned int, IConnection*>::const_iterator p = m_notifiers.find(nid);
	if (p == m_notifiers.end()) return;
	IConnection& conn = *p->second;

	// Send notifier
	hessian_ipc::Writer& writer = conn.get_reply_writer();
	if (state) writer.write_notifier(nid, true);  // true for change, false for close
	else writer.write_notifier_ended(nid);

	conn.notifier_done();
}
