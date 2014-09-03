// VBMonitor.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "VBMonitor.h"
#include <tchar.h>


#ifdef _MANAGED
#pragma managed(push, off)
#endif
HINSTANCE g_hInstance = NULL;   // global instance handle
HWND g_hWnd = NULL;
const int TESTDATASIZE = 5;

#pragma data_seg(".MyMonitor")
HHOOK 	g_hHkKeyboard = NULL;	// handle to the keyboard hook
HHOOK 	g_hHkMouse = NULL;	// handle to the mouse hook
DWORD	g_dwLastTick = 0;	// tick time of last input event

DWORD g_dwStoreTick = 0;
POINT g_ptLast;
TCHAR clsName[MAX_PATH] = {0};

#ifdef _DEBUG
int g_a[TESTDATASIZE] = {0};
#endif

#pragma data_seg()
#pragma comment(linker, "/section:.MyMonitor,rws")




#ifdef _MANAGED
#pragma managed(pop)
#endif


BOOL IsBrowserWindow(HWND hwnd)
{
	HWND hwndCur = GetAncestor(hwnd,GA_ROOTOWNER);
#ifdef _DEBUG
	g_a[3] = (int)hwndCur;
#endif
	//while( hwndCur )
	{
		memset(clsName,0,sizeof(TCHAR)*MAX_PATH);
		GetClassName(hwndCur,clsName,MAX_PATH);
		if( ( _tcscmp(clsName,_T("IEFrame")) == 0 ) ||
			( _tcscmp(clsName,_T("MozillaUIWindowClass")) == 0 ) ||
			( _tcscmp(clsName,_T("MozillaWindowClass")) == 0))
		{
#ifdef _DEBUG
			g_a[0] = TRUE;
#endif
			return TRUE;
		}
		hwndCur = ::GetParent(hwndCur);
	}
#ifdef _DEBUG
	g_a[0] = FALSE;
#endif
	return FALSE;
}

LRESULT CALLBACK KeyboardTracker(int code, WPARAM wParam, LPARAM lParam)
{
	if (code==HC_ACTION)
	{
		HWND hwndFocus = ::GetForegroundWindow();
		if( IsBrowserWindow(hwndFocus) )
		{
			g_dwLastTick = GetTickCount();
#ifdef _DEBUG
			g_a[0] = (int)hwndFocus;
#endif
			g_dwStoreTick = g_dwLastTick;
			/*if( g_hWnd )
				::PostMessage(g_hWnd, WM_MONITOR_KEYBOARD, 1,0);*/
		}
		else
		{
			g_dwLastTick = g_dwStoreTick;
		}
	}

	return ::CallNextHookEx(g_hHkKeyboard, code, wParam, lParam);
}

/**
* Mouse hook: record tick count
**/
LRESULT CALLBACK MouseTracker(int code, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT *ms = (MOUSEHOOKSTRUCT*)lParam;
#ifdef _DEBUG
	g_a[1] = ms->pt.x;
	g_a[2] = wParam;
	g_a[4] = ms->pt.y;
#endif

	if (code==HC_ACTION)
	{
		if( IsBrowserWindow(ms->hwnd) )
		{
			if( ( (g_ptLast.x == ms->pt.x) && (g_ptLast.y == ms->pt.y) ) &&
				( ( wParam == WM_MOUSEMOVE ) || ( wParam == WM_NCMOUSEMOVE ) ) )
			{
				g_dwLastTick = g_dwStoreTick;
			}
			else
			{
				g_dwLastTick = GetTickCount();
				g_dwStoreTick = g_dwLastTick;
				g_ptLast.x = ms->pt.x;
				g_ptLast.y = ms->pt.y;
			}
			/*if( g_hWnd )
				::PostMessage(g_hWnd, WM_MONITOR_MOUSE, 0,0);*/
		}
		else
		{
			g_dwLastTick = g_dwStoreTick;
		}
	}

	return ::CallNextHookEx(g_hHkMouse, code, wParam, lParam);
}


VBMONITOR_API BOOL MonitorInit(HWND hwnd)
{
	g_hWnd = hwnd;
	if(g_hHkKeyboard == NULL)
		g_hHkKeyboard = SetWindowsHookEx(WH_KEYBOARD, KeyboardTracker, g_hInstance, 0);
	if(g_hHkMouse == NULL)
		g_hHkMouse = SetWindowsHookEx(WH_MOUSE, MouseTracker, g_hInstance, 0);
	if( hwnd != NULL )
	{
		g_dwLastTick = GetTickCount();
		g_dwStoreTick = g_dwLastTick;
	}
	if (!g_hHkKeyboard || !g_hHkMouse)
		return FALSE;
	return TRUE;
}
VBMONITOR_API void MonitorExit()
{
	if (g_hHkKeyboard)
	{
		UnhookWindowsHookEx(g_hHkKeyboard);
		g_hHkKeyboard = NULL;
	}
	if (g_hHkMouse)
	{
		UnhookWindowsHookEx(g_hHkMouse);
		g_hHkMouse = NULL;
	}
}

VBMONITOR_API DWORD GetLastTick()
{
	return g_dwLastTick;
}

VBMONITOR_API BOOL SetMessageHandle(HWND hwnd)
{
	g_hWnd = hwnd;
	return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = (HMODULE)hModule;
		DisableThreadLibraryCalls(g_hInstance);
		g_ptLast.x = g_ptLast.y = 0;
		/*g_dwLastTick = GetTickCount();
		g_dwStoreTick = g_dwLastTick;*/
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _DEBUG
VBMONITOR_API void GetTestData(int a[])
{
	for( int i = 0; i < TESTDATASIZE; i++)
		a[i] = g_a[i];
}
#endif