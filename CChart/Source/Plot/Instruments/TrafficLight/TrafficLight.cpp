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

#include "TrafficLight.h"

using namespace NsCChart;

CTrafficLight::CTrafficLight()
{
	SetDefaults();
}

CTrafficLight::~CTrafficLight()
{

}

void	CTrafficLight::SetDefaults()
{
	m_nRadius = 40;
	m_nStatus = kTrafficGreen;
	m_crOffColor = RGB(224, 224, 224);
}