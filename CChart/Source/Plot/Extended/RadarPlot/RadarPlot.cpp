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

#include "RadarPlot.h"

using namespace NsCChart;

CRadarPlot::CRadarPlot()
{
	SetDefaults();
}

CRadarPlot::~CRadarPlot()
{

}

void	CRadarPlot::SetDefaults( )
{
	m_nRadarFrameWidth = 2;
	m_nRadarFrameStyle = PS_SOLID;
	m_crRadarFrameColor = RGB(0,0,0);

	m_nRadarGridWidth = 1;
	m_nRadarGridStyle = PS_DASH;
	m_crRadarGridColor = RGB(128,128,128);
}

