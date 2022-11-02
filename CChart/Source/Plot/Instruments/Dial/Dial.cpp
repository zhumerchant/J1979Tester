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

#include "Dial.h"
using namespace NsCChart;

CDial::CDial()
{
	SetDefaults();
}

CDial::~CDial()
{

}

void	CDial::SetDefaults()
{
	m_strSubTitle = _TEXT("Sub title");
	m_bShowSubTitle = false;
	m_sizeSubTitleOffset.cx = m_sizeSubTitleOffset.cy = 0;
	m_crSubTitleColor = RGB(128, 96, 96);
	m_bSubTitleAsValue = false;
	m_bFillSubTitleRect = false;
	m_crSubTitleRectColor = RGB(128, 128, 128);

	HFONT hFont;
	hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_ROMAN, _TEXT("Arial"));
	GetObject(hFont, sizeof(LOGFONT), &m_lfSubTitleFont);
	DeleteObject(hFont);
}