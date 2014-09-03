#include "StdAfx.h"
#include "XRelogin.h"
#include "utility/XmlNodeWrapper.h"
#include "ParserRelogin.h"

using namespace utility;

namespace http
{

	CXRelogin::CXRelogin(void)
	{
	}

	CXRelogin::CXRelogin(const CString &xml): m_strXML(xml)
	{

	}

	CXRelogin::~CXRelogin(void)
	{
	}

	void CXRelogin::FillRequest()
	{
		CBiz::FillRequest(m_strXML);
	}

	BOOL CXRelogin::Login()
	{
 		CString xml;
 		//	<?xml version="1.0" encoding="UTF-8" ?> 
 		//	.98.1.0.1" searchEngzhao" domainName="amaxgs.local" osVersion="windows.2k" /> 
 
 		CXmlDocumentWrapper xmlDoc;
 		xmlDoc.LoadXML(_T("<loginReq />"));
 		CXmlNodeWrapper node(xmlDoc.AsNode());
 		node.SetValue(_T("userName"), thePrefs.GetUsername());
 		node.SetValue(_T("password"), thePrefs.GetPassword());
		node.SetValue(_T("viewbarId"), thePrefs.GetVersion());
 		xml = node.GetXML();
 		CString strXML;
 		strXML.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n%s"), xml);
 
 		CBiz *pBiz = new CXRelogin(strXML);
 
 		CString ret;
 		pBiz->Request(_T("autoLoginService"), ret);
 
 		delete pBiz;
 
		CString response;
 		CParserRelogin parser(ret);
 		if (parser.IsLogin(response))
 		{
 			return TRUE;
 		}
 
 		return FALSE;
	}
}
