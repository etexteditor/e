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

#ifndef __FILEACTIONTHREAD_H__
#define __FILEACTIONTHREAD_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

class FileActionThread : public wxThread {
public:
	enum FileAction {
		FILEACTION_DELETE,
		FILEACTION_DELETE_SILENT,
		FILEACTION_COPY,
		FILEACTION_MOVE
	};
	FileActionThread(FileAction action, const wxArrayString& sources, bool allowUndo);
	FileActionThread(FileAction action, const wxArrayString& sources, const wxString& targetDir, bool allowUndo);
	virtual void* Entry();

	static void DoFileAction(FileAction action, const wxArrayString& sources, const wxString& targetDir=wxEmptyString, bool allowUndo=false);

private:
	FileAction m_action;
	const wxArrayString m_paths;
	const wxString m_targetDir;
	bool m_allowUndo;
};

#endif //__FILEACTIONTHREAD_H__
