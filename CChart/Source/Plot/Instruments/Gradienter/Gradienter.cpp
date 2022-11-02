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

#include "Gradienter.h"

using namespace NsCChart;

CGradienter::CGradienter()
{
	SetDefaults();
}

CGradienter::~CGradienter()
{

}

void	CGradienter::SetDefaults()
{
	m_crWaterColor = RGB(200, 200, 255);
	m_crAirColor = RGB(248, 248, 248);
	m_crSurfaceColor = RGB(128, 128, 128);
	m_nSurfaceThick = 2;
}