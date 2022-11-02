#include "stdafx.h"
#include "CommonUtil.h"
#include <mmsystem.h> 
#pragma comment(lib,"winmm.lib")

/*
2012-09-09
wuding guangzhou
主要提供一些工具类，用来处理编码转换
*/

  
DWORD CSECommonUtil::HexToRGB(CDuiString rgbHex)
{
	LPCTSTR pstrValue = rgbHex.GetData();
	if( *pstrValue == _T('#')) 
		pstrValue = ::CharNext(pstrValue);
    
	LPTSTR pstr = NULL;
    return _tcstoul(pstrValue, &pstr, 16);	 
}

CDuiString CSECommonUtil::RGBToHex(unsigned long num)
{
	static TCHAR *hexDigits = _T("0123456789ABCDEF");
	CDuiString rgb;
	for(int i=(3*2)-1; i>=0; i--)
	{
		rgb +=hexDigits[((num >> i*4) & 0xF)];
	}
	return rgb;
} 

CDuiString CSECommonUtil::RGBToHex(int r,int g,int b)
{
	unsigned long rgbNum= ((r &0xFF)<<16 | ((g & 0xFF)<<8) | (b & 0xFF));
	return RGBToHex(rgbNum);
}

BOOL CSECommonUtil::FolderExists(CDuiString s)   
{   
    DWORD attr;    
	attr = ::GetFileAttributes(s);    
    return (attr != (DWORD)(-1) ) &&   ( attr & FILE_ATTRIBUTE_DIRECTORY);    
}   



BOOL CSECommonUtil::PlayResource(LPTSTR lpName)
{
	BOOL bRtn; 
    LPTSTR lpRes; 
    HRSRC hResInfo;
	HGLOBAL hRes; 
 
    // Find the WAVE resource. 
	HINSTANCE hInst =CPaintManagerUI::GetInstance();
    hResInfo = FindResource(hInst, lpName, _T("WAVE")); 
    if (hResInfo == NULL) 
        return FALSE; 
 
    // Load the WAVE resource. 
 
    hRes = LoadResource(hInst, hResInfo); 
    if (hRes == NULL) 
        return FALSE; 
 
    // Lock the WAVE resource and play it. 
 
    lpRes = (LPTSTR)LockResource(hRes); 
    if (lpRes != NULL) 
	{ 
        bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_SYNC | SND_NODEFAULT); 
        UnlockResource(hRes); 
    } 
    else 
        bRtn = FALSE; 
 
    // Free the WAVE resource and return success or failure. 
    FreeResource(hRes); 
    return bRtn; 
}



BOOL CSECommonUtil::CreatDir(CDuiString sDir)
{
	   //设置属性
        SECURITY_ATTRIBUTES attribute;
        attribute.nLength = sizeof(attribute);
        attribute.lpSecurityDescriptor = NULL;
        attribute.bInheritHandle = FALSE;
        //创建
		return ::CreateDirectory(sDir,&attribute);
}
 
std::string CSECommonUtil::DuiStringToAnsi(CDuiString message)
{ 
	;
	#ifdef _UNICODE
		/*
		int nLen = ::WideCharToMultiByte(CP_ACP, 0 , message, -1,NULL, 0, NULL, NULL);
		if(nLen <=0) return std::string("");
		char *pszDst=new char[nLen];
		if(NULL == pszDst) return std::string("");
		::WideCharToMultiByte(CP_ACP, 0, message, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen-1] =0;
		std::string strTemp(pszDst);
		delete []pszDst;
		return strTemp;
		*/
		UToG  gbkstring(message.GetData());
		std::string strTemp((const char *)gbkstring); 
	#else  
		std::string strTemp((const char *)message.GetData());
	#endif

	return strTemp;
}


std::string CSECommonUtil::DuiStringToUtf8(CDuiString message)
{  
	#ifdef _UNICODE
		/*
		int nLen = ::WideCharToMultiByte(CP_ACP, 0 , message, -1,NULL, 0, NULL, NULL);
		if(nLen <=0) return std::string("");
		char *pszDst=new char[nLen];
		if(NULL == pszDst) return std::string("");
		::WideCharToMultiByte(CP_ACP, 0, message, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen-1] =0;
		std::string strTemp(pszDst);
		delete []pszDst;
		return strTemp;
		*/
		UToU8  utf8string(message.GetData());
		std::string strTemp((const char *)utf8string);
	#else 
		GToU8 utf8string(message.GetData());
		std::string strTemp((const char *)utf8string);
	#endif

	return strTemp;
}

CDuiString CSECommonUtil::Utf8ToDuiString(std::string message)
{
	#ifdef _UNICODE 
		U8ToU  gstring(message.c_str());
		return CDuiString((const TCHAR *)gstring);
	#else 
		U8ToG gstring(message.c_str());
		std::string strTemp((const char *)gstring);
		return strTemp;
	#endif
}

std::string CSECommonUtil::StringToUtf8(std::string message)
{  
	#ifdef _UNICODE  
		GToU8  utf8string(message.c_str());
		std::string strTemp((const char *)utf8string);
	#else 
		GToU8 utf8string(message.c_str());
		std::string strTemp((const char *)utf8string);
	#endif

	return strTemp;
}


CDuiString CSECommonUtil::StringToDuiString(std::string message)
{
	#ifdef _UNICODE 
		AToU  str(message.c_str());
		return CDuiString(str);
	#else  
		return CDuiString(message.c_str());
	#endif 
}

std::string CSECommonUtil::DuiStringToString(CDuiString message)
{
	#ifdef _UNICODE 
		UToA  str(message.GetData());
		std::string strTemp(str);
	#else  
		std::string strTemp(message.GetData());
	#endif

	return strTemp;
}
  
 
GToU8::GToU8(const char *strGBK)
{
   szUtf8 =GBKToUTF8(strGBK);
}

GToU8::~GToU8()
{
    free(szUtf8);
}

char *GToU8::GBKToUTF8(const char *strGBK)
{ 
     wchar_t *wszUtf8;
     int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK, -1, NULL,0);
     wszUtf8 = (wchar_t *)malloc(len * 2 + 2);
     memset(wszUtf8, 0, len * 2 + 2);
     MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK, -1, (LPWSTR)wszUtf8, len);

     len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
     szUtf8= (char *)malloc(len + 1);
     memset(szUtf8, 0, len + 1);
     WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);

     free(wszUtf8);
     return szUtf8;
} 

U8ToG::U8ToG(const char *strUtf8)
{
          szGBK =UTF8ToGBK(strUtf8);
}

U8ToG::~U8ToG()
{
          free(szGBK);
}

char *U8ToG::UTF8ToGBK(const char *strUtf8)
{ 
	   wchar_t *wszGBK;
	   //长度
	   int len = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8, -1, NULL, 0);
	   wszGBK = ( wchar_t *)malloc(len*2 +2);
	   memset(wszGBK, 0, len*2 +2);
	   MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUtf8, -1, (LPWSTR)wszGBK, len);

	   //长度
	   len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	   szGBK = (char *)malloc(len+1);
	   memset(szGBK, 0, len + 1);
	   WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);

	   free(wszGBK);

	   return szGBK; 
} 


//UTF-8到Unicode的转换
U8ToU::U8ToU(const char *strUtf8)
{
	szUnicode =UTF8ToUnicode(strUtf8);
}
U8ToU::~U8ToU()
{
	free(szUnicode);
}

wchar_t *U8ToU::UTF8ToUnicode(const char *strUtf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);

	wchar_t*unicode   =   (wchar_t*)malloc((len+1)*sizeof(wchar_t)); 
	memset(unicode, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, unicode, len); 
	return unicode;
}


//Unicode到UTF-8的转换
UToU8::UToU8(const wchar_t *strUnicode)
{
	szUtf8 = UnicodeToUtf8(strUnicode);
}

UToU8::~UToU8()
{
	free(szUtf8);
}

char *UToU8::UnicodeToUtf8(const wchar_t *strUnicode)
{  
	int len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	char *utf8 = (char *)malloc(len+1);
	memset(utf8, 0, len+1);
	WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, utf8, len, NULL, NULL);
	return utf8;
} 


//Unicode到GB2312的转换
UToG::UToG(const wchar_t *strUnicode)
{
	szGBK = UnicodeToGBK(strUnicode);
}

UToG::~UToG()
{
	free(szGBK);
}

char *UToG::UnicodeToGBK(const wchar_t *strUnicode)
{ 
	int len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	char *gb2312 = (char *)malloc(len+1);
	memset(gb2312, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, gb2312, len, NULL, NULL);
	return  gb2312;
}


//GB2312到Unicode的转换
GToU::GToU(const char *strGBK)
{
	szUnicode=GBKToUnicode(strGBK);
}

GToU::~GToU()
{
	free(szUnicode);
}

wchar_t *GToU::GBKToUnicode(const char *strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	wchar_t *unicode = (wchar_t *)malloc(len+1);
	memset(unicode, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, unicode, len);
	return unicode;
} 


//Ansi到Unicode的转换
AToU::AToU(const char *strAnsi)
{
	szUnicode= AnsiToUnicode(strAnsi);
}

AToU::~AToU()
{
	free(szUnicode);
}

wchar_t *AToU::AnsiToUnicode(const char *strAnsi)
{
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strAnsi, -1, NULL, 0 );
    if (nLen == 0)
    {
        return NULL;
    }
    wchar_t *pResult = new wchar_t[nLen];
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, strAnsi, -1, pResult, nLen );
    return pResult;
}

//Unicode到Ansi的转换
UToA::UToA(const wchar_t *strUnicode)
{
	szAnsi =UnicodeToAnsi(strUnicode);
}
 
UToA::~UToA()
{
	free(szAnsi);
}

char *UToA::UnicodeToAnsi(const wchar_t *strUnicode)
{
	int nLen = WideCharToMultiByte( CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL );
	if (nLen == 0)
	{
		 return NULL;
	}
	char *pResult = new char[nLen];
	WideCharToMultiByte( CP_ACP, 0, strUnicode, -1, pResult, nLen, NULL, NULL );
	return pResult;
}
 