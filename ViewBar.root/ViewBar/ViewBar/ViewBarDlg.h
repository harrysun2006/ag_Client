// ViewBarDlg.h : header file
//

#pragma once

#include "afxwin.h"

// for hyperlink static controls
#include "ui/static/Label.h"
#include "MyAppBar.h"
#include "ui/trayicon/SystemTray.h"

#include "HiTimer.h"
#include "SurfTime.h"
#include "Updater.h"
#include "UrlTrack.h"
#include "ui/edit/SecureEdit.h"
#include "MyCounter.h"

#define WM_ICON_NOTIFY			WM_APP + 10
#define WM_VB_LOGIN				WM_USER + 2008
#define WM_VB_MULTIPLE_LOGIN	WM_USER + 2009
#define WM_VB_FORCE_UPDATE		WM_USER + 2030
#define WM_VB_FORCE_QUIT		WM_USER + 2031

#define LOGIN_SUCCESS 0 
#define LOGIN_FAILURE -1


// CViewBarDlg dialog
class CDlgLogin : public CDialog, public IUIObserver
{
	friend class CAppBarManager;
// Construction
public:
	CDlgLogin(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_LOGIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

CSystemTray m_TrayIcon;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void UpdateUI();	// update UI
	CString GetUsername()		{ return m_strUsername; }
	CString GetPassword();
	void SetLoginResult(CString &strLoginResult) {m_strLoginResult = strLoginResult;}
	CString GetLoginResult()	{ return m_strLoginResult; }
	void UpdateTrayMenu();
	void SetAutoLogin();

private:
	BOOL InitUI(void);
	BOOL ShowViewBar(BOOL bLogin);
	BOOL InitUserInfo(void);
	BOOL SaveUserInfo(void);
	BOOL IsViewBarShowed(void);
	void StartLoginThread(void);	// start login thread
	void StartUpdaterThread(void);
	void LogOut(BOOL bQuit = FALSE);
	BOOL QuitApp();

private:
	BOOL m_bIsAutoLogin;

	CLabel m_staticAccount;
	CLabel m_staticPassword;
	CLabel m_staticNotAMember;
	CLabel m_staticSignup;
	CLabel m_staticRememberMe;
	CLabel m_staticRememberAll;

	CString m_strLoginResult;

	CBitmap m_Bitmap;	// the background image

	CString m_strUsername;
	//CString m_strPassword;
	CButton m_cbRememberMe;
	CButton m_cbRememberAll;

	CWinThread *m_pLoginThead;

	friend class CMyAppBar;
	CMyAppBar *m_pAppBar;
	
private:
	BOOL InitTrayIcon(void);
	BOOL DestroyViewBar(BOOL bQuit);
	static UINT LoginThread(LPVOID pParam);
	static UINT AutoUpdate(LPVOID pParam);
	void ReportDownloadError();

	// Added by Victor Li. 03/30-2007
	// Detect mouse and keyboard action
	// {
	afx_msg LRESULT OnMonitorKeyboardAndMOuse(WPARAM wParam, LPARAM lParam);
	int m_MoniterFlag;
	//DWORD m_dwNoActTime;
	CSurfTime m_SurfTime;
	BOOL m_bNoAction;
	BOOL m_bNetBad;
	DWORD m_dwLastSend;
	DWORD m_dwStart;
public:
	void StartTimer(BOOL bLogin);
	void DestroyTimer(BOOL bStore);// = FALSE);
private:
	BOOL m_bSetTime; // Flag to set calculate, True is set, FALSE unset;


	CUrlTrack *m_UrlTrack;

	static UINT AffiliateReq(LPVOID lpParam);
	DWORD m_LastTimeAffiliate;
	void ParseAffiliateResponse(CString &strResponse);

	afx_msg LRESULT OnProcessMessage(WPARAM wParam, LPARAM lParam);

	void OnSurfTimeProc();
	void SendMessageToAppbar(UINT msg, BOOL);
	UINT m_uMsgShellHook; // for register shell hook message
	BOOL IsExistBrowser(); // Are there some browser runing: IE, FireFox.
	BOOL IsBrowserHwnd(HWND hwnd); 
	int m_nMonitorReset;
	void GetBrowserTitle(HWND hwnd);
public:
	void SetMonitorParam();
	// }

//	afx_msg void OnDestroy();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPopupQuit();
	afx_msg void OnPopupAbout();
	afx_msg void OnPopupHideviewbar();
	afx_msg void OnBnClickedCheckRememberMe();
	afx_msg void OnBnClickedCheckRememberAll();
	afx_msg void OnPopupShowvb();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg LRESULT OnViewBarLogin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAnotherUserLogin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnForceUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnForceQuit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnSignin();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBarOptions();
	afx_msg LRESULT OnMultiInstance(WPARAM wParam, LPARAM lParam);	// message handler for multi-instance

	// the following two functions is called after downloaded or installed.
	//afx_msg LRESULT OnDownloaded(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT OnInstalled(WPARAM wParam, LPARAM lParam);

public:
	//CWinThread *ut;
public:
	CSecureEdit m_editPasswd;

public:
	afx_msg void OnPopupSignin();
public:
	afx_msg void OnPopupSignout();
public:
	afx_msg void OnDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);



public:
	afx_msg void OnClose();
public:
	afx_msg void OnPopupShowmainwindow();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

extern CDlgLogin *g_MainDlg;
