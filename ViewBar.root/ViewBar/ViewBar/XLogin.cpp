#include "StdAfx.h"
#include "XLogin.h"
#include "utility/XmlNodeWrapper.h"
#include "ParserLogin.h"
#include "SearchManager.h"
#include "AglocoFavorites.h"
#include "MyUtility.h"

using namespace utility;
using namespace viewbar;
namespace http
{
	CXLogin::CXLogin(const CString &xml):m_strXML(xml)
	{
	}

	CXLogin::~CXLogin(void)
	{
	}

	void CXLogin::FillRequest()
	{
		CBiz::FillRequest(m_strXML);
	}

	BOOL CXLogin::Login(const CString &username, const CString &password, CString &response)
	{
		CString xml;
	//	<?xml version="1.0" encoding="UTF-8" ?> 
	//	.98.1.0.1" searchEngzhao" domainName="amaxgs.local" osVersion="windows.2k" /> 
		g_log.OutputDebug(_T("Login Request ") + username + _T("-") + CMyUtility::GetIP() );

		CXmlDocumentWrapper xmlDoc;
		xmlDoc.LoadXML(_T("<loginReq />"));
		CXmlNodeWrapper node(xmlDoc.AsNode());
		node.SetValue(_T("userName"), username);
		node.SetValue(_T("password"), password);
		node.SetValue(_T("viewbarId"), thePrefs.GetVersion());
		node.SetValue(_T("searchEngineHash"), CSearchManager::GetInstance()->GetEngineHashCode());
		CAglocoFavorites fav;
		node.SetValue(_T("favoriteHash"), fav.GetFavHashCode());
		node.SetValue(_T("ipAddr"), CMyUtility::GetIP());
		node.SetValue(_T("macAddr"), CMyUtility::GetMAC());
		node.SetValue(_T("computerName"), CMyUtility::GetMachineName());
		node.SetValue(_T("domainName"), CMyUtility::GetDomainName());
		node.SetValue(_T("osVersion"), CMyUtility::GetOSStringVersion());

		xml = node.GetXML();
		CString strXML;
		strXML.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n%s"), xml);
		
		CBiz *pBiz = new CXLogin(strXML);

		CString ret;
		pBiz->Request(_T("loginService"), ret);
		g_log.OutputDebug(_T("Login Response ") + ret );

		delete pBiz;

		CParserLogin parser(ret);
		if (parser.IsLogin(response))
		{
			return TRUE;
		}

		return FALSE;
	}
}