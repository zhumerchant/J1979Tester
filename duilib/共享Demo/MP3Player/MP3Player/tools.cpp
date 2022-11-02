/**
 * @author WACR
 * $Id: EnterValue.cpp,v 1.5 2008/10/27 23:42:09 wacr $
 */

/*
Original code copyright (c) 2008 wacr

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#include "StdAfx.h"
#include "tools.h" 
#include <shlguid.h>
#include <comdef.h>

void Tools::FindTarget(LPCTSTR str)
{
	HRESULT hres; 
	IShellLink *psl; 
	ITEMIDLIST *pidl;
	IPersistFile *ppf; 

    ::CoInitialize(NULL);

	// Get a pointer to the IShellLink interface. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
			    IID_IShellLink, (LPVOID*)&psl); 
	if (SUCCEEDED(hres)) 
	{ 
		// 设置目标文件
		psl->SetPath(str);
		/// 获得目标文件的ITEMIDLIST
		psl->GetIDList(&pidl);

		// Get a pointer to the IPersistFile interface. 
		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
		if (SUCCEEDED(hres)) 
		{ 
			WCHAR wsz[MAX_PATH]; 
#ifdef _UNICODE
			wcscpy(wsz, str);
#else
			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, str, -1, wsz, MAX_PATH); 
#endif 
 
			// Load the shortcut. 
			hres = ppf->Load(wsz, STGM_READ); 

			if (SUCCEEDED(hres)) 
			{ 
				/// 获得快捷方式的ITEMIDLIST
				psl->GetIDList(&pidl);
			}
			ppf->Release();
		}

		/// 打开文件夹并选中项目
		XZSHOpenFolderAndSelectItems(pidl);

		psl->Release();
	}
     ::CoUninitialize();
}
//--------------------------------------------
HRESULT Tools::GetShellFolderViewDual(ITEMIDLIST* pidl, IShellFolderViewDual** ppIShellFolderViewDual)
{
	IWebBrowserApp* pIWebBrowserApp;
	IDispatch* pDoc;
	HWND hWnd;
	HRESULT hr;
	HINSTANCE ghSHDOCVW;


	HRESULT (WINAPI*gpfSHGetIDispatchForFolder)(ITEMIDLIST* pidl, IWebBrowserApp** ppIWebBrowserApp);

	*ppIShellFolderViewDual = NULL;

	ghSHDOCVW = LoadLibrary(_T("SHDOCVW.DLL"));
	if (ghSHDOCVW == NULL)
		return FALSE;

	pIWebBrowserApp=NULL;
	gpfSHGetIDispatchForFolder = (HRESULT (WINAPI*)(ITEMIDLIST*, IWebBrowserApp**)) GetProcAddress(ghSHDOCVW, "SHGetIDispatchForFolder");
	if (gpfSHGetIDispatchForFolder == NULL)
		return FALSE;

	/// 调用未公开的API函数 SHGetIDispatchForFolder
	if (SUCCEEDED(gpfSHGetIDispatchForFolder(pidl, &pIWebBrowserApp))) 
	{
		if (SUCCEEDED(pIWebBrowserApp->get_HWND((long*)&hWnd))) 
		{
			SetForegroundWindow(hWnd);
			ShowWindow(hWnd, SW_SHOWNORMAL);
		}

		if (SUCCEEDED(hr = pIWebBrowserApp->get_Document(&pDoc)))
		{
			pDoc->QueryInterface(IID_IShellFolderViewDual, (void**) ppIShellFolderViewDual);
			pDoc->Release();
		}

		pIWebBrowserApp->Release();
	}
	FreeLibrary(ghSHDOCVW);

	return TRUE;
}

BOOL Tools::XZSHOpenFolderAndSelectItems(ITEMIDLIST *pidlFolder)
{ 
	ITEMIDLIST *pidl, *pidl2;
	ITEMIDLIST* pIdlFile;
	USHORT cb;
	IShellFolderViewDual* pIShellFolderViewDual;

	HRESULT (WINAPI *gpfSHOpenFolderAndSelectItems)(LPCITEMIDLIST *pidlFolder, UINT cidl, LPCITEMIDLIST *apidl, DWORD dwFlags);
	HINSTANCE ghShell32;
/// 只有WinXp及以上及系统才支持SHOpenFolderAndSelectItems() API
/// 那其它系统该怎么实现这个功能呢？只能采用其它的方法来处理
/// 首先用XP跟踪到SHOpenFolderAndSelectItems()API中，看它是如何处理的，再用同样的方法去实现
/// 其它系统的这个功能使用工具 VC6 .net 2003 MSDN Ollydbg v1.10中文版

	ghShell32 = LoadLibrary(_T("Shell32.DLL"));
	if (ghShell32 == NULL)
		return FALSE;

	gpfSHOpenFolderAndSelectItems = (HRESULT (WINAPI*)(LPCITEMIDLIST*, UINT, LPCITEMIDLIST*, DWORD)) GetProcAddress(ghShell32, "SHOpenFolderAndSelectItems");
	if (gpfSHOpenFolderAndSelectItems != NULL)
	{
		/// 可以获得SHOpenFolderAndSelectItems()函数的API地址
		if (SUCCEEDED(gpfSHOpenFolderAndSelectItems((LPCITEMIDLIST*)pidlFolder,0,(LPCITEMIDLIST*)NULL,0))) 
		{
			///直接调用系统的功能
			FreeLibrary(ghShell32);
			return TRUE;
		}
		FreeLibrary(ghShell32);
		return FALSE;
	}
	FreeLibrary(ghShell32);

	/// 当操作系统不支持SHOpenFolderAndSelectItems()函数的API时的处理，
	/// 自已动手写一个与系统功能相同的代码
	pidl = pidlFolder;
	pIdlFile = pidl;
	/// 找出目标文件的偏移量
	while (cb = pIdlFile->mkid.cb) 
	{
		pidl2 = pIdlFile;
		pIdlFile = (ITEMIDLIST*)((BYTE*)pIdlFile + cb);
	}

	cb = pidl2->mkid.cb;
	pidl2->mkid.cb = 0;

	/// 打开目标文件所在的文件夹
	if (SUCCEEDED(GetShellFolderViewDual(pidl, &pIShellFolderViewDual))) 
	{
		pidl2->mkid.cb = cb;
		// 0 Deselect the item.  
		// 1 Select the item.  
		// 3 Put the item in edit mode.  
		// 4 Deselect all but the specified item.  
		// 8 Ensure the item is displayed in the view.  
		// 0x10 Give the item the focus.  

		//pidl2
        VARIANT bszFile; 
		memset(&bszFile, 0, sizeof(bszFile));
		// walk through entries in the list and accumulate their size

		UINT cbTotal = 0;
		SAFEARRAY *psa = NULL;
		LPCITEMIDLIST pidlWalker = pidl2;

		while (pidlWalker->mkid.cb)
		{
			cbTotal += pidlWalker->mkid.cb;
			pidlWalker = (LPCITEMIDLIST)
				(((LPBYTE)pidlWalker) + pidlWalker->mkid.cb);
		}

		// add the base structure size
		cbTotal += sizeof(ITEMIDLIST);

		// get a safe array for them
		psa = SafeArrayCreateVector(VT_UI1, 0, cbTotal);
 
		// copy it and set members
		if (psa != NULL)
		{
			memcpy(psa->pvData, (LPBYTE) pidl2, cbTotal);
			bszFile.vt = VT_ARRAY | VT_UI1;
			bszFile.parray = psa;
		}
 

		if(pIShellFolderViewDual != NULL)
		{
			/// 选中相应的选项
			pIShellFolderViewDual->SelectItem(&bszFile, 0x1d);
			pIShellFolderViewDual->Release();
		}
		return TRUE;
	}

	return FALSE;
}



