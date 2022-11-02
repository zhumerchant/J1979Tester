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

//#include "stdafx.h"

#include "ContourLinePlot.h"
#include "ListContour.h"
#include <math.h>

using namespace NsCChart;

CContourLinePlot::CContourLinePlot()
{
	m_bColorContour = true;
}

CContourLinePlot::~CContourLinePlot()
{
}

int CContourLinePlot::GetNContour()
{
	return m_pListContour->GetNContour();
}

void CContourLinePlot::SetNContour(int nContour)
{
	if(nContour<=0)return;
	
	m_pListContour->SetNContour(nContour);
}


void CContourLinePlot::SetContourPrecision(int n)
{
	if( n<=5 )
		return;
	m_pListContour->SetFirstGrid((int)pow(2.0, n), (int)pow(2.0,n));
	m_pListContour->SetSecondaryGrid((int)pow(2.0, n+3), (int)pow(2.0,n+3));
}