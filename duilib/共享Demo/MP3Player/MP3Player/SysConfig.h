#ifndef _SYS_CONFIG_H
#define _SYS_CONFIG_H

#include "WinImplBase.hpp"
#include "resource.h"

class SysConfig
{
public:
	SysConfig(){};
	~SysConfig(){};
public:
	static CDuiString GetSkinFolder()
	{
			#ifdef _DEBUG
					return _T("skin\\MP3PlayerRes\\");
			#else
					return _T("skin\\");
			#endif  
	}
	static UILIB_RESOURCETYPE GetResourceType() 
	{
#ifdef _DEBUG
		return UILIB_FILE;
#else
		return UILIB_ZIPRESOURCE;
#endif
	}

	static LPCTSTR GetResourceID() 
	{
		return MAKEINTRESOURCE(IDR_ZIPRES);
	} 

	static  CDuiString GetZIPFileName() 
	{
		return _T("MP3PlayerRes.zip");
	} 

};

#endif