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

#include "RemoteThread.h"
#include "ftpparse.h"
#include <wx/ffile.h>
#include <wx/filename.h>
#include "DirWatcher.h"
#include <wx/dir.h>
#include "urlencode.h"

// tinyxml includes unused vars so it can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include "tinyxml.h"
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

DEFINE_EVENT_TYPE(wxEVT_REMOTEACTION)
DEFINE_EVENT_TYPE(wxEVT_REMOTELIST_RECEIVED)

BEGIN_EVENT_TABLE(RemoteThread, wxEvtHandler)
	EVT_REMOTEACTION(RemoteThread::OnRemoteAction)
	EVT_REMOTELIST_RECEIVED(RemoteThread::OnRemoteListReceived)
END_EVENT_TABLE()

RemoteThread::RemoteThread()
: m_removedEvtHandler(NULL), m_newActionsCond(m_condMutex), m_lastErrorCode(CURLE_OK), m_fiList(NULL) {
	// Create and run the thread
	Create();
	Run();
}

wxString RemoteThread::GetErrorText(CURLcode errorCode) const {
	return wxString(curl_easy_strerror(errorCode), wxConvUTF8);
}

void* RemoteThread::Entry() {
	// Get curl handle
	m_curlHandle = curl_easy_init();
	if (!m_curlHandle) return NULL;

	while (1) {
		while (!m_actionList.empty()) {
			m_removedEvtHandler = NULL; // only valid for running event

			// Get action
			m_listCrit.Enter();
				const RemoteAction ra = m_actionList.back();
				m_actionList.pop_back();
			m_listCrit.Leave();

			// verify
			wxASSERT(!ra.m_url.empty());

			// Do action
			switch(ra.m_action) {
			case cxRA_LIST:
				GetList(ra);
				break;
			case cxRA_DOWNLOAD:
				DoDownload(ra);
				break;
			case cxRA_UPLOAD:
			case cxRA_UPLOAD_AND_DATE:
				DoUpload(ra);
				break;
			case cxRA_DATE:
			case cxRA_DATE_LOCKING:
				DoGetDate(ra);
				break;
			case cxRA_DELETE:
				DoDelete(ra);
				break;
			case cxRA_CREATEFILE:
				DoCreateFile(ra);
				break;
			case cxRA_MKDIR:
				DoMakeDir(ra);
				break;
			case cxRA_RENAME:
				DoRename(ra);
				break;
			default:
				wxASSERT(false);
			}
		}

		// Wait for new actions being pushed to the list
		wxMutexLocker lock(m_condMutex);
		m_newActionsCond.Wait();
	}

	// If the thread ever closes down, we have to cleanup
	curl_easy_cleanup(m_curlHandle);

	return NULL;
}

void RemoteThread::GetRemoteList(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));

	AddAction(cxRA_LIST, url, wxEmptyString, rp, &evtHandler);
}

void RemoteThread::Delete(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty());

	AddAction(cxRA_DELETE, url, wxEmptyString, rp, &evtHandler);
}

void RemoteThread::CreateFile(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty() && url.Last() != wxT('/'));

	AddAction(cxRA_CREATEFILE, url, wxEmptyString, rp, &evtHandler);
}

void RemoteThread::MkDir(const wxString& url, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty());

	AddAction(cxRA_MKDIR, url, wxEmptyString, rp, &evtHandler);
}

void RemoteThread::Rename(const wxString& url, const wxString& newname, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty());
	wxASSERT(!newname.empty());

	AddAction(cxRA_RENAME, url, newname, rp, &evtHandler);
}

void RemoteThread::UploadAsync(const wxString& url, const wxString& filePath, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty() && url.Last() != wxT('/'));

	AddAction(cxRA_UPLOAD, url, filePath, rp, &evtHandler);
}

void RemoteThread::DownloadAsync(const wxString& url, const wxString& filePath, const RemoteProfile& rp, wxEvtHandler& evtHandler) {
	wxASSERT(!url.empty() && url.Last() != wxT('/'));

	AddAction(cxRA_DOWNLOAD, url, filePath, rp, &evtHandler);
}

CURLcode RemoteThread::Download(const wxString& url, const wxString& buffPath, const RemoteProfile& rp) {
	wxASSERT(!url.empty() && url.Last() != wxT('/'));

	AddAction(cxRA_DOWNLOAD, url, buffPath, rp, this);

	WaitForEvent();
	return m_lastErrorCode;
}

CURLcode RemoteThread::GetRemoteListWait(const wxString& url, const RemoteProfile& rp, vector<cxFileInfo>& fiList) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));

	m_fiList = &fiList;
	AddAction(cxRA_LIST, url, wxEmptyString, rp, this);

	WaitForEvent();
	m_fiList = NULL;

	return m_lastErrorCode;
}

CURLcode RemoteThread::UploadAndDate(const wxString& url, const wxString& buffPath, const RemoteProfile& rp) {
	wxASSERT(!url.empty() && url.Last() != wxT('/'));

	// Changes date on source file to match file on server
	AddAction(cxRA_UPLOAD_AND_DATE, url, buffPath, rp, this);

	WaitForEvent();
	return m_lastErrorCode;
}

wxDateTime RemoteThread::GetModDate(const wxString& url, const RemoteProfile& rp) {
	wxASSERT(!url.empty());

	AddAction(cxRA_DATE, url, wxEmptyString, rp, this);

	WaitForEvent();
	return m_modDate;
}


wxDateTime RemoteThread::GetModDateLocking(const wxString& url, const RemoteProfile& rp) {
	// This function is only to be used by ChangeCheckerThread
	wxASSERT(!url.empty());

	AddAction(cxRA_DATE_LOCKING, url, wxEmptyString, rp, NULL);

	m_isLocked = true;
	while (m_isLocked) {
		wxMilliSleep(50); // don't eat 100% of the CPU
	}
	return m_lockedModDate;
}

void RemoteThread::WaitForEvent() {
	m_waitingForEvent = true;
	while (m_waitingForEvent) {
        wxMilliSleep(50); // don't eat 100% of the CPU

#ifdef __WXMSW__
		// WORKAROUND: Yield resets busy cursor
		// so we have to buffer it and reset it after yielding
		HCURSOR currentCursor = ::GetCursor();
#endif //__WXMSW__

		wxSafeYield(); // we must process messages or we'd never get the cxRemoteAction event

#ifdef __WXMSW__
		::SetCursor(currentCursor);
		//wxSetCursor(*wxHOURGLASS_CURSOR); // WORKAROUND: Yield resets busy cursor
#endif //__WXMSW__
	}
}

void RemoteThread::OnRemoteAction(cxRemoteAction& event) {
	if (event.Succeded()) {
		m_modDate = event.GetDate();
	}
	m_lastErrorCode = event.GetErrorCode();
	m_lastError = event.GetError();
	m_waitingForEvent = false;
}

void RemoteThread::OnRemoteListReceived(cxRemoteListEvent& event) {
	if (event.Succeded() && m_fiList) {
		m_fiList->swap(event.GetFileList());
	}
	m_lastErrorCode = event.GetErrorCode();
	m_lastError = event.GetError();
	m_waitingForEvent = false;
}

void RemoteThread::AddAction(cxActionType action, const wxString& url, const wxString& target, const RemoteProfile& rp, wxEvtHandler* evtHandler) {
	// Urls have to be utf8 url-encoded
	const wxString encodedUrl = url.StartsWith(wxT("http://")) ? UrlEncode::EscapeUrl(url) : url;
	//const wxString encodedTarget = UrlEncode::Encode(target);

	m_listCrit.Enter();
		m_actionList.push_front(RemoteAction(action, encodedUrl, target, rp, evtHandler));
	m_listCrit.Leave();

	// Signal thread that we have new actions
	wxMutexLocker lock(m_condMutex);
	m_newActionsCond.Signal();
}

void RemoteThread::RemoveEventHandler(wxEvtHandler& evtHandler) {
	// Delete all actions that should report to this handler
	m_listCrit.Enter();
		deque<RemoteAction>::iterator p = m_actionList.begin();
		while (p != m_actionList.end()) {
			if (p->m_evtHandler == &evtHandler) p = m_actionList.erase(p);
			else ++p;
		}
	m_listCrit.Leave();

	// The current action might also report to this handler
	// so make sure it get intercepted
	m_removedEvtHandler = &evtHandler;
}

void RemoteThread::PostEvent(wxEvtHandler* evtHandler, wxEvent& event) {
	if (evtHandler != m_removedEvtHandler) evtHandler->AddPendingEvent(event);
}

void RemoteThread::GetList(const RemoteAction& ra) {
	cxRemoteListEvent event;
	vector<cxFileInfo>& fiList = event.GetFileList();

	// Get lists of files and dirs
	CURLcode res;
	if (ra.m_url.StartsWith(wxT("http://"))) res = DoGetDirWebDav(ra.m_url, ra, fiList);
	else res = DoGetDir(ra.m_url, ra, fiList);

	// Return event
	if (ra.m_evtHandler) {
		event.SetUrl(ra.m_url);
		event.SetActionType(ra.m_action);
		event.SetError(res);
		PostEvent(ra.m_evtHandler, event);
	}
}

void RemoteThread::DoGetDate(const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);

	CURLcode res = CURLE_OK;
	wxDateTime modTime;

	// Set options
	curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1); // no file transfer
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, ra.m_url.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
	curl_easy_setopt(m_curlHandle, CURLOPT_FILETIME, 1);

	// Get modification time
	res = curl_easy_perform(m_curlHandle);
	if (res == CURLE_OK) {
		// Set time in event
		time_t time;
		curl_easy_getinfo(m_curlHandle, CURLINFO_FILETIME, &time);
		if (time != -1) {
			modTime = wxDateTime(time);
		}
	}

	// Return event
	if (ra.m_action == cxRA_DATE_LOCKING) {
		// This action is only to be used by ChangeCheckerThread
		m_lockedModDate = modTime;
		m_isLocked = false;
	}
	else if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(ra.m_url);
		event.SetError(res);
		event.SetDate(modTime);
		PostEvent(ra.m_evtHandler, event);
	}
}

void RemoteThread::DoDownload(const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);
	CURLcode error = CURLE_OK;

	// Open file
	wxFFile file(ra.m_target, wxT("wb"));
	if (!file.IsOpened()) error = CURLE_WRITE_ERROR; // Could not open temp buffer file
	else {
		// Set options
		curl_easy_setopt(m_curlHandle, CURLOPT_URL, ra.m_url.mb_str(wxConvUTF8).data());
		if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
		curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION, WriteFileCallback);
		curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, &file);
		curl_easy_setopt(m_curlHandle, CURLOPT_FILETIME, 1); // we also want modification time
		
		// Set proxy
		if (ra.m_url.StartsWith(wxT("http"))) curl_use_proxy(m_curlHandle);

		// Do the Download
		wxLogDebug(wxT("FTP download \"%s\" to \"%s\""), ra.m_url.c_str(), ra.m_target.c_str());
		error = curl_easy_perform(m_curlHandle);
		if (error == CURLE_OK)  {
			// Set modification time
			time_t time;
			curl_easy_getinfo(m_curlHandle, CURLINFO_FILETIME, &time);
			if (time != -1) {
				file.Close(); // need to close first to avoid overwriting date

				wxFileName fn(ra.m_target);
				if (!fn.FileExists()) error = CURLE_GOT_NOTHING;
				else {
					const wxDateTime modTime(time);
					fn.SetTimes(NULL, &modTime, NULL);
					wxLogDebug(wxT("  Set modTime %s"), modTime.Format(wxT("%x %X")).c_str());
				}
			}
		}
	}

	// Return event
	if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(ra.m_url);
		event.SetError(error);

		PostEvent(ra.m_evtHandler, event);
	}
}

void RemoteThread::DoUpload(const RemoteAction& ra) {
	CURLcode res;

	const bool isDir = wxDirExists(ra.m_target);
	wxString url = ra.m_url;

	if (isDir) {
		// Create the dir
		if (url.Last() != wxT('/')) url += wxT('/');
		res = DoUploadDir(url, ra.m_target, ra);
	}
	else res = DoUploadFile(url, ra.m_target, ra);

	// Return event
	if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(url);
		event.SetError(res);

		PostEvent(ra.m_evtHandler, event);
	}
}

CURLcode RemoteThread::DoUploadDir(const wxString& url, const wxString& path, const RemoteAction& ra) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));
	CURLcode res;

	// Create the dir
	res = DoCreateDir(url, ra);
	if (res != CURLE_OK) return res;

	wxDir d;
	d.Open(path);
	if (!d.IsOpened()) return CURLE_READ_ERROR;

	// Upload all subdirs
	wxString dirname;
    if (d.GetFirst(&dirname, wxEmptyString, wxDIR_DIRS)) do
    {
        const wxString newUrl = url + dirname + wxT('/');
		const wxString newPath = path + wxFILE_SEP_PATH + dirname;
		res = DoUploadDir(newUrl, newPath, ra);
		if (res != CURLE_OK) return res;
    }
    while (d.GetNext(&dirname));

	// Upload all files
	wxString filename;
    if (d.GetFirst(&filename, wxEmptyString, wxDIR_FILES)) do
    {
        const wxString newUrl = url + filename;
		const wxString newPath = path + wxFILE_SEP_PATH + filename;
		res = DoUploadFile(newUrl, newPath, ra);
		if (res != CURLE_OK) return res;
    }
    while (d.GetNext(&filename));

	return res;
}


CURLcode RemoteThread::DoUploadFile(const wxString& url, const wxString& filePath, const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);
	CURLcode error;

	// Open file
	wxFFile file(filePath, wxT("rb"));
	if (!file.IsOpened()) error = CURLE_FILE_COULDNT_READ_FILE; // Could not open temp buffer file
	else {
		// Get file size
		const wxULongLong_t size = wxFileName::GetSize(filePath).GetValue();

		// Set options
		curl_easy_setopt(m_curlHandle, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(m_curlHandle, CURLOPT_URL, url.mb_str(wxConvUTF8).data());
		if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
		curl_easy_setopt(m_curlHandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);
		curl_easy_setopt(m_curlHandle, CURLOPT_READFUNCTION, ReadFileCallback);
		curl_easy_setopt(m_curlHandle, CURLOPT_READDATA, &file);

		// Do the Upload
		wxLogDebug(wxT("FTP upload \"%s\" to \"%s\""), ra.m_target.c_str(), ra.m_url.c_str());
		error = curl_easy_perform(m_curlHandle);
		if (error == CURLE_OK && ra.m_action == cxRA_UPLOAD_AND_DATE) {
			// We also need to get the modification time so it matches file on server
			curl_easy_reset(m_curlHandle);
			curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1); // no file transfer
			curl_easy_setopt(m_curlHandle, CURLOPT_URL, url.mb_str(wxConvUTF8).data());
			if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
			curl_easy_setopt(m_curlHandle, CURLOPT_FILETIME, 1);

			error = curl_easy_perform(m_curlHandle);
			if (error == CURLE_OK) {
				time_t time;
				curl_easy_getinfo(m_curlHandle, CURLINFO_FILETIME, &time);
				if (time != -1) {
					file.Close(); // need to close first to avoid overwriting date

					const wxDateTime modTime(time);
					wxFileName fn(ra.m_target);
					fn.SetTimes(NULL, &modTime, NULL);
					wxLogDebug(wxT("  Set modTime %s"), modTime.Format(wxT("%x %X")).c_str());
				}
			}
		}
	}

	return error;
}

bool RemoteThread::IsDir(const wxString& url, const RemoteAction& ra) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));

	// Clean up first
	curl_easy_reset(m_curlHandle);

	curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1); // no file transfer
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, url.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());

	// Just connect and see if curl can CWD to path
	const CURLcode res = curl_easy_perform(m_curlHandle);

	return (res == CURLE_OK);
}

CURLcode RemoteThread::DoGetDir(const wxString& url, const RemoteAction& ra, vector<cxFileInfo>& fiList) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));

	// Clean up first
	curl_easy_reset(m_curlHandle);

	// Set options
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, url.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
	curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, this);

	// Get list
	const CURLcode res = curl_easy_perform(m_curlHandle);

	if (res == CURLE_OK && !m_data.empty()) {
		// Parse file list
		vector<char>::iterator linestart = m_data.begin();
		vector<char>::iterator lineend = m_data.begin();
		while (lineend != m_data.end()) {
			// Find end of line
			while (lineend != m_data.end() && *lineend != '\r' && *lineend != '\n') ++lineend;

			// Parse individual line
			ftpparse_struct fp;
			if (ftpparse(&fp, &*linestart, lineend - linestart)) {
				const wxString name(fp.name, wxConvUTF8, fp.namelen);
				if (name != wxT(".") && name != wxT("..")) { // ignore meta dirs
					cxFileInfo fi;
					fi.m_name = name;
					fi.m_size = fp.size;
					if (fp.flagtrycwd) {
						if (fp.flagtryretr) fi.m_isDir = IsDir(url + name + wxT('/'), ra); // may be a link to file
						else fi.m_isDir = true;
					}
					fiList.push_back(fi);
				}
			}

			// Advance to next line
			while (lineend != m_data.end() && (*lineend == '\r' || *lineend == '\n')) ++lineend;
			linestart = lineend;
		}

		wxLogDebug(wxString(&*m_data.begin(), wxConvUTF8, m_data.size()));

		// Clean up internal buffer
		m_data.clear();
	}

	return res;
}

#include <wx/tokenzr.h>

CURLcode RemoteThread::DoGetDirWebDav(const wxString& url, const RemoteAction& ra, vector<cxFileInfo>& fiList) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));
	wxASSERT(url.StartsWith(wxT("http://")) || url.StartsWith(wxT("https://")));

	wxLogDebug(wxT("Get webdav url: %s"), url.c_str());

	// Clean up first
	curl_easy_reset(m_curlHandle);

	// Set basic options
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, url.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
	curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, this);

	// Set proxy
	curl_use_proxy(m_curlHandle);

#ifdef __WXDEBUG__
	//curl_easy_setopt(m_curlHandle, CURLOPT_VERBOSE, 1);
	//curl_easy_setopt(m_curlHandle, CURLOPT_DEBUGFUNCTION, CurlDebugCallback);
#endif

	// Set WebDav Propfind request options
	const char hdr1[] = "Depth: 1";
	const char hdr2[] = "Content-Type: text/xml; charset=\"utf-8\"";
	struct curl_slist *headerlist = NULL;
	headerlist = curl_slist_append(headerlist, hdr1);
	headerlist = curl_slist_append(headerlist, hdr2);
	curl_easy_setopt(m_curlHandle, CURLOPT_HTTPHEADER, headerlist);
	curl_easy_setopt(m_curlHandle, CURLOPT_CUSTOMREQUEST, "PROPFIND");

	// Get list
	const CURLcode res = curl_easy_perform(m_curlHandle);

	if (res == CURLE_OK) {
		/*const wxString response(m_data.begin(), wxConvUTF8, m_data.size());
		wxStringTokenizer tkz(response, wxT("\n"));
		while ( tkz.HasMoreTokens() ) wxLogDebug(tkz.GetNextToken());*/

		// Parse file list
		ParseWebDavXml(m_data, fiList);

		// First item is dir itself
		if (!fiList.empty()) fiList.erase(fiList.begin());

		// Clean up internal buffer
		m_data.clear();
	}
	else {
		wxLogDebug(wxT("Curl failed: %d"), res);
	}

	return res;
}

CURLcode RemoteThread::DoDeleteFile(const wxString& url, const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);

	// Get the path and name
	bool isDir = false;
	wxString path = url;
	if (url.Last() == wxT('/')) {
		isDir = true;
		path.RemoveLast();
	}
	const wxString name = path.AfterLast(wxT('/'));
	path = path.BeforeLast(wxT('/'));
	path += wxT('/'); // curl need to know it is a dir

	// Set options
	curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1); // no file transfer
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, path.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());

	//wxCharBuffer errorBuf(CURL_ERROR_SIZE);
	//curl_easy_setopt(m_curlHandle, CURLOPT_ERRORBUFFER, errorBuf.data());

	// Build and set command
	const wxString cmd = (isDir ? wxT("RMD ") : wxT("DELE ")) + name;
	struct curl_slist *headerlist=NULL;
	headerlist = curl_slist_append(headerlist, cmd.mb_str(wxConvUTF8));
	curl_easy_setopt(m_curlHandle, CURLOPT_POSTQUOTE, headerlist);

	// Do the action
	const CURLcode res = curl_easy_perform(m_curlHandle);

	// Clean up
	curl_slist_free_all(headerlist);

	return res;
}

CURLcode RemoteThread::DoDeleteDir(const wxString& url, const RemoteAction& ra) {
	wxASSERT(!url.empty() && url.Last() == wxT('/'));

	// Get lists of files and folders in dir
	vector<cxFileInfo> fiList;
	CURLcode res = DoGetDir(url, ra, fiList);
	if (res != CURLE_OK) return res;

	// Delete all files and folders
	for (vector<cxFileInfo>::const_iterator p = fiList.begin(); p != fiList.end(); ++p) {
		wxString path = url + p->m_name;
		if (p->m_isDir) {
			path += wxT('/');
			res = DoDeleteDir(path, ra);
		}
		else res = DoDeleteFile(path, ra);

		if (res != CURLE_OK) return res;
	}

	// Delete current dir
	res = DoDeleteFile(url, ra); // knows this is a dir from trailing slash
	return res;
}

void RemoteThread::DoDelete(const RemoteAction& ra) {
	// Do the action
	const CURLcode res = ra.m_url.Last() == wxT('/') ? DoDeleteDir(ra.m_url, ra) : DoDeleteFile(ra.m_url, ra);

	// Send event
	if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(ra.m_url);
		event.SetError(res);
		PostEvent(ra.m_evtHandler, event);
	}
}

void RemoteThread::DoMakeDir(const RemoteAction& ra) {
	// Do the action
	const CURLcode res = DoCreateDir(ra.m_url, ra);

	// Send event
	if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(ra.m_url);
		event.SetError(res);
		PostEvent(ra.m_evtHandler, event);
	}
}

CURLcode RemoteThread::DoCreateDir(const wxString& url, const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);

	// Get the path and name
	wxString path = url;
	if (url.Last() == wxT('/')) path.RemoveLast();
	const wxString name = path.AfterLast(wxT('/'));
	path = path.BeforeLast(wxT('/'));
	path += wxT('/'); // needed for curl to know it is a folder

	// Set options
	curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1); // no file transfer
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, path.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());

	// Build and set command
	const wxString cmd = wxT("MKD ") + name;
	struct curl_slist *headerlist=NULL;
	headerlist = curl_slist_append(headerlist, cmd.mb_str(wxConvUTF8));
	curl_easy_setopt(m_curlHandle, CURLOPT_POSTQUOTE, headerlist);

	// Do the action
	const CURLcode res = curl_easy_perform(m_curlHandle);

	// Clean up
	curl_slist_free_all(headerlist);

	return res;
}

void RemoteThread::DoRename(const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);

	// Get the path and name of source
	wxString path = ra.m_url;
	if (ra.m_url.Last() == wxT('/')) path.RemoveLast();
	const wxString name = path.AfterLast(wxT('/'));
	path = path.BeforeLast(wxT('/'));
	path += wxT('/'); // needed for curl to know it is a folder

	// Get name of target
	// TODO: handle change of path
	const wxString newname = ra.m_target.AfterLast(wxT('/'));

	// Set options
	curl_easy_setopt(m_curlHandle, CURLOPT_NOBODY, 1); // no file transfer
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, path.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());

	// Build and set command
	const wxString cmd1 = wxT("RNFR ") + name;
	const wxString cmd2 = wxT("RNTO ") + newname;
	struct curl_slist *headerlist=NULL;
	headerlist = curl_slist_append(headerlist, cmd1.mb_str(wxConvUTF8));
	headerlist = curl_slist_append(headerlist, cmd2.mb_str(wxConvUTF8));
	curl_easy_setopt(m_curlHandle, CURLOPT_POSTQUOTE, headerlist);

	// Do the action
	const CURLcode res = curl_easy_perform(m_curlHandle);

	// Send event
	if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(ra.m_url);
		event.SetTarget(newname);
		event.SetError(res);
		PostEvent(ra.m_evtHandler, event);
	}

	// Clean up
	curl_slist_free_all(headerlist);
}

void RemoteThread::DoCreateFile(const RemoteAction& ra) {
	// Clean up first
	curl_easy_reset(m_curlHandle);

	// Set options
	curl_easy_setopt(m_curlHandle, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(m_curlHandle, CURLOPT_URL, ra.m_url.mb_str(wxConvUTF8).data());
	if (!ra.m_login.empty()) curl_easy_setopt(m_curlHandle, CURLOPT_USERPWD, ra.m_login.mb_str(wxConvUTF8).data());
	curl_easy_setopt(m_curlHandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)0);
	curl_easy_setopt(m_curlHandle, CURLOPT_READFUNCTION, ReadEmptyFileCallback);

	// Do the action
	const CURLcode res = curl_easy_perform(m_curlHandle);

	// Send event
	if (ra.m_evtHandler) {
		cxRemoteAction event;
		event.SetActionType(ra.m_action);
		event.SetUrl(ra.m_url);
		event.SetError(res);
		PostEvent(ra.m_evtHandler, event);
	}
}

int RemoteThread::WriteCallback(void *buffer, size_t size, size_t nmemb, void* data) { // static
	RemoteThread* self = (RemoteThread*)data;
	const size_t realsize = size * nmemb;

	self->m_data.insert(self->m_data.end(), (char*)buffer, (char*)buffer+realsize);

	return nmemb;
}

int RemoteThread::WriteFileCallback(void *buffer, size_t size, size_t nmemb, void* data) { // static
	wxFFile* file = (wxFFile*)data;
	const size_t realsize = size * nmemb;

	return file->Write(buffer, realsize);
}

int RemoteThread::ReadFileCallback(void *buffer, size_t size, size_t nmemb, void* data) { // static
	wxFFile* file = (wxFFile*)data;
	const size_t realsize = size * nmemb;

	return file->Read(buffer, realsize);
}

int RemoteThread::ReadEmptyFileCallback(void*, size_t, size_t, void*) { // static
	return 0;
}

int RemoteThread::CurlDebugCallback(void*, curl_infotype type, char * text, size_t len, void *) {
	wxLogDebug(wxT("CURL (%d): %s"), type, wxString(text, wxConvUTF8, len).c_str());
	return 0;
}

void RemoteThread::ParseWebDavXml(const vector<char>& data, vector<cxFileInfo>& fiList) const {
	if (data.empty()) return;

	TiXmlDocument doc;
	doc.Parse(&*data.begin());
	if (doc.Error()) return;

	// Strip Past First "Multistatus" tag...
	const TiXmlElement* status = doc.FirstChildElement("D:multistatus");
	if (!status) status = doc.FirstChildElement();

	// Process "Response" tags...
	const TiXmlElement* response = status->FirstChildElement("D:response");
	while (response) {
		cxFileInfo fi;
		if (ParseResponseXml(response, fi)) {
			fiList.push_back(fi);
		}

		response = response->NextSiblingElement("D:response");
	}
}

bool RemoteThread::ParseResponseXml(const TiXmlElement* response, cxFileInfo& fi) const {
	// Get path
	const TiXmlElement* href = response->FirstChildElement("D:href");
	if (!href) return false;
	const char* text = href->GetText();
	if (!text) return false;

	// Convert url-encoded chars
	// (has to be done before converting to wxString as UTF8 chars can also be encoded)
	const char* convText = curl_easy_unescape(m_curlHandle, text, 0, NULL);
	fi.m_name = wxString(convText, wxConvUTF8);
	curl_free((void*)convText);

	// Strip path
	wxString& name = fi.m_name;
	if (name.Last() == wxT('/')) name.RemoveLast(); // dirs have trailing slash
	name = name.AfterLast(wxT('/'));

	// Get properties
	const TiXmlElement* propstat = response->FirstChildElement("D:propstat");
	if (propstat) {
		const TiXmlElement* prop = propstat->FirstChildElement("D:prop");
		if (prop) {
			// Get type (file/folder)
			const TiXmlElement* type = prop->FirstChildElement("lp1:resourcetype");
			if (type && type->FirstChildElement("D:collection")) fi.m_isDir = true;

			// Get size
			if (!fi.m_isDir) {
				const TiXmlElement* length = prop->FirstChildElement("lp1:getcontentlength");
				if (length) {
					const char* len_str = length->GetText();
					fi.m_size = atoi(len_str);
				}
			}

			// Get modification date
			const TiXmlElement* lastmodified = prop->FirstChildElement("lp1:getlastmodified");
			if (lastmodified) {
				const char* text = lastmodified->GetText();
				if (text) {
					const wxString modStr(text, wxConvUTF8);
					fi.m_modDate.ParseRfc822Date(modStr);
				}
			}
		}
	}

	return true;
}

wxString RemoteThread::EscapeUrl(const wxString& url) const {
	if (url.empty()) return wxEmptyString;

	// The each part of the url has to be escaped separately
	// so that the separators are preserved.
	wxString escUrl;

	// Protocol
	int pos = url.Find(wxT("://"));
	if (pos == wxNOT_FOUND) return url;
	pos += 3;
	escUrl += url.substr(0, pos);

	wxString address = url.substr(pos);
	wxStringTokenizer tkz(address, wxT('/'));

	// Domain is not escaped
	if (!tkz.HasMoreTokens()) return url;
	escUrl += tkz.GetNextToken();

	// Each element
	while (tkz.HasMoreTokens()) {
		escUrl += wxT('/');
		const wxString token = tkz.GetNextToken();

		// Convert to utf8 url-escaped
		wxCharBuffer utf8buf = token.mb_str(wxConvUTF8);
		const char* escaped = curl_easy_escape(m_curlHandle, utf8buf.data(), 0);
		escUrl += wxString(escaped, wxConvUTF8);
		curl_free((void*)escaped);
	}

	// Check if there is trailing slash
	if (url.Last() == wxT('/')) escUrl += wxT('/');

	return escUrl;
}

#ifdef __WXMSW__
#include <Wininet.h>
#endif //__WXMSW__

CURLcode RemoteThread::curl_use_proxy(CURL * WXUNUSED(curlHandle)) {
#if 0
	unsigned long        nSize = 4096;
	char                 szBuf[4096];
	INTERNET_PROXY_INFO* pInfo = (INTERNET_PROXY_INFO*)szBuf;

	if(!InternetQueryOption(NULL, INTERNET_OPTION_PROXY, pInfo, &nSize)) return CURLE_FUNCTION_NOT_FOUND;

	// if a proxy is set
	if(pInfo->dwAccessType == INTERNET_OPEN_TYPE_PROXY) {
		// push it into the curl handle
		CURLcode curlError = curl_easy_setopt(curlHandle, CURLOPT_PROXY, (void *)pInfo->lpszProxy);
		if(curlError != CURLE_OK) return curlError;

		nSize = 1024; // give ourselves some leeway for the password

		// try and get a proxy username - ignore a blank result
		if(InternetQueryOption(NULL, INTERNET_OPTION_PROXY_USERNAME, szBuf, &nSize)) {
			if(szBuf[0] != '\0') {
				// try and put the password on the end of it
				strcat(szBuf, ":");
				nSize = 2000 - strlen(szBuf);

				if(InternetQueryOption(NULL, INTERNET_OPTION_PROXY_PASSWORD, (void *)((char *)szBuf + strlen(szBuf)), &nSize)) {
					// we have it, give it to curl
					curl_easy_setopt(curlHandle, CURLOPT_PROXYUSERPWD, szBuf);
				}
			}
		}
	}

#endif
	return CURLE_OK;
}

// ---- RemoteProfile ---------------------------------------------

RemoteProfile::RemoteProfile(const RemoteProfile& rp) {
	m_id = rp.m_id;
	m_protocol = rp.m_protocol;
	m_name = rp.m_name;
	m_address = rp.m_address;
	m_dir = rp.m_dir;
	m_username = rp.m_username;
	m_pwd = rp.m_pwd;
}

wxString RemoteProfile::GetUrl() const {
	wxString url = m_protocol + wxT("://") + m_address;
	if (m_dir.empty()) url += wxT('/');
	else {
		if (m_dir[0] != wxT('/')) url += wxT('/');
		url += m_dir;
		if (m_dir.Last() != wxT('/')) url += wxT('/');
	}

	return url;
}

wxString RemoteProfile::GetUsernamePwd() const {
	if (m_username.empty() && m_pwd.empty()) return wxEmptyString; // anonymous login

	return m_username + wxT(":") + m_pwd;
}

// ---- cxRemoteAction ---------------------------------------------

void cxRemoteAction::SetUrl(const wxString& url) {
	m_url = UrlEncode::Unescape(url);
}

