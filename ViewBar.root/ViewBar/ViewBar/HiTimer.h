/****************************************************************************
*	High resolution performance timer										*
* Revision:		1.0		Victor Li		03/30-2007							*
****************************************************************************/
#pragma once
#include <Afxmt.h>
/*
#define WM_HITIMER WM_USER + 2100
class CHiTimer
{
public:
	CHiTimer(void);
	virtual ~CHiTimer(void);
	void SetHiTimer(HWND hwnd = NULL, BOOL bTimerProc = FALSE,int Interval = 100);
	DWORD GetTotalLapseSeconds();
	DWORD GetTotalLapseMinutes();

	// this function should be called each time before retrieve lapse time
	// bRestart: TRUE, restart time
	//			 FALSE, next querying time is still based on the original time.
	void KillHiTimer();
private:
	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_LastCounter;
	LARGE_INTEGER m_FirstCounter;
	LARGE_INTEGER m_CurrentCounter;
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
	int m_nInterval;
	CEvent m_EndEvent;
	HANDLE m_hThread;
	CRITICAL_SECTION m_csCurCounter;
	HWND m_hwnd;
	void AccessCriticalSection();
};
*/

class CHiTimer
{
public:
	CHiTimer(void);
	virtual ~CHiTimer(void);
	DWORD GetTotalLapseSeconds();
	DWORD GetTotalLapseMinutes();
	void SetHiTimer();
private:	
	LARGE_INTEGER m_Frequency;
	//LARGE_INTEGER m_LastCounter;
	LARGE_INTEGER m_FirstCounter;
	LARGE_INTEGER m_CurrentCounter;
	CRITICAL_SECTION m_cs;
};

extern CHiTimer g_HiTimer;