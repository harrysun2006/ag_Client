// ViewBar.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ViewBar.h"
#include "ViewBarDlg.h"

// single instance
#include "utility/LimitSingleInstance.h"

#include "LangManager.h"
#include "MyAppBar.h"
#include "SkinManager.h"
#include "SearchManager.h"
#include "VBSession.h"
#include "AdsManager.h"
#include "CppSQLiteManager.h"
#include "MyUtility.h"
#include "Updater.h"
#include "utility/FavoriteMenu.h"
#include "AppBarManager.h"

using namespace http;
using namespace viewbar;

CLimitSingleInstance g_SingleInstanceObj(TEXT("{4B1B5a33-A0B9-4fta3-BD4D-497032CA7977}"));
CLogMessage g_log;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewBarApp

BEGIN_MESSAGE_MAP(CViewBarApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CViewBarApp construction

CViewBarApp::CViewBarApp(): m_bLock(FALSE)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CViewBarApp object

CViewBarApp theApp;

BOOL CViewBarApp::LockFile()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetSystemDirectory(szPath, MAX_PATH);
	CString path = szPath;
	path += _T("\\dummy.dat");

	m_bLock = m_fileLocker.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);

	return m_bLock;
}

BOOL CViewBarApp::UnlockFile()
{
	if (m_bLock)
		m_fileLocker.Close();
	return TRUE;
}

// UINT AutoUpdateThread2(LPVOID pParam)
// {
// 	CoInitialize(NULL);
// 
// 	CAutoUpdate au;
// 
// 	au.AutoUpdate();
// 
// 	CoInitialize(NULL);
// 
// 	return true;
// }

// CViewBarApp initialization
BOOL CViewBarApp::InitInstance()
{
	//enable leak check
#ifdef DEBUG
	_CrtSetDbgFlag( _CRTDBG_REPORT_FLAG); 
#endif
	

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// single instance, esp. for XP users
	if (LockFile() == FALSE)
	{
		//CString msg, title;
		//msg.LoadString(IDS_WARNING_MULTIINTANCE);

		//AfxMessageBox(msg, MB_ICONWARNING);

		CAppBarManager::GetInstance()->OnMultiInstance();

		return FALSE;
	}


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Agloco Viewbar"));

	// initialize com
	CoInitialize(NULL);


	thePrefs.LoadConfigFromRegistry();

//#ifdef DEBUG
	g_log.Open(_T("log.txt"));
//#endif

	// single instance
	if (g_SingleInstanceObj.IsAnotherInstanceRunning()) 
		return FALSE;

	//// Update
	//CUpdater updater;
	//updater.AutoUpdate();

	// initialization
	CUISubject::GetInstance();

	// lang
	CLangManager::GetInstance()->GetDefaultLanguage();

	// skin
	CSkinManager::GetInstance();

	// ViewBar session
	http::CVBSession::GetInstance();

	// SearchEngine
	CSearchManager::GetInstance();

	// Advertisement
	CAdsManager::GetInstance();

	// SQLite
	CCppSQLiteManager::GetInstance();

	// Appbar Manager
	CAppBarManager::GetInstance();

	CDataStore::GetInstance();

	CMyUtility::m_uProcessMsg = RegisterWindowMessage(_T("78910EFA-50F9-4f7d-A66E-A8365C60140B"));
	
	GetUserInfo();

	CDlgLogin dlg;

	if (IsAutoLogin())
	{
		dlg.SetAutoLogin();
	}
	m_pMainWnd = &dlg;

	// log
	g_log.OutputDebug(_T("++++++++++++++++++++++++++++++++++++++++++++++APPLICATION START+++++++++++++++++++++++++++++++++++++++++++++++"));

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		// dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
int CViewBarApp::ExitInstance()
{
	// free ui observer
	CUISubject::FreeInstance();
	CLangManager::FreeInstance();
	CSkinManager::FreeInstance();
	CDockFacade::FreeInstance();
	CSearchManager::FreeInstance();
	http::CVBSession::FreeInstance();
	CAdsManager::FreeInstance();
	CCppSQLiteManager::FreeInstance();
	CDataStore::FreeInstance();
	CAppBarManager::FreeInstance();

	CoUninitialize();
	UnlockFile();

	g_log.OutputDebug(_T("++++++++++++++++++++++++++++++++++++++++++++++APPLICATION END+++++++++++++++++++++++++++++++++++++++++++++++"));


	return CWinApp::ExitInstance();
}

void CViewBarApp::GetUserInfo()
{
	CFavorite fav;
	BROWSERTYPE iBrowserType;
	fav.getBrowserType(iBrowserType);
	if (iBrowserType == IEXPLORE)
	{
		thePrefs.SetDefaultBrowserName(_T("IE"));
	}
	else
	if (iBrowserType == FIREFOX)
	{
		thePrefs.SetDefaultBrowserName(_T("FireFox"));
	}
}


BOOL CViewBarApp::IsAutoLogin()
{
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(cmdInfo.m_strFileName.IsEmpty())
		return FALSE;

	int iAutoLogin = 0;
	_stscanf(cmdInfo.m_strFileName, _T("%d"), &iAutoLogin);

	if (iAutoLogin == 1)
	{
		return TRUE;
	}

	return FALSE;
}