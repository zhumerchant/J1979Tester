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

#include "PlotDataPointInfo.h"

using namespace NsCChart;

CPlotDataPointInfo::CPlotDataPointInfo()
{
	SetDefaults();
}

CPlotDataPointInfo::~CPlotDataPointInfo()
{

}

void	CPlotDataPointInfo::SetDefaults()
{
	m_bDataPointInfoShow = false;
	m_nDataPointInfoType = kShowDataPointInfoY;
	m_nDataPointInfoFrequency = 1;
	m_nDataPointInfoStartIndex = 0;
}