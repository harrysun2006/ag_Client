// ViewBar.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CViewBarApp:
// See ViewBar.cpp for the implementation of this class
//

class CViewBarApp : public CWinApp
{
public:
	CViewBarApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
private:
	BOOL LockFile();
	BOOL UnlockFile();
	void GetUserInfo();	// browser type, ip, ..
	BOOL IsAutoLogin();
private:
	CFile m_fileLocker;
	BOOL m_bLock;
};

extern CViewBarApp theApp;
