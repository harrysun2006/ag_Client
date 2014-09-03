#include "StdAfx.h"
#include "UrlTrack.h"
#include "XSendURLTrack.h"
#include "CppSQLiteManager.h"
#include "MyUtility.h"
#include "utility/XmlNodeWrapper.h"

#include "..\VBSniffer\VBSniffer.h"

using namespace http;
using namespace viewbar;
using namespace utility;

CUrlTrack* CUrlTrack::m_pInstance = NULL;

CUrlTrack::CUrlTrack(void) : m_strKey(_T("send URL track key"))
{
//	m_sendXML = _T("");
	m_sendxml = "";
	m_bSniffering = FALSE;
	m_nNumSend = 0;
	CleanIDArray();
}

CUrlTrack::~CUrlTrack(void)
{
	if( m_bSniffering )
		EndSniffer();
}


CUrlTrack* CUrlTrack::GetInstance()
{
	if( NULL == m_pInstance )
		m_pInstance = new CUrlTrack();
	return m_pInstance;
}

void CUrlTrack::FreeInstance()
{
	if( m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

BOOL CUrlTrack::UTStartSniffer(int major, int minor)
{
	m_bSniffering = StartSniffer(major,minor);
	return m_bSniffering;
}

void CUrlTrack::UTEndSniffer()
{
	m_bSniffering = FALSE;
	EndSniffer();
}

void CUrlTrack::CreateSendXML()
{
	m_sendxml = "";
	m_nNumSend = 0;

	std::string itemlist = "";
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	if( SQLite )
	{
		char chCondition[128];
		sprintf(chCondition,"where MemID='%d' and Send='0' ",thePrefs.GetMemberId());
		SQLite->UTExecTableQuery(TABLE_URLTRACK, "select * ",chCondition );
		int nRows = SQLite->UTGetRowsNumber();
		nRows = nRows > NUMBER_URLSEND ? NUMBER_URLSEND : nRows;
		for( int i = 0; i < nRows; i++ )
		{
			std::string item;
			item = "<item domainName=\"";
			item += "";
			item += "\" ";
			item += "completeURL=\"";
			item += SQLite->UTGetStringValue(i,3);
			item += "\" ";
			item += "title=\"";
			item += SQLite->UTGetStringValue(i,4);
			item += "\" ";
			item += "keywords=\"\" ";
			item += "errorDescription=\"\" ";
			item += "errorStatus=\"\" ";
			item += "browser=\"\" ";
			item += "description=\"\" />";
			itemlist += item;
			m_nID[m_nNumSend] = SQLite->UTGetIntValue(i,6);
			m_nNumSend++;
		}
		if( m_nNumSend > 0 )
		{

			CString szContent;
			szContent = _T("set Send='1' where ");
			for( int i = 0; i < m_nNumSend - 1; i++ )
			{
				CString szTemp;
				szTemp.Format(_T("ID='%d' or "),m_nID[i]);
				szContent += szTemp;
			}
			CString szLast;
			szLast.Format(_T("ID='%d' "),m_nID[m_nNumSend-1]);
			szContent += szLast;
			SQLite->UpdateItem(TABLE_URLTRACK,szContent);
		}
	}

	if( m_nNumSend > 0 )
	{
		char chTemp[64];
		sprintf(chTemp,"%d",thePrefs.GetMemberId());
		m_sendxml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><urlList memberId=\"";
		m_sendxml += chTemp;
		m_sendxml += "\">";
		m_sendxml += itemlist;
		m_sendxml += "</urlList>";
	}
}
void CUrlTrack::Send(BOOL bDestroy /*= FALSE*/)
{
	// First end sniffer
	if( bDestroy )
	{		
		UTEndSniffer();
		StoreURLToDataBase();
		CreateSendXML();
		UrlTrackProcedure();
	}
	else
	{
		StoreURLToDataBase();
		CreateSendXML();
		CreateThread(NULL,0,SendUrlTrackProc,this,0,0);
	}
}

DWORD CUrlTrack::SendUrlTrackProc(LPVOID lpParameter)
{
	CUrlTrack* pThis = (CUrlTrack*)lpParameter;
	pThis->UrlTrackProcedure();
	return 0;
}

void CUrlTrack::UrlTrackProcedure()
{
	CString strOut;
	//strOut.Format(_T("URL's size is: %d"),m_nNumSend);
	//g_log.OutputDebug(strOut);

	CoInitialize(NULL);
	CXSendURLTrack SendURLTrack(m_sendxml);
	if( m_nNumSend <= 0 )
		return;
	m_nNumSend = 0;
	CString strResponse = _T("");
	if( SendURLTrack.RequestURL(_T(SERVICE_URLTRACK),strResponse) )
	{
		thePrefs.SetNetState(TRUE);
		if( !ParseResponse(strResponse) )
			ReStoreURLInLocale();
		else
			FlushURLInLocale();
	}
	else
	{
		thePrefs.SetNetState(FALSE);
		ReStoreURLInLocale();
	}
	CoUninitialize();
}

void CUrlTrack::FlushURLInLocale()
{
	if( m_nNumSend <= 0 )
		return;
	char szContent[256];
	strcpy(szContent,"where ");
	int len = 0;
	for( int i = 0; i < m_nNumSend - 1; i++ )
	{
		int len = strlen(szContent);
		sprintf(szContent+len,"ID='%d' or ",m_nID[i]);
	}
	len = strlen(szContent);
	sprintf(szContent+len,"ID='%d' ",m_nID[m_nNumSend-1]);
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	SQLite->DeleteItem(TABLE_URLTRACK,szContent);

	CleanIDArray();
}

void CUrlTrack::ReStoreURLInLocale()
{	
	CString szContent;
	szContent = _T("set Send='1' where ");
	for( int i = 0; i < m_nNumSend - 1; i++ )
	{
		CString szTemp;
		szTemp.Format(_T("ID='%d' or "),m_nID[i]);
		szContent += szTemp;
	}
	CString szLast;
	szLast.Format(_T("ID='%d' "),m_nID[m_nNumSend-1]);
	szContent += szLast;
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	SQLite->UpdateItem(TABLE_URLTRACK,szContent);

	CleanIDArray();
}

void CUrlTrack::LoadURLInLocale()
{/*
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();

	char chCondition[128];
	sprintf(chCondition,"where MemID='%d'",thePrefs.GetMemberId());
	SQLite->UTExecTableQuery(TABLE_URLTRACK, "select * ",chCondition );
	int nRows = SQLite->UTGetRowsNumber();
	for( int i = 0; i < nRows; i++ )
	{
		PConnect_HTTP pconHttp = new Connect_HTTP;
		pconHttp->bSended = FALSE;
		strcpy(pconHttp->host,SQLite->UTGetStringValue(i,3));
		strcpy(pconHttp->title,SQLite->UTGetStringValue(i,4));
		AddTail(pconHttp);
	}*/
}

BOOL CUrlTrack::ParseResponse(const CString &response)
{
	CXmlDocumentWrapper xmlDoc;
	xmlDoc.LoadXML(response);
	CXmlNodeWrapper node(xmlDoc.AsNode());
	CString strRtn = node.GetAttribVal(0);
	if( strRtn == _VB_SUCCESS )
		return TRUE;
	if( strRtn == _VB_BAD_REQUEST )
		return TRUE;
	if( strRtn == _VB_OTHER_PLACE_LOGIN )
		return TRUE;
	if( strRtn == _VB_DUPLICATE_RECORD )
		return TRUE;
	return FALSE;
}

void CUrlTrack::StoreURLToDataBase()
{
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	if( NULL == SQLite )
		return;

	char szValue[2048];
	sprintf(szValue,"%d,0,0,",thePrefs.GetMemberId());
	int atValue = strlen(szValue);

	PConnect_HTTP pConnHttp;
	EnterConnectCS();
	POSITION pos = GetHeadPosition();
	POSITION posDelete;
	while( pos )
	{
		posDelete = pos;
		pConnHttp = GetNext(pos);
		if( pConnHttp->bRespond )
		{
			sprintf(szValue+atValue, "'%s','%s','%s'", pConnHttp->host, pConnHttp->title, " ");
			SQLite->UTInsertItem(szValue);
			if( pConnHttp )
				delete pConnHttp;
			RemoveAt(posDelete);
		}
	}
	LeaveConnectCS();
}