// ***************************************************************
//  CMyAppBar   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  A class derived from CAppBarDlg
//  The Appbar
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

//#include "Resource.h"
#include "ui/taskbar/ApBarDlg.h"
#include "ui/static/XColorStatic.h"
#include "explorer1.h"
#include "afxwin.h"
#include "MyUtility.h"
#include "ui/searchbox/SearchCombo.h"
#include "SearchBox.h"
#include "ui/button/BtnST.h"


// preference dialog
#include "PreferencesDlg.h"

// CMyAppBar dialog

class CSkinManager;
//class CPPgDisplay;

#define WM_NOTICE_SURF				WM_USER + 4100
#define WM_NOTICE_NETSTATE			WM_USER + 4101

class CMyAppBar : public CAppBarDlg, public IUIObserver
{
	DECLARE_DYNAMIC(CMyAppBar)

	friend class CSkinManager;
	friend class CDockFacade;
	friend class CSearchManager;
private:


public:
	CMyAppBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMyAppBar();

	void UpdateUI();	// update UI
	void LoadAffiliateList();

// Dialog Data
	enum { IDD = IDD_MYAPPBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	void OnMenuTools();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDock(UINT nID);
	afx_msg void OnBnClickedOk();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBarHide();
	afx_msg void OnBarTest();
	afx_msg void OnBarAglocowebsite();
	afx_msg void OnBarOptions();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNoticeAffiliate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNoticeSurf(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnMinmize();
	afx_msg void OnBnClickedBtnTools();
	afx_msg LRESULT OnMonitorKeyboardAndMOuse(WPARAM,LPARAM);
	afx_msg void OnStnClickedStaticEngineLogo();
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnStnClickedStaticArrow();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBarMyAccount();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);



private:
	BOOL InitSearchBox();
	BOOL InitPPg();
	BOOL InitDocking();
	BOOL InitControlPosition(void);				// calculate the position of all controls
	void InitToolTips();
	void ShowDefaultAds();

	virtual BOOL OnInitDialog();
	void SetPos(CWnd *pCtrl, CRect &rect);
	void SetPos(CWnd *pCtrl, CWnd *pAfterCtrl, CRect &rect);
	int GetViewBarPos();			// get the position of the view bar

	BOOL OnSkinMenu(int nMenuId);
	BOOL OnLangMenu(int nMenuId);
	BOOL OnToolsMenu(int nMenuId);
	BOOL OnFavoriteMenuIE(int nMenuId);
	BOOL OnFavoriteMenuFF(int nMenuId);
	BOOL OnFavoriteMenuAgloco(int nMenuId);
	BOOL OnSearchEngineChanging(int nMenuId);
	void PopSearchEngineMenu();


	void TravelTree(CTreeCtrl *pTree, int iMenuBase, HTREEITEM parentItem, CMenu &menu);
	void DrawRectangle(void);
	void EraseOldRect(void);

	void AddFavoriteMenu(CMenu &menu);
	void MathAffiliate(int id,const char* url);
	void NoticeUserAffiliate(CString message, BOOL bOK = FALSE);

private:
	//CStatic m_staticMisc;
	CStatic m_staticLogo;
	CStatic m_staticSearchBoxRight;
	CStatic m_staticSearchLogo;
	CStatic m_staticSearchLeft;
	CStatic m_staticSearchBottom;
	CStatic m_staticSearchTop;
	CStatic m_staticSearchArrow;

	CWnd * m_pParent;
	bool m_bIsLButtonDown;

	CRect m_rtViewBarRect;
	CPoint m_ptMousePosOnDown;	// pos of mouse when left click
	CStringArray m_arrayFavUrlIE;
	CStringArray m_arrayFavUrlFF;

	int m_nDockSide;	// dock side
	CExplorer1 m_ie;
	CSearchBox m_editSearch;

	CButtonST m_btnMinimize;
	CButtonST m_btnSearch;
	CButtonST m_btnTools;
	DECLARE_EVENTSINK_MAP()
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	HWND GetParentWebBrowserWnd(HWND hMyWnd);
	BOOL IsThisOurBrowser(HWND hMyWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


