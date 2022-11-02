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

#include "Slider.h"

using namespace NsCChart;

CSlider::CSlider()
{
	SetDefaults();
}

CSlider::~CSlider()
{

}

void	CSlider::SetDefaults()
{
	m_bShowBarLine = false;
	m_nSliderHalfLen = 15;
	m_crSliderColor = RGB(96, 96, 96);
	m_bShowBarCenter = true;
	m_nBarCenterWidth = 4;
}