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

#include "ThermometerImpl.h"

using namespace NsCChart;

CThermometerImpl::CThermometerImpl()
{

}

CThermometerImpl::~CThermometerImpl()
{

}

void	CThermometerImpl::OnDraw(HWND hWnd, RECT destRect)
{
	HDC hDC = GetDC(hWnd);
	OnDraw(hDC, destRect);
	ReleaseDC(hWnd, hDC);
}

void	CThermometerImpl::OnDraw(HDC hDC, RECT destRect)
{
	RECT plotRect = GetPlotRect(hDC, destRect);
	RECT barRect = GetBarRect(hDC, plotRect);

	DrawBorder(hDC, plotRect);
	DrawBar(hDC, barRect);
	//DrawCurrentFilling(hDC, barRect);
	DrawTitle(hDC, barRect, plotRect);
	DrawAxis(hDC, barRect);
}