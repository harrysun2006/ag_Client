#pragma once

#include "ViewBarDlg.h"
class CAppBarManager
{
public:
	static CAppBarManager* GetInstance();
	static void FreeInstance();

	void Attach(CDlgLogin *pWnd);
	void ActiveViewbar();
	void OnMultiInstance();

public:
	static UINT m_uMessageId;

private:
	CAppBarManager(void);
	~CAppBarManager(void);
	CDlgLogin *GetLoginWnd();	
	BOOL GetTokenByName(HANDLE &hToken, LPTSTR lpName);
	CString GetCurrentUserName(LPTSTR lpstrAppName);


private:
	CDlgLogin *m_pDlgLogin;

	static CAppBarManager *m_pInstance;
};
