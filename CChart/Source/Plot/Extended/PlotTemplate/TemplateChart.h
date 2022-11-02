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
#include <time.h>

namespace NsCChart{
	
//���ڶ�̬���ӿ��ʱ��ע�͵�#else�еĵڶ���
//���ھ�̬�����Դ��ʱ��ע�͵�#else�еĵ�һ��
#ifdef CKLINECHART_EXPORTS
#define CKLINECHART_API __declspec(dllexport)
#else
//#define CKLINECHART_API __declspec(dllimport)
#define CKLINECHART_API
#endif

class	CKLINECHART_API	CTemplateChart
{
public:	
	// CTemplateChart�ڲ����ݽṹ
	struct stTemplateChartPara;
	// ����ڲ����ݵ�ָ��
	stTemplateChartPara	*GetPara();
protected:
	// �ڲ�����
	stTemplateChartPara	*m_pPara;
	// �ͷ��ڲ���Դ
	void		Release();	
	
public:
	CTemplateChart();
	~CTemplateChart();


	// ��ñ�ʶ�����ܹ�����
	int			GetPlotID();
	// ���ñ�ʶ�����ܹ�����
	void		SetPlotID(int nID);
public:
	// ���Ԥ�ô���
	HWND		GetWnd();
	// ����Ԥ�ô���
	void		SetWnd(HWND hWnd);
	// �����Ƿ����ƻ�ͼ��
	bool		IsRectConfined();
	// �����Ƿ����ƻ�ͼ��
	void		SetRectConfined(bool confine);
	// ��ȡ��������
	RECT		GetConfineRect(); 
	// ������������
	void		SetConfineRect(RECT ConfineRect);
	
	// �����ڲ��Ƿ�������
	bool		IsEmpty();

public:
	// ��SetWnd()���õĴ��ھ����ͼ
	void		OnDraw();
	// �Դ��ھ����ͼ
	void		OnDraw(HWND hWnd);
	// ���豸�����Ļ�ͼ
	void		OnDraw(HDC hDC);
	// �ڴ�����ѡ�������ͼ
	void		OnDraw(HWND hWnd, RECT destRect);
	// ���豸�����Ķ�Ӧ�Ĵ����ϣ�ѡ�������ͼ
	void		OnDraw(HDC hDC, RECT destRect);

public:
	// �����Ƿ���Ӧ��������Ϣ
	bool		IsReact();
	// �����Ƿ���Ӧ��������Ϣ
	void		SetReact(bool react);
	// ��������������Ϣ�������ػ棬���û����ݷ���ֵ�ػ�
	bool		OnLButtonDown( HWND hWnd, POINT point, UINT ctrlKey );
	bool		OnLButtonUp( HWND hWnd, POINT point, UINT ctrlKey );
	bool		OnLButtonDblClk( HWND hWnd, POINT point, UINT ctrlKey );
	bool		OnMouseMove( HWND hWnd, POINT point, UINT ctrlKey );
	bool		OnMouseLeave( HWND hWnd, POINT point, UINT ctrlKey );
	bool		OnContextMenu( HMENU hMenu, HWND hWnd, POINT point );
	bool		OnKeyDown( HWND hWnd, UINT key );
	// ��Ӧ��Ϣ�������ػ棬���û����ݷ���ֵ�ػ�
	bool		OnEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	// ��Ӧ��Ϣ����AutoRedrawΪ��������ػ�
	void		Interactive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:

public:
	// ���ñ���
	void		SetTitle(TCHAR* title);
	// ���ñ����Ƿ���ʾ
	void		SetTitleShow(bool show);
	// ���ñ�����ɫ
	void		SetTitleColor(COLORREF color);
	// ���ñ�������
	void		SetTitleFont(LOGFONT logFont);
	// ���ñ�������߶�
	void		SetTitleHeight(int height);
	// ���ñ�����������
	void		SetTitleFaceName(TCHAR* facename);

public:
	// ��ñ�����ɫ
	COLORREF	GetBkgndColor();
	// ���ñ�����ɫ
	void		SetBkgndColor(COLORREF color);
	// �����Ƿ���Ʊ�Ե
	void		SetEdgeShow(bool show);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Declaration of class CTemplateChartWnd

// �����࣬��װ����Ϣ������
class	CKLINECHART_API	CTemplateChartWnd
{
public:
	// CTemplateChartWnd�ڲ����õ����ݽṹ
	struct	stTemplateChartWndPara;
protected:
	// �ڲ�����
	stTemplateChartWndPara *m_pPara;
	// �ͷ��ڲ���Դ
	void	Release();
	
public:
	CTemplateChartWnd();
	~CTemplateChartWnd();
	
public:
	// ���ݱ�ʶ������
	int		GetChartIndex(int nChartID);
	// ������Ż�ñ�ʶ
	int		GetChartID(int nChartIndex);
	// ��ȡCTemplateChart���ָ�룬�Ե����亯��
	CTemplateChart	*GetChart(int nChartIndex=0);
	// ����CChart�������Ҫ�������໯������ֵΪԭCChart��ָ��
	CTemplateChart	*SetChart(CTemplateChart *pChart, int nChartIndex=0);
	// �����Attach�Ĵ���
	int		GetChartCount();
	
	// ճ���ڣ��������ʽ
	int		Attach(HWND hWnd);
	// ճ���ڣ�ͬʱ���û�ͼ���ͣ������ƻ�ͼ��
	int		Attach(HWND hWnd, RECT rtClient);
	// ճ���ڣ�ճ���Ի�����, �����ؼ����
	int		AttachDlg(HWND hDlg, HWND hCtrl);
	// ճ���ڣ�ճ���Ի����ϣ������ؼ�ID
	int		AttachDlg(HWND hDlg, UINT nCtrlID);
	// ж����, ���к�����nChartIndex�������ճɹ�Attach��˳������Ϊ0, 1, 2, ...
	bool	Detach();
	bool	Detach(HWND hWnd);
	
	// �ػ�
	void	ReDraw();
	void	ReDraw(HWND hWnd);
	void	ReDraw(int nChartIndex);
	void	ReDraw(CTemplateChart *chart);		
};


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// namespace ending
}