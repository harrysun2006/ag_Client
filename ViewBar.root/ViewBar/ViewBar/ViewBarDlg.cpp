// ViewBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "ViewBarDlg.h"
#include "ParserLogin.h"
#include "Registry.h"
#include "utility/base64/CBase64.h"
#include "utility/md5/md5class.h"
#include "utility/des/MyAes.h"
#include "utility/http/MyHttps.h"
#include "XLogin.h"
#include "XLogout.h"
#include "XSendSurfTime.h"
#include "XAffiliate.h"
#include "utility/XmlNodeWrapper.h"
#include "AglocoFavorites.h"
#include "DlgMessage.h"
#include "AppBarManager.h"
#include "AdsManager.h"

// Added by Victor Li. 03/30-2007
// for keyboard and mouse detect.
#include "..\VBMonitor\VBMonitor.h"

#pragma warning(disable:4996)

#include "CppSQLiteManager.h"
#include "afxwin.h"


//#pragma comment(lib,"VBMonitor.lib")

using namespace http;
using namespace utility;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NOACTIONTIMER	120

#define INTERVAL_SENDSURFTIME	1000
#define TIMER_SURFTIME 1


//this is for autoupdate, let's do it every an hour.
#define INTERVAL_SENDDOWNLOAD  1000*60*60
#define TIMER_DOWNLOAD 2

#define INTERVAL_URLSTOREDB		1000*60*2
#define TIMER_URLSTORE 3

#define INTERVAL_URLSEND		INTERVAL_URLSTOREDB*2
#define TIMER_URLSEND 4

// for convenience, g_MainDlg is made globally.
CDlgLogin *g_MainDlg = NULL;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CStatic m_staticVersion;
public:
	afx_msg void OnStnClickedStaticVersion();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_staticVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)

	ON_STN_CLICKED(IDC_STATIC_VERSION, &CAboutDlg::OnStnClickedStaticVersion)
END_MESSAGE_MAP()


// CDlgLogin dialog




CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
	, m_pAppBar(NULL)
	, m_strUsername(_T(""))
	, m_pLoginThead(NULL)
	, m_bIsAutoLogin(FALSE)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// Added by Victor Li. 03/30-2007
	// Detect mouse and keyboard action
	// {
	m_MoniterFlag = -1;
//	m_dwNoActTime = 0;
	m_bNoAction = FALSE;
	m_dwLastSend = 0;
	m_bSetTime = FALSE;
	m_bNetBad = FALSE;

	m_UrlTrack = NULL;
	m_LastTimeAffiliate = 0;
	m_uMsgShellHook = 0;
	m_nMonitorReset = 0;
	// }

	// by Bruce.
	// this is very important, please don't touch it, please ...
	// this->au.hWnd = this->m_hWnd;
	//ut=NULL;
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ID, m_staticAccount);
	DDX_Control(pDX, IDC_STATIC_PASSWD, m_staticPassword);
	DDX_Control(pDX, IDC_STATIC_RMB_ME, m_staticNotAMember);
	DDX_Control(pDX, IDC_STATIC_SIGNUP, m_staticSignup);
	DDX_Text(pDX, IDC_EDIT_ID, m_strUsername);
	//DDX_Text(pDX, IDC_EDIT_PASSWD, m_strPassword);
	DDX_Control(pDX, IDC_CHECK_REMEMBER_ME, m_cbRememberMe);
	DDX_Control(pDX, IDC_CHECK_REMEMBER_ALL, m_cbRememberAll);
	DDX_Control(pDX, IDC_EDIT_PASSWD, m_editPasswd);
	DDX_Control(pDX, IDC_STATIC_REMEMBER_ME, m_staticRememberMe);
	DDX_Control(pDX, IDC_STATIC_REMEMBER_ALL, m_staticRememberAll);
}

BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDlgLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLogin::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SIGNIN, &CDlgLogin::OnBnClickedBtnSignin)
	ON_WM_NCHITTEST()
	ON_COMMAND(ID_POPUP_QUIT, &CDlgLogin::OnPopupQuit)
	ON_COMMAND(ID_POPUP_ABOUT, &CDlgLogin::OnPopupAbout)
	ON_COMMAND(ID_POPUP_HIDEVIEWBAR, &CDlgLogin::OnPopupHideviewbar)
	ON_BN_CLICKED(IDC_CHECK_REMEMBER_ME, &CDlgLogin::OnBnClickedCheckRememberMe)
	ON_BN_CLICKED(IDC_CHECK_REMEMBER_ALL, &CDlgLogin::OnBnClickedCheckRememberAll)
	ON_COMMAND(ID_POPUP_SHOWVB, &CDlgLogin::OnPopupShowvb)
	ON_WM_INITMENUPOPUP()
	ON_MESSAGE(WM_VB_LOGIN, OnViewBarLogin)
	ON_MESSAGE(WM_VB_MULTIPLE_LOGIN, OnAnotherUserLogin)
	ON_MESSAGE(WM_VB_FORCE_UPDATE, OnForceUpdate)
	ON_MESSAGE(WM_VB_FORCE_QUIT, OnForceQuit)
	ON_MESSAGE(WM_MONITOR_KEYBOARD,OnMonitorKeyboardAndMOuse)
	ON_MESSAGE(WM_MONITOR_MOUSE,OnMonitorKeyboardAndMOuse)
	ON_WM_TIMER()
	ON_COMMAND(ID_BAR_OPTIONS, &CDlgLogin::OnBarOptions)
//	ON_MESSAGE(WM_DOWNLOADFINISHED, &CDlgLogin::OnDownloaded)
//	ON_MESSAGE(WM_INSTALLFINISHED, &CDlgLogin::OnInstalled)
	ON_REGISTERED_MESSAGE(CMyUtility::m_uProcessMsg,OnProcessMessage)
	ON_COMMAND(ID_POPUP_SIGNIN, &CDlgLogin::OnPopupSignin)
	ON_COMMAND(ID_POPUP_SIGNOUT, &CDlgLogin::OnPopupSignout)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_COMMAND(ID_POPUP_SHOWMAINWINDOW, &CDlgLogin::OnPopupShowmainwindow)
	ON_REGISTERED_MESSAGE(CAppBarManager::m_uMessageId, OnMultiInstance)
END_MESSAGE_MAP()


void CDlgLogin::SetAutoLogin()
{
	m_bIsAutoLogin = TRUE;
}

void CDlgLogin::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CUISubject::GetInstance()->Attatch(this);
	
	// TODO: Add extra initialization here
	g_MainDlg = this;
	CAppBarManager::GetInstance()->Attach(this);
	InitUI();
	InitTrayIcon();
	InitUserInfo();

	m_UrlTrack = CUrlTrack::GetInstance();

	// auto login
	if (m_bIsAutoLogin)
	{
		StartLoginThread();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CDlgLogin::OnViewBarLogin(WPARAM wParam, LPARAM lParam)
{
	UNUSED_ALWAYS(lParam);

	if (wParam == LOGIN_SUCCESS)
	{
		// log
		g_log.OutputDebug(_T("-------------------------------------------SESSION START------------------------------------------"));
		g_log.OutputDebug(_T("Login successfully, CDlgLogin::OnViewBarLogin"));

 		thePrefs.LoadConfigFromRegistry(m_strUsername);
		CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
		if( SQLite )
		{
			thePrefs.SetLastTimeAff(SQLite->execQuery(TABLE_AFFILIATES,"select max(createDate)"));
			SQLite->AffilatesExecTableQuery(TABLE_AFFILIATES,"select max(createDate) "," ");
			/*if( 0 != SQLite->AffLiatesGetRowsNumber() )
			{
				thePrefs.SetLastTimeAff(SQLite->AffDetailsGetIntValue(0,0));
			}*/
		}
		AfxBeginThread(AffiliateReq,(LPVOID)this);
 		ShowViewBar(TRUE);
		thePrefs.UserLogin();

 		this->ShowWindow(SW_HIDE);
		SaveUserInfo();


		// check updates
		// AfxBeginThread(AutoUpdate, (LPVOID)this);
		StartUpdaterThread();
	}
	else
	if (wParam == LOGIN_FAILURE)
	{
		// log
		g_log.OutputDebug(_T("Failed to login, CDlgLogin::OnViewBarLogin"));

		CString msg, title;
		msg.LoadString(IDS_ERR_FAILED_TO_LOGIN);
		title.LoadString(IDS_ERR_LOGIN_TITLE);

		//CDlgMessage dlg(msg, msg);
		//dlg.DoModal();
		

		MessageBox(msg, title, MB_OK|MB_ICONSTOP);
	}

	// enable button 'login'
	GetDlgItem(IDC_BTN_SIGNIN)->EnableWindow(TRUE);

	return 0;
}

void CDlgLogin::StartUpdaterThread()
{
	AfxBeginThread(AutoUpdate, (LPVOID)this);
}

LRESULT CDlgLogin::OnForceUpdate(WPARAM wParam, LPARAM lParam)
{
	StartUpdaterThread();

	return 0;
}

LRESULT CDlgLogin::OnForceQuit(WPARAM wParam, LPARAM lParam)
{
	DestroyTimer(TRUE);

	CString title, msg;
	title.LoadString(IDS_WARNING_FORCEUPDATE_TITLE);
	msg.LoadString(IDS_WARNING_FORCEUPDATE_MSG);

	CMyCounter counter;
	int nRet = MessageBox(msg, title, MB_OK);

	PostMessage(WM_QUIT);
	DestroyViewBar(TRUE);

	return 0;
}

LRESULT CDlgLogin::OnAnotherUserLogin(WPARAM wParam, LPARAM lParam)
{
	UNUSED_ALWAYS(wParam);
	UNUSED_ALWAYS(lParam);

	// log
	g_log.OutputDebug(_T("User signed in another computer, CDlgLogin::OnAnotherUserLogin"));

	CString msg;
	CString title;
	msg.LoadString(IDS_WARNING_MULTILOGIN);
	title.LoadString(IDS_WARNING);
	
	MessageBox(msg, title, MB_ICONWARNING);

	OnPopupSignout();

	//DestroyViewBar(FALSE);
	//this->ShowWindow(SW_SHOW);
	//
	//UpdateTrayMenu();

	

	return 0;
}


void CDlgLogin::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDlgLogin::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgLogin::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDlgLogin::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	 OnOK();
}

BOOL CDlgLogin::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CDC MemDC;

	MemDC.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = MemDC.SelectObject(&m_Bitmap);
	BITMAP bm;
	CRect rect;
	this->GetClientRect(&rect);
	m_Bitmap.GetObject(sizeof(BITMAP), &bm);
	pDC->StretchBlt(0, 0, rect.Width(), rect.Height(),
		&MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	MemDC.SelectObject(pOldBmp);

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

BOOL CDlgLogin::InitUI(void)
{
	// load the background image
	CString dir;
	dir = CMyUtility::GetCurDir();

	dir = dir + _T("\\skins\\sign-in.bmp");

	HBITMAP hbmp= (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		dir, IMAGE_BITMAP,0,0, 
		LR_LOADFROMFILE); 

	ASSERT(hbmp);

	m_Bitmap.Attach(hbmp); 

	// change the color of all static controls
	COLORREF white = RGB(255, 255, 255);
	COLORREF green = RGB(0, 128, 0);
	COLORREF blue = RGB(0, 0, 255);

	m_staticAccount.SetBkColor(white);
	m_staticAccount.SetFontBold(TRUE);

	m_staticPassword.SetBkColor(white);
	m_staticPassword.SetFontBold(TRUE);

	m_staticNotAMember.SetBkColor(white);
	m_staticNotAMember.SetFontBold(TRUE);

	m_staticSignup.SetHyperLink(_T("http://www.agloco.com/web/guest/signup"));
	m_staticSignup.SetLink(TRUE, FALSE);
	m_staticSignup.SetTextColor(blue);
	m_staticSignup.SetBkColor(white);
	m_staticSignup.SetFontUnderline(TRUE);
	m_staticSignup.SetLinkCursor(LoadCursor(NULL,IDC_HAND));


	m_staticRememberMe.SetBkColor(white);
	m_staticRememberAll.SetBkColor(white);

	// set color of the checkbox
	//m_chxRememberMe.SetTransparent(FALSE);
	//m_chxRememberMe.SetBkColor(white);
	//m_chxRememberMe.SetParentRedraw(TRUE);

	// set the title
	AfxGetMainWnd()->SetWindowText(_T("AGLOCO Viewbar"));

	return TRUE;
}

// Load user configuration from registry
// the settings include user name, user password
// and whether user check 'remember me' and 'remember all' buttons or not
BOOL CDlgLogin::InitUserInfo()
{
	CRegistry reg;
	if (thePrefs.GetRememberFlag() == CPreferences::REMEMBER_ALL)	// 2 means all
	{
		m_strUsername = thePrefs.GetUsername();
		m_editPasswd.SetWindowText(thePrefs.GetPassword());
		m_editPasswd.SetRealText(thePrefs.GetPassword());
		m_cbRememberAll.SetCheck(TRUE);
		m_cbRememberMe.SetCheck(TRUE);
	}
	else
	if (thePrefs.GetRememberFlag() == CPreferences::REMEMBER_ME)	// 1 means 'remember me'
	{
		m_strUsername = thePrefs.GetUsername();
		m_cbRememberAll.SetCheck(FALSE);
		m_cbRememberMe.SetCheck(TRUE);
	}
	else
	{
		m_cbRememberAll.SetCheck(FALSE);
		m_cbRememberMe.SetCheck(FALSE);
	}

	UpdateData(FALSE);

	return TRUE;
}

// Save user configuration to registry
// the settings include user name, user password
// and whether user check 'remember me' and 'remember all' buttons or not
BOOL CDlgLogin::SaveUserInfo()
{
	thePrefs.SetUsername(m_strUsername);
	thePrefs.SetPassword(m_editPasswd.m_strRealText);

	CPreferences::REMEMBER_FLAG iRememberFlag = CPreferences::NOT_REMEMBER;
	if (m_cbRememberAll.GetCheck())
		iRememberFlag = CPreferences::REMEMBER_ALL;
	else
	if (m_cbRememberMe.GetCheck())
		iRememberFlag = CPreferences::REMEMBER_ME;

	thePrefs.SetRememberFlag(iRememberFlag);

	thePrefs.SaveConfigToRegistry();

	return TRUE;
}

// Create appbar and dock it on top or bottom of screen
BOOL CDlgLogin::ShowViewBar(BOOL bLogin)
{
	// log
	g_log.OutputDebug(_T("Create appbar and show it, CDlgLogin::ShowViewBar"));

	if (m_pAppBar == NULL)
	{
		m_pAppBar = new CMyAppBar(this);
		ASSERT (m_pAppBar);
		m_pAppBar->Create(IDD_MYAPPBAR);

		// bug fix by Charry
		this->ShowWindow(SW_HIDE);
	}
	else
	{
		//::AfxMessageBox (_T("I will only allow for one appbar for this example"));
	}

	StartTimer(bLogin);
	SetMessageHandle(m_pAppBar->GetSafeHwnd());
	//SetViewBarShowed(TRUE);

	UpdateTrayMenu();

	return TRUE;
}

// update tray icon menu
void CDlgLogin::UpdateTrayMenu()
{
	if (thePrefs.IsUserLoggedIn() == false)
	{
		m_TrayIcon.SetMenuDefaultItem(NULL, FALSE);
		return;
	}

	if (thePrefs.IsViewBarVisible())
	{
		m_TrayIcon.SetMenuDefaultItem(ID_POPUP_HIDEVIEWBAR, FALSE);
	}
	else
	{
		m_TrayIcon.SetMenuDefaultItem(ID_POPUP_SHOWVB, FALSE);
	}
}

void CDlgLogin::OnBnClickedBtnSignin()
{
	UpdateData();

	// check user's input
	if (m_strUsername == _T(""))
	{
		CString msg;
		msg.LoadString(IDS_ERR_EMPTY_USERNAME);
		MessageBox(msg);
		return;
	}

	CString strPasswd;
	m_editPasswd.GetWindowText(strPasswd);
	if (strPasswd == _T(""))
	{
		CString msg;
		msg.LoadString(IDS_ERR_EMPTY_PASSWD);
		MessageBox(msg);
		return;
	}

	// disable button 'login'
	GetDlgItem(IDC_BTN_SIGNIN)->EnableWindow(FALSE);

	StartLoginThread();
}

// Show a notification icon on taskbar(at the lower right corner of the screen)
BOOL CDlgLogin::InitTrayIcon(void)
{

	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));  // Icon to use
	if (!m_TrayIcon.Create(
		NULL,					// Let icon deal with its own messages
		WM_ICON_NOTIFY,			// Icon notify message to use
		_T("Agloco Viewbar - Own The Internet!"),  // tooltip
		hIcon,
		IDR_MENU_POPUP,			// ID of tray icon
		FALSE,
		NULL,					// balloon tip
		NULL,					// balloon title
		NIIF_INFO,				// balloon icon
		10 ))					// balloon timeout
	{
		return -1;
	}

	m_TrayIcon.SetMenuDefaultItem(NULL, FALSE);

	return TRUE;
}

// Enable dragging by click any places on main window
LRESULT CDlgLogin::OnNcHitTest(CPoint point)
{
	UINT nHitTest = (UINT)CDialog::OnNcHitTest(point); 
	return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;

	// return CDialog::OnNcHitTest(point);
}

// Quit ViewBar, after destroying ViewBar, make sure LogOut be called
void CDlgLogin::OnPopupQuit()
{
	if (thePrefs.IsWarningBeforeQuitAllowed())
	{
		CMyCounter counter;
		if (counter.IsEmpty())
		{
			CString title, msg;
			title.LoadString(IDS_WARNING_QUIT_TITLE);
			msg.LoadString(IDS_WARNING_QUIT_MSG);
			int nRet = ::MessageBox(NULL,
				msg, title, MB_YESNO|MB_ICONQUESTION);

			if (nRet == IDNO)
				return;
		}
	}

	QuitApp();
}

// Shutdown Viewbar
BOOL CDlgLogin::QuitApp()
{
	// log
	g_log.OutputDebug(_T("Shutdown Viewbar, CDlgLogin::QuitApp"));

	// remove notification icon
	m_TrayIcon.RemoveIcon();

	PostMessage(WM_QUIT);
	DestroyViewBar(TRUE);
	if (thePrefs.IsUserLoggedIn())
	{
		LogOut(TRUE);	
	}
	
	return TRUE;
}

// Destroy appbar
BOOL CDlgLogin::DestroyViewBar(BOOL bQuit)
{
	// log
	g_log.OutputDebug(_T("Destroy appbar, CDlgLogin::DestroyViewBar"));

	if (m_pAppBar)
	{
		m_pAppBar->SendMessage(WM_DESTROY, 0, 0L);
		delete m_pAppBar;
		m_pAppBar = NULL;
		thePrefs.ShowViewBar(false);	// flag to indicate VB is destroyed
	}

	// m_TrayIcon.SetMenuDefaultItem(ID_POPUP_SHOWVB, FALSE);
	UpdateTrayMenu();

	return TRUE;
}

// Update UI, this function is called when new language selected
void CDlgLogin::UpdateUI()
{
	if (m_hWnd)
	{
		GetDlgItem(IDC_STATIC_ID)->SetWindowText(GetResString(IDS_LOGIN_ACCOUNT));
		GetDlgItem(IDC_STATIC_PASSWD)->SetWindowText(GetResString(IDS_LOGIN_PASSWD));
		GetDlgItem(IDC_STATIC_REMEMBER_ME)->SetWindowText(GetResString(IDS_LOGIN_REMEMBERME));
		GetDlgItem(IDC_STATIC_REMEMBER_ALL)->SetWindowText(GetResString(IDS_LOGIN_REMEMBERALL));
		GetDlgItem(IDC_BTN_SIGNIN)->SetWindowText(GetResString(IDS_LOGIN_SIGNIN));
		GetDlgItem(IDC_STATIC_NOTAMEMBER)->SetWindowText(GetResString(IDS_LOGIN_NOTAMEMBER));
		GetDlgItem(IDC_STATIC_SIGNUP)->SetWindowText(GetResString(IDS_LOGIN_SIGNUP));
	}

	return;
}

// Handler for menu 'About'
void CDlgLogin::OnPopupAbout()
{
	CMyCounter couter;
	if (couter.IsEmpty())
	{
		CAboutDlg dlg;
		dlg.DoModal();
	}
}

void CDlgLogin::OnPopupHideviewbar()
{
	DestroyViewBar(FALSE);
}

void CDlgLogin::OnBnClickedCheckRememberMe()
{
	m_cbRememberAll.SetCheck(FALSE);
}

void CDlgLogin::OnBnClickedCheckRememberAll()
{
	if (m_cbRememberAll.GetCheck())
	{
		m_cbRememberMe.SetCheck(TRUE);
	}
}

void CDlgLogin::OnPopupShowvb()
{
	if (IsViewBarShowed() == FALSE)
	{
		ShowViewBar(FALSE);
	}
}

BOOL CDlgLogin::IsViewBarShowed()
{
	return m_pAppBar == NULL ? FALSE : TRUE;
}


void CDlgLogin::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	__super::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// show main window
	if (IsWindowVisible())
	{
		pPopupMenu->EnableMenuItem(ID_POPUP_SHOWMAINWINDOW, MF_DISABLED|MF_GRAYED);
	}
	else if (thePrefs.IsUserLoggedIn() == false)
	{
		pPopupMenu->EnableMenuItem(ID_POPUP_SHOWMAINWINDOW, MF_ENABLED);
	}
	else
		pPopupMenu->EnableMenuItem(ID_POPUP_SHOWMAINWINDOW, MF_DISABLED|MF_GRAYED);

	UpdateTrayMenu();

	// show viewbar/hide viewbar
	if (thePrefs.IsUserLoggedIn() == false)
	{
		pPopupMenu->EnableMenuItem(ID_POPUP_HIDEVIEWBAR, MF_DISABLED|MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_POPUP_SHOWVB, MF_DISABLED|MF_GRAYED);
	}
	else
	if (IsViewBarShowed())
	{
		pPopupMenu->EnableMenuItem(ID_POPUP_SHOWVB, MF_DISABLED|MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_POPUP_HIDEVIEWBAR, MF_ENABLED);
	}
	else
	{
		pPopupMenu->EnableMenuItem(ID_POPUP_SHOWVB, MF_ENABLED);
		pPopupMenu->EnableMenuItem(ID_POPUP_HIDEVIEWBAR, MF_DISABLED|MF_GRAYED);
	}

	// sign in/sign out
	if (thePrefs.IsUserLoggedIn())
	{
		pPopupMenu->EnableMenuItem(ID_POPUP_SIGNIN, MF_DISABLED|MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_POPUP_SIGNOUT, MF_ENABLED);
	}
	else
	{
		if (GetDlgItem(IDC_BTN_SIGNIN)->IsWindowEnabled())
		{
			pPopupMenu->EnableMenuItem(ID_POPUP_SIGNIN, MF_ENABLED);
		}
		else
		{
			// if login window is visible and user already clicked 'sign in', then disable 'sign in'
			pPopupMenu->EnableMenuItem(ID_POPUP_SIGNIN, MF_DISABLED|MF_GRAYED);
		}

		pPopupMenu->EnableMenuItem(ID_POPUP_SIGNOUT, MF_DISABLED|MF_GRAYED);
	}
}

void CDlgLogin::StartLoginThread()
{
	m_pLoginThead = AfxBeginThread(LoginThread, this);
}

UINT CDlgLogin::LoginThread(LPVOID pParam)
{
	// log
	g_log.OutputDebug(_T("Login Thread Start, CDlgLogin::LoginThread"));

	CoInitialize(NULL);

	CDlgLogin *pDlg  = static_cast<CDlgLogin*>(pParam);


 	WPARAM flag = (WPARAM)LOGIN_FAILURE;
	BOOL bFlag = FALSE;

	CString reason;
	bFlag = CXLogin::Login(pDlg->GetUsername(), pDlg->GetPassword(), reason);
	if (bFlag)
  	{
 		flag = LOGIN_SUCCESS;

		// save new password
		thePrefs.SetPassword(pDlg->GetPassword());
		pDlg->StartTimer(TRUE);
 	}
  	else
  	{
 		flag = (WPARAM)LOGIN_FAILURE;
  	}
 
 	// return login result
 	pDlg->SetLoginResult(reason);

	// notify CDlgLogin
	// replace PostMessage with SendMessage, by Charry
	// ::PostMessage((HWND)pDlg->GetSafeHwnd(), WM_VB_LOGIN, flag, NULL);
	::SendMessage((HWND)pDlg->GetSafeHwnd(), WM_VB_LOGIN, flag, NULL);
	
	CoUninitialize();

	// log
	g_log.OutputDebug(_T("Login Thread End, CDlgLogin::LoginThread"));

	return NULL;
}

void CDlgLogin::LogOut(BOOL bQuit/* = FALSE*/)
{
	DestroyViewBar(FALSE);
	DestroyTimer(bQuit);
	thePrefs.UserLogout();
	CXLogout::Logout();
	UpdateTrayMenu();
	if( !bQuit )
	{
		this->ShowWindow(SW_SHOW);

		// logout, the server time must be reset.
		thePrefs.SetServerTime(0);
	}

	// log
	g_log.OutputDebug(_T("----------------------------------------------------------SESSTION END------------------------------------------------"));

}

UINT CDlgLogin::AutoUpdate(LPVOID pParam)
{
	// log
	g_log.OutputDebug(_T("Auto updater thread start, CDlgLogin::AutoUpdate"));

	CoInitialize(NULL);
	CDlgLogin * tmp = (CDlgLogin*)pParam;
	//CAutoUpdate au;
	//CUpdater au;
	//au.AutoUpdate();
	CUpdater updater;
	updater.AutoUpdate();
	CoUninitialize();

	return 0;
}

// Added by Victor Li. 03/30-2007
// {
void CDlgLogin::OnTimer(UINT nIDEvent)
{
	if( TIMER_SURFTIME == nIDEvent )
	{
		OnSurfTimeProc();
	}
	else if( TIMER_URLSTORE == nIDEvent )
	{
		m_UrlTrack->StoreURLToDataBase();
	}
	else if( TIMER_URLSEND == nIDEvent )
	{
		m_UrlTrack->Send();
	}
	else if ( TIMER_DOWNLOAD ==nIDEvent)
	{
		// check updates for Viewbar and if necessary, download it 
		StartUpdaterThread();

		// download ad template
		CAdsManager::GetInstance()->DownloadAdTemplate();

		if( thePrefs.GetMemberId() )
			AfxBeginThread(AffiliateReq,(LPVOID)this);
	}
	__super::OnTimer(nIDEvent);
}


LRESULT CDlgLogin::OnMonitorKeyboardAndMOuse(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;

//	m_dwNoActTime = m_HiTimer.GetTotalLapseSeconds();
	return 0;
}

CString CDlgLogin::GetPassword()
{
	return m_editPasswd.m_strRealText;
}

void CDlgLogin::DestroyTimer(BOOL bStore )//= FALSE)
{
	g_log.OutputDebug(_T("DestroyTimer "),bStore);
	if( !m_bSetTime )
	{
		return;
	}
	
	KillTimer(TIMER_SURFTIME);
	KillTimer(TIMER_URLSTORE);
	KillTimer(TIMER_URLSEND);

	// 2007-5-8
	if( bStore )
	{
		if( m_uMsgShellHook )
		{
			DeregisterShellHookWindow(this->GetSafeHwnd());
			m_uMsgShellHook = 0;
		}
	}

	MonitorExit();
	m_UrlTrack->UTEndSniffer();
	
	m_bSetTime = FALSE;
	DWORD dwEnd = g_HiTimer.GetTotalLapseSeconds();

	if( !m_bNoAction && !m_bNetBad)
		m_SurfTime.AddSurfTime(m_dwStart,dwEnd,thePrefs.GetMemberId());

	if( bStore )
	{
		m_SurfTime.StoreSurfTimeInLocale();
		m_UrlTrack->StoreURLToDataBase();
	}
	else
	{
		m_SurfTime.Send(TRUE);
		m_UrlTrack->Send(TRUE);
	}
}

void CDlgLogin::StartTimer(BOOL bLogin)
{
	// Added by Victor Li. 03/30-2007
	// Detect mouse and keyboard action
	// {
	if( m_bSetTime )
		return;

	m_bSetTime = TRUE;
	if( bLogin )
		g_HiTimer.SetHiTimer();
	m_dwStart = g_HiTimer.GetTotalLapseSeconds();
	m_SurfTime.LoadStoredSurfTime();
	m_SurfTime.Send(FALSE);
	
	m_UrlTrack->LoadURLInLocale();
	m_UrlTrack->Send(FALSE);

	// Register shellhook message. 2007-5-8
	if( bLogin )
	{
		if( RegisterShellHookWindow(this->GetSafeHwnd()) )
			m_uMsgShellHook = RegisterWindowMessage(TEXT("SHELLHOOK"));
	}
	
	m_MoniterFlag = MonitorInit(GetSafeHwnd());
	
	// temporally remove URLTrack (by Charry)
	m_UrlTrack->UTStartSniffer(2,2);

	SetTimer(TIMER_SURFTIME,INTERVAL_SENDSURFTIME,NULL);
	SetTimer(TIMER_DOWNLOAD, INTERVAL_SENDDOWNLOAD,NULL);
	SetTimer(TIMER_URLSTORE,INTERVAL_URLSTOREDB,NULL);
	SetTimer(TIMER_URLSEND,INTERVAL_URLSEND,NULL);
	// } 
}
// }


// hander for popupmenu
void CDlgLogin::OnBarOptions()
{
	if (IsViewBarShowed())
	{
		CDockFacade::GetInstance()->GetDockInfo();
	}
	
	CPreferencesDlg dlg;
	int iRet = 0;

	if (CPreferencesDlg::bShow == FALSE)
	{
		CMyCounter counter;

		if (counter.IsEmpty())
		{
			iRet = dlg.DoModal();
		}
		CPreferencesDlg::bShow = FALSE;
	}

	if (iRet == IDOK)
	{
		thePrefs.SaveConfigToRegistry();
	}

}

UINT CDlgLogin::AffiliateReq(LPVOID lpParam)
{
	g_log.OutputDebug( _T("Affiliate Request ") );
	int LastTimeAffDetails = 0;
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	if(SQLite)
	{
		SQLite->AffDetailsExecTableQuery(TABLE_AFFDETIALS,"select max(createDate) ", " ");
		if( SQLite->AffDetailsGetRowsNumber())
		{
			LastTimeAffDetails = SQLite->AffDetailsGetIntValue(0,0);
		}
	}
	g_log.OutputDebug( _T("Affiliate Request LastTimeAffDetails "), LastTimeAffDetails );
	CDlgLogin* pThis = (CDlgLogin*)lpParam;
	CXAffiliate ReqAffiliate(pThis->m_LastTimeAffiliate,LastTimeAffDetails);
	CString strResponse;
	if( ReqAffiliate.Request(_T(SERVICE_AFFILIATE),strResponse) )
	{
		pThis->ParseAffiliateResponse(strResponse);
	}
	g_log.OutputDebug( _T("Affiliate Response ") + strResponse );
	return 0;
}

void CDlgLogin::ParseAffiliateResponse(CString &strResponse)
{
	// Parse update time
	// ???????????

	CoInitialize(NULL);

	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	if( SQLite == NULL )
		return;

	CXmlDocumentWrapper xml;
	xml.LoadXML(strResponse);

	CXmlNodeWrapper theNode(xml.AsNode());

	CXmlNodeWrapper affsNode(theNode.GetNode(_T("affiliates")));
	CXmlNodeListWrapper affs(affsNode.FindNodes(_T("affiliate")));
	for( int i=0; i<affs.Count(); i++)
	{
		CXmlNodeWrapper item = affs.Node(i);
		CString affiliatedId = item.GetValue(_T("affiliateId"));
		CString name = item.GetValue(_T("name"));
		if( name.IsEmpty() )
			name = _T(" ");
		CString createDate = item.GetValue(_T("createDate"));
		CString category = item.GetValue(_T("category"));
		if( category.IsEmpty() )
			category = _T(" ");

		CString strValue;
		strValue = affiliatedId + _T(",0,0,'") + name +_T("',") + createDate +_T(",'") + category + _T("'");
		char condition[64];
		sprintf(condition,"where affiliateId=\"%s\" ",affiliatedId);
		SQLite->AffilatesExecTableQuery(TABLE_AFFILIATES,"select * ",condition);
		if( 0 == SQLite->AffLiatesGetRowsNumber() )
			SQLite->InsertItem(TABLE_AFFILIATES,strValue);
		else
		{
			strValue = _T("set name='") + name + _T("'") +
				_T(",createDate=") + createDate +
				_T(",category='") + category +
				_T("' where affiliateId=") + affiliatedId;
			SQLite->UpdateItem(TABLE_AFFILIATES,strValue);
		}
	}

	CXmlNodeWrapper detailsNode(theNode.GetNode(_T("afDetails")));
	CXmlNodeListWrapper details(detailsNode.FindNodes(_T("afDetail")));
	for( int i=0; i<details.Count(); i++)
	{
		CXmlNodeWrapper item = details.Node(i);
		CString afDetailId = item.GetValue(_T("afDetailId"));
		CString affiliateId = item.GetValue(_T("affiliateId"));
		CString createDate = item.GetValue(_T("createDate"));
		CString pattern = item.GetValue(_T("pattern"));
		if( pattern.IsEmpty() )
			pattern = _T(" ");
		CString agPattern = item.GetValue(_T("agPattern"));
		if( agPattern.IsEmpty() )
			agPattern = _T(" ");
		CString category = item.GetValue(_T("category"));
		if( category.IsEmpty() )
			category = _T(" ");
		CString rebate = item.GetValue(_T("rebate"));
		CString status = item.GetValue(_T("status"));
		if( status.IsEmpty() )
			status = _T(" ");
		CString info = item.GetValue(_T("info"));
		if( info.IsEmpty() )
			info = _T(" ");
		CString description = item.GetValue(_T("description"));
		if( description.IsEmpty() )
			description = _T(" ");
		CString strValue;
		strValue = afDetailId + _T(",0,") + affiliateId +_T(",")
			+ createDate +_T(",'") + pattern +_T("','")
			+ agPattern +_T("','") + category +_T("','")
			+ rebate +_T("','") + status +_T("','")
			+ info +_T("','") + description + _T("'");
		char condition[64];
		sprintf(condition,"where afDetailId=\"%s\" and affiliateId=\"%s\" ",afDetailId,affiliateId);
		SQLite->AffDetailsExecTableQuery(TABLE_AFFDETIALS,"select * ",condition);
		if( 0 == SQLite->AffDetailsGetRowsNumber() )
			SQLite->InsertItem(TABLE_AFFDETIALS,strValue);
		else
		{
			strValue = _T("set createDate=") + createDate +
				_T(", pattern='") + pattern +
				_T("', agPattern='") + agPattern +
				_T("', category='") + category +
				_T("',rebate='") + rebate +
				_T("', status='") + status +
				_T("', info='") + info +
				_T("', description='") + description + _T("' ") +
				_T("where afDetailId=") + afDetailId + 
				_T(" and affiliateId=") + affiliateId;
			SQLite->UpdateItem(TABLE_AFFDETIALS,strValue);
		}
	}

	if( m_pAppBar != NULL )
	{
		m_pAppBar->LoadAffiliateList();
	}
	CoUninitialize();
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_staticVersion.SetWindowText(thePrefs.GetVersion());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// when viewbar is running and user tries to run another instance, the following function
// will be invoked.
LRESULT CDlgLogin::OnMultiInstance(WPARAM wParam, LPARAM lParam)
{
	if (wParam == GetCurrentProcessId())
	{
		return 0;
	}

	// active Viewbar
	CAppBarManager::GetInstance()->ActiveViewbar();

	return 0;
}

void CDlgLogin::ReportDownloadError()
{
	// download error
	CFile file;
	BOOL bOpen = FALSE;

	try
	{
		bOpen = file.Open(_T("./update.log"), CFile::modeRead);
		int iLen = file.GetLength();
		TCHAR *pBuffer = new TCHAR[iLen + 1];
		memset(pBuffer, 0, iLen + 1);
		file.Read(pBuffer, iLen);
		CString strFileName = pBuffer;
		delete [] pBuffer;

		// get real file name
		int iPos = strFileName.ReverseFind('\\');
		CString strRealFilename = strFileName.Mid(iPos + 1);

		CString msg, format;
		format.LoadString(IDS_ERR_DOWNLOAD_FAILURE);
		msg.Format(format, strRealFilename);

		CString title;
		title.LoadString(IDS_WARNING);

		CDlgMessage dlg(title, msg, IDI_EXCLAMATION);
		dlg.DoModal();
	}
	catch (CException* e)
	{
		if (bOpen)
		{
			file.Close();
		}
		e->Delete();
	}

	if (bOpen)
	{
		file.Close();
	}

	if (thePrefs.IsForceUpdate)
	{
		// quit Viewbar
		PostMessage(WM_QUIT);
	}
	
}

LRESULT CDlgLogin::OnProcessMessage(WPARAM wParam, LPARAM lParam)
{
	if( wParam == GetCurrentProcessId() )
		return 0;

	if (lParam == _VB_MSG_DOWNLOAD_FAILURE)
	{
		ReportDownloadError();

		return 0;
	}


	// update automatically
	if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_AUTO)
	{
		// save user's id and password for auto relogin
		thePrefs.SetRememberFlag(CPreferences::REMEMBER_ALL);
		thePrefs.SaveConfigToRegistry();

		// notify updater to copy files
		::PostMessage(HWND_BROADCAST, CMyUtility::m_uProcessMsg,GetCurrentProcessId(), 0);
		PostQuitMessage(0);
		return 0;
	}

	// ask user to confirm
	if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_SEMIAUTO)
	{
		CString msg;
		msg.LoadString(IDS_WARNING_NEW_VERSION);
		BOOL bReadyToQuit = AfxMessageBox(msg, MB_YESNO);
 		if (bReadyToQuit == IDNO)
 		{
 			// notify updater to close
			// '3' means : I don't want to update, the updater should quit immediately.
 			::PostMessage(HWND_BROADCAST, CMyUtility::m_uProcessMsg,GetCurrentProcessId(), 3);
 			return 0;
 		}
		else
		{
			// notify updater to copy files
			::PostMessage(HWND_BROADCAST, CMyUtility::m_uProcessMsg,GetCurrentProcessId(), 0);
			PostQuitMessage(0);
			return 0;
		}
	}

	// update manually, do nothing
	if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_MANNUALLY && lParam == 5)
	{
		CString msg;
		msg.LoadString(IDS_WARNING_UPDATE_CONFIRM);
		BOOL bReadyToQuit = AfxMessageBox(msg, MB_YESNO);
		if (bReadyToQuit == IDNO)
		{
			// notify updater to close
			// '3' means : I don't want to update, the updater should quit immediately.
			::PostMessage(HWND_BROADCAST, CMyUtility::m_uProcessMsg,GetCurrentProcessId(), 3);
			return 0;
		}
		else
		{
			// notify updater to copy files
			::PostMessage(HWND_BROADCAST, CMyUtility::m_uProcessMsg,GetCurrentProcessId(), 0);
			PostQuitMessage(0);
			return 0;
		}

		return 0;
	}

	return 0;
}

void CDlgLogin::OnPopupSignin()
{
	// disable button 'sign in' on login window
	GetDlgItem(IDC_BTN_SIGNIN)->EnableWindow(FALSE);
	StartLoginThread();
}

void CDlgLogin::OnPopupSignout()
{
	LogOut();
}
 
void CDlgLogin::OnDestroy()
{
	// Added by Victor Li. 03/30-2007
	// Detect mouse and keyboard action
	// {
	MonitorExit();
	m_UrlTrack->FreeInstance();

	//m_HiTimer.KillHiTimer();
	// }

	CUISubject::GetInstance()->Detatch(this);

	__super::OnDestroy();
	m_TrayIcon.RemoveIcon();
// 	::PostMessage(HWND_BROADCAST, 
// 		CMyUtility::m_uProcessMsg, 	
// 		GetCurrentProcessId(),
// 		0);   	
}


void CDlgLogin::SetMonitorParam()
{
//	m_dwNoActTime = m_HiTimer.GetTotalLapseSeconds();
}

// prevent from closing
BOOL CDlgLogin::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)   
	{   
		if (VK_ESCAPE == pMsg->wParam)   
		{
			return TRUE;   
		}   
	}

	return __super::PreTranslateMessage(pMsg);
}

#ifdef _DEBUG
BOOL IsBrowserWindowx(HWND hwnd)
{
	static TCHAR clsName[MAX_PATH] = {0};
	HWND hwndCur = hwnd;
	//while( hwndCur )
	{
		memset(clsName,0,sizeof(TCHAR)*MAX_PATH);
		GetClassName(hwndCur,clsName,MAX_PATH);
		//ATLTRACE(clsName);
		//ATLTRACE("\t[%X]\n",hwndCur);
		if( 0 == _tcscmp(clsName,_T("IEFrame")) )
		{
			return TRUE;
		}
		if( 0 == _tcscmp(clsName,_T("MozillaUIWindowClass")) )
		{
			return TRUE;
		}
		if( 0 == _tcscmp(clsName,_T("MozillaWindowClass")) )
		{
			return TRUE;
		}
		hwndCur = ::GetAncestor(hwndCur,GA_ROOTOWNER);
	}
	return FALSE;
}
#endif

void CDlgLogin::OnSurfTimeProc()
{
	if( m_nMonitorReset == 0)
	{
		MonitorExit();
		MonitorInit(NULL);		
	}
	m_nMonitorReset = ++m_nMonitorReset % 4;
	DWORD dwCurrent = g_HiTimer.GetTotalLapseSeconds();
	//DWORD NoActime = dwCurrent - m_dwNoActTime;
	DWORD NoActime = (GetTickCount() - GetLastTick())/1000;
#ifdef _DEBUG
	//ATLTRACE("current[%d], NoActime[%d]\n",dwCurrent,NoActime);
	//int a[5];
	//GetTestData(a);
	//ATLTRACE("hwnd[%X],PT[%d,%d],MS[%X],hwnd[%X],BOOL[%d]\n",a[0],a[1],a[4],a[2],a[3],IsBrowserWindowx((HWND)a[0]));
#endif
	//g_log.OutputDebug(_T("Current time"),dwCurrent);
	//g_log.OutputDebug(_T("NoAction time"),NoActime);

	if( (dwCurrent - m_dwLastSend) > thePrefs.GetNextSendTime() )//m_SurfTime.GetNextTime() )
	{
		if( !m_bNoAction && !m_bNetBad )
		{
			m_SurfTime.AddSurfTime(m_dwStart,dwCurrent,thePrefs.GetMemberId());
			m_dwStart = dwCurrent;
		}
		m_dwLastSend = dwCurrent;
		m_SurfTime.Send(FALSE);
		m_UrlTrack->Send(FALSE);
	}

	// bad net, query test request.
	if( !thePrefs.GetNetState() )
	{
		m_SurfTime.TestNetState();
	}

	// Net OK, and action
	if( !m_bNetBad && !m_bNoAction )
	{
		// net still ok
		if( thePrefs.GetNetState() )
		{
			// still action
			if( NoActime < NOACTIONTIMER )
			{

			}
			else // no action
			{
				// send noaction message
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				// add a item into time list
				m_SurfTime.AddSurfTime(m_dwStart,dwCurrent,thePrefs.GetMemberId());
				m_bNoAction = TRUE;
			}
		}
		else // net bad
		{
			// send net change state bad
			SendMessageToAppbar(WM_NOTICE_NETSTATE,m_bNetBad );

			// still action
			if( NoActime < NOACTIONTIMER )
			{
				m_SurfTime.AddSurfTime(m_dwStart,dwCurrent,thePrefs.GetMemberId());
			}
			else // no action
			{
				// send noaction message
				m_SurfTime.AddSurfTime(m_dwStart,dwCurrent,thePrefs.GetMemberId());
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_bNoAction = TRUE;
			}
			m_bNetBad = TRUE;
		}
	}
	// net OK, and noaction
	else if( !m_bNetBad && m_bNoAction )
	{
		// net still ok
		if( thePrefs.GetNetState() )
		{
			if( NoActime < NOACTIONTIMER )
			{
				// send action message
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_dwStart = dwCurrent;
				m_bNoAction = FALSE;
			}
			else // still noaction
			{
			}
		}
		else // net bad
		{
			// send bad net msg
			SendMessageToAppbar(WM_NOTICE_NETSTATE,m_bNetBad);
			// change active
			if( NoActime < NOACTIONTIMER )
			{
				// send active msg
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_bNoAction = FALSE;
			}
			else
			{

			}
			m_bNetBad = TRUE;
		}
	}
	// net bad and action
	else if( m_bNetBad && !m_bNoAction)
	{
		// net ok
		if( thePrefs.GetNetState() )
		{
			SendMessageToAppbar(WM_NOTICE_NETSTATE,m_bNetBad);
			// still action
			if( NoActime < NOACTIONTIMER )
			{
				m_dwStart = dwCurrent;
			}
			else // change to noaction
			{
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_bNoAction = TRUE;
			}
			m_bNetBad = FALSE;
		}
		else // net still bad
		{
			if( NoActime < NOACTIONTIMER )
			{

			}
			else
			{
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_bNoAction = FALSE;
			}
		}
	}
	else // bad net, and noaction
	{
		// net change to ok
		if( thePrefs.GetNetState() )
		{
			SendMessageToAppbar(WM_NOTICE_NETSTATE,m_bNetBad);
			// change to action
			if( NoActime < NOACTIONTIMER )
			{
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_bNoAction = FALSE;
				m_dwStart = dwCurrent;
			}
			else // still noaction
			{

			}
			m_bNetBad = FALSE;
		}
		else // net still bad
		{
			// change to action
			if( NoActime <  NOACTIONTIMER )
			{
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				m_bNoAction = FALSE;
			}
			else // still noaction
			{

			}
		}
	}

	/////////////////////////////////////////////////////////////
/*	if( thePrefs.GetNetState() )
	{
		if( m_bNetBad )
		{
			//send message net OK;
			SendMessageToAppbar(WM_NOTICE_NETSTATE,m_bNetBad );
			m_dwStart = dwCurrent; // start time
		}

		if( NoActime < 120 )
		{
			if( m_bNoAction )
			{
				// send message action;
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction );
				m_dwStart = dwCurrent;
			}
			m_bNoAction = FALSE;
		}
		else
		{
			if( !m_bNoAction )
			{
				// send message noaction;
				SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction );
				m_SurfTime.AddSurfTime(m_dwStart,dwCurrent,thePrefs.GetMemberId());
			}
			m_bNoAction = TRUE;
		}

		m_bNetBad = FALSE;
	}
	else
	{
		if( !m_bNetBad )
		{
			//send message net bad
			SendMessageToAppbar(WM_NOTICE_NETSTATE,m_bNetBad);
			if( NoActime < 120 )
			{
				if( !m_bNoAction )
				{
					m_SurfTime.AddSurfTime(m_dwStart,dwCurrent,thePrefs.GetMemberId());
				}
				else
				{
					// send message active
					SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
					m_bNoAction = FALSE;
				}
			}
			else
			{
				if( !m_bNoAction )
				{
					// send message noactive
					SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction);
				}
				m_bNoAction = TRUE;
			}
		}
		else
		{
			if( NoActime < 120 )
			{
				if( m_bNoAction )
				{
					// sedn message active
					SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction );
				}
				m_bNoAction = FALSE;
			}
			else
			{
				if( !m_bNoAction)
				{
					// send message noactive
					SendMessageToAppbar(WM_NOTICE_SURF,m_bNoAction );
				}
				m_bNoAction = TRUE;
			}
		}
		
		m_bNetBad = TRUE;
	}*/
}

void CDlgLogin::SendMessageToAppbar(UINT msg, BOOL wParam)
{
	//ATLTRACE("AppBar[%X]-PostMessage [%d],%d\n",m_pAppBar,msg,wParam);
	//g_log.OutputDebug(_T("Post Message to AppBar"),msg);
	if( m_pAppBar )
	{
		::PostMessage(m_pAppBar->GetSafeHwnd(),msg,(WPARAM)wParam,0);
	}
}

void CDlgLogin::OnClose()
{
//#define _TEST
#ifndef _TEST
	m_TrayIcon.MinimiseToTray(this, TRUE);
#else
	QuitApp(); // for test only
#endif	
}

void CDlgLogin::OnPopupShowmainwindow()
{
	m_TrayIcon.MaximiseFromTray(this, TRUE);
}

BOOL CDlgLogin::IsBrowserHwnd(HWND hwnd)
{
	HWND hwndCur = ::GetAncestor(hwnd,GA_ROOTOWNER);

	TCHAR clsName[MAX_PATH] = {0};
	GetClassName(hwndCur,clsName,MAX_PATH);
	if( ( _tcscmp(clsName,_T("IEFrame")) == 0 ) ||
		( _tcscmp(clsName,_T("MozillaUIWindowClass")) == 0 ) || 
		( _tcscmp(clsName,_T("MozillaWindowClass")) == 0) )
	{
		return TRUE;
	}
	return FALSE;
}

LRESULT CDlgLogin::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == m_uMsgShellHook )
	{
		switch( wParam )
		{
		case HSHELL_WINDOWCREATED:
			if( IsBrowserHwnd((HWND)lParam) )
			{
				if( !IsExistBrowser() )
				{
					//g_log.OutputDebug(_T("Not Exist Browser"));
					OnPopupShowvb();
				}
				else
				{
					//g_log.OutputDebug(_T("Exist Browser"));
				}
			}
			break;
		//case HSHELL_WINDOWDESTROYED: //2
		//	if( IsBrowserHwnd((HWND)lParam) )
		//	{
		//		strCmd = _T("HSHELL_WINDOWDESTROYED");
		//	}
		//	break;
		case HSHELL_WINDOWACTIVATED: // 4
		case HSHELL_REDRAW:
			if( IsBrowserHwnd((HWND)lParam) )
			{
				GetBrowserTitle((HWND)lParam);
			}
			break;
		}
	}

	//
	// When screen resolution changes, Viewbar should arrange it's position
	// 
	/*
	if (message == WM_DISPLAYCHANGE)
	{
		// 
		DestroyViewBar(FALSE);
		ShowViewBar();
	}
	*/

	return __super::WindowProc(message, wParam, lParam);
}

BOOL CDlgLogin::IsExistBrowser()
{
	HWND hwnd = NULL;
	int nCount = 0;
	hwnd = ::FindWindowEx(NULL,hwnd,_T("IEFrame"),NULL);
	while( hwnd )
	{
		nCount++;
		if( nCount > 1 )
			return TRUE;
		hwnd = ::FindWindowEx(NULL,hwnd,_T("IEFrame"),NULL);
	}

	hwnd = ::FindWindowEx(NULL,hwnd,_T("MozillaUIWindowClass"),NULL);
	while( hwnd )
	{
		nCount++;
		if( nCount > 1 )
			return TRUE;
		hwnd = ::FindWindowEx(NULL,hwnd,_T("MozillaUIWindowClass"),NULL);
	}

	return FALSE;
}

void CDlgLogin::GetBrowserTitle(HWND hwnd)
{
	// - Microsoft Internet Explorer
	//Mozilla Firefox
	// - Mozilla Firefox
	// deliminate "http://......."
	// To be decided:
	/*
	How to retrieve key words from title string?
	Where are key words stored?
	How to get key words stored in AD library before?
	How to match key words?
	*/
	HWND hwndCur = ::GetAncestor(hwnd,GA_ROOTOWNER);

	TCHAR clsName[MAX_PATH] = {0};
	int nLen = ::GetWindowText(hwndCur, clsName,MAX_PATH);
	if( nLen > 0 )
	{
		//clsName[nLen] = _T('\0');
		TRACE(_T("%s\n"),clsName);
	}
	else
	{
	}
}
void CAboutDlg::OnStnClickedStaticVersion()
{
	// TODO: Add your control notification handler code here
}
