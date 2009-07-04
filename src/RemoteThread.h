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

#ifndef __REMOTETHREAD_H__
#define __REMOTETHREAD_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <curl/curl.h>

#include <deque>
#include <vector>

// pre-definitions
class cxRemoteAction;
class cxRemoteListEvent;
class TiXmlElement;

class RemoteProfile {
public:
	RemoteProfile() : m_id(-1) {};
	RemoteProfile(const RemoteProfile& rp);

	bool IsValid() const {return !m_address.empty();};
	bool IsTemp() const {return m_id == -1;};
	bool IsActive() const {return m_id != -2;};
	void DisActivate() {m_id = -2;};

	wxString GetUrl() const;
	wxString GetUsernamePwd() const;

	int m_id;
	wxString m_protocol;
	wxString m_name;
	wxString m_address;
	wxString m_dir;
	wxString m_username;
	wxString m_pwd;
};

class cxFileInfo {
public:
	cxFileInfo() : m_isDir(false) {};
	cxFileInfo(const cxFileInfo& fi) {
		m_isDir = fi.m_isDir;
		m_name = fi.m_name.c_str();
		m_modDate = fi.m_modDate;
		m_size = fi.m_size;
	};

	bool m_isDir;
	wxString m_name;
	wxDateTime m_modDate;
	size_t m_size;
};

enum cxActionType {
	cxRA_UPLOAD,
	cxRA_UPLOAD_AND_DATE,
	cxRA_DOWNLOAD,
	cxRA_LIST,
	cxRA_DELETE,
	cxRA_RENAME,
	cxRA_CREATEFILE,
	cxRA_MKDIR,
	cxRA_DATE,
	cxRA_DATE_LOCKING // only to be used by ChangeCheckerThread
};

class RemoteThread : public wxEvtHandler, public wxThread {
public:
	RemoteThread();
	virtual void* Entry();

	// Async commands (result comes as event)
	void GetRemoteList(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler);
	void Delete(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler);
	void CreateFile(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler);
	void MkDir(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler);
	void Rename(const wxString& url, const wxString& newname, const RemoteProfile& rp, wxEvtHandler& evtHandler);
	void UploadAsync(const wxString& url, const wxString& filePath, const RemoteProfile& rp, wxEvtHandler& evtHandler);
	void DownloadAsync(const wxString& url, const wxString& filePath, const RemoteProfile& rp, wxEvtHandler& evtHandler);

	// Sync commands (locks until result arrives)
	wxDateTime GetModDate(const wxString& url, const RemoteProfile& rp);
	CURLcode Download(const wxString& url, const wxString& buffPath, const RemoteProfile& rp);
	CURLcode UploadAndDate(const wxString& url, const wxString& buffPath, const RemoteProfile& rp);
	CURLcode GetRemoteListWait(const wxString& url, const RemoteProfile& rp, std::vector<cxFileInfo>& fiList);

	// Locking commands only to be used by ChangeCheckerThread
	wxDateTime GetModDateLocking(const wxString& url, const RemoteProfile& rp);

	void RemoveEventHandler(wxEvtHandler& evtHandler);

	CURLcode GetLastErrorCode() const {return m_lastErrorCode;};
	const wxString& GetLastError() const {return m_lastError;};
	wxString GetErrorText(CURLcode errorCode) const;

private:
	class RemoteAction {
	public:
		RemoteAction(cxActionType action, const wxString& url, const wxString& target, const RemoteProfile& rp, wxEvtHandler* evtHandler)
		: m_action(action), m_url(url.c_str()), m_login(rp.GetUsernamePwd().c_str()), m_target(target.c_str()), m_rp(rp), m_evtHandler(evtHandler) {};
		cxActionType m_action;
		wxString m_url;
		wxString m_login;
		wxString m_target;
		RemoteProfile m_rp;
		wxEvtHandler* m_evtHandler;
	};

	void AddAction(cxActionType action, const wxString& url, const wxString& target, const RemoteProfile& rp, wxEvtHandler* evtHandler);

	void GetList(const RemoteAction& ra);
	void DoDownload(const RemoteAction& ra);
	void DoUpload(const RemoteAction& ra);
	void DoGetDate(const RemoteAction& ra);
	void DoDelete(const RemoteAction& ra);
	void DoCreateFile(const RemoteAction& ra);
	void DoMakeDir(const RemoteAction& ra);
	void DoRename(const RemoteAction& ra);

	bool IsDir(const wxString& url, const RemoteAction& ra);
	CURLcode DoGetDir(const wxString& url, const RemoteAction& ra, std::vector<cxFileInfo>& fiList);
	CURLcode DoGetDirWebDav(const wxString& url, const RemoteAction& ra, std::vector<cxFileInfo>& fiList);
	CURLcode DoDeleteFile(const wxString& url, const RemoteAction& ra);
	CURLcode DoDeleteDir(const wxString& url, const RemoteAction& ra);
	CURLcode DoUploadDir(const wxString& url, const wxString& path, const RemoteAction& ra);
	CURLcode DoUploadFile(const wxString& url, const wxString& path, const RemoteAction& ra);
	CURLcode DoCreateDir(const wxString& url, const RemoteAction& ra);

	void WaitForEvent();
	void PostEvent(wxEvtHandler* evtHandler, wxEvent& event);

	static int WriteCallback(void *buffer, size_t size, size_t nmemb, void* data);
	static int WriteFileCallback(void *buffer, size_t size, size_t nmemb, void* data);
	static int ReadFileCallback(void *buffer, size_t size, size_t nmemb, void* data);
	static int ReadEmptyFileCallback(void *buffer, size_t size, size_t nmemb, void* data);
	static int CurlDebugCallback(void*, curl_infotype type, char * text, size_t len, void *);

	// WebDav xml parsers
	void ParseWebDavXml(const std::vector<char>& data, std::vector<cxFileInfo>& fiList) const;
	bool ParseResponseXml(const TiXmlElement* response, cxFileInfo& fi) const;

	wxString EscapeUrl(const wxString& url) const;

	CURLcode curl_use_proxy(CURL* curlHandle);

	// Event handlers
	void OnRemoteAction(cxRemoteAction& event);
	void OnRemoteListReceived(cxRemoteListEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	wxCriticalSection m_listCrit;
	std::deque<RemoteAction> m_actionList;
	wxEvtHandler* m_removedEvtHandler;

	std::vector<char> m_data;

	wxMutex m_condMutex;
	wxCondition m_newActionsCond;
	CURL* m_curlHandle;

	// Event state
	bool m_waitingForEvent;
	wxDateTime m_modDate;
	CURLcode m_lastErrorCode;
	wxString m_lastError;
	std::vector<cxFileInfo>* m_fiList;

	// Locked state
	// (only to be used by ChangeCheckerThread)
	bool m_isLocked;
	wxDateTime m_lockedModDate;
};

// Declare custom event
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_REMOTEACTION, 801)
	DECLARE_EVENT_TYPE(wxEVT_REMOTELIST_RECEIVED, 801)
END_DECLARE_EVENT_TYPES()

class cxRemoteAction : public wxEvent {
public:
	cxRemoteAction(int id = 0, wxEventType type=wxEVT_REMOTEACTION)
		: wxEvent(id, type), m_errorCode(CURLE_OK) {};
	cxRemoteAction(const cxRemoteAction& event) : wxEvent(event) {
		m_action = event.m_action;
		m_errorCode = event.m_errorCode;
		m_url = event.m_url.c_str(); // wxString is not threadsafe, so we force full copy to avoid COW 
		m_target = event.m_target.c_str();
		m_date = event.m_date;
	};
	virtual wxEvent* Clone() const {
		return new cxRemoteAction(*this);
	};

	bool Succeded() const {return m_errorCode == CURLE_OK;};
	void SetError(CURLcode errorCode) {m_errorCode = errorCode;};
	wxString GetError() const {return wxString(curl_easy_strerror(m_errorCode),wxConvUTF8);};
	CURLcode GetErrorCode() const {return m_errorCode;};

	void SetActionType(cxActionType action) {m_action = action;};
	void SetUrl(const wxString& url);
	void SetTarget(const wxString& target) {m_target = target;};
	void SetDate(const wxDateTime& date) {m_date = date;};
	cxActionType GetActionType() const {return m_action;};
	const wxString& GetUrl() const {return m_url;};
	const wxString& GetTarget() const {return m_target;};
	const wxDateTime& GetDate() const {return m_date;};

protected:
	cxActionType m_action;
	CURLcode m_errorCode;
	wxString m_url;
	wxString m_target;
	wxDateTime m_date;
};

class cxRemoteListEvent : public cxRemoteAction {
public:
	cxRemoteListEvent(int id = 0) : cxRemoteAction(id, wxEVT_REMOTELIST_RECEIVED) {};
	cxRemoteListEvent(const cxRemoteListEvent& event) : cxRemoteAction(event) {
		m_fiList = event.m_fiList;
	};
	virtual wxEvent* Clone() const {
		return new cxRemoteListEvent(*this);
	};

	std::vector<cxFileInfo>& GetFileList() {return m_fiList;};

private:
	std::vector<cxFileInfo> m_fiList;
};

typedef void (wxEvtHandler::*cxRemoteActionEventFunction) (cxRemoteAction&);
typedef void (wxEvtHandler::*cxRemoteListEventFunction) (cxRemoteListEvent&);

#define cxRemoteActionEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (cxRemoteActionEventFunction) &func
#define cxRemoteListEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (cxRemoteListEventFunction) &func
#define EVT_REMOTEACTION(func) wx__DECLARE_EVT0(wxEVT_REMOTEACTION, cxRemoteActionEventHandler(func))
#define EVT_REMOTELIST_RECEIVED(func) wx__DECLARE_EVT0(wxEVT_REMOTELIST_RECEIVED, cxRemoteListEventHandler(func))


#endif // __PROJECTPANE_H__
