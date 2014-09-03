///////////////////////////////////////////
#pragma once
#include "HiTimer.h"

#define SERVICE_SRUFTIME "timeTrackService"
typedef struct tagSURFTIME
{
	int bSend;
	int start;
	int end;
	int MemId;
	int subID;
} SURFTIME, *PSURFTIME;

class CSurfTime
{
public:
	CSurfTime();
	~CSurfTime();

	void AddSurfTime(int,int,int mId,BOOL bSend = FALSE);
	void AddSurfTime(SURFTIME st);
	void Send(BOOL bDestroy);// = FALSE);
	void LoadStoredSurfTime();
	//void SetServerTime(DWORD serverTime){m_ServerTime=serverTime;};
	void SetNextTime(DWORD NextTime);

	// test net state, send null surftime request
	void TestNetState();
private:
	static DWORD WINAPI SendSurfTimeProc(LPVOID lpParameter);
	void FlushTimeList(int nSurfID);
	void TimeProcedure(BOOL bDestroy = FALSE);
	DWORD GetCurrentSecond();

	static DWORD WINAPI SendTestRequest(LPVOID lpParameter);
	void TestRequest();
public:
	void StoreSurfTimeInLocale();

	BOOL ParseResponse(const CString &strResponse,int &nSurID);

private:
	CList<SURFTIME,SURFTIME&>m_listTime;
	CRITICAL_SECTION m_csList;
	CString m_sendXML;
	//DWORD m_ServerTime;
	const CString m_strKey;
	int m_nTick;
};