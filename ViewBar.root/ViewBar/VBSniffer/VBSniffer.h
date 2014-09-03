// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VBSNIFFER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VBSNIFFER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once
#ifdef VBSNIFFER_EXPORTS
//#define VBSNIFFER_API __declspec(dllexport)
#define VBSNIFFER_API 
#else
//#define VBSNIFFER_API __declspec(dllimport)
#define VBSNIFFER_API 
#endif
#define MAX_STRING	(256 - 1)

#define WM_NOTICE_AFFILIATE WM_USER + 6000
typedef struct tagConnect_HTTP
{
	int MemID;
	unsigned char bRespond;
	unsigned char bSended;
	unsigned short SourPort;
	unsigned short DestPort;
	unsigned int IP;
	char CompUrl[MAX_STRING];
	unsigned char lenUrl;
	char host[MAX_STRING];
	unsigned char lenHost;
	char title[MAX_STRING];
	unsigned char lenTitle;
	LARGE_INTEGER time;
} Connect_HTTP, *PConnect_HTTP;

VBSNIFFER_API BOOL StartSniffer(int MajorVersion,int MinorVersion);
VBSNIFFER_API void EndSniffer();
VBSNIFFER_API POSITION GetHeadPosition();
VBSNIFFER_API PConnect_HTTP GetNext(POSITION &pos);
VBSNIFFER_API CList<PConnect_HTTP>& GetConnList();
VBSNIFFER_API void AddTail(PConnect_HTTP pcon);
VBSNIFFER_API void RemoveAt(POSITION pos);
VBSNIFFER_API void EnterConnectCS();
VBSNIFFER_API void LeaveConnectCS();
VBSNIFFER_API void RemoveAllConn();
VBSNIFFER_API void AddAffiliateList(HWND hwnd);
