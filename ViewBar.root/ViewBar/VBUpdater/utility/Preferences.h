// ***************************************************************
//  CPreferences   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  This class aims to record all preferences of user
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "Resource.h"
namespace viewbar
{
	class CPreferences
	{
	public:

		CPreferences();
		~CPreferences();

		// getter
		static CString	GetProxyIp()					{return m_strProxyIp;}				// proxy settings
		static CString	GetProxyPort()					{return m_strProxyPort;}
		static CString	GetProxyUser()					{return m_strProxyUsername;}
		static CString	GetProxyPassword()				{return m_strProxyPassword;}
		static bool		IsProxyEnable()					{return m_bIsProxyEnable;}
		static CString	GetWebServiceURL()				{return m_strWebServiceURL;}
		static CString	GetWebServicePath()				{return m_strWebServicePath;}
		static CString	GetUpdateFlag()					{return m_strUpdateFlag;}
		static CString	GetVersion()					{return m_strVersion;}


		static void LoadConfig(const CString &cmd);

	private:
		// field
		static CString	m_strProxyIp;				// proxy settings
		static CString	m_strProxyPort;
		static CString	m_strProxyUsername;
		static CString	m_strProxyPassword;
		static bool		m_bIsProxyEnable;
		static CString	m_strWebServiceURL;
		static CString	m_strWebServicePath;
		static CString	m_strVersion;
		static CString	m_strUpdateFlag;
	};

	extern CPreferences thePrefs;
};