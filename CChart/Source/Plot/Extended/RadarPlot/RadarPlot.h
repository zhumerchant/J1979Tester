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

#include "../../Basic/CChartNameSpace.h"
#include <windows.h>

Declare_Namespace_CChart

class	CRadarPlot
{
public:
	CRadarPlot();
	virtual ~CRadarPlot();

	void				SetDefaults( );

protected:
	int					m_nRadarFrameWidth;
	int					m_nRadarFrameStyle;
	COLORREF			m_crRadarFrameColor;

	int					m_nRadarGridWidth;
	int					m_nRadarGridStyle;
	COLORREF			m_crRadarGridColor;

};

Declare_Namespace_End
