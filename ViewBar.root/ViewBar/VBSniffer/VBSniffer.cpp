// VBSniffer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "VBSniffer.h"
#include "SnifferSocket.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif
CSnifferSocket gsocket;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


VBSNIFFER_API BOOL StartSniffer(int MajorVersion,int MinorVersion)
{
	if( gsocket.LoadSockLib(MajorVersion,MinorVersion) )
	{
		if( gsocket.CreateSocket() )
		{
			return gsocket.StartSniffer();
		}
		else
			gsocket.ReleaseSockLib();
	}
	return FALSE;
}

VBSNIFFER_API void EndSniffer()
{
	gsocket.EndSniffer(TRUE);
	gsocket.CloseSocket();
	gsocket.ReleaseSockLib();
}

VBSNIFFER_API POSITION GetHeadPosition()
{
	return gsocket.m_listCon.GetHeadPosition();
}

VBSNIFFER_API PConnect_HTTP GetNext(POSITION &pos)
{
	return gsocket.m_listCon.GetNext(pos);
}

VBSNIFFER_API void RemoveAt(POSITION pos)
{
	gsocket.m_listCon.RemoveAt(pos);
}

VBSNIFFER_API void AddTail(PConnect_HTTP pcon)
{
	gsocket.m_listCon.AddTail(pcon);
}

VBSNIFFER_API CList<PConnect_HTTP>& GetConnList()
{
	return gsocket.m_listCon;
}

VBSNIFFER_API void EnterConnectCS()
{
	gsocket.EnterConnectCS();
}
VBSNIFFER_API void LeaveConnectCS()
{
	gsocket.LeaveConnectCS();
}

VBSNIFFER_API void RemoveAllConn()
{
	gsocket.RemoveAllConn();
}

VBSNIFFER_API void AddAffiliateList(HWND hwnd)
{
	gsocket.AddAffiliateList(hwnd);
}