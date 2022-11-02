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
void	AddGridPopup(PlotImplT *plot, HMENU addMenu, tstring titleString, int startVal)
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( plot->IsMajorHGrids() && plot->IsMajorVGrids());
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("M&ajor Grids") );
		flag = GetPopupCheckedFlag( plot->IsMajorHGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("---&Horizontal") );
		flag = GetPopupCheckedFlag(plot->IsMajorVGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("---&Vertical") );
		flag = GetPopupCheckedFlag( plot->IsMinorHGrids() && plot->IsMinorVGrids());
		
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("M&inor Grids") );
		flag = GetPopupCheckedFlag(plot->IsMinorHGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("---H&orizontal") );
		flag = GetPopupCheckedFlag(plot->IsMinorVGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+5, _TEXT("---V&ertical") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( plot->IsMajorHGrids() && plot->IsMajorVGrids());
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("������(&A)") );
		flag = GetPopupCheckedFlag( plot->IsMajorHGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("---ˮƽ(&H)") );
		flag = GetPopupCheckedFlag(plot->IsMajorVGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("---��ֱ(&V)") );
		flag = GetPopupCheckedFlag( plot->IsMinorHGrids() && plot->IsMinorVGrids());
		
		AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
		
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("������(&I)") );
		flag = GetPopupCheckedFlag(plot->IsMinorHGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("---ˮƽ(&O)") );
		flag = GetPopupCheckedFlag(plot->IsMinorVGrids() );
		AppendMenu(subMenu, MF_STRING | flag, startVal+5, _TEXT("---��ֱ(&E)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str() );
	
	DestroyMenu(subMenu);
}

template<class PlotImplT>
void	AddGradientPopup( PlotImplT *plot, bool bWholeRegion, HMENU addMenu, tstring titleString, int startVal)
{
	HMENU		subMenu;
	HMENU		gradMenu;
	int			flag;
	int			gradType;

	subMenu = CreatePopupMenu();

	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( bWholeRegion?plot->IsGradientBkgnd():plot->IsGradientDataRegion() );
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("&Enabled") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( bWholeRegion?plot->IsGradientBkgnd():plot->IsGradientDataRegion() );
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("Ӧ�ý���(&E)") );
		break;
	}
	
	gradMenu = CreatePopupMenu();
	gradType = (bWholeRegion?plot->GetGradientType():plot->GetDataRegionGradientType());
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( gradType == kGradientL2R );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientL2R, _TEXT("&Left to Right") );
		flag = GetPopupCheckedFlag( gradType == kGradientT2B );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientT2B, _TEXT("&Top to Bottom") );
		flag = GetPopupCheckedFlag( gradType == kGradientLT2RB );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientLT2RB, _TEXT("LT to &RB") );
		flag = GetPopupCheckedFlag( gradType == kGradientLB2RT );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientLB2RT, _TEXT("L&B to RT") );
		flag = GetPopupCheckedFlag( gradType == kGradientCt2Bd );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientCt2Bd, _TEXT("&Center to Bound") );
		flag = GetPopupCheckedFlag( gradType == kGradientHCt2Bd );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientCt2Bd, _TEXT("&H Center to Bound") );
		flag = GetPopupCheckedFlag( gradType == kGradientVCt2Bd );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientCt2Bd, _TEXT("&V Center to Bound") );
		flag = GetPopupCheckedFlag( gradType == kGradientRadial );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientRadial, _TEXT("&Radial") );
		AppendMenu( subMenu, MF_POPUP | MF_STRING, (UINT)gradMenu, _TEXT("&Gradient Type"));
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( gradType == kGradientL2R );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientL2R, _TEXT("������(&L)") );
		flag = GetPopupCheckedFlag( gradType == kGradientT2B );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientT2B, _TEXT("���ϵ���(&T)") );
		flag = GetPopupCheckedFlag( gradType == kGradientLT2RB );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientLT2RB, _TEXT("���ϵ�����(&R)") );
		flag = GetPopupCheckedFlag( gradType == kGradientLB2RT );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientLB2RT, _TEXT("���µ�����(&B)") );
		flag = GetPopupCheckedFlag( gradType == kGradientCt2Bd );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientCt2Bd, _TEXT("���ĵ��߽�(&C)") );
		flag = GetPopupCheckedFlag( gradType == kGradientCt2Bd );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientHCt2Bd, _TEXT("ˮƽ���ĵ��߽�(&C)") );
		flag = GetPopupCheckedFlag( gradType == kGradientCt2Bd );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientVCt2Bd, _TEXT("��ֱ���ĵ��߽�(&C)") );
		flag = GetPopupCheckedFlag( gradType == kGradientRadial );
		AppendMenu(gradMenu, MF_STRING | flag, startVal+1+kGradientRadial, _TEXT("����(&R)") );
		AppendMenu( subMenu, MF_POPUP | MF_STRING, (UINT)gradMenu, _TEXT("���䷽ʽ(&G)"));
		break;
	}
	DestroyMenu(gradMenu);
	
	gradMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AppendMenu( gradMenu, MF_STRING, startVal+1+kGradientTypeCount, _TEXT("&Start Color...") );
		AppendMenu( gradMenu, MF_STRING, startVal+2+kGradientTypeCount, _TEXT("Stop &Color...") );
		AppendMenu( subMenu, MF_POPUP | MF_STRING, (UINT)gradMenu, _TEXT("&Gradient Colors"));
		break;
	case kLangChinese:
		AppendMenu( gradMenu, MF_STRING, startVal+1+kGradientTypeCount, _TEXT("��ɫ1(&S)...") );
		AppendMenu( gradMenu, MF_STRING, startVal+2+kGradientTypeCount, _TEXT("��ɫ2(&C)...") );
		AppendMenu( subMenu, MF_POPUP | MF_STRING, (UINT)gradMenu, _TEXT("����ɫ(&G)"));
		break;
	}
	DestroyMenu(gradMenu);
	
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str() );
	DestroyMenu(subMenu);
}

template<class PlotImplT>
void	AddBkgndPopup(PlotImplT *plot, HMENU addMenu, tstring titleString, int startVal)
{
	HMENU subMenu, tMenu, subM;
	UINT	flag;

	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AddGridPopup(plot, subMenu, _TEXT("&Grids"), kMenuGrids);
		AddGradientPopup(plot, true, subMenu, _TEXT("Gra&dient"), kMenuGradient);
		break;
	case kLangChinese:
		AddGridPopup(plot, subMenu, _TEXT("������ʾ(&G)"), kMenuGrids);
		AddGradientPopup(plot, true, subMenu, _TEXT("������ɫ(&D)"), kMenuGradient);
		break;
	}

	//	AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );

	subM = CreatePopupMenu();
	tMenu = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( plot->IsBkgndImage() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuBkgndImage, _TEXT("&Use Image") );
		AppendMenu(tMenu, MF_STRING, kMenuBkgndImage+1, _TEXT("&Load Image...") );
		AppendMenu(subM, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("Background Image") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( plot->IsBkgndImage() );
		AppendMenu(tMenu, MF_STRING | flag, kMenuBkgndImage, _TEXT("ʹ��ͼ����Ϊ����(&U)") );
		AppendMenu(tMenu, MF_STRING, kMenuBkgndImage+1, _TEXT("����ͼ��(&L)...") );
		AppendMenu(subM, MF_POPUP | MF_STRING, (UINT) tMenu, _TEXT("ͼ�񱳾�(&G)") );
		break;
	}
	DestroyMenu(tMenu);

	AppendMenu(subM, MF_SEPARATOR, 0, (LPCTSTR)0 );

	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AppendMenu(subM, MF_STRING, kMenuBkgnd, _TEXT("&Background Color...") );
		AddBrushTypePopup(subM, _TEXT("Background B&rush"), plot->GetBkgndBrushType(), kMenuBkgndBrushType);
		AddHatchTypePopup(subM, _TEXT("Background &Hatch"), plot->GetBkgndHatchStyle(), kMenuBkgndHatchType);
		AppendMenu(subM, MF_STRING, kMenuBkgndHatchType + kHatchTypeCount, _TEXT("Background Hatch &Color...") );
		AppendMenu(subM, MF_STRING, kMenuBkgndHatchType + kHatchTypeCount +1, _TEXT("&Load Background Pattern..."));
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) subM, _TEXT("&Whole region") );
		break;
	case kLangChinese:
		AppendMenu(subM, MF_STRING, kMenuBkgnd, _TEXT("������ɫ(&B)...") );
		AddBrushTypePopup(subM, _TEXT("������ˢ(&R)"), plot->GetBkgndBrushType(), kMenuBkgndBrushType);
		AddHatchTypePopup(subM, _TEXT("������Ӱ(&H)"), plot->GetBkgndHatchStyle(), kMenuBkgndHatchType);
		AppendMenu(subM, MF_STRING, kMenuBkgndHatchType + kHatchTypeCount, _TEXT("��Ӱ��ɫ(&C)...") );
		AppendMenu(subM, MF_STRING, kMenuBkgndHatchType + kHatchTypeCount +1, _TEXT("���뱳��ͼ��(&L)..."));
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) subM, _TEXT("���屳��(&W)") );
		break;
	}
	DestroyMenu(subM);
	
//	AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
	subM = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		if( plot->IsUseDataRegionBrush() )
			AppendMenu(subM, MF_STRING, kMenuDataRegion, _TEXT("D&isable Data Area Brush") );
		else
			AppendMenu(subM, MF_STRING, kMenuDataRegion, _TEXT("&Enable Data Area Brush") );

		AppendMenu(subM, MF_SEPARATOR, 0, (LPCTSTR)0 );

		AppendMenu(subM, MF_STRING, kMenuDataRegion+1, _TEXT("&Data Area Color...") );
		AddBrushTypePopup(subM, _TEXT("Data Area B&rush"), plot->GetDataRegionBrushType(), kMenuDataRegionBrushType);
		AddHatchTypePopup(subM, _TEXT("Data Area &Hatch"), plot->GetDataRegionHatchStyle(), kMenuDataRegionHatchType);
		AppendMenu(subM, MF_STRING, kMenuDataRegionHatchType + kHatchTypeCount, _TEXT("Data Area H&atch Color...") );
		AppendMenu(subM, MF_STRING, kMenuDataRegionHatchType + kHatchTypeCount +1, _TEXT("&Load Data Area Pattern..."));
		
		AppendMenu(subM, MF_SEPARATOR, 0, (LPCTSTR)0 );

		AddGradientPopup(plot, false, subM, _TEXT("Gra&dient"), kMenuDataRegionGradient);

		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) subM, _TEXT("D&ata region") );
		break;
	case kLangChinese:
		if( plot->IsUseDataRegionBrush() )
			AppendMenu(subM, MF_STRING, kMenuDataRegion, _TEXT("ȡ������������ѡ��(&I)") );
		else
			AppendMenu(subM, MF_STRING, kMenuDataRegion, _TEXT("ʹ������������ѡ��(&E)") );
		
		AppendMenu(subM, MF_SEPARATOR, 0, (LPCTSTR)0 );

		AppendMenu(subM, MF_STRING, kMenuDataRegion+1, _TEXT("��������ɫ(&D)...") );
		AddBrushTypePopup(subM, _TEXT("��������ˢ(&R)"), plot->GetDataRegionBrushType(), kMenuDataRegionBrushType);
		AddHatchTypePopup(subM, _TEXT("��������Ӱ(&H)"), plot->GetDataRegionHatchStyle(), kMenuDataRegionHatchType);
		AppendMenu(subM, MF_STRING, kMenuDataRegionHatchType + kHatchTypeCount, _TEXT("��������ɫ(&A)...") );
		AppendMenu(subM, MF_STRING, kMenuDataRegionHatchType + kHatchTypeCount +1, _TEXT("����������ͼ��(&L)..."));
		
		AppendMenu(subM, MF_SEPARATOR, 0, (LPCTSTR)0 );
		AddGradientPopup(plot, false, subM, _TEXT("������ɫ(&D)"), kMenuDataRegionGradient);
		
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) subM, _TEXT("����������(&A)") );
		break;
	}
	DestroyMenu(subM);

//	AppendMenu(subMenu, MF_SEPARATOR, 0, (LPCTSTR)0 );
	subM = CreatePopupMenu();
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AppendMenu(subM, MF_STRING, kMenuBorder, _TEXT("Border &Color...") );
		AddLineSizePopup(subM, _TEXT("Border &Sizes"), plot->GetBorderSize(), kMenuBorder+1);
		if( plot->IsBorderShow())
			AppendMenu(subM, MF_STRING, kMenuBorder+nBorderSizes+1, _TEXT("&Hide Border") );
		else
			AppendMenu(subM, MF_STRING, kMenuBorder+nBorderSizes+1, _TEXT("&Show Border") );
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) subM, _TEXT("&Data Border") );
		break;
	case kLangChinese:
		AppendMenu(subM, MF_STRING, kMenuBorder, _TEXT("�߿���ɫ(&C)...") );
		AddLineSizePopup(subM, _TEXT("�߿��ϸ(&S)"), plot->GetBorderSize(), kMenuBorder+1);
		if( plot->IsBorderShow())
			AppendMenu(subM, MF_STRING, kMenuBorder+nBorderSizes+1, _TEXT("���ر߿�(&H)") );
		else
			AppendMenu(subM, MF_STRING, kMenuBorder+nBorderSizes+1, _TEXT("��ʾ�߿�(&S)") );
		AppendMenu(subMenu, MF_POPUP | MF_STRING, (UINT) subM, _TEXT("�������߿�(&B)") );
		break;
	}
	DestroyMenu(subM);
	
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str() );

	DestroyMenu(subMenu);
}

template<class PlotImplT>
void	CreateBkgndPopup(PlotImplT *plot, HMENU menu)
{
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AddBkgndPopup(plot, menu, _TEXT("&Background"), kMenuGrids );
		break;
	case kLangChinese:
		AddBkgndPopup(plot, menu, _TEXT("����ѡ��(&B)"), kMenuGrids );
		break;
	}		
}

template<class PlotImplT>
int		InterpretBkgndPopupItem( PlotImplT *plot, HWND hWnd, int result )
{
	tstring		filename, fileext, filepath;
	
	COLORREF	color=0;

	if( result >= kMenuBackground && result < kMenuBackground + kMenuBackgroundRange )//gradient back ground
	{
		if( result < kMenuGrids + nGridsSizes )
		{
			bool bSet;
			switch( result - kMenuGrids )
			{
			case 0: 
				bSet=plot->IsMajorHGrids()&&plot->IsMajorVGrids();
				plot->SetMajorHGrids(!bSet);plot->SetMajorVGrids(!bSet);break;
			case 1: plot->SetMajorHGrids( !plot->IsMajorHGrids() ); break;
			case 2: plot->SetMajorVGrids( !plot->IsMajorVGrids() ); break;
			case 3:
				bSet=plot->IsMinorHGrids()&&plot->IsMinorVGrids();
				plot->SetMinorHGrids(!bSet);plot->SetMinorVGrids(!bSet);break;;
			case 4: plot->SetMinorHGrids( !plot->IsMinorHGrids() ); break;
			case 5: plot->SetMinorVGrids( !plot->IsMinorVGrids() ); break;
			default: break;
			}
			return kPopupUpdate;
		}
		if( result >= kMenuGradient && result < kMenuGradient + nGradientSizes)
		{
			if(result - kMenuGradient == 0)
			{
				plot->SetGradientBkgnd(!plot->IsGradientBkgnd());
				if(plot->IsBkgndImage() && plot->IsGradientBkgnd())plot->SetBkgndImage(false);
				return kPopupUpdate;
			}
			if(result - kMenuGradient >= 1 && result - kMenuGradient < 1 + kGradientTypeCount)
			{
				plot->SetGradientType(result - kMenuGradient - 1);
				return kPopupUpdate;
			}
			if(result - kMenuGradient == kGradientTypeCount + 1)
			{
				color = plot->GetGradientColor1();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetGradientColor1(color);
					return kPopupUpdate;
				}
				return kPopupUpdate;
			}
			if(result - kMenuGradient == kGradientTypeCount + 2)
			{
				color = plot->GetGradientColor2();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetGradientColor2(color);
					return kPopupUpdate;
				}
				return kPopupUpdate;
			}
		}
		if( result >= kMenuBkgnd && result < kMenuBkgnd + nBkgndSizes )
		{
			if( result - kMenuBkgnd ==0 )
			{
				color = plot->GetBkgndColor();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetBkgndColor(color);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result - kMenuBkgndBrushType >= 0 && result - kMenuBkgndBrushType < kBrushTypeCount )
			{
				plot->SetBkgndBrushType(result - kMenuBkgndBrushType );
				if(result - kMenuBkgndBrushType == kBrushPattern && !plot->GetBkgndPattern())
				{
					MessageBox(hWnd, _TEXT("No pattern setted, please load file!"), _TEXT("Warning"), MB_OK);
				}
				//
				plot->SetGradientBkgnd(false);
				plot->SetBkgndImage(false);
				return kPopupUpdate;
			}
			if( result - kMenuBkgndHatchType >= 0 && result - kMenuBkgndHatchType < kHatchTypeCount )
			{
				plot->SetBkgndHatchStyle(result - kMenuBkgndHatchType );
				return kPopupUpdate;
			}
			if( result - kMenuBkgndHatchType == kHatchTypeCount)
			{
				color = plot->GetBkgndHatchColor();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetBkgndHatchColor(color);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result - kMenuBkgndHatchType == kHatchTypeCount + 1)
			{
				if(ShowFileDialog( hWnd, true, filename, fileext, filepath,
					_TEXT("Bitmap Files(*.bmp)\0*.BMP\0Png Files(*.png)\0*.PNG\0Jpeg Files(*.jpg)\0*.JPG\0Tiff Files(*.tif)\0*.TIF\0Gif Files(*.gif)\0*.GIF\0All Files(*.*)\0*.*\0\0"),
					_TEXT("bmp") ) )
				{
					HBITMAP hBitmap = HBitmapFromFile(filename);
					if(!hBitmap)
						return kPopupError;
					plot->SetBkgndPattern(hBitmap);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result - kMenuBkgndImage == 0)
			{
				plot->SetBkgndImage(!plot->IsBkgndImage());
				if(!plot->GetBkgndImage())
				{
					MessageBox(hWnd, _TEXT("No pattern setted, please load file!"), _TEXT("Warning"), MB_OK);
				}
				return kPopupUpdate;
			}
			if( result - kMenuBkgndImage == 1)
			{
				if(ShowFileDialog( hWnd, true, filename, fileext, filepath,
					_TEXT("Bitmap Files(*.bmp)\0*.BMP\0Png Files(*.png)\0*.PNG\0Jpeg Files(*.jpg)\0*.JPG\0Tiff Files(*.tif)\0*.TIF\0Gif Files(*.gif)\0*.GIF\0All Files(*.*)\0*.*\0\0"),
					_TEXT("bmp") ) )
				{
					HBITMAP hBitmap = HBitmapFromFile(filename);
					if(!hBitmap)
						return kPopupError;					
					plot->SetBkgndImage(hBitmap);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			
		}

		if( result >= kMenuDataRegion && result < kMenuDataRegion + nDataRegionSizes )
		{
			if( result - kMenuDataRegion == 0 )
			{
				plot->SetUseDataRegionBrush( !plot->IsUseDataRegionBrush() );
				return kPopupUpdate;
			}
			if( result - kMenuDataRegion == 1 )
			{
				color = plot->GetDataRegionColor();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetDataRegionColor(color);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result - kMenuDataRegionBrushType >= 0 && result - kMenuDataRegionBrushType < kBrushTypeCount )
			{
				plot->SetDataRegionBrushType(result - kMenuDataRegionBrushType );
				if(result - kMenuDataRegionBrushType == kBrushPattern && !plot->GetDataRegionPattern())
				{
					MessageBox(hWnd, _TEXT("No pattern setted, please load file!"), _TEXT("Warning"), MB_OK);
				}
				return kPopupUpdate;
			}
			if( result - kMenuDataRegionHatchType >= 0 && result - kMenuDataRegionHatchType < kHatchTypeCount )
			{
				plot->SetDataRegionHatchStyle(result - kMenuDataRegionHatchType );
				return kPopupUpdate;
			}
			if( result - kMenuDataRegionHatchType == kHatchTypeCount )
			{
				color = plot->GetDataRegionHatchColor();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetDataRegionHatchColor(color);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result - kMenuDataRegionHatchType == kHatchTypeCount + 1)
			{
				if(ShowFileDialog( hWnd, true, filename, fileext, filepath,
					_TEXT("Bitmap Files(*.bmp)\0*.BMP\0Png Files(*.png)\0*.PNG\0Jpeg Files(*.jpg)\0*.JPG\0Tiff Files(*.tif)\0*.TIF\0Gif Files(*.gif)\0*.GIF\0All Files(*.*)\0*.*\0\0"),
					_TEXT("bmp") ) )
				{
					HBITMAP hBitmap = HBitmapFromFile(filename);
					if(!hBitmap)
						return kPopupError;
					
					plot->SetDataRegionPattern(hBitmap);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result >= kMenuDataRegionGradient && result < kMenuDataRegionGradient + nGradientSizes)
			{
				if(result - kMenuDataRegionGradient == 0)
				{
					plot->SetGradientDataRegion(!plot->IsGradientDataRegion());
					return kPopupUpdate;
				}
				if(result - kMenuDataRegionGradient >= 1 && result - kMenuDataRegionGradient < 1 + kGradientTypeCount)
				{
					plot->SetDataRegionGradientType(result - kMenuDataRegionGradient - 1);
					return kPopupUpdate;
				}
				if(result - kMenuDataRegionGradient == kGradientTypeCount + 1)
				{
					color = plot->GetDataRegionGradientColor1();
					if(ShowColorDialog(hWnd, color))
					{
						plot->SetDataRegionGradientColor1(color);
						return kPopupUpdate;
					}
					return kPopupUpdate;
				}
				if(result - kMenuDataRegionGradient == kGradientTypeCount + 2)
				{
					color = plot->GetDataRegionGradientColor2();
					if(ShowColorDialog(hWnd, color))
					{
						plot->SetDataRegionGradientColor2(color);
						return kPopupUpdate;
					}
					return kPopupUpdate;
				}
			}
		}
		if( result >= kMenuBorder && result < kMenuBorder + nBorderSizes + 2 )
		{
			if( result - kMenuBorder == 0 )
			{
				color = plot->GetBorderColor();
				if(ShowColorDialog(hWnd, color))
				{
					plot->SetBorderColor(color);
					return kPopupUpdate;
				}
				return kPopupNoAction;
			}
			if( result - kMenuBorder > 0 && result - kMenuBorder <= nLineSizes )
			{
				plot->SetBorderSize( result - kMenuBorder );
				return kPopupUpdate;
			}
			if( result - kMenuBorder == nBorderSizes + 1)
			{
				plot->SetBorderShow(!plot->IsBorderShow());
				return kPopupUpdate;
			}
		}
	}
	return kPopupNoAction;
}

template<class PlotImplT>
bool	IsBkgndItem(PlotImplT *plot, int result)
{
	return ( result >= kMenuBackground && result < kMenuBackground + kMenuBackgroundRange );
}

Declare_Namespace_End