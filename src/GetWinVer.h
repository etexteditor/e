// GetWinVer.h  Version 1.1
//
// Copyright (C) 2001-2003 Hans Dietrich
//
// This software is released into the public domain.  
// You are free to use it in any way you like, except
// that you may not sell this source code.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GETWINVER_H
#define GETWINVER_H

#define WUNKNOWNSTR	_T("unknown Windows version")

#define W95STR			_T("Windows 95")
#define W95SP1STR		_T("Windows 95 SP1")
#define W95OSR2STR		_T("Windows 95 OSR2")
#define W98STR			_T("Windows 98")
#define W98SP1STR		_T("Windows 98 SP1")
#define W98SESTR		_T("Windows 98 SE")
#define WMESTR			_T("Windows ME")

#define WNT351STR		_T("Windows NT 3.51")
#define WNT4STR			_T("Windows NT 4")
#define W2KSTR			_T("Windows 2000")
#define WXPSTR			_T("Windows XP")
#define W2003SERVERSTR	_T("Windows 2003 Server")

#define WCESTR			_T("Windows CE")


#define WUNKNOWN	0

#define W9XFIRST	1
#define W95			1
#define W95SP1		2
#define W95OSR2		3
#define W98			4
#define W98SP1		5
#define W98SE		6
#define WME			7
#define W9XLAST		99

#define WNTFIRST	101
#define WNT351		101
#define WNT4		102
#define W2K			103
#define WXP			104
#define W2003SERVER	105
#define WNTLAST		199

#define WCEFIRST	201
#define WCE			201
#define WCELAST		299

BOOL GetWinVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild);

#endif //GETWINVER_H
