//
// win_impl_base.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com), wangchyz (wangchyz at gmail dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#include "stdafx.h"
#include "WinImplBase.hpp"
#include "MainFrame.h"

#include <atlbase.h>
CComModule _Module;
#include <atlwin.h>
 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
   
	HINSTANCE hInstRich = ::LoadLibrary(_T("Riched20.dll"));

	::CoInitialize(NULL);
	::OleInitialize(NULL);

	_Module.Init( 0, hInstance );
 
	HRESULT Hr = ::CoInitialize(NULL); 

	if( FAILED(Hr) ) return 0;

	MainFrame* pFrame = new MainFrame();
	if( pFrame == NULL ) return 0; 
	pFrame->Create(NULL, _T("MP3Player"), UI_WNDSTYLE_FRAME, WS_EX_TOOLWINDOW, 0, 0, 600, 800);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();

	_Module.Term();

	WindowImplBase::Cleanup();

	::OleUninitialize();
	::CoUninitialize();

	::FreeLibrary(hInstRich);
 
	return 0;
}