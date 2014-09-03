// ***************************************************************
//  CVBSession   version:  1.0  date: 04/03/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "utility/http/MyHttps.h"
namespace http
{
	class CVBSession
	{
	public:
		enum VB_SESSION{SESSION_OK, SESSION_TIMEOUT, SESSION_MULTIPLE, SESSION_FORCEUPDATE};

		CVBSession(void);
		~CVBSession(void);
	public:
		static CVBSession * GetInstance();
		static void FreeInstance();

		BOOL Request(CString strServiceName, CString &strRequest, CString &strResponse);
		BOOL Request(CString strServiceName, std::string &strRequest, CString &strResponse);

	private:
		VB_SESSION IsSessionValid(const CString &strServerResponse);
	private:
		static CVBSession *m_pInstance;
		utility::CMyHttps m_http;
		CRITICAL_SECTION m_CriticalSection;
		CString m_strWebServicePath;
	};
}