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

#include "PlotDataBar.h"

using namespace NsCChart;

CPlotDataBar::CPlotDataBar()
{
	SetDefaults();
}

CPlotDataBar::~CPlotDataBar()
{

}

void	CPlotDataBar::SetDefaults()
{
	m_bDataBarShow = false;
	m_nDataBarBase = kDataBarBaseBottom;
	m_nDataBarLineSize = 1;
	m_nDataBarLineStyle = PS_SOLID;
	m_crDataBarLineColor = RGB(128, 128, 128);
	m_nDataBarAlpha = 255;
	m_bDataBarSameColor = false;
	m_nDataBarOffset = 0;
}