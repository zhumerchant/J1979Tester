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

#include "FixAxisPlot.h"

using namespace NsCChart;

CFixAxisPlot::CFixAxisPlot()
{
	SetDefaults();
}

CFixAxisPlot::~CFixAxisPlot()
{

}

void		CFixAxisPlot::SetDefaults( )
{
	m_bDualX	=	false;
	m_bDualY	=	false;

	m_nMajorDivsX	=	9;
	m_nMajorDivsY	=	9;
	m_nMinorDivsX	=	5;
	m_nMinorDivsY	=	5;

	m_bShowYScaleOnly	= false;
}

