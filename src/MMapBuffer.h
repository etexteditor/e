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

#ifndef _MMAP_BUFFER_H_
#define _MMAP_BUFFER_H_

#include <wx/filename.h>

#ifdef __WXGTK__
  #include <string.h>
  #include <sys/mman.h>
  #include <errno.h>
  #include <wx/file.h>
#endif

class MMapBuffer {
public:
	MMapBuffer() : m_bufptr(NULL) 
#if defined(__WXMSW__)
		,m_hFile(0), m_hMMFile(0)
#endif
	{};

	MMapBuffer(const wxFileName& path) : m_bufptr(NULL) 
#if defined(__WXMSW__)
		,m_hFile(0), m_hMMFile(0)
#endif
	{
		Open(path);
	};

	~MMapBuffer() {
		Close();
	};

	void Open(const wxFileName& path) {
		//wxLogDebug(wxT("MMapBuffer::%s: entry."), wxString(__FUNCTION__, wxConvUTF8).c_str());
#if defined(__WXMSW__)
		Close(); // Close previous mapping

		// Open & Memory map the file
		m_hFile = CreateFile(path.GetFullPath().c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, \
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile) {
			m_hMMFile = CreateFileMapping (m_hFile, NULL, PAGE_READONLY, 0,0, NULL);
			if (m_hMMFile) {
				m_bufptr = (const char *)MapViewOfFile (m_hMMFile, FILE_MAP_READ, 0, 0, 0);
			}
		}
#elif defined(__WXGTK__)
		if (true == m_file.Open(path.GetFullPath(), wxFile::read_write)) {
			if (MAP_FAILED == (m_bufptr = 
				(char*)mmap(NULL, (size_t)Length(), PROT_READ|PROT_WRITE, MAP_SHARED, m_file.fd(), 0))) {
				wxLogDebug(wxT("%s: Can't mmap file %s errno: %i (%s)"),
	                                wxString(__FUNCTION__, wxConvUTF8).c_str(), path.GetFullPath().c_str(), errno, 
					wxString(strerror(errno), wxConvUTF8).c_str());
				m_bufptr = NULL;
			}
		} else {
			wxLogDebug(wxT("%s: Can't open file %s! errno: %i (%s)"),
				wxString(__FUNCTION__, wxConvUTF8).c_str(), path.GetFullPath().c_str(), errno,
				wxString(strerror(errno), wxConvUTF8).c_str());
		}
#endif
	};

	void Close() {
		//wxLogDebug(wxT("MMapBuffer::%s: entry."), wxString(__FUNCTION__, wxConvUTF8).c_str());
#if defined (__WXMSW__)
		if (m_bufptr) UnmapViewOfFile(m_bufptr);
		if (m_hMMFile) CloseHandle(m_hMMFile);
		if (m_hFile) CloseHandle(m_hFile);
		m_bufptr = 0; m_hMMFile = m_hFile = 0;
#elif defined (__WXGTK__)
		if (m_file.IsOpened()) {
			if (NULL != m_bufptr) {
				if (-1 == munmap((void*)m_bufptr, (size_t)Length())) {
					wxLogDebug(wxT("%s: Can't do unmmap! errno: %i (%s)"),
                	                        wxString(__FUNCTION__, wxConvUTF8).c_str(), errno, 
						wxString(strerror(errno), wxConvUTF8).c_str());
				}
			}
			m_file.Close();
		}
#endif
	};

	bool IsOpened() const {
#if defined (__WXMSW__)
		return (m_hFile != 0);
#elif defined (__WXGTK__)
		return (m_file.IsOpened());
#endif
	};

	bool IsMapped() const {
		return (m_bufptr != 0);
	};

	wxFileOffset Length() const {
		wxASSERT(IsOpened());
#if defined (__WXMSW__)
		return GetFileSize(m_hFile, NULL);
#elif defined (__WXGTK__)
		return m_file.Length();
#endif
	};

	const char* data() const {
		wxASSERT(IsMapped());
		return m_bufptr;
	};

	bool IsInMap(const char* pointer) const {
		const wxFileOffset len = Length();
		if (pointer < m_bufptr) return false;
		else if (pointer >= m_bufptr + len) return false;
		else return true;
	};

private:
#if defined (__WXMSW__)
	HANDLE m_hFile;
	HANDLE m_hMMFile;
#elif defined (__WXGTK__)
	wxFile m_file;
#endif
	const char* m_bufptr;
};

#endif

