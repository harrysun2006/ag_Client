#include "StdAfx.h"
#include "SnifferSocket.h"
#include <Mstcpip.h>
#include <Ws2tcpip.h>
#include <Nb30.h>

#include <string>
#include <atlrx.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Netapi32.lib")

typedef struct tagCodePage
{
	UINT cpID;
	char cpName[32];
} CODEPAGE;

CODEPAGE g_codepage[] =
{
	{936,"gb2312"},				{936,"GBK"},					{950,"big5"},					{37, "IBM037"},
	//{65000, "utf-7"}, {65001 "utf-8"},
	{65000, "utf-7"},			{0, "utf-8"},
	{437, "IBM437" },			{500, "IBM500" },				{708, "ASMO-708" },				{720, "DOS-720" },
	{737, "ibm737" },			{775, "ibm775" },				{850, "ibm850" },				{852, "ibm852" },
	{855, "IBM855" },			{857, "ibm857" },				{858, "IBM00858" },				{860, "IBM860" },
	{861, "ibm861" },			{862, "DOS-862" },				{863, "IBM863" },				{864,"IBM864" },
	{865, "IBM865" },			{866, "cp866" },				{869, "ibm869" },				{870, "IBM870" },
	{874, "windows-874" },		{875, "cp875" },				{932, "shift_jis" },			{936, "gb2312" },
	{949, "ks_c_5601-1987" },	{1026, "IBM1026" },				{1047, "IBM01047" },			{1140, "IBM01140" },
	{1141, "IBM01141" },		{1142, "IBM01142" },			{1143, "IBM01143" },			{1144, "IBM01144" },
	{1145, "IBM01145" },		{1146, "IBM01146" },			{1147, "IBM01147" },			{1148, "IBM01148" },
	{1149, "IBM01149" },		{1200, "utf-16" },				{1201, "unicodeFFFE" },			{1250, "windows-1250" },
	{1251, "windows-1251" },	{1252, "windows-1252" },		{1253, "windows-1253" },		{1254, "windows-1254" },
	{1255, "windows-1255" },	{1256, "windows-1256" },		{1257, "windows-1257" },		{1258, "windows-1258" },
	{1361, "Johab" },			{10000, "macintosh" },			{10001, "x-mac-japanese" },		{10002, "x-mac-chinesetrad" },
	{10003, "x-mac-korean" },	{10004, "x-mac-arabic" },		{10005, "x-mac-hebrew" },		{10006, "x-mac-greek" },
	{10007, "x-mac-cyrillic" },	{10008, "x-mac-chinesesimp" },	{10003, "x-mac-korean"},		{10004, "x-mac-arabic"},
	{10005, "x-mac-hebrew"},	{10006, "x-mac-greek"},			{10007, "x-mac-cyrillic"},		{10008, "x-mac-chinesesimp"},
	{10010, "x-mac-romanian"},	{10017, "x-mac-ukrainian"},		{10021, "x-mac-thai"},			{10029, "x-mac-ce"},
	{10079, "x-mac-icelandic"},	{10081, "x-mac-turkish"},		{10082, "x-mac-croatian"},		{12000, "utf-32"},
	{12001, "utf-32BE"},		{20000, "x-Chinese_CNS"},		{20001, "x-cp20001"},			{20002, "x_Chinese-Eten"},
	{20003, "x-cp20003"},		{20004, "x-cp20004"},			{20005, "x-cp20005"},			{20105, "x-IA5"},
	{20106, "x-IA5-German"},	{20107, "x-IA5-Swedish"},		{20108, "x-IA5-Norwegian"},		{20127, "us-ascii"},
	{20261, "x-cp20261"},		{20269, "x-cp20269"},			{20273, "IBM273"},				{20277, "IBM277"},
	{20278, "IBM278"},			{20280, "IBM280"},				{20284, "IBM284"},				{20285, "IBM285"},
	{20290, "IBM290"},			{20297, "IBM297"},				{20420, "IBM420"},				{20423, "IBM423"},
	{20424, "IBM424"},			{20833, "x-EBCDIC-KoreanExtended"},	{20838, "IBM-Thai"},		{20866, "koi8-r"},
	{20871, "IBM871"},			{20880, "IBM880"},				{20905, "IBM905"},				{20924, "IBM00924"},
	{20932, "EUC-JP"},			{20936, "x-cp20936"},			{20949, "x-cp20949"},			{21025, "cp1025"},
	{21866, "koi8-u"},			{28591, "iso-8859-1"},			{28592, "iso-8859-2"},			{28593, "iso-8859-3"},
	{28594, "iso-8859-4"},		{28595, "iso-8859-5"},			{28596, "iso-8859-6"},			{28597, "iso-8859-7"},
	{28598, "iso-8859-8"},		{28599, "iso-8859-9"},			{28603, "iso-8859-13"},			{28605, "iso-8859-15"},
	{29001, "x-Europa"},		{38598, "iso-8859-8-i"},		{50220, "iso-2022-jp"},			{50221, "csISO2022JP"},
	{50222, "iso-2022-jp"},		{50225, "iso-2022-kr"},			{50227, "x-cp50227"},			{51932, "euc-jp"},
	{51936, "EUC-CN"},			{51949, "euc-kr"},				{52936, "hz-gb-2312"},			{57002, "x-iscii-de"},
	{57003, "x-iscii-be"},		{57004, "x-iscii-ta"},			{57005, "x-iscii-te"},			{57006, "x-iscii-as"},
	{57007, "x-iscii-or"},		{57008, "x-iscii-ka"},			{57009, "x-iscii-ma"},			{57010, "x-iscii-gu"},
	{57011, "x-iscii-pa"},
	};

const int NUM_CODEPAGE = sizeof(g_codepage) / sizeof(g_codepage[0]);

int CodePageID(const char * strCharset)
{
	if(NULL == strCharset)
		return 0;
	char* strFormate = NULL;
	for( int i = 0; i < NUM_CODEPAGE; i++ )
	{
		__try
		{
			strFormate =StrStrA(strCharset,g_codepage[i].cpName);
			if( strFormate )
			{
				if( ( strFormate - strCharset ) < 15 )
					return g_codepage[i].cpID;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return 0;
		}
	}
	return 0;
}

void ToUTF8(UINT codePage, char* text, int &len)
{
	if( len > MAX_STRING )
		len = MAX_STRING;
	TCHAR strUnicode[MAX_PATH];
	int k = MultiByteToWideChar(codePage,MB_PRECOMPOSED,text,len,strUnicode,MAX_PATH);
	if( k == 0 )
		return;
	strUnicode[k] = '\0';

	k = (int)_tcslen(strUnicode);
	k = WideCharToMultiByte(CP_UTF8,NULL, strUnicode,k,text,MAX_PATH,NULL,NULL);
	if( k == 0 )
		return;
	if( k > MAX_STRING )
		k = MAX_STRING;
	text[k] = '\0';
	len = k;
}
/////////
// UTF-8 [1,1,1,0,A5,A6,A7,A8][1,0,B3,B4,B5,B6,B7,B8][1,0,C3,C4,C5,C6,C7,C8]
// GB2312
// [B7,B8,C3,C4,C5,C6,C7,C8][A5,A6,A7,A8,B3,B4,B5,B6]
//////

// UTF-8 to Unicode
void UTF8ToUnicode(WCHAR* pOut, char * pText)
{
	char *uChar = (char*)pOut;
	uChar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uChar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
}

// Unicode to UTF8
void UnicodeToUTF8(char* pOut, WCHAR* pText)
{
	char* pchar = (char*)pText;
	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
}

// Unicode to GB2312
void UnicodeToGB2312(char* pOut,unsigned short uData)
{
	WideCharToMultiByte(CP_ACP,NULL,(LPCWSTR)&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
}

// GB2312 to Unicode
void GB2312ToUnicode(WCHAR* pOut,char *gbBuffer)
{
	MultiByteToWideChar(936,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
}


// GB2312 to UTF8
void GB2312ToUTF8(std::string &pOut,char *pText, int pLen)
{
	char buf[4];
	char* rst = new char[pLen + (pLen >> 2) + 2];
	memset(buf,0,4);
	memset(rst,0,pLen + (pLen >> 2) + 2);
	
	int i = 0;
	int j = 0;
	while(i < pLen)
	{
		// English just copy
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			WCHAR pbuffer;
			GB2312ToUnicode(&pbuffer,pText+i);
			UnicodeToUTF8(buf,&pbuffer);
			
			rst[j] = buf[0];
			rst[j+1] = buf[1];
			rst[j+2] = buf[2];
			
			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	pOut = rst;
	delete []rst;
	
}

//UTF-8 to GB2312
void UTF8ToGB2312(std::string &pOut, char *pText, int pLen)
{
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];
		}
		else
		{
			WCHAR Wtemp;
			UTF8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Ctemp,Wtemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;
			j += 2;
		}
	}
	newBuf[j] = '\0';

	pOut = newBuf;
	delete []newBuf;
}

/////////////////////////////////////////////////////////////////////////////
CSnifferSocket::CSnifferSocket(void) : m_socket(INVALID_SOCKET),
m_bLibraryLoaded(FALSE), m_hThread(NULL), m_EndSniffer(FALSE),m_hWnd(NULL)
{
	InitializeCriticalSection(&m_section);
	InitializeCriticalSection(&m_sectionConn);
}

CSnifferSocket::~CSnifferSocket(void)
{
	EnterCriticalSection(&m_sectionConn);
	RemoveAllConn();
	LeaveCriticalSection(&m_sectionConn);
	DeleteCriticalSection(&m_section);
	DeleteCriticalSection(&m_sectionConn);
}


BOOL CSnifferSocket::LoadSockLib(int nMajorVersion, int nMinorVersion)
{
	QueryPerformanceFrequency(&m_Frequency);
	WORD wVersionRequested = MAKEWORD(nMajorVersion,nMinorVersion);
	WSADATA wsaData;
	if( 0 != WSAStartup(wVersionRequested,&wsaData) )
		return FALSE;
	if( (LOBYTE(wsaData.wVersion) != nMinorVersion ) ||
		(HIBYTE(wsaData.wVersion) != nMajorVersion) )
	{
		WSACleanup();
		return FALSE;
	}
	m_bLibraryLoaded = TRUE;

	return TRUE;
}

int CSnifferSocket::ReleaseSockLib(void)
{
	if(m_bLibraryLoaded)
	{
		m_bLibraryLoaded = FALSE;
		return WSACleanup();
	}
	return 0;
}

int CSnifferSocket::CloseSocket(void)
{
	return closesocket(m_socket);
}

BOOL CSnifferSocket::CreateSocket(void)
{
	// Create a raw socket.
	m_socket = socket(AF_INET, SOCK_RAW,IPPROTO_IP);
	if( INVALID_SOCKET == m_socket )
	{
		return FALSE;
	}

	BOOL bFlag = TRUE;
	if( 0 != setsockopt(m_socket, IPPROTO_IP, IP_HDRINCL, (char *)&bFlag, sizeof(bFlag)))
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	char hostName[256];
	if( SOCKET_ERROR  == gethostname(hostName,256) )
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	hostent * host = gethostbyname(hostName);
	if( host == NULL )
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	SOCKADDR_IN addr_in;
	memset(&addr_in,0,sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	// random port
	srand((unsigned)time(NULL));
	UINT uPort= rand() % 10000 + 20000;
	addr_in.sin_port = htons(uPort);
	addr_in.sin_addr = *(in_addr*)host->h_addr;
	int i;
	for( i = 0; i < 3; i++ )
	{
		if( SOCKET_ERROR == bind(m_socket,(SOCKADDR*)&addr_in,sizeof(addr_in)) )
		{
			if( WSAEADDRINUSE == WSAGetLastError() )
				addr_in.sin_port = htons(++uPort);
			else
			{
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
				return FALSE;
			}
		}
		else
			break;
	}
	if( i > 2 )
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	m_LocalIP = addr_in.sin_addr.S_un.S_addr;

	// set SIO_RCVALL falg to receive all IP packets
	DWORD dwRcvAll = 1;
	ioctlsocket(m_socket,SIO_RCVALL,&dwRcvAll);
	
	return TRUE;
}

BOOL CSnifferSocket::StartSniffer(void)
{
	m_hThread = CreateThread(NULL,0,SnifferThreadProc,this,0,NULL);
	return TRUE;
}

DWORD WINAPI CSnifferSocket::SnifferThreadProc(LPVOID lpParamter)
{
	const int BUF_SIZE = 65535;
	char buffer[BUF_SIZE];
	CSnifferSocket* sniffersocket = (CSnifferSocket*)lpParamter;
	while(true)
	{
		if( sniffersocket->IsEndsniffer() )
			break;
		sniffersocket->TimerProc();
		memset(buffer,0,sizeof(buffer));
		int lenRecv = sniffersocket->ReceivePacket(buffer,BUF_SIZE-1);
		if( lenRecv > 0 )
		{
			buffer[lenRecv] = '\0';
			PIP_HEADER pIP = (PIP_HEADER)buffer;
			int lenIpHeader = pIP->len * 4;
			if( TCP_PACKET != pIP->proto )
				continue;
			
			PTCP_HEADER pTcp = (PTCP_HEADER)(buffer+lenIpHeader);
			if( sniffersocket->IsSendData(pIP->srcip) )
			{
				sniffersocket->ParseRequestTcp((char*)pTcp,lenRecv-lenIpHeader,pIP);
			}

			if( sniffersocket->IsRecvData(pIP->desip) )
			{
				sniffersocket->ParseRespondTcp((char*)pTcp,lenRecv-lenIpHeader,pIP);
			}
			
			/*in_addr a;
			a.S_un.S_addr = pIP->desip;
			char *addrchar = inet_ntoa(a);
			a.S_un.S_addr = pIP->srcip;
			addrchar = inet_ntoa(a);*/
		}
	}
	return 0;
}

int CSnifferSocket::ReceivePacket(char* buf,int len)
{
	return recv(m_socket,buf,len,0);
}

void CSnifferSocket::ParseRequestTcp(char* buffer,int len,PIP_HEADER pIP)
{
	PTCP_HEADER pTcp = (PTCP_HEADER)buffer;
	int lenTcp = ( pTcp->HLen >> 4 ) * 4;
	int nLenContent = len - lenTcp;

	if( nLenContent < 6 ) // number may be bigger, according to the format fo HTTP
		return;
	
	__try
	{
	char* content = buffer+(pTcp->HLen>>4)*4;
	
	char* str = StrStrA(content,"GET");
	if( NULL == str )
		return;
	str += 4;

	char* strUrlEnd = StrStrA(content," HTTP/");
	if( NULL == strUrlEnd )
		return;

	static char Url[256],host[256];
	int lenUrl = (int)(strUrlEnd - str);
	if( lenUrl > 256 )
		lenUrl = MAX_STRING;
	//TRACE("lenUrl[%d] nLenContent[%d]\n",lenUrl,nLenContent);
	memcpy(Url,str,lenUrl);
	Url[lenUrl] = '\0';
	MyStrTrim(Url);
	

	int lenHost = 0;
	char* curStr = StrStrA(strUrlEnd,"Host:");
	if( NULL != curStr )
	{
		curStr += 6;
		int nLeave = nLenContent - ( content-curStr);
		while( (*(curStr+lenHost) != '\r') && (lenHost < nLeave) )
		{
			lenHost++;
		}
		if( lenHost < nLeave )
		{
			if( lenHost >= 256 )
				lenHost = MAX_STRING;
			memcpy(host,curStr,lenHost);
			host[lenHost] = '\0';
			MyStrTrim(host);
		}
		else
		{
			lenHost = 0;
			host[0] = '\0';
		}
		AddConnect(ntohs(pTcp->DestPort),ntohs(pTcp->SourPort),pIP->desip,host,lenHost,Url,lenUrl);
	}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}
}

BOOL CSnifferSocket::IsSendData(DWORD dwIP)
{
	return( m_LocalIP == dwIP );
}

BOOL CSnifferSocket::IsRecvData(DWORD dwIP)
{
	return( m_LocalIP == dwIP );
}

void CSnifferSocket::ParseRespondTcp(char* buffer,int len,PIP_HEADER pIP)
{
	PTCP_HEADER pTcp = (PTCP_HEADER)buffer;
	int lenTcp = ( pTcp->HLen >> 4 ) * 4;
	int nLenContent = len - lenTcp;
	
	if( nLenContent < 6 ) // number may be bigger, according to the format fo HTTP
		return;

	Connect_HTTP http;
	http.SourPort = ntohs(pTcp->DestPort);
	http.DestPort = ntohs(pTcp->SourPort);
	http.IP = pIP->srcip;

	POSITION pos;
	__try
	{
	EnterCriticalSection(&m_sectionConn);
	if( !IsExistInList(&http,pos) )
		goto Exit;

	char* content = buffer+lenTcp;

	UINT uCodePage = 0;
	char* strCharSet = StrStrA(content,"charset=");
	if( strCharSet )
	{		
		uCodePage = CodePageID(strCharSet);
	}

	PConnect_HTTP phttp = m_listCon.GetAt(pos);
	if( NULL == phttp )
		goto Exit;
	
	
	int nLenTemp = 0;
	static char chTemp[16];
	while( nLenTemp < nLenContent )
	{
		char *strStart = StrStrA(content,"<");
		if( NULL == strStart )
			break;
		nLenTemp = strStart - (buffer+lenTcp) + 7;
		if( nLenTemp >= nLenContent )
			break;
		memcpy(chTemp,strStart,7);
		chTemp[7] = '\0';
		strlwr(chTemp);
		if( strcmp(chTemp,"<title>") != 0 )
		{
			content = strStart+7;
			continue;
		}
		
		char *strEnd = StrStrA(content,"</");
		if( NULL == strEnd )
			break;
		nLenTemp = strEnd - (buffer+lenTcp) + 8;
		if( nLenTemp >= nLenContent )
			break;
		memcpy(chTemp,strEnd,8);
		chTemp[8] = '\0';
		strlwr(chTemp);
		if( strcmp(chTemp,"</title>") != 0 )
			break;
		int lentitle = (int)(strEnd-strStart-7);
		CopyCharacters(phttp->title,strStart+7,lentitle);	
		if( uCodePage )
		{
			ToUTF8(uCodePage,phttp->title,lentitle);
		}
		phttp->lenTitle = lentitle;
		phttp->bRespond = TRUE;
		break;
	}	
	
Exit:
	LeaveCriticalSection(&m_sectionConn);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		LeaveCriticalSection(&m_sectionConn);
		return;
	}
}

void CSnifferSocket::AddConnect(u_short dport,u_short sPort, u_long ip,CHAR* host,int lenHost,CHAR* url, int lenUrl)
{
	PConnect_HTTP phttp = new Connect_HTTP;
	phttp->bRespond = FALSE;
	phttp->bSended = FALSE;
	phttp->SourPort = sPort;
	phttp->DestPort = dport;
	phttp->IP = ip;
	CopyCharacters(phttp->CompUrl,url,lenUrl);
	phttp->lenUrl = lenUrl;
	CopyCharacters(phttp->host,host,lenHost);
	phttp->lenHost = lenHost;
	QueryPerformanceCounter(&phttp->time);

	EnterCriticalSection(&m_sectionConn);
	POSITION pos;
	if( !IsExistInList(phttp,pos) )
	{
		pos = m_listCon.AddTail(phttp);
	}
	else
		delete phttp;
	LeaveCriticalSection(&m_sectionConn);
	
	SendMessage(m_hWnd,WM_NOTICE_AFFILIATE,WPARAM(pos), 0);
}


BOOL CSnifferSocket::IsExistInList(PConnect_HTTP phttp,POSITION &posElem)
{
	POSITION pos = m_listCon.GetHeadPosition();
	PConnect_HTTP pElem = NULL;
	while( pos )
	{
		posElem = pos;
		pElem = m_listCon.GetNext(pos);
		if( pElem )
		{
			if( ( pElem->SourPort == phttp->SourPort ) &&
				( pElem->DestPort == phttp->DestPort ) &&
				( pElem->IP == phttp->IP ) )
			{
				return TRUE;
			}
		}
	}
	posElem = NULL;
	return FALSE;
}

void CSnifferSocket::RemoveAllConn()
{
	POSITION pos = m_listCon.GetHeadPosition();
	PConnect_HTTP pElem = NULL;
	while( pos )
	{
		pElem = m_listCon.GetNext(pos);
		if( pElem )
			delete pElem;
	}
	m_listCon.RemoveAll();
}

void CSnifferSocket::EndSniffer(BOOL bEnd/* = TRUE*/)
{
	EnterCriticalSection(&m_section);
	m_EndSniffer=bEnd;
	LeaveCriticalSection(&m_section);
}

BOOL CSnifferSocket::IsEndsniffer()
{
	BOOL result;
	EnterCriticalSection(&m_section);
	result = m_EndSniffer;
	LeaveCriticalSection(&m_section);
	return result;
}

void CSnifferSocket::TimerProc()
{
	LARGE_INTEGER pfCounter;
	QueryPerformanceCounter(&pfCounter);
	int seconds;
	seconds = (int)((pfCounter.QuadPart - m_LastCounter.QuadPart) / m_Frequency.QuadPart);
	if( seconds <= 20 )
		return;

	m_LastCounter.QuadPart = pfCounter.QuadPart;

	PConnect_HTTP phttp;
	POSITION posBack;
	EnterCriticalSection(&m_sectionConn);
	POSITION pos = m_listCon.GetHeadPosition();
	while(pos)
	{
		posBack = pos;
		phttp = m_listCon.GetNext(pos);
		if( phttp )
		{
			seconds = (int)((pfCounter.QuadPart - phttp->time.QuadPart) / m_Frequency.QuadPart);
			if( seconds > 120 )
			{
				delete phttp;
			}
			else
			{
				continue;
			}
		}
		m_listCon.RemoveAt(posBack);
	}
	LeaveCriticalSection(&m_sectionConn);
}

void CSnifferSocket::EnterConnectCS()
{
	EnterCriticalSection(&m_sectionConn);
}
void CSnifferSocket::LeaveConnectCS()
{
	LeaveCriticalSection(&m_sectionConn);
}

void CSnifferSocket::CopyCharacters(char* chDest,const char* chSour, int &len)
{
	if( NULL == chSour )
		return;
	if( NULL == chDest )
		return;
	if( len >= MAX_STRING )
		len = MAX_STRING;
	memcpy(chDest,chSour, len);
	chDest[len] = '\0';
	for( int i = 0; i < len; i++)
	{
		if( ( chDest[i] == '\'') ||
			( chDest[i] == '<') ||
			( chDest[i] == '>') )
			chDest[i] = ',';
	}
}

void CSnifferSocket::AddAffiliateList(HWND hwnd)
{
	m_hWnd = hwnd;
}

void CSnifferSocket::CompareHostName(const char* host)
{
	/*if( (NULL == m_pstrList) || (NULL == m_hWnd) )
		return;*/
//	POSITION pos = m_pstrList->GetHeadPosition();
	TCHAR tcHost[MAX_PATH];
#ifdef UNICODE
	MultiByteToWideChar(CP_ACP, 0, host, (int)strlen(host),tcHost,MAX_PATH);
#else
	char* tcHost = host;
#endif
//	POSITION posMatch;
	//while( pos )
	{
		//posMatch = pos;
		//CString strStored = m_pstrList->GetNext(pos);
		CAtlRegExp<> reUrl;
		//REParseError status = reUrl.Parse(strStored);
		REParseError status = reUrl.Parse(_T("[%s.baidu.com]"));
		CAtlREMatchContext<> mcUrl;
		if(reUrl.Match(tcHost,&mcUrl))
		//if( 0 == strStored.Compare(tcHost) )
		{
	//		PostMessage(m_hWnd,WM_NOTICE_AFFILIATE,(WPARAM)posMatch,0);
		}
	}
}

void CSnifferSocket::MyStrTrim(char* host)
{
	int len = (int)strlen(host);
	int i = len-1;
	while( i > 0 )
	{
		if( *(host+i) != ' ' )
			break;
		i--;
	}
	*(host+i+1) = '\0';
}