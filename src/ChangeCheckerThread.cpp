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

#include "ChangeCheckerThread.h"
#include <wx/filename.h>

using namespace std;

DEFINE_EVENT_TYPE(wxEVT_FILESCHANGED)

ChangeCheckerThread::ChangeCheckerThread(const vector<ChangePath>& paths, wxEvtHandler& evtHandler, RemoteThread& rt, ChangeCheckerThread*& pointer)
: m_paths(paths), m_evtHandler(evtHandler), m_remoteThread(rt), m_pointer(pointer) {
	// Start thread
	m_pointer = this;
	Create();
	Run();
}

void* ChangeCheckerThread::Entry() {
	if (m_paths.empty()) return NULL;

	wxArrayString changedFiles;
	vector<wxDateTime> modDates;

	for (vector<ChangePath>::const_iterator p = m_paths.begin(); p != m_paths.end(); ++p) {
		wxDateTime modDate;
		if (p->isModified) {
			// Some file types (like bundle items) may already have been checked
			modDate = p->skipDate;
		}
		else {
			if (p->remoteProfile) {
				continue; // Checking remote files is disabled as it did not always give correct results
				/*modDate = m_remoteThread.GetModDateLocking(p->path, *p->remoteProfile);
	#ifdef __WXDEBUG__
				if (modDate.IsValid()) wxLogDebug(wxT("Checking remote date for %s - %s"), p->path.c_str(), modDate.Format(wxT("%x %X")).c_str());
				else wxLogDebug(wxT("Checking remote date for %s - invalid"), p->path.c_str());
	#endif*/
			}
			else {
				// Check if file exists
				wxFileName fn(p->path);
				if (!fn.FileExists()) continue;

				modDate = fn.GetModificationTime();
			}

			// file could be locked by another app
			// or the remote site could be unavailable
			if (!modDate.IsValid()) continue;

			// Check if this change have been marked to be skipped
			if (p->skipDate.IsValid() && modDate == p->skipDate) continue;

			bool isChanged = false;
			if (!p->date.IsValid()) isChanged = true;
			else {
				// Windows does not really handle the minor parts of file dates
				wxDateTime mDate(p->date);
				mDate.SetMillisecond(0);
				modDate.SetMillisecond(0);

				if (mDate != modDate) isChanged = true;
			}

			if (!isChanged) continue;
		}

		changedFiles.Add(p->path);
		modDates.push_back(modDate);
	}

	// Send the event
	if (!changedFiles.IsEmpty()) {
		wxFilesChangedEvent event(changedFiles, modDates);
		m_evtHandler.AddPendingEvent(event);
	}

	m_pointer = NULL; // let parent know that thread is done
	return NULL;
}
