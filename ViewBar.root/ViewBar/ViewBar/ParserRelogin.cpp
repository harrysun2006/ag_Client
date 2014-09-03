#include "StdAfx.h"
#include "ParserRelogin.h"
#include "utility/XmlNodeWrapper.h"

using namespace utility;

namespace http
{

	CParserRelogin::CParserRelogin(void)
	{
	}

	CParserRelogin::CParserRelogin(const CString&xml) :
		m_xml(xml), m_bLoginSuccess(NULL)
	{
		Parse();
	}

	CParserRelogin::~CParserRelogin(void)
	{
	}

	void CParserRelogin::Parse()
	{
		CXmlDocumentWrapper xml;
		xml.LoadXML(m_xml);

		CXmlNodeWrapper theNode(xml.AsNode());
		CString strResult = theNode.GetValue(_T("result"));
		if (strResult == _VB_SUCCESS)
			m_bLoginSuccess = TRUE;
		else
			m_bLoginSuccess = FALSE;

	}

	BOOL CParserRelogin::IsLogin(CString &reason)
	{
		return m_bLoginSuccess;
	}

}
