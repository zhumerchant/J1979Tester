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

#include "../../../../Resources/PlotResources.h"
#include <windows.h>
#include "../../../CChartNameSpace.h"

Declare_Namespace_CChart

template<class PlotImplT>
void SetupDataForChartType(HWND hWnd, PlotImplT *plot, int whichDataSet)
{
	CPlotData<DataPoint2D> *data = plot->GetAbsPlotData(whichDataSet);
	
	// Deal with the chart type
	SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_RESETCONTENT, 0, 0);
	switch(CGlobal::m_nLangurage)
	{
	case kLangEnglish:
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Scatter"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Connected"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Step H-V"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Step V-H"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Step H-V-H"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Step V-H-V"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("Bezier"));
		break;
	case kLangChinese:
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("ɢ��ͼ"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("����ͼ"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("̨��ͼH-V"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("̨��ͼV-H"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("̨��ͼH-V-H"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("̨��ͼV-H-V"));
		SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_ADDSTRING, 0, (LPARAM)_TEXT("����������"));
		break;
	}
	SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_SETCURSEL, (WPARAM)plot->GetPlotType(plot->GetDataID(whichDataSet)), 0);
}

template<class PlotImplT>
void SaveDataForChartType(HWND hWnd, PlotImplT *plot, int whichDataSet)
{
	CPlotData<DataPoint2D> *data = plot->GetAbsPlotData(whichDataSet);
#ifndef _WIN64
	int			num;
#else
	LRESULT		num;
#endif
	
	// Deal with the chart type
	num = SendDlgItemMessage(hWnd, IDC_DS_CHART_TYPE, CB_GETCURSEL, 0, 0);
	plot->SetPlotType(plot->GetDataID(whichDataSet), (int)num);
}

template<class PlotImplT>
class ClassForDataChartTypeDlg
{
public:
#ifndef _WIN64
	static BOOL CALLBACK DataChartTypeSettingsFunc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
#else
		static LRESULT CALLBACK DataChartTypeSettingsFunc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
#endif
	{
		static DataSettingParam<PlotImplT> *para;
		static PlotImplT	*plot;
		static int whichDataSet;
		
		switch(message)
		{
		case WM_INITDIALOG:
			para = (DataSettingParam<PlotImplT> *)lparam;
			plot = para->plot;
			whichDataSet = para->whichDataSet;
			
			// Deal with the title
			SetupDataForChartType(hWnd, plot, whichDataSet);
			return TRUE;
		case WM_COMMAND:
			
			switch(LOWORD(wparam))
			{
			case IDOK:
				if(IsWindowVisible(hWnd))
					SaveDataForChartType(hWnd, plot, whichDataSet);
				return TRUE;
			}
			break;
		}
		
		return FALSE;
	}
};

Declare_Namespace_End
