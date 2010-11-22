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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>

class RemoteThread;
class RemoteProfile;

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

	ChangeCheckerThread(const std::vector<ChangePath>& paths, wxEvtHandler& evtHandler, RemoteThread& rt, ChangeCheckerThread*& pointer);
	virtual void* Entry();

private:
	const std::vector<ChangePath> m_paths;
	wxEvtHandler& m_evtHandler;
	RemoteThread& m_remoteThread;
	ChangeCheckerThread*& m_pointer;
};

// Declare custom event
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_FILESCHANGED, 801)
	DECLARE_EVENT_TYPE(wxEVT_FILESDELETED, 802)
END_DECLARE_EVENT_TYPES()

class wxFilesChangedEvent : public wxEvent {
public:
	wxFilesChangedEvent(const wxArrayString& paths, const std::vector<wxDateTime>& dates, int id = 0)
		: wxEvent(id, wxEVT_FILESCHANGED), m_changedFiles(paths), m_modDates(dates) {};
	wxFilesChangedEvent(const wxFilesChangedEvent& event)
		: wxEvent(event), m_changedFiles(event.m_changedFiles), m_modDates(event.m_modDates) {};
	virtual wxEvent* Clone() const {
		return new wxFilesChangedEvent(*this);
	};

	const wxArrayString& GetChangedFiles() const {return m_changedFiles;};
	const std::vector<wxDateTime>& GetModDates() const {return m_modDates;};

private:
	const wxArrayString m_changedFiles;
	const std::vector<wxDateTime> m_modDates;
};
typedef void (wxEvtHandler::*wxFilesChangedEventFunction) (wxFilesChangedEvent&);

#define wxFilesChangedEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (wxFilesChangedEventFunction) &func
#define EVT_FILESCHANGED(func) wx__DECLARE_EVT0(wxEVT_FILESCHANGED, wxFilesChangedEventHandler(func))

class wxFilesDeletedEvent : public wxEvent {
public:
	wxFilesDeletedEvent(const wxArrayString& paths, int id = 0)
		: wxEvent(id, wxEVT_FILESDELETED), m_deletedFiles(paths) {};
	wxFilesDeletedEvent(const wxFilesDeletedEvent& event)
		: wxEvent(event), m_deletedFiles(event.m_deletedFiles) {};
	virtual wxEvent* Clone() const {
		return new wxFilesDeletedEvent(*this);
	};

	const wxArrayString& GetDeletedFiles() const {return m_deletedFiles;};

private:
	const wxArrayString m_deletedFiles;
};
typedef void (wxEvtHandler::*wxFilesDeletedEventFunction) (wxFilesDeletedEvent&);

#define wxFilesDeletedEventHandler(func) (wxObjectEventFunction)(wxEventFunction) (wxFilesDeletedEventFunction) &func
#define EVT_FILESDELETED(func) wx__DECLARE_EVT0(wxEVT_FILESDELETED, wxFilesDeletedEventHandler(func))


#endif //__CHANGECHECKERTHREAD_H__
