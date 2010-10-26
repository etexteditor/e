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

#ifndef __APIHANDLER_H__
#define __APIHANDLER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include "Catalyst.h"

#include <vector>
#include <map>
#include <set>
#include <boost/ptr_container/ptr_map.hpp>
using namespace std;

// Pre-declarations
class eApp;
class eIpcThread;
class IConnection;
class EditorCtrl;

class ApiHandler : public wxEvtHandler {
public:
	ApiHandler(eApp& app);
	~ApiHandler();

	// Ipc notifications
	void OnInputLineChanged(unsigned int nid, const wxString& text);
	void OnInputLineClosed(unsigned int nid);
	void OnEditorTab(int editorId);

private:
	struct ConnectionState {
		vector<doc_id> docHandles;
		set<int> editorsInChange;
	};

	// Event handlers
	void OnIdle(wxIdleEvent& event);
	void OnIpcCall(wxCommandEvent& event);
	void OnIpcClosed(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	ConnectionState& GetConnState(IConnection& conn);
	unsigned int GetNextNotifierId() {return m_ipcNextNotifierId++;};
	
	// Command handlers
	void IpcGetActiveEditor(IConnection& conn);
	void IpcIsSoftTabs(IConnection& conn);
	void IpcGetTabWidth(IConnection& conn);
	void IpcLog(IConnection& conn);
	void IpcReloadBundles(IConnection& conn);
	void IpcEditorPrompt(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetVersionId(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetLength(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetScope(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetPos(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetText(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetCurrentLine(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetLineText(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetLineRange(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetSelections(EditorCtrl& editor, IConnection& conn);
	void IpcEditorSelect(EditorCtrl& editor, IConnection& conn);
	void IpcEditorInsertTabStops(EditorCtrl& editor, IConnection& conn);
	void IpcEditorInsertAt(EditorCtrl& editor, IConnection& conn);
	void IpcEditorDeleteRange(EditorCtrl& editor, IConnection& conn);
	void IpcEditorShowCompletions(EditorCtrl& editor, IConnection& conn);
	void IpcEditorShowInputLine(EditorCtrl& editor, IConnection& conn);
	void IpcEditorWatchTab(EditorCtrl& editor, IConnection& conn);
	void IpcEditorWatchChanges(EditorCtrl& editor, IConnection& conn);
	void IpcEditorGetChangesSince(EditorCtrl& editor, IConnection& conn);
	void IpcEditorStartChange(EditorCtrl& editor, IConnection& conn);
	void IpcEditorEndChange(EditorCtrl& editor, IConnection& conn);

	// Notification Handlers
	void OnEditorChanged(unsigned int nid, bool state);

	// Member variables
	eApp& m_app;
	eIpcThread* m_ipcThread;

	enum WatchType {
		WATCH_EDITOR_CHANGE,
		WATCH_EDITOR_TAB
	};
	struct EditorWatch {
		WatchType type;
		int editorId;
		unsigned int changeToken;
		unsigned int notifierId;
	};
	typedef void (ApiHandler::* PIpcFun)(IConnection& conn);
	typedef void (ApiHandler::* PIpcEdFun)(EditorCtrl& ed, IConnection& conn);
	map<string, PIpcFun> m_ipcFunctions;
	map<string, PIpcEdFun> m_ipcEditorFunctions;
	map<unsigned int, IConnection*> m_notifiers;
	unsigned int m_ipcNextNotifierId;
	boost::ptr_map<IConnection*, ConnectionState> m_connStates;
	vector<EditorWatch> m_editorWatchers;
};

#endif //__APIHANDLER_H__
