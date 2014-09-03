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
#define WM_MONITOR_KEYBOARD		(WM_USER + 9000)
#define WM_MONITOR_MOUSE		(WM_MONITOR_KEYBOARD + 1)

enum HOOKTYPE
{
	ALL_HOOK = 0,		// all hook will be set/unset
	KEYBOARD_HOOK = 1,	// Keyboard hook
	MOUSE_HOOK = 2		// Mouse Hook
};

/****************************************************************************
*	Initialization function for seting hooks, according hook type			*
* Parameters:																*
*		hWnd: [in]message would be send to window of hWnd.					*
*		nSwitch: [in]flag of the type of hook.								*
* Return value:	return the type of hook. -1, if no hook be set.				*
* Remark:																	*
*	Message would not be send, if the hWnd is NULL.							*
* revision:		1.0		Victor Li	03/29-2007								*
****************************************************************************/
extern "C" VBMONITOR_API int VBMonitorInit(HWND hWnd,HOOKTYPE nSwitch = ALL_HOOK);

/****************************************************************************
*	Release the hook procedure, according of hook type						*
* Parameters:																*
*		nSwitch: [i]flag of the type of hook unstalled.						*
* Remark:																	*
*		This function should be called with association of VBMonitorInit()	* 
* Revision:		1.0		Victor Li	03/29-2007								*
****************************************************************************/
extern "C" VBMONITOR_API void VBMonitorUninit(HOOKTYPE nSwitch = ALL_HOOK);

/****************************************************************************
*	Retrieve last input event tick.											*
* Return value: tick count.													*
* revision:		1.0		Victor Li	03/29-2007								*
****************************************************************************/
extern "C" VBMONITOR_API DWORD GetLastTick();

/****************************************************************************
*	Retrieve last mouse position.											*
* Return value: mouse position.												*
* revision:		1.0		Victor Li	03/29-2007								*
****************************************************************************/
extern "C" VBMONITOR_API POINT GetMousePoint();

/****************************************************************************
*	Retrieve last input virtual-key.										*
* Return value: virtual-key.												*
* revision:		1.0		Victor Li	03/29-2007								*
****************************************************************************/
extern "C" VBMONITOR_API int GetVirtualKeyCode();
