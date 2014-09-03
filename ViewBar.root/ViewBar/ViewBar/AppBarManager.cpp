#include "StdAfx.h"
#include "AppBarManager.h"
#include "Tlhelp32.h."

CAppBarManager * CAppBarManager::m_pInstance = NULL;
UINT CAppBarManager::m_uMessageId = 0;

CAppBarManager::CAppBarManager(void)
	: m_pDlgLogin(NULL)
{
	m_uMessageId = RegisterWindowMessage(_T("{10931CBC-AE85-40d7-8103-D013A81602EE}"));
}

CAppBarManager::~CAppBarManager(void)
{
}

CAppBarManager* CAppBarManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CAppBarManager;
	}

	return m_pInstance;
}

void CAppBarManager::FreeInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CAppBarManager::Attach(CDlgLogin *pWnd)
{
	m_pDlgLogin = pWnd;
}

CDlgLogin * CAppBarManager::GetLoginWnd()
{
	return m_pDlgLogin;
}

void CAppBarManager::ActiveViewbar()
{
	if (!thePrefs.IsUserLoggedIn())
	{
		// show Viewbar Login Dialog
		GetLoginWnd()->m_TrayIcon.MaximiseFromTray(GetLoginWnd(), TRUE);
	}
	else
	// user is logged in
	{
		if (!thePrefs.IsViewBarVisible())
		{
			GetLoginWnd()->ShowViewBar(FALSE);
		}
	}
}


// get token by process name
BOOL CAppBarManager::GetTokenByName(HANDLE &hToken, LPTSTR lpName)
{   
	if(!lpName)   
	{   
		return   FALSE;   
	}   

	HANDLE hProcessSnap = NULL;     
	BOOL bRet = FALSE;     
	PROCESSENTRY32 pe32 =  {0};     

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);   
	if (hProcessSnap == INVALID_HANDLE_VALUE)     
		return FALSE;     

	pe32.dwSize = sizeof(PROCESSENTRY32);     

	if (Process32First(hProcessSnap, &pe32))     
	{   
		do     
		{   
			if (!_tcsicmp(pe32.szExeFile, lpName))
			{   
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,   
					FALSE, pe32.th32ProcessID);   
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);   
				CloseHandle (hProcessSnap);     
				return (bRet);   
			}   
		}     
		while (Process32Next(hProcessSnap, &pe32));     
		bRet = FALSE;     
	}     
	else     
		bRet = FALSE;   

	CloseHandle (hProcessSnap);     
	return (bRet);   
}   

CString CAppBarManager::GetCurrentUserName(LPTSTR lpstrAppName)
{   
	HANDLE hToken;   

	// 得到shell的token   
	if(!GetTokenByName(hToken, lpstrAppName))   
	{   
		return FALSE;   
	}   
	DWORD cbti = 0;   
	PTOKEN_USER ptiUser = NULL;   
	SID_NAME_USE snu;   

	// 取得所需空间大小   
	if (GetTokenInformation(hToken, TokenUser, NULL, 0, &cbti))     
	{   
		CloseHandle(hToken);   
		return FALSE;   
	}   

	// 分配空间   
	ptiUser = (PTOKEN_USER) HeapAlloc(GetProcessHeap(), 0, cbti);   
	if(!ptiUser)   
	{   
		CloseHandle(hToken);   
		return FALSE;   
	}   

	// 取得token信息   
	if (!GetTokenInformation(hToken, TokenUser, ptiUser, cbti, &cbti))   
	{   
		CloseHandle(hToken);   
		HeapFree(GetProcessHeap(), 0, ptiUser);   
		return FALSE;   
	}   

	TCHAR szUser[MAX_PATH];   
	TCHAR szDomain[MAX_PATH];   
	DWORD nUser = MAX_PATH;   
	DWORD nDomain = MAX_PATH;   

	// 根据用户的sid得到用户名和domain   
	if (!LookupAccountSid(NULL, ptiUser->User.Sid, szUser, &nUser,     
		szDomain, &nDomain, &snu))   
	{   
		CloseHandle(hToken);   
		HeapFree(GetProcessHeap(), 0, ptiUser);   
		return FALSE;   
	}   

	CloseHandle(hToken);   
	HeapFree(GetProcessHeap(), 0, ptiUser);

	CString strUserName(szUser);

	return strUserName;
}   

void CAppBarManager::OnMultiInstance()
{
	CString strCurrentUser = GetCurrentUserName(_T("Explorer.exe"));
	CString strViewBarUser = GetCurrentUserName(_T("ViewBar.exe"));

	if (strViewBarUser == strCurrentUser)
	{
		::PostMessage(HWND_BROADCAST, m_uMessageId, GetCurrentProcessId(), 0);
	}
	else
	{
		CString msg, title;
		msg.LoadString(IDS_WARNING_MULTIINTANCE);

		AfxMessageBox(msg, MB_ICONWARNING);
	}
}