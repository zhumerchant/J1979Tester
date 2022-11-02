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

#pragma once

#include "../../Basic/XYPlotImpl.h"
#include "HistoPlot.h"

Declare_Namespace_CChart

class CHistoPlotImpl : public CHistoPlot, public CXYPlotImplI
{
public:
	CHistoPlotImpl();
	virtual	~CHistoPlotImpl();

public:
	bool		Statistic();
};

typedef CHistoPlotImpl CHistoPlotImplI;

Declare_Namespace_End