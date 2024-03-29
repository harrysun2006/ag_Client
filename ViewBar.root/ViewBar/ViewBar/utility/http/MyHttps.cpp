// ***************************************************************
//  CMyHttps   version:  1.0  date: 03/30/2007
//  -------------------------------------------------------------
//  A wrapper class for https(default) and http
//  this class supports uploading files
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#include "StdAfx.h"
#include "MyHttps.h"
#include "../../Registry.h"
#include "../../MyUtility.h"

namespace utility
{
	CMyHttps::CMyHttps() : 
		m_pSession(NULL),
		m_pConnection(NULL),
		m_Port(0),
		m_bSSL(FALSE)
	{

	}

	CMyHttps::CMyHttps(const CString &strHttpServer, INTERNET_PORT port) :
		m_pSession(NULL),
		m_pConnection(NULL),
		m_strHttpServer(strHttpServer),
		m_Port(port),
		m_bSSL(FALSE)
	{
		Open();
	}

	CMyHttps::~CMyHttps()
	{
		Close();
	}

	BOOL CMyHttps::SetHttpProxy(CString ip, UINT port) 
	{ 

		CString proxy; 
		proxy.Format(_T("%s:%d"), ip, port); 

		CRegistry reg;

		std::string key = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ProxyServer";
		reg.SetKeyValue(CRegistry::HK_CURRENT_USER, key, proxy);
		key = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ProxyEnable";
		reg.SetKeyValue(CRegistry::HK_CURRENT_USER, key, 1);

		// enable proxy
		if(!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0)) 
		{ 
			return FALSE; 
		} 


		return TRUE; 
	} 

	BOOL CMyHttps::Open()
	{
		Close();
		
		if (thePrefs.IsProxyEnable())
		{
			int port = _ttoi(thePrefs.GetProxyPort());
			SetHttpProxy(thePrefs.GetProxyIp(),port);
		}

		try
		{
			m_pSession = new CInternetSession();

			// default timeout: 5 secs
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000);
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
			m_pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 0);

			m_pConnection = m_pSession->GetHttpConnection(m_strHttpServer, m_Port);
		}
		catch(CInternetException * e)
		{
			//e->ReportError();
			e->Delete();
			return FALSE;
		}
		return TRUE;
	}

	BOOL CMyHttps::Close()
	{
		if(m_pConnection)
		{
			m_pConnection->Close();
			delete m_pConnection;
			m_pConnection = NULL;
		}

		if(m_pSession)
		{
			m_pSession->Close();
			delete m_pSession;
			m_pSession = NULL;
		}

		return TRUE;
	}

	DWORD CMyHttps::GetInternetFlag()
	{
		DWORD dwFlags = 0;

		// http with SSL
		if (m_bSSL)
		{
			dwFlags = INTERNET_FLAG_RELOAD |     
			INTERNET_FLAG_NO_CACHE_WRITE |   
			INTERNET_FLAG_KEEP_CONNECTION;  

			dwFlags |= INTERNET_FLAG_SECURE |
			INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
			INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
		}
		// http
		else
		{
			dwFlags = INTERNET_FLAG_RELOAD | 
				INTERNET_FLAG_DONT_CACHE |
				INTERNET_FLAG_KEEP_CONNECTION;
		}

		return dwFlags;
	}

	BOOL CMyHttps::SetHttpServer(const CString &strHttpServer, INTERNET_PORT port, BOOL bSSL /* = TRUE */)
	{
		if(m_strHttpServer == strHttpServer && m_Port == port)
			return TRUE;

		m_strHttpServer = strHttpServer;
		m_Port = port;
		m_bSSL = bSSL;

		return Open();
	}

	// Post data to server
	// url indicates the relative path to the server, "/foo.asp"
	int CMyHttps::Post(CString &request, CString &url, CString &response)
	{
		if(!m_pConnection)
			return FALSE;

		CHttpFile *pFile = NULL;
		try
		{
			DWORD dwFlags = GetInternetFlag();

			pFile = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
				url, NULL, 1, NULL,	NULL, dwFlags);

			if(pFile)
			{
				pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded;"));
				BOOL bRequestOK = pFile->SendRequest(NULL, 0, 
					(LPVOID)(LPTSTR)(request.GetBuffer(0)),
					request.GetLength());
				request.ReleaseBuffer();

				if(!bRequestOK)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				// Status Code
				pFile->QueryInfoStatusCode(m_dwStatusCode);
				if(m_dwStatusCode != 200)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				// get the size of Content-Length
				CString strContentLength;
				pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strContentLength);
				int nContentLength = _ttoi(strContentLength);

				UINT nReadCount = 0;

				int HTTP_READ_BUFFER_SIZE = nContentLength > 0 ? nContentLength : 4096;
				TCHAR * szBuf = new TCHAR[HTTP_READ_BUFFER_SIZE+1];
				memset(szBuf, 0, HTTP_READ_BUFFER_SIZE+1);

				response.Empty();

				while(true)
				{
					memset(szBuf,0,HTTP_READ_BUFFER_SIZE+1);
					// nReadCount = pFile->Read(szBuf,HTTP_READ_BUFFER_SIZE);
					nReadCount = pFile->Read(szBuf,
						(pFile->GetLength() < HTTP_READ_BUFFER_SIZE) ? pFile->GetLength() : HTTP_READ_BUFFER_SIZE);

					if(nReadCount <= 0)
					{
						break;
					}

					response += szBuf;
				}

				response += szBuf;
				delete []szBuf;

				pFile->Close();
				delete pFile;
			}
		}
		catch(CInternetException * e)
		{
			// e->ReportError();
			pFile->Close();
			delete pFile;
			e->Delete();
			return FALSE;
		}

		return TRUE;
	}

	// Post data to server
	// url indicates the relative path to the server, "/foo.asp"
	BOOL CMyHttps::PostTextFile(CString &text, CString filename, CString strServiceName, CString url, CString &response)
	{
		CStringA strMultiByteText = CMyUtility::GetCStringA(text);
		int iLen = strMultiByteText.GetLength();

		CStringA strMultiByteResponse;
		BOOL bRet = PostFile(text.GetBuffer(), iLen, _T("text/plain"), 
			filename, strServiceName,  url, strMultiByteResponse);

		int len = strMultiByteResponse.GetLength() + 1;

		TCHAR *buf = new TCHAR[len];
		MultiByteToWideChar(CP_UTF8, 0, strMultiByteResponse, len,
			buf, len);

		response.Format(_T("%s"), buf);
		delete [] buf;

		//response = CMyUtility::GetCStringW(strMultiByteResponse);

		return bRet;
	}

	BOOL CMyHttps::PostxFile(const char *pFileToUpload, int iFileLen, const char* filetype,
		const char* filename, const char* strServiceName, CString url, CString &response)
	{
		if(!m_pConnection)
			return FALSE;

		CHttpFile *pFile = NULL;

		try
		{
			DWORD dwFlags = GetInternetFlag();
			pFile = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
				url, NULL, 1, NULL,	NULL, dwFlags);

			CString strHTTPBoundary = _T("IllBeVerySurprisedIfThisTurnsUp");
			std::string strPreFileData;
			strPreFileData = "--IllBeVerySurprisedIfThisTurnsUp";
			strPreFileData += "\r\n";
			strPreFileData += "Content-Disposition: form-data; name=\"serviceName\"";
			strPreFileData += "\r\n\r\n";
			strPreFileData += strServiceName;
			strPreFileData += "\r\n";
			strPreFileData += "--IllBeVerySurprisedIfThisTurnsUp";
			strPreFileData += "\r\n";
			strPreFileData += "Content-Disposition: form-data; name=\"trackdata\"; filename=\"";
			strPreFileData += filename;
			strPreFileData += "\"";
			strPreFileData += "\r\n";
			strPreFileData += "Content-Type: text/plain; chartset=utf-8";
			strPreFileData += "\r\n";
			strPreFileData += "Content-Transfer-Encoding: binary";
			strPreFileData += "\r\n\r\n";

			std::string strPostFileData;
			strPostFileData = "\r\n";
			strPostFileData += "--IllBeVerySurprisedIfThisTurnsUp";
			strPostFileData += "\r\n";
			strPostFileData += "Content-Disposition: form-data; name=\"submitted\"";
			strPostFileData += "\r\n\r\n";
			strPostFileData += "charry.org";
			strPostFileData += "\r\n";
			strPostFileData += "--IllBeVerySurprisedIfThisTurnsUp--";
			strPostFileData += "\r\n";


			DWORD dwTotalRequestLength = 
				strPreFileData.size() +
				strPostFileData.size() + iFileLen;

			if(pFile)
			{
				pFile->AddRequestHeaders(MakeRequestHeaders(strHTTPBoundary));
				BOOL bRequestOK = pFile->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);

				if(!bRequestOK)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				USES_CONVERSION;

 				pFile->Write((strPreFileData.c_str()), strPreFileData.size());
 				pFile->Write((pFileToUpload), iFileLen);
				pFile->Write((strPostFileData.c_str()), strPostFileData.size());

				pFile->EndRequest(HSR_SYNC);


				// Status Code
				pFile->QueryInfoStatusCode(m_dwStatusCode);
				if(m_dwStatusCode != 200)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				// get the size of Content-Length
				CString strContentLength;
				pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strContentLength);
				int nContentLength = _ttoi(strContentLength);

				UINT nReadCount = 0;

				int HTTP_READ_BUFFER_SIZE = nContentLength > 0 ? nContentLength : 4096;
				char * szBuf = new char[HTTP_READ_BUFFER_SIZE+1];
				memset(szBuf, 0, HTTP_READ_BUFFER_SIZE+1);

				response.Empty();

				while(true)
				{
					memset(szBuf,0,HTTP_READ_BUFFER_SIZE+1);
					nReadCount = pFile->Read(szBuf,
						(pFile->GetLength() < HTTP_READ_BUFFER_SIZE) ? pFile->GetLength() : HTTP_READ_BUFFER_SIZE);

					if(nReadCount <= 0)
					{
						break;
					}

					response += szBuf;
				}

				response += szBuf;
				delete []szBuf;

				pFile->Close();
				delete pFile;
			}
		}
		catch(CInternetException * e)
		{
			//		e->ReportError();
			pFile->Close();
			delete pFile;
			e->Delete();
			return FALSE;
		}

		return TRUE;
	}
	BOOL CMyHttps::PostFile(TCHAR *pFileToUpload, int iFileLen, CString filetype,
		CString filename, CString strServiceName, CString url, CStringA &response)
	{
		if(!m_pConnection)
			return FALSE;

		CHttpFile *pFile = NULL;

		try
		{
			DWORD dwFlags = GetInternetFlag();

			pFile = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
				url, NULL, 1, NULL,	NULL, dwFlags);

			CString strHTTPBoundary = _T("IllBeVerySurprisedIfThisTurnsUp");
			CString strPreFileData = MakePreFileData(strHTTPBoundary, filename, strServiceName, filetype);
			CString strPostFileData = MakePostFileData(strHTTPBoundary);

			DWORD dwTotalRequestLength = 
				strPreFileData.GetLength() +
				strPostFileData.GetLength() + iFileLen;

			if(pFile)
			{
				pFile->AddRequestHeaders(MakeRequestHeaders(strHTTPBoundary));
				BOOL bRequestOK = pFile->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);

				if(!bRequestOK)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				USES_CONVERSION;

 #ifdef _UNICODE
 				pFile->Write(W2A(strPreFileData), strPreFileData.GetLength());
 #else
 				pFile->Write((LPTSTR)(LPCTSTR)strPreFileData, strPreFileData.GetLength());
 #endif
 
 
 #ifdef _UNICODE
 				pFile->Write(W2A(pFileToUpload), iFileLen);
 #else
 				pFile->Write((LPTSTR)(LPCTSTR)pFileToUpload, iFileLen);
 #endif
 
 
 #ifdef _UNICODE
 				pFile->Write(W2A(strPostFileData), strPostFileData.GetLength());
 #else
 				pFile->Write((LPTSTR)(LPCTSTR)strPostFileData, strPostFileData.GetLength());
 #endif
 				pFile->EndRequest(HSR_SYNC);


				// Status Code
				pFile->QueryInfoStatusCode(m_dwStatusCode);
				if(m_dwStatusCode != 200)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				// get the size of Content-Length
				CString strContentLength;
				pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strContentLength);
				int nContentLength = _ttoi(strContentLength);

				UINT nReadCount = 0;

				int HTTP_READ_BUFFER_SIZE = nContentLength > 0 ? nContentLength : 4096;
				char * szBuf = new char[HTTP_READ_BUFFER_SIZE+1];
				memset(szBuf, 0, HTTP_READ_BUFFER_SIZE+1);

				response.Empty();

				while(true)
				{
					memset(szBuf,0,HTTP_READ_BUFFER_SIZE+1);
					//				nReadCount = pFile->Read(szBuf,HTTP_READ_BUFFER_SIZE);
					nReadCount = pFile->Read(szBuf,
						(pFile->GetLength() < HTTP_READ_BUFFER_SIZE) ? pFile->GetLength() : HTTP_READ_BUFFER_SIZE);

					if(nReadCount <= 0)
					{
						break;
					}

					response += szBuf;
				}

				response += szBuf;
				delete []szBuf;

				pFile->Close();
				delete pFile;
			}
		}
		catch(CInternetException * e)
		{
			//		e->ReportError();
			pFile->Close();
			delete pFile;
			e->Delete();
			return FALSE;
		}

		return TRUE;
	}

	int CMyHttps::Get(CString url, CString &response)
	{
		if(!m_pConnection)
			return FALSE;

		CHttpFile *pFile = NULL;
		CStringA strMultiByteResponse;

		try
		{
			DWORD dwFlags = GetInternetFlag();

			pFile = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, url
				,NULL, 1, NULL,	NULL,
				dwFlags);

// 			INTERNET_PROXY_INFO   proxyinfo;   
// 			proxyinfo.dwAccessType   =   INTERNET_OPEN_TYPE_PROXY;   
// 			proxyinfo.lpszProxy   =   "119.119.119.9:8080";   
// 			proxyinfo.lpszProxyBypass   =   NULL;   
// 
// 			BOOL xxx=	m_pConnection->SetOption(INTERNET_OPTION_PROXY,   (LPVOID)&proxyinfo,   sizeof(INTERNET_PROXY_INFO));   
// 			//pFile->SetOption(INTERNET_OPTION_PROXY_USERNAME,   strUsername,   strlen(strUsername)+1);   
			//pFile->SetOption(INTERNET_OPTION_PROXY_PASSWORD,   strPassword,   strlen(strPassword)+1);  

			if(pFile)
			{
				BOOL bRequestOK = pFile->SendRequest();
				if(!bRequestOK)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				pFile->QueryInfoStatusCode(m_dwStatusCode);
				if(m_dwStatusCode != 200)
				{
					pFile->Close();
					delete pFile;
					return FALSE;
				}

				CString strContentLength;
				pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strContentLength);
				int nContentLength = _ttoi(strContentLength);

				UINT nReadCount = 0;

				int HTTP_READ_BUFFER_SIZE = nContentLength > 0 ? nContentLength : 4096;
				char * szBuf = new char[HTTP_READ_BUFFER_SIZE+1];
				memset(szBuf, 0, HTTP_READ_BUFFER_SIZE+1);

				response.Empty();

				while(true)
				{
					memset(szBuf,0,HTTP_READ_BUFFER_SIZE+1);
					nReadCount = pFile->Read(szBuf, HTTP_READ_BUFFER_SIZE);
					//				nReadCount = pFile->Read(szBuf,(pFile->GetLength() < HTTP_READ_BUFFER_SIZE) ? pFile->GetLength() : HTTP_READ_BUFFER_SIZE);
					if(nReadCount <= 0)
					{
						break;
					}

					strMultiByteResponse += szBuf;
				}

				response += szBuf;
				delete []szBuf;

				pFile->Close();
				delete pFile;
			}
		}
		catch(CInternetException * e)
		{
			//		e->ReportError();
			pFile->Close();
			delete pFile;
			e->Delete();
			return FALSE;
		}

		int len = strMultiByteResponse.GetLength() + 1;

		TCHAR *buf = new TCHAR[len];
		MultiByteToWideChar(CP_UTF8, 0, strMultiByteResponse, len,
			buf, len);

		response.Format(_T("%s"), buf);
		delete [] buf;

		return TRUE;
	}



	CString CMyHttps::MakeRequestHeaders(CString& strBoundary)
	{
		CString strFormat;
		CString strData;
		strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");
		strData.Format(strFormat, strBoundary);
		return strData;
	}

	CString CMyHttps::MakePreFileData(CString& strBoundary, CString& strFileName, CString strServiceName,
		CString strFileType)
	{

 	
//  		strFormat += _T("--%s");
//  		strFormat += _T("\r\n");
//  		strFormat += _T("Content-Disposition: form-data; name=\"trackdata\"; filename=\"%s\"");
//  		strFormat += _T("\r\n");
//  		strFormat += _T("Content-Type: %s");
//  		strFormat += _T("\r\n");
//  		strFormat += _T("Content-Transfer-Encoding: binary");
//  		strFormat += _T("\r\n\r\n");
//  		strData.Format(strFormat, strBoundary, strFileName, strFileType);

 		CString strFormat;
 		CString strData;
 		strFormat += _T("--%s");
 		strFormat += _T("\r\n");
 		strFormat += _T("Content-Disposition: form-data; name=\"serviceName\"");
 		strFormat += _T("\r\n\r\n");
 		strFormat += _T("%s");
 		strFormat += _T("\r\n");
 		strFormat += _T("--%s");
 		strFormat += _T("\r\n");
 		strFormat += _T("Content-Disposition: form-data; name=\"trackdata\"; filename=\"%s\"");
 		strFormat += _T("\r\n");
 		strFormat += _T("Content-Type: text/plain");
 		strFormat += _T("\r\n");
 		strFormat += _T("Content-Transfer-Encoding: binary");
 		strFormat += _T("\r\n\r\n");
 		strData.Format(strFormat,strBoundary, strServiceName, strBoundary, strFileName);

		return strData;
	}

	CString CMyHttps::MakePostFileData(CString& strBoundary)
	{
		CString strFormat;
		CString strData;
		strFormat = _T("\r\n");
		strFormat += _T("--%s");
		strFormat += _T("\r\n");
		strFormat += _T("Content-Disposition: form-data; name=\"submitted\"");
		strFormat += _T("\r\n\r\n");
		strFormat += _T("charry.org");
		strFormat += _T("\r\n");
		strFormat += _T("--%s--");
		strFormat += _T("\r\n");
		strData.Format(strFormat, strBoundary, strBoundary);

		return strData;
	}

}