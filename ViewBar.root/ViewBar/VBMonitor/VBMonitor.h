// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VBMONITOR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VBMONITOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

// Mouse and Keyboard HOOK are used here to monitor the action of user
// Victor Li

#ifdef VBMONITOR_EXPORTS
#define VBMONITOR_API __declspec(dllexport)
#else
#define VBMONITOR_API __declspec(dllimport)
#endif

// Message to be send to parent window
// WPARMA is the tick
#define WM_MONITOR_KEYBOARD		(WM_USER + 1500)
#define WM_MONITOR_MOUSE		(WM_MONITOR_KEYBOARD + 1)

VBMONITOR_API BOOL SetMessageHandle(HWND hwnd);
VBMONITOR_API BOOL MonitorInit(HWND hwnd);
VBMONITOR_API void MonitorExit();
VBMONITOR_API DWORD GetLastTick();
#ifdef _DEBUG
VBMONITOR_API void GetTestData(int a[]);
#endif