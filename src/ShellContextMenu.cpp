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

#include "ShellContextMenu.h"
#include <malloc.h> // for _msize

#define MIN_ID 1
#define MAX_ID 10000

// Some header does not define GUID
struct __declspec(uuid("000214e6-0000-0000-c000-000000000046")) IShellFolder; 

ShellContextMenu::ShellContextMenu()
{
	m_psfFolder = NULL;
	m_pidlArray = NULL;
	m_Menu = NULL;

	m_IContext2 = NULL;
	m_IContext3 = NULL;
}

ShellContextMenu::~ShellContextMenu()
{
	// free all allocated datas
	if (m_psfFolder && bDelete)
		m_psfFolder->Release ();
	m_psfFolder = NULL;
	FreePIDLArray (m_pidlArray);
	m_pidlArray = NULL;

	if (m_Menu)
		delete m_Menu;
}

UINT ShellContextMenu::ShowContextMenu(wxWindow* pWnd, wxPoint pt, bool showExtendedItems)
{
	int iMenuType = 0;	// to know which version of IContextMenu is supported
	LPCONTEXTMENU pContextMenu;	// common pointer to IContextMenu and higher version interface
   
	if (!GetContextMenu ((void**) &pContextMenu, iMenuType))	
		return (0);	// something went wrong

	if (!m_Menu)
		m_Menu = new wxMenu;

	// lets fill the our popupmenu
	UINT flags = CMF_NORMAL | CMF_EXPLORE;
	if (showExtendedItems)
		flags |= CMF_EXTENDEDVERBS;

	pContextMenu->QueryContextMenu(GetHmenuOf(m_Menu), m_Menu->GetMenuItemCount(), MIN_ID, MAX_ID, flags);
 
	// set context menu
	if (iMenuType > 1)	// only subclass if its version 2 or 3
	{
		if (iMenuType == 2)
			m_IContext2 = (LPCONTEXTMENU2) pContextMenu;
		else	// version 3
			m_IContext3 = (LPCONTEXTMENU3) pContextMenu;
	}

	//UINT idCommand = m_Menu->TrackPopupMenu (TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, pWnd);
	UINT idCommand = ::TrackPopupMenu(GetHmenuOf(m_Menu), TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, 0, GetHwndOf(pWnd), NULL);

	if (idCommand >= MIN_ID && idCommand <= MAX_ID)	// see if returned idCommand belongs to shell menu entries
	{
		InvokeCommand (pContextMenu, idCommand - MIN_ID);	// execute related command
		idCommand = 0;
	}
	
	pContextMenu->Release();
	m_IContext2 = NULL;
	m_IContext3 = NULL;

	return (idCommand);
}

void ShellContextMenu::SetObjects(const wxArrayString &strArray)
{
	// free all allocated datas
	if (m_psfFolder && bDelete)
		m_psfFolder->Release ();
	m_psfFolder = NULL;
	FreePIDLArray (m_pidlArray);
	m_pidlArray = NULL;
	
	// get IShellFolder interface of Desktop (root of shell namespace)
	IShellFolder * psfDesktop = NULL;
	SHGetDesktopFolder (&psfDesktop);	// needed to obtain full qualified pidl

	// ParseDisplayName creates a PIDL from a file system path relative to the IShellFolder interface
	// but since we use the Desktop as our interface and the Desktop is the namespace root
	// that means that it's a fully qualified PIDL, which is what we need
	LPITEMIDLIST pidl = NULL;
	
	psfDesktop->ParseDisplayName(NULL, 0, (LPOLESTR)strArray[0].c_str(), NULL, &pidl, NULL);

	// now we need the parent IShellFolder interface of pidl, and the relative PIDL to that interface
	LPITEMIDLIST pidlItem = NULL;	// relative pidl
	SHBindToParentEx (pidl, IID_IShellFolder, (void **) &m_psfFolder, NULL);
	//free (pidlItem);

	// get interface to IMalloc (need to free the PIDLs allocated by the shell functions)
	LPMALLOC lpMalloc = NULL;
	SHGetMalloc (&lpMalloc);
	lpMalloc->Free (pidl);

	// now we have the IShellFolder interface to the parent folder specified in the first element in strArray
	// since we assume that all objects are in the same folder (as it's stated in the MSDN)
	// we now have the IShellFolder interface to every objects parent folder
	
	IShellFolder * psfFolder = NULL;
	nItems = strArray.GetCount();
	for (int i = 0; i < nItems; i++)
	{
		psfDesktop->ParseDisplayName (NULL, 0, (LPOLESTR)strArray[i].c_str(), NULL, &pidl, NULL);

		// get relative pidl via SHBindToParent
		HRESULT hr = SHBindToParentEx(pidl, IID_IShellFolder, (void **) &psfFolder, (LPCITEMIDLIST *) &pidlItem);
		if (FAILED(hr)) break;

		m_pidlArray = (LPITEMIDLIST *) realloc (m_pidlArray, (i + 1) * sizeof (LPITEMIDLIST));
		m_pidlArray[i] = CopyPIDL(pidlItem);	// copy relative pidl to pidlArray
		
		free (pidlItem);
		if (lpMalloc) lpMalloc->Free(pidl);		// free pidl allocated by ParseDisplayName
		if (psfFolder) psfFolder->Release();
	}
	lpMalloc->Release ();
	psfDesktop->Release ();

	bDelete = TRUE;	// indicates that m_psfFolder should be deleted by CShellContextMenu
}

void ShellContextMenu::FreePIDLArray(LPITEMIDLIST *pidlArray)
{
	if (!pidlArray)
		return;

	int iSize = _msize (pidlArray) / sizeof (LPITEMIDLIST);
	for (int i = 0; i < iSize; i++)
		free (pidlArray[i]);
	free (pidlArray);
}


LPITEMIDLIST ShellContextMenu::CopyPIDL (LPCITEMIDLIST pidl, int cb)
{
	if (cb == -1)
		cb = GetPIDLSize (pidl); // Calculate size of list.

    LPITEMIDLIST pidlRet = (LPITEMIDLIST) calloc (cb + sizeof (USHORT), sizeof (BYTE));
    if (pidlRet)
		CopyMemory(pidlRet, pidl, cb);

    return (pidlRet);
}


UINT ShellContextMenu::GetPIDLSize (LPCITEMIDLIST pidl)
{  
	if (!pidl) 
		return 0;
	int nSize = 0;
	LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
	while (pidlTemp->mkid.cb)
	{
		nSize += pidlTemp->mkid.cb;
		pidlTemp = (LPITEMIDLIST) (((LPBYTE) pidlTemp) + pidlTemp->mkid.cb);
	}
	return nSize;
}

LPBYTE ShellContextMenu::GetPIDLPos (LPCITEMIDLIST pidl, int nPos)
{
	if (!pidl)
		return 0;
	int nCount = 0;
	
	BYTE * pCur = (BYTE *) pidl;
	while (((LPCITEMIDLIST) pCur)->mkid.cb)
	{
		if (nCount == nPos)
			return pCur;
		nCount++;
		pCur += ((LPCITEMIDLIST) pCur)->mkid.cb;	// + sizeof(pidl->mkid.cb);
	}
	if (nCount == nPos) 
		return pCur;
	return NULL;
}


int ShellContextMenu::GetPIDLCount (LPCITEMIDLIST pidl)
{
	if (!pidl)
		return 0;

	int nCount = 0;
	BYTE*  pCur = (BYTE *) pidl;
	while (((LPCITEMIDLIST) pCur)->mkid.cb)
	{
		nCount++;
		pCur += ((LPCITEMIDLIST) pCur)->mkid.cb;
	}
	return nCount;
}

// this is workaround function for the Shell API Function SHBindToParent
// SHBindToParent is not available under Win95/98
HRESULT ShellContextMenu::SHBindToParentEx (LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast)
{
	HRESULT hr = 0;
	if (!pidl || !ppv)
		return E_POINTER;
	
	int nCount = GetPIDLCount (pidl);
	if (nCount == 0)	// desktop pidl of invalid pidl
		return E_POINTER;

	IShellFolder * psfDesktop = NULL;
	SHGetDesktopFolder (&psfDesktop);
	if (nCount == 1)	// desktop pidl
	{
		if ((hr = psfDesktop->QueryInterface(riid, ppv)) == S_OK)
		{
			if (ppidlLast) 
				*ppidlLast = CopyPIDL (pidl);
		}
		psfDesktop->Release ();
		return hr;
	}

	LPBYTE pRel = GetPIDLPos (pidl, nCount - 1);
	LPITEMIDLIST pidlParent = NULL;
	pidlParent = CopyPIDL (pidl, pRel - (LPBYTE) pidl);
	IShellFolder * psfFolder = NULL;
	
	if ((hr = psfDesktop->BindToObject (pidlParent, NULL, __uuidof (psfFolder), (void **) &psfFolder)) != S_OK)
	{
		free (pidlParent);
		psfDesktop->Release ();
		return hr;
	}
	if ((hr = psfFolder->QueryInterface (riid, ppv)) == S_OK)
	{
		if (ppidlLast)
			*ppidlLast = CopyPIDL ((LPCITEMIDLIST) pRel);
	}
	free (pidlParent);
	psfFolder->Release ();
	psfDesktop->Release ();
	return hr;
}

// this functions determines which version of IContextMenu is avaibale for those objects (always the highest one)
// and returns that interface
BOOL ShellContextMenu::GetContextMenu (void ** ppContextMenu, int & iMenuType)
{
	*ppContextMenu = NULL;
	LPCONTEXTMENU icm1 = NULL;
	
	// first we retrieve the normal IContextMenu interface (every object should have it)
	if (!m_psfFolder) return (FALSE);
	m_psfFolder->GetUIObjectOf (NULL, nItems, (LPCITEMIDLIST *) m_pidlArray, IID_IContextMenu, NULL, (void**) &icm1);

	if (icm1)
	{	// since we got an IContextMenu interface we can now obtain the higher version interfaces via that
		if (icm1->QueryInterface (IID_IContextMenu3, ppContextMenu) == NOERROR)
			iMenuType = 3;
		else if (icm1->QueryInterface (IID_IContextMenu2, ppContextMenu) == NOERROR)
			iMenuType = 2;

		if (*ppContextMenu) 
			icm1->Release(); // we can now release version 1 interface, cause we got a higher one
		else 
		{	
			iMenuType = 1;
			*ppContextMenu = icm1;	// since no higher versions were found
		}							// redirect ppContextMenu to version 1 interface
	}
	else
		return (FALSE);	// something went wrong
	
	return (TRUE); // success
}

void ShellContextMenu::InvokeCommand (LPCONTEXTMENU pContextMenu, UINT idCommand)
{
	CMINVOKECOMMANDINFO cmi = {0};
	cmi.cbSize = sizeof (CMINVOKECOMMANDINFO);
	cmi.lpVerb = (LPSTR) MAKEINTRESOURCE (idCommand);
	cmi.nShow = SW_SHOWNORMAL;
	
	pContextMenu->InvokeCommand (&cmi);
}


bool ShellContextMenu::HookWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res)
{
	if (!m_IContext2 && !m_IContext3) return false;
	
	switch (message)
	{ 
	case WM_MENUCHAR:	// only supported by IContextMenu3
		if (m_IContext3)
		{
			LRESULT lResult = 0;
			m_IContext3->HandleMenuMsg2 (message, wParam, lParam, &lResult);
			res = lResult;
			return true;
		}
		break;

	case WM_DRAWITEM:
	case WM_MEASUREITEM:
		if (wParam) 
			break; // if wParam != 0 then the message is not menu-related
  
	case WM_INITMENUPOPUP:
		if (m_IContext2) {
			if (SUCCEEDED((m_IContext2->HandleMenuMsg (message, wParam, lParam)))) {
				res = 0;
				return true;
			}
		}
		else {	// version 3
			LRESULT lres;
			//wxLogDebug(wxT("HandleMenuMsg %x %x), wParam, lParam);
			HRESULT res = m_IContext3->HandleMenuMsg2(message, wParam, lParam, &lres);
			if (res != S_OK) {
				wxLogDebug(wxT("  HandleMenuMsg failed %x"), res);
			}
			else {
				wxLogDebug(wxT("  HandleMenuMsg succeeded %d"), lres);
				res = lres;
				return true;
			}
		}
		break;

	default:
		break;
	}

	return false;
}