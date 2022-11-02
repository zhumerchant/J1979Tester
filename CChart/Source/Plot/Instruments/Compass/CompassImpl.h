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

#include "Compass.h"
#include "../InstruBase/InstruBaseImpl.h"

Declare_Namespace_CChart

class CCompassImpl : public CCompass, public CInstruBaseImpl
{
public:
	CCompassImpl();
	virtual	~CCompassImpl();

public:
	virtual	void	OnDraw(HWND hWnd, RECT destRect);
	virtual	void	OnDraw(HDC hDC, RECT destRect);

public:
	void		Set4Directions();
	void		Set8Directions();
};

Declare_Namespace_End
