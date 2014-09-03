#include "StdAfx.h"
#include "HiTimer.h"

//#define HITIMER_TEST_
/*
CHiTimer::CHiTimer(void)
{
	m_hThread = NULL;
	InitializeCriticalSection(&m_csCurCounter);
}

CHiTimer::~CHiTimer(void)
{
	if( m_hThread )
	{
		SetEvent(m_EndEvent);
		Sleep(200);
		CloseHandle(m_hThread);
	}
	DeleteCriticalSection(&m_csCurCounter);
}

void CHiTimer::SetHiTimer(HWND hwnd = NULL,BOOL bTimerProc = FALSE,int Interval = 100)
{
	m_hwnd = hwnd;
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_LastCounter);
	m_FirstCounter.QuadPart = m_LastCounter.QuadPart;
	if( bTimerProc )
	{
		m_nInterval = Interval;
		m_hThread = CreateThread(NULL,NULL,ThreadProc,this,0,NULL);
	}
}

void CHiTimer::KillHiTimer()
{
	SetEvent(m_EndEvent);
}

DWORD CHiTimer::GetTotalLapseSeconds()
{
	DWORD second = 0;
	EnterCriticalSection(&m_csCurCounter);
	if( m_hThread == NULL )
		QueryPerformanceCounter(&m_CurrentCounter);
	second = (m_CurrentCounter.QuadPart - m_FirstCounter.QuadPart) / m_Frequency.QuadPart;
	LeaveCriticalSection(&m_csCurCounter);
	return second;
}

DWORD CHiTimer::GetTotalLapseMinutes()
{
	return (GetTotalLapseSeconds() / 60);
}

DWORD CHiTimer::ThreadProc(LPVOID lpParameter)
{
	CHiTimer* pThis = (CHiTimer*)lpParameter;
	int MinSeconds;
	while(TRUE)
	{
		if( WaitForSingleObject(pThis->m_EndEvent,0) == WAIT_OBJECT_0 )
			break;
		pThis->AccessCriticalSection();

		MinSeconds = ( pThis->m_CurrentCounter.QuadPart - pThis->m_LastCounter.QuadPart ) * 1000 / pThis->m_Frequency.QuadPart;
		if( MinSeconds >= pThis->m_nInterval )
		{
			pThis->m_LastCounter.QuadPart = pThis->m_CurrentCounter.QuadPart;
			PostMessage(pThis->m_hwnd,WM_HITIMER,0,0);
		}
	}
	return 0;
}

void CHiTimer::AccessCriticalSection()
{
	EnterCriticalSection(&m_csCurCounter);
	QueryPerformanceCounter(&m_CurrentCounter);
	LeaveCriticalSection(&m_csCurCounter);
}*/

CHiTimer::CHiTimer(void)
{
	InitializeCriticalSection(&m_cs);
	SetHiTimer();
}

CHiTimer::~CHiTimer(void)
{
	DeleteCriticalSection(&m_cs);
}

void CHiTimer::SetHiTimer()
{
	EnterCriticalSection(&m_cs);
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_FirstCounter);
	//m_LastCounter.QuadPart = m_FirstCounter.QuadPart;
	LeaveCriticalSection(&m_cs);
}

DWORD CHiTimer::GetTotalLapseSeconds()
{
	EnterCriticalSection(&m_cs);
	//QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_CurrentCounter);
	DWORD second = (DWORD)((m_CurrentCounter.QuadPart - m_FirstCounter.QuadPart) / m_Frequency.QuadPart);
#ifdef HITIMER_TEST_
	LARGE_INTEGER liTemp;
	liTemp.QuadPart = (CurrentCounter.QuadPart - m_FirstCounter.QuadPart) / m_Frequency.QuadPart;
	CString error;
	error.Format(_T("LONGLONG(%d,%d),second[%d]\tErrorCode[%d,%d]\n"), liTemp.HighPart, liTemp.LowPart,second,dwPerformance,dwCounter);
	g_log.OutputDebug(error);
#endif
	LeaveCriticalSection(&m_cs);
	return second;
}

DWORD CHiTimer::GetTotalLapseMinutes()
{
	return GetTotalLapseSeconds() / 60;
}
CHiTimer g_HiTimer;