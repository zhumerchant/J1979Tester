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

#include "BendingPlot.h"
#include "../../Basic/XYPlotImpl.h"

Declare_Namespace_CChart

class CBendingPlotImpl : public CBendingPlot, public CXYPlotImplI
{
public:
	CBendingPlotImpl();
	virtual	~CBendingPlotImpl();

	virtual	TCHAR* GetTypeName(){return _TEXT("BendingPlot");}

public:
	void	InitBendingAxis();
	
public:
	int		AddCurve( double *pX, double *pY, int nLen );
};

typedef CBendingPlotImpl CBendingPlotImplI;

Declare_Namespace_End