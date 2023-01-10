#pragma once
#include "framework.h"
#include "CFTPDownloader.h"
#include <Setupapi.h>
#include <Cfgmgr32.h>
#include <initguid.h>
#include "CYesNoMBDlg.h"

#pragma comment(lib, "Setupapi.lib")

#define	MAX_DEVICES	(256)

extern void FtpDownloadStatusCallback(
	DWORD_PTR dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength
);
extern int callback1(int status, int misc_para);

static DWORD WaitObjectAndMsg(HANDLE hEventThread, DWORD dwMilliseconds)
{
	BOOL  bWait = TRUE;
	DWORD dwEvt = WAIT_FAILED;
	MSG msg;
	clock_t start, finish;
	double  duration;

	start = clock();

	while (bWait)
	{
		dwEvt = ::MsgWaitForMultipleObjects(1, &hEventThread, FALSE, dwMilliseconds, QS_ALLINPUT);
		switch (dwEvt)
		{
		case WAIT_OBJECT_0:
		{
			return WAIT_OBJECT_0;
		}
		break;

		case WAIT_OBJECT_0 + 1:
		{
			while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (WM_CLOSE == msg.message || WM_QUIT == msg.message)
				{
					return WAIT_FAILED;
				}
				else
				{
					PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				if (INFINITE != dwMilliseconds)
				{
					finish = clock();
					duration = (double)(finish - start) / CLOCKS_PER_SEC;
					if (duration * 1000 > dwMilliseconds)
					{
						return WAIT_TIMEOUT;
					}
				}
			}
			break;
		}

		default:
		{
			return dwEvt;
		}
		}
	}

	return dwEvt;
}

static char* w2c(char* pcstr, const wchar_t* pwstr, size_t len)
{
	int nlength = (int)wcslen(pwstr);

	//��ȡת����ĳ���?

	int nbytes = WideCharToMultiByte(0, // specify the code page used to perform the conversion

		0,         // no special flags to handle unmapped characters

		pwstr,     // wide character string to convert

		nlength,   // the number of wide characters in that string

		NULL,      // no output buffer given, we just want to know how long it needs to be

		0,

		NULL,      // no replacement character given

		NULL);    // we don't want to know if a character didn't make it through the translation

	// make sure the buffer is big enough for this, making it larger if necessary

	if (nbytes > (int)len)   nbytes = (int)len;

	// ͨ�����ϵõ��Ľ����ת��unicode �ַ�Ϊascii �ַ�

	WideCharToMultiByte(0, // specify the code page used to perform the conversion

		0,         // no special flags to handle unmapped characters

		pwstr,   // wide character string to convert

		-1,   // the number of wide characters in that string

		pcstr, // put the output ascii characters at the end of the buffer

		nbytes,   // there is at least this much space there

		NULL,      // no replacement character given

		NULL);

	*(pcstr + nbytes) = 0;
	return pcstr;
}


DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0X86E0D1E0L, 0X8089, 0X11D0, 0X9C, 0XE4, 0X08, 0X00, 0X3E, 0X30, 0X1F, 0X73);

static int Detect(int* nPortNum)
{
	int nPortCount = 0;

	std::string strErr;
	std::vector<std::wstring> strFriendNames;
	std::string strPortName;
	int i = 0;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA* pDetData = nullptr;
	WCHAR wszKey[30]; //"USB\\VID_E327&PID_2534"

	wszKey[0] = 'U';
	wszKey[1] = 'S';
	wszKey[2] = 'B';
	wszKey[3] = '\\';
	wszKey[4] = 'V';
	wszKey[5] = 'I';
	wszKey[6] = 'D';
	wszKey[7] = '_';
	wszKey[8] = 'E';
	wszKey[9] = '3';
	wszKey[10] = '2';
	wszKey[11] = '7';
	wszKey[12] = '&';
	wszKey[13] = 'P';
	wszKey[14] = 'I';
	wszKey[15] = 'D';
	wszKey[16] = '_';
	wszKey[17] = '2';
	wszKey[18] = '5';
	wszKey[19] = '3';
	wszKey[20] = '4';
	wszKey[21] = '\\';
	wszKey[22] = '\0';

	try
	{
		hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT,
			nullptr,
			nullptr,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			strErr = "SetupDiGetClassDevs failed.";
			throw strErr;
		}

		// Enumerate the serial ports
		BOOL bOk = TRUE;
		BOOL bSuccess = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.

		for (DWORD ii = 0; bOk; ii++)
		{
			memset(&ifcData, 0, sizeof(ifcData));
			ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				nullptr, &GUID_DEVINTERFACE_COMPORT, ii, &ifcData);
			if (bOk)
			{
				// Got a device. Get the details.
				memset(pDetData, 0, sizeof(dwDetDataSize));
				pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, nullptr, &devdata);
				if (bOk)
				{
					ULONG DeviceInstancePathLength = 0;
					PWSTR DeviceInstancePath = NULL;
					CONFIGRET cr = CM_Get_Device_ID_Size(&DeviceInstancePathLength,
						devdata.DevInst,
						0);

					if (cr != CR_SUCCESS)
					{
						strErr = "Error retrieving device instance path size.";
					}
					else
					{
						DeviceInstancePath = (PWSTR)malloc(DeviceInstancePathLength * sizeof(WCHAR));

						if (DeviceInstancePath != NULL)
						{
							cr = CM_Get_Device_IDW(devdata.DevInst,
								DeviceInstancePath,
								DeviceInstancePathLength,
								0);

							if (cr != CR_SUCCESS)
							{
								strErr = "Error retrieving device instance path.\n";
							}
							else
							{
								// USB\VID_E327&PID_2534\MT000001
								if ((30 <= DeviceInstancePathLength) && (0 == wcsncmp(DeviceInstancePath, wszKey, 22)))
								{
									wchar_t fname[256];
									bSuccess = SetupDiGetDeviceRegistryProperty(
										hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
										(PBYTE)fname, sizeof(fname), NULL);
									if (bSuccess)
									{
										strFriendNames.push_back(fname);
									}
								}
							}
							free(DeviceInstancePath);
						}
					}
				}
				else
				{
					strErr = "SetupDiGetDeviceInterfaceDetail failed.";
					throw strErr;
				}
			}
			else
			{
				DWORD err = GetLastError();
				if (err != ERROR_NO_MORE_ITEMS)
				{
					strErr = "SetupDiEnumDeviceInterfaces failed";
					throw strErr;
				}
			}
		}
	}
	catch (std::string strCatchErr)
	{
		strErr = strCatchErr;
	}

	if (pDetData != nullptr)
		delete[](char*) pDetData;
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	if (!strErr.empty())
		throw strErr;

	int nDeviceCount = (int)strFriendNames.size();
	if ((0 == nDeviceCount) || (nDeviceCount > MAX_DEVICES))
	{
		return 0;
	}
	char szSerialNumber[MAX_PATH];

	for (i = 0; i < nDeviceCount; i++)
	{
		w2c(szSerialNumber, strFriendNames[i].c_str(), strFriendNames[i].size() * 2);
		strPortName = szSerialNumber;
		int nPos1 = 0;
		int nPos2 = 0;
		DWORD dwPortNum = 0;
		if (((nPos1 = (int)strPortName.find_last_of('(')) >= 0) && ((nPos2 = (int)strPortName.find_last_of(')')) >= 0))
		{
			if (nPos2 > nPos1)
			{
				strPortName = strPortName.substr(nPos1 + 1, nPos2 - nPos1 - 1);
				if (0 == strPortName.compare(0, 3, "COM"))
				{
					sscanf_s(strPortName.c_str(), "COM%d", &dwPortNum);
					*nPortNum++ = (int)dwPortNum;
					nPortCount++;
				}
			}
		}
	}
	return nPortCount;
}


typedef int(_cdecl* F_FNURANUS)(int x, int y);
typedef int (*CALLBYU)(int status, int misc_para);
typedef int(_cdecl* F_URANUSDO)(int pn, char* px, int len, CALLBYU pf);

static bool get_value_by_key(uint8_t* tp_info, uint16_t dict_size, const char* key, unsigned char* value, int* val_len)
{
	int i = 0;
	int ptr = 0;
	int size = 0;
	int pairs = 0;
	int kl = 0;
	int vl = 0;
	bool keymatched = false;

	size = (int)dict_size;
	if (ptr >= size)
	{
		return false;
	}
	pairs = (int)tp_info[ptr++];

	for (i = 0; i < pairs; i++)
	{
		if (ptr >= size)
		{
			return false;
		}
		kl = (int)tp_info[ptr++];
		if (0 == kl)
		{
			return false;
		}
		if ((ptr + kl) > size)
		{
			return false;
		}
		if (0 == strncmp((char*)&tp_info[ptr], key, kl))
		{
			keymatched = true;
		}
		ptr += kl;

		if (ptr >= size)
		{
			return false;
		}
		vl = (int)tp_info[ptr++];
		if (0 == vl)
		{
			return false;
		}
		if ((ptr + vl) > size)
		{
			return false;
		}
		if (keymatched)
		{
			if (vl > *val_len)
			{
				return false;
			}
			memcpy(value, &tp_info[ptr], vl);
			*val_len = vl;
			return true;
		}
		ptr += vl;
	}
	return false;
}

static LPVOID g_pMainWnd = NULL;


class CUpgradeDlg : public CWindowWnd, public INotifyUI
{
public:
	CUpgradeDlg() { m_pBusyBoxDlg = NULL; m_hEvtUpgrade = ::CreateEvent(NULL, FALSE, FALSE, NULL); }

public:
	CPaintManagerUI m_pm;
	CBusyBoxDlg* m_pBusyBoxDlg;
	CDuiString m_strRemoteAppVer;
	CDuiString m_strRemoteSoftVer;
	CDuiString m_strLocalAppVer;
	CDuiString m_strLocalSoftVer;
	HANDLE m_hEvtUpgrade;

public:
	LPCTSTR GetWindowClassName() const { return _T("UIUpgradeDlg"); }
	virtual UINT GetClassStyle() const { return CS_DBLCLKS; }
	void OnFinalMessage(HWND /*hWnd*/) { ::CloseHandle(m_hEvtUpgrade);  delete this; }
	void Notify(TNotifyUI& msg)
	{
		DUITRACE(_T("CUpgradeDlg::Notify. msg.sType=%s"), msg.sType);
		if (msg.sType == DUI_MSGTYPE_CLICK)
		{
			if (msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_upgrade"))))
			{
				DWORD dwThreadExitCode = 0;
				static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_upgrade")))->SetEnabled(false);
				static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_ok")))->SetEnabled(false);

				HANDLE hThread = ::CreateThread(NULL, 0, &CUpgradeDlg::UpdgradeThreadProc, (LPVOID)this, 0, NULL);
				m_pBusyBoxDlg = new CBusyBoxDlg(CPaintManagerUI::GetMultiLanguageString(99));
				m_pBusyBoxDlg->Create(::GetParent(m_hWnd), _T("BusyBoxDlg"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 300, 150);
				m_pBusyBoxDlg->CenterWindow();
				m_pBusyBoxDlg->ShowModal();
				if (hThread)
				{
					WaitObjectAndMsg(hThread, INFINITE);
					::GetExitCodeThread(hThread, &dwThreadExitCode);
					::CloseHandle(hThread);
				}
				static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_ok")))->SetEnabled(true);
				if (1000 == dwThreadExitCode)
				{					
					CYesNoMessageBoxDlg * pYesNoDlg = new CYesNoMessageBoxDlg(CPaintManagerUI::GetMultiLanguageString(114));
					pYesNoDlg->Create(::GetParent(m_hWnd), _T("BusyBoxDlg"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 450, 350);
					pYesNoDlg->CenterWindow();
					if (IDYES == pYesNoDlg->ShowModal())
					{
						::PostMessage(this->GetHWND(), WM_CLOSE, 1000, NULL);
					}
					else
					{
						::PostMessage(this->GetHWND(), WM_CLOSE, 0, 0);
					}
				}
				else
				{
					static_cast<CTextUI*>(m_pm.FindControl(_T("tip_status")))->SetText(CPaintManagerUI::GetMultiLanguageString(110));
				}
			}
			else if (msg.pSender == static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_ok"))))
			{
				::PostMessage(this->GetHWND(), WM_CLOSE, NULL, NULL);
			}
		}
	}

	static DWORD WINAPI RetriveInfoFromServerThreadProc(LPVOID lpParameter)
	{
		CUpgradeDlg* pMainWnd = (CUpgradeDlg*)lpParameter;
		CFTPDownloader ftpDwnldr;
		unsigned char szAppVer[100];
		unsigned char szSoftVer[100];
		uint8_t* databuf = NULL;
		int nValLen = 0;
		int i;
		TCHAR tszLocalFwVer[20];

#ifdef BUILD_GODIAG
		if (!ftpDwnldr.Download(_T("sw_j1979_godiag.dat")))
#else
		if (!ftpDwnldr.Download(_T("sw_j1979.dat")))
#endif
		{
			pMainWnd->m_pBusyBoxDlg->Close();
			return 1;
		}
		nValLen = sizeof (szSoftVer) - 1;
		if (!get_value_by_key(ftpDwnldr.GetDownloadedFileData(), (uint16_t)ftpDwnldr.GetDownloadedFileLength(), "version", szSoftVer, &nValLen))
		{
			pMainWnd->m_pBusyBoxDlg->Close();
			return 2;
		}
		szSoftVer[nValLen] = 0;

		pMainWnd->m_strRemoteSoftVer.Empty();
		for (i = 0; i < nValLen; i++)
		{
			pMainWnd->m_strRemoteSoftVer += szSoftVer[i];
		}

		if (!ftpDwnldr.Download(_T("app_nano.dat")))
		{
			pMainWnd->m_pBusyBoxDlg->Close();
			return 1;
		}
		nValLen = sizeof(szAppVer) - 1;
		if (!get_value_by_key(ftpDwnldr.GetDownloadedFileData(), (uint16_t)ftpDwnldr.GetDownloadedFileLength(), "version", szAppVer, &nValLen))
		{
			pMainWnd->m_pBusyBoxDlg->Close();
			return 2;
		}
		szAppVer[nValLen] = 0;

		pMainWnd->m_strRemoteAppVer.Empty();
		for (i = 0; i < nValLen; i++)
		{
			pMainWnd->m_strRemoteAppVer += szAppVer[i];
		}

		pMainWnd->m_strLocalSoftVer = SOFTWARE_VERSION;
		if (theVehicleComm->ConnectDevice())
		{
			if (theVehicleComm->GetFirmwareVersion(tszLocalFwVer))
			{
				pMainWnd->m_strLocalAppVer = tszLocalFwVer;
			}
			theVehicleComm->DisconnectDevice();
		}

		pMainWnd->m_pBusyBoxDlg->Close();
		return 0;
	}

	static DWORD WINAPI UpdgradeThreadProc(LPVOID lpParameter)
	{
		CUpgradeDlg* pMainWnd = (CUpgradeDlg*)lpParameter;
		CFTPDownloader ftpDwnldr;
		unsigned char szAppName[100];
		unsigned char szSoftName[100];
		CDuiString strSoftFileName;
		CDuiString strAppFileName;
		int nValLen = 0;
		int i;

		if (0 != pMainWnd->m_strRemoteSoftVer.Compare(pMainWnd->m_strLocalSoftVer))
		{
#ifdef BUILD_GODIAG
			if (!ftpDwnldr.Download(_T("sw_j1979_godiag.dat")))
#else
			if (!ftpDwnldr.Download(_T("sw_j1979.dat")))
#endif
			{
				pMainWnd->m_pBusyBoxDlg->Close();
				return 1;
			}
			nValLen = sizeof(szSoftName) - 1;
			if (!get_value_by_key(ftpDwnldr.GetDownloadedFileData(), (uint16_t)ftpDwnldr.GetDownloadedFileLength(), "name", szSoftName, &nValLen))
			{
				pMainWnd->m_pBusyBoxDlg->Close();
				return 2;
			}
			szSoftName[nValLen] = 0;

			for (i = 0; i < nValLen; i++)
			{
				strSoftFileName += szSoftName[i];
			}
		}

		if (0 != pMainWnd->m_strRemoteAppVer.Compare(pMainWnd->m_strLocalAppVer))
		{
			if (!ftpDwnldr.Download(_T("app_nano.dat")))
			{
				pMainWnd->m_pBusyBoxDlg->Close();
				return 3;
			}
			nValLen = sizeof(szAppName) - 1;
			if (!get_value_by_key(ftpDwnldr.GetDownloadedFileData(), (uint16_t)ftpDwnldr.GetDownloadedFileLength(), "name", szAppName, &nValLen))
			{
				pMainWnd->m_pBusyBoxDlg->Close();
				return 4;
			}
			szAppName[nValLen] = 0;

			for (i = 0; i < nValLen; i++)
			{
				strAppFileName += szAppName[i];
			}
		}
		pMainWnd->m_pBusyBoxDlg->Close();

		if (!strAppFileName.IsEmpty())
		{
			ftpDwnldr.SetStatusCallback(FtpDownloadStatusCallback, (DWORD_PTR)pMainWnd);
			if (!ftpDwnldr.Download(strAppFileName))
			{
				return 5;
			}
			int nPortNum[MAX_DEVICES];
			if (1 == Detect(nPortNum))
			{
				HINSTANCE hdll;
				F_URANUSDO f_uranus;
				g_pMainWnd = (LPVOID)pMainWnd;
				hdll = ::LoadLibraryA("uranus_p1.dll");
				if (!hdll)
				{
					return 6;
				}
				f_uranus = (F_URANUSDO)GetProcAddress(hdll, "uranus_do");
				if (!f_uranus)
				{
					FreeLibrary(hdll);
					return 7;
				}
				::ResetEvent(pMainWnd->m_hEvtUpgrade);
				if (S_OK != f_uranus(nPortNum[0], (char*)ftpDwnldr.GetDownloadedFileData(), (int)ftpDwnldr.GetDownloadedFileLength(), (CALLBYU)&callback1))
				{
					FreeLibrary(hdll);
					return 8;
				}
				if (WAIT_OBJECT_0 != ::WaitForSingleObject(pMainWnd->m_hEvtUpgrade, 2 * 60 * 1000)) //wait for 2 minutes
				{
					FreeLibrary(hdll);
					return 9;
				}
				FreeLibrary(hdll);
			}
		}

		if (!strSoftFileName.IsEmpty())
		{
			ftpDwnldr.SetStatusCallback(FtpDownloadStatusCallback, (DWORD_PTR)pMainWnd);
			if (!ftpDwnldr.Download(strSoftFileName))
			{
				return 10;
			}
			FILE* file = NULL;
			if (_wfopen_s(&file, _T("update\\install_package.exe"), _T("wb")))
			{
				return 11;
			}
			if (ftpDwnldr.GetDownloadedFileLength() != fwrite(ftpDwnldr.GetDownloadedFileData(), 1, ftpDwnldr.GetDownloadedFileLength(), file))
			{
				fclose(file);
				return 12;
			}
			fclose(file);

			return 1000;
		}

		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("upgrade_dlg.xml"), NULL, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		HANDLE hThread = ::CreateThread(NULL, 0, &CUpgradeDlg::RetriveInfoFromServerThreadProc, (LPVOID)this, 0, NULL);
		m_pBusyBoxDlg = new CBusyBoxDlg(CPaintManagerUI::GetMultiLanguageString(99));
		m_pBusyBoxDlg->Create(::GetParent(m_hWnd), _T("BusyBoxDlg"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 300, 150);
		m_pBusyBoxDlg->CenterWindow();
		m_pBusyBoxDlg->ShowModal();
		if (hThread)
		{
			DWORD dwThreadExitCode = 0;
			::WaitForSingleObject(hThread, INFINITE);
			::GetExitCodeThread(hThread, &dwThreadExitCode);
			::CloseHandle(hThread);
			if (0 == dwThreadExitCode)
			{
				static_cast<CTextUI*>(m_pm.FindControl(_T("remote_soft_ver")))->SetText(m_strRemoteSoftVer);
				static_cast<CTextUI*>(m_pm.FindControl(_T("remote_fw_ver")))->SetText(m_strRemoteAppVer);
				static_cast<CTextUI*>(m_pm.FindControl(_T("local_soft_ver")))->SetText(m_strLocalSoftVer);
				static_cast<CTextUI*>(m_pm.FindControl(_T("local_fw_ver")))->SetText(m_strLocalAppVer);

				if ((!m_strRemoteSoftVer.IsEmpty()) && (!m_strRemoteAppVer.IsEmpty()) &&
					((0 != m_strRemoteSoftVer.Compare(m_strLocalSoftVer)) || (0 != m_strRemoteAppVer.Compare(m_strLocalAppVer))))
				{
					static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_upgrade")))->SetEnabled(true);
				}
				else
				{
					static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_upgrade")))->SetEnabled(false);
				}
			}
		}

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
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
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
		default:
			bHandled = FALSE;
		}
		if (bHandled) return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}
};

static void FtpDownloadStatusCallback(
	DWORD_PTR dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength
)
{
	CUpgradeDlg* pMainWnd = (CUpgradeDlg*)dwContext;

	if (INTERNET_STATUS_RESPONSE_RECEIVED == dwInternetStatus)
	{
		static_cast<CTextUI*>(pMainWnd->m_pm.FindControl(_T("tip_status")))->SetText(CPaintManagerUI::GetMultiLanguageString(108));
		static_cast<CProgressUI*>(pMainWnd->m_pm.FindControl(_T("progress_upgrade")))->SetValue((int)*(DWORD*)lpvStatusInformation);
	}
	else if (INTERNET_STATUS_CONNECTING_TO_SERVER == dwInternetStatus)
	{
		static_cast<CTextUI*>(pMainWnd->m_pm.FindControl(_T("tip_status")))->SetText(CPaintManagerUI::GetMultiLanguageString(106));
	}
	else if (INTERNET_STATUS_CONNECTED_TO_SERVER == dwInternetStatus)
	{
		static_cast<CTextUI*>(pMainWnd->m_pm.FindControl(_T("tip_status")))->SetText(CPaintManagerUI::GetMultiLanguageString(107));
	}
}

static int g_lastPercent = -1;

static int callback1(int status, int misc_para)
{
	CUpgradeDlg* pMainWnd = (CUpgradeDlg * )g_pMainWnd;

	if (0 == status)
	{
		if (g_lastPercent != misc_para)
		{
			static_cast<CTextUI*>(pMainWnd->m_pm.FindControl(_T("tip_status")))->SetText(CPaintManagerUI::GetMultiLanguageString(109));
			static_cast<CProgressUI*>(pMainWnd->m_pm.FindControl(_T("progress_upgrade")))->SetValue(misc_para);
			g_lastPercent = misc_para;
		}

		if (100 == g_lastPercent)
		{
			::SetEvent(pMainWnd->m_hEvtUpgrade);
		}
	}
	else
	{
		if (2 == status)
		{
			static_cast<CTextUI*>(pMainWnd->m_pm.FindControl(_T("tip_status")))->SetText(CPaintManagerUI::GetMultiLanguageString(111));
			::SetEvent(pMainWnd->m_hEvtUpgrade);
		}
	}

	return 0;
}
