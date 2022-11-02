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

#include "UsedPopup.h"

#include "../../XYPlotImpl.h"
#include "../DefOperation.h"

Declare_Namespace_CChart

// Adds a popup menu for font sizes.
void	AddFontSizePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu = CreatePopupMenu();
	UINT			flag;
	
	for( int i = 0; i < nFontSizes; i++ )
	{
		flag = GetPopupCheckedFlag( compare == fontSizes[i] );
		tostringstream ostr;
		tstring str;
		ostr<<fontSizes[i];
		str=ostr.str();
		AppendMenu(subMenu, MF_STRING | flag, startVal+i, str.c_str() );
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT)subMenu, titleString.c_str() );

	DestroyMenu(subMenu);
}

void	AddLineSizePopup(HMENU addMenu, tstring titleString, long compare, int startVal)
{
	HMENU			subMenu = CreatePopupMenu();
	UINT			flag;

	for(int i=0; i<nLineSizes; i++)
	{
		flag = GetPopupCheckedFlag( compare == i+1 );
		tostringstream ostr;
		tstring str;
		ostr<<i+1;
		str=ostr.str();
		AppendMenu( subMenu, MF_STRING | flag, startVal+i, str.c_str() );
	}
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str() );

	DestroyMenu(subMenu);
}

// Add a popup menu for brush type
void	AddBrushTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == kBrushNull  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushNull , _TEXT("&Null Brush") );
		flag = GetPopupCheckedFlag( compare == kBrushSolid );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushSolid, _TEXT("&Solid Brush") );
		flag = GetPopupCheckedFlag( compare == kBrushHatch );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushHatch, _TEXT("&Hatch Brush") );
		flag = GetPopupCheckedFlag( compare == kBrushPattern );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushPattern, _TEXT("&Pattern Brush") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == kBrushNull  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushNull , _TEXT("�ջ�ˢ(&N)") );
		flag = GetPopupCheckedFlag( compare == kBrushSolid );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushSolid, _TEXT("ʵ�Ļ�ˢ(&S)") );
		flag = GetPopupCheckedFlag( compare == kBrushHatch );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushHatch, _TEXT("��Ӱ��ˢ(&H)") );
		flag = GetPopupCheckedFlag( compare == kBrushPattern );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kBrushPattern, _TEXT("ͼ����ˢ(&P)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());

	DestroyMenu(subMenu);
}

// Add a popup menu for hatch type
void	AddHatchTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();

	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("&Horizontal Hatch") );/* ----- */
		flag = GetPopupCheckedFlag( compare == 1 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("&Vertical Hatch") );/* ||||| */
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("&FDiagonal Hatch") );/* \\\\\ */
		flag = GetPopupCheckedFlag( compare == 3 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("&BDiagonal Hatch") );/* ///// */
		flag = GetPopupCheckedFlag( compare == 4 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("&Cross Hatch") );/* +++++ */
		flag = GetPopupCheckedFlag( compare == 5 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+5, _TEXT("&Diagcross Hatch") );/* xxxxx */
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("ˮƽ(&H)") );/* ----- */
		flag = GetPopupCheckedFlag( compare == 1 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("��ֱ(&V)") );/* ||||| */
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("��б(&F)") );/* \\\\\ */
		flag = GetPopupCheckedFlag( compare == 3 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("��б(&B)") );/* ///// */
		flag = GetPopupCheckedFlag( compare == 4 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("ʮ��(&C)") );/* +++++ */
		flag = GetPopupCheckedFlag( compare == 5 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+5, _TEXT("бʮ��(&D)") );/* xxxxx */
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());

	DestroyMenu(subMenu);
}

// Add a popup menu for langurage
void	AddLanguragePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("&English") );
		flag = GetPopupCheckedFlag( compare == 1 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("&Chinese") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("Ӣ��(&E)") );
		flag = GetPopupCheckedFlag( compare == 1 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("����(&C)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

void	AddTitlePositionPopup(HMENU addMenu, tstring titleString, long compare, int startVal)
{
	HMENU subMenu;
	UINT flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == 0 );
		AppendMenu(subMenu, flag | MF_STRING, startVal, _TEXT("&Left"));
		flag = GetPopupCheckedFlag( compare == 1 );
		AppendMenu(subMenu, flag | MF_STRING, startVal+1, _TEXT("&Middle"));
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, flag | MF_STRING, startVal+2, _TEXT("&Right"));
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == 0 );
		AppendMenu(subMenu, flag | MF_STRING, startVal, _TEXT("��(&L)"));
		flag = GetPopupCheckedFlag( compare == 1 );
		AppendMenu(subMenu, flag | MF_STRING, startVal+1, _TEXT("��(&M)"));
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, flag | MF_STRING, startVal+2, _TEXT("��(&R)"));
		break;
	}
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str());
	DestroyMenu(subMenu);
}

void	AddNumberOfTicksPopup( HMENU addMenu, tstring titleString, long compare, int startVal, BOOL bAuto )
{
	HMENU			subMenu = CreatePopupMenu();
	UINT			flag;
	
	//flag = GetPopupCheckedFlag( compare <= 0 );
	flag = GetPopupCheckedFlag( bAuto );
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("Auto") );
		break;
	case kLangChinese:
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("�Զ�") );
		break;
	}
	
	for( int i = 0; i < nTickSizes; i++ )
	{
		//flag = GetPopupCheckedFlag( compare == tickSizes[i] );
		flag = GetPopupCheckedFlag( compare == tickSizes[i] && !bAuto );
		tostringstream ostr;
		tstring str;
		ostr<<tickSizes[i];
		str=ostr.str();
		AppendMenu(subMenu, MF_STRING | flag, startVal+i+1, str.c_str() );
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str() );
	DestroyMenu(subMenu);
}

void	AddAxisTickPositionPopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU subMenu;
	UINT flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == kTickPositionNone );
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("&None"));
		flag = GetPopupCheckedFlag( compare == kTickPositionInner );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("&Inner"));
		flag = GetPopupCheckedFlag( compare == kTickPositionOuter );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("&Outer"));
		flag = GetPopupCheckedFlag( compare == kTickPositionBoth );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("&Both"));
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == kTickPositionNone );
		AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("��(&N)"));
		flag = GetPopupCheckedFlag( compare == kTickPositionInner );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("�ڲ�(&I)"));
		flag = GetPopupCheckedFlag( compare == kTickPositionOuter );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("�ⲿ(&O)"));
		flag = GetPopupCheckedFlag( compare == kTickPositionBoth );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("˫��(&B)"));
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

void	AddAxisTitlePositionPopup(CAxis *axis, HMENU addMenu, tstring titleString, long compare, int startVal)
{
	HMENU subMenu;
	UINT flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		if(axis->IsXAxis())
		{
			flag = GetPopupCheckedFlag( compare == 0 );
			AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("&Left"));
			flag = GetPopupCheckedFlag( compare == 1 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("&Middle"));
			flag = GetPopupCheckedFlag( compare == 2 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("&Right"));
		}
		else
		{
			flag = GetPopupCheckedFlag( compare == 2 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("&Top"));
			flag = GetPopupCheckedFlag( compare == 1 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("&Middle"));
			flag = GetPopupCheckedFlag( compare == 0 );
			AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("&Bottom"));
		}
		break;
	case kLangChinese:
		if(axis->IsXAxis())
		{
			flag = GetPopupCheckedFlag( compare == 0 );
			AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("��(&L)"));
			flag = GetPopupCheckedFlag( compare == 1 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("��(&M)"));
			flag = GetPopupCheckedFlag( compare == 2 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("��(&R)"));
		}
		else
		{
			flag = GetPopupCheckedFlag( compare == 2 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("��(&T)"));
			flag = GetPopupCheckedFlag( compare == 1 );
			AppendMenu(subMenu, MF_STRING | flag, startVal+1, _TEXT("��(&M)"));
			flag = GetPopupCheckedFlag( compare == 0 );
			AppendMenu(subMenu, MF_STRING | flag, startVal, _TEXT("��(&B)"));
		}
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP | MF_STRING, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

void	AddLegendPosPopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == kLegendArbitrary  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendArbitrary, _TEXT("&Arbitrary") );

		flag = GetPopupCheckedFlag( compare == kLegendInnerLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerLeft, _TEXT("&Inner Left") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerLeftTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerLeftTop, _TEXT("I&nner Left Top") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerLeftBottom  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerLeftBottom, _TEXT("Inn&er Left Bottom") );

		flag = GetPopupCheckedFlag( compare == kLegendInnerRight  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerRight, _TEXT("&Inner Right") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerRightTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerRightTop, _TEXT("I&nner Right Top") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerRightBottom  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerRightBottom, _TEXT("Inn&er Right Bottom") );

		flag = GetPopupCheckedFlag( compare == kLegendInnerTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerTop, _TEXT("&Inner Top") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerBottom  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerBottom, _TEXT("&Inner Bottom") );
		
		flag = GetPopupCheckedFlag( compare == kLegendLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendLeft, _TEXT("&Left") );
		flag = GetPopupCheckedFlag( compare == kLegendLeftTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendLeftTop, _TEXT("L&eft Top") );
		flag = GetPopupCheckedFlag( compare == kLegendLeftBottom );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendLeftBottom, _TEXT("Le&ft Bottom") );
		
		flag = GetPopupCheckedFlag( compare == kLegendRight  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendRight, _TEXT("&Right") );
		flag = GetPopupCheckedFlag( compare == kLegendRightTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendRightTop, _TEXT("R&ight Top") );
		flag = GetPopupCheckedFlag( compare == kLegendRightBottom );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendRightBottom, _TEXT("Ri&ght Bottom") );
		
		flag = GetPopupCheckedFlag( compare == kLegendTop );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendTop, _TEXT("&Top") );
		flag = GetPopupCheckedFlag( compare == kLegendTopLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendTopLeft, _TEXT("T&op Left") );
		flag = GetPopupCheckedFlag( compare == kLegendTopRight );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendTopRight, _TEXT("To&p right") );

		flag = GetPopupCheckedFlag( compare == kLegendBottom );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendBottom, _TEXT("&Bottom") );
		flag = GetPopupCheckedFlag( compare == kLegendBottomLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendBottomLeft, _TEXT("B&ottom Left") );
		flag = GetPopupCheckedFlag( compare == kLegendBottomRight );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendBottomRight, _TEXT("Botto&m right") );
		
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == kLegendArbitrary  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendArbitrary, _TEXT("����λ��(&A)") );

		flag = GetPopupCheckedFlag( compare == kLegendInnerLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerLeft, _TEXT("����(&I)") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerLeftTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerLeftTop, _TEXT("������(&N)") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerLeftBottom  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerLeftBottom, _TEXT("�����£�&E)") );
		
		flag = GetPopupCheckedFlag( compare == kLegendInnerRight  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerRight, _TEXT("����(&I)") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerRightTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerRightTop, _TEXT("������(&N)") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerRightBottom  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerRightBottom, _TEXT("������(&E)") );
		
		flag = GetPopupCheckedFlag( compare == kLegendInnerTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerTop, _TEXT("����(&I)") );
		flag = GetPopupCheckedFlag( compare == kLegendInnerBottom  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendInnerBottom, _TEXT("����(&I)") );
		
		flag = GetPopupCheckedFlag( compare == kLegendLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendLeft, _TEXT("��(&L)") );
		flag = GetPopupCheckedFlag( compare == kLegendLeftTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendLeftTop, _TEXT("����(&E)") );
		flag = GetPopupCheckedFlag( compare == kLegendLeftBottom );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendLeftBottom, _TEXT("����(&F)") );
		
		flag = GetPopupCheckedFlag( compare == kLegendRight  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendRight, _TEXT("��(&R)") );
		flag = GetPopupCheckedFlag( compare == kLegendRightTop  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendRightTop, _TEXT("����(&I)") );
		flag = GetPopupCheckedFlag( compare == kLegendRightBottom );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendRightBottom, _TEXT("����(&G)") );
		
		flag = GetPopupCheckedFlag( compare == kLegendTop );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendTop, _TEXT("��(&T)") );
		flag = GetPopupCheckedFlag( compare == kLegendTopLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendTopLeft, _TEXT("����(&O)") );
		flag = GetPopupCheckedFlag( compare == kLegendTopRight );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendTopRight, _TEXT("����(&P)") );

		flag = GetPopupCheckedFlag( compare == kLegendBottom );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendBottom, _TEXT("��(&B)") );
		flag = GetPopupCheckedFlag( compare == kLegendBottomLeft  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendBottomLeft, _TEXT("����(&O)") );
		flag = GetPopupCheckedFlag( compare == kLegendBottomRight );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kLegendBottomRight, _TEXT("����(&M)") );
		
		break;
	}
		
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());

	DestroyMenu(subMenu);
}

void	AddLegendColumnPopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU		subMenu = CreatePopupMenu();
	UINT		flag;
	
	// Line size
	for( int i = 0; i < nMenuLegendMaxColumn; i++ )
	{
		flag = GetPopupCheckedFlag( compare == i+1 );
		tostringstream ostr;
		tstring str;
		ostr<<i+1;
		str=ostr.str();
		AppendMenu(subMenu, MF_STRING | flag, startVal+i, str.c_str() );
	}
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str() );

	DestroyMenu(subMenu);
}

void	AddPlotTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == kXYPlotScatter  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotScatter , _TEXT("XY &Scatter") );
		flag = GetPopupCheckedFlag( compare == kXYPlotConnect );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotConnect, _TEXT("XY &Line") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepHV );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepHV, _TEXT("Step &H-V") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepVH );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepVH, _TEXT("Step &V-H") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepHVH );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepHVH, _TEXT("S&tep H-V-H") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepVHV );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepVHV, _TEXT("St&ep V-H-V") );
		flag = GetPopupCheckedFlag( compare == kXYPlotBezier );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotBezier, _TEXT("&Bezier Line") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == kXYPlotScatter  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotScatter , _TEXT("ɢ��ͼ(&S)") );
		flag = GetPopupCheckedFlag( compare == kXYPlotConnect );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotConnect, _TEXT("����ͼ(&L)") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepHV );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepHV, _TEXT("̨��ͼ(&H-V)") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepVH );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepVH, _TEXT("̨��ͼ(&V-H)") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepHVH );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepHVH, _TEXT("̨��ͼ(H-V-H)") );
		flag = GetPopupCheckedFlag( compare == kXYPlotStepVHV );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotStepVHV, _TEXT("̨��ͼ(V-H-V)") );
		flag = GetPopupCheckedFlag( compare == kXYPlotBezier );
		AppendMenu(subMenu, MF_STRING | flag, startVal+kXYPlotBezier, _TEXT("����������(&B)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

void	AddTooltipTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("&Screen Coords") );
		flag = GetPopupCheckedFlag( compare == 1  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("&Data Coords") );
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("&Image elements") );
		flag = GetPopupCheckedFlag( compare == 3 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("&X and Y value") );
		flag = GetPopupCheckedFlag( compare == 4 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("&Nearest data") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("��Ļ����(&C)") );
		flag = GetPopupCheckedFlag( compare == 1  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("��������(&D)") );
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2 , _TEXT("ͼԪ��(&I)") );
		flag = GetPopupCheckedFlag( compare == 3 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("����ֵ(&V)") );
		flag = GetPopupCheckedFlag( compare == 4 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("��������ݵ�(&N)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

void	AddApperanceTypePopup( HMENU addMenu, tstring titleString, long compare, int startVal )
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("&Extra small") );
		flag = GetPopupCheckedFlag( compare == 1  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("&Small") );
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("&Normal") );
		flag = GetPopupCheckedFlag( compare == 3 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("&Large") );
		flag = GetPopupCheckedFlag( compare == 4 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("E&xtra large") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("��С(&E)") );
		flag = GetPopupCheckedFlag( compare == 1  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("С(&S)") );
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2, _TEXT("����(&N)") );
		flag = GetPopupCheckedFlag( compare == 3 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+3, _TEXT("��(&L)") );
		flag = GetPopupCheckedFlag( compare == 4 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+4, _TEXT("����(&X)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

void	AddRangeDragPopup(HMENU addMenu, tstring titleString, long compare, int startVal)
{
	HMENU			subMenu;
	UINT			flag;
	
	subMenu = CreatePopupMenu();
	
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("Drag &X") );
		flag = GetPopupCheckedFlag( compare == 1  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("Drag &Y") );
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2 , _TEXT("Drag &Both") );
		break;
	case kLangChinese:
		flag = GetPopupCheckedFlag( compare == 0  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+0 , _TEXT("�϶�&X") );
		flag = GetPopupCheckedFlag( compare == 1  );
		AppendMenu(subMenu, MF_STRING | flag, startVal+1 , _TEXT("�϶�&Y") );
		flag = GetPopupCheckedFlag( compare == 2 );
		AppendMenu(subMenu, MF_STRING | flag, startVal+2 , _TEXT("�϶�ȫ��(&B)") );
		break;
	}
	
	AppendMenu(addMenu, MF_POPUP, (UINT) subMenu, titleString.c_str());
	
	DestroyMenu(subMenu);
}

Declare_Namespace_End