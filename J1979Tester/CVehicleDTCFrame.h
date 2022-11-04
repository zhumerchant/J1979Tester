#pragma once
#include "UIlib.h"
#include "CVehicleComm.h"
#include "CVehicleDatabase.h"
#include <algorithm>
#include "CYesNoMBDlg.h"

using namespace DuiLib;


class CVehicleCurrentDTCUI : public CContainerUI
{
public:
	CVehicleCurrentDTCUI()
	{
		CDialogBuilder builder;
		CContainerUI* pBasicInfoUI = static_cast<CContainerUI*>(builder.Create(_T("current_dtc.xml"), NULL));
		if (pBasicInfoUI) {
			this->Add(pBasicInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CVehiclePendingDTCUI : public CContainerUI
{
public:
	CVehiclePendingDTCUI()
	{
		CDialogBuilder builder;
		CContainerUI* pBasicInfoUI = static_cast<CContainerUI*>(builder.Create(_T("pending_dtc.xml"), NULL));
		if (pBasicInfoUI) {
			this->Add(pBasicInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CVehiclePermanentDTCUI : public CContainerUI
{
public:
	CVehiclePermanentDTCUI()
	{
		CDialogBuilder builder;
		CContainerUI* pBasicInfoUI = static_cast<CContainerUI*>(builder.Create(_T("current_dtc.xml"), NULL));
		if (pBasicInfoUI) {
			this->Add(pBasicInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CVehicleFFDUI : public CContainerUI
{
public:
	CVehicleFFDUI()
	{
		CDialogBuilder builder;
		CContainerUI* pBasicInfoUI = static_cast<CContainerUI*>(builder.Create(_T("current_dtc.xml"), NULL));
		if (pBasicInfoUI) {
			this->Add(pBasicInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CDialogBuilderCallbackEx_DTC : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if (_tcscmp(pstrClass, _T("current_dtc")) == 0) return new CVehicleCurrentDTCUI;
		if (_tcscmp(pstrClass, _T("pending_dtc")) == 0) return new CVehiclePendingDTCUI;
		if (_tcscmp(pstrClass, _T("permanent_dtc")) == 0) return new CVehiclePermanentDTCUI;
		if (_tcscmp(pstrClass, _T("freeze_frame_data")) == 0) return new CVehicleFFDUI;
		return NULL;
	}
};

class CVehicleDTCFrame : public CWindowWnd, public INotifyUI
{
public:
    CVehicleDTCFrame();
public:
    CPaintManagerUI m_pm;
public:
	void DisplayCurrentDTC(void)
	{
		CDuiString name;
		CDuiString content;
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		CListUI* pList = static_cast<CListUI*>(m_pm.FindSubControlByName(pControl, _T("list_vehicle_current_dtc")));
		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();

		size_t i = 0;
		size_t m = 0;
		CLabelUI* pLab = NULL;

		for (i = 0; i < theVehicleComm->GetEcuID().size(); i++)
		{
			std::vector<unsigned long> dtcs;
			if (!theVehicleComm->GetConfirmedDtc((int)i, dtcs))
			{
				continue;
			}
			std::sort(dtcs.begin(), dtcs.end());

			for (m = 0; m < dtcs.size(); m++)
			{
				int line_heigth = 25;
				CDialogBuilder builder;
				CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("one_dtc.xml")));
				if (NULL == pListElement)
				{
					continue;
				}
				DTC_REC dtc;
				if (!theVehicleDb->GetDTC(dtcs[m], dtc))
				{
					continue;
				}
				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_module")));
				if (pLab != NULL)
				{
					name.Format(_T("ECU#%u"), i + 1);
					pLab->SetText(name);
				}

				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_number")));
				if (pLab != NULL)
				{
					pLab->SetText(dtc.number);
				}
				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_naming")));
				if (pLab != NULL)
				{
					pLab->SetToolTip(dtc.naming);
					//pLab->SetMultiLine(true);
					pLab->SetText(dtc.naming);
					if (dtc.naming.GetLength() > 0)
					{
						//line_heigth = (dtc.naming.GetLength() + 79) / 80 * 25;
					}
				}
				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_status")));
				if (pLab != NULL)
				{
					pLab->SetText(CPaintManagerUI::GetMultiLanguageString(31));
				}

				pListElement->SetFixedHeight(line_heigth);
				if (!pList->Add(pListElement))
				{
					delete pListElement;
				}
			}
		}
	}
	void DisplayPendingDTC(void)
	{
		CDuiString name;
		CDuiString content;
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		CListUI* pList = static_cast<CListUI*>(m_pm.FindSubControlByName(pControl, _T("list_vehicle_pending_dtc")));
		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();

		size_t i = 0;
		size_t m = 0;
		CLabelUI* pLab = NULL;

		for (i = 0; i < theVehicleComm->GetEcuID().size(); i++)
		{
			std::vector<unsigned long> dtcs;
			if (!theVehicleComm->GetPendingDtc((int)i, dtcs))
			{
				continue;
			}
			std::sort(dtcs.begin(), dtcs.end());

			for (m = 0; m < dtcs.size(); m++)
			{
				int line_heigth = 25;
				CDialogBuilder builder;
				CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("one_dtc.xml")));
				if (NULL == pListElement)
				{
					continue;
				}
				DTC_REC dtc;
				if (!theVehicleDb->GetDTC(dtcs[m], dtc))
				{
					continue;
				}
				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_module")));
				if (pLab != NULL)
				{
					name.Format(_T("ECU#%u"), i + 1);
					pLab->SetText(name);
				}

				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_number")));
				if (pLab != NULL)
				{
					pLab->SetText(dtc.number);
				}
				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_naming")));
				if (pLab != NULL)
				{
					pLab->SetToolTip(dtc.naming);
					//pLab->SetMultiLine(true);
					pLab->SetText(dtc.naming);
					if (dtc.naming.GetLength() > 0)
					{
						//line_heigth = (dtc.naming.GetLength() + 79) / 80 * 25;
					}
				}
				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("dtc_status")));
				if (pLab != NULL)
				{
					pLab->SetText(CPaintManagerUI::GetMultiLanguageString(32));
				}

				pListElement->SetFixedHeight(line_heigth);
				if (!pList->Add(pListElement))
				{
					delete pListElement;
				}
			}
		}
	}

    LPCTSTR GetWindowClassName() const { return _T("UIVehicleDTCFrame"); }
    virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
    void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void Notify(TNotifyUI& msg)
	{
		//DUITRACE(_T("CVehicleDTCFrame::Notify. msg.sType=%s"), msg.sType);
		if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CDuiString name = msg.pSender->GetName();
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if (name == _T("current_dtc"))
			{
				pControl->SelectItem(0);
				DisplayCurrentDTC();
			}
			else if (name == _T("pending_dtc"))
			{
				pControl->SelectItem(1);
				DisplayPendingDTC();
			}
			else if (name == _T("permanent_dtc"))
				pControl->SelectItem(2);
			else if (name == _T("freeze_frame_data"))
				pControl->SelectItem(3);
		}
		else if (msg.sType == DUI_MSGTYPE_CLICK)
		{
			if (msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_refresh"))))
			{
				theVehicleComm->ScanForDtc();
				CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
				if (pControl)
				{
					switch (pControl->GetCurSel())
					{
					case 0:
						DisplayCurrentDTC();
						break;
					case 1:
						DisplayPendingDTC();
						break;
					}
				}
			} 
			else if (msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_clear"))))
			{
				CYesNoMessageBoxDlg* pYesNoDlg = new CYesNoMessageBoxDlg(CPaintManagerUI::GetMultiLanguageString(115));
				pYesNoDlg->Create(m_hWnd, _T("YesNoMsgBoxDlg"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 450, 150);
				pYesNoDlg->CenterWindow();
				if (IDYES == pYesNoDlg->ShowModal())
				{
					theVehicleComm->ClearDtc();
				}
			}
		}
	}
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ((BOOL)wParam)
		{
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if (pControl)
			{
				switch (pControl->GetCurSel())
				{
				case 0:
					DisplayCurrentDTC();
					break;
				case 1:
					DisplayPendingDTC();
					break;
				}
			}
		}
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CDialogBuilderCallbackEx_DTC cb;
		CControlUI* pRoot = builder.Create(_T("vehicle_dtc.xml"), NULL, &cb, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		DisplayCurrentDTC();
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

