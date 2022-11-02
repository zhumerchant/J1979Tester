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

#include <windows.h>
#include "../../Global.h"

#include "MyMenuID.h"

Declare_Namespace_CChart

template<class PlotImplT>
void	AddOscilloPopup( PlotImplT *plot, HMENU addMenu )
{
	HMENU	subMenu;
	UINT	flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( plot->IsStartOscilloScope() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloStart, _TEXT("&Start oscillo mode") );
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		
		flag = GetPopupCheckedFlag( plot->IsShowHCursor() && plot->IsShowVCursor() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor, _TEXT("Show &Cursors") );
		flag = GetPopupCheckedFlag( plot->IsShowHCursor() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+1, _TEXT("---Show &HCursor") );
		flag = GetPopupCheckedFlag( plot->IsShowVCursor() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+2, _TEXT("---Show &VCursor") );
		flag = GetPopupCheckedFlag( plot->IsHoldCursorToCurve() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+3, _TEXT("&Bind to curve") );
		flag = GetPopupCheckedFlag( plot->IsCursorByMouse() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+4, _TEXT("Follow &mouse") );
		
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		flag = GetPopupCheckedFlag( plot->IsEnableXZone() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloSelZone, _TEXT("&X Zone") );
		flag = GetPopupCheckedFlag( plot->IsEnableYZone() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloSelZone+1, _TEXT("&Y Zone") );
		
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		AppendMenu(subMenu, MF_STRING, kMenuOscilloDataDrag, _TEXT("&Reset dragged data"));
		
		AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, _TEXT("&OscilloScope") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( plot->IsStartOscilloScope() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloStart, _TEXT("����ʾ����ģʽ(&E)") );
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		
		flag = GetPopupCheckedFlag( plot->IsShowHCursor() && plot->IsShowVCursor() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor, _TEXT("��ʾʮ����(&C)") );
		flag = GetPopupCheckedFlag( plot->IsShowHCursor() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+1, _TEXT("---��ʾˮƽָʾ��(&H)") );
		flag = GetPopupCheckedFlag( plot->IsShowVCursor() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+2, _TEXT("---��ʾ��ֱָʾ��(&V)") );
		flag = GetPopupCheckedFlag( plot->IsHoldCursorToCurve() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+3, _TEXT("�󶨵�����(&B)") );
		flag = GetPopupCheckedFlag( plot->IsCursorByMouse() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloCursor+4, _TEXT("�������(&M)") );
		
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		flag = GetPopupCheckedFlag( plot->IsEnableXZone() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloSelZone, _TEXT("&X����ѡ��") );
		flag = GetPopupCheckedFlag( plot->IsEnableYZone() );
		AppendMenu(subMenu, MF_STRING | flag, kMenuOscilloSelZone+1, _TEXT("&Y����ѡ��") );
		
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		AppendMenu(subMenu, MF_STRING, kMenuOscilloDataDrag, _TEXT("�����϶�����(&R)"));
		
		AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, _TEXT("ʾ����ģʽ(&O)") );
		break;
	}
	
	DestroyMenu(subMenu);
}

template<class PlotImplT>
void	CreateOscilloPopup(PlotImplT *plot, HMENU menu)
{
	if(plot->IsOscilloScopeMode())
	{
		AddOscilloPopup(plot, menu);
	}
}

template<class PlotImplT>
int		InterpretOscilloPopupItem( PlotImplT *plot, HWND hWnd, int result )
{
	if(result >= kMenuOscilloStart && result < kMenuOscilloStart + nMenuOscilloStart)
	{
		if(result == kMenuOscilloStart)
		{
			plot->SetStartOscilloScope(!plot->IsStartOscilloScope());
			return kPopupNoAction;
		}
	}
	else if(result >= kMenuOscilloCursor && result < kMenuOscilloCursor + nMenuOscilloCursors)
	{
		if(result == kMenuOscilloCursor)
		{
			if(!plot->IsShowHCursor() || !plot->IsShowVCursor())
			{
				plot->SetShowHCursor(true);
				plot->SetShowVCursor(true);
			}
			else
			{
				plot->SetShowHCursor(false);
				plot->SetShowVCursor(false);
			}
			return kPopupUpdate;
		}
		else if(result == kMenuOscilloCursor+1)
		{
			plot->SetShowHCursor(!plot->IsShowHCursor());
			return kPopupUpdate;
		}
		else if(result == kMenuOscilloCursor+2)
		{
			plot->SetShowVCursor(!plot->IsShowVCursor());
			return kPopupUpdate;
		}
		else if(result == kMenuOscilloCursor+3)
		{
			plot->SetHoldCursorToCurve(!plot->IsHoldCursorToCurve());
			return kPopupUpdate;
		}
		else if(result == kMenuOscilloCursor+4)
		{
			plot->SetCursorByMouse(!plot->IsCursorByMouse());
			return kPopupUpdate;
		}
	}
	else if(result >= kMenuOscilloSelZone && result < kMenuOscilloSelZone + nMenuOscilloSelZone)
	{
		if(result == kMenuOscilloSelZone)
		{
			plot->SetEnableXZone(!plot->IsEnableXZone());
			return kPopupUpdate;
		}
		else if(result == kMenuOscilloSelZone + 1)
		{
			plot->SetEnableYZone(!plot->IsEnableYZone());
			return kPopupUpdate;
		}
	}
	else if(result >= kMenuOscilloDataDrag && result < kMenuOscilloDataDrag + nMenuOscilloDataDrag)
	{
		if(result == kMenuOscilloDataDrag)
		{
			plot->RessetAllPlotDataOffset();
			return kPopupUpdate;
		}
	}
	return kPopupNoAction;
}

template<class PlotImplT>
bool	IsOscilloItem(PlotImplT *plot, int result)
{
	return (result >= kMenuOscillo && result < kMenuOscillo + nMenuOscillo);
}

Declare_Namespace_End
