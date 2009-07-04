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

#include "FileActionThread.h"

FileActionThread::FileActionThread(FileAction action, const wxArrayString& sources, bool allowUndo):
	m_action(action), m_paths(sources), m_allowUndo(allowUndo) 
{
	wxASSERT(m_action == FILEACTION_DELETE || m_action == FILEACTION_DELETE_SILENT);
	Create();
	Run();
}

FileActionThread::FileActionThread(FileAction action, const wxArrayString& sources, const wxString& targetDir, bool allowUndo):
m_action(action), m_paths(sources), m_targetDir(targetDir), m_allowUndo(allowUndo) 
{
	Create();
	Run();
}

void* FileActionThread::Entry() {
	DoFileAction(m_action, m_paths, m_targetDir, m_allowUndo);
	return NULL;
}

void FileActionThread::DoFileAction(FileAction action, const wxArrayString& sources, const wxString& targetDir, bool allowUndo) { //static
	if (sources.empty()) return;

#ifdef __WXMSW__

	// Paths have to be double-null terminated
	wxString filePaths;
	for (unsigned int i = 0; i < sources.GetCount(); ++i) {
		filePaths += sources[i] + wxT('\0');
	}
	const wxString target = targetDir + wxT('\0');

	// The File Operation Structure
	SHFILEOPSTRUCT SHFileOp;
	ZeroMemory(&SHFileOp, sizeof(SHFILEOPSTRUCT));
	SHFileOp.hwnd = NULL;
	SHFileOp.pFrom = filePaths.c_str();
	SHFileOp.fFlags = FOF_NOCONFIRMMKDIR;
	if (allowUndo) SHFileOp.fFlags |= FOF_ALLOWUNDO;

	switch (action) {
	case FILEACTION_DELETE:
		SHFileOp.wFunc = FO_DELETE;
		break;

	case FILEACTION_DELETE_SILENT:
		SHFileOp.wFunc = FO_DELETE;
		SHFileOp.fFlags |= FOF_SILENT|FOF_NOCONFIRMATION|FOF_NOERRORUI;
		break;

	case FILEACTION_COPY:
		SHFileOp.wFunc = FO_COPY;
		SHFileOp.pTo = target.c_str();
		break;

	case FILEACTION_MOVE:
		SHFileOp.wFunc = FO_MOVE;
		SHFileOp.pTo = target.c_str();
		break;

	default:
		wxASSERT(false);
		return;
	}

	// Do the file operation
	SHFileOperation(&SHFileOp);

#endif //__WXMSW__

	return;
}

