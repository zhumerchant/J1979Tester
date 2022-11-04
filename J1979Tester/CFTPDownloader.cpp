#include "framework.h"
#include "CFTPDownloader.h"

#pragma comment(lib, "WinInet.lib")


BOOL GetFTPServerInfo(CString& strAddress, CString& strUserName, CString& strPwd, CString& strDir)
{
	CString IFName;
	CString strAppName;
	CString strKeyName;
	CString strKeyValue;
	CString strTemp;

	GetCurrentDirectory(MAX_PATH, IFName.GetBuffer(MAX_PATH));
	IFName.ReleaseBuffer();
	IFName = IFName + _T("\\setting.ini");

	strAppName = _T("FTP");

	strKeyName = _T("IPAddr");
	strKeyValue.Empty();
	GetPrivateProfileString(strAppName, strKeyName, strKeyValue, strKeyValue.GetBuffer(128), 128, IFName);
	strKeyValue.ReleaseBuffer();
	strAddress = strKeyValue;

	strKeyName = _T("Dir");
	strKeyValue.Empty();
	GetPrivateProfileString(strAppName, strKeyName, strKeyValue, strKeyValue.GetBuffer(MAX_PATH), MAX_PATH, IFName);
	strKeyValue.ReleaseBuffer();
	strDir = strKeyValue;

	strUserName = _T("j2534customer");
	strPwd = _T("www.rokwy.com");

	return TRUE;
}

static DWORD g_lastDownloadPercent = -1;

void CALLBACK FtpInetCallback(HINTERNET hInet,
	DWORD dwContex, 
	DWORD dwStatus, 
	LPVOID lpvStatus, 
	DWORD dwLength)
{
	CFTPDownloader * pWnd = (CFTPDownloader*)dwContex;

	if (INTERNET_STATUS_RESPONSE_RECEIVED == dwStatus)
	{
		if (pWnd->m_pfnStatusCallback)
		{
			pWnd->m_dwDownloadDataPtr += *(LPDWORD)lpvStatus;
			if (pWnd->m_dwDownloadDataPtr > pWnd->m_dwDownloadDataSize)
			{
				return;
			}
			DWORD dwPercent = (pWnd->m_dwDownloadDataPtr + 1) * 100 / pWnd->m_dwDownloadDataSize;
			if (dwPercent != g_lastDownloadPercent)
			{
				pWnd->m_pfnStatusCallback(pWnd->m_pdwCallbackContex, dwStatus, &dwPercent, sizeof(DWORD));
				g_lastDownloadPercent = dwPercent;
			}
		}
	}
	else if ((INTERNET_STATUS_CONNECTING_TO_SERVER == dwStatus) || (INTERNET_STATUS_CONNECTED_TO_SERVER == dwStatus))
	{
		if (pWnd->m_pfnStatusCallback)
		{
			pWnd->m_pfnStatusCallback(pWnd->m_pdwCallbackContex, dwStatus, NULL, 0);
		}
		pWnd->m_dwDownloadDataPtr = 0;
	}
}

CFTPDownloader::CFTPDownloader()
{
	m_dwDownloadDataPtr = 0;
	m_dwDownloadDataSize = 0;
	m_pDownloadData = NULL;
	m_pfnStatusCallback = NULL;
	m_pdwCallbackContex = NULL;
	GetFTPServerInfo(m_strHostName, m_strUserName, m_strPassword, m_strDir);
}

CFTPDownloader::~CFTPDownloader()
{
	if (m_pDownloadData)
	{
		delete[]m_pDownloadData;
	}
}

void CFTPDownloader::SetStatusCallback(FTP_STATUS_CALLBACK* lpfnStatusCallback, DWORD_PTR pArgs)
{
	m_pfnStatusCallback = lpfnStatusCallback;
	m_pdwCallbackContex = pArgs;
}

BOOL CFTPDownloader::Download(LPCTSTR lpstrUrlPath)
{
	HINTERNET hInternet, hConnect, hFTPFile = NULL;
	DWORD dwBufferSize = 4096;
	BYTE* pBuf = NULL;
	DWORD dwBytesReturn = 0;
	DWORD dwOffset = 0;
	BOOL bRet = FALSE;

	hInternet = InternetOpen(_T("WinInet Ftp"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (NULL == hInternet)
	{
		return FALSE;
	}
	InternetSetStatusCallback(hInternet, FtpInetCallback);

	hConnect = InternetConnect(hInternet, m_strHostName, INTERNET_INVALID_PORT_NUMBER,
		m_strUserName, m_strPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (NULL == hConnect)
	{
		InternetCloseHandle(hInternet);
		return FALSE;
	}

	if (!FtpSetCurrentDirectory(hConnect, m_strDir)) 
	{
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return FALSE;
	}

	hFTPFile = FtpOpenFile(hConnect, lpstrUrlPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, (DWORD)this);
	if (0 == hFTPFile)
	{
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	InternetSetStatusCallback(hFTPFile, FtpInetCallback);

	m_dwDownloadDataSize = FtpGetFileSize(hFTPFile, NULL);
	if (0 == m_dwDownloadDataSize)
	{	
		InternetCloseHandle(hFTPFile);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	if (m_pDownloadData)
	{
		delete[]m_pDownloadData;
	}
	m_dwDownloadDataPtr = 0;
	m_pDownloadData = new BYTE[m_dwDownloadDataSize];
	::memset(m_pDownloadData, 0, m_dwDownloadDataSize);
	pBuf = new BYTE[dwBufferSize];
	::memset(pBuf, 0, dwBufferSize);

	do
	{
		bRet = InternetReadFile(hFTPFile, pBuf, dwBufferSize, &dwBytesReturn);
		if (!bRet)
		{
			break;
		}
		::memcpy((m_pDownloadData + dwOffset), pBuf, dwBytesReturn);
		dwOffset = dwOffset + dwBytesReturn;
	} while (m_dwDownloadDataSize > dwOffset);

	delete[]pBuf;

	if (!bRet)
	{
		InternetCloseHandle(hFTPFile);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return FALSE;
	}

	InternetCloseHandle(hFTPFile);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	return TRUE;
}

DWORD CFTPDownloader::GetDownloadedFileLength(void) const
{
	return m_dwDownloadDataSize;
}

BYTE* CFTPDownloader::GetDownloadedFileData(void) const
{
	return m_pDownloadData;
}
