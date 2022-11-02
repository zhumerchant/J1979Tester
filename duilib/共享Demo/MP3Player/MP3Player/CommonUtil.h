#ifndef _COMMONUTIL_H
#define _COMMONUTIL_H

#pragma once
#include <windows.h>
#include <string> 
/*
2012-09-09
wuding guangzhou
��Ҫ�ṩһЩ�����࣬�����������ת��
*/

class CSECommonUtil
{
public:
	static std::string DuiStringToAnsi(CDuiString message);
	static std::string DuiStringToUtf8(CDuiString message);
	static CDuiString Utf8ToDuiString(std::string message);
	static std::string StringToUtf8(std::string message);
	static CDuiString StringToDuiString(std::string message);
	static std::string DuiStringToString(CDuiString message);

	static BOOL FolderExists(CDuiString s)  ;
	static BOOL PlayResource(LPTSTR lpName);
	static BOOL CreatDir(CDuiString sDir);
	static DWORD HexToRGB(CDuiString rgbHex);
	static CDuiString RGBToHex(unsigned long num);
	static CDuiString RGBToHex(int r,int g,int b);
 
};

//GBK��Utf8��ת��
class GToU8
{
private:
      char *szUtf8;
public:
      GToU8(const char *strGBK);
      ~GToU8();
      operator const char *(){ return szUtf8;};
private:
        //TODO:GBKת��UTF8��
        char *GBKToUTF8(const char *strGBK);
};

//Utf8��GBK��ת��
class U8ToG
{
private:
      char *szGBK;
public:
      U8ToG(const char *strUtf8);
      ~U8ToG();
      operator const char *(){ return szGBK;};
private:
      char *UTF8ToGBK(const char *strUtf8);
};


//Utf8��Unicode��ת��
class U8ToU
{
private:
      wchar_t *szUnicode; 
public:
      U8ToU(const char *strUtf8);
      ~U8ToU();
      operator const wchar_t *(){ return szUnicode;};
private:
      wchar_t *UTF8ToUnicode(const char *strUtf8);
};



//Unicode��Utf8��ת��
class UToU8
{
private:
      char *szUtf8;
public:
      UToU8(const wchar_t *strUnicode);
      ~UToU8();
      operator const char *(){ return szUtf8;};
private:
      char *UnicodeToUtf8(const wchar_t *strUnicode);
};


//Unicode��GBK��ת��
class UToG
{
private:
      char *szGBK;
public:
      UToG(const wchar_t *strUnicode);
      ~UToG();
      operator const char *(){ return szGBK;};
private:
      char *UnicodeToGBK(const wchar_t *strUnicode);
};


//GBK��Unicode��ת��
class GToU
{
private:
      wchar_t *szUnicode;
public:
      GToU(const char *strGBK);
      ~GToU();
      operator const wchar_t *(){ return szUnicode;};
private:
      wchar_t *GBKToUnicode(const char *strGBK);
};


//Ansi��Unicode��ת��
class AToU
{
private:
      wchar_t *szUnicode;
public:
      AToU(const char *strAnsi);
      ~AToU();
      operator const wchar_t *(){ return szUnicode;};
private:
      wchar_t *AnsiToUnicode(const char *strAnsi);
};

//Unicode��Ansi��ת��
class UToA
{
private:
      char *szAnsi;
public:
      UToA(const wchar_t *strUnicode);
      ~UToA();
      operator const char *(){ return szAnsi;};
private:
      char *UnicodeToAnsi(const wchar_t *strUnicode);
};


#endif

