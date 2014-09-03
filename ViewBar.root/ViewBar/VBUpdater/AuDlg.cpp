// AuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VBUpdater.h"
#include "AuDlg.h"
#include "MyUtility.h"


CVBUpdaterDlg *g_pMainDlg = NULL;

// CVBUpdaterDlg dialog
UINT CVBUpdaterDlg::m_ProcessMsg = 0;
IMPLEMENT_DYNAMIC(CVBUpdaterDlg, CDialog)

CVBUpdaterDlg::CVBUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVBUpdaterDlg::IDD, pParent)
{
	m_auT	= NULL;
}

CVBUpdaterDlg::~CVBUpdaterDlg()
{
}

void CVBUpdaterDlg::ParseCmd()
{
	if (theApp.m_bAutoDown && theApp.m_bAutoInstall)
	{
		// UpdateSilent();
		m_AutoUpdate.AutoDownLoad();
	}

	if (theApp.m_bAutoDown && theApp.m_bAutoInstall == 0)
	{
		m_AutoUpdate.AutoDownLoad();
		//m_AutoUpdate.InstallEx();
	}

}



BOOL CVBUpdaterDlg::UpdateSilent()
{
	m_AutoUpdate.AutoDownLoad();

	::PostMessage(HWND_BROADCAST,m_ProcessMsg,GetCurrentProcessId(), 9);

//	m_AutoUpdate.AutoInstall();

	return TRUE;
}

BOOL CVBUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pMainDlg = this;

	m_AutoUpdate.SetParameters(CMyUtility::GetCurDir() + _T("\\update"), GetSafeHwnd());

	// according to parameters, show or hide main window of Updater.exe
	if (theApp.m_bAutoDown)
	{
		WINDOWPLACEMENT oldwp;
		GetWindowPlacement(&oldwp);
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		wp.flags = WPF_RESTORETOMAXIMIZED;
		wp.showCmd = SW_HIDE;
		SetWindowPlacement(&wp);
	}

	reset();
	//ParseCmd();

	return true;
}
void CVBUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRG, m_prog);
	DDX_Control(pDX, IDC_AUH, m_edtCon);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, ID_EXIT, m_btnExit);
}
/*
#define WM_DOWNLOADFINISHED	WM_USER+2501
#define WM_INSTALLFINISHED	WM_USER+2502
#define	WM_CHECKSTATUS		WM_USER+2503
#define WM_CHECKPERFORMANCE WM_USER+2504
#define WM_BEGINDOWNLOAD	WM_USER+2505
#define WM_DOWNLOADONE		WM_USER+2506
#define WM_BEGININSTALL		WM_USER+2507
#define WM_INSTALLONE		WM_USER+2508
*/
BEGIN_MESSAGE_MAP(CVBUpdaterDlg, CDialog)
	ON_BN_CLICKED(ID_EXIT, &CVBUpdaterDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_START, &CVBUpdaterDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_CANCEL, &CVBUpdaterDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_DOWNLOADFINISHED, &CVBUpdaterDlg::OnDownloadFinished )
	ON_MESSAGE(WM_INSTALLFINISHED, &CVBUpdaterDlg::OnInstallFinished )
	ON_MESSAGE(WM_CHECKSTATUS, &CVBUpdaterDlg::OnCheckStatus )
	ON_MESSAGE(WM_CHECKPERFORMANCE, &CVBUpdaterDlg::OnCheckPerformance )
	ON_MESSAGE(WM_BEGINDOWNLOAD, &CVBUpdaterDlg::OnBeginDownload )
	ON_MESSAGE(WM_DOWNLOADONE, &CVBUpdaterDlg::OnDownloadOne )
	ON_MESSAGE(WM_BEGININSTALL, &CVBUpdaterDlg::OnBegininstall )
	ON_MESSAGE(WM_INSTALLONE, &CVBUpdaterDlg::OnInstallOne)
	ON_BN_CLICKED(IDOK, &CVBUpdaterDlg::OnBnClickedOk)
	ON_REGISTERED_MESSAGE(CVBUpdaterDlg::m_ProcessMsg,OnProcessMsg)
	ON_MESSAGE(WM_AUTODOWNLOAD,OnAutoDownload)
	ON_MESSAGE(WM_AUTOINSTALL,OnAutoInstall)
END_MESSAGE_MAP()

// CVBUpdaterDlg message handlers

void CVBUpdaterDlg::OnBnClickedExit()
{
// 	if(m_auT)
// 	{
// 		m_auT->ExitInstance();
// 		m_auT = NULL;
// 	}
	CDialog::OnCancel();	// TODO: Add your control notification handler code here
}
// 1. check status.
LRESULT CVBUpdaterDlg::OnCheckStatus(WPARAM wParam, LPARAM lParam)
{
	CString con;
	GetDlgItemTextW(IDC_AUH, con);
	BOOL bDone = (BOOL)lParam;
	if( bDone )
		con += _T("Checking the status...\r\n");
	else
		con += _T("Failed to check status!\r\n");

	SetDlgItemTextW(IDC_AUH, con);
	
	UpdateData(FALSE);

	return 0;
}

// 2. check performance.
LRESULT CVBUpdaterDlg::OnCheckPerformance(WPARAM wParam, LPARAM lParam)
{
	CString con;
	GetDlgItemTextW(IDC_AUH, con);
	BOOL bDone = (BOOL)lParam;
	if( bDone )
	{
		con += _T("Checking performance...\r\n");
	}
	else
	{
		con += _T("Failed to check performance\r\n");
	}
	SetDlgItemTextW(IDC_AUH, con);
	
	UpdateData(FALSE);

	return 0;
}

// 3.begindownload
LRESULT CVBUpdaterDlg::OnBeginDownload(WPARAM wParam, LPARAM lParam)
{
	CString con;
	m_cfc = lParam;

	GetDlgItemTextW(IDC_AUH, con);
	con += _T("Start downloading...\r\n");
	SetDlgItemTextW(IDC_AUH, con);
	
	UpdateData(FALSE);

	return 0;
}

// 4. downloaded a file.
LRESULT CVBUpdaterDlg::OnDownloadOne(WPARAM wParam, LPARAM lParam)
{
	static int cpos=0;
	cpos++;
	CString con, tmp;
	GetDlgItemTextW(IDC_AUH, con);
	
	tmp.Format(_T("Downloading %dth file.\r\n"), cpos);
	con += tmp;
	SetDlgItemTextW(IDC_AUH, con);
	m_prog.SetPos((int)(100/m_cfc)+ m_prog.GetPos());

	UpdateData(FALSE);

	return 0;
}


// manually download over and before copying files
LRESULT CVBUpdaterDlg::OnDownloadFinished(WPARAM wParam, LPARAM lParam)
{



// 	CString con, tmp;
// 	GetDlgItemTextW(IDC_AUH, con);
// 	
// 	con += _T("download over.\r\n");
// 	SetDlgItemTextW(IDC_AUH, con);
// 	m_prog.SetPos(100);
// 
// 	UpdateData(FALSE);

// 	if (theApp.m_bAutoInstall)
// 	{
// 		// notify ViewBar to close itself and relogin if username and passwd are available
// 		::PostMessage(HWND_BROADCAST,m_ProcessMsg,GetCurrentProcessId(), 9);
// 	}
// 	else
// 	{
// 		::PostMessage(HWND_BROADCAST,m_ProcessMsg,GetCurrentProcessId(), 0);
// 	}

	return 0;
}
// 6. begin installing.
LRESULT CVBUpdaterDlg::OnBegininstall(WPARAM wParam, LPARAM lParam)
{
	static int cpos=0;
	cpos++;
	CString con;
	GetDlgItemTextW(IDC_AUH, con);
	
	con += _T("Start installing ... \r\n");
	SetDlgItemTextW(IDC_AUH, con);
	m_prog.SetPos(0);

	UpdateData(FALSE);

	return 0;
}
// 7. installed a file.
LRESULT CVBUpdaterDlg::OnInstallOne(WPARAM wParam, LPARAM lParam)
{
	static int cpos=0;
	cpos++;
	CString con, tmp;
	GetDlgItemTextW(IDC_AUH, con);
	
	tmp.Format(_T("installing %dth file \r\n"), cpos);
	con += tmp;
	SetDlgItemTextW(IDC_AUH, con);
	m_prog.SetPos(m_prog.GetPos() + (int)(100/m_cfc));

	UpdateData(FALSE);

	return 0;
}
// 8. over.

// for parameter 0.0
LRESULT CVBUpdaterDlg::OnInstallFinished(WPARAM wParam, LPARAM lParam)
{
// 
// 	m_AutoUpdate.AutoInstall();
// 	m_prog.SetPos(100);
// 	UpdateData(FALSE);

	CString con, tmp;
	GetDlgItemTextW(IDC_AUH, con);
	
	con += _T(" installed. \r\n");
	SetDlgItemTextW(IDC_AUH, con);
	m_prog.SetPos(100);

	UpdateData(FALSE);

	::PostMessage(HWND_BROADCAST, m_ProcessMsg, GetCurrentProcessId(), 5);

// 	// Call
// 	CString ViewBar = CMyUtility::GetCurDir();
// 	ViewBar += _T("\\ViewBar.exe");
// 	ShellExecute(NULL, _T("open"), ViewBar,NULL,NULL,SW_SHOW);
// 	
// 	PostQuitMessage(0);

	return 0;
}

// downloading files and when it finished notify ViewBar
UINT AuThread(LPVOID pParam)
{
	CoInitialize(NULL);	
	
	CVBUpdaterDlg * pThis = (CVBUpdaterDlg*)pParam;

	// downloading files
	BOOL bDownloadOkay = pThis->m_AutoUpdate.AutoDownLoad();

	// notify ViewBar downloading is complete
	if (bDownloadOkay)
	{
		::PostMessage(HWND_BROADCAST, pThis->m_ProcessMsg, GetCurrentProcessId(), 0);
	}
	

// 	if( theApp.m_bAutoInstall )
// 	{
// 		::PostMessage(HWND_BROADCAST, pThis->m_ProcessMsg,GetCurrentProcessId(), 9);
// 	}
// 	else
// 	{
// 		::PostMessage(HWND_BROADCAST, pThis->m_ProcessMsg,GetCurrentProcessId(), 0);
// 		// Quit self
// 	}
// 		//pThis->m_AutoUpdate.AutoInstall();
// 	pThis->m_auT = NULL;

	CoUninitialize();
	return true;
}

// thread for copying files
// for parameter 1.1 and 1.0
UINT AuThread2(LPVOID pParam)
{
	CoInitialize(NULL);	
	
	static int flag = 0;

	if (flag > 0)
	{
		return 0;
	}

	flag++;

	CVBUpdaterDlg * pThis = (CVBUpdaterDlg*)pParam;
	pThis->m_AutoUpdate.AutoInstall();
	pThis->m_auT = NULL;

	CoUninitialize();

	CString strVB = CMyUtility::GetCurDir() + _T("\\ViewBar.exe");
	if (theApp.m_bAutoInstall)
		ShellExecute(NULL, _T("open"), strVB, _T("1"), NULL,SW_SHOW);
	else
		ShellExecute(NULL, _T("open"), strVB, NULL,NULL,SW_SHOW);

	::PostMessage(pThis->GetSafeHwnd(), WM_QUIT,0,0);

	return true;
}





void CVBUpdaterDlg::reset()
{
	m_edtCon.SetWindowTextW(_T(""));
	m_prog.SetRange(0,100);
	m_prog.SetPos(0);
	m_btnStart.EnableWindow(TRUE);
	//m_btnCancel.EnableWindow(TRUE);
}

// Automatically update, do all stuffs for user
void CVBUpdaterDlg::StartDownload()
{
	AfxBeginThread(AuThread, this);	
}

void CVBUpdaterDlg::OnBnClickedStart()
{
	if (!m_auT)
	{
		m_edtCon.SetWindowTextW(_T(""));
		m_prog.SetPos(0);
		
		m_auT = AfxBeginThread(AuThread, this);
		m_btnStart.EnableWindow(FALSE);
		//m_btnCancel.EnableWindow(TRUE);
	}
}

void CVBUpdaterDlg::OnBnClickedCancel()
{
	if(m_auT)
	{
		m_auT->ExitInstance();
		m_auT = NULL;
		m_btnStart.EnableWindow(true);
		//m_btnCancel.EnableWindow(false);
	}
	// TODO: Add your control notification handler code here
}

void CVBUpdaterDlg::OnBnClickedOk()
{
	CString con;
	this->GetDlgItemTextW(IDC_AUH, con);
	con += _T("checking the status...");
	this->SetDlgItemTextW(IDC_AUH, con);	// TODO: Add your control notification handler code here
	//OnOK();
}


LRESULT CVBUpdaterDlg::OnProcessMsg(WPARAM wParam, LPARAM lParam)
{
	if( wParam == GetCurrentProcessId() )
		return 0;


	WaitForSingleObject((HANDLE)wParam, 5000);
	Sleep(1500);

 	m_AutoUpdate.SetParameters(CMyUtility::GetCurDir()+_T("\\update"), GetSafeHwnd());

	// user doesn't want to update now, so updater quits immediately
	if (lParam == 3)
	{
		PostQuitMessage(0);
		return 0;
	}

	// copy files
	AfxBeginThread(AuThread2, this);



// 	if( lParam == 9 )
// 	{
// 		// install and quit
// 		AfxBeginThread(AuThread2,this);
// 		return 0;
// 	}
// 
// 	// user clicked button NO
// 	if( lParam == 3 )
// 	{
// 		PostQuitMessage(0);
// 		return 0;
// 	}
// 
// 	if ( lParam == 0 )
// 	{
// 		this->ShowWindow(SW_SHOW);
// 
// 	}
// 
// 
// 	m_prog.SetPos(0);
// 	//m_AutoUpdate.SetParameters(CMyUtility::GetCurDir()+_T("\\update"),GetSafeHwnd());
// 
// 	//m_auT= AfxBeginThread(AuThread,this);
// 	m_btnStart.EnableWindow(TRUE);
// 	m_btnCancel.EnableWindow(TRUE);

	return 0;
}

LRESULT CVBUpdaterDlg::OnAutoDownload(WPARAM wParam,LPARAM lParam)
{
	BOOL bDone = (BOOL)lParam;
	if( !bDone )
	{
		CString con;
		GetDlgItemTextW(IDC_AUH, con);
		con += _T("Don't auto download");
		SetDlgItemTextW(IDC_AUH, con);
	}
	return 0;
}

LRESULT CVBUpdaterDlg::OnAutoInstall(WPARAM wParam, LPARAM lParam)
{
	BOOL bDone = (BOOL)lParam;
	if( !bDone )
	{
		CString con;
		GetDlgItemTextW(IDC_AUH, con);
		con += _T("Don't auto install");
		SetDlgItemTextW(IDC_AUH, con);
	}
	return 0;
}

