#pragma once
#include <Winsock2.h>
#include "VBSniffer.h"

#define TCP_PACKET 6
typedef struct tagIP_HEADER
{
	unsigned char len : 4;
	unsigned char ver : 4;
	unsigned char tos;
	unsigned short total_len;
	unsigned short id;
	unsigned short frag_flags;
	unsigned char ttl;
	unsigned char proto;
	unsigned short chksum;
	unsigned int srcip;
	unsigned int desip;

} IP_HEADER, *PIP_HEADER;

typedef struct tagTCP_HEADER
{
	unsigned short SourPort;
	unsigned short DestPort;
	unsigned int SeqNo;
	unsigned int AckNo;
	unsigned char HLen;
	unsigned char Flag;
	unsigned short Window;
	unsigned short ChkSum;
	unsigned short UrgPtr;
} TCP_HEADER, *PTCP_HEADER;

class CSnifferSocket
{
public:
	CSnifferSocket(void);
	~CSnifferSocket(void);

	BOOL LoadSockLib(int nMajorVersion, int nMinorVersion);
	int ReleaseSockLib(void);
	
	BOOL CreateSocket(void);
	int CloseSocket(void);

	BOOL StartSniffer(void);
	void EndSniffer(BOOL bEnd = TRUE);
	BOOL IsEndsniffer();

	int ReceivePacket(char* buf,int len);

	void EnterConnectCS();
	void LeaveConnectCS();
	void RemoveAllConn();
	void AddAffiliateList(HWND hwnd);
public:
	static DWORD WINAPI SnifferThreadProc(LPVOID lpParamter);
private:
	void ParseRequestTcp(char* buffer,int len,PIP_HEADER pIP);
	void ParseRespondTcp(char* buffer,int len,PIP_HEADER pIP);
	BOOL IsSendData(DWORD dwIP);
	BOOL IsRecvData(DWORD dwIP);

	void AddConnect(u_short dport,u_short sPort, u_long ip,CHAR* host,int lenHost,CHAR* url, int lenUrl);
	BOOL IsExistInList(PConnect_HTTP phttp,POSITION &posElem);

	void CopyCharacters(char* chDest,const char* chSour, int &len);
	

	void TimerProc();

	void CompareHostName(const char* host);
	void MyStrTrim(char* host);
private:
	BOOL m_bLibraryLoaded;
	SOCKET m_socket;
	HANDLE m_hThread;
	DWORD m_LocalIP;
	BOOL m_EndSniffer;
	CRITICAL_SECTION m_section;
	CRITICAL_SECTION m_sectionConn;
	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_LastCounter;
	HWND m_hWnd;
public:
	CList<PConnect_HTTP> m_listCon;
};
