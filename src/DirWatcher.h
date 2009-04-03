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

#ifndef __DIRWATCHER_H__
#define __DIRWATCHER_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif
#include <wx/thread.h>

#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

#define READ_DIR_CHANGE_BUFFER_SIZE 4096

class DirWatcher : public wxThread {
public:
	DirWatcher();
	~DirWatcher();

	void* WatchDirectory(const wxString& path, wxEvtHandler& changeHandler, bool watchSubDirs);
	void UnwatchDirectory(void* handle);
#ifdef __WXGTK__
	void UnwatchDirByName(const wxString& path, bool unwatchSubDir = false);
#endif
	void UnwatchAllDirectories();

	virtual void* Entry();

private:
#if defined(__WXGTK__)
	int m_fd; // file descriptor associated with inotify event

	class DirWatchInfo {
	public:
		DirWatchInfo(int desc, wxEvtHandler& hndl, const wxString& dir) : wd(desc), handler(hndl), path(dir) {};
		int GetWD() {return wd;};
		wxEvtHandler& GetHandler() {return handler;};
		wxString GetPath() {return path;};
	private:
		int wd;
		wxEvtHandler& handler;
		wxString path;
	};

	wxEvtHandler* getEvtHandlerByWD(int desc);
	DirWatcher::DirWatchInfo* getDirWatchInfoByWD(int desc);

#elif defined(__WXMSW__)
	class DirWatchInfo {
	public:
		DirWatchInfo(HANDLE hDir, const wxString& directoryName,
					  wxEvtHandler& changeHandler,
					  DWORD dwChangeFilter, bool watchSubDir,
					  const wxString& includeFilter,
					  const wxString& excludeFilter,
					  DWORD dwFilterFlags);

		bool StartMonitor(HANDLE hCompPort);
		void UnwatchDirectory(HANDLE hCompPort);
		void SignalShutdown(HANDLE hCompPort);
		bool CloseDirectoryHandle();
		void ProcessNotification(DWORD& ref_dwReadBuffer_Offset);

		void Lock() {m_cs.Enter();};
		void UnLock() {m_cs.Leave();};
		void SignalStartStop() {wxMutexLocker lock(m_sigMutex);m_startStopCond.Signal();};

	private:
		HANDLE m_hDir;
		wxString m_dirName;
		wxEvtHandler& m_changeHandler;
		bool m_watchSubDir;
		wxString m_includeFilter;
		wxString m_excludeFilter;
		int m_changeFilter;
		int m_filterFlags;
		OVERLAPPED  m_Overlapped;
		bool m_readDirSuccess; // indicates the success of the call to ReadDirectoryChanges()
		enum RunningState{
			 RUNNING_STATE_NOT_SET, RUNNING_STATE_START_MONITORING, RUNNING_STATE_STOP, RUNNING_STATE_STOP_STEP2,
			  RUNNING_STATE_STOPPED, RUNNING_STATE_NORMAL
			 };
		RunningState m_runningState;
		wxCriticalSection m_cs;
		union {
			DWORD	m_alignment; // buffer has to be DWORD aligned
			CHAR    m_Buffer[ READ_DIR_CHANGE_BUFFER_SIZE ];//buffer for ReadDirectoryChangesW
		};
		DWORD       m_dwBufLength;//length or returned data from ReadDirectoryChangesW -- ignored?...

		wxMutex m_sigMutex;
		wxCondition m_startStopCond;


		friend class DirWatcher;
	};

	// Member variables
	HANDLE m_hCompPort;	//i/o completion port
#endif //__WXMSW__
	vector<DirWatchInfo*> m_dirsWatched;
};

enum {
	DIRWATCHER_FILE_ADDED,
	DIRWATCHER_FILE_REMOVED,
	DIRWATCHER_FILE_MODIFIED,
	DIRWATCHER_FILE_RENAMED
};

// Declare custom event
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_DIRWATCHER, 801)
END_DECLARE_EVENT_TYPES()

class wxDirWatcherEvent : public wxEvent {
public:
	wxDirWatcherEvent(int id = 0) : wxEvent(id, wxEVT_DIRWATCHER) {};
	wxDirWatcherEvent(const wxDirWatcherEvent& event) : wxEvent(event) {
			m_changeType = event.m_changeType;
			m_changedFile = event.m_changedFile.c_str();
			m_newFile = event.m_newFile.c_str();
#ifdef __WXGTK__
			m_isDir = event.m_isDir;
#endif
	};
	virtual ~wxDirWatcherEvent() {
		// DEBUG:
		/*if (m_changeType == 0) OutputDebugString(wxT("~DWE: 0 "));
		else if (m_changeType == 1) OutputDebugString(wxT("~DWE: 1 "));
		else if (m_changeType == 2) OutputDebugString(wxT("~DWE: 2 "));
		else {
			const wxString msg = wxString::Format(wxT("~DWE: %d "), m_changeType);
			OutputDebugString(msg);
		}
		OutputDebugString(m_changedFile);
		OutputDebugString(m_newFile);
		OutputDebugString(wxT("\n"));*/
	};

	virtual wxEvent* Clone() const {
		return new wxDirWatcherEvent(*this);
	};

	int GetChangeType() const {return m_changeType;};
	void SetChangeType(int type) {m_changeType = type;};
	const wxString& GetChangedFile() const {return m_changedFile;};
	const wxString& GetNewFile() const {return m_newFile;};
	void SetChangedFile(const wxString& path) {m_changedFile = path;};
	void SetNewFile(const wxString& path) {m_newFile = path;};
#ifdef __WXGTK__
	void SetDirFlag(bool dir) {m_isDir = dir;};
	bool IsDir() {return m_isDir;};
#endif
private:
	int m_changeType;
	wxString m_changedFile;
	wxString m_newFile;
#ifdef __WXGTK__
	bool m_isDir;
#endif
};

typedef void (wxEvtHandler::*wxDirWatcherEventFunction) (wxDirWatcherEvent&);

#define wxDirWatcherEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (wxDirWatcherEventFunction) &func
#define EVT_DIRWATCHER(func) wx__DECLARE_EVT0(wxEVT_DIRWATCHER, wxDirWatcherEventHandler(func))

#endif // __DIRWATCHER_H__
