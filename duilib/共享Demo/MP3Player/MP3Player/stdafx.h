
#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

// you're defining this?
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <objbase.h>
#include <vector>
#include <string>
 

#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

 

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib.lib")
#   endif
#endif
 
 

#define POPUPMENU_OPENDIR_CLICK        (NOTIFY_USER_START_VALUE+1)
#define POPUPMENU_DELETEITEM_CLICK     (NOTIFY_USER_START_VALUE+2)
#define POPUPMENU_DELETEFILE_CLICK     (NOTIFY_USER_START_VALUE+3)
#define POPUPMENU_CLEARFILE_CLICK     (NOTIFY_USER_START_VALUE+4)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
