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

#include "StemHandler.h"

Declare_Namespace_CChart

class CStemPlotImplI;

void	InitPageParaSpecial(CStemPlotImplI *plot, vPagePara &vPP)
{

}
void	CreateOscilloPopup(CStemPlotImplI *plot, HMENU menu)
{

}
int		InterpretOscilloPopupItem( CStemPlotImplI *plot, HWND hWnd, int result )
{
	return kPopupNoAction;
}
void	CreateExtraFunctionPopup(CStemPlotImplI *plot, HMENU subMenu)
{

}
void	CreateTooltipPopup(CStemPlotImplI *plot, HMENU subMenu)
{

}
int		InterpretExtraFunctionPopupItem( CStemPlotImplI *plot, HWND hWnd, int result )
{
	return kPopupNoAction;
}
int		InterpretTooltipPopupItem( CStemPlotImplI *plot, HWND hWnd, int result )
{
	return kPopupNoAction;
}


Declare_Namespace_End