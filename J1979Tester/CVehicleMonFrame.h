#pragma once
#include "framework.h"
#include "Chart.h"
#include "CBusyBoxDlg.h"
#include "CVehicleComm.h"
#include "CVehicleDatabase.h"
#include <algorithm>
#include <time.h>

using namespace NsCChart;

typedef struct tagPARAM_XY
{
	double x;
	double y;

} PARAM_XY;

static const COLORREF g_crTableDef[] =
{
	RGB(255,0,0),
	RGB(0,0,255),
	RGB(64,128,128),
	RGB(0,255,0),
	RGB(128,255,255),
	RGB(128,128,0),
	RGB(255,255,128),
	RGB(0,64,0),
	RGB(0,255,64),
	RGB(0,64,128),
	RGB(0,128,64),
	RGB(255,128,192),
	RGB(0,128,192),
	RGB(0,0,64),
	RGB(128,0,128),
	RGB(255,0,128),
	RGB(255,128,128),
	RGB(192,192,192),
	RGB(128,64,0),
	RGB(128,128,255),
	RGB(128,128,192),
	RGB(255,128,255),
	RGB(128,64,64),
	RGB(255,255,0),
	RGB(128,255,128),
	RGB(128,128,128),
	RGB(0,0,128),
	RGB(0,0,160),
	RGB(128,0,64),
	RGB(255,0,255),
	RGB(0,128,0),
	RGB(0,128,128),
	RGB(0,255,255),
	RGB(0,128,255),
	RGB(64,0,0),
	RGB(255,128,0),
	RGB(128,0,0),
	RGB(255,128,64),
	RGB(128,255,0),
	RGB(0,255,128),
	RGB(128,128,64),
	RGB(0,64,64),
	RGB(64,0,64),
	RGB(128,0,255),
	RGB(64,0,128),
	RGB(192,0,192),
	RGB(10,10,10),
	RGB(245,245,245),
};

static int			g_nColorsInTable = sizeof(g_crTableDef) / sizeof(g_crTableDef[0]);
static COLORREF* g_pColorTable = (COLORREF*)g_crTableDef;

class CMonitorAllInOneGraphWnd : public CWindowWnd, public INotifyUI
{
public:
	CPaintManagerUI m_pm;
	CChartWnd m_chart;
	CDialogBuilder m_dlgBuilder;
	std::vector<std::vector<DD_PARAM_REC>> m_vectDataList;
	HANDLE m_hThreadMon;
	HANDLE m_hEventResume;
	HANDLE m_hEventQuit;
	std::vector<int> m_vectDataIndex;
	std::vector <std::list<PARAM_XY>> m_vectDataValues;
	HWND m_hwndGraph;

	CMonitorAllInOneGraphWnd()
	{
		m_hwndGraph = NULL;
		m_hThreadMon = NULL;
		m_hEventResume = ::CreateEvent(NULL, TRUE, TRUE, NULL);
		m_hEventQuit = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	~CMonitorAllInOneGraphWnd()
	{
		Stop();

		if (m_hEventResume)
		{
			::CloseHandle(m_hEventResume);
			m_hEventResume = NULL;
		}
		if (m_hEventQuit)
		{
			::CloseHandle(m_hEventQuit);
			m_hEventQuit = NULL;
		}
	}

	static DWORD WINAPI GraphViewMonitorThreadProc(LPVOID lpParameter)
	{
		CMonitorAllInOneGraphWnd* pWnd = (CMonitorAllInOneGraphWnd*)lpParameter;
		CListUI* pList = static_cast<CListUI*>(pWnd->m_pm.FindControl(_T("list_param_view")));
		std::vector<std::map<unsigned long, std::vector<unsigned char>>> values;
		DWORD dwWaitResult = 0;
		HANDLE hEvents[2];
		int nDataIndex = 0;
		clock_t ts = 0;
		clock_t tm = 0;
		std::vector<double> x_val;
		std::vector<double> y_val;
		double min_x = 0.0;

		hEvents[0] = pWnd->m_hEventResume;
		hEvents[1] = pWnd->m_hEventQuit;

		if (pList)
		{
			for (int i = 0; i < pList->GetCount(); i++)
			{
				CControlUI* pItem = static_cast<CListContainerElementUI*>(pList->GetItemAt(i));
				CLabelUI* pLab = static_cast<CLabelUI*>(pWnd->m_pm.FindSubControlByName(pItem, _T("param_value")));
				if (pLab != NULL)
				{
					CDuiString strValue;
					strValue.Format(_T("----"));
					pLab->SetText(strValue);
					pLab->Invalidate();
				}
			}
		}

		while (pList)
		{
			std::vector<CDuiString> texts;
			dwWaitResult = ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
			if (WAIT_OBJECT_0 != dwWaitResult)
			{
				break;
			}
			if (!theVehicleComm->ReadDataValues(pWnd->m_vectDataList, values))
			{
				break;
			}
			theVehicleDb->CalcDataValues(pWnd->m_vectDataList, values, texts);

			for (int i = 0; i < pList->GetCount(); i++)
			{
				CControlUI* pItem = static_cast<CListContainerElementUI*>(pList->GetItemAt(i));
				CLabelUI* pLab = static_cast<CLabelUI*>(pWnd->m_pm.FindSubControlByName(pItem, _T("param_value")));
				if (pLab != NULL)
				{
					pLab->SetText(texts[i]);
					pLab->Invalidate();
				}
			}

			tm = clock();
			nDataIndex = 0;
			for (auto & paramList : pWnd->m_vectDataList)
			{
				for (auto& param : paramList)
				{
					if ((1 == param.displayMode) || (2 == param.displayMode))
					{
						PARAM_XY xyVal;
						xyVal.x = (double)tm;
						xyVal.y = (param.curValue - param.minValue) * 100.0 / (param.maxValue - param.minValue);
						pWnd->m_vectDataValues[nDataIndex].push_back(xyVal);
						nDataIndex++;
					}
				}
			}
			
			ts = clock();
			nDataIndex = 0;
			for (auto & line : pWnd->m_vectDataValues)
			{
				x_val.clear();
				y_val.clear();
				for (auto& paramVal : line)
				{
					if ((paramVal.x - (double)ts >= -60000.0) && (paramVal.x - (double)ts <= 0.0))
					{
						x_val.push_back(paramVal.x - (double)ts);
						y_val.push_back(paramVal.y);
					}
				}
				if (x_val.size() > 0)
				{
					pWnd->m_chart.GetChart()->UpdateCurve(nDataIndex, &x_val[0], &y_val[0], x_val.size());
				}

				nDataIndex++;
			}
			pWnd->m_chart.ReDraw();

			for (auto& line : pWnd->m_vectDataValues)
			{
				while (line.size() > 0)
				{
					PARAM_XY& xy_val = line.front();
					if (xy_val.x - (double)ts < -60000.0)
					{
						line.pop_front();
					}
					else
					{
						break;
					}
				}
			}
		}

		pWnd->ClearList();

		return 0;
	}

	void Start(void)
	{
		if (NULL == m_hThreadMon)
		{
			::SetEvent(m_hEventResume);
			::ResetEvent(m_hEventQuit);
			m_hThreadMon = CreateThread(NULL, 0, &CMonitorAllInOneGraphWnd::GraphViewMonitorThreadProc, (LPVOID)this, 0, NULL);
		}
	}

	void Stop(void)
	{
		DUITRACE(_T("CMonitorAllInOneGraphWnd::Stop."));
		if (m_hThreadMon)
		{
			if (m_hEventResume)
			{
				::ResetEvent(m_hEventResume);
			}
			if (m_hEventQuit)
			{
				::SetEvent(m_hEventQuit);
			}
			::WaitForSingleObject(m_hThreadMon, INFINITE);
			::CloseHandle(m_hThreadMon);
			m_hThreadMon = NULL;
		}
	}

	void Pause(void)
	{
		::ResetEvent(m_hEventResume);
	}

	void Resume(void)
	{
		::SetEvent(m_hEventResume);
	}

	void ClearList(void)
	{
		m_vectDataIndex.clear();
		m_vectDataList.clear();
		m_vectDataValues.clear();

		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_param_view")));

		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();
		m_chart.GetChart()->ClrAllData();
	}

	int GetListItemCount(void)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_param_view")));

		if (NULL == pList)
		{
			return -1;
		}
		return pList->GetCount();
	}

	void AddOneCurve(CDuiString& module, CDuiString& name, COLORREF & color)
	{
		int nDataId = m_chart.GetChart()->AddCurve();
		int nDataIndex = m_chart.GetChart()->GetIndex(nDataId);
		m_vectDataIndex.push_back(nDataIndex);
		std::list<PARAM_XY> xyValues;
		xyValues.clear();
		m_vectDataValues.push_back(xyValues);
		color = g_pColorTable[nDataIndex % g_nColorsInTable];
	}
	void AddOneParam(CDuiString& module, CDuiString& name, CDuiString& mnem, CDuiString& unit, COLORREF & color)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_param_view")));

		if (NULL == pList)
		{
			return;
		}

		CListContainerElementUI* pListElement = NULL;
		if (!m_dlgBuilder.GetMarkup()->IsValid())
		{
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("one_list_param_without_mnem.xml"), NULL, NULL, &m_pm));
		}
		else
		{
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((IDialogBuilderCallback*)0, &m_pm));
		}
		if (pListElement == NULL)
			return;

		pListElement->SetBkColor(0xFF000000 | ((GetRValue(color) << 16) | (GetGValue(color) << 8) | (GetBValue(color) << 0) & 0xFFFFFF));

		CLabelUI* pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_module")));
		if (pLab != NULL)
		{
			pLab->SetText(module);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_name")));
		if (pLab != NULL)
		{
			pLab->SetText(name);
			pLab->SetToolTip(mnem);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_unit")));
		if (pLab != NULL)
		{
			pLab->SetText(unit);
		}

		pListElement->SetFixedHeight(25);
		if (!pList->Add(pListElement))
		{
			delete pListElement;
		}
	}

public:
	LPCTSTR GetWindowClassName() const { return _T("UIMonitorAllInOneGraph"); }
	virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
	void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void Notify(TNotifyUI& msg)
	{
		DUITRACE(_T("CMonitorAllInOneGraphWnd::Notify. msg.sType=%s"), msg.sType);
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("allinone_graph_view.xml"), NULL, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		m_hwndGraph = ::CreateWindow(_T("BUTTON"), _T("win32"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1, 0, 480, 425, m_hWnd, NULL, NULL, NULL);
		m_chart.Attach(m_hwndGraph, kTypeXY);

		m_chart.GetChart()->SetBkgndColor(RGB(95, 158, 160));
		m_chart.GetChart()->SetStaticXRange(true);
		m_chart.GetChart()->SetStaticYRange(true);
		m_chart.GetChart()->SetXRange(-60000.0, 0);
		m_chart.GetChart()->SetYRange(0, 100);
		m_chart.GetChart()->SetTitleShow(false);
		m_chart.GetChart()->SetOptimalYExtend(true);
		m_chart.GetChart()->SetLegendShow(false);
		m_chart.GetChart()->SetAxisTitle(CPaintManagerUI::GetMultiLanguageString(91), 1); //kLocationBottom
		m_chart.GetChart()->SetAxisTitle(CPaintManagerUI::GetMultiLanguageString(92), 0); //kLocationLeft
		
		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Stop();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Stop();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::IsIconic(*this)) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
				return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMaxPosition.x = rcWork.left;
		lpMMI->ptMaxPosition.y = rcWork.top;
		lpMMI->ptMaxSize.x = rcWork.right;
		lpMMI->ptMaxSize.y = rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if (wParam == SC_CLOSE) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT OnChangeLanguage(void)
	{
		m_pm.GetRoot()->NeedUpdate();
		return 0;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_ERASEBKGND:    lRes = 0; break;
		case WM_CHANGELANG:    lRes = OnChangeLanguage(); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

class CMonitorParamListViewWnd : public CWindowWnd, public INotifyUI
{
public:
	CMonitorParamListViewWnd() 
	{ 
		m_hThreadMon = NULL; 
		m_hEventResume = ::CreateEvent(NULL, TRUE, TRUE, NULL);
		m_hEventQuit = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_vectDataList.clear();
	}

	~CMonitorParamListViewWnd()
	{
		Stop();

		if (m_hEventResume)
		{
			::CloseHandle(m_hEventResume);
		}
		if (m_hEventQuit)
		{
			::CloseHandle(m_hEventQuit);
		}
	}

	static DWORD WINAPI ListViewMonitorThreadProc(LPVOID lpParameter)
	{
		CMonitorParamListViewWnd* pWnd = (CMonitorParamListViewWnd*)lpParameter;
		CListUI* pList = static_cast<CListUI*>(pWnd->m_pm.FindControl(_T("list_param_view")));
		std::vector<std::map<unsigned long, std::vector<unsigned char>>> values;
		DWORD dwWaitResult = 0;
		HANDLE hEvents[2];

		hEvents[0] = pWnd->m_hEventResume;
		hEvents[1] = pWnd->m_hEventQuit;

		if (pList)
		{
			for (int i = 0; i < pList->GetCount(); i++)
			{
				CControlUI* pItem = static_cast<CListContainerElementUI*>(pList->GetItemAt(i));
				CLabelUI* pLab = static_cast<CLabelUI*>(pWnd->m_pm.FindSubControlByName(pItem, _T("param_value")));
				if (pLab != NULL)
				{
					CDuiString strValue;
					strValue.Format(_T("----"));
					pLab->SetText(strValue);
					pLab->Invalidate();
				}
			}
		}

		while (pList)
		{
			std::vector<CDuiString> texts;

			dwWaitResult = ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
			if (WAIT_OBJECT_0 != dwWaitResult)
			{
				break;
			}
			if (!theVehicleComm->ReadDataValues(pWnd->m_vectDataList, values))
			{
				break;
			}
			theVehicleDb->CalcDataValues(pWnd->m_vectDataList, values, texts);

			for (int i = 0; i < pList->GetCount(); i++)
			{
				CControlUI* pItem = static_cast<CListContainerElementUI*>(pList->GetItemAt(i));
				CLabelUI* pLab = static_cast<CLabelUI*>(pWnd->m_pm.FindSubControlByName(pItem, _T("param_value")));
				if (pLab != NULL)
				{
					pLab->SetText(texts[i]);
					pLab->Invalidate();
				}
			}
		}

		if (pList)
		{
			for (int i = 0; i < pList->GetCount(); i++)
			{
				CControlUI* pItem = static_cast<CListContainerElementUI*>(pList->GetItemAt(i));
				CLabelUI* pLab = static_cast<CLabelUI*>(pWnd->m_pm.FindSubControlByName(pItem, _T("param_value")));
				if (pLab != NULL)
				{
					CDuiString strValue;
					strValue.Format(_T("----"));
					pLab->SetText(strValue);
					pLab->Invalidate();
				}
			}
		}

		return 0;
	}

	void Start(void)
	{
		if (NULL == m_hThreadMon)
		{
			::SetEvent(m_hEventResume);
			::ResetEvent(m_hEventQuit);
			m_hThreadMon = CreateThread(NULL, 0, &CMonitorParamListViewWnd::ListViewMonitorThreadProc, (LPVOID)this, 0, NULL);
		}
	}

	void Stop(void)
	{
		DUITRACE(_T("CMonitorParamListViewWnd::Stop."));
		if (m_hThreadMon)
		{
			if (m_hEventResume)
			{
				::ResetEvent(m_hEventResume);
			}
			if (m_hEventQuit)
			{
				::SetEvent(m_hEventQuit);
			}
			::WaitForSingleObject(m_hThreadMon, INFINITE);
			::CloseHandle(m_hThreadMon);
			m_hThreadMon = NULL;
		}
	}

	void Pause(void)
	{
		::ResetEvent(m_hEventResume);
	}

	void Resume(void)
	{
		::SetEvent(m_hEventResume);
	}

	void ClearList(void)
	{
		m_vectDataList.clear();
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_param_view")));

		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();
	}

	int GetListItemCount(void)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_param_view")));

		if (NULL == pList)
		{
			return -1;
		}
		return pList->GetCount();
	}

	void AddOneParam(CDuiString& module, CDuiString& name, CDuiString& mnem, CDuiString& unit)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_param_view")));

		if (NULL == pList)
		{
			return;
		}

		CListContainerElementUI* pListElement = NULL;
		if (!m_dlgBuilder.GetMarkup()->IsValid())
		{
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("one_list_param.xml"), NULL, NULL, &m_pm));
		}
		else
		{
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((IDialogBuilderCallback*)0, &m_pm));
		}
		if (pListElement == NULL)
			return ;

		CLabelUI* pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_module")));
		if (pLab != NULL)
		{
			pLab->SetText(module);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_name")));
		if (pLab != NULL)
		{
			pLab->SetText(name);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_mnem")));
		if (pLab != NULL)
		{
			pLab->SetText(mnem);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_unit")));
		if (pLab != NULL)
		{
			pLab->SetText(unit);
		}

		pListElement->SetFixedHeight(25);
		if (!pList->Add(pListElement))
		{
			delete pListElement;
		}
	}
public:
	CPaintManagerUI m_pm;
	CDialogBuilder m_dlgBuilder;
	std::vector<std::vector<DD_PARAM_REC>> m_vectDataList;
	HANDLE m_hThreadMon;
	HANDLE m_hEventResume;
	HANDLE m_hEventQuit;

public:
	LPCTSTR GetWindowClassName() const { return _T("UIMonitorParamListView"); }
	virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
	void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void Notify(TNotifyUI& msg)
	{
		DUITRACE(_T("CMonitorParamListViewWnd::Notify. msg.sType=%s, msg.pSender=0x%p"), msg.sType, msg.pSender);
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("param_list_view.xml"), NULL, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Stop();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Stop();
		::PostQuitMessage(0L);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::IsIconic(*this)) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
				return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMaxPosition.x = rcWork.left;
		lpMMI->ptMaxPosition.y = rcWork.top;
		lpMMI->ptMaxSize.x = rcWork.right;
		lpMMI->ptMaxSize.y = rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if (wParam == SC_CLOSE) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
	LRESULT OnChangeLanguage(void)
	{
		m_pm.GetRoot()->NeedUpdate();
		return 0;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (WM_PAINT != uMsg)
		{
			DUITRACE(_T("CMonitorParamListViewWnd::HandleMessage. uMsg=0x%x, wParam=0x%x, lParam=0x%x"), uMsg, wParam, lParam);
		}

		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_ERASEBKGND:    lRes = 0; break;
		case WM_CHANGELANG:    lRes = OnChangeLanguage(); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

class CMonitorSupportParamListWnd : public CWindowWnd, public INotifyUI
{
public:
	CMonitorSupportParamListWnd() {};

	void ClearList(void)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_support_param")));

		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();

		CHorizontalLayoutUI* pCheckHBox = static_cast<CHorizontalLayoutUI*>(m_pm.FindSubControlByName(pList, _T("check_list")));
		if (pCheckHBox)
		{
			CCheckBoxUI* pCheckList = static_cast<CCheckBoxUI*>(pCheckHBox->GetItemAt(0));
			pCheckList->Selected(false);
		}
		pCheckHBox = static_cast<CHorizontalLayoutUI*>(m_pm.FindSubControlByName(pList, _T("check_graph")));
		if (pCheckHBox)
		{
			CCheckBoxUI* pCheckList = static_cast<CCheckBoxUI*>(pCheckHBox->GetItemAt(0));
			pCheckList->Selected(false);
		}
	}

	int GetListItemCount(void)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_support_param")));

		if (NULL == pList)
		{
			return -1;
		}
		return pList->GetCount();
	}

	void AddOneParam(CDuiString& module, CDuiString & name, CDuiString& mnem, CDuiString & unit)
	{
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_support_param")));

		if (NULL == pList)
		{
			return;
		}

		CListContainerElementUI* pListElement = NULL;
		if (!m_dlgBuilder.GetMarkup()->IsValid())
		{
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("one_support_param.xml"), NULL, NULL, &m_pm));
		}
		else
		{
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((IDialogBuilderCallback*)0, &m_pm));
		}
		if (pListElement == NULL)
			return;

		CLabelUI* pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_module")));
		if (pLab != NULL)
		{
			pLab->SetText(module);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_name")));
		if (pLab != NULL)
		{
			pLab->SetText(name);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_mnem")));
		if (pLab != NULL)
		{
			pLab->SetText(mnem);
			pLab->SetAttribute(_T("tooltip"), mnem);
		}

		pLab = static_cast<CLabelUI*>(m_pm.FindSubControlByName(pListElement, _T("param_unit")));
		if (pLab != NULL)
		{
			pLab->SetText(unit);
		}

		pListElement->SetFixedHeight(25);
		if (!pList->Add(pListElement))
		{
			delete pListElement;
		}
		m_vectListSelected.push_back(false);
		m_vectGraphSelected.push_back(false);
	}
public:
	CPaintManagerUI m_pm;
	CDialogBuilder m_dlgBuilder;
	std::vector<bool> m_vectListSelected;
	std::vector<bool> m_vectGraphSelected;

public:
	LPCTSTR GetWindowClassName() const { return _T("UIMonitorSupportParamList"); }
	virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
	void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void Notify(TNotifyUI& msg)
	{
		//DUITRACE(_T("CMonitorSupportParamListWnd::Notify. msg.sType=%s, msg.pSender=0x%p"), msg.sType, msg.pSender);
		if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("list_support_param")));
			if (pList)
			{
				CHorizontalLayoutUI* pCheckHBox = static_cast<CHorizontalLayoutUI*>(m_pm.FindSubControlByName(pList, _T("check_list")));
				if (pCheckHBox)
				{
					if (msg.pSender == pCheckHBox->GetItemAt(0)) //list view checkbox
					{
						CCheckBoxUI* pCheckList = static_cast<CCheckBoxUI*>(msg.pSender);
						for (int i = 0; i < pList->GetCount(); i++)
						{
							CControlUI* pControl = pList->GetItemAt(i);
							CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(pControl);
							CCheckBoxUI* pCheck = static_cast<CCheckBoxUI*>(m_pm.FindSubControlByName(pItem, _T("select_list_single")));
							if (pCheck)
							{
								if (pCheckList->IsSelected())
								{
									//m_vectListSelected[i] = true;
									pCheck->Selected(true);
								}
								else
								{
									//m_vectListSelected[i] = false;
									pCheck->Selected(false);
								}
							}
						}
					}

					for (int i = 0; i < pList->GetCount(); i++)
					{
						CControlUI* pControl = pList->GetItemAt(i);
						CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(pControl);
						CCheckBoxUI* pCheck = static_cast<CCheckBoxUI*>(m_pm.FindSubControlByName(pItem, _T("select_list_single")));
						if (pCheck->IsSelected())
						{
							m_vectListSelected[i] = true;
						}
						else
						{
							m_vectListSelected[i] = false;
						}
					}
				}

				pCheckHBox = static_cast<CHorizontalLayoutUI*>(m_pm.FindSubControlByName(pList, _T("check_graph")));
				if (pCheckHBox)
				{
					if (msg.pSender == pCheckHBox->GetItemAt(0)) //list view checkbox
					{
						CCheckBoxUI* pCheckList = static_cast<CCheckBoxUI*>(msg.pSender);
						for (int i = 0; i < pList->GetCount(); i++)
						{
							CControlUI* pControl = pList->GetItemAt(i);
							CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(pControl);
							CCheckBoxUI* pCheck = static_cast<CCheckBoxUI*>(m_pm.FindSubControlByName(pItem, _T("select_graph_single")));
							if (pCheck)
							{
								if (pCheckList->IsSelected())
								{
									m_vectGraphSelected[i] = true;
									pCheck->Selected(true);
								}
								else
								{
									m_vectGraphSelected[i] = false;
									pCheck->Selected(false);
								}
							}
						}
					}
					for (int i = 0; i < pList->GetCount(); i++)
					{
						CControlUI* pControl = pList->GetItemAt(i);
						CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(pControl);
						CCheckBoxUI* pCheck = static_cast<CCheckBoxUI*>(m_pm.FindSubControlByName(pItem, _T("select_graph_single")));
						if (pCheck->IsSelected())
						{
							m_vectGraphSelected[i] = true;
						}
						else
						{
							m_vectGraphSelected[i] = false;
						}
					}
				}
			}
		}
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("support_param_list_view.xml"), NULL, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);
		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::IsIconic(*this)) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
				return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMaxPosition.x = rcWork.left;
		lpMMI->ptMaxPosition.y = rcWork.top;
		lpMMI->ptMaxSize.x = rcWork.right;
		lpMMI->ptMaxSize.y = rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if (wParam == SC_CLOSE) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT OnChangeLanguage(void)
	{
		m_pm.GetRoot()->NeedUpdate();
		return 0;
	}
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_ERASEBKGND:    lRes = 0; break;
		case WM_CHANGELANG:    lRes = OnChangeLanguage(); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

#define MON_FUNCTION_COUNT	(3)

class CVehicleMonFrame : public CWindowWnd, public INotifyUI
{
private:
	CWindowWnd* m_pSubWnds[MON_FUNCTION_COUNT];
	int m_nCurWndIndex;

public:
    CVehicleMonFrame();
public:
    CPaintManagerUI m_pm;
public:
    LPCTSTR GetWindowClassName() const { return _T("UIVehicleMonFrame"); }
    virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
    void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void Notify(TNotifyUI& msg)
	{
		int i;
		DUITRACE(_T("CVehicleMonFrame::Notify. msg.sType=%s"), msg.sType);
		if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			if (static_cast<COptionUI*>(m_pm.FindControl(_T("support_param_list"))) == msg.pSender)
			{
				m_nCurWndIndex = 0;
			}
			else if (static_cast<COptionUI*>(m_pm.FindControl(_T("param_list_view"))) == msg.pSender)
			{
				m_nCurWndIndex = 1;
			}
			else if (static_cast<COptionUI*>(m_pm.FindControl(_T("param_allinone_view"))) == msg.pSender)
			{
				m_nCurWndIndex = 2;
			}
			else
			{
				m_nCurWndIndex = -1;
			}

			if (m_nCurWndIndex < 0)
			{
				return; 
			}
			if (NULL == m_pSubWnds[m_nCurWndIndex])
			{
				switch (m_nCurWndIndex)
				{
				case 0:
					m_pSubWnds[0] = new CMonitorSupportParamListWnd();
					ASSERT(m_pSubWnds[0] && "Failed to create frame");
					m_pSubWnds[0]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 1L, 6, 26, 790, 425);
					break;
				case 1:
					m_pSubWnds[1] = new CMonitorParamListViewWnd();
					ASSERT(m_pSubWnds[1] && "Failed to create frame");
					m_pSubWnds[1]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 1L, 6, 26, 790, 425);
					break;
				case 2:
					m_pSubWnds[2] = new CMonitorAllInOneGraphWnd();
					ASSERT(m_pSubWnds[2] && "Failed to create frame");
					m_pSubWnds[2]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 1L, 6, 26, 790, 425);
					break;
				}
			}
			
			if (0 == m_nCurWndIndex)
			{
				if (0 == ((CMonitorSupportParamListWnd*)m_pSubWnds[0])->GetListItemCount())
				{
					for (i = 0; i < (int)theVehicleComm->GetEcuID().size(); i++)
					{
						std::map<unsigned long, std::vector<unsigned char>> mapValue;
						if (!theVehicleComm->GetCurrentDataValue((int)i, mapValue))
						{
							continue;
						}
						std::vector<DD_PARAM_REC> data_list;
						theVehicleDb->GetSupportParameters(mapValue, data_list);
						for (auto param : data_list)
						{
							CDuiString module;
							CDuiString name = param.name;
							CDuiString mnem = param.mnemonic;
							CDuiString unit = param.unit;

							module.Format(_T("ECU#%d"), i + 1);
							((CMonitorSupportParamListWnd*)m_pSubWnds[0])->AddOneParam(module, name, mnem, unit);
						}
					}
				}

				if (m_pSubWnds[1])
				{
					((CMonitorParamListViewWnd*)m_pSubWnds[1])->Stop();
				}
				if (m_pSubWnds[2])
				{
					((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->Stop();
				}
			}
			else if (1 == m_nCurWndIndex)
			{
				if (m_pSubWnds[2])
				{
					((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->Stop();
				}
				if (m_pSubWnds[0])
				{
					std::vector<DD_PARAM_REC> data_list;
					std::vector<bool>::iterator itSel = ((CMonitorSupportParamListWnd*)m_pSubWnds[0])->m_vectListSelected.begin();
					((CMonitorParamListViewWnd*)m_pSubWnds[1])->ClearList();

					for (i = 0; i < (int)theVehicleComm->GetEcuID().size(); i++)
					{
						std::map<unsigned long, std::vector<unsigned char>> mapValue;
						if (!theVehicleComm->GetCurrentDataValue((int)i, mapValue))
						{
							continue;
						}
						data_list.clear();
						((CMonitorParamListViewWnd*)m_pSubWnds[1])->m_vectDataList.push_back(data_list);

						theVehicleDb->GetSupportParameters(mapValue, data_list);

						for (auto param : data_list)
						{
							CDuiString module;
							CDuiString name = param.name;
							CDuiString mnem = param.mnemonic;
							CDuiString unit = param.unit;

							param.ecuIndex = i;
							if (itSel != ((CMonitorSupportParamListWnd*)m_pSubWnds[0])->m_vectListSelected.end())
							{
								if (*itSel)
								{
									module.Format(_T("ECU#%d"), i + 1);
									((CMonitorParamListViewWnd*)m_pSubWnds[1])->AddOneParam(module, name, mnem, unit);
									((CMonitorParamListViewWnd*)m_pSubWnds[1])->m_vectDataList[i].push_back(param);
								}
								itSel++;
							}
						}
					}

					if (((CMonitorParamListViewWnd*)m_pSubWnds[1])->GetListItemCount() > 0)
					{
						((CMonitorParamListViewWnd*)m_pSubWnds[1])->Start();
					}
				}
			}
			else if (2 == m_nCurWndIndex)
			{
				if (m_pSubWnds[1])
				{
					((CMonitorParamListViewWnd*)m_pSubWnds[1])->Stop();
				}

				if (m_pSubWnds[0])
				{
					std::vector<DD_PARAM_REC> data_list;
					std::vector<bool>::iterator itSel = ((CMonitorSupportParamListWnd*)m_pSubWnds[0])->m_vectGraphSelected.begin();
					((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->ClearList();

					for (i = 0; i < (int)theVehicleComm->GetEcuID().size(); i++)
					{
						std::map<unsigned long, std::vector<unsigned char>> mapValue;
						if (!theVehicleComm->GetCurrentDataValue((int)i, mapValue))
						{
							continue;
						}
						data_list.clear();
						((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->m_vectDataList.push_back(data_list);

						theVehicleDb->GetSupportParameters(mapValue, data_list);

						for (auto param : data_list)
						{
							CDuiString module;
							CDuiString name = param.name;
							CDuiString unit = param.unit;
							CDuiString mnem = param.mnemonic;
							COLORREF color;

							param.ecuIndex = i;
							if (itSel != ((CMonitorSupportParamListWnd*)m_pSubWnds[0])->m_vectGraphSelected.end())
							{
								if (*itSel)
								{
									module.Format(_T("ECU#%d"), i + 1);
									if ((1 == param.displayMode) || (2 == param.displayMode))
									{
										((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->AddOneCurve(module, name, color);
									}
									else
									{
										color = RGB(255, 255, 255); //white
									}
									((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->AddOneParam(module, name, mnem, unit, color);
									((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->m_vectDataList[i].push_back(param);
								}
								itSel++;
							}
						}
					}

					if (((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->GetListItemCount() > 0)
					{
						((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->Start();
					}
				}
			}

			for (i = 0; i < MON_FUNCTION_COUNT; i++)
			{
				if (NULL != m_pSubWnds[i])
				{
					if (i == m_nCurWndIndex)
					{
						::ShowWindow(*m_pSubWnds[i], SW_SHOW);
					}
					else
					{
						::ShowWindow(*m_pSubWnds[i], SW_HIDE);
					}
				}
			}
		}
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("vehicle_mon.xml"), NULL, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		m_pSubWnds[0] = new CMonitorSupportParamListWnd();
		ASSERT(m_pSubWnds[0] && "Failed to create frame");
		m_pSubWnds[0]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 1L, 6, 26, 790, 425);
		m_pSubWnds[0]->ShowWindow();

		for (size_t i = 0; i < theVehicleComm->GetEcuID().size(); i++)
		{
			std::map<unsigned long, std::vector<unsigned char>> mapValue;
			if (!theVehicleComm->GetCurrentDataValue((int)i, mapValue))
			{
				continue;
			}
			std::vector<DD_PARAM_REC> data_list;
			theVehicleDb->GetSupportParameters(mapValue, data_list);
			for (auto param : data_list)
			{
				CDuiString module;
				CDuiString name = param.name;
				CDuiString mnem = param.mnemonic;
				CDuiString unit = param.unit;

				module.Format(_T("ECU#%d"), i + 1);
				((CMonitorSupportParamListWnd*)m_pSubWnds[0])->AddOneParam(module, name, mnem, unit);
			}
		}

		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		for (int i = 0; i < MON_FUNCTION_COUNT; i++)
		{
			if (NULL != m_pSubWnds[i])
			{
				::CloseWindow(m_pSubWnds[i]->GetHWND());
			}
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		for (int i = 0; i < MON_FUNCTION_COUNT; i++)
		{
			if (NULL != m_pSubWnds[i])
			{
				m_pSubWnds[i] = NULL;
			}
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (::IsIconic(*this)) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
				return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMaxPosition.x = rcWork.left;
		lpMMI->ptMaxPosition.y = rcWork.top;
		lpMMI->ptMaxSize.x = rcWork.right;
		lpMMI->ptMaxSize.y = rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if (wParam == SC_CLOSE) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (0 == wParam)
		{
			if (m_pSubWnds[1])
			{
				((CMonitorParamListViewWnd*)m_pSubWnds[1])->Pause();
			}
			if (m_pSubWnds[2])
			{
				((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->Pause();
			}
		}
		else
		{
			if (m_pSubWnds[1])
			{
				((CMonitorParamListViewWnd*)m_pSubWnds[1])->Resume();
			}
			if (m_pSubWnds[2])
			{
				((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->Resume();
			}
		}

		if (!theVehicleComm->IsChannelConnected())
		{
			if (m_pSubWnds[0])
			{
				((CMonitorSupportParamListWnd*)m_pSubWnds[0])->ClearList();
			}
			if (m_pSubWnds[1])
			{
				((CMonitorParamListViewWnd*)m_pSubWnds[1])->ClearList();
			}
			if (m_pSubWnds[2])
			{
				((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->ClearList();
			}
		}
		else
		{
			if (0 == m_nCurWndIndex)
			{
				if (0 == ((CMonitorSupportParamListWnd*)m_pSubWnds[0])->GetListItemCount())
				{
					for (int i = 0; i < (int)theVehicleComm->GetEcuID().size(); i++)
					{
						std::map<unsigned long, std::vector<unsigned char>> mapValue;
						if (!theVehicleComm->GetCurrentDataValue((int)i, mapValue))
						{
							continue;
						}
						std::vector<DD_PARAM_REC> data_list;
						theVehicleDb->GetSupportParameters(mapValue, data_list);
						for (auto param : data_list)
						{
							CDuiString module;
							CDuiString name = param.name;
							CDuiString mnem = param.mnemonic;
							CDuiString unit = param.unit;

							module.Format(_T("ECU#%d"), i + 1);
							((CMonitorSupportParamListWnd*)m_pSubWnds[0])->AddOneParam(module, name, mnem, unit);
						}
					}
				}

				if (m_pSubWnds[1])
				{
					((CMonitorParamListViewWnd*)m_pSubWnds[1])->Stop();
				}
				if (m_pSubWnds[2])
				{
					((CMonitorAllInOneGraphWnd*)m_pSubWnds[2])->Stop();
				}
			}
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnChangeLanguage(void)
	{
		m_pm.GetRoot()->NeedUpdate();
		for (int i = 0; i < 3; i++)
		{
			if (NULL != m_pSubWnds[i])
			{
				m_pSubWnds[i]->PostMessage(WM_CHANGELANG, 0, 0);
			}
		}
		return 0;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//if (WM_PAINT != uMsg)
		//{
		//	DUITRACE(_T("CVehicleMonFrame::HandleMessage. uMsg=0x%x, wParam=0x%x, lParam=0x%x"), uMsg, wParam, lParam);
		//}

		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_SHOWWINDOW:    lRes = OnShowWindow(uMsg, wParam, lParam, bHandled); break;
		case WM_CHANGELANG:    lRes = OnChangeLanguage(); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

