// ViewBar.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VBUpdater.h"


#include ".\utility\VBSession.h"

#include "AutoUpdate.h"
#include "MyUtility.h"

#include "AuDlg.h"


using namespace http;
using namespace viewbar;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVBUpdaterApp

BEGIN_MESSAGE_MAP(CVBUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVBUpdaterApp construction

CVBUpdaterApp::CVBUpdaterApp()
{
	m_bAutoDown = 1;
	m_bAutoInstall = 1;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CVBUpdaterApp object

CVBUpdaterApp theApp;




// CVBUpdaterApp initialization
BOOL CVBUpdaterApp::InitInstance()
{
	//enable leak check
	_CrtSetDbgFlag( _CRTDBG_REPORT_FLAG); 

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
	/*if (LockFile() == FALSE)
	{
		return FALSE;
	}*/

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Agloco ViewBar"));

	// initialize com
	CoInitialize(NULL);

	//thePrefs.LoadConfigFromRegistry();

	CVBUpdaterDlg::m_ProcessMsg = RegisterWindowMessage(_T("78910EFA-50F9-4f7d-A66E-A8365C60140B"));

	if (ParseOwnCmdLine() == FALSE)
	{
		return FALSE;
	}
	

	CVBUpdaterDlg auDlg;
	if( m_bAutoDown )
		auDlg.StartDownload();

	m_pMainWnd = &auDlg;
	INT_PTR nResponse = auDlg.DoModal();
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
	return FALSE;
}

int CVBUpdaterApp::ExitInstance()
{
	CoUninitialize();
	return CWinApp::ExitInstance();
}


BOOL CVBUpdaterApp::ParseOwnCmdLine()
{
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(cmdInfo.m_strFileName.IsEmpty())
	{
		MessageBox(NULL, _T("You shouldn't run updater.exe"), _T("Error"), MB_OK);
		return FALSE;
	}
	
	CString in;
	in = cmdInfo.m_strFileName;
	thePrefs.LoadConfig(in);

	_stscanf(thePrefs.GetUpdateFlag(), _T("%d.%d"), &m_bAutoDown, &m_bAutoInstall);

	return TRUE;
}