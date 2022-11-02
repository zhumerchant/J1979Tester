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

//#include "StdAfx.h"
#include "HugePlotImpl.h"

using namespace NsCChart;

CHugePlotImpl::CHugePlotImpl()
{
	SetFloatXTicks(true);
}

CHugePlotImpl::~CHugePlotImpl()
{
	int i;
	for(i=0; i<GetPlotDataCount(); i++)
	{
		vector<DataPoint2D> *pData = GetAbsPlotData(i)->GetPDataSaved();
		delete pData;
		pData = 0;
	}
}

void	CHugePlotImpl::FinalDraw(HDC hDC, RECT destRect)
{
	//if there is no data, doing nothing


}

