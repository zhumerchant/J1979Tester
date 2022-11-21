// J1979Tester.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "CTesterMainFrame.h"
#include "CVehicleComm.h"
#include "CVehicleDatabase.h"
#include  <time.h>

using namespace DuiLib;

CTesterMainFrame* g_pMainFrame = NULL;

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
{
	TCHAR buf[512];
	::SendMessage(hWnd, WM_GETTEXT, 512, (LPARAM)buf);
	DUITRACE(_T("EnumChildWindowCallBack=%s"), buf);
	::PostMessage(hWnd, WM_CHANGELANG, 0, 0);
	return ::EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    // 递归查找子窗口   
}

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD dwPid = 0;
	::GetWindowThreadProcessId(hWnd, &dwPid);
	if (dwPid == lParam)
	{
		TCHAR buf[512];
		::SendMessage(hWnd, WM_GETTEXT, 512, (LPARAM)buf);
		DUITRACE(_T("EnumWindowCallBack=%s"), buf);
		::PostMessage(hWnd, WM_CHANGELANG, 0, 0);
		::EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}

	return  TRUE;
}

void ReloadLanguage(void)
{
	const char* g_szLang;
	switch (g_nLanguageIndex)
	{
	case 0: //ENG
		g_szLang = "ENG";
		break;
	case 1: //CHS
		g_szLang = "CHS";
		break;
	case 2: //JPN
		g_szLang = "JPN";
		break;
	case 3: //CHT
		g_szLang = "CHT";
		break;
	case 4: //CSY
		g_szLang = "CSY";
		break;
	case 5: //DAN
		g_szLang = "DAN";
		break;
	case 6: //DEU
		g_szLang = "DEU";
		break;
	case 7: //FRA
		g_szLang = "FRA";
		break;
	case 8: //ITA
		g_szLang = "ITA";
		break;
	case 9: //ESP
		g_szLang = "ESP";
		break;
	case 10: //NLD
		g_szLang = "NLD";
		break;
	case 11: //SVE
		g_szLang = "SVE";
		break;
	case 12: //FIN
		g_szLang = "FIN";
		break;
	case 13: //NOR
		g_szLang = "NOR";
		break;
	case 14: //PTG
		g_szLang = "PTG";
		break;
	case 15: //PTB
		g_szLang = "PTB";
		break;
	case 16: //ELL
		g_szLang = "ELL";
		break;
	case 17: //TRK
		g_szLang = "TRK";
		break;
	case 18: //HUN
		g_szLang = "HUN";
		break;
	case 19: //SLO
		g_szLang = "SLO";
		break;
	case 20: //PLK
		g_szLang = "PLK";
		break;
	case 21: //RUS
		g_szLang = "RUS";
		break;
	case 22: //THA
		g_szLang = "THA";
		break;
	case 23: //ROM
		g_szLang = "ROM";
		break;
	default:
		g_szLang = "ENG";
		break;
	}

	theVehicleDb.reset();
	theVehicleDb = std::unique_ptr<CVehicleDatabase>(new CVehicleDatabase());
	theVehicleDb->Open(g_szLang);

	g_pMainFrame->PostMessage(WM_CHANGELANG, 0, 0);
}


int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
	const char* g_szLang;
    GetUserPreference();

    if (!theVehicleComm->LoadPassThruLibrary())
    {
        return 0;
    }
    HRESULT Hr = ::OleInitialize(NULL);
    if (FAILED(Hr)) return 0;

    CPaintManagerUI::SetInstance(hInstance);
#ifdef _DEBUG
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("..\\J1979Tester\\resource"));
#else
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("resource"));
#endif

	switch (g_nLanguageIndex)
	{
	case 0: //ENG
		g_szLang = "ENG";
		break;
	case 1: //CHS
		g_szLang = "CHS";
		break;
	case 2: //JPN
		g_szLang = "JPN";
		break;
	case 3: //CHT
		g_szLang = "CHT";
		break;
	case 4: //CSY
		g_szLang = "CSY";
		break;
	case 5: //DAN
		g_szLang = "DAN";
		break;
	case 6: //DEU
		g_szLang = "DEU";
		break;
	case 7: //FRA
		g_szLang = "FRA";
		break;
	case 8: //ITA
		g_szLang = "ITA";
		break;
	case 9: //ESP
		g_szLang = "ESP";
		break;
	case 10: //NLD
		g_szLang = "NLD";
		break;
	case 11: //SVE
		g_szLang = "SVE";
		break;
	case 12: //FIN
		g_szLang = "FIN";
		break;
	case 13: //NOR
		g_szLang = "NOR";
		break;
	case 14: //PTG
		g_szLang = "PTG";
		break;
	case 15: //PTB
		g_szLang = "PTB";
		break;
	case 16: //ELL
		g_szLang = "ELL";
		break;
	case 17: //TRK
		g_szLang = "TRK";
		break;
	case 18: //HUN
		g_szLang = "HUN";
		break;
	case 19: //SLO
		g_szLang = "SLO";
		break;
	case 20: //PLK
		g_szLang = "PLK";
		break;
	case 21: //RUS
		g_szLang = "RUS";
		break;
	case 22: //THA
		g_szLang = "THA";
		break;
	case 23: //ROM
		g_szLang = "ROM";
		break;
	default:
		g_szLang = "ENG";
		break;
	}

	if (!theVehicleDb->Open(g_szLang))
	{
		::OleUninitialize();
		return 0;
	}

    CTesterMainFrame* pFrame = new CTesterMainFrame();
    if (pFrame == NULL) return 0;
	g_pMainFrame = pFrame;
    pFrame->Create(NULL, NULL, UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();
    ::OleUninitialize();
    return 0;
}