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

#ifndef __CHANGECHECKERTHREAD_H__
#define __CHANGECHECKERTHREAD_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "RemoteThread.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;


class ChangeCheckerThread : public wxThread {
public:
	class ChangePath {
	public:
		ChangePath() {};
		ChangePath(const ChangePath& cp) {
			path = cp.path.c_str();
			remoteProfile = cp.remoteProfile;
			date = cp.date;
			skipDate = cp.skipDate;
			isModified = cp.isModified;
		};

		wxString path;
		const RemoteProfile* remoteProfile;
		wxDateTime date;
		wxDateTime skipDate;
		bool isModified;
	};

	ChangeCheckerThread(const vector<ChangePath>& paths, wxEvtHandler& evtHandler, RemoteThread& rt, ChangeCheckerThread*& pointer);
	virtual void* Entry();

private:
	const vector<ChangePath> m_paths;
	wxEvtHandler& m_evtHandler;
	RemoteThread& m_remoteThread;
	ChangeCheckerThread*& m_pointer;
};

// Declare custom event
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_FILESCHANGED, 801)
END_DECLARE_EVENT_TYPES()

class wxFilesChangedEvent : public wxEvent {
public:
	wxFilesChangedEvent(const wxArrayString& paths, const vector<wxDateTime>& dates, int id = 0)
		: wxEvent(id, wxEVT_FILESCHANGED), m_changedFiles(paths), m_modDates(dates) {};
	wxFilesChangedEvent(const wxFilesChangedEvent& event)
		: wxEvent(event), m_changedFiles(event.m_changedFiles), m_modDates(event.m_modDates) {};
	virtual wxEvent* Clone() const {
		return new wxFilesChangedEvent(*this);
	};

	const wxArrayString& GetChangedFiles() const {return m_changedFiles;};
	const vector<wxDateTime>& GetModDates() const {return m_modDates;};

private:
	const wxArrayString m_changedFiles;
	const vector<wxDateTime> m_modDates;
};
typedef void (wxEvtHandler::*wxFilesChangedEventFunction) (wxFilesChangedEvent&);

#define wxFilesChangedEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (wxFilesChangedEventFunction) &func
#define EVT_FILESCHANGED(func) wx__DECLARE_EVT0(wxEVT_FILESCHANGED, wxFilesChangedEventHandler(func))


#endif //__CHANGECHECKERTHREAD_H__
