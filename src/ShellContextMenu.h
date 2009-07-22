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

#ifndef __SHELLCONTEXTMENU_H__
#define __SHELLCONTEXTMENU_H__

#include "wx/wxprec.h"
#include <shlobj.h>

class ShellContextMenu {
public:
	ShellContextMenu();
	virtual ~ShellContextMenu();

	void SetObjects(const wxArrayString& strArray);
	UINT ShowContextMenu(wxWindow* pWnd, wxPoint pt);

	bool HookWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res);

private:
	
	BOOL GetContextMenu (void ** ppContextMenu, int & iMenuType);
	HRESULT SHBindToParentEx (LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast);
	void InvokeCommand (LPCONTEXTMENU pContextMenu, UINT idCommand);

	// PIDL handling
	void FreePIDLArray (LPITEMIDLIST * pidlArray);
	LPITEMIDLIST CopyPIDL (LPCITEMIDLIST pidl, int cb = -1);
	UINT GetPIDLSize (LPCITEMIDLIST pidl);
	LPBYTE GetPIDLPos (LPCITEMIDLIST pidl, int nPos);
	int GetPIDLCount (LPCITEMIDLIST pidl);

	// Member variables
	int nItems;
	bool bDelete;
	wxMenu* m_Menu;
	IShellFolder* m_psfFolder;
	LPITEMIDLIST* m_pidlArray;

	IContextMenu2* m_IContext2;
	IContextMenu3* m_IContext3;
};


#endif // __SHELLCONTEXTMENU_H__