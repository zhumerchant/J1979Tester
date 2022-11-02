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

#include "PartialDial.h"

using namespace NsCChart;

CPartialDial::CPartialDial()
{
	SetDefaults();
}

CPartialDial::~CPartialDial()
{

}

void	CPartialDial::SetDefaults()
{
	m_nPartialDialType = kPartialDialTop;
	m_nPointerGap = 30;
	m_bSubtitleOuter = true;
	m_bShowGlass = true;
	m_crGlassColor = RGB(192, 192, 192);
	m_nGlassAlpha = 96;
}