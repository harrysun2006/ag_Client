#include "StdAfx.h"
#include "VBSession.h"
#include "XRelogin.h"
#include "utility/XmlNodeWrapper.h"

#include "ViewBarDlg.h"

namespace http
{
	//#define SERVLET_URL _T("/servlet/")
	//#define SERVLET_URL _T("/fileUploadTest.jsp")

	CVBSession * CVBSession::m_pInstance = NULL;


	CVBSession::CVBSession(void)
	{
		InitializeCriticalSection(&m_CriticalSection);

		// http server
		CString strHttpServer = thePrefs.GetWebServiceURL();
		int iPos = strHttpServer.Find(_T(":"));
		int iPort = _ttoi(strHttpServer.Mid(iPos+1));
		m_http.SetHttpServer(strHttpServer.Left(iPos), iPort);

		// path
		m_strWebServicePath.Format(_T("/%s/"), thePrefs.GetWebServicePath());
	}

	CVBSession::~CVBSession(void)
	{
		DeleteCriticalSection(&m_CriticalSection);
	}

	CVBSession * CVBSession::GetInstance()
	{
		if (m_pInstance == NULL)
			m_pInstance = new CVBSession;

		return m_pInstance;
	}

	void CVBSession::FreeInstance()
	{
		if (m_pInstance)
			delete m_pInstance;
		m_pInstance = NULL;
	}

	// send a request to server
	BOOL CVBSession::Request(CString strServiceName, CString &strRequest, CString &strResponse)
	{
		g_log.OutputDebug(strServiceName+ _T(" Request ") + strRequest);
		EnterCriticalSection(&m_CriticalSection);

		BOOL bRet = FALSE;

		bRet = m_http.PostTextFile(strRequest, _T("c:\\charry.txt"), strServiceName, m_strWebServicePath, strResponse);
		
		// check whether session is time out or not
		// and whether another user login or not
		CVBSession::VB_SESSION vbSession;
		vbSession = IsSessionValid(strResponse);
		if ( vbSession == CVBSession::SESSION_TIMEOUT)
		{
			g_log.OutputDebug(_T("SESSION_TIMEOUT, relogin... ") + strResponse);
			LeaveCriticalSection(&m_CriticalSection);

			bRet = CXRelogin::Login();
			if (bRet == FALSE)	// failed to login
			{
				LeaveCriticalSection(&m_CriticalSection);
				return FALSE;
			}

			g_log.OutputDebug(_T("relogin OK! PostTextFile again"));
			TRACE(_T("Re Login\n"));
			bRet = m_http.PostTextFile(strRequest, _T("c:\\charry.txt"), strServiceName, m_strWebServicePath, strResponse);
		}
		else
		if (vbSession == CVBSession::SESSION_MULTIPLE)
		{
			g_log.OutputDebug(_T("SESSION_MULTIPLE") + strResponse);
			::PostMessage(g_MainDlg->GetSafeHwnd(), WM_VB_MULTIPLE_LOGIN, 0, 0);
		}
		else
		// force user to update ViewBar
		if (vbSession == CVBSession::SESSION_FORCEUPDATE)
		{
			g_log.OutputDebug(_T("SESSION_FORCEUPDATE") + strResponse);
			::PostMessage(g_MainDlg->GetSafeHwnd(), WM_VB_FORCE_UPDATE, 0, 0);
		}

		LeaveCriticalSection(&m_CriticalSection);
		//g_log.OutputDebug(_T("CVBSession::Request return "),bRet);

		return bRet;	
	}

	BOOL CVBSession::Request(CString strServiceName, std::string &strRequest, CString &strResponse)
	{
		EnterCriticalSection(&m_CriticalSection);
		const char* str = strRequest.c_str();
		int leng = strRequest.length();

		int k;
		char chSName[MAX_PATH];
		k = WideCharToMultiByte(CP_ACP,0,strServiceName.GetBuffer(),strServiceName.GetLength(),chSName,MAX_PATH,NULL,NULL);
		chSName[k] = '\0';


		BOOL bRet = m_http.PostxFile((char*)str,strRequest.size(),"text/plain","c:\\charry.txt",
			chSName,m_strWebServicePath,strResponse);

		CVBSession::VB_SESSION vbSession;
		vbSession = IsSessionValid(strResponse);
		if ( vbSession == CVBSession::SESSION_TIMEOUT)
		{
			g_log.OutputDebug(_T("SESSION_TIMEOUT, relogin... ") + strResponse);
			LeaveCriticalSection(&m_CriticalSection);
			bRet = CXRelogin::Login();
			if (bRet == FALSE)	// failed to login
			{
				LeaveCriticalSection(&m_CriticalSection);
				return FALSE;
			}
			g_log.OutputDebug(_T("relogin OK! PostTextFile again"));

			TRACE(_T("Re Login\n"));
			bRet = m_http.PostxFile((char*)str,strRequest.size(),"text/plain","c:\\charry.txt",
				chSName,m_strWebServicePath,strResponse);
		}
		else
		if (vbSession == CVBSession::SESSION_MULTIPLE)
		{
			g_log.OutputDebug(_T("SESSION_MULTIPLE") + strResponse);
			::PostMessage(g_MainDlg->GetSafeHwnd(), WM_VB_MULTIPLE_LOGIN, 0, 0);
		}
		else
		// force user to update ViewBar
		if (vbSession == CVBSession::SESSION_FORCEUPDATE)
		{
			g_log.OutputDebug(_T("SESSION_FORCEUPDATE") + strResponse);
			::PostMessage(g_MainDlg->GetSafeHwnd(), WM_VB_FORCE_UPDATE, 0, 0);
		}

		LeaveCriticalSection(&m_CriticalSection);
		g_log.OutputDebug(_T("CVBSession::Request return "),bRet);
		return bRet;
	}

	// check if the session is valid
	CVBSession::VB_SESSION CVBSession::IsSessionValid(const CString &strServerResponse)
	{
		// parse response

		//<?xml version="1.0" encoding="UTF-8"?><authorize result="401"/>
		utility::CXmlDocumentWrapper xmlDoc;
		xmlDoc.LoadXML(strServerResponse);
		utility::CXmlNodeWrapper node(xmlDoc.AsNode());
		CString code = node.GetValue(_T("result"));
		if (code == _VB_UNAUTHORIZED)
		{
			return SESSION_TIMEOUT;
		}
		else
		if (code == _VB_OTHER_PLACE_LOGIN)
		{
			return SESSION_MULTIPLE;
		}
		else
		if (code == _VB_FORCE_UPDATE)
		{
			return SESSION_FORCEUPDATE;
		}

		return SESSION_OK;
	}
}
