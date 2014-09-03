#include "stdafx.h"
#include "SurfTime.h"
#include "XSendSurfTime.h"
#include "CppSQLiteManager.h"
#include "utility/XmlNodeWrapper.h"

#include ".\utility\des\MyAes.h"
#include "MyUtility.h"

using namespace http;
using namespace utility;
using namespace viewbar;

#define RES_NEXTTIME	"nextTime="

CSurfTime::CSurfTime() : m_strKey(_T("send surf time key"))
{
	InitializeCriticalSection(&m_csList);
	m_sendXML = _T("");
//	m_ServerTime = 0;
	m_nTick = 0;
}

CSurfTime::~CSurfTime()
{
	DeleteCriticalSection(&m_csList);
}

void CSurfTime::AddSurfTime(int start,int end,int mId, BOOL bSend /*= FALSE*/)
{
	SURFTIME st;
	st.bSend = bSend;
	st.start = start + thePrefs.GetServerTime();
	st.end = end + thePrefs.GetServerTime();
	st.MemId = mId;
	st.subID = 0;
	g_log.OutputDebug(_T("AddSurfTime start="),st.start);
	AddSurfTime(st);
}
void CSurfTime::AddSurfTime(SURFTIME st)
{
	EnterCriticalSection(&m_csList);
	m_listTime.AddTail(st);
	LeaveCriticalSection(&m_csList);
}

void CSurfTime::Send(BOOL bDestroy /*= FALSE*/)
{
	EnterCriticalSection(&m_csList);
	m_sendXML = _T("");
	int nSubTime = thePrefs.GetServerTime() + GetCurrentSecond();
	g_log.OutputDebug(_T("Send Time: SubTime="),nSubTime);
	POSITION pos = m_listTime.GetHeadPosition();
	if( pos == NULL )
	{
		m_sendXML = _T("");
	}
	else
	{
		SURFTIME stHead;
		while(pos)
		{
			stHead = m_listTime.GetAt(pos);
			if( stHead.bSend != TRUE )
			{
				break;
			}
			m_listTime.GetNext(pos);
		}
		if( pos == NULL )
		{
			LeaveCriticalSection(&m_csList);
			return;
		}

		if( stHead.subID != 0 )
		{
			nSubTime = stHead.subID;
		}

		int nCount = 0;
		SURFTIME stCur;
		CString ItemList = _T("");
		POSITION posCur;
		pos = m_listTime.GetHeadPosition();
		while(pos)
		{
			posCur = pos;
			stCur = m_listTime.GetNext(pos);
			if( stCur.bSend == TRUE )
				continue;
			if( stCur.subID != stHead.subID )
				continue;
			stCur.subID = nSubTime;
			stCur.bSend = TRUE;
			m_listTime.SetAt(posCur,stCur);
			stCur = m_listTime.GetNext(posCur);
			nCount++;

			CString Item;
			Item.Format(
				_T("<item ")
				_T("beginPoint=\"%d\" ")
				_T("endPoint=\"%d\" ")
				_T("point=\"%f\" ")
				_T("status=\"%d\" ")
				_T("xsi:type=\"timeTrack\"/>")
				,stCur.start,stCur.end,0.0f,1);
			ItemList += Item;
			//ATLTRACE("subID[%d]-[%d]\n",stCur.subID,thePrefs.GetServerTime());
		}
		if( nCount > 0 )
		{
			CString Header;
			Header.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><timeList memberId=\"%d\" submitId=\"%d\">"),thePrefs.GetMemberId(),nSubTime);
			m_sendXML = Header + ItemList + _T("</timeList>");
		}
	}

	g_log.OutputDebug(m_sendXML);
	LeaveCriticalSection(&m_csList);

	if(bDestroy)
	{
		TimeProcedure(bDestroy);
	}
	else
		CreateThread(NULL,0,SendSurfTimeProc,this,0,0);
}

DWORD CSurfTime::SendSurfTimeProc(LPVOID lpParameter)
{
	CSurfTime* pThis = (CSurfTime*)lpParameter;
	pThis->TimeProcedure();
	return 0;
}

void CSurfTime::FlushTimeList(int nSurfID)
{
	if( nSurfID == 0 )
		return;
	EnterCriticalSection(&m_csList);
	POSITION pos = m_listTime.GetHeadPosition();
	POSITION posDelete;
	while( pos != NULL )
	{
		posDelete = pos;
		SURFTIME st = m_listTime.GetNext(pos);
		if( st.subID == nSurfID )
		{
			g_log.OutputDebug(_T("FlushTimeList "),st.subID);
			m_listTime.RemoveAt(posDelete);
		}
	}
	LeaveCriticalSection(&m_csList);
}

void CSurfTime::TimeProcedure(BOOL bDestroy /*= FALSE*/)
{
	CoInitialize(NULL);
	CXSendSurfTime SendSurfTime(m_sendXML);
	if( m_sendXML == _T("") )
		return;
	CString strResponse = _T("");
	if( bDestroy )
	{
		if( SendSurfTime.Request(_T(SERVICE_SRUFTIME),strResponse) )
		{
			thePrefs.SetNetState(TRUE);
			int nSurfID = 0;
			ParseResponse(strResponse,nSurfID);
			FlushTimeList(nSurfID);
			StoreSurfTimeInLocale();
		}
		else
		{
			thePrefs.SetNetState(FALSE);
			StoreSurfTimeInLocale();
		}
	}
	else
	{
		if( SendSurfTime.Request(_T(SERVICE_SRUFTIME),strResponse) )
		{
			g_log.OutputDebug(_T("Sending Surfing Time..."));
			g_log.OutputDebug(strResponse);

			thePrefs.SetNetState(TRUE);
			int nSurfID = 0;
			if( ParseResponse(strResponse,nSurfID) )
			{
				FlushTimeList(nSurfID);
				Send(FALSE);
			}
		}
		else
		{
			thePrefs.SetNetState(FALSE);
		}
	}

	CoUninitialize();
}

void CSurfTime::LoadStoredSurfTime()
{
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	char chCondition[128];
	sprintf(chCondition,"where MemID='%d'",thePrefs.GetMemberId());
	SQLite->STExecTableQuery(TABLE_SURFTIME, "select distinct * ",chCondition );
	int nRows = SQLite->STGetRowsNumber();
	SURFTIME st;
	st.MemId = thePrefs.GetMemberId();
	st.bSend = FALSE;
	CMyCrypt myAES;
	for( int i = 0; i < nRows; i++ )
	{		
		CStringA test;
		test.Format("%s",SQLite->STGetStringValue(i,1));
		CString strIn = CMyUtility::GetCStringW(test);

		CString strOut;
		myAES.Decrypt(m_strKey,strIn,strOut);

		int nPos1 = strOut.Find(_T(","));
		int nPos2 = strOut.Find(_T(','),nPos1+2);
		int nPos3 = strOut.ReverseFind(_T(','));

		CString strID = strOut.Left(nPos1);
		CString strTime1 = strOut.Mid(nPos1+1,nPos2-nPos1-1);
		CString strTime2 = strOut.Mid(nPos2+1,nPos3-nPos2-1);
		CString strSubID = strOut.Right(strOut.GetLength()-nPos3-1);

		strOut += " LoadStoredSurfTime";
		g_log.OutputDebug(strOut);

		st.MemId = _ttoi(strID);
		if( thePrefs.GetMemberId() != st.MemId )
			continue;
		st.start = _ttoi(strTime1);
		st.end = _ttoi(strTime2);
		st.subID = _ttoi(strSubID);
		
		AddSurfTime(st);
	}
	SQLite->DeleteItem(TABLE_SURFTIME,chCondition);
}

void CSurfTime::StoreSurfTimeInLocale()
{
	g_log.OutputDebug(_T("StoreSurfTimeInLocale..."));

	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	POSITION pos = m_listTime.GetHeadPosition();
	CMyCrypt myAES;
	char szValue[128];
	sprintf(szValue,"%d,'",thePrefs.GetMemberId());
	int atValue = strlen(szValue);
	int nCurSecond = GetCurrentSecond();
	while( pos )
	{
		CString strIn,strOut;
		SURFTIME st = m_listTime.GetNext(pos);
		if( st.subID == 0 )
			st.subID = thePrefs.GetServerTime() + nCurSecond;
		strIn.Format(_T("%d,%d,%d,%d"),st.MemId,st.start,st.end,st.subID);
		myAES.Encrypt(m_strKey,strIn,strOut);
		CStringA strA = CMyUtility::GetCStringA(strOut);

		strIn += " Store";
		g_log.OutputDebug(strIn);

		sprintf(szValue+atValue,"%s'",strA);
		SQLite->InsertItem(TABLE_SURFTIME,szValue);
	}
}

BOOL CSurfTime::ParseResponse(const CString &strResponse,int &nSurID)
{
	CXmlDocumentWrapper docXML;
	docXML.LoadXML(strResponse);

	CXmlNodeWrapper nodeXML;
	nodeXML = docXML.AsNode();
	CString strResult = nodeXML.GetValue(_T("result"));
	
	CString strNextTime = nodeXML.GetValue(_T("nextTime"));
	SetNextTime(_ttoi(strNextTime));

	CString strSubID = nodeXML.GetValue(_T("submitId"));
	nSurID = _ttoi(strSubID);


	if( strResult == _VB_SUCCESS )
		return TRUE;
	if( strResult == _VB_BAD_REQUEST )
		return TRUE;
	if( strResult == _VB_OTHER_PLACE_LOGIN )
		return TRUE;
	if( strResult == _VB_DUPLICATE_RECORD )
		return TRUE;

	return FALSE;
	
	nSurID = 0;
	return FALSE;
}

void CSurfTime::SetNextTime(DWORD NextTime)
{
	if( NextTime == 0 )
		return;
 	thePrefs.SetNextSendTime(NextTime);
}

DWORD CSurfTime::GetCurrentSecond()
{
	return g_HiTimer.GetTotalLapseSeconds();
}

void CSurfTime::TestNetState()
{
	if( m_nTick < 20 )
	{
		m_nTick++;
	}
	else
	{
		m_nTick = 0;
		CreateThread(NULL,0,SendTestRequest,this,0,0);
	}
}

DWORD CSurfTime::SendTestRequest(LPVOID lpParameter)
{
	CSurfTime* pThis = (CSurfTime*)lpParameter;
	pThis->TestRequest();
	return 0;
}

void CSurfTime::TestRequest()
{
	CoInitialize(NULL);
	CString strTest = _T("");
	strTest.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
		_T("<timeList memberId=\"%d\" submitId=\"0\">")
		_T("</timeList>"),thePrefs.GetMemberId());

	CXSendSurfTime SendSurfTime(strTest);
	CString strResponse = _T("");
	if( SendSurfTime.Request(_T(SERVICE_SRUFTIME),strResponse) )
	{
		thePrefs.SetNetState(TRUE);
	}
	CoUninitialize();
}