#pragma once
#include "CBusyBoxDlg.h"
#include "CMessageBoxDlg.h"
#include "CVehicleComm.h"

class CVehicleBasicInfoUI : public CContainerUI
{
public:
	CVehicleBasicInfoUI()
	{
		CDialogBuilder builder;
		CContainerUI* pBasicInfoUI = static_cast<CContainerUI*>(builder.Create(_T("basic_info.xml"), NULL));
		if (pBasicInfoUI) {
			this->Add(pBasicInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CVehicleDTCInfoUI : public CContainerUI
{
public:
	CVehicleDTCInfoUI()
	{
		CDialogBuilder builder;
		CContainerUI* pDTCInfoUI = static_cast<CContainerUI*>(builder.Create(_T("dtc_info.xml"), NULL));
		if (pDTCInfoUI) {
			this->Add(pDTCInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CVehicleReadinessInfoUI : public CContainerUI
{
public:
	CVehicleReadinessInfoUI()
	{
		CDialogBuilder builder;
		CContainerUI* pDTCInfoUI = static_cast<CContainerUI*>(builder.Create(_T("readiness_info.xml"), NULL));
		if (pDTCInfoUI) {
			this->Add(pDTCInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CVehicleFreezeFrameInfoUI : public CContainerUI
{
public:
	CVehicleFreezeFrameInfoUI()
	{
		CDialogBuilder builder;
		CContainerUI* pDTCInfoUI = static_cast<CContainerUI*>(builder.Create(_T("freeze_frame_info.xml"), NULL));
		if (pDTCInfoUI) {
			this->Add(pDTCInfoUI);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CDialogBuilderCallbackEx_Info : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if (_tcscmp(pstrClass, _T("vehicle_info")) == 0) return new CVehicleBasicInfoUI;
		if (_tcscmp(pstrClass, _T("summary_dtc")) == 0) return new CVehicleDTCInfoUI;
		if (_tcscmp(pstrClass, _T("summary_readiness")) == 0) return new CVehicleReadinessInfoUI;
		if (_tcscmp(pstrClass, _T("summary_freeze")) == 0) return new CVehicleFreezeFrameInfoUI;
		return NULL;
	}
};

class CVehicleInfoFrame : public CWindowWnd, public INotifyUI
{
public:
    CVehicleInfoFrame();
	static DWORD WINAPI ConnectToVehicleThreadProc(LPVOID lpParameter)
	{
		CVehicleInfoFrame* pMainWnd = (CVehicleInfoFrame*)lpParameter;
		if (!theVehicleComm->ConnectDevice())
		{
			pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
			return ERR_DEVICE_NOT_CONNECTED;
		}
		pMainWnd->m_pBusyBoxDlg->SetTipMessage(CPaintManagerUI::GetMultiLanguageString(49));
		if (!theVehicleComm->ScanForChannel())
		{
			pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
			return ERR_BUFFER_EMPTY;
		}
		if (!theVehicleComm->ScanForDtc())
		{
			pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
			return ERR_BUFFER_EMPTY;
		}
		
		//if (!theVehicleComm->ScanForSupportInfoType())
		//{
		//	pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
		//	return ERR_BUFFER_EMPTY;
		//}
		if (!theVehicleComm->ScanForSupportCurrentData())
		{
			pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
			return ERR_BUFFER_EMPTY;
		}
		//if (!theVehicleComm->ScanForSupportFreezeFrameData())
		//{
		//	pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
		//	return ERR_BUFFER_EMPTY;
		//}
		//if (!theVehicleComm->ReadInfoTypes(pMainWnd->m_tInfoTypes))
		//{
		//	pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
		//	return ERR_BUFFER_EMPTY;
		//}
		if (!theVehicleComm->ScanForCurrentDataValue())
		{
			pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);
			return ERR_BUFFER_EMPTY;
		}

		pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);

		return STATUS_NOERROR;
	}

	static DWORD WINAPI DisconnectToVehicleThreadProc(LPVOID lpParameter)
	{
		CVehicleInfoFrame* pMainWnd = (CVehicleInfoFrame*)lpParameter;
		theVehicleComm->DisconnectDevice();
		pMainWnd->m_pBusyBoxDlg->PostMessage(WM_CLOSE);

		return 0;
	}

	void ClearBasicInfo(void)
	{
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		CListUI* pList = static_cast<CListUI*>(m_pm.FindSubControlByName(pControl, _T("list_vehicle_basic_info")));

		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();
	}

	void AddOneBasicInfo(CDuiString& name, CDuiString& content)
	{
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		CListUI* pList = static_cast<CListUI*>(m_pm.FindSubControlByName(pControl, _T("list_vehicle_basic_info")));

		if (NULL == pList)
		{
			return;
		}

		CListContainerElementUI* pListElement = NULL;
		CDialogBuilder builder;
		pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("one_basic_info.xml")));
	
		if (pListElement == NULL)
			return;

		CLabelUI* pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_name")));
		if (pLab != NULL)
		{
			pLab->SetText(name);
		}

		pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_content")));
		if (pLab != NULL)
		{
			pLab->SetText(content);
		}

		pListElement->SetFixedHeight(25);
		if (!pList->Add(pListElement))
		{
			delete pListElement;
		}
	}

	void DisplayDtcInfo(void)
	{
		CDuiString name;
		CDuiString content;
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		CListUI* pList = static_cast<CListUI*>(m_pm.FindSubControlByName(pControl, _T("list_vehicle_dtc_info")));
		if (NULL == pList)
		{
			return;
		}
		pList->RemoveAll();

		size_t i = 0;
		CLabelUI* pLab = NULL;

		for (i = 0; i < theVehicleComm->GetEcuID().size(); i++)
		{
			std::vector<unsigned long> dtcs;
			if (!theVehicleComm->GetConfirmedDtc((int)i, dtcs))
			{
				continue;
			}
			CDialogBuilder builder;
			CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("one_basic_info.xml")));
			if (NULL == pListElement)
			{
				continue;
			}

			pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_name")));
			if (pLab != NULL)
			{
				name.Format(_T("ECU#%u %s"), i + 1, CPaintManagerUI::GetMultiLanguageString(88));
				pLab->SetText(name);
			}

			pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_content")));
			if (pLab != NULL)
			{
				content.Format(_T("%u"), dtcs.size());
				pLab->SetText(content);
			}

			pListElement->SetFixedHeight(25);
			if (!pList->Add(pListElement))
			{
				delete pListElement;
			}
		}

		for (i = 0; i < theVehicleComm->GetEcuID().size(); i++)
		{
			std::vector<unsigned long> dtcs;
			if (!theVehicleComm->GetPendingDtc((int)i, dtcs))
			{
				continue;
			}
			CDialogBuilder builder;
			CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("one_basic_info.xml")));
			if (NULL == pListElement)
			{
				continue;
			}

			pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_name")));
			if (pLab != NULL)
			{
				name.Format(_T("ECU#%u %s"), i + 1, CPaintManagerUI::GetMultiLanguageString(89));
				pLab->SetText(name);
			}

			pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_content")));
			if (pLab != NULL)
			{
				content.Format(_T("%u"), dtcs.size());
				pLab->SetText(content);
			}

			pListElement->SetFixedHeight(25);
			if (!pList->Add(pListElement))
			{
				delete pListElement;
			}
		}
		if (0 == theVehicleComm->GetProtocolName().Compare(_T("ISO15765")))
		{
			for (i = 0; i < theVehicleComm->GetEcuID().size(); i++)
			{
				std::vector<unsigned long> dtcs;
				if (!theVehicleComm->GetPermanentDtc((int)i, dtcs))
				{
					continue;
				}
				CDialogBuilder builder;
				CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(builder.Create(_T("one_basic_info.xml")));
				if (NULL == pListElement)
				{
					continue;
				}

				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_name")));
				if (pLab != NULL)
				{
					name.Format(_T("ECU#%u %s"), i + 1, CPaintManagerUI::GetMultiLanguageString(90));
					pLab->SetText(name);
				}

				pLab = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("param_content")));
				if (pLab != NULL)
				{
					content.Format(_T("%u"), dtcs.size());
					pLab->SetText(content);
				}

				pListElement->SetFixedHeight(25);
				if (!pList->Add(pListElement))
				{
					delete pListElement;
				}
			}
		}
	}
public:
	CBusyBoxDlg* m_pBusyBoxDlg;
    CPaintManagerUI m_pm;
	std::vector <T_INFO_TYPE> m_tInfoTypes;

public:
    LPCTSTR GetWindowClassName() const { return _T("UIVehicleInfoFrame"); }
    virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
    void OnFinalMessage(HWND /*hWnd*/) { delete this; }
	void Notify(TNotifyUI& msg)
	{
		DUITRACE(_T("CVehicleInfoFrame::Notify. msg.sType=%s"), msg.sType);
		if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			CDuiString name = msg.pSender->GetName();
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if (name == _T("vehicle_info"))
				pControl->SelectItem(0);
			else if (name == _T("summary_dtc"))
			{
				pControl->SelectItem(1);
				DisplayDtcInfo();
			}
			else if (name == _T("summary_readiness"))
				pControl->SelectItem(2);
			else if (name == _T("summary_freeze"))
				pControl->SelectItem(3);
		}
		else if (msg.sType == DUI_MSGTYPE_CLICK)
		{
			CDuiString name = msg.pSender->GetName();
			if (name == _T("btn_connect"))
			{
				ClearBasicInfo();

				m_pBusyBoxDlg = new CBusyBoxDlg(CPaintManagerUI::GetMultiLanguageString(47));
				m_pBusyBoxDlg->Create(m_hWnd, _T("BusyBoxDlg"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 300, 150);
				m_pBusyBoxDlg->CenterWindow();
				HANDLE hThread = CreateThread(NULL, 0, &CVehicleInfoFrame::ConnectToVehicleThreadProc, (LPVOID)this, 0, NULL);
				m_pBusyBoxDlg->ShowModal();

				if (hThread)
				{
					DWORD dwThreadExitCode = 0;
					size_t i = 0;
					size_t m = 0;

					WaitForSingleObject(hThread, INFINITE);
					GetExitCodeThread(hThread, &dwThreadExitCode);
					if (STATUS_NOERROR == dwThreadExitCode)
					{
						CDuiString strName = CPaintManagerUI::GetMultiLanguageString(50);
						CDuiString strContent;
						CDuiString strByte;
						strContent.Format(_T("%s"), theVehicleComm->GetProtocolName());
						AddOneBasicInfo(strName, strContent);

						const std::vector<unsigned long>& ecuid = theVehicleComm->GetEcuID();
						strName = CPaintManagerUI::GetMultiLanguageString(51);
						strContent.Format(_T("%u"), ecuid.size());
						AddOneBasicInfo(strName, strContent);

						for (i = 0; i < ecuid.size(); i++)
						{
							strName.Format(_T("ECU#%u ID"), i + 1);
							switch (theVehicleComm->GetECUIDLength())
							{
							case 1:
								strContent.Format(_T("%02X"), ecuid[i]);
								break;
							case 2:
								strContent.Format(_T("%04X"), ecuid[i]);
								break;
							case 4:
								strContent.Format(_T("%08X"), ecuid[i]);
								break;
							default:
								strContent.Format(_T("%02X"), ecuid[i]);
								break;
							}
							AddOneBasicInfo(strName, strContent);
						}

						for (i = 0; i < m_tInfoTypes.size(); i++)
						{
							switch (m_tInfoTypes[i].infoType)
							{
							case 4:
								strName.Format(_T("ECU#%u %s"), m_tInfoTypes[i].ecuIndex + 1, CPaintManagerUI::GetMultiLanguageString(52));
								strContent.Empty();
								for (m = 1; m < m_tInfoTypes[i].infoData.size(); m++)
								{
									if (0 != m_tInfoTypes[i].infoData[m])
									{
										strByte.Format(_T("%c"), m_tInfoTypes[i].infoData[m]);
										strContent += strByte;
									}
								}
								break;
							case 6:
								strName.Format(_T("ECU#%u %s"), m_tInfoTypes[i].ecuIndex + 1, CPaintManagerUI::GetMultiLanguageString(53));
								strContent.Format(_T("%08X"), (unsigned long)((m_tInfoTypes[i].infoData[1] << 24) |
									(m_tInfoTypes[i].infoData[2] << 16) |
									(m_tInfoTypes[i].infoData[3] << 8) |
									(m_tInfoTypes[i].infoData[4] << 0)));
								break;
							}
							AddOneBasicInfo(strName, strContent);
						}

						msg.pSender->SetEnabled(false);
						CButtonUI* pDisconnectBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_disconnect")));
						pDisconnectBtn->SetEnabled(true);
						
					}
					else
					{			
						CDuiString msg = CPaintManagerUI::GetMultiLanguageString(54);
						msg.Replace(_T("\\n"), _T("\n"));
						CMessageBoxDlg* pMessageBox = new CMessageBoxDlg(msg);
						pMessageBox->Create(m_hWnd, _T("MessageBoxDlg"), WS_POPUP | UI_WNDSTYLE_DIALOG, 0, 0, 0, 400, 250);
						pMessageBox->CenterWindow();
						pMessageBox->ShowModal();
						theVehicleComm->DisconnectDevice();
					}
				}
			}
			else if (name == _T("btn_disconnect"))
			{
				ClearBasicInfo();

				HANDLE hThread = CreateThread(NULL, 0, &CVehicleInfoFrame::DisconnectToVehicleThreadProc, (LPVOID)this, 0, NULL);
				m_pBusyBoxDlg = new CBusyBoxDlg(CPaintManagerUI::GetMultiLanguageString(48));
				m_pBusyBoxDlg->Create(m_hWnd, _T("BusyBoxDlg"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 300, 150);
				m_pBusyBoxDlg->CenterWindow();
				m_pBusyBoxDlg->ShowModal();
				if (hThread)
				{
					DWORD dwThreadExitCode = 0;
					WaitForSingleObject(hThread, INFINITE);
					GetExitCodeThread(hThread, &dwThreadExitCode);

					msg.pSender->SetEnabled(false);
					CButtonUI* pConnectBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_connect")));
					pConnectBtn->SetEnabled(true);
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
		CDialogBuilderCallbackEx_Info cb;
		CControlUI* pRoot = builder.Create(_T("vehicle_information.xml"), NULL, &cb, &m_pm);
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
		// ��ʱ�����յ�WM_NCDESTROY���յ�wParamΪSC_CLOSE��WM_SYSCOMMAND
		if (wParam == SC_CLOSE) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if (::IsZoomed(*this) != bZoomed) {
			if (!bZoomed) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if (pControl) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if (pControl) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if (pControl) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if (pControl) pControl->SetVisible(false);
			}
		}
		return lRes;
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
		case WM_CHANGELANG:    lRes = OnChangeLanguage(); break;
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

