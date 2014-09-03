#include "StdAfx.h"
#include "XAdsUpdate.h"

#include "utility/XmlNodeWrapper.h"
using namespace utility;

namespace http
{

	CXAdsUpdate::CXAdsUpdate(const CString &strKeyworkd):m_strKeyword(strKeyworkd)
	{
	}

	CXAdsUpdate::~CXAdsUpdate(void)
	{
	}

	void CXAdsUpdate::FillRequest()
	{
	//	<?xml version="1.0" encoding="UTF-8"?>
	//		<searchKeywordReq memberId="1" searchEngineId="2" browser="ie6"><searchKeywords>car</searchKeywords><searchKeywords>news</searchKeywords><searchKeywords>nba</searchKeywords><adKeywords>car</adKeywords><adKeywords>nba</adKeywords></searchKeywordReq>

		CXmlDocumentWrapper xmlDoc;
		xmlDoc.LoadXML(_T("<searchKeywordReq />"));
		CXmlNodeWrapper node(xmlDoc.AsNode());
		CString id;
		id.Format(_T("%d"), thePrefs.GetMemberId());
		node.SetValue(_T("memberId"), id);
		CString strEngineId;
		strEngineId.Format(_T("%d"), thePrefs.GetDefaultSearchEngine());
		node.SetValue(_T("searchEngineId"), strEngineId);
		node.SetValue(_T("browser"), thePrefs.GetDefaultBrowserName());
		node.SetValue(_T("needAdvertise"), _T("true"));

		CXmlNodeWrapper nodeKeyword(node.InsertNode(node.NumNodes(), _T("keywords")));
		nodeKeyword.SetText(m_strKeyword);


		m_strKeyword.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n%s"),
			node.GetXML());


		CBiz::FillRequest(m_strKeyword);
	}

	void CXAdsUpdate::DownloadAds(const CString &strKeyword, CString &strResponse)
	{
		CBiz *pBiz = new CXAdsUpdate(strKeyword);
		pBiz->Request(_T("searchKeywordsService"), strResponse);
		delete pBiz;
	}
}
