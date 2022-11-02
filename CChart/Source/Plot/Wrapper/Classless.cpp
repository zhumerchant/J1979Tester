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

#include "Chart.h"
#include "Classless.h"//the dll cannot export functions without this line

#include "../Basic/Handler.h"
#include "../Basic/Plot.h"
#include "../Extended/ContourLinePlot/ContourBasicPlot.h"

#define DeclareFunc_0P_Ret(TP, FN) \
	TP FN( )\
	{\
		return pGlobalChartWnd->GetChart()->FN();\
	}

#define DeclareFunc_0P_Noret(TP, FN) \
	TP FN( )\
	{\
		pGlobalChartWnd->GetChart()->FN();\
	}

#define DeclareFunc_1P_Ret(TP, FN, T1, P1) \
	TP FN(T1 P1)\
	{\
		return pGlobalChartWnd->GetChart()->FN(P1);\
	}

#define DeclareFunc_1P_Noret(TP, FN, T1, P1) \
	TP FN(T1 P1)\
	{\
		pGlobalChartWnd->GetChart()->FN(P1);\
	}

#define DeclareFunc_2P_Ret(TP, FN, T1, P1, T2, P2) \
	TP FN(T1 P1, T2 P2)\
	{\
		return pGlobalChartWnd->GetChart()->FN(P1, P2);\
	}

#define DeclareFunc_2P_Noret(TP, FN, T1, P1, T2, P2) \
	TP FN(T1 P1, T2 P2)\
	{\
		pGlobalChartWnd->GetChart()->FN(P1, P2);\
	}

#define DeclareFunc_3P_Ret(TP, FN, T1, P1, T2, P2, T3, P3) \
	TP FN(T1 P1, T2 P2, T3 P3)\
	{\
		return pGlobalChartWnd->GetChart()->FN(P1, P2, P3);\
	}

#define DeclareFunc_3P_Noret(TP, FN, T1, P1, T2, P2, T3, P3) \
	TP FN(T1 P1, T2 P2, T3 P3)\
	{\
		pGlobalChartWnd->GetChart()->FN(P1, P2, P3);\
	}

#define DeclareFunc_4P_Ret(TP, FN, T1, P1, T2, P2, T3, P3, T4, P4) \
	TP FN(T1 P1, T2 P2, T3 P3, T4 P4)\
	{\
		return pGlobalChartWnd->GetChart()->FN(P1, P2, P3, P4);\
	}

#define DeclareFunc_4P_Noret(TP, FN, T1, P1, T2, P2, T3, P3, T4, P4) \
	TP FN(T1 P1, T2 P2, T3 P3, T4 P4)\
	{\
		pGlobalChartWnd->GetChart()->FN(P1, P2, P3, P4);\
	}

#define DeclareFunc_5P_Ret(TP, FN, T1, P1, T2, P2, T3, P3, T4, P4, T5, P5) \
	TP FN(T1 P1, T2 P2, T3 P3, T4 P4, T5 P5)\
	{\
	return pGlobalChartWnd->GetChart()->FN(P1, P2, P3, P4, P5);\
	}

#define DeclareFunc_5P_Noret(TP, FN, T1, P1, T2, P2, T3, P3, T4, P4, T5, P5) \
	TP FN(T1 P1, T2 P2, T3 P3, T4 P4, T5 P5)\
	{\
	pGlobalChartWnd->GetChart()->FN(P1, P2, P3, P4, P5);\
	}

#define DeclareFunc_6P_Ret(TP, FN, T1, P1, T2, P2, T3, P3, T4, P4, T5, P5, T6, P6) \
	TP FN(T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6)\
	{\
	return pGlobalChartWnd->GetChart()->FN(P1, P2, P3, P4, P5, P6);\
	}

#define DeclareFunc_6P_Noret(TP, FN, T1, P1, T2, P2, T3, P3, T4, P4, T5, P5, T6, P6) \
	TP FN(T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6)\
	{\
	pGlobalChartWnd->GetChart()->FN(P1, P2, P3, P4, P5, P6);\
	}

using namespace NsCChart; 

namespace Classless
{
	// ȫ�ֵ�CChartWnd����
	static CChartWnd *pGlobalChartWnd = 0;
	static int nCreateCount = 0;

	void	CreateChart()
	{
		if(nCreateCount==0 && !pGlobalChartWnd)
		{
			pGlobalChartWnd = new CChartWnd;
		}
		nCreateCount++;
	}
	void	DestroyChart()
	{
		nCreateCount--;
		if(nCreateCount<=0 && pGlobalChartWnd)
		{
			delete pGlobalChartWnd;
			pGlobalChartWnd=0;
		}
	}
	bool	IsCreated()
	{
		return pGlobalChartWnd != 0;
	}
	// ����һ����������
	HWND	CreatePopupChartWndGlobal(HWND hWndParent, int nChartType, TCHAR *wndTitle/*=_TEXT("")*/, int x/*=0*/, int y/*=0*/, int cx/*=CW_USEDEFAULT*/, int cy/*=CW_USEDEFAULT*/)
	{
		if(hWndParent)
		{
			POINT point;
			point.x = x;
			point.y = y;
			ClientToScreen(hWndParent, &point);
			x = point.x;
			y = point.y;
		}
		HWND hWnd = CreateWindow(_TEXT("button"), wndTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP,
			x, y, cx, cy, hWndParent, NULL, GetModuleHandle(NULL), NULL);
		if(hWnd)
		{
			if(!IsCreated())CreateChart();
			pGlobalChartWnd->Attach(hWnd, nChartType);
			pGlobalChartWnd->GetChart()->SetContinuePaint(false);
			pGlobalChartWnd->GetChart()->SetContinueReact(false);

			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
		}
		return hWnd;
	}

	typedef struct stCreateWndParam
	{
		HWND hWndParent;
		int nChartType;
		TCHAR *wndTitle;
		int x, y, cx, cy;
		
		HWND hWndCreated;
	}CreateWndParam, *pCreateWndParam;
	
	HANDLE hEvent4CreateWnd = 0;
	
	DWORD WINAPI MyCreateWndThread(LPVOID pArg)
	{
		if(!pArg)return -1;
		
		CreateWndParam *pCwp = (CreateWndParam *)pArg;
		HWND hWnd = CreatePopupChartWndGlobal(pCwp->hWndParent, pCwp->nChartType, pCwp->wndTitle, pCwp->x, pCwp->y, pCwp->cx, pCwp->cy);
		pCwp->hWndCreated = hWnd;
		
		SetEvent(hEvent4CreateWnd);
		
		//The message loop    
		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		return 0;
	}
	// ����һ��������Ϣѭ���ĵ�������
	HWND	CreatePopupChartWndGlobalWithMsgLoop(HWND hWndParent, int nChartType, TCHAR *wndTitle/*=_TEXT("")*/, int x/*=0*/, int y/*=0*/, int cx/*=CW_USEDEFAULT*/, int cy/*=CW_USEDEFAULT*/)
	{
		CreateWndParam *pCwp = new CreateWndParam;
		pCwp->hWndParent = hWndParent;
		pCwp->nChartType = nChartType;
		pCwp->wndTitle = wndTitle;
		pCwp->x = x;
		pCwp->y = y;
		pCwp->cx = cx;
		pCwp->cy = cy;
		
		hEvent4CreateWnd = CreateEvent(NULL, TRUE, FALSE, NULL);
		
		HANDLE hThread = CreateThread(NULL, 0, MyCreateWndThread, pCwp, 0, NULL);
		WaitForSingleObject(hEvent4CreateWnd, INFINITE);
		CloseHandle(hThread);
		CloseHandle(hEvent4CreateWnd);
		
		return pCwp->hWndCreated;
	}
	// ����һ���Ӵ���
	HWND	CreateSubChartWndGlobal(HWND hWndParent, int nChartType, TCHAR *wndTitle/*=_TEXT("")*/, int x/*=0*/, int y/*=0*/, int cx/*=CW_USEDEFAULT*/, int cy/*=CW_USEDEFAULT*/)
	{
		HWND hWnd = CreateWindow(_TEXT("ChartCtrl"), wndTitle, WS_CHILD | WS_VISIBLE,
			x, y, cx, cy, hWndParent, NULL, GetModuleHandle(NULL), NULL);
		if(hWnd)
		{
			if(!IsCreated())CreateChart();
			pGlobalChartWnd->Attach(hWnd, nChartType);
			pGlobalChartWnd->GetChart()->SetContinuePaint(false);
			pGlobalChartWnd->GetChart()->SetContinueReact(false);

			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
		}
		return hWnd;
	}
	/////////////////////////////////////////////////////////////////////////////
	// ���º�������CChartWnd
	// ���ݱ�ʶ������
	int		GetChartIndex(int nChartID)
	{
		return pGlobalChartWnd->GetChartIndex(nChartID);
	}
	// ������Ż�ñ�ʶ
	int		GetChartID(int nChartIndex)
	{
		return pGlobalChartWnd->GetChartID(nChartIndex);
	}
	// �����ͼ����
	int		GetChartType(int nChartIndex)
	{
		return pGlobalChartWnd->GetChartType(nChartIndex);
	}
	// �ı���ͼ����
	bool	SetChartType(int nType, int nChartIndex)
	{
		return pGlobalChartWnd->SetChartType(nType, nChartIndex);
	}
	// �����Attach�Ĵ���
	int		GetChartCount()
	{
		return pGlobalChartWnd->GetChartCount();
	}
	
	// ճ���ڣ��������ʽ
	int		Attach(HWND hWnd)
	{
		return pGlobalChartWnd->Attach(hWnd);
	}
	// ճ���ڣ������ƻ�ͼ��
	int		Attach(HWND hWnd, RECT rtClient)
	{
		return pGlobalChartWnd->Attach(hWnd, rtClient);
	}
	// ճ���ڣ�ճ���Ի�����, �����ؼ����
	int		AttachDlg(HWND hDlg, HWND hCtrl)
	{
		return pGlobalChartWnd->AttachDlg(hDlg, hCtrl);
	}
	// ճ���ڣ�ճ���Ի����ϣ������ؼ�ID
	int		AttachDlg(HWND hDlg, UINT nCtrlID)
	{
		return pGlobalChartWnd->AttachDlg(hDlg, nCtrlID);
	}
	// ճ���ڣ�ͬʱ���û�ͼ����
	int		Attach(HWND hWnd, int nType)
	{
		return pGlobalChartWnd->Attach(hWnd, nType);
	}
	// ճ���ڣ�ͬʱ���û�ͼ���ͣ������ƻ�ͼ��
	int		Attach(HWND hWnd, RECT rtClient, int nType)
	{
		return pGlobalChartWnd->Attach(hWnd, rtClient, nType);
	}
	// ճ���ڣ�ճ���Ի�����, �����ؼ����
	int		AttachDlg(HWND hDlg, HWND hCtrl, int nType)
	{
		return pGlobalChartWnd->AttachDlg(hDlg, hCtrl, nType);
	}
	// ճ���ڣ�ճ���Ի����ϣ������ؼ�ID
	int		AttachDlg(HWND hDlg, UINT nCtrlID, int nType)
	{
		return pGlobalChartWnd->AttachDlg(hDlg, nCtrlID, nType);
	}
	// ж����
	bool	Detach()
	{
		return pGlobalChartWnd->Detach();
	}
	bool	Detach(HWND hWnd)
	{
		return pGlobalChartWnd->Detach(hWnd);
	}
	// �ػ�
	void	ReDraw()
	{
		pGlobalChartWnd->ReDraw();
	}
	void	ReDraw(HWND hWnd)
	{
		pGlobalChartWnd->ReDraw(hWnd);
	}
	void	ReDraw(int nChartIndex)
	{
		pGlobalChartWnd->ReDraw(nChartIndex);
	}
	//void	ReDraw(CChart *chart)
	//{
	//	pGlobalChartWnd->ReDraw(chart);
	//}
	
	/////////////////////////////////////////////////////////////////////////////
	//CChart_API  ���º�������CChart

	// ���öԻ����Ҽ��˵�ʹ�õ�����
	DeclareFunc_1P_Noret(void, SetLangurage, int, nLang);
	
	// �������ݴ���˵��Ƿ���ʾ
	DeclareFunc_1P_Noret(void, SetDataProcessing, bool, process);
	
	// �����Ƿ����ù�����ʾ
	DeclareFunc_0P_Ret(bool, IsEnableToolTip);
	// �����Ƿ����ù�����ʾ
	DeclareFunc_1P_Noret(void, SetEnableToolTip, bool, enable);
	// �����ͼ���Ƿ���Ч
	DeclareFunc_1P_Ret(bool, CheckPlotType, int, nType);	
	
	// ����Ĭ����ɫ���ñ�
	DeclareFunc_1P_Noret(void, SetColorTable, int, nTableIndex);

	// �����Ƿ���ʾͼ������Ҽ��˵�
	DeclareFunc_1P_Noret(void, SetImagePopupMenu, bool, bShow);

	// ����ͼ�񵽼�����
	DeclareFunc_1P_Noret(void, CopyImageToClipBoard, HWND, hWnd);
	// ����ͼ���ļ�
	DeclareFunc_3P_Noret(void, SaveImageToFile, HWND, hWnd, TCHAR *, strFileName, TCHAR *, strFileExt);
	// ��ӡͼ��
	DeclareFunc_1P_Noret(void, PrintImage, HWND, hWnd);

	// ��ȡ�ڲ�Plotָ��
	DeclareFunc_0P_Ret(void	*, GetPlot);
	// ��ȡͼ�����ͱ���
	DeclareFunc_0P_Ret(int,	GetType);
	// ����ͼ�����ͱ��룬ע�⽫�����������
	DeclareFunc_1P_Ret(bool, SetType, int, nType);
	// ����ͼ�����ͱ��룬�����û�ͼ���ڣ���Ҫ������ά��ͼ����ά��ͼ������hWnd
	DeclareFunc_2P_Ret(bool, SetType, int, nType, HWND, hWnd);
	// ����ͼ�����ͱ��룬ע�⽫�����������
	DeclareFunc_1P_Ret(bool, CreateChart, int, nType);
	// ����ͼ�����ͱ��룬�����û�ͼ���ڣ���Ҫ������ά��ͼ����ά��ͼ������hWnd
	DeclareFunc_2P_Ret(bool, CreateChart, int, nType, HWND, hWnd);

	// �����Ƿ����ƻ�ͼ��
	DeclareFunc_0P_Ret(bool, IsRectConfined);
	// �����Ƿ����ƻ�ͼ��
	DeclareFunc_1P_Noret(void, SetRectConfined, bool, confine);
	// ������������
	DeclareFunc_1P_Ret(bool, SetConfineRect, RECT, ConfineRect);
	// ��ȡ��������
	DeclareFunc_0P_Ret(RECT, GetConfineRect);

	// �����ڲ��Ƿ�������
	DeclareFunc_0P_Ret(bool, IsEmpty);
	
	// �����������
	DeclareFunc_0P_Noret(void, ResetApperance);
	// ������ģʽ���
	DeclareFunc_0P_Ret(int, GetApperanceMode);
	// �������ģʽ
	DeclareFunc_1P_Noret(void, SetApperanceMode, int, mode);

	// ���û�ͼ��ɺ��û�������Խ��Ż�ͼ
	DeclareFunc_1P_Noret(void, SetContinuePaint, bool, ctn);
	// ������Ϣ������û�������Խ��Ŵ���
	DeclareFunc_1P_Noret(void, SetContinueReact, bool, ctn);

	// �����Ƿ���Ӧ��������Ϣ
	DeclareFunc_0P_Ret(bool, IsReact);
	// �����Ƿ���Ӧ��������Ϣ
	DeclareFunc_1P_Noret(void, SetReact, bool, react);
	// �����Ƿ���Ӧ�������������Ϣ
	DeclareFunc_1P_Noret(void, SetReactLButtonDown, bool, react);
	// �����Ƿ���Ӧ�������̧����Ϣ
	DeclareFunc_1P_Noret(void, SetReactLButtonUp, bool, react);
	// �����Ƿ���Ӧ���˫����Ϣ
	DeclareFunc_1P_Noret(void, SetReactLButtonDblClk, bool, react);
	// �����Ƿ���Ӧ����ƶ���Ϣ
	DeclareFunc_1P_Noret(void, SetReactMouseMove, bool, react);
	// �����Ƿ���Ӧ�Ҽ��˵���Ϣ
	DeclareFunc_1P_Noret(void, SetReactContextMenu, bool, react);
	// �����Ƿ���Ӧ������Ϣ
	DeclareFunc_1P_Noret(void, SetReactKeyDown, bool, react);
	// �����Ƿ���Ӧ����
	DeclareFunc_1P_Noret(void, SetReactTitle, bool, react);
	// �����Ƿ���Ӧ������
	DeclareFunc_1P_Noret(void, SetReactAxes, bool, react);
	// �����Ƿ���Ӧͼ��
	DeclareFunc_1P_Noret(void, SetReactLegend, bool, react);
	// ������Ϣ��Ӧʱ�Ƿ��Զ�ˢ�£���������ĩβ��R�ļ���������Interactive
	// ��Ҫ����ʵʱ������ʾ,��ʱ����Ϊfalse
	DeclareFunc_0P_Ret(bool, IsAutoRedraw);
	// ������Ϣ��Ӧʱ�Ƿ��Զ�ˢ�£���������ĩβ��R�ļ���������Interactive
	// ��Ҫ����ʵʱ������ʾ,��ʱ����Ϊfalse
	DeclareFunc_1P_Noret(void, SetAutoRedraw, bool, redraw);
	// ���û�ͼ��ƫ��
	DeclareFunc_2P_Noret(void, SetOffsetClient, int, offsetX, int, offsetY);

	// ��SetWnd()���õĴ��ھ����ͼ
	DeclareFunc_0P_Noret(void, OnDraw);
	// �Դ��ھ����ͼ
	DeclareFunc_1P_Noret(void, OnDraw, HWND, hWnd);
	// ���豸�����Ļ�ͼ
	DeclareFunc_1P_Noret(void, OnDraw, HDC, hDC);
	// �ڴ�����ѡ�������ͼ
	DeclareFunc_2P_Noret(void, OnDraw, HDC, hDC, RECT, destRect);
	// ���豸�����Ķ�Ӧ�Ĵ����ϣ�ѡ�������ͼ
	DeclareFunc_2P_Noret(void, OnDraw, HWND, hWnd, RECT, destRect);

	// ��������������Ϣ�������ػ棬���û����ݷ���ֵ�ػ�
	DeclareFunc_3P_Ret(bool, OnLButtonDown, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Ret(bool, OnLButtonUp, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Ret(bool, OnLButtonDblClk, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Ret(bool, OnMouseMove, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Ret(bool, OnMouseLeave, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Ret(bool, OnMouseWheel, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Ret(bool, OnContextMenu, HMENU, hMenu, HWND, hWnd, POINT, point );
	DeclareFunc_2P_Ret(bool, OnKeyDown, HWND, hWnd, UINT, key );

	// ��������������Ϣ����AutoRedrawΪ��������ػ�
	DeclareFunc_3P_Noret(void, OnLButtonDownR, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Noret(void, OnLButtonUpR, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Noret(void, OnLButtonDblClkR, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Noret(void, OnMouseMoveR, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Noret(void, OnMouseLeaveR, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Noret(void, OnMouseWheelR, HWND, hWnd, POINT, point, UINT, ctrlKey );
	DeclareFunc_3P_Noret(void, OnContextMenuR, HMENU, hMenu, HWND, hWnd, POINT, point );
	DeclareFunc_2P_Noret(void, OnKeyDownR, HWND, hWnd, UINT, key );

	// ��Ӧ��Ϣ�������ػ棬���û����ݷ���ֵ�ػ�
	DeclareFunc_4P_Ret(bool, OnEvent, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam);
	// ��Ӧ��Ϣ����AutoRedrawΪ��������ػ�
	DeclareFunc_4P_Noret(void, Interactive, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam);

	// �����Ҽ��˵������˵�����
	DeclareFunc_4P_Noret(void, SetPluginMenu, FcnAddPluginPopupMenu, fcnAddPluginMenu, FcnInterpretPluginPopupMenu, fcnInterpretPluginMenu, void *, pParaAdd, void *, pParaInt);
	// ���ػ���ʾȱʡ�Ҽ��˵�����ϲ���˵����ܿ�ʵ���Ҽ��˵�ȫ���Զ���
	DeclareFunc_1P_Noret(void, SetDefMenus, bool, bShow);
	// �û��Զ����ͼ����
	DeclareFunc_2P_Noret(void, SetUserDrawingFunc, FcnUserDrawing, fcnUserDrawing, void *, pPara);
	// �û��Զ����ͼ����
	DeclareFunc_3P_Noret(void, SetUserDrawingFunc, FcnUserDrawing, fcnUserDrawing, void *, pPara, int, nPlotIndex);
	// �û��Զ�����²���
	DeclareFunc_2P_Noret(void, SetUserUpdateFunc, FcnUserUpdate, fcnUserUpdate, void *, pPara);
	// �û��Զ�����²���
	DeclareFunc_3P_Noret(void, SetUserUpdateFunc, FcnUserUpdate, fcnUserUpdate, void *, pPara, int, nPlotIndex);
	
	// �û��Զ�������ƶ�ǰ������bContinue����Ϊfalse����Ĭ������
	DeclareFunc_2P_Noret(void, SetPreMouseMoveFunc, FcnAddonMouseAction, fcnPreMouseMove, void *, pPara);
	// �û��Զ�����굥��ǰ������bContinue����Ϊfalse����Ĭ������
	DeclareFunc_2P_Noret(void, SetPreLButtonDownFunc, FcnAddonMouseAction, fcnPreLButtonDown, void *, pPara);
	// �û��Զ������̧��ǰ������bContinue����Ϊfalse����Ĭ������
	DeclareFunc_2P_Noret(void, SetPreLButtonUpFunc, FcnAddonMouseAction, fcnPreLButtonUp, void *, pPara);
	// �û��Զ������˫��ǰ������bContinue����Ϊfalse����Ĭ������
	DeclareFunc_2P_Noret(void, SetPreLButtonDblClkFunc, FcnAddonMouseAction, fcnPreLButtonDblClk, void *, pPara);
	// �û��Զ��尴��ǰ������bContinue����Ϊfalse����Ĭ������
	DeclareFunc_2P_Noret(void, SetPreKeyDownFunc, FcnAddonKeyAction, fcnPreKeyDown, void *, pPara);
	
	// �û��Զ�������ƶ��������bContinue������
	DeclareFunc_2P_Noret(void, SetPostMouseMoveFunc, FcnAddonMouseAction, fcnPostMouseMove, void *, pPara);
	// �û��Զ�����굥���������bContinue������
	DeclareFunc_2P_Noret(void, SetPostLButtonDownFunc, FcnAddonMouseAction, fcnPostLButtonDown, void *, pPara);
	// �û��Զ������̧��������bContinue������
	DeclareFunc_2P_Noret(void, SetPostLButtonUpFunc, FcnAddonMouseAction, fcnPostLButtonUp, void *, pPara);
	// �û��Զ������˫���������bContinue������
	DeclareFunc_2P_Noret(void, SetPostLButtonDblClkFunc, FcnAddonMouseAction, fcnPostLButtonDblClk, void *, pPara);
	// �û��Զ��尴���������bContinue������
	DeclareFunc_2P_Noret(void, SetPostKeyDownFunc, FcnAddonKeyAction, fcnPostKeyDown, void *, pPara);

	// �����ͼ��ţ���������kTypeSplit
	DeclareFunc_1P_Ret(int, GetPlotIndexByMousePos, POINT, point);
	
	// ������û�����ݵ�ʱ���Ƿ������ἰ����
	DeclareFunc_1P_Noret(void, SetDrawBasicAnyway, bool, draw);
	// ������û�����ݵ�ʱ����ͼ�Ƿ������ἰ����
	DeclareFunc_2P_Noret(void, SetDrawBasicAnyway, bool, draw, int, nPlotIndex);
	// ����������ʱĬ����Ļ
	DeclareFunc_2P_Noret(void, SetDefScreen, FcnDefScreen, fcnDefScreen, void *, pPara );
	// ����������ʱ��ͼĬ����Ļ
	DeclareFunc_3P_Noret(void, SetDefScreen, FcnDefScreen, fcnDefScreen, void *, pPara, int, nPlotIndex );
	// ����Ĭ����Ļ�����ַ���
	DeclareFunc_1P_Noret(void, SetDefMainString, TCHAR *, str);
	// ������ͼĬ����Ļ�����ַ���
	DeclareFunc_2P_Noret(void, SetDefMainString, TCHAR *, str, int, nPlotIndex);
	//  ����Ĭ����Ļ�ĸ��ַ���
	DeclareFunc_1P_Noret(void, SetDefSubString, TCHAR *, str);
	//  ������ͼĬ����Ļ�ĸ��ַ���
	DeclareFunc_2P_Noret(void, SetDefSubString, TCHAR *, str, int, nPlotIndex);
	//  ����Ĭ����Ļ�ı���ͼ
	DeclareFunc_1P_Noret(void, SetDefImage, HBITMAP, hbm);
	//  ������ͼĬ����Ļ�ı���ͼ
	DeclareFunc_2P_Noret(void, SetDefImage, HBITMAP, hbm, int, nPlotIndex);


	// �����ͼ����Ƿ��ڷ�Χ��
	DeclareFunc_1P_Ret(bool, CheckSubPlotIndex, int, nPlotIndex);

	// ��ù�����ʾ�ַ���
	DeclareFunc_2P_Ret(TCHAR*, GetDescriptionByMousePos, HDC, hDC, POINT, point);
	// ���ù�����ʾ�ַ�������
	DeclareFunc_1P_Noret(void, SetToolTipType, int, type);

	// ������������ģʽ���򿪺����Ӧ����Ϸ�
	DeclareFunc_1P_Noret(void, SetZoomModeBuildIn, bool, zoom);
	// �������ŷ�Χ
	DeclareFunc_0P_Noret(void, ResetZoomedRanges);
	// ����ͼ����������ģʽ��ֻ�ǻ��ƣ������ڲ�����CChartWnd�����
	DeclareFunc_1P_Noret(void, SetZoomMode, bool, zoom);
	// ����ͼ����������ʱX���������ϵ��
	DeclareFunc_1P_Noret(void, SetZoomFactorX, double, fact);
	// ����ͼ����������ʱY���������ϵ��
	DeclareFunc_1P_Noret(void, SetZoomFactorY, double, fact);
	// ����ͼ����������ʱ�������������ϵ��
	DeclareFunc_1P_Noret(void, SetZoomFactor, double, fact);
	// ����ͼ����������ʱ���������ģ����ĵĴ��ż����ļ���ʼ��ע�͵���enum
	DeclareFunc_1P_Noret(void, SetZoomCenterMode, int, center);

	// ������ߣ�ǰ������������������
	// ���ĸ�������Ҫ���ڷ�����ͼ���ֲ���ͼ������X����ͼ����ʾ��ͼ��š�����������ͬ
	// ����ֵ�����ߵ�ID�ţ�ע�ⲻ����ţ�����-1��ʾ�������ʧ�ܡ�����������ͬ
	DeclareFunc_4P_Ret(int, AddCurve, double *, pX, double *, pY, int, nLen, int, nPlotIndex);
	// ������ߣ�����X����Ϊʱ�䣬���ַ�����ʽ����
	// �ַ���ӦΪ"20130528234428"����������ʱ���룬��14λ���֣��������
	// ʱ��ֻ�ܾ�ȷ���룬��Ϊ�ڲ�ʹ��time_t
	DeclareFunc_5P_Ret(int, AddCurve, TCHAR **, pStrTime, TCHAR *, fmt, double *, pY, int, nLen, int, nPlotIndex);
	// ��ӿ�����
	DeclareFunc_1P_Ret(int, AddCurve, int, nPlotIndex);
	// ������������
	DeclareFunc_5P_Ret(int, UpdateCurve, int, nDataIndex, double *, pX, double *, pY, int, nLen, int, nPlotIndex);
	// ����ID�Ÿ�����������
	DeclareFunc_5P_Ret(int, UpdateCurveByID, int, nDataID, double *, pX, double *, pY, int, nLen, int, nPlotIndex);
	// �滻�������ݣ������·�Χ
	DeclareFunc_5P_Ret(int, ReplaceCurve, int, nDataIndex, double *, pX, double *, pY, int, nLen, int, nPlotIndex);
	// ����ID���滻�������ݣ������·�Χ
	DeclareFunc_5P_Ret(int, ReplaceCurveByID, int, nDataID, double *, pX, double *, pY, int, nLen, int, nPlotIndex);
	// �����ߵ�DataID���Index
	DeclareFunc_2P_Ret(int, GetIndex, int, dataID, int, nPlotIndex);
	// �����ߵ�Index���DataID
	DeclareFunc_2P_Ret(int, GetDataID, int, nIndex, int, nPlotIndex);
	// �������ߵ�dataID��������;���������콣����������Ҫ������
	DeclareFunc_3P_Noret(void, SetDataID, int, newID, int, nIndex, int, nPlotIndex);

	// ���2ά���ߵ�һ�����ݵ�
	// nDataIndex��ʾ���ߵ����,
	// nDataIndex������ڵ�ǰ���ߵ�������������һ�����ߣ����С��0���ߴ��ڵ�ǰ���������������ʧ��
	DeclareFunc_4P_Ret(int, AddPoint2D, double, x, double, y, int, nDataIndex, int, nPlotIndex);
	// ������ݵ㣬����X����Ϊʱ�䣬���ַ�����ʽ����
	// �ַ�����ʽ��fmtȷ�����ο�C�⺯��strftime
	// ʱ��ֻ�ܾ�ȷ���룬��Ϊ�ڲ�ʹ��time_t
	DeclareFunc_5P_Ret(int, AddPoint2D, TCHAR *, strTime, TCHAR *, fmt, double, y, int, nDataIndex, int, nPlotIndex);
	// ����2ά���ߵ�һ�����ݵ�
	// nPos��ʾ��Ҫ�����λ��,
	DeclareFunc_5P_Ret(int, InsertPoint2D, double, x, double, y, int, nPos, int, nDataIndex, int, nPlotIndex);

	// �����ά���ߵ�һ�����ݵ�
	DeclareFunc_4P_Ret(int, AddPoint3D, double, x, double, y, double, z, int, nDataIndex);
	// ���һ����ά����
	DeclareFunc_4P_Ret(int, AddCurve, double *, pX, double *, pY, double *, pZ, int, nLen);	

	// ���������������
	DeclareFunc_0P_Noret(void, ClrAllData);
	// �������ͼȫ����������
	DeclareFunc_1P_Noret(void, ClrPlotData, int, nPlotIndex);
	// ���������������
	DeclareFunc_2P_Noret(void, ClrSingleData, int, nDataIndex, int, nPlotIndex);
	// ��յ�����������
	DeclareFunc_2P_Noret(void, EmptySingleData, int, nDataIndex, int, nPlotIndex);
	// Ϊ����Ԥ���ڴ�ռ䡣��ҪĿ���Ǽӿ��ٶ�
	// CChart�ڲ����ݲ���vector��������ݵ���࣬�������ݵ�����ӣ�CChart�����ϵ�Ϊvector���·����ڴ棬Ӱ���ٶ�
	// �������ʵ�����Ԥ�����ڴ棬���һ���Գɹ������ڴ�
	DeclareFunc_2P_Noret(void, SetReservedDataLength, int, nLen, int, nPlotIndex);
	// ������ߵ����ݵ���
	DeclareFunc_2P_Ret(int, GetDataPointsCount, int, nDataIndex, int, nPlotIndex);
	// ��ȡ���ݵ�x
	DeclareFunc_3P_Ret(double, GetDataPointX, int, nPointIndex, int, nDataIndex, int, nPlotIndex);
	// ��ȡ���ݵ�y
	DeclareFunc_3P_Ret(double, GetDataPointY, int, nPointIndex, int, nDataIndex, int, nPlotIndex);
	//��ȡ���ݵ�
	DeclareFunc_4P_Ret(bool, GetDataPoint, double *, data, int, nPointIndex, int, nDataIndex, int, nPlotIndex);
	// ȥ�������ϵ������ݵ�
	DeclareFunc_3P_Ret(int, RemoveSinglePoint, int, nPointIndex, int, nDataIndex, int, nPlotIndex);
	// ȥ�������ϵ�һ�����ݵ�
	DeclareFunc_2P_Ret(int, RemoveFirstPoint, int, nDataIndex, int, nPlotIndex);
	// ȥ�������ϵ�һ�����ݵ㣬ͬʱ�ڲ��������ƣ��ɱ�֤ռ���ڴ��λ�ò��䣬��Ч�ʱ�RemoveFirstPoint�͡�
	DeclareFunc_2P_Ret(int, RemoveFirstPointAndShift, int, nDataIndex, int, nPlotIndex);
	// ȥ�����������һ�����ݵ�
	DeclareFunc_2P_Ret(int, RemoveLastPoint, int, nDataIndex, int, nPlotIndex);
	// ����X����
	DeclareFunc_3P_Ret(int, SlipXValue, int, nStep, int, nDataIndex, int, nPlotIndex);
	// ����Y����
	DeclareFunc_3P_Ret(int, SlipYValue, int, nStep, int, nDataIndex, int, nPlotIndex);

	// ��ӱ�ͼ��һ��ֵ
	DeclareFunc_1P_Ret(int, AddPie, double, value);
	// ��ӱ�ͼ��һ��ֵ��ͬʱ���øÿ���ı���
	DeclareFunc_2P_Ret(int, AddPie, double, value, TCHAR*, title);
	// �����ͼ��һ������
	DeclareFunc_2P_Ret(int, AddStems, double *, pData, int, nLen);
	// �����ͼ��һ�����У�ͬʱ���ø����еı���
	DeclareFunc_3P_Ret(int, AddStems, double *, pData, int, nLen, TCHAR*, title);

	// ���ó����������ڵȸ���ͼ����ͼ
	DeclareFunc_1P_Noret(void, SetFieldFcn, FcnField, _pFieldFcn);
#ifndef _WIN64
	void		SetFieldFcn(double (_stdcall *_pFieldFcn) (double, double))
	{
		pGlobalChartWnd->GetChart()->SetFieldFcn(_pFieldFcn);
	}
#endif
	// ��ó�����ָ�룬���ڵȸ���ͼ����ͼ
	DeclareFunc_0P_Ret(FcnField*, GetFieldFcn );
	// ��ӵȸ������ݵ�
	DeclareFunc_3P_Noret(void, AddContourPoint, double, x, double, y, double, h);
	// ����ȸ������ݵ�
	DeclareFunc_0P_Noret(void, ClrContourPoints);
	// ���ñ�ͼ�������ı���
	DeclareFunc_2P_Noret(void, SetPieTitle, const TCHAR*, title, int, which);
	// ���ñ�ͼ����������ɫ
	DeclareFunc_2P_Noret(void, SetPieColor, COLORREF, color, int, which);
	// ���ñ�ͼ������
	DeclareFunc_1P_Noret(void, SetMaxPies, int, maxp);
	// ��ñ�ͼ����������
	DeclareFunc_0P_Ret(LOGFONT	&, GetPieTitleFont);
	
	// ������ͼ�������ӵı�ǩ
	DeclareFunc_2P_Noret(void, SetStemLabel, const TCHAR*, label, int, which);
	// ������ͼ�������еı���
	DeclareFunc_2P_Noret(void, SetStemTitle, const TCHAR*, title, int, nDataIndex);
	// ������ͼ����������ռ����
	DeclareFunc_1P_Noret(void, SetStemRatio, double, ratio);
	// ������ͼ�����������Ὺʼ��
	DeclareFunc_1P_Noret(void, SetStemBase, int, base);
	// ������ͼ���������
	DeclareFunc_1P_Noret(void, SetStemMaxNum, int, num);

	// ���õȸ���ͼ�ȸ��ߵ�����
	DeclareFunc_1P_Noret(void, SetContourLineNum, int, num);
	// ���õȸ���ͼ�ȸ��߻��Ƶľ��ȣ�nԽ�󣬾���Խ�ߣ�һ��5�Ϳ�����
	DeclareFunc_1P_Noret(void, SetContourPrecision, int, n);
	// ������ʾ�ȸ�����ֵ
	DeclareFunc_1P_Noret(void, SetContourValueShow, bool, show);
	
	// ������ͼ����ʼ��ɫ
	DeclareFunc_1P_Noret(void, SetContourMapColor1, COLORREF, cr);
	// ������ͼ��������ɫ
	DeclareFunc_1P_Noret(void, SetContourMapColor2,COLORREF, cr);
	
	// ���������ݵ����룬�����ǳ�����
	DeclareFunc_0P_Noret(void, SetContourByPoints);
	
	// ������ͼ������������Ϊͼ��
	DeclareFunc_1P_Noret(void, SetRightAxisAsLegend, bool, as);
	// ������ͼ������������Ϊͼ��
	DeclareFunc_1P_Noret(void, SetTopAxisAsLegend, bool, as);

	// ����һ��ͼ�㣬���ڷֲ���ͼ������X����ͼ
	DeclareFunc_0P_Ret(int, AddLayer);
	// ������ͼ���������ڷ�����ͼ������mode��ʾ����ģʽ��nRows��ʾ������nCols��ʾ����
	// ����ģʽ�μӱ��ļ�ͷ����ע�͵���enum�����Խ��ע�ͣ�����ֱ��ʹ����Щenum�ˡ�
	DeclareFunc_3P_Noret(void, ResizePlots, int, mode, int, nRows, int, nCols);
	// ����ͼ�����������ڷֲ���ͼ������X����ͼ
	DeclareFunc_1P_Noret(void, ResizePlots, int, nLayers);

	// �����ͼ����
	DeclareFunc_0P_Ret(int, GetSubPlotCount);
	// ����ͼʱ��õ�ǰ��ͼ���
	DeclareFunc_0P_Ret(int, GetTop);

	// ���ö����ͼ��ͼ���Ƿ���ɫ��һ��
	DeclareFunc_1P_Noret(void, SetUniteLayerColor, bool, bUnite);
	// ���ö����ͼͼ����ɫ��һ��ʱ�Ƿ��������
	DeclareFunc_2P_Noret(void, SetExcludeDataWhenUniteColor, bool, bExclude, int, nPlotIndex);

	// ����������������ڶ����ͼΪ��ͼ��������
	DeclareFunc_1P_Ret(int, GetPlotDataCount, int, nPlotIndex);
	// ���õ��������Ƿ���Ӧ��������Ϣ
	DeclareFunc_3P_Noret(void, SetDataReact, bool, react, int, nDataIndex, int, nPlotIndex);
	// ǿ��ʹ��GDI+������Ⱦ
	DeclareFunc_3P_Noret(void, SetDataForceGdiPlus, bool, bForce, int, nDataIndex, int, nPlotIndex);
	// ����������ߵı���,nDataIndex��ʾ���ߵ����
	DeclareFunc_2P_Ret(const TCHAR*, GetDataTitle, int, nDataIndex, int, nPlotIndex);
	// �������ݱ���
	DeclareFunc_3P_Noret(void, SetDataTitle, const TCHAR*, title, int, nDataIndex, int, nPlotIndex);
	// ���������Զ�������ɫ
	DeclareFunc_2P_Noret(void, SetAutoDataColorGeneral, bool, bAuto, int, nPlotIndex);
	// �����Զ�������ɫ
	DeclareFunc_3P_Noret(void, SetAutoDataColor, bool, bAuto, int, nDataIndex, int, nPlotIndex);
	// ���������ɫ
	DeclareFunc_2P_Ret(COLORREF, GetDataColor, int, nDataIndex, int, nPlotIndex);
	// ����������ɫ
	DeclareFunc_3P_Noret(void, SetDataColor, COLORREF, color, int, nDataIndex, int, nPlotIndex);
	// �Ƿ�˫ɫ����ģʽ
	DeclareFunc_2P_Ret(bool, IsBiColorMode, int, nDataIndex, int, nPlotIndex);
	// ����˫ɫ����ģʽ
	DeclareFunc_3P_Noret(void, SetBiColorMode, bool, bBi, int, nDataIndex, int, nPlotIndex);
	// ���˫ɫ����ģʽ�µڶ�������ɫ
	DeclareFunc_2P_Ret(COLORREF, GetDataColor2, int, nDataIndex, int, nPlotIndex);
	// ����˫ɫ����ģʽ�µڶ�������ɫ
	DeclareFunc_3P_Noret(void, SetDataColor2, COLORREF, color, int, nDataIndex, int, nPlotIndex);
	// �Ƿ��ɫ����ģʽ��������˫ɫ����
	DeclareFunc_2P_Ret(bool, IsMultiColorMode, int, nDataIndex, int, nPlotIndex);
	// ���ö�ɫ����ģʽ
	DeclareFunc_3P_Noret(void, SetMultiColorMode, bool, bMul, int, nDataIndex, int, nPlotIndex);
	// ��ö�ɫ����ģʽ������ڵ�ķ�����0�����ݵ����0.0-1.0��1��X����ֵ��2��Y����ֵ
	DeclareFunc_2P_Ret(int, GetMultiColorInputType, int, nDataIndex, int, nPlotIndex);
	// ���ö�ɫ����ģʽ������ڵ�ķ���
	DeclareFunc_3P_Noret(void, SetMultiColorInputType, int, nType, int, nDataIndex, int, nPlotIndex);
	// ���ö�ɫ����ģʽ�Ľڵ㣬����ratio�ĵķ�Χ�����뷽������
	DeclareFunc_4P_Noret(void, AddSegmentColor, double, ratio, COLORREF, color, int, nDataIndex, int, nPlotIndex);
	// �Ƿ������ݵ㶨ɫ
	DeclareFunc_2P_Ret(bool, IsColorPtByPt, int, nDataIndex, int, nPlotIndex);
	// �Ƿ������ݵ㶨ɫ
	DeclareFunc_3P_Noret(void, SetColorPtByPt, bool, bBy, int, nDataIndex, int, nPlotIndex);
	// ������ݵ���ɫ��ע�⣬��ӵ���ɫ���ݵ��������ݵ������ȫһ������Ȼû������
	DeclareFunc_3P_Noret(void, AddDataPointColor, COLORREF, color, int, nDataIndex, int, nPlotIndex);
	// �������ݵ���ɫ
	DeclareFunc_4P_Noret(void, SetDataPointColor, COLORREF, color, int, nPointIndex, int, nDataIndex, int, nPlotIndex);
	// ɾ��һ�����ݵ���ɫ
	DeclareFunc_3P_Noret(void, EraseDataPointColor, int, nPointIndex, int, nDataIndex, int, nPlotIndex);
	// ����һ�����ݵ���ɫ
	DeclareFunc_4P_Noret(void, InsertDataPointColor, COLORREF, color, int, nPointIndex, int, nDataIndex, int, nPlotIndex);

	// �����������
	DeclareFunc_2P_Ret(int, GetDataLineStyle, int, nDataIndex, int, nPlotIndex);
	// ������������
	DeclareFunc_3P_Noret(void, SetDataLineStyle, int, nStyle, int, nDataIndex, int, nPlotIndex);
	// ��������߿�
	DeclareFunc_2P_Ret(int, GetDataLineSize, int, nDataIndex, int, nPlotIndex);
	// ���������߿�
	DeclareFunc_3P_Noret(void, SetDataLineSize, int, nSize, int, nDataIndex, int, nPlotIndex);
	// �������ߵĻ��Ʒ�ʽ
	DeclareFunc_3P_Noret(void, SetPlotType, int, nType, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ����Ƿ���ʾ
	DeclareFunc_3P_Noret(void, SetMarkerShow, bool, bShow, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ����Ƿ����
	DeclareFunc_3P_Noret(void, SetMarkerFill, bool, bFill, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ��ǵ���״
	DeclareFunc_3P_Noret(void, SetMarkerType, int, nType, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ�Ĵ�С
	DeclareFunc_3P_Noret(void, SetMarkerSize, int, nSize, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ���Ƶ�ʣ�0��ʾ����ǣ���n��ʾÿ��n������һ�Σ���n��ʾһ�����n����
	DeclareFunc_3P_Noret(void, SetMarkerFreq, int, nFreq, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ��ǵ���ʼ�㣬���ڲ���ȫ�����ʱ
	DeclareFunc_3P_Noret(void, SetMarkerStartIndex, int, nStartIndex, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ��һ������
	DeclareFunc_3P_Noret(void, SetMarkFirstPoint, bool, bMark, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ����һ������
	DeclareFunc_3P_Noret(void, SetMarkLastPoint, bool, bMark, int, nDataIndex, int, nPlotIndex);

	// ��������ɢ����ʾģʽ
	DeclareFunc_2P_Noret(void, SetScatter, int, nDataIndex, int, nPlotIndex);
	// �������������ɫ
	DeclareFunc_6P_Noret(void, SetDataFillColor, bool, bFill, COLORREF, color, int, nOpaque, int, nFillMode, int, nDataIndex, int, nPlotIndex);
	// �����������ݰ���ʾ
	DeclareFunc_5P_Noret(void, SetDataBarMode, bool, bShowBar, int, nBarBase, int, nBarSize, int, nDataIndex, int, nPlotIndex);
	// ����ѡ��͸���ģʽ
	DeclareFunc_3P_Noret(void, SetSLMode, int, nMode, int, nDataIndex, int, nPlotIndex);
	// ��ȡ���ݵı�־��������;
	DeclareFunc_2P_Ret(bool, GetDataFlag, int, nDataIndex, int, nPlotIndex);
	// �������ݵı�־��������;
	DeclareFunc_3P_Noret(void, SetDataFlag, bool, bFlag, int, nDataIndex, int, nPlotIndex);
	// ��ȡ���ݿɼ���־
	DeclareFunc_2P_Ret(bool, IsDataVisible, int, nDataIndex, int, nPlotIndex);
	//���������Ƿ�ɼ�
	DeclareFunc_3P_Noret(void, SetDataVisible, bool, bVis, int, nDataIndex, int, nPlotIndex);
	// ����ѹ����ʾģʽ����Ҫ���ڴ�������ʱ����ʾ
	// ѹ����ʾģʽ�У�����������ݵ�����Ļ����ʾʱ��X��ͬ����ѹ����һ�����ݵ㣬ֵȡƽ����
	DeclareFunc_3P_Noret(void, SetDataCompactDraw, bool, bCompact, int, nDataIndex, int, nPlotIndex);
	// �������ݰ�ֱ��ͼģʽ��ʾ
	DeclareFunc_3P_Noret(void, SetDataHistro, bool, bHistro, int, nDataIndex, int, nPlotIndex);

	// ��ø������ߵ���ţ�Ŀǰֻ����������ͼ
	DeclareFunc_0P_Ret(int, GetIndexOfLighted);
	
	// �����������ݵ���ֵ�Ƿ���ʾ
	DeclareFunc_3P_Noret(void, SetDataValueShow, bool, bShow, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ���ֵ��ʾ��ʽ��1ΪX��2ΪY��3ΪX:Y������Ϊ����ʾ
	DeclareFunc_3P_Noret(void, SetDataValueType, int, nType, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ���ֵ�������С
	DeclareFunc_3P_Noret(void, SetDataValueFontSize, int, nSize, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ���ֵ��ʾƵ�ʣ�0��ʾ����ʾ����n��ʾÿ��n������ʾһ�Σ���n��ʾһ����ʾn����
	DeclareFunc_3P_Noret(void, SetDataValueFreq, int, nFreq, int, nDataIndex, int, nPlotIndex);
	// �����������ݵ���ֵ��ʾ����ʼ�㣬���ڲ���ȫ����ʾʱ
	DeclareFunc_3P_Noret(void, SetDataValueStartIndex, int, nStartIndex, int, nDataIndex, int, nPlotIndex);

	// ���ʾ����ģʽ
	DeclareFunc_1P_Ret(bool, IsOscilloMode, int, nPlotIndex);
	// ����ʾ����ģʽ
	DeclareFunc_2P_Noret(void, SetOscilloMode, bool, bOscillo, int, nPlotIndex);
	// ����Ƿ�������ʾ����ģʽ
	DeclareFunc_1P_Ret(bool, IsStartOscilloScope, int, nPlotIndex);
	// �����Ƿ�������ʾ����ģʽ
	DeclareFunc_2P_Noret(void, SetStartOscilloScope, bool, bOscillo, int, nPlotIndex);
	// ����Ƿ�����϶�����
	DeclareFunc_0P_Ret(bool, IsEnableDataDrag);
	// �����Ƿ�����϶�����
	DeclareFunc_1P_Noret(void, SetEnableDataDrag, bool, enable);
	// ����Ƿ�����϶��������
	DeclareFunc_0P_Ret(bool, IsEnableZoneDrag);
	// �����Ƿ�����϶��������
	DeclareFunc_1P_Noret(void, SetEnableZoneDrag, bool, enable);
	// �ƶ�����
	DeclareFunc_4P_Ret(bool, MoveData, double, offsetx, double, offsety, int, nDataIndex, int, nPlotIndex);
	// ������ߵ�ˮƽƫ��
	DeclareFunc_2P_Ret(double, GetDataOffsetX, int, nDataIndex, int, nPlotIndex);
	// �������ߵ�ˮƽƫ��
	DeclareFunc_3P_Noret(void, SetDataOffsetX, double, offset, int, nDataIndex, int, nPlotIndex);
	// ������ߵ���ֱƫ��
	DeclareFunc_2P_Ret(double, GetDataOffsetY, int, nDataIndex, int, nPlotIndex);
	// �������ߵ���ֱƫ��
	DeclareFunc_3P_Noret(void, SetDataOffsetY, double, offset, int, nDataIndex, int, nPlotIndex);
	// ����X����λ����������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetXUnit, TCHAR *, unit, int, nDataIndex);
	// ����Y����λ����������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetYUnit, TCHAR *, unit, int, nDataIndex);
	// ��ͼ���н�����ʾY����ı����ߣ���������ģ��ʾ����
	DeclareFunc_1P_Noret(void, SetShowYSCaleOnly, bool, only);
	// �����󶨵����
	DeclareFunc_1P_Ret(bool, IsHoldCursorToMouse, int, nPlotIndex);
	// ���ù��󶨵����
	DeclareFunc_2P_Noret(void, SetHoldCursorToMouse, bool, hold, int, nPlotIndex);
	// �����󶨵�����
	DeclareFunc_1P_Ret(bool, IsHoldCursorToCurve, int, nPlotIndex);
	// ���ù��󶨵�����
	DeclareFunc_2P_Noret(void, SetHoldCursorToCurve, bool, hold, int, nPlotIndex);
	// ��ù��󶨵��������
	DeclareFunc_1P_Ret(int, GetIndexToHoldCursor, int, nPlotIndex);
	// ���ù��󶨵��������
	DeclareFunc_2P_Noret(void, SetIndexToHoldCursor, int, nCurveIndex, int, nPlotIndex);
	// �������ͷ����
	DeclareFunc_1P_Ret(bool, IsShowZeroArrow, int, nPlotIndex);
	// ��������ͷ����
	DeclareFunc_2P_Noret(void, SetShowZeroArrow, bool, show, int, nPlotIndex);
	// ��ⴥ����ͷ����
	DeclareFunc_1P_Ret(bool, IsShowTrigArrow, int, nPlotIndex);
	// ���ô�����ͷ����
	DeclareFunc_2P_Noret(void, SetShowTrigArrow, bool, show, int, nPlotIndex);
	// ��ʾˮƽ����ߣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetShowHCursor, bool, show, int, nPlotIndex);
	// ��ʾ��ֱ����ߣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetShowVCursor, bool, show, int, nPlotIndex);
	// ��ù������ɫ����������ģ��ʾ����
	DeclareFunc_1P_Ret(COLORREF, GetCursorColor, int, nPlotIndex);
	// ���ù������ɫ����������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetCursorColor, COLORREF, color, int, nPlotIndex);
	// ��ù�����߿���������ģ��ʾ����
	DeclareFunc_1P_Ret(int, GetCursorSize, int, nPlotIndex);
	// ���ù�����߿���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetCursorSize, int, size, int, nPlotIndex);
	// ��ù�������ͣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(int, GetCursorStyle, int, nPlotIndex);
	// ���ù�������ͣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetCursorStyle, int, style, int, nPlotIndex);
	// ���ˮƽ���ֵ����������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetCursorX, int, nPlotIndex);
	// ����ˮƽ���ֵ����������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetCursorX, double, cursor, int, nPlotIndex);
	// �����ֱ���ֵ����������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetCursorY, int, nPlotIndex);
	// ������ֱ���ֵ����������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetCursorY, double, cursor, int, nPlotIndex);
	// ��ʾˮƽѡ��������������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetShowHSel, bool, show, int, nPlotIndex);
	// ��ʾ��ֱѡ��������������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetShowVSel, bool, show, int, nPlotIndex);
	// ���ˮƽѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetHSelLower, int, nPlotIndex);
	// ����ˮƽѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetHSelLower, int, val, int, nPlotIndex);
	// ���ˮƽѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetHSelUpper, int, nPlotIndex);
	// ����ˮƽѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetHSelUpper, int, val, int, nPlotIndex);
	// �����ֱѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetVSelLower, int, nPlotIndex);
	// ������ֱѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetVSelLower, int, val, int, nPlotIndex);
	// �����ֱѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetVSelUpper, int, nPlotIndex);
	// ������ֱѡ���������ޣ���������ģ��ʾ����
	DeclareFunc_2P_Noret(void, SetVSelUpper, int, val, int, nPlotIndex);
	// ���ˮƽѡ�����Ŀ�ȣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetHSelWidth, int, nPlotIndex);
	// �����ֱѡ�����Ŀ�ȣ���������ģ��ʾ����
	DeclareFunc_1P_Ret(double, GetVSelWidth, int, nPlotIndex);


	// ���ͼ�����
	DeclareFunc_0P_Ret(TCHAR*, GetTitle);
	// �����ͼ���⣬�����ڷ�����ͼ���ֲ���ͼ������X����ͼ
	DeclareFunc_1P_Ret(TCHAR*, GetTitle, int, nPlotIndex);
	// ����ͼ�����
	DeclareFunc_1P_Noret(void, SetTitle, const TCHAR*, title);
	// ������ͼ����
	DeclareFunc_2P_Noret(void, SetTitle, const TCHAR*, title, int, nPlotIndex);
	// ���ͼ�����λ��
	DeclareFunc_0P_Ret(int, GetTitlePosition);
	// �����ͼ����λ��
	DeclareFunc_1P_Ret(int, GetTitlePosition, int, nPlotIndex);
	// ����ͼ�����λ�á���������0���У���������
	DeclareFunc_1P_Noret(void, SetTitlePosition, int, position);
	// ������ͼ����λ�á���������0���У���������
	DeclareFunc_2P_Noret(void, SetTitlePosition, int, position, int, nPlotIndex);
	// ͼ������Ƿ���ʾ
	DeclareFunc_0P_Ret(bool, IsTitleShow);
	// ��ͼ�����Ƿ���ʾ
	DeclareFunc_1P_Ret(bool, IsTitleShow, int, nPlotIndex);
	// ������ʾͼ�����
	DeclareFunc_1P_Noret(void, SetTitleShow, bool, show);
	// ������ʾ��ͼ����
	DeclareFunc_2P_Noret(void, SetTitleShow, bool, show, int, nPlotIndex);
	// ���ͼ�������ɫ
	DeclareFunc_0P_Ret(COLORREF, GetTitleColor);
	// �����ͼ������ɫ
	DeclareFunc_1P_Ret(COLORREF, GetTitleColor, int, nPlotIndex);
	// ����ͼ�������ɫ
	DeclareFunc_1P_Noret(void, SetTitleColor, COLORREF, color);
	// ������ͼ������ɫ
	DeclareFunc_2P_Noret(void, SetTitleColor, COLORREF, color, int, nPlotIndex);
	// ��ñ�������
	DeclareFunc_0P_Ret(LOGFONT, GetTitleFont);
	// �����ͼ��������
	DeclareFunc_1P_Ret(LOGFONT, GetTitleFont, int, nPlotIndex);
	// ���ñ�������
	DeclareFunc_1P_Noret(void, SetTitleFont, LOGFONT, logFont);
	// ������ͼ��������
	DeclareFunc_2P_Noret(void, SetTitleFont, LOGFONT, logFont, int, nPlotIndex);
	// �ֲ���ͼ��ʾ������
	DeclareFunc_1P_Noret(void, SetShowParentTitle, bool, set);

	// ���ͼ�񸱱���
	DeclareFunc_0P_Ret(TCHAR*, GetSubTitle); 
	// �����ͼ�����⣬�����ڷ�����ͼ���ֲ���ͼ������X����ͼ
	DeclareFunc_1P_Ret(TCHAR*, GetSubTitle, int, nPlotIndex);
	// ����ͼ�񸱱���
	DeclareFunc_1P_Noret(void, SetSubTitle, const TCHAR*, title);
	// ������ͼ������
	DeclareFunc_2P_Noret(void, SetSubTitle, const TCHAR*, title, int, nPlotIndex);
	// ���ͼ�񸱱���λ��
	DeclareFunc_0P_Ret(int, GetSubTitlePosition);
	// �����ͼ������λ��
	DeclareFunc_1P_Ret(int, GetSubTitlePosition, int, nPlotIndex);
	// ����ͼ�񸱱���λ�á���������0���У���������
	DeclareFunc_1P_Noret(void, SetSubTitlePosition, int, position);
	// ������ͼ������λ�á���������0���У���������
	DeclareFunc_2P_Noret(void, SetSubTitlePosition, int, position, int, nPlotIndex);
	// ͼ�񸱱����Ƿ���ʾ
	DeclareFunc_0P_Ret(bool, IsSubTitleShow);
	// ��ͼ�������Ƿ���ʾ
	DeclareFunc_1P_Ret(bool, IsSubTitleShow, int, nPlotIndex);
	// ������ʾͼ�񸱱���
	DeclareFunc_1P_Noret(void, SetSubTitleShow, bool, show);
	// ������ʾ��ͼ������
	DeclareFunc_2P_Noret(void, SetSubTitleShow, bool, show, int, nPlotIndex);
	// ���ͼ�񸱱�����ɫ
	DeclareFunc_0P_Ret(COLORREF, GetSubTitleColor);
	// �����ͼ��������ɫ
	DeclareFunc_1P_Ret(COLORREF, GetSubTitleColor, int, nPlotIndex);
	// ����ͼ�񸱱�����ɫ
	DeclareFunc_1P_Noret(void, SetSubTitleColor, COLORREF, color);
	// ������ͼ��������ɫ
	DeclareFunc_2P_Noret(void, SetSubTitleColor, COLORREF, color, int, nPlotIndex);
	// ��ø���������
	DeclareFunc_0P_Ret(LOGFONT, GetSubTitleFont);
	// �����ͼ����������
	DeclareFunc_1P_Ret(LOGFONT, GetSubTitleFont, int, nPlotIndex);
	// ���ø���������
	DeclareFunc_1P_Noret(void, SetSubTitleFont, LOGFONT, logFont);
	// ������ͼ����������
	DeclareFunc_2P_Noret(void, SetSubTitleFont, LOGFONT, logFont, int, nPlotIndex);
	
	// ���ø����߿���ɫ
	DeclareFunc_1P_Noret(void, SetLightColor, COLORREF, color);
	// ������ͼ�����߿���ɫ
	DeclareFunc_2P_Noret(void, SetLightColor, COLORREF, color, int, nPlotIndex);
	// ���ø����߿��߿�
	DeclareFunc_1P_Noret(void, SetLightLineSize, int, nSize);
	// ������ͼ�����߿��߿�
	DeclareFunc_2P_Noret(void, SetLightLineSize, int, nSize, int, nPlotIndex);
	// ���ø����߿�����
	DeclareFunc_1P_Noret(void, SetLightLineStyle, int, nStyle);
	// ������ͼ�����߿�����
	DeclareFunc_2P_Noret(void, SetLightLineStyle, int, nStyle, int, nPlotIndex);


	// ���������ᣬ�������λ��location�����ļ�ͷ����ע�͵���enum
	DeclareFunc_2P_Noret(void, AddAxis, int, location, int, nPlotIndex);
	// ������ͼ���������
	DeclareFunc_3P_Noret(void, SetAxisTitle, const TCHAR*, title, int, location, int, nPlotIndex);
	// �������������
	DeclareFunc_2P_Noret(void, SetAxisTitle, const TCHAR*, title, int, location);
	// ������ͼ�������������
	DeclareFunc_3P_Noret(void, SetAxisTitleFont, LOGFONT, logFont, int, location, int, nPlotIndex);
	// �����������������
	DeclareFunc_2P_Noret(void, SetAxisTitleFont, LOGFONT, logFont, int, location);
	// ������ͼ�������ǩ����
	DeclareFunc_3P_Noret(void, SetAxisLabelFont, LOGFONT, logFont, int, location, int, nPlotIndex);
	// �����������ǩ����
	DeclareFunc_2P_Noret(void, SetAxisLabelFont, LOGFONT, logFont, int, location);
	// ������ͼ���������λ��
	DeclareFunc_3P_Noret(void, SetAxisTitlePosition, int, position, int, location, int, nPlotIndex);
	// �������������λ��
	DeclareFunc_2P_Noret(void, SetAxisTitlePosition, int, position, int, location);
	// ������ͼ���������ͼ���ļ�϶
	DeclareFunc_3P_Noret(void, SetAxisGap, int, gap, int, location, int, nPlotIndex);
	// �������������ͼ���ļ�϶
	DeclareFunc_2P_Noret(void, SetAxisGap, int, gap, int, location);
	// ������ͼ������̶���λ��
	DeclareFunc_3P_Noret(void, SetAxisTickPosition, int, pos, int, location, int, nPlotIndex);
	// ����������̶���λ��
	DeclareFunc_2P_Noret(void, SetAxisTickPosition, int, pos, int, location);
	// ������ͼ�����ḱ�̶���λ��
	DeclareFunc_3P_Noret(void, SetAxisMinorTickPosition, int, pos, int, location, int, nPlotIndex);
	// ���������ḱ�̶���λ��
	DeclareFunc_2P_Noret(void, SetAxisMinorTickPosition, int, pos, int, location);
	// ������ͼʱ��������
	DeclareFunc_3P_Noret(void, SetAxisToTime, bool, bTime, int, location, int, nPlotIndex);
	// ����ʱ��������
	DeclareFunc_2P_Noret(void, SetAxisToTime, bool, bTime, int, location);
	// ������ͼʱ���������ʽ,"%Y%m%d%H%M%S",�ο�strftime������CTime��ĸ�ʽ������
	DeclareFunc_3P_Noret(void, SetAxisTimeFormat, TCHAR *, format, int, location, int, nPlotIndex);
	// ����ʱ���������ʽ,"%Y%m%d%H%M%S",�ο�strftime������CTime��ĸ�ʽ������
	DeclareFunc_2P_Noret(void, SetAxisTimeFormat, TCHAR *, format, int, location);
	// ������ͼ��ʾ��ɫ��
	DeclareFunc_3P_Noret(void, SetAxisColorBar, bool, bShow, int, location, int, nPlotIndex);
	// ������ʾ��ɫ��
	DeclareFunc_2P_Noret(void, SetAxisColorBar, bool, bShow, int, location);
	// ������ͼ��������ɫ
	DeclareFunc_3P_Noret(void, SetAxisColor, COLORREF, color, int, location, int, nPlotIndex);
	// ������������ɫ
	DeclareFunc_2P_Noret(void, SetAxisColor, COLORREF, color, int, location);
	// �����������Ƿ���ʾ�̶�ֵ
	DeclareFunc_2P_Noret(void, SetAxisLabelShow, bool, bShow, int, location);
	// ������ͼ�������Ƿ���ʾ�̶�ֵ
	DeclareFunc_3P_Noret(void, SetAxisLabelShow, bool, bShow, int, location, int, nPlotIndex);
	// ������ͼ����ֵ�ĸ�ʽ,�ο�printf�ĸ�ʽ������
	DeclareFunc_3P_Noret(void, SetAxisLabelFormat, TCHAR *, format, int, location, int, nPlotIndex);
	// ��������ֵ�ĸ�ʽ,�ο�printf�ĸ�ʽ������
	DeclareFunc_2P_Noret(void, SetAxisLabelFormat, TCHAR *, format, int, location);
	// ������ͼ�������Ƿ���ʾ
	DeclareFunc_3P_Noret(void, SetAxisShow, bool, bShow, int, location, int, nPlotIndex);
	// �����������Ƿ���ʾ
	DeclareFunc_2P_Noret(void, SetAxisShow, bool, bShow, int, location);
	// ��ͼ�������Ƿ���ʾ
	DeclareFunc_2P_Ret(bool, IsAxisShow, int, location, int, nPlotIndex);
	//�������Ƿ���ʾ
	DeclareFunc_1P_Ret(bool, IsAxisShow, int, location);
	// �������������ģʽ
	DeclareFunc_1P_Noret(void, SetAxesCompact, int, nPlotIndex);
	// �����ͼ�����������ǩ��һ�����ݷ�Χ
	DeclareFunc_4P_Noret(void, AddRange4LabelHiden, double, low, double, high, int, location, int, nPlotIndex);
	// ��������������ǩ��һ�����ݷ�Χ
	DeclareFunc_3P_Noret(void, AddRange4LabelHiden, double, low, double, high, int, location);
	// �����������ǩ�滻
	DeclareFunc_2P_Noret(void, SetReplaceLabel, bool, bReplace, int, location);
	// ������ͼ�������ǩ�滻
	DeclareFunc_3P_Noret(void, SetReplaceLabel, bool, bReplace, int, location, int, nPlotIndex);
	// ���һ���滻�ı�ǩ
	DeclareFunc_2P_Noret(void, AddAlternateLabel, TCHAR *, label, int, location);
	// ��ͼ���һ���滻�ı�ǩ
	DeclareFunc_3P_Noret(void, AddAlternateLabel, TCHAR *, label, int, location, int, nPlotIndex);
	// ����������滻��ǩ
	DeclareFunc_1P_Noret(void, ClrAlternateLabel, int, location);
	// �����ͼ�������滻��ǩ
	DeclareFunc_2P_Noret(void, ClrAlternateLabel, int, location, int, nPlotIndex);

	// ��ͼ�Զ�����X������̶���
	DeclareFunc_2P_Noret(void, SetXAutoTicks, bool, bAuto, int, nPlotIndex);
	// �Զ�����X������̶���
	DeclareFunc_1P_Noret(void, SetXAutoTicks, bool, bAuto);
	// ��ͼ�Զ�����Y������̶���
	DeclareFunc_2P_Noret(void, SetYAutoTicks, bool, bAuto, int, nPlotIndex);
	// �Զ�����Y������̶���
	DeclareFunc_1P_Noret(void, SetYAutoTicks, bool, bAuto);
	// ������ͼX������̶���
	DeclareFunc_2P_Noret(void, SetXTickCount, int, count, int, nPlotIndex);
	// ��������X��̶���
	DeclareFunc_1P_Noret(void, SetXTickCount, int, count);
	// ������ͼY������̶���
	DeclareFunc_2P_Noret(void, SetYTickCount, int, count, int, nPlotIndex);
	// ��������Y��̶���
	DeclareFunc_1P_Noret(void, SetYTickCount, int, count);
	// ��ͼ�Զ�����X�����ḱ�̶���
	DeclareFunc_2P_Noret(void, SetXAutoMinorTicks, bool, bAuto, int, nPlotIndex);
	// �Զ�����X�����ḱ�̶���
	DeclareFunc_1P_Noret(void, SetXAutoMinorTicks, bool, bAuto);
	// ��ͼ�Զ�����Y�����ḱ�̶���
	DeclareFunc_2P_Noret(void, SetYAutoMinorTicks, bool, bAuto, int, nPlotIndex);
	// �Զ�����Y�����ḱ�̶���
	DeclareFunc_1P_Noret(void, SetYAutoMinorTicks, bool, bAuto);
	// ������ͼX�����ḱ�̶���
	DeclareFunc_2P_Noret(void, SetXMinorTickCount, int, count, int, nPlotIndex);
	// ����X�����ḱ�̶���
	DeclareFunc_1P_Noret(void, SetXMinorTickCount, int, count);
	// ������ͼY�����ḱ�̶���
	DeclareFunc_2P_Noret(void, SetYMinorTickCount, int, count, int, nPlotIndex);
	// ����Y�����ḱ�̶���
	DeclareFunc_1P_Noret(void, SetYMinorTickCount, int, count);
	// ������ͼX�����ḡ�����
	DeclareFunc_2P_Noret(void, SetXFloatTicks, bool, flt, int, nPlotIndex);
	// ����X�����ḡ�����
	DeclareFunc_1P_Noret(void, SetXFloatTicks, bool, flt);
	// ������ͼY�����ḡ�����
	DeclareFunc_2P_Noret(void, SetYFloatTicks, bool, flt, int, nPlotIndex);
	// ����Y�����ḡ�����
	DeclareFunc_1P_Noret(void, SetYFloatTicks, bool, flt);
	// ������ͼX�������ָ��
	DeclareFunc_2P_Noret(void, SetXLogarithm, bool, bLog, int, nPlotIndex);
	// ����X�������ָ��
	DeclareFunc_1P_Noret(void, SetXLogarithm, bool, bLog);
	// ������ͼY�������ָ��
	DeclareFunc_2P_Noret(void, SetYLogarithm, bool, bLog, int, nPlotIndex);
	// ����Y�������ָ��
	DeclareFunc_1P_Noret(void, SetYLogarithm, bool, bLog);
	// ������ͼX���귴�����
	DeclareFunc_2P_Noret(void, SetXOpposite, bool, bOpposite, int, nPlotIndex);
	// ����X���귴�����
	DeclareFunc_1P_Noret(void, SetXOpposite, bool, bOpposite);
	// ������ͼY���귴�����
	DeclareFunc_2P_Noret(void, SetYOpposite, bool, bOpposite, int, nPlotIndex);
	// ����Y���귴�����
	DeclareFunc_1P_Noret(void, SetYOpposite, bool, bOpposite);
	// ������ͼ�������������ĳ���ı���
	DeclareFunc_2P_Noret(void, SetAtomX, double, atom, int, nPlotIndex);
	// �����������������ĳ���ı���
	DeclareFunc_1P_Noret(void, SetAtomX, double, atom);
	// ������ͼ��������������ĳ���ı���
	DeclareFunc_2P_Noret(void, SetAtomY, double, atom, int, nPlotIndex);
	// ������������������ĳ���ı���
	DeclareFunc_1P_Noret(void, SetAtomY, double, atom);
	// ������ͼ������������ĳ���ı���
	DeclareFunc_2P_Noret(void, SetAtomZeroX, double, zero, int, nPlotIndex);
	// ����������������ĳ���ı���
	DeclareFunc_1P_Noret(void, SetAtomZeroX, double, zero);
	// ������ͼ�������������ĳ���ı���
	DeclareFunc_2P_Noret(void, SetAtomZeroY, double, zero, int, nPlotIndex);
	// �����������������ĳ���ı���
	DeclareFunc_1P_Noret(void, SetAtomZeroY, double, zero);
	// ���������귶Χ��ĳ����������£���ͼ�������ָ����
	DeclareFunc_2P_Noret(void, SetSegsPreferX, int, segs, int, nPlotIndex);
	// ���������귶Χ��ĳ����������£��������ָ����
	DeclareFunc_1P_Noret(void, SetSegsPreferX, int, segs);
	// ���������귶Χ��ĳ����������£���ͼ��������ָ����
	DeclareFunc_2P_Noret(void, SetSegsPreferY, int, segs, int, nPlotIndex);
	// ���������귶Χ��ĳ����������£���������ָ����
	DeclareFunc_1P_Noret(void, SetSegsPreferY, int, segs);

	// ���������᷶Χ
	DeclareFunc_2P_Noret(void, SetPlotRange, double*, xRange, double*, yRange);
	DeclareFunc_4P_Noret(void, SetPlotRange, double, xl, double, xu, double, yl, double, yu);
	DeclareFunc_3P_Noret(void, SetXRange, double, low, double, high, int, nPlotIndex);
	DeclareFunc_2P_Noret(void, SetXRange, double, low, double, high);
	DeclareFunc_3P_Noret(void, SetYRange, double, low, double, high, int, nPlotIndex);
	DeclareFunc_2P_Noret(void, SetYRange, double, low, double, high);
	// ���������᷶Χ�Զ�����
	DeclareFunc_1P_Noret(void, SetXAutoRange, bool, bAuto);
	DeclareFunc_1P_Noret(void, SetYAutoRange, bool, bAuto);
	DeclareFunc_2P_Noret(void, SetXAutoRange, bool, bAuto, int, nPlotIndex);
	DeclareFunc_2P_Noret(void, SetYAutoRange, bool, bAuto, int, nPlotIndex);
	// ���������᷶Χ�����ݷ�Χ��ͬ������������չ
	DeclareFunc_2P_Noret(void, SetExactXRange, bool, bExact, int, nPlotIndex);
	DeclareFunc_2P_Noret(void, SetExactYRange, bool, bExact, int, nPlotIndex);
	// ���������᷶Χ���������ݷ�Χ��ͬ
	DeclareFunc_2P_Noret(void, SetOptimalExactXRange, bool, bExact, int, nPlotIndex);
	DeclareFunc_2P_Noret(void, SetOptimalExactYRange, bool, bExact, int, nPlotIndex);
	// ���������᷶Χ������������չһ��
	DeclareFunc_2P_Noret(void, SetOptimalXExtend, bool, bExtend, int, nPlotIndex);
	DeclareFunc_2P_Noret(void, SetOptimalYExtend, bool, bExtend, int, nPlotIndex);
	// ���������᷶Χһ�²���
	DeclareFunc_2P_Noret(void, SetStaticRange, bool, sr, int, nPlotIndex);
	// ����X�����᷶Χһ�²���
	DeclareFunc_2P_Noret(void, SetStaticXRange, bool, sr, int, nPlotIndex);
	// ����Y�����᷶Χһ�²���
	DeclareFunc_2P_Noret(void, SetStaticYRange, bool, sr, int, nPlotIndex);
	
	// ����������ȳ���ֻ��kTypeXY��Ч
	DeclareFunc_1P_Noret(void, SetEqualXYAxis, bool, bEqual);
	// ����������ȷ�Χ��ֻ��kTypeXY��Ч
	DeclareFunc_1P_Noret(void, SetEqualXYRange, bool, bEqual);
	// ����������ȱ����ߣ�ֻ��kTypeXY��Ч
	DeclareFunc_1P_Noret(void, SetEqualXYScale, bool, bEqual);

	// X���곬����Ļ�ĵ㲻������㣬���Ż��ٶ�
	DeclareFunc_3P_Noret(void, SetClipXData, bool, clip, int, nDataIndex, int, nPlotIndex);
	// Y���곬����Ļ�ĵ㲻������㣬���Ż��ٶ�
	DeclareFunc_3P_Noret(void, SetClipYData, bool, clip, int, nDataIndex, int, nPlotIndex);

	// ����Xʱ����󣬶Ի�����X���ݷ�Χ����ʱ��ȥһ����ֵ���Ա������ݹ���
	DeclareFunc_3P_Noret(void, SetXStartTime, TCHAR *, strTime, TCHAR *, fmt, int, nPlotIndex);
	// ����Yʱ����󣬶Ի�����Y���ݷ�Χ����ʱ��ȥһ����ֵ���Ա������ݹ���
	DeclareFunc_3P_Noret(void, SetYStartTime, TCHAR *, strTime, TCHAR *, fmt, int, nPlotIndex);

	// ���ñ�����������һ����ͼ�󶨣����ڹ���X����ͼ
	DeclareFunc_1P_Noret(void, SetGridBindLayer, int, nPlotIndex);
	// �����ͼ��������
	DeclareFunc_5P_Ret(bool, GetGridLine, bool &, MajorH, bool &, MajorV, bool &, MinorH, bool &, MinorV, int, nPlotIndex);
	// ���������
	DeclareFunc_4P_Ret(bool, GetGridLine, bool &, MajorH, bool &, MajorV, bool &, MinorH, bool &, MinorV);
	// ������ͼ�����ߵĻ���
	DeclareFunc_5P_Noret(void, SetGridLine, bool, MajorH, bool, MajorV, bool, MinorH, bool, MinorV, int, nPlotIndex);
	// ���������ߵĻ���
	DeclareFunc_4P_Noret(void, SetGridLine, bool, MajorH, bool, MajorV, bool, MinorH, bool, MinorV);
	// ��������������ɫ
	DeclareFunc_1P_Noret(void, SetMajorGridColor, COLORREF, color);
	// ������ͼ����������ɫ
	DeclareFunc_2P_Noret(void, SetMajorGridColor, COLORREF, color, int, nPlotIndex);
	// ���ø���������ɫ
	DeclareFunc_1P_Noret(void, SetMinorGridColor, COLORREF, color);
	// ������ͼ����������ɫ
	DeclareFunc_2P_Noret(void, SetMinorGridColor, COLORREF, color, int, nPlotIndex);
	// �������������߿�
	DeclareFunc_1P_Noret(void, SetMajorGridLineSize, int, nSize);
	// ������ͼ���������߿�
	DeclareFunc_2P_Noret(void, SetMajorGridLineSize, int, nSize, int, nPlotIndex);
	// ���ø��������߿�
	DeclareFunc_1P_Noret(void, SetMinorGridLineSize, int, nSize);
	// ������ͼ���������߿�
	DeclareFunc_2P_Noret(void, SetMinorGridLineSize, int, nSize, int, nPlotIndex);
	// ����������������
	DeclareFunc_1P_Noret(void, SetMajorGridLineStyle, int, nStyle);
	// ������ͼ������������
	DeclareFunc_2P_Noret(void, SetMajorGridLineStyle, int, nStyle, int, nPlotIndex);
	// ���ø�����������
	DeclareFunc_1P_Noret(void, SetMinorGridLineStyle, int, nStyle);
	// ������ͼ������������
	DeclareFunc_2P_Noret(void, SetMinorGridLineStyle, int, nStyle, int, nPlotIndex);
	// ��ȡ��ͼ������ɫ
	DeclareFunc_1P_Ret(COLORREF, GetBkgndColor, int, nPlotIndex);
	// ��ȡ������ɫ
	DeclareFunc_0P_Ret(COLORREF, GetBkgndColor);
	// ������ͼ������ɫ
	DeclareFunc_2P_Noret(void, SetBkgndColor, COLORREF, color, int, nPlotIndex);
	// ���ñ�����ɫ
	DeclareFunc_1P_Noret(void, SetBkgndColor, COLORREF, color);
	// ����ɫ�Ƿ񽥱�
	DeclareFunc_0P_Ret(bool, IsGradientBkgnd);
	// ����ɫ����
	DeclareFunc_1P_Noret(void, SetGradientBkgnd, bool, bGrad);
	
	// ����ͼ��߿���ʾ
	DeclareFunc_1P_Noret(void, SetEdgeShow, bool, bShow);
	// ����ͼ��߿���ʾ
	DeclareFunc_1P_Noret(void, SetEdgeRoundConor, bool, bRound);
	// ����ͼ��߿���ɫ
	DeclareFunc_1P_Noret(void, SetEdgeColor, COLORREF, color);
	// ����ͼ��߿���
	DeclareFunc_1P_Noret(void, SetEdgeWidth, int, width);

	// ����ͼ����հ�
	DeclareFunc_1P_Noret(void, SetMarginLeft, int, margin);
	// ���÷�����ͼ��հ�
	DeclareFunc_2P_Noret(void, SetMarginLeft, int, margin, int, nPlotIndex);
	// ����ͼ���ҿհ�
	DeclareFunc_1P_Noret(void, SetMarginRight, int, margin);
	// ���÷�����ͼ�ҿհ�
	DeclareFunc_2P_Noret(void, SetMarginRight, int, margin, int, nPlotIndex);
	// ����ͼ���Ͽհ�
	DeclareFunc_1P_Noret(void, SetMarginTop, int, margin);
	// ���÷�����ͼ�Ͽհ�
	DeclareFunc_2P_Noret(void, SetMarginTop, int, margin, int, nPlotIndex);
	// ����ͼ���¿հ�
	DeclareFunc_1P_Noret(void, SetMarginBottom, int, margin);
	// ���÷�����ͼ�¿հ�
	DeclareFunc_2P_Noret(void, SetMarginBottom, int, margin, int, nPlotIndex);
	
	// �����������߿���ʾ
	DeclareFunc_1P_Noret(void, SetBorderShow, bool, bShow);
	// �����������߿���ɫ
	DeclareFunc_1P_Noret(void, SetBorderColor, COLORREF, color);
	// �����������߿���
	DeclareFunc_1P_Noret(void, SetBorderWidth, int, width);
	
	// ����Ƿ�ʹ��ͼ��
	DeclareFunc_1P_Ret(bool, IsLegendShow, int, nPlotIndex);
	// �����Ƿ�ʹ��ͼ��
	DeclareFunc_2P_Noret(void, SetLegendShow, bool, bUse, int, nPlotIndex);
	// ����ͼ�����������
	DeclareFunc_2P_Noret(void, SetLegendMaxRows, int, nRows, int, nPlotIndex);
	// ����ÿ����ʾ��ͼ����
	DeclareFunc_2P_Noret(void, SetLegendMaxCols, int, nCols, int, nPlotIndex);
	// ����ͼ��λ��
	DeclareFunc_2P_Noret(void, SetLegendPosition, int, nPos, int, nPlotIndex);
	// ����ͼ������
	DeclareFunc_2P_Noret(void, SetLegendFont, LOGFONT, logFont, int, nPlotIndex);
	// ����ͼ������߶�
	DeclareFunc_2P_Noret(void, SetLegendFontHeight, int, nHeight, int, nPlotIndex);
	
	// �ڷֲ���ͼ�ͷ�����ͼ�У����õ�һ����ʾ�������ᣬ�ڶ�����ʾ��������
	DeclareFunc_0P_Noret(void, SetLRAxis);
	// �ڷֲ���ͼ�У����õ�һ����ʾ�������ᣬ�ڶ�����ʾ��������
	DeclareFunc_0P_Noret(void, SetTBAxis);
	// �ײ㺯������ȡ��ͼ�Ļ�ͼ����
	DeclareFunc_1P_Ret(RECT, GetLastClientRect, int, nPlotIndex);
	// �ײ㺯������ȡ��ͼ����
	DeclareFunc_0P_Ret(RECT, GetLastClientRect);
	// �ײ㺯������ȡ��ͼ�Ļ�ͼ����
	DeclareFunc_1P_Ret(RECT, GetLastPlotRect, int, nPlotIndex);
	// �ײ㺯������ȡ��ͼ����
	DeclareFunc_0P_Ret(RECT, GetLastPlotRect);
	// �ײ㺯������ȡ������ķ�Χ
	DeclareFunc_3P_Noret(void, GetLastPlotRange, double*, xRange, double*, yRange, int, nPlotIndex);
	// �ײ㺯������ȡ���ݵķ�Χ
	DeclareFunc_3P_Noret(void, GetLastDataRange, double*, xRange, double*, yRange, int, nPlotIndex);
	// �ײ㺯����������Ļ���������������
	DeclareFunc_3P_Noret(void, ClientToData, POINT *, point, double *, data, int, nPlotIndex);
	// �ײ㺯���������������������Ļ����
	DeclareFunc_3P_Noret(void, DataToClient, double *, data, POINT *, point, int, nPlotIndex);

	// ������ά���ߵ�����
	DeclareFunc_6P_Noret(void, SetGridLine, bool, MajorXY, bool, MinorXY, bool, MajorYZ, bool, MinorYZ, bool, MajorZX, bool, MinorZX);
	// ����XY��Ļ������
	DeclareFunc_1P_Noret(void, SetShowXYMajorGrid, bool, show );
	// ����YZ��Ļ������
	DeclareFunc_1P_Noret(void, SetShowYZMajorGrid, bool, show );
	// ����ZX��Ļ������
	DeclareFunc_1P_Noret(void, SetShowZXMajorGrid, bool, show );
	// ����XY��Ļ������
	DeclareFunc_1P_Noret(void, SetShowXYMinorGrid, bool, show );
	// ����YZ��Ļ������
	DeclareFunc_1P_Noret(void, SetShowYZMinorGrid, bool, show );
	// ����ZX��Ļ������
	DeclareFunc_1P_Noret(void, SetShowZXMinorGrid, bool, show );
	
	// ������ͼ���ݵ��ļ�
	DeclareFunc_2P_Ret(bool, WriteToFile, TCHAR *, pathName, int, nPlotIndex);
	// ������ͼĳ�������ݵ��ļ�
	DeclareFunc_3P_Ret(bool, WriteOneToFile, TCHAR *, pathName, int, nDataIndex, int, nPlotIndex);
	
		
	// ����ڲ�����ָ�룬��ָ��һ��û�ã�ֻ������ĳЩ����´��ݲ����������̺߳���
	DeclareFunc_0P_Ret(void	*, GetUserPointer);
	// �����ڲ�����ָ��
	DeclareFunc_1P_Noret(void, SetUserPointer, void *, pUser);
	// ����ڲ������־״̬��������;
	DeclareFunc_0P_Ret(bool, GetUserFlag);
	// �����ڲ������־״̬��������;
	DeclareFunc_1P_Noret(void, SetUserFlag, bool, flag);

}

