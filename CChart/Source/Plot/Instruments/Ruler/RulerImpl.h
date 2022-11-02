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

#include "Ruler.h"
#include "../InstruBase/InstruBaseImpl.h"

Declare_Namespace_CChart

class CRulerImpl : public CRuler, public CInstruBaseImpl
{
public:
	CRulerImpl();
	virtual	~CRulerImpl();

protected:
	void	DrawDodgedLabels(HDC hDC, RECT axisRect, RECT barRect);

public:
	virtual	void	OnDraw(HWND hWnd, RECT destRect);
	virtual	void	OnDraw(HDC hDC, RECT destRect);
};

Declare_Namespace_End