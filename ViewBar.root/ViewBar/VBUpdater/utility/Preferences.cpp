#include "..\StdAfx.h"
#include "Preferences.h"
#include "XmlNodeWrapper.h"

CPreferences thePrefs;

CString CPreferences::m_strProxyIp;		// proxy
CString CPreferences::m_strProxyPort;
CString CPreferences::m_strProxyUsername;
CString CPreferences::m_strProxyPassword;
bool	CPreferences::m_bIsProxyEnable = false;
CString	CPreferences::m_strWebServiceURL = _T("157.22.244.29:8443");	// ip:port
CString CPreferences::m_strWebServicePath = _T("viewbar/servlet");		// /servlet
CString CPreferences::m_strVersion;
CString CPreferences::m_strUpdateFlag;

namespace viewbar
{

	CPreferences::CPreferences(void)
	{
	}

	CPreferences::~CPreferences(void)
	{
	}

	// general preferences
	void CPreferences::LoadConfig(const CString &cmd)
	{
	
		utility::CXmlDocumentWrapper xmlDoc;
		xmlDoc.LoadXML(cmd);

		utility::CXmlNodeWrapper root(xmlDoc.AsNode());

		m_strProxyIp = root.GetValue(_T("proxyIp"));
		m_strProxyPort = root.GetValue(_T("proxyPort"));
		m_strVersion = root.GetValue(_T("version"));
		m_strWebServicePath = root.GetValue(_T("WSPath"));
		m_strWebServiceURL = root.GetValue(_T("WS"));
		m_strUpdateFlag = root.GetValue(_T("update"));
	}

};