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

#include "DirWatcher.h"
#include <algorithm>
#ifdef __WXGTK__

#include <sys/inotify.h>
#include <errno.h>

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN (32 * (EVENT_SIZE + 16))

#endif

DEFINE_EVENT_TYPE(wxEVT_DIRWATCHER)

#ifdef __WXMSW__
class FileNotifyInformation {
public:
	FileNotifyInformation( BYTE * lpFileNotifyInfoBuffer, DWORD dwBuffSize)
	: m_pBuffer( lpFileNotifyInfoBuffer ),
	  m_dwBufferSize( dwBuffSize )
	{
		wxASSERT( lpFileNotifyInfoBuffer && dwBuffSize );
		
		m_pCurrentRecord = (PFILE_NOTIFY_INFORMATION) m_pBuffer;

		// DEBUG: Make sure entry fits in buffer
		if ((BYTE*)&m_pCurrentRecord->FileName + m_pCurrentRecord->FileNameLength > m_pBuffer + m_dwBufferSize) {
			OutputDebugString(wxT("WARNING: DirBuffer overflow 1\n"));
		}
	}

	bool GetNextNotifyInformation();
	
	bool CopyCurrentRecordToBeginningOfBuffer(OUT DWORD & ref_dwSizeOfCurrentRecord);

	DWORD	GetAction() const; // Gets the type of file change notifiation
	wxString GetFileName() const; // Gets the file name from the FILE_NOTIFY_INFORMATION record
	wxString GetFileNameWithPath(const wxString& strRootPath) const; // Same as GetFileName() only it prefixes the strRootPath into the file name

	
protected:
	BYTE * m_pBuffer; // <-- all of the FILE_NOTIFY_INFORMATION records 'live' in the buffer this points to...
	DWORD  m_dwBufferSize;
	PFILE_NOTIFY_INFORMATION m_pCurrentRecord; // this points to the current FILE_NOTIFY_INFORMATION record in m_pBuffer
	
};
#endif

// ---- DirWatcher ------------------------------------------------------------------
DirWatcher::DirWatcher() : wxThread(wxTHREAD_DETACHED) {
#if defined(__WXGTK__)
	wxLogDebug(wxT("DirWatcher::%s()"), wxString(__FUNCTION__, wxConvUTF8).c_str());
	if (-1 == (m_fd = inotify_init())) {
		wxLogDebug(wxT("inotify_init() failed! errno=%i (%s)"), errno, wxString(strerror(errno), wxConvUTF8).c_str());
	} else {
		wxThreadError err;
		if (wxTHREAD_NO_ERROR != (err = Create())) {
			wxLogDebug(wxT("Thread creation failed! Error: %i"), err);
		} else {
			if (wxTHREAD_NO_ERROR != (err = Run())) {
				wxLogDebug(wxT("Thread starting failed! Error: %i"), err);
			}
		}
	}
#elif defined(__WXMSW__)
	m_hCompPort = NULL;
#endif
}

DirWatcher::~DirWatcher() {
	UnwatchAllDirectories();
#ifdef __WXMSW__
	if( m_hCompPort ) {
		CloseHandle( m_hCompPort );
		m_hCompPort = NULL;
	}
#endif
}

void* DirWatcher::Entry() {
#if defined(__WXGTK__)
	wxLogDebug(wxT("DirWatcher::%s()"), wxString(__FUNCTION__, wxConvUTF8).c_str());
	unsigned int iSize, iTmp;
	char buf[BUF_LEN];
	struct inotify_event *pEvent;

	wxDirWatcherEvent wdEvent; // watch dir event for appropriate window
	DirWatchInfo* DirWatchInfo = NULL;
	wxEvtHandler* EventHandler = NULL;

	while (true) {
		iSize = read(m_fd, buf, BUF_LEN);
		iTmp = 0;
		if (0 < iSize) {
			while (iTmp < iSize) {
		        	pEvent = (struct inotify_event *) &buf[iTmp];
		        	if (pEvent->len <= 0) {
					break;
				}
				wxLogDebug(wxT("Event: wd=%d mask=%u cookie=%u len=%u name=%s"),
					pEvent->wd, pEvent->mask, pEvent->cookie, pEvent->len,\
					wxString(pEvent->name, wxConvUTF8).c_str());
				if (NULL == (DirWatchInfo = getDirWatchInfoByWD(pEvent->wd))) {
					wxLogDebug(wxT("Can't obtain dir watch info"));
					iTmp += EVENT_SIZE + pEvent->len; //process next event
					continue;
				}
				if (NULL == (EventHandler = &DirWatchInfo->GetHandler())) {
					wxLogDebug(wxT("Can't obtain event handler"));
					iTmp += EVENT_SIZE + pEvent->len; //process next event
					continue;
				}
				// process inotify event
				bool bIsNeedtoProcess = true;
				bool bIsDir = false;

				switch (pEvent->mask) {
					case IN_ISDIR | IN_CREATE: // new directory
						bIsDir = true;
					case IN_CREATE:
						wdEvent.SetChangeType(DIRWATCHER_FILE_ADDED);
						break;
					case IN_ISDIR | IN_DELETE: // remove directory
						bIsDir = true;
					case IN_DELETE:
						wdEvent.SetChangeType(DIRWATCHER_FILE_REMOVED);
						break;
					case IN_ISDIR | IN_MOVED_FROM: // rename directory
						bIsDir = true;
					case IN_MOVED_FROM:
						wdEvent.SetChangeType(DIRWATCHER_FILE_RENAMED);
						bIsNeedtoProcess = false;
						break;
					case IN_ISDIR | IN_MOVED_TO: // rename directory
						bIsDir = true;
					case IN_MOVED_TO:
						if (DIRWATCHER_FILE_RENAMED == wdEvent.GetChangeType()) {
							wdEvent.SetNewFile(DirWatchInfo->GetPath() + wxT("/") +
								wxString(pEvent->name, wxConvUTF8));
						} else {
							wxLogDebug(wxT("Previous event was wrong"));
							bIsNeedtoProcess = false;
						}
						break;
					default:
						bIsNeedtoProcess = false;
						break;
				} // switch (pEvent->mask)
				if (IN_MOVED_TO != (IN_MOVED_TO & pEvent->mask)) {
					wdEvent.SetChangedFile(DirWatchInfo->GetPath() + wxT("/") +
						wxString(pEvent->name, wxConvUTF8));
				}
				if (true == bIsNeedtoProcess) {
					wdEvent.SetDirFlag(bIsDir);
					EventHandler->AddPendingEvent(wdEvent); // Send the event
				}
				iTmp += EVENT_SIZE + pEvent->len; //process next event
			}
		} // if (0 < iSize) {
	} // main loop
	return NULL;
#elif defined(__WXMSW__)
	wxASSERT(m_hCompPort);

	DWORD numBytes;
    DirWatchInfo* pdi;
    LPOVERLAPPED lpOverlapped;

	do {
		if (TestDestroy()) return NULL;

		// Retrieve the directory info for this directory
        // through the io port's completion key
		GetQueuedCompletionStatus( m_hCompPort,
								   &numBytes,
								   (LPDWORD) &pdi, // <-- completion Key
								   &lpOverlapped,
								   INFINITE);
		
		if ( pdi ) { // pdi will be null if I call PostQueuedCompletionStatus(m_hCompPort, 0,0,NULL);
			// The DirWatchInfo::m_runningState is pretty much the only member
			// of DirWatchInfo that can be modified from the other thread.
			pdi->Lock();
			DirWatchInfo::RunningState runState = pdi->m_runningState;
			pdi->UnLock();

			switch( runState )
			{
			case DirWatchInfo::RUNNING_STATE_START_MONITORING:
				{
					// Issue the initial call to ReadDirectoryChangesW()
					if( !ReadDirectoryChangesW( pdi->m_hDir,
										pdi->m_Buffer,//<--FILE_NOTIFY_INFORMATION records are put into this buffer
										READ_DIR_CHANGE_BUFFER_SIZE,
										pdi->m_watchSubDir,
										pdi->m_changeFilter,
										&pdi->m_dwBufLength,//this var not set when using asynchronous mechanisms...
										&pdi->m_Overlapped,
										NULL) )//no completion routine!
					{
						pdi->m_readDirSuccess = false;
						wxLogDebug(wxT("ReadDirectoryChangesW error: %d"), GetLastError());
					}
					else {
						// Read directory changes was successful!
						// allow it to run normally
						pdi->m_runningState = DirWatchInfo::RUNNING_STATE_NORMAL;
						pdi->m_readDirSuccess = true;

					}

					// Signal that the ReadDirectoryChangesW has been called
					// check DirWatchInfo::m_dwReadDirError to see whether or not ReadDirectoryChangesW succeeded...
					pdi->SignalStartStop();
				}
				break;

			case DirWatchInfo::RUNNING_STATE_NORMAL:
				{
					DWORD dwReadBuffer_Offset = 0UL;
					pdi->ProcessNotification(dwReadBuffer_Offset);
	
					//	Changes have been processed,
					//	Reissue the watch command
					if( !ReadDirectoryChangesW( pdi->m_hDir,
										  pdi->m_Buffer + dwReadBuffer_Offset,//<--FILE_NOTIFY_INFORMATION records are put into this buffer 
								          READ_DIR_CHANGE_BUFFER_SIZE - dwReadBuffer_Offset,
										  pdi->m_watchSubDir,
										  pdi->m_changeFilter,
										  &pdi->m_dwBufLength,//this var not set when using asynchronous mechanisms...
										&pdi->m_Overlapped,
										NULL) )//no completion routine!
					{
						// error: Shut down the watch
						wxASSERT(false);
					}
					else {
						pdi->m_readDirSuccess = true;
					}
				}
				break;

			case DirWatchInfo::RUNNING_STATE_STOP:
				{
					//We want to shut down the monitoring of the directory
					//that pdi is managing...
					
					if( pdi->m_hDir != INVALID_HANDLE_VALUE )
					{
					 //Since I've previously called ReadDirectoryChangesW() asynchronously, I am waiting
					 //for it to return via GetQueuedCompletionStatus().  When I close the
					 //handle that ReadDirectoryChangesW() is waiting on, it will
					 //cause GetQueuedCompletionStatus() to return again with this pdi object....
					 // Close the handle, and then wait for the call to GetQueuedCompletionStatus()
					 //to return again by breaking out of the switch, and letting GetQueuedCompletionStatus()
					 //get called again
						pdi->CloseDirectoryHandle();
						pdi->m_runningState = DirWatchInfo::RUNNING_STATE_STOP_STEP2;//back up step...GetQueuedCompletionStatus() will still need to return from the last time that ReadDirectoryChangesW() was called.....
					}
					else
					{
						//either we weren't watching this direcotry in the first place,
						//or we've already stopped monitoring it....
						pdi->SignalStartStop();//set the event that ReadDirectoryChangesW has returned and no further calls to it will be made...
					}
					
				
				}
				break;

			case DirWatchInfo::RUNNING_STATE_STOP_STEP2:
				{

					//GetQueuedCompletionStatus() has returned from the last
					//time that ReadDirectoryChangesW was called...
					//Using CloseHandle() on the directory handle used by
					//ReadDirectoryChangesW will cause it to return via GetQueuedCompletionStatus()....
					if( pdi->m_hDir == INVALID_HANDLE_VALUE )
						pdi->SignalStartStop();//signal that no further calls to ReadDirectoryChangesW will be made
														 //and this pdi can be deleted 
					else
					{//for some reason, the handle is still open..
											
						pdi->CloseDirectoryHandle();

						//wait for GetQueuedCompletionStatus() to return this pdi object again


					}
	
				}
				break;

			default:
				wxASSERT(false);
			}

			// DEBUG: Check the heap
			if (_heapchk() != _HEAPOK) {
				OutputDebugString(wxT("WARNING: heapchk failed\n"));
				__debugbreak();
			}
        }

	} while( pdi );

	return NULL;
#endif
}


void* DirWatcher::WatchDirectory(const wxString& path, wxEvtHandler& changeHandler, bool watchSubDirs) {
#if defined(__WXGTK__)
	int mask = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE_SELF | IN_MOVE | IN_DELETE_SELF;
	int wd;

	wxLogDebug(wxT("DirWatcher::%s() path=%s"), wxString(__FUNCTION__, wxConvUTF8).c_str(), path.c_str());
	if (-1 == m_fd) {
		wxLogDebug(wxT("inotify was not init!"));
		return NULL;
	}
	wd = inotify_add_watch (m_fd, (const char*)path.mb_str(wxConvUTF8), mask);
	//wd = inotify_add_watch (m_fd, "/proj/e_port/project", mask);
	if (0 > wd) {
		wxLogDebug(wxT("inotify_add_watch() failed! errno=%i (%s)"), errno, wxString(strerror(errno), wxConvUTF8).c_str());
		return NULL;
	}
	// Add descriptor to the list of active watches
	DirWatchInfo * pDirInfo = new DirWatchInfo(wd, changeHandler, path);
	if (NULL != pDirInfo) {
		m_dirsWatched.push_back(pDirInfo);
		return pDirInfo;
	} else {
		wxLogDebug(wxT("Memory allocation failed!"));
		return NULL;
	}
#elif defined(__WXMSW__)
	// Check that it is really a directory

	// Check that it is not already being watched

	// Open the directory to watch....
	HANDLE hDir = CreateFileW(path.c_str(), 
								FILE_LIST_DIRECTORY, 
								FILE_SHARE_READ | FILE_SHARE_WRITE| FILE_SHARE_DELETE,								NULL, //security attributes
								OPEN_EXISTING,
								FILE_FLAG_BACKUP_SEMANTICS | //<- the required priviliges for this flag are: SE_BACKUP_NAME and SE_RESTORE_NAME.  CPrivilegeEnabler takes care of that.
                                FILE_FLAG_OVERLAPPED, //OVERLAPPED!
								NULL);
	if( hDir == INVALID_HANDLE_VALUE ) return NULL;

	const DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_SIZE; //|FILE_NOTIFY_CHANGE_ATTRIBUTES;
	
	DirWatchInfo * pDirInfo = new DirWatchInfo( hDir, path, changeHandler, dwNotifyFilter, watchSubDirs, wxEmptyString, wxEmptyString, 0);

	// Create a IO completion port/or associate this key with
	// the existing IO completion port
	const bool isFirst = (m_hCompPort == NULL);
	m_hCompPort = CreateIoCompletionPort(hDir, 
										m_hCompPort, //if m_hCompPort is NULL, hDir is associated with a NEW completion port,
													 //if m_hCompPort is NON-NULL, hDir is associated with the existing completion port that the handle m_hCompPort references
										(DWORD)pDirInfo, //the completion 'key'... this ptr is returned from GetQueuedCompletionStatus() when one of the events in the dwChangesToWatchFor filter takes place
										0);
	if( m_hCompPort == NULL ) goto error;

	// If the thread isn't running start it....
	if (isFirst) {
		Create();
		Run();
	}

	// Signal the thread to issue the initial call to ReadDirectoryChangesW()
	if (!pDirInfo->StartMonitor( m_hCompPort )) goto error;

	// Add pDirInfo to list of active watches
	m_dirsWatched.push_back(pDirInfo);

	// Return a handle to this watcher
	return pDirInfo;

error:
	delete pDirInfo;
	return NULL;
#endif // __WXMSW__
}

void DirWatcher::UnwatchDirectory(void* handle) {
#if defined(__WXGTK__)
	wxLogDebug(wxT("DirWatcher::%s()"), wxString(__FUNCTION__, wxConvUTF8).c_str());
	if (-1 == m_fd) {
		wxLogDebug(wxT("inotify was not init!"));
		return;
	}
#elif defined(__WXMSW__)
	if (m_hCompPort == NULL) return; // all dirs are already unwatched
#endif
	DirWatchInfo* pDirInfo = (DirWatchInfo*)handle;

	// Check if the handle is valid
	std::vector<DirWatchInfo*>::iterator p = find(m_dirsWatched.begin(), m_dirsWatched.end(), pDirInfo);
	if (p == m_dirsWatched.end()) return;

	// Unwatch the dir
#if defined(__WXGTK__)
	inotify_rm_watch(m_fd, pDirInfo->GetWD());
#elif defined(__WXMSW__)
	pDirInfo->UnwatchDirectory(m_hCompPort);
#endif
	m_dirsWatched.erase(p);
	delete pDirInfo;
}

#ifdef __WXGTK__
void DirWatcher::UnwatchDirByName(const wxString& path, bool unwatchSubDir /* = false */) {
	DirWatchInfo * pDirInfo;
	wxLogDebug(wxT("DirWatcher::%s() path=%s"), wxString(__FUNCTION__, wxConvUTF8).c_str(), path.c_str());
	for (unsigned int i = 0; i < m_dirsWatched.size(); ++i) {
		if((pDirInfo = m_dirsWatched[i]) != NULL ) {
			bool isNeedToRemove = false;
			if (true == unwatchSubDir) {
				// remove it in case of child directory
				if (-1 != pDirInfo->GetPath().Find(path.c_str())) {
					isNeedToRemove = true;
					wxLogDebug(wxT("DirWatcher::%s() remove subdir"), wxString(__FUNCTION__,
						 wxConvUTF8).c_str());
				}
			}
			if ((pDirInfo->GetPath() == path) || (true == isNeedToRemove)) {
				// this one
				std::vector<DirWatchInfo*>::iterator p = find(m_dirsWatched.begin(),
					m_dirsWatched.end(), pDirInfo);
				if (p != m_dirsWatched.end()) {
					m_dirsWatched.erase(p);
				}
			}
		}
	}

}
#endif

void DirWatcher::UnwatchAllDirectories() {
	DirWatchInfo * pDirInfo;
#if defined(__WXGTK__)
	wxLogDebug(wxT("DirWatcher::%s()"), wxString(__FUNCTION__, wxConvUTF8).c_str());
	for(unsigned int i = 0; i < m_dirsWatched.size(); ++i) {
		if( (pDirInfo = m_dirsWatched[i]) != NULL ) {
			UnwatchDirectory(pDirInfo);
			m_dirsWatched[i] = NULL;
		}
	}
	m_dirsWatched.clear();
#elif defined(__WXMSW__)
	wxASSERT(m_hCompPort);

	//Unwatch each of the watched directories
	//and delete the CDirWatchInfo associated w/ that directory...
	for(unsigned int i = 0; i < m_dirsWatched.size(); ++i) {
		if( (pDirInfo = m_dirsWatched[i]) != NULL ) {
			pDirInfo->UnwatchDirectory(m_hCompPort);
			m_dirsWatched[i] = NULL;
		}
	}
	m_dirsWatched.clear();
	
	// Kill off the thread
	PostQueuedCompletionStatus(m_hCompPort, 0, 0, NULL);//The thread will catch this and exit the thread

	// Close the completion port...
	CloseHandle( m_hCompPort );
	m_hCompPort = NULL;
#endif // __WXMSW__
}

#ifdef __WXGTK__
wxEvtHandler* DirWatcher::getEvtHandlerByWD(int desc) {
	wxEvtHandler *res = NULL;
	for(unsigned int i = 0; i < m_dirsWatched.size(); ++i) {
		if (NULL != m_dirsWatched[i]) {
			if(desc == m_dirsWatched[i]->GetWD()) {
				res = &m_dirsWatched[i]->GetHandler();
				break;
			}
		}
	}
	return res;
}
DirWatcher::DirWatchInfo* DirWatcher::getDirWatchInfoByWD(int desc) {
	DirWatcher::DirWatchInfo* res = NULL;
	for(unsigned int i = 0; i < m_dirsWatched.size(); ++i) {
		if (NULL != m_dirsWatched[i]) {
			if(desc == m_dirsWatched[i]->GetWD()) {
				res = m_dirsWatched[i];
				break;
			}
		}
	}
	return res;
}
#endif

// ---- DirWatchInfo --------------------------------------------------------------------------------
#ifdef __WXMSW__
DirWatcher::DirWatchInfo::DirWatchInfo(HANDLE hDir, const wxString& directoryName, 
					  wxEvtHandler& changeHandler, 
					  DWORD dwChangeFilter, bool watchSubDir, 
					  const wxString& includeFilter,
					  const wxString& excludeFilter,
					  DWORD dwFilterFlags)
: m_hDir(hDir), m_dirName(directoryName), m_changeHandler(changeHandler), m_watchSubDir(watchSubDir),
  m_includeFilter(includeFilter), m_excludeFilter(excludeFilter), m_changeFilter(dwChangeFilter),
  m_filterFlags(dwFilterFlags), m_readDirSuccess(false), m_startStopCond(m_sigMutex) {
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));

	// the mutex should be initially locked
	m_sigMutex.Lock();
}

bool DirWatcher::DirWatchInfo::StartMonitor(HANDLE hCompPort) {

	m_cs.Enter(); // Guard the properties of this object 
		
	m_runningState = RUNNING_STATE_START_MONITORING;//set the state member to indicate that the object is to START monitoring the specified directory
	PostQueuedCompletionStatus(hCompPort, sizeof(this), (DWORD)this, &m_Overlapped);//make the thread waiting on GetQueuedCompletionStatus() wake up

	m_cs.Leave(); // unlock this object so that the thread can get at them...

	// Wait for signal that the initial call 
	// to ReadDirectoryChanges has been made
	if (m_startStopCond.WaitTimeout(10*10000) != wxCOND_NO_ERROR) return false;
	
	return m_readDirSuccess; // This value is set in the worker thread when it first calls ReadDirectoryChangesW().
}

void DirWatcher::DirWatchInfo::UnwatchDirectory(HANDLE hCompPort) {
	wxASSERT( hCompPort );

	// Signal that the worker thread is to stop watching the directory
	SignalShutdown(hCompPort);
	
	//and wait for the thread to signal that it has shutdown
	m_startStopCond.Wait();
}

void DirWatcher::DirWatchInfo::SignalShutdown(HANDLE hCompPort) {
	m_cs.Enter(); // Guard the properties of this object 

	// Set the state member to indicate that the object is to stop monitoring the 
	// directory that this CDirWatchInfo is responsible for...
	m_runningState = RUNNING_STATE_STOP;
	// Put this object in the I/O completion port... GetQueuedCompletionStatus() will return it inside the worker thread.
	PostQueuedCompletionStatus(hCompPort, sizeof(DirWatchInfo*), (DWORD)this, &m_Overlapped);

	m_cs.Leave(); // unlock this object so that the thread can get at them...
}

bool DirWatcher::DirWatchInfo::CloseDirectoryHandle() {
	bool b = true;
	if( m_hDir != INVALID_HANDLE_VALUE )
	{
		b = (CloseHandle(m_hDir) != 0);
		m_hDir = INVALID_HANDLE_VALUE;
	}
	return b;
}

void DirWatcher::DirWatchInfo::ProcessNotification(DWORD& ref_dwReadBuffer_Offset) {
	FileNotifyInformation notify_info( (LPBYTE)m_Buffer, READ_DIR_CHANGE_BUFFER_SIZE);

	DWORD dwLastAction = 0;
	ref_dwReadBuffer_Offset = 0UL;

	wxDirWatcherEvent event;

	do
	{
		//The FileName member of the FILE_NOTIFY_INFORMATION
		//structure contains the NAME of the file RELATIVE to the 
		//directory that is being watched...
		//ie, if watching C:\Temp and the file C:\Temp\MyFile.txt is changed,
		//the file name will be "MyFile.txt"
		//If watching C:\Temp, AND you're also watching subdirectories
		//and the file C:\Temp\OtherFolder\MyOtherFile.txt is modified,
		//the file name will be "OtherFolder\MyOtherFile.txt
		const wxString strFileName = notify_info.GetFileNameWithPath(m_dirName);

		// Apply Filters

		event.SetChangedFile(strFileName);
		
		switch( notify_info.GetAction() )
		{
		case FILE_ACTION_ADDED:		// a file was added!
			event.SetChangeType(DIRWATCHER_FILE_ADDED);
			break;

		case FILE_ACTION_REMOVED:	//a file was removed
			event.SetChangeType(DIRWATCHER_FILE_REMOVED);
			break;

		case FILE_ACTION_MODIFIED:
			//a file was changed
			event.SetChangeType(DIRWATCHER_FILE_MODIFIED);
			break;

		case FILE_ACTION_RENAMED_OLD_NAME:
			{//a file name has changed, and this is the OLD name
			 //This record is followed by another one w/
			 //the action set to FILE_ACTION_RENAMED_NEW_NAME (contains the new name of the file
				event.SetChangeType(DIRWATCHER_FILE_RENAMED);

				if( notify_info.GetNextNotifyInformation() )
				{//there is another PFILE_NOTIFY_INFORMATION record following the one we're working on now...
				 //it will be the record for the FILE_ACTION_RENAMED_NEW_NAME record

					wxASSERT( notify_info.GetAction() == FILE_ACTION_RENAMED_NEW_NAME );//making sure that the next record after the OLD_NAME record is the NEW_NAME record

					// Get the new file name
					const wxString strNewFileName = notify_info.GetFileNameWithPath(m_dirName);
					event.SetNewFile(strNewFileName);
				}
				else
				{
					//this OLD_NAME was the last record returned by ReadDirectoryChangesW
					//I will have to call ReadDirectoryChangesW again so that I will get 
					//the record for FILE_ACTION_RENAMED_NEW_NAME

					//Adjust an offset so that when I call ReadDirectoryChangesW again,
					//the FILE_NOTIFY_INFORMATION will be placed after 
					//the record that we are currently working on.
					notify_info.CopyCurrentRecordToBeginningOfBuffer( ref_dwReadBuffer_Offset );
					wxASSERT(notify_info.GetNextNotifyInformation() == NULL);
					return;
				}
			}
			break;

		case FILE_ACTION_RENAMED_NEW_NAME:
			{
				//This should have been handled in FILE_ACTION_RENAMED_OLD_NAME
				wxASSERT( dwLastAction == FILE_ACTION_RENAMED_OLD_NAME );
				wxASSERT( false );//this shouldn't get here
			}
		
		default:
			wxASSERT(false);
		}

		dwLastAction = notify_info.GetAction();
		
		// Send the event
		m_changeHandler.AddPendingEvent(event);
    
	} while( notify_info.GetNextNotifyInformation() );
}

// ---- FileNotifyInformation ----------------------------------------------------------

bool FileNotifyInformation::GetNextNotifyInformation()
{
	if( m_pCurrentRecord 
	&&	m_pCurrentRecord->NextEntryOffset != 0UL)//is there another record after this one?
	{
		//set the current record to point to the 'next' record
		PFILE_NOTIFY_INFORMATION pOld = m_pCurrentRecord;
		m_pCurrentRecord = (PFILE_NOTIFY_INFORMATION) ((LPBYTE)m_pCurrentRecord + m_pCurrentRecord->NextEntryOffset);

		wxASSERT( (DWORD)((BYTE*)m_pCurrentRecord - m_pBuffer) < m_dwBufferSize); // make sure we haven't gone too far
		
		// DEBUG: Make sure entry fits in buffer
		if ((BYTE*)&m_pCurrentRecord->FileName + m_pCurrentRecord->FileNameLength > m_pBuffer + m_dwBufferSize) {
			OutputDebugString(wxT("WARNING: DirBuffer overflow\n"));
		}

		if( (DWORD)((BYTE*)m_pCurrentRecord - m_pBuffer) > m_dwBufferSize )
		{
			//we've gone too far.... this data is hosed.
			//
			// This sometimes happens if the watched directory becomes deleted... remove the FILE_SHARE_DELETE flag when using CreateFile() to get the handle to the directory...
			m_pCurrentRecord = pOld;
		}
					
		return (m_pCurrentRecord != pOld);
	}
	return false;
}

bool FileNotifyInformation::CopyCurrentRecordToBeginningOfBuffer(OUT DWORD & ref_dwSizeOfCurrentRecord)
{
	wxASSERT( m_pBuffer && m_pCurrentRecord );
	if( !m_pCurrentRecord ) return false;

	bool bRetVal = TRUE;

	//determine the size of the current record.
	ref_dwSizeOfCurrentRecord = sizeof( FILE_NOTIFY_INFORMATION );
	//subtract out sizeof FILE_NOTIFY_INFORMATION::FileName[1]
	WCHAR FileName[1];//same as is defined for FILE_NOTIFY_INFORMATION::FileName
	UNREFERENCED_PARAMETER(FileName);
	ref_dwSizeOfCurrentRecord -= sizeof(FileName);   
	//and replace it w/ value of FILE_NOTIFY_INFORMATION::FileNameLength
	ref_dwSizeOfCurrentRecord += m_pCurrentRecord->FileNameLength;

	wxASSERT( (DWORD)((LPBYTE)m_pCurrentRecord + ref_dwSizeOfCurrentRecord) <= m_dwBufferSize );

	wxASSERT( (void*)m_pBuffer != (void*)m_pCurrentRecord );//if this is the case, your buffer is way too small
	if( (void*)m_pBuffer != (void*) m_pCurrentRecord ) {
		wxASSERT( (DWORD)m_pCurrentRecord > (DWORD)m_pBuffer + ref_dwSizeOfCurrentRecord);//will it overlap?
		
		// Copy the m_pCurrentRecord to the beginning of m_pBuffer
		memcpy(m_pBuffer, m_pCurrentRecord, ref_dwSizeOfCurrentRecord);
		bRetVal = TRUE;
		
	}
	//else
	//there was only one record in this buffer, and m_pCurrentRecord is already at the beginning of the buffer
	return bRetVal;
}

DWORD FileNotifyInformation::GetAction() const
{ 
	wxASSERT( m_pCurrentRecord );
	if( m_pCurrentRecord )
		return m_pCurrentRecord->Action;
	return 0UL;
}

wxString FileNotifyInformation::GetFileName() const
{
	if( m_pCurrentRecord )
	{
		return wxString(m_pCurrentRecord->FileName, m_pCurrentRecord->FileNameLength / sizeof(WCHAR));
	}
	return wxString();
}		

static inline bool HasTrailingBackslash(const wxString& str )
{
	return (!str.empty() && str.Last() == wxT('\\'));
}

wxString FileNotifyInformation::GetFileNameWithPath(const wxString& strRootPath) const
{
	wxString strFileName( strRootPath );
	//if( strFileName.Right(1) != _T("\\") )
	if( !HasTrailingBackslash( strRootPath ) )
		strFileName += _T("\\");

	strFileName += GetFileName();
	return strFileName;
}
#endif
