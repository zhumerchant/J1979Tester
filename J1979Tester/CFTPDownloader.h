#pragma once
#include <WinInet.h>

typedef void FTP_STATUS_CALLBACK(
	DWORD_PTR dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength
);

class CFTPDownloader
{
	CString m_strHostName;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strDir;
	BYTE * m_pDownloadData;
public:
	FTP_STATUS_CALLBACK* m_pfnStatusCallback;
	DWORD_PTR m_pdwCallbackContex;
	DWORD m_dwDownloadDataSize;
	DWORD m_dwDownloadDataPtr;
	CFTPDownloader();
	~CFTPDownloader();
	BOOL Download(LPCTSTR lpstrUrlPath);
	void SetStatusCallback(FTP_STATUS_CALLBACK* lpfnStatusCallback, DWORD_PTR pArgs);
	DWORD GetDownloadedFileLength(void) const;
	BYTE * GetDownloadedFileData(void) const;
};

