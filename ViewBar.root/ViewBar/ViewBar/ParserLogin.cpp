#include "StdAfx.h"
#include "ParserLogin.h"
#include "utility/XmlNodeWrapper.h"
#include "SearchManager.h"
#include "AglocoFavorites.h"
#include "AdsManager.h"

using namespace utility;

namespace http
{
	CParserLogin::CParserLogin(const CString&xml) :
			m_xml(xml), m_bLoginSuccess(NULL)
	{
		Parse();
	}

	CParserLogin::~CParserLogin(void)
	{
	}

	void CParserLogin::Parse()				
 	{
		CXmlDocumentWrapper xml;
		xml.LoadXML(m_xml);

#ifdef DEBUG
		g_log.WriteLog(m_xml);
#endif

		CXmlNodeWrapper theNode(xml.AsNode());
		CString strResult = theNode.GetValue(_T("result"));
		if (strResult == _VB_SUCCESS)
			m_bLoginSuccess = TRUE;
		else
		{
			m_bLoginSuccess = FALSE;
			return;
		}

		CString strServerTime = theNode.GetValue(_T("serverTime"));
		if( 0 == thePrefs.GetServerTime() )
		{
			int nServerTime = _ttoi(strServerTime);
			thePrefs.SetServerTime(nServerTime);
		}
		strServerTime += _T(" Login ServerTime");
		g_log.OutputDebug(strServerTime);

		CString strMemberId = theNode.GetValue(_T("memberId"));// added by victor 4/6-07
		thePrefs.SetMemberId(_ttoi(strMemberId));
		CString strNextSend = theNode.GetValue(_T("nextSubmitTime"));
		thePrefs.SetNextSendTime(_ttoi(strNextSend) );

		// search engine list
		CXmlNodeWrapper nodeSearchEngine(theNode.GetNode(_T("engines")));
		GetSearchEngineList(nodeSearchEngine.GetXML());

		// save favorites
		CXmlNodeWrapper nodeFavorites(theNode.GetNode(_T("favorites")));
		CAglocoFavorites fav;
		fav.SaveFavorites(nodeFavorites.GetXML());

		 //template and javascript
		 CAdsManager::GetInstance()->SetJavaScriptName(theNode.GetValue(_T("ads-jsname")));
		 CAdsManager::GetInstance()->SetTemplateName(theNode.GetValue(_T("ads-basename")));


		// breeze
		 //CAdsManager::GetInstance()->SetJavaScriptName(_T("footer.js"));
		 //CAdsManager::GetInstance()->SetTemplateName(_T("ads_ask"));

		 CAdsManager::GetInstance()->DownloadAdTemplate();

	}

	BOOL CParserLogin::IsLogin(CString &reason)
	{
		return m_bLoginSuccess;
	}

	BOOL CParserLogin::GetSearchEngineList(const CString &engineList)
	{
		if (engineList.IsEmpty())
			return FALSE;

		CXmlDocumentWrapper xmlDoc;
		xmlDoc.LoadXML(engineList);
		
		CXmlNodeWrapper root(xmlDoc.AsNode());
		CXmlNodeListWrapper nodes(root.FindNodes(_T("engine")));
		std::list<CSearchEngine*> listSearchEngine;
		for (int i=0; i<nodes.Count(); ++i)
		{
			CXmlNodeWrapper node = nodes.Node(i);

			// memory will be freed in CSearchManager
			CSearchEngine *pNode = new CSearchEngine;
			pNode->m_iSearchEngineId		= _ttoi(node.GetValue(_T("engineId")));
			pNode->m_strSearchEngineName	= node.GetValue(_T("name"));
			pNode->m_strSearchEnginePattern	= node.GetValue(_T("pattern"));
			pNode->m_strSearchEngineURL		= node.GetValue(_T("url"));
			listSearchEngine.push_back(pNode);
		}
		CSearchManager::GetInstance()->SetSearchEngineList(listSearchEngine);

		return TRUE;
	}
} 