#pragma once

#define SERVICE_URLTRACK "urlTrackService"
#define NUMBER_URLSEND	10

class CUrlTrack
{
private:
	CUrlTrack(void);
public:
	~CUrlTrack(void);
	static CUrlTrack* GetInstance();
	static void FreeInstance();
public:
	BOOL UTStartSniffer(int major, int minor);
	void UTEndSniffer();

	void Send(BOOL bDestroy = FALSE);
	void LoadURLInLocale();
	void StoreURLToDataBase();
private:
	void UrlTrackProcedure();
	void FlushURLInLocale();
	void ReStoreURLInLocale();
	BOOL ParseResponse(const CString &response);
private:
	static CUrlTrack* m_pInstance;
	static DWORD WINAPI SendUrlTrackProc(LPVOID lpParameter);
private:
	std::string m_sendxml;
	int m_nNumSend;
	//CString m_sendXML;
	BOOL m_bSniffering;
	const CString m_strKey;
	int m_nID[NUMBER_URLSEND];
	void CleanIDArray()
	{
		for( int i = 0; i < NUMBER_URLSEND; i++ )
			m_nID[i] = -1;
	};
	void CreateSendXML();
};
