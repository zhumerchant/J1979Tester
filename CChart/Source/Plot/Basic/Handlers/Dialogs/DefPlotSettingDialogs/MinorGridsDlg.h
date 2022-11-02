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

#include "../../../../Resources/PlotResources.h"
#include "../ColorControl.h"

Declare_Namespace_CChart

template<class PlotImplT>
void	SetupDataForMinorGrid(HWND hWnd, PlotImplT *plot)
{
	tostringstream ostr;
	tstring str;
	SendDlgItemMessage(hWnd, IDC_PS_MINOR_HORIZONTAL_GRIDS, BM_SETCHECK, plot->IsMinorHGrids(), 0);
	SendDlgItemMessage(hWnd, IDC_PS_MINOR_VERTICAL_GRIDS, BM_SETCHECK, plot->IsMinorVGrids(), 0);
	SendDlgItemMessage(hWnd, IDC_PS_COLOR_MINOR_GRIDS, WM_SETCOLOR, 0, (LPARAM)plot->GetMinorGridColor()); 
	ostr<<plot->GetMinorGridLineSize();
	str=ostr.str();
	SendDlgItemMessage(hWnd, IDC_PS_MINOR_GRID_SIZE, WM_SETTEXT, 0, (LPARAM)str.c_str()); 
	
	SendDlgItemMessage(hWnd, IDC_PS_MINOR_LINESTYLE, CB_RESETCONTENT, 0, 0);
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("Solid _______________________"));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("Dashed _ _ _ _ _ _ _ _ _ _ _ _ _ _"));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("Dotted . . . . . . . . . . . . . . . . . . . . . ."));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("Dash-Dot _ . _ . _ . _ . _ . _ . _ . _ ."));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("Dash-Dot-Dot _ . . _ . . _ . . _ . . _ ."));
		break;
	case kLangChinese:
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("实线 _______________________"));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("虚划线 _ _ _ _ _ _ _ _ _ _ _ _ _ _"));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("虚点线 . . . . . . . . . . . . . . . . . . . . . ."));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("划点线 _ . _ . _ . _ . _ . _ . _ . _ ."));
		SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_ADDSTRING, 0, (LPARAM)_TEXT("划点点线 _ . . _ . . _ . . _ . . _ ."));
		break;
	}
	SendMessage(GetDlgItem(hWnd,IDC_PS_MINOR_LINESTYLE), CB_SETCURSEL, (WPARAM)plot->GetMinorGridLineStyle(), 0);
}

template<class PlotImplT>
void	SaveDataForMinorGrid(HWND hWnd, PlotImplT *plot)
{
#ifndef _WIN64
	int			check,num;
#else
	LRESULT		check,num;
#endif
	TCHAR strtmp[nMaxString];
	COLORREF color;
	check = SendDlgItemMessage(hWnd, IDC_PS_MINOR_HORIZONTAL_GRIDS, BM_GETCHECK, 0, 0);
	plot->SetMinorHGrids(check==BST_CHECKED);
	check = SendDlgItemMessage(hWnd, IDC_PS_MINOR_VERTICAL_GRIDS, BM_GETCHECK, 0, 0);
	plot->SetMinorVGrids(check==BST_CHECKED);
	
	SendDlgItemMessage(hWnd, IDC_PS_COLOR_MINOR_GRIDS, WM_GETCOLOR, 0, (LPARAM)&color); 
	plot->SetMinorGridColor(color);
	GetWindowText(GetDlgItem(hWnd, IDC_PS_MINOR_GRID_SIZE), strtmp, nMaxString);
	num=(int)_tcstod(strtmp, 0);
	plot->SetMinorGridLineSize((int)num);
	num = SendDlgItemMessage(hWnd, IDC_PS_MINOR_LINESTYLE, CB_GETCURSEL, 0, 0);
	plot->SetMinorGridLineStyle((int)num);
}


template<class PlotImplT>
class ClassForPlotMinorGridsDlg
{
public:
#ifndef _WIN64
	static BOOL	CALLBACK PlotMinorGridsDlgFunc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
#else
		static LRESULT	CALLBACK PlotMinorGridsDlgFunc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
#endif
	{
		static PlotImplT	*plot;
		
		// local varibles
		
		
		LPNMHDR pnmh;
		
		switch(message)
		{
		case WM_INITDIALOG:
			plot = (PlotImplT *)((LPPROPSHEETPAGE)lparam)->lParam;
			SetupDataForMinorGrid(hWnd, plot);		
			return TRUE;
			
		case WM_NOTIFY:
			pnmh = (LPNMHDR) lparam;
			switch(pnmh->code)
			{
			case PSN_APPLY:
				// Apply Changes
				if(IsWindowVisible(hWnd))
					SaveDataForMinorGrid(hWnd, plot);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
};

Declare_Namespace_End