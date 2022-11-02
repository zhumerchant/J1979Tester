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

#include "InterConnect.h"

using namespace NsCChart;

CInterConnect::CInterConnect()
{
	SetDefaults();
}

CInterConnect::~CInterConnect()
{

}

void	CInterConnect::SetDefaults()
{
	m_bInterConnect = false;
	m_nInterConnectLineSize = 1;
	m_nInterConnectLineStyle = PS_DASH;
	m_crInterConnectLineColor = RGB(128, 128, 128);
	m_nInterConnectAlpha = 255;
}