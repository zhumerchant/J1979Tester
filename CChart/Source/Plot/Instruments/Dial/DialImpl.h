/*============================================================================*/
/*                                                                            */
/*                            C O P Y R I G H T                               */
/*                                                                            */
/*                          (C) Copyright 2019 by                             */
/*                              Yang Guojun                                   */
/*                           All Rights Reserved                              */
/*                                                                            */
/*      The author assumes no responsibility for the use or reliability of    */
/*      his software.                                                         */
/*                                                                            */
/*============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                  ��Ȩ����                                  //
//                         ��Ȩ����(C)2006-2019�������                       //
//                                ����ȫ��Ȩ��                                //
////////////////////////////////////////////////////////////////////////////////
/* ############################################################################################################################## */

#pragma once

#include "Dial.h"
#include "../InstruBase/InstruBaseImpl.h"

Declare_Namespace_CChart

class CDialImpl : public CDial, public CInstruBaseImpl
{
public:
	CDialImpl();
	virtual	~CDialImpl();

public:
	virtual	void	OnDraw(HWND hWnd, RECT destRect);
	virtual	void	OnDraw(HDC hDC, RECT destRect);

protected:
	virtual	RECT	GetSubTitleRect(HDC hDC, RECT barRect);
	void			DrawSubTitle(HDC hDC, RECT barRect);

protected:
	tstring			GetRealSubTitle();
};

Declare_Namespace_End

