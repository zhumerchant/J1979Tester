#pragma once
#include "framework.h"

#include "CVehicleInfoFrame.h"
#include "CPINToolFrame.h"
#include "CVehicleDTCFrame.h"
#include "CVehicleDiagFrame.h"
#include "CVehicleMonFrame.h"
#include "CVehicleSpecFuncFrame.h"
#include "UIMenu.h"

#define MAIN_FUNCTION_COUNT	(4)

class CTesterMainFrame : public CWindowWnd, public INotifyUI
{

private:
    CButtonUI* m_pCloseBtn;
    CButtonUI* m_pMinBtn;

	CWindowWnd* m_pSubWnds[MAIN_FUNCTION_COUNT];
	int m_nCurWndIndex;

public:
    CTesterMainFrame();
public:
    CPaintManagerUI m_pm;
public:
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); }
    virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
    void OnFinalMessage(HWND /*hWnd*/) { delete this; }

    void Init() {
        m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
        m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));

		m_pSubWnds[0] = new CVehicleInfoFrame();
		ASSERT(m_pSubWnds[0] && "Failed to create frame");
		m_pSubWnds[0]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0L, 1, 96, 795, 470);
		::ShowWindow(*m_pSubWnds[0], SW_SHOW);
	}

    void OnPrepare() {}

	void Notify(TNotifyUI& msg)
	{
		int i = 0;

		DUITRACE(_T("CTesterMainFrame::Notify. msg.sType=%s"), msg.sType);

		if (msg.sType == DUI_MSGTYPE_WINDOWINIT) OnPrepare();
		else if (msg.sType == DUI_MSGTYPE_CLICK) {
			if (msg.pSender == m_pCloseBtn) {
				for (i = 0; i < MAIN_FUNCTION_COUNT; i++)
				{
					if (m_pSubWnds[i])
					{
						m_pSubWnds[i]->Close();
					}
				}
				Close();
				::PostQuitMessage(0);
				return;
			}
			else if (msg.pSender == m_pMinBtn) {
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
			}
			else if (!_tcsicmp(msg.pSender->GetName(), _T("menubtn")))
			{
				CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
				CDuiPoint point = msg.ptMouse;
				ClientToScreen(m_hWnd, &point);
				STRINGorID xml(_T("menu_setting.xml"));
				pMenu->Init(NULL, xml, _T("xml"), point);
			}
		}
		else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			if (static_cast<COptionUI*>(m_pm.FindControl(_T("vehicle_info"))) == msg.pSender)
			{
				m_nCurWndIndex = 0;
			}
			else if (static_cast<COptionUI*>(m_pm.FindControl(_T("vehicle_dtc"))) == msg.pSender)
			{
				m_nCurWndIndex = 1;
			}
			else if (static_cast<COptionUI*>(m_pm.FindControl(_T("vehicle_mon"))) == msg.pSender)
			{
				m_nCurWndIndex = 2;
			}
			else if (static_cast<COptionUI*>(m_pm.FindControl(_T("pin_tool"))) == msg.pSender)
			{
				m_nCurWndIndex = 3;
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
					m_pSubWnds[0] = new CVehicleInfoFrame();
					ASSERT(m_pSubWnds[0] && "Failed to create frame");
					m_pSubWnds[0]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0L, 1, 96, 795, 472);
					break;
				case 1:
					m_pSubWnds[1] = new CVehicleDTCFrame();
					ASSERT(m_pSubWnds[1] && "Failed to create frame");
					m_pSubWnds[1]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0L, 1, 96, 795, 472);
					break;
				case 2:
					m_pSubWnds[2] = new CVehicleMonFrame();
					ASSERT(m_pSubWnds[2] && "Failed to create frame");
					m_pSubWnds[2]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0L, 1, 96, 795, 472);
					break;
				case 3:
					m_pSubWnds[3] = new CPINToolFrame();
					ASSERT(m_pSubWnds[3] && "Failed to create frame");
					m_pSubWnds[3]->Create(m_hWnd, NULL, UI_WNDSTYLE_CHILD, 0L, 1, 96, 795, 472);
					break;
				}
			}

			for (i = 0; i < MAIN_FUNCTION_COUNT; i++)
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
        CControlUI* pRoot = builder.Create(_T("skin.xml"), NULL, NULL, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);

        Init();
        return 0;
    }

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
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
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT OnChangeLanguage(void)
	{
		m_pm.GetRoot()->NeedUpdate();
		for (int i = 0; i < MAIN_FUNCTION_COUNT; i++)
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
		//DUITRACE(_T("CTesterMainFrame::HandleMessage=0x%x"), uMsg);
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
		case WM_CHANGELANG:    lRes = OnChangeLanguage(); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

