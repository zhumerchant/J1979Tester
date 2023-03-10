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
//                                  版权申明                                  //
//                         版权所有(C)2006-2019，杨国君                       //
//                                保留全部权利                                //
////////////////////////////////////////////////////////////////////////////////
/* ############################################################################################################################## */

#pragma once

#include <windows.h>
#include "../../Global.h"
#include "MyMenuID.h"

Declare_Namespace_CChart

extern void	AddLineSizePopup(HMENU addMenu, tstring titleString, long compare, int startVal);
extern void	AddBrushTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal );
extern void	AddHatchTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal );


template<class PlotImplT>
void	AddLegendPopup(PlotImplT *plot, HMENU addMenu, tstring titleString, int startVal)
{
	HMENU	subMenu, tMenu;
	UINT	flag;
	
	subMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		if( plot->IsLegendShow() )
			AppendMenu(subMenu, MF_STRING, kMenuLegend, _TEXT("&Hide Legend") );
		else
			AppendMenu(subMenu, MF_STRING, kMenuLegend, _TEXT("&Show Legend") );
		break;
	case kLangChinese:
		if( plot->IsLegendShow() )
			AppendMenu(subMenu, MF_STRING, kMenuLegend, _TEXT("隐藏图例(&H)") );
		else
			AppendMenu(subMenu, MF_STRING, kMenuLegend, _TEXT("显示图例(&S)") );
		break;
	}
	
	tMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( plot->IsLegendTransparent() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature, _TEXT("&Transparent") );
		flag = GetPopupCheckedFlag( plot->IsLegendConfined() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature+1, _TEXT("&Confined") );
		AppendMenu(tMenu, MF_STRING, kMenuLegendFeature+2, _TEXT("Legend &Font...") );
		flag = GetPopupCheckedFlag( plot->IsLegendRound() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature+3, _TEXT("&Round Corner"));
		flag = GetPopupCheckedFlag( plot->IsLegendShadow() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature+4, _TEXT("&Shadow"));
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("&Features") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( plot->IsLegendTransparent() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature, _TEXT("透明(&T)") );
		flag = GetPopupCheckedFlag( plot->IsLegendConfined() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature+1, _TEXT("位置限制(&C)") );
		AppendMenu(tMenu, MF_STRING, kMenuLegendFeature+2, _TEXT("字体(&F)...") );
		flag = GetPopupCheckedFlag( plot->IsLegendRound() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature+3, _TEXT("圆角(&R)"));
		flag = GetPopupCheckedFlag( plot->IsLegendShadow() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendFeature+4, _TEXT("阴影(&S)"));
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("图例特征(&F)") );
		break;
	}
	DestroyMenu(tMenu);
	
	//AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
	tMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( plot->IsLegendBorderShow() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendBorder, _TEXT("&Show Border"));
		AppendMenu(tMenu, MF_STRING, kMenuLegendBorder+1, _TEXT("Border &Color...") );
		AddLineSizePopup(tMenu, _TEXT("Border &Size"), plot->GetLegendBorderLineSize(),kMenuLegendBorder+2);
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("&Border") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( plot->IsLegendBorderShow() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuLegendBorder, _TEXT("显示边框(&S)"));
		AppendMenu(tMenu, MF_STRING, kMenuLegendBorder+1, _TEXT("边框颜色(&C)...") );
		AddLineSizePopup(tMenu, _TEXT("边框粗细(&S)"), plot->GetLegendBorderLineSize(),kMenuLegendBorder+2);
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("图例边框(&B)") );
		break;
	}
	DestroyMenu(tMenu);
	
	//AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
	tMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AppendMenu(tMenu, MF_STRING, kMenuLegendBkgnd, _TEXT("&Background Color...") );
		AddBrushTypePopup(tMenu, _TEXT("Legend B&rush"), plot->GetLegendBkgndBrushType(), kMenuLegendBrushType);
		AddHatchTypePopup(tMenu, _TEXT("Legend &Hatch"), plot->GetLegendBkgndHatchStyle(), kMenuLegendHatchType);
		AppendMenu(tMenu, MF_STRING, kMenuLegendHatchType + kHatchTypeCount, _TEXT("Legend H&atch Color...") );
		AppendMenu(tMenu, MF_STRING, kMenuLegendHatchType + kHatchTypeCount +1, _TEXT("&Load Legend Pattern...") );
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("B&ackground") );
		break;
	case kLangChinese:
		AppendMenu(tMenu, MF_STRING, kMenuLegendBkgnd, _TEXT("背景颜色(&B)...") );
		AddBrushTypePopup(tMenu, _TEXT("图例画刷(&R)"), plot->GetLegendBkgndBrushType(), kMenuLegendBrushType);
		AddHatchTypePopup(tMenu, _TEXT("图例阴影(&H)"), plot->GetLegendBkgndHatchStyle(), kMenuLegendHatchType);
		AppendMenu(tMenu, MF_STRING, kMenuLegendHatchType + kHatchTypeCount, _TEXT("阴影颜色(&A)...") );
		AppendMenu(tMenu, MF_STRING, kMenuLegendHatchType + kHatchTypeCount +1, _TEXT("载入背景图案(&L)...") );
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("图例背景(&A)") );
		break;
	}
	DestroyMenu(tMenu);

	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AddLegendPosPopup(subMenu, _TEXT("&Position"), plot->GetLegendPosition(), kMenuLegendPosition);
		AddLegendColumnPopup(subMenu, _TEXT("&Columns"), plot->GetHorizontalNumOfLegend(), kMenuLegendColumns);
		break;
	case kLangChinese:
		AddLegendPosPopup(subMenu, _TEXT("图例位置(&P)"), plot->GetLegendPosition(), kMenuLegendPosition);
		AddLegendColumnPopup(subMenu, _TEXT("图例分栏(&C)"), plot->GetHorizontalNumOfLegend(), kMenuLegendColumns);
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str() );
	DestroyMenu(subMenu);
}

template<class PlotImplT>
void	CreateLegendPopup(PlotImplT *plot, HMENU menu)
{
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AddLegendPopup(plot, menu,  _TEXT("L&egend"), kMenuLegend);
		break;
	case kLangChinese:
		AddLegendPopup(plot, menu, _TEXT("图例选项(&E)"), kMenuLegend );
		break;
	}	
}

template<class PlotImplT>
int		InterpretLegendPopupItem( PlotImplT *plot, HWND hWnd, int result )
{
	COLORREF color;
	tstring filename, fileext, filepath;
	LOGFONT	logfont;

	if( result == kMenuLegend )
	{
		plot->SetLegendShow(!plot->IsLegendShow());
		return kPopupUpdate;
	}
	else if( result >= kMenuLegendFeature && result < kMenuLegendFeature + nMenuLegendFeature)
	{
		if( result == kMenuLegendFeature )
		{
			plot->SetLegendTransparent( !plot->IsLegendTransparent() );
			return kPopupUpdate;
		}
		else if( result == kMenuLegendFeature + 1 )
		{
			plot->SetLegendConfined( !plot->IsLegendConfined() );
			return kPopupUpdate;
		}
		else if( result == kMenuLegendFeature + 2 )
		{
			logfont = plot->GetLegendFont();
			if(ShowFontDialog(hWnd, logfont, color))
			{
				plot->SetLegendFont(logfont);
				return kPopupUpdate;
			}
			return kPopupNoAction;
		}
		else if( result == kMenuLegendFeature + 3)
		{
			plot->SetLegendRound(!plot->IsLegendRound());
			return kPopupUpdate;
		}
		else if( result == kMenuLegendFeature + 4)
		{
			plot->SetLegendShadow(!plot->IsLegendShadow());
			return kPopupUpdate;
		}
	}
	
	else if( result == kMenuLegendBorder)
	{
		plot->SetLegendBorderShow(!plot->IsLegendBorderShow());
		return kPopupUpdate;
	}
	else if( result == kMenuLegendBorder +1 )
	{
		color = plot->GetLegendBorderColor();
		if(ShowColorDialog(hWnd, color))
		{
			plot->SetLegendBorderColor(color);
			return kPopupUpdate;
		}
		return kPopupNoAction;
	}
	else if( result >= kMenuLegendBorder + 2 && result < kMenuLegendBorder + 2 + nLineSizes )
	{
		plot->SetLegendBorderLizeSize( result - kMenuLegendBorder - 1 );
		return kPopupUpdate;
	}
	else if( result == kMenuLegendBkgnd )
	{
		color = plot->GetLegendBkgndColor();
		if(ShowColorDialog(hWnd, color))
		{
			plot->SetLegendBkgndColor(color);
			return kPopupUpdate;
		}
		return kPopupNoAction;
	}
	else if( result >= kMenuLegendBrushType && result < kMenuLegendBrushType + kBrushTypeCount)
	{
		plot->SetLegendBkgndBrushType(result - kMenuLegendBrushType);
		if(result - kMenuLegendBrushType == kBrushPattern && !plot->GetLegendBkgndPattern())
		{
			MessageBox(hWnd, _TEXT("No pattern setted, please load file!"), _TEXT("Warning"), MB_OK);
		}
		return kPopupUpdate;
	}
	else if( result >= kMenuLegendHatchType && result < kMenuLegendHatchType + kHatchTypeCount)
	{
		plot->SetLegendBkgndHatchStyle(result - kMenuLegendHatchType);
		return kPopupUpdate;
	}
	else if( result - kMenuLegendHatchType == kHatchTypeCount )
	{
		color = plot->GetLegendBkgndHatchColor();
		if(ShowColorDialog(hWnd, color))
		{
			plot->SetLegendBkgndHatchColor(color);
			return kPopupUpdate;
		}
		return kPopupNoAction;
	}
	else if( result - kMenuLegendHatchType == kHatchTypeCount + 1)
	{
		if(ShowFileDialog( hWnd, true, filename, fileext, filepath, 
			_TEXT("Bitmap Files(*.bmp)\0*.BMP\0Png Files(*.png)\0*.PNG\0Jpeg Files(*.jpg)\0*.JPG\0Tiff Files(*.tif)\0*.TIF\0Gif Files(*.gif)\0*.GIF\0All Files(*.*)\0*.*\0\0"),
			_TEXT("bmp") ) )
		{
			HBITMAP hBitmap = HBitmapFromFile(filename);
			if(!hBitmap)
				return kPopupError;
			
			plot->SetLegendBkgndPattern(hBitmap);
			return kPopupUpdate;
		}
		return kPopupNoAction;
	}
	else if( result >= kMenuLegendPosition && result < kMenuLegendPosition + kLegendPositionCount)
	{
		plot->SetLegendPosition( result - kMenuLegendPosition );
		return kPopupUpdate;
	}
	else if( result >= kMenuLegendColumns && result < kMenuLegendColumns + nMenuLegendMaxColumn)
	{
		plot->SetHorizontalNumOfLegend( result - kMenuLegendColumns + 1 );
		return kPopupUpdate;
	}
	return kPopupNoAction;
}

template<class PlotImplT>
bool	IsLegendItem(PlotImplT plot, int result)
{
	return ( result >= kMenuLegend && result < kMenuLegend + kMenuLegendRange );
}

Declare_Namespace_End
