// ***************************************************************
//  CMyHttps   version:  1.0  date: 03/30/2007
//  -------------------------------------------------------------
//  A wrapper class for http, https(default)
//  support http proxy
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include <afxinet.h>

namespace utility
{
	class CMyHttps
	{
	public:
		CMyHttps(void);
		CMyHttps(const CString &strHttpServer, INTERNET_PORT port);
	public:
		~CMyHttps(void);

		BOOL SetHttpServer(const CString & strHttpServer,INTERNET_PORT port, BOOL bSSL = TRUE);
		BOOL Post(CString &request, CString &url, CString &response);
		BOOL PostTextFile(CString &text, CString filename, CString strServiceName, CString url, CString &response);
		BOOL PostFile(TCHAR *pFile, int iFileLen, CString filetype, CString filename, CString strServiceName, CString url, CString &response);
		BOOL Get(CString &response, CString &url);

		BOOL PostxFile(const char *pFileToUpload, int iFileLen, const char* filetype, const char* filename, const char* strServiceName, CString url, CString &response);

	private:
		BOOL Open();
		BOOL Close();
		BOOL SetHttpProxy(CString ip, UINT port);
		DWORD GetInternetFlag();
		CString MakeRequestHeaders(CString& strBoundary);
		CString MakePreFileData(CString& strBoundary, CString& strFileName, CString strServiceName, CString strFileType);
		CString MakePostFileData(CString& strBoundary);

		CInternetSession *m_pSession;
		CHttpConnection *m_pConnection;
		CString m_strHttpServer;
		INTERNET_PORT m_Port;
		DWORD m_dwStatusCode;

		BOOL m_bSSL;
	};

}