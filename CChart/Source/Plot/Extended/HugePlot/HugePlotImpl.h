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

#include "HugePlot.h"
#include "../../Basic/XYPlotImpl.h"

Declare_Namespace_CChart

class CHugePlotImpl : public CHugePlot, public CXYPlotImpl
{
public:
	CHugePlotImpl();
	virtual ~CHugePlotImpl();

public:
	virtual	TCHAR*		GetTypeName(){return _TEXT("HugePlot");}
	virtual	void		FinalDraw( HDC hDC, RECT destRect );

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

Declare_Namespace_End