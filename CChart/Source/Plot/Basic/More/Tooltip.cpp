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

#include "Tooltip.h"
using namespace NsCChart;

CTooltip::CTooltip()
{
	SetDefaults();
}

CTooltip::~CTooltip()
{

}

void	CTooltip::SetDefaults()
{
	m_bShowTooltip = false;

	LOGFONT lf = { 0 };
	m_lfTooltipFont = lf;
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &m_lfTooltipFont);
	m_lfTooltipFont.lfHeight = 16;

	m_crTooltip = RGB(35, 35, 35);

	m_bTooltipRectShow = false;
	m_crTooltipRect = RGB(200, 50, 50);
	m_nTooltipRectLineSize = 2;
	m_nTooltipRectLineStyle = PS_SOLID;
}