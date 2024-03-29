// MyAppBar.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "MyAppBar.h"

#include "LangManager.h"

#include "ViewBarDlg.h"
#include "SearchManager.h"
#include "AdsManager.h"
#include "utility/FavoriteMenu.h"
#include "module/favorites/NetscapteBookmark.h"

#include "..\VBSniffer\VBSniffer.h"
#include "CppSQLiteManager.h"
#include "AglocoFavorites.h"
#include "MyCounter.h"

#include "mshtml.h"
#include <atlrx.h>
#include "../VBMonitor/VBMonitor.h"
const int MENU_SKIN_BASE			= 90009;
const int MENU_LANG_BASE			= 91009;
const int MENU_TOOL_BASE			= 92009;

const int MENU_FAV_IE_BASE			= 93009;
const int MENU_FAV_FF_BASE			= 94009;
const int MENU_FAV_AGLOCO_BASE		= 95009;

const int MENU_SEARCH_ENGINE_BASE	= 96009;	// search engine

#pragma warning(disable:4996)
#pragma warning(disable:4311)
// CMyAppBar dialog

// for refreshing ads
#define TIMER_REFRESH_AD	1234
#define REFRESH_INTERVAL	5*60*1000

IMPLEMENT_DYNAMIC(CMyAppBar, CAppBarDlg)

CMyAppBar::CMyAppBar(CWnd* pParent /*=NULL*/)
: CAppBarDlg(CMyAppBar::IDD, pParent)
, m_nDockSide(ABE_BOTTOM)
//, m_SkinManager(this)
, m_pParent(pParent)
, m_bIsLButtonDown(false)
{	
}

CMyAppBar::~CMyAppBar()
{
	KillTimer(TIMER_REFRESH_AD);
}

void CMyAppBar::DoDataExchange(CDataExchange* pDX)
{
	CAppBarDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_ie);
//	DDX_Control(pDX, IDC_CMB_SEARCH, m_cbxSearch);
	DDX_Control(pDX, IDC_STATIC_LOGO, m_staticLogo);
	//DDX_Control(pDX, IDC_STATIC_MISC, m_staticMisc);
	DDX_Control(pDX, IDC_BTN_MINMIZE, m_btnMinimize);
	DDX_Control(pDX, IDC_BTN_TOOLS, m_btnTools);
	DDX_Control(pDX, IDC_STATIC_SEARCHBOX_RIGHT, m_staticSearchBoxRight);
	DDX_Control(pDX, IDC_BTN_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_STATIC_ENGINE_LOGO, m_staticSearchLogo);
	DDX_Control(pDX, IDC_STATIC_S_LEFT, m_staticSearchLeft);
	DDX_Control(pDX, IDC_STATIC_S_BOTTOM, m_staticSearchBottom);
	DDX_Control(pDX, IDC_STATIC_S_TOP, m_staticSearchTop);
	DDX_Control(pDX, IDC_STATIC_ARROW, m_staticSearchArrow);
	DDX_Control(pDX, IDC_EDIT_SEARCHBOX, m_editSearch);
}


BEGIN_MESSAGE_MAP(CMyAppBar, CAppBarDlg)
	ON_BN_CLICKED(IDCANCEL, &CMyAppBar::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMyAppBar::OnBnClickedOk)
	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(ID_DOCK_TOP, ID_DOCK_BOTTOM, OnDock)
	//ON_COMMAND(ID_BAR_QUIT, &CMyAppBar::OnBarQuit)
	ON_COMMAND(ID_BAR_HIDE, &CMyAppBar::OnBarHide)
	ON_COMMAND(ID_BAR_TEST, &CMyAppBar::OnBarTest)
	ON_COMMAND(ID_BAR_AGLOCOWEBSITE, &CMyAppBar::OnBarAglocowebsite)
	ON_COMMAND(ID_BAR_OPTIONS, &CMyAppBar::OnBarOptions)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_NOTICE_AFFILIATE,OnNoticeAffiliate)
	ON_MESSAGE(WM_NOTICE_SURF,OnNoticeSurf)
	ON_BN_CLICKED(IDC_BTN_MINMIZE, &CMyAppBar::OnBnClickedBtnMinmize)
	ON_BN_CLICKED(IDC_BTN_TOOLS, &CMyAppBar::OnBnClickedBtnTools)
	ON_MESSAGE(WM_MONITOR_KEYBOARD,OnMonitorKeyboardAndMOuse)
	ON_MESSAGE(WM_MONITOR_MOUSE,OnMonitorKeyboardAndMOuse)
	ON_STN_CLICKED(IDC_STATIC_ENGINE_LOGO, &CMyAppBar::OnStnClickedStaticEngineLogo)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CMyAppBar::OnBnClickedBtnSearch)
	ON_STN_CLICKED(IDC_STATIC_ARROW, &CMyAppBar::OnStnClickedStaticArrow)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_BAR_MY_ACCOUNT, &CMyAppBar::OnBarMyAccount)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMyAppBar message handlers

void CMyAppBar::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
}

void CMyAppBar::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}

void CMyAppBar::ShowDefaultAds()
{
	CString html;
	html = CMyUtility::GetCurDir();
	html += _T("\\ads\\default-ad.html");

	m_ie.Navigate(html, NULL, NULL, NULL, NULL);
}

void CMyAppBar::UpdateUI()
{
	InitToolTips();

	return;
}

BOOL CMyAppBar::OnInitDialog()
{
	__super::OnInitDialog();

	CUISubject::GetInstance()->Attatch(this);

	// VB is visible
	thePrefs.ShowViewBar(true);		// flag to indicate VB is created

	// get position of all controls and put'em in the right places.
	InitControlPosition();
	InitSearchBox();
	InitDocking();
	InitToolTips();

	// set control style
	m_staticLogo.ModifyStyle(0xF, SS_BITMAP|SS_CENTERIMAGE);
	m_staticSearchBoxRight.ModifyStyle(0xF, SS_BITMAP|SS_CENTERIMAGE);
	m_staticSearchLogo.ModifyStyle(0xF, SS_ICON);
	m_staticSearchLeft.ModifyStyle(0xF, SS_BITMAP|SS_CENTERIMAGE);
	m_staticSearchTop.ModifyStyle(0xF, SS_BITMAP|SS_CENTERIMAGE);
	m_staticSearchBottom.ModifyStyle(0xF, SS_BITMAP|SS_CENTERIMAGE);
	m_staticSearchArrow.ModifyStyle(0xF, SS_ICON);

	CDockFacade::GetInstance()->SetTarget(this);

	CSearchManager::GetInstance()->AttachAppBar(this);

	// attach IE to CAdsManager
	CAdsManager::GetInstance()->AttachControl(&m_ie);

	CSkinManager::GetInstance()->SetTarget(this);

	CSkinManager::GetInstance()->LoadSkins(thePrefs.GetDefaultSkin());

	InitPPg();	// initialize preferences dialog

	// ShowDefaultAds();
	CAdsManager::GetInstance()->ShowAds();

	CSearchManager::GetInstance()->SetSearchEnglineLogo();

	SetTimer(TIMER_REFRESH_AD, REFRESH_INTERVAL, NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyAppBar::InitToolTips()
{
	// set tool tips for buttons
	int iTime = 30;
	CString tips;
	tips.LoadString(IDS_TOOLTIPS_SEARCH);
	m_btnSearch.SetTooltipText(tips);
	m_btnSearch.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, iTime);

	tips.LoadString(IDS_TOOLTIPS_TOOLS);
	m_btnTools.SetTooltipText(tips);
	m_btnTools.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, iTime);

	tips.LoadString(IDS_TOOLTIPS_TRAYICON);
	m_btnMinimize.SetTooltipText(tips);
	m_btnMinimize.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, iTime);

}

BOOL CMyAppBar::InitControlPosition()
{

#define MY_SIZE_BUTTON_MIN_X		32
#define MY_SIZE_BUTTON_MIN_Y		30
#define MY_SIZE_BUTTON_TOOL_X		45
#define MY_SIZE_BUTTON_TOOL_Y		30
#define MY_SIZE_BUTTON_SEARCH_X		44
#define MY_SIZE_BUTTON_SEARCH_Y		30



#define MY_SIZE_LOGO_TOP_X			167
#define MY_SIZE_LOGO_TOP_Y			6
#define MY_SIZE_LOGO_BOTTOM_X		167
#define MY_SIZE_LOGO_BOTTOM_Y		7

#define MY_SIZE_LOGO_LEFT_X			17
#define MY_SIZE_LOGO_LEFT_Y			30

#define MY_SIZE_COMBO_SEARCH_X		(MY_SIZE_LOGO_TOP_X - 26)
#define MY_SIZE_COMBO_SEARCH_Y		17

#define MY_SIZE_COMBO_SEARCHBOX_RIGHT_X 3

	// set position for logo
	CRect rtLogo = thePrefs.GetLogoRect();
	SetPos(&m_staticLogo, rtLogo);

	CRect rtBar;
	// get rect of all bar
	GetClientRect(rtBar);

	// 	// right place
	CRect rtMisc = thePrefs.GetMiscRect();
	rtMisc.left = rtBar.right - rtMisc.Width();
	rtMisc.right = rtBar.right;
	//	SetPos(&m_staticMisc, rtMisc);

	// set position for minimize button
	CRect rtBtnMin(rtBar.BottomRight(), CSize(MY_SIZE_BUTTON_MIN_X, MY_SIZE_BUTTON_MIN_Y));
	rtBtnMin.OffsetRect(CSize(-MY_SIZE_BUTTON_MIN_X, -MY_SIZE_BUTTON_MIN_Y));
	SetPos(&m_btnMinimize, rtBtnMin);

	// set position for tool menu
	CRect rtBtnTools(rtBtnMin.BottomRight(), CSize(MY_SIZE_BUTTON_TOOL_X, MY_SIZE_BUTTON_TOOL_Y));
	rtBtnTools.OffsetRect(CSize(-(MY_SIZE_BUTTON_TOOL_X + MY_SIZE_BUTTON_MIN_X), -MY_SIZE_BUTTON_TOOL_Y));
	SetPos(&m_btnTools, rtBtnTools);

	// set position for button search
	CRect rtBtnSearch(rtBtnMin.BottomRight(), CSize(MY_SIZE_BUTTON_SEARCH_X, MY_SIZE_BUTTON_SEARCH_Y));
	rtBtnSearch.OffsetRect( CSize(-(MY_SIZE_BUTTON_SEARCH_X + MY_SIZE_BUTTON_TOOL_X + MY_SIZE_BUTTON_MIN_X),
		-MY_SIZE_BUTTON_SEARCH_Y) );
	SetPos(&m_btnSearch, rtBtnSearch);

	// set position for search box static (background for CComboBox)
	// CRect rtSearchStatic = thePrefs.GetSearchRect();
	//CRect rtSearchStatic = CRect(0, MY_SIZE_LOGO_TOP_Y, MY_SIZE_COMBO_SEARCH_X , MY_SIZE_COMBO_SEARCH_Y);
	//rtSearchStatic.left = rtBtnSearch.left - MY_SIZE_COMBO_SEARCH_X;//rtSearchStatic.Width();
	//rtSearchStatic.right = rtBtnSearch.left;
	//SetPos(&m_staticSearchBox, rtSearchStatic);
	//m_staticSearchBoxRight;
	// set position for search box (CComboBox control)
	//CRect rtSearchBox = rtSearchStatic;

	CRect rtSearchBox;
	rtSearchBox.left = rtBtnSearch.left - MY_SIZE_COMBO_SEARCH_X;
	rtSearchBox.right = rtBtnSearch.left - MY_SIZE_COMBO_SEARCHBOX_RIGHT_X;
	rtSearchBox.top = MY_SIZE_LOGO_TOP_Y;
	rtSearchBox.bottom = rtBar.bottom - MY_SIZE_LOGO_BOTTOM_Y;
	CRect rtSearchBoxOffset = rtSearchBox;
	rtSearchBoxOffset.OffsetRect(5, 0);
	rtSearchBoxOffset.right -= 5;
	//rtSearchBoxOffset.right += 6;
	//rtSearchBoxOffset.bottom += 3;
	//SetPos(&m_cbxSearch, rtSearchBoxOffset);
	SetPos(&m_editSearch, rtSearchBoxOffset);

	CRect rtSearchBoxRight;
	rtSearchBoxRight.top = rtSearchBox.top;
	rtSearchBoxRight.bottom = rtBar.bottom - MY_SIZE_LOGO_BOTTOM_Y;
	rtSearchBoxRight.left = rtBtnSearch.left - MY_SIZE_COMBO_SEARCHBOX_RIGHT_X;
	rtSearchBoxRight.right = rtBtnSearch.left;
	SetPos(&m_staticSearchBoxRight, rtSearchBoxRight);

	// set position for search engine logo(top)
	CRect rtEngineMenuTop;
	rtEngineMenuTop.left = rtBtnSearch.left - MY_SIZE_LOGO_TOP_X;
	rtEngineMenuTop.right = rtBtnSearch.left;
	rtEngineMenuTop.top = 0;
	rtEngineMenuTop.bottom = MY_SIZE_LOGO_TOP_Y;
	SetPos(&m_staticSearchTop, rtEngineMenuTop);

	// set position for search engine logo(bottom)
	CRect rtEngineMenuBottom;
	rtEngineMenuBottom.left = rtBtnSearch.left - MY_SIZE_LOGO_TOP_X;
	rtEngineMenuBottom.right = rtBtnSearch.left;
	rtEngineMenuBottom.top = rtBar.bottom - MY_SIZE_LOGO_BOTTOM_Y;
	rtEngineMenuBottom.bottom = rtBar.bottom;
	SetPos(&m_staticSearchBottom, rtEngineMenuBottom);

	// set position for search engine logo(middle)
	CRect rtEngineMenu;
	rtEngineMenu.left = rtEngineMenuTop.left;
	rtEngineMenu.right = rtEngineMenu.left + 16;	// 16x16 icon
	rtEngineMenu.top = rtEngineMenuTop.bottom;
	rtEngineMenu.bottom = rtEngineMenuBottom.top;
	SetPos(&m_staticSearchLogo, rtEngineMenu);

	// set position for search engine logo(arrow)
	CRect rtEngineMenuArrow;
	rtEngineMenuArrow.left = rtEngineMenu.right;
	//rtEngineMenuArrow.right = rtSearchStatic.left;
	rtEngineMenuArrow.right = rtEngineMenuArrow.left + 10;// width of icon 'arrow'
	rtEngineMenuArrow.top = rtEngineMenuTop.bottom;
	rtEngineMenuArrow.bottom = rtEngineMenuBottom.top;
	SetPos(&m_staticSearchArrow, rtEngineMenuArrow);

	// set position for search engine logo(left)
	CRect rtSearchEngineLeft;
	rtSearchEngineLeft.left = rtEngineMenuTop.left - MY_SIZE_LOGO_LEFT_X;
	rtSearchEngineLeft.right = rtEngineMenuTop.left;
	rtSearchEngineLeft.top = 0;
	rtSearchEngineLeft.bottom = MY_SIZE_LOGO_LEFT_Y;
	SetPos(&m_staticSearchLeft, rtSearchEngineLeft);

	// set position for ads
	CRect rtAds = thePrefs.GetAdsRect();
	rtAds.left = rtLogo.right;
	rtAds.right = rtSearchEngineLeft.left;
	SetPos(&m_ie, rtAds);
	//m_ie.BringWindowToTop();
	//m_ie.SetParent(this);

	return TRUE;
}

void CMyAppBar::SetPos(CWnd *pCtrl, CRect &rect)
{
	::SetWindowPos(pCtrl->GetSafeHwnd(), NULL,
		rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
}

void CMyAppBar::SetPos(CWnd *pCtrl, CWnd *pAfterCtrl, CRect &rect)
{
	::SetWindowPos(pCtrl->GetSafeHwnd(), pAfterCtrl->GetSafeHwnd(),
		rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
}


void CMyAppBar::OnContextMenu(CWnd* pWnd, CPoint point)
{

	int iCtrlId = pWnd->GetDlgCtrlID();
	if (iCtrlId == IDC_BTN_TOOLS	||
		iCtrlId == IDC_BTN_MINMIZE	||
		iCtrlId == IDC_BTN_SEARCH	||
		iCtrlId == IDC_STATIC_ARROW	||
		iCtrlId == IDC_STATIC_SE_MENU ||
		iCtrlId == IDC_STATIC_S_TOP ||
		iCtrlId == IDC_STATIC_S_BOTTOM ||
		iCtrlId == IDC_STATIC_S_LEFT
		)
		return ;


	// TODO: Add your message handler code here
	CMenu menu;
	menu.LoadMenu(IDR_MENU_BAR);

	CMenu *pMenu = NULL;
	pMenu = menu.GetSubMenu(0);
	if (pMenu == NULL) return;


	// get the position of the view bar
	m_nDockSide = GetViewBarPos();
	switch(m_nDockSide) 
	{
	case ABE_TOP:
		pMenu->CheckMenuItem(ID_DOCK_TOP, MF_CHECKED);
		break;
	case ABE_BOTTOM:
		pMenu->CheckMenuItem(ID_DOCK_BOTTOM, MF_CHECKED);
		break;
	default:
		break;
	}

	// create skin menu
	CStringArray skins;
	CSkinManager::GetInstance()->GetSkinList(skins);

	CMenu SkinMenu;
	SkinMenu.CreatePopupMenu();

	for (int i=0; i<skins.GetCount(); ++i)
	{
		SkinMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
			MENU_SKIN_BASE + i, skins.GetAt(i));
		if (CSkinManager::GetInstance()->GetCurrentSkin().CompareNoCase(skins.GetAt(i)) == 0)
			SkinMenu.CheckMenuItem(MENU_SKIN_BASE + i, MF_CHECKED);
	}

	CString tmp;
	tmp.LoadString(IDS_MENU_SKIN);
	pMenu->InsertMenu(0, MF_BYPOSITION | MF_ENABLED | MF_POPUP,
		reinterpret_cast<UINT>(SkinMenu.GetSafeHmenu()), tmp);
	SkinMenu.Detach();


	// create lang menu
	CStringArray langs;
	CLangManager *pLang = CLangManager::GetInstance();
	pLang->GetLangList(langs);

	CMenu LangMenu;
	LangMenu.CreatePopupMenu();

	for (int i=0; i<langs.GetCount(); ++i)
	{
		LangMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
			MENU_LANG_BASE + i, langs.GetAt(i));
		if (pLang->GetCurrentLang().CompareNoCase(langs.GetAt(i)) == 0)
			LangMenu.CheckMenuItem(MENU_LANG_BASE + i, MF_CHECKED);
	}

	//CMenu *pLangSkin = pMenu->GetSubMenu(0);	

	tmp.LoadString(IDS_MENU_LANGUAGE);
	pMenu->InsertMenu(0, MF_BYPOSITION | MF_ENABLED | MF_POPUP,
		(UINT)(LangMenu.GetSafeHmenu()), tmp);
	LangMenu.Detach();


	// pop up menu
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);


	//
	menu.DestroyMenu();	

	// redraw button, this is a bug
	//CSkinManager::GetInstance()->RedrawButton();
}

// when user drag ViewBar to a new position(top or bottom)
// we should get the new position of it according to its screen
// coordinate
int CMyAppBar::GetViewBarPos()
{
	// 	CRect desktop;
	// 	CRect viewbar;
	// 
	// 	int y = GetSystemMetrics(SM_CYSCREEN);
	// 	GetClientRect(viewbar);
	// 	ClientToScreen(viewbar);
	// 
	// 	if (viewbar.top > y/2)
	// 		return ABE_BOTTOM;
	// 	else
	// 		return ABE_TOP;
	return CAppBarDlg::GetViewBarPos();
}

// dock to the edge of the desktop
void CMyAppBar::OnDock(UINT nID) 
{
	g_log.OutputDebug(_T("OnDock() "),nID);
	CRegistry reg(thePrefs.GetUsername());

	switch (nID)
	{
	case ID_DOCK_TOP:
		SetDockingSide(ABE_TOP);
		m_nDockSide = ABE_TOP;
		reg.SetDockPositionTop(true);
		break;
	case ID_DOCK_BOTTOM:
		SetDockingSide(ABE_BOTTOM);
		m_nDockSide = ABE_BOTTOM;
		reg.SetDockPositionTop(false);
		break;
	default:
		break;
	}
}


// post message to parent window
// notify parent to quit
/*
void CMyAppBar::OnBarQuit()
{
// TODO: Add your command handler code here
m_pParent->PostMessage(WM_QUIT);
OnBarHide();
}
*/

void CMyAppBar::OnBarHide()
{
	g_log.OutputDebug(_T("OnBarHide") );
	// Before hiding ViewBar, save Viewbar's postion
	thePrefs.SetDockTop( GetViewBarPos() == ABE_TOP ? true : false);

	g_MainDlg->DestroyTimer(FALSE);        //maybe there're something wrong while minimize
	g_MainDlg->m_pAppBar = NULL;
	thePrefs.ShowViewBar(false);
	g_MainDlg->UpdateTrayMenu();

	SendMessage(WM_DESTROY, 0, 0L);

	//::SendMessage(this->GetSafeHwnd(), WM_DESTROY, NULL, NULL);
	delete this;


	//CDlgLogin *pMain = static_cast<CDlgLogin *>(m_pParent);
	//	pMain->PostMessage(WM_USER_CLOSEBAR);
}


BOOL CMyAppBar::InitSearchBox()
{
	CSearchManager::GetInstance()->AttachSearchBox(&m_editSearch);
	
	//DWORD dwStyle = GetWindowLong(m_cbxSearch.GetSafeHwnd(), GWL_STYLE);
	//dwStyle |= ES_AUTOHSCROLL;
	//SetWindowLong(m_cbxSearch.GetSafeHwnd(), GWL_STYLE, dwStyle);

	//CEdit* edit = (CEdit*)m_cbxSearch.GetDlgItem(1001);
	//edit->ModifyStyle(0, ES_AUTOHSCROLL);

	//m_cbxSearch.LimitText(256);

	return TRUE;
}

BOOL CMyAppBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if(wParam >= MENU_SKIN_BASE && wParam < MENU_SKIN_BASE + 1000)	
	{
		OnSkinMenu((int)(wParam - MENU_SKIN_BASE));
		return TRUE;
	}

	if(wParam >= MENU_LANG_BASE && wParam < MENU_LANG_BASE + 1000)	
	{
		OnLangMenu((int)(wParam - MENU_LANG_BASE));
		return TRUE;
	}

	if(wParam >= MENU_TOOL_BASE && wParam < MENU_TOOL_BASE + 1000)	
	{
		OnToolsMenu((int)(wParam - MENU_TOOL_BASE));
		return TRUE;
	}

	if(wParam >= MENU_FAV_IE_BASE && wParam < MENU_FAV_IE_BASE + 1000)	
	{
		OnFavoriteMenuIE((int)(wParam - MENU_FAV_IE_BASE));
		return TRUE;
	}

	if(wParam >= MENU_FAV_FF_BASE && wParam < MENU_FAV_FF_BASE + 1000)	
	{
		OnFavoriteMenuFF((int)(wParam - MENU_FAV_FF_BASE));
		return TRUE;
	}


	if (wParam>= MENU_FAV_AGLOCO_BASE && wParam < MENU_FAV_AGLOCO_BASE + 1000 )
	{
		OnFavoriteMenuAgloco((int)(wParam - MENU_FAV_AGLOCO_BASE));
	}

	if (wParam>= MENU_SEARCH_ENGINE_BASE && wParam < MENU_SEARCH_ENGINE_BASE + 1000 )
	{
		OnSearchEngineChanging((int)(wParam - MENU_SEARCH_ENGINE_BASE));
	}
	return __super::OnCommand(wParam, lParam);
}


// skin menu event handler
BOOL CMyAppBar::OnSkinMenu(int nMenuId)
{

	CStringArray skins;
	CSkinManager::GetInstance()->GetSkinList(skins);

	CSkinManager::GetInstance()->LoadSkins(skins.GetAt(nMenuId));

	return TRUE;
}

// lang menu event handler
BOOL CMyAppBar::OnLangMenu(int nMenuId)
{

	CStringArray langs;
	CLangManager *pLang = CLangManager::GetInstance();
	pLang->GetLangList(langs);
	pLang->SelectLang(langs.GetAt(nMenuId));

	return TRUE;
}

// tools menu event handler
BOOL CMyAppBar::OnToolsMenu(int nMenuId)
{
	//  Create Menu From Dir for Tools
	// 	CStringArray tools;
	// 	CMyUtility::GetFileList(tools, _T("tools"));
	// 
	// 	CString dir = CMyUtility::GetCurDir();
	// 	dir = dir + _T("\\tools\\") + tools.GetAt(nMenuId);
	// 
	// 	ShellExecute(NULL, _T("open"), dir, NULL, NULL, SW_SHOW); 

	// Warning!!!!!!!!!!!!!!!
	// the following code for tools is temp
	// so 'nMenuId == 1' is hard-coded
	// in the future, it'll be replaced with macro

	if (nMenuId == 1)
	{
		ShellExecute(NULL, _T("open"), _T("calc.exe"), NULL, NULL, SW_SHOW);
	}
	else
		if (nMenuId == 2)
		{
			WinExec("rundll32.exe shell32.dll,Control_RunDLL timedate.cpl", SW_SHOWNORMAL);

			//ShellExecute(NULL, _T("open"), _T("timedate.cpl"), NULL, NULL, SW_SHOW);
		}

		return TRUE;
}

// favorite menu event handler
BOOL CMyAppBar::OnFavoriteMenuIE(int nMenuId)
{
	CString url = m_arrayFavUrlIE.GetAt(nMenuId);
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW); 

	return TRUE;
}

BOOL CMyAppBar::OnFavoriteMenuFF(int nMenuId)
{
	CString url = m_arrayFavUrlFF.GetAt(nMenuId);
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW); 

	return TRUE;
}

BOOL CMyAppBar::OnFavoriteMenuAgloco(int nMenuId)
{
	CAglocoFavorites fav;
	CString url = fav.GetFavUrl(nMenuId);
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW); 

	return TRUE;
}

BOOL CMyAppBar::OnSearchEngineChanging(int nMenuId)
{
	std::list<std::pair<int, std::tstring> >list;
	std::list<std::pair<int, std::tstring> >::iterator it;
	CSearchManager::GetInstance()->GetSearchEngineList(list);

	int iSearchEngine = 0;
	for (it = list.begin(); it!=list.end(); ++it)
	{
		std::pair<int, std::tstring> item = *it;
		if (nMenuId == iSearchEngine)
		{
			CSearchManager::GetInstance()->SetDefaultEngine(item.first);

			return TRUE;
		}

		iSearchEngine++;
	}

	return FALSE;
}

void CMyAppBar::OnBarTest()
{
	// TODO: Add your command handler code here
	/*
	HINSTANCE hDll = NULL;
	hDll = CMultiLanguage::LoadResourceDLL(_T("E:\\Project\\VSS\\Client\\LangZH\\Release\\LangZH.dll"));

	if (hDll)
	{
	AfxSetResourceHandle(hDll);
	CString xxx;
	xxx.LoadString(IDS_ABOUTBOX);
	MessageBox(xxx);
	}
	*/

}

void CMyAppBar::OnBarAglocowebsite()
{
	// TODO: Add your command handler code here
	ShellExecute(NULL, _T("open"), _T("http://www.agloco.com"), NULL, NULL, SW_SHOW); 
}


void CMyAppBar::TravelTree(CTreeCtrl *pTree, int iMenuBase, HTREEITEM parentItem, CMenu &menu)
{   
	CString strMenuName = pTree->GetItemText(parentItem);
	int iItemData = (int)pTree->GetItemData(parentItem);

	if( !pTree->ItemHasChildren(parentItem) )   
	{   
		menu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
			iMenuBase + iItemData, strMenuName);
		return;   
	}   

	CMenu subMenu;
	subMenu.CreatePopupMenu();

	//IXMLDOMNodePtr pChildNode;   
	HTREEITEM  hChildItem = pTree->GetChildItem(parentItem);
	while (hChildItem != NULL)   
	{   
		CString strMenuName = pTree->GetItemText(hChildItem);   
		if(!pTree->ItemHasChildren(hChildItem))
		{   
			// map
			int iItemData = (int)pTree->GetItemData(hChildItem);
			subMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
				iMenuBase + iItemData, strMenuName);
		}   
		else //
		{   
			TravelTree(pTree, iMenuBase, hChildItem, menu);
		}

		hChildItem = pTree->GetNextItem(hChildItem, TVGN_NEXT);
	}

	int iPos = menu.GetMenuItemCount();
	menu.InsertMenu(iPos, MF_BYPOSITION | MF_ENABLED | MF_POPUP,
		(UINT)(subMenu.GetSafeHmenu()), strMenuName);
}   

void CMyAppBar::OnMenuTools()
{
	CMenu menu;
	// menu.LoadMenu(IDR_MENU_MISC);
	menu.CreatePopupMenu();

	//CMenu *pMenu = NULL;
	//pMenu = menu.GetSubMenu(0);

	//  //////////////// Create Tools Menu From Dir /////////////////
	// 	CStringArray tools;
	// 	CMyUtility::GetFileList(tools, _T("tools"));
	// 
	// 	
	// 	CMenu ToolsMenu;
	// 	ToolsMenu.CreatePopupMenu();
	// 
	// 	for (int i=0; i<tools.GetCount(); ++i)
	// 	{
	// 		CString temp;
	// 		temp = tools.GetAt(i);
	// 		temp = temp.Left(temp.GetLength() - 4);
	// 		ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
	// 			MENU_TOOL_BASE + i, temp);
	// 	}
	// ///////////////////////////////////////////////////////////////

	// Warning!!!!!!!!!!!!!!!
	// the following code for tools is temp
	// so the 'MENU_TOOL_BASE + 1' is hard-coded
	// in the future, it'll be replaced with macro
	/////////  Tools Menu //////////
	CMenu ToolsMenu;
	ToolsMenu.CreatePopupMenu();
	CString strMenuName;
	strMenuName.LoadString(IDS_MENU_CALCULATOR);
	ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
		MENU_TOOL_BASE + 1, strMenuName);
	strMenuName.LoadString(IDS_MENU_CALENDAR);
	ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
		MENU_TOOL_BASE + 2, strMenuName);
	CString tmp;
	tmp.LoadString(IDS_MENU_TOOLS);
	//CMenu *pMenu = pMenu->GetSubMenu(0);	
	menu.InsertMenu(0, MF_BYPOSITION | MF_ENABLED | MF_POPUP,
		(UINT)(ToolsMenu.GetSafeHmenu()), tmp);
	ToolsMenu.Detach();


	/////// favorites menu /////
	AddFavoriteMenu(menu);


	// pop up menu
	CPoint point;
	//GetCursorPos(&point);
	CRect rtTools;
	m_btnTools.GetWindowRect(rtTools);

	if(GetViewBarPos() == ABE_BOTTOM)
	{
		point = rtTools.TopLeft();
		point.x += rtTools.Width();
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN, point.x, point.y, this);
	}
	else
	{
		point = rtTools.BottomRight();

		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN, point.x, point.y, this);
	}

	menu.DestroyMenu();	
}


BOOL CMyAppBar::InitPPg()
{
	return TRUE;
}

// docking ViewBar
BOOL CMyAppBar::InitDocking()
{
	if (thePrefs.IsDockTop())
	{
		SetDockingSide(ABE_TOP);
	}
	else
	{
		SetDockingSide(ABE_BOTTOM);
	}


	DWORD dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | WS_CLIPCHILDREN);

	return TRUE;
}



void CMyAppBar::OnBarOptions()
{
	// TODO: Add your command handler code here

	CDockFacade::GetInstance()->GetDockInfo();
	CPreferencesDlg dlg;

	if (CPreferencesDlg::bShow == FALSE)
	{
		CMyCounter counter;
		if (counter.IsEmpty())
		{
			dlg.DoModal();
		}
		CPreferencesDlg::bShow = FALSE;
	}

}

void CMyAppBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// user click left button
	m_bIsLButtonDown = true;

	// get mouse point
	m_ptMousePosOnDown = point;

	__super::OnLButtonDown(nFlags, point);
}

void CMyAppBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// user release the left button
	m_bIsLButtonDown = false;
	EraseOldRect();

	// restore the Rectangle of ViewBar
	m_rtViewBarRect = CRect(0,0,0,0);

	__super::OnLButtonUp(nFlags, point);
}

void CMyAppBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// if user click the left button, change the cursor
	if (m_bIsLButtonDown)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		DrawRectangle();
	}
	// reset cursor
	else
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}


	__super::OnMouseMove(nFlags, point);
}

void CMyAppBar::OnDestroy()
{
	__super::OnDestroy();
	// TODO: Add your message handler code here

	CDockFacade::GetInstance()->GetDockInfo();
	thePrefs.SaveConfigToRegistry();

	CUISubject::GetInstance()->Detatch(this);
}

// when VB is moving, erase the track of VB
void CMyAppBar::EraseOldRect()
{
	static int i;
	if (i != 0)
	{
		HDC hdc = ::GetDC(NULL);   
		CDC* pDC = CDC::FromHandle(hdc);   
		CRectTracker rectTracker;   
		m_rtViewBarRect.NormalizeRect();   
		rectTracker.DrawTrackerRect(&m_rtViewBarRect, AfxGetMainWnd(), pDC, AfxGetMainWnd());   
		::ReleaseDC(NULL, hdc);   
	}

	i++;
}

// draw a rectangle around ViewBar to indicate VB is moving
void CMyAppBar::DrawRectangle()
{
	EraseOldRect();

	HDC hdc = ::GetDC(NULL);   
	CDC* pDC = CDC::FromHandle(hdc);   

	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	rect.MoveToXY(ptMouse);
	rect.OffsetRect(-m_ptMousePosOnDown);

	m_rtViewBarRect = rect;

	CRectTracker t;
	t.DrawTrackerRect(&m_rtViewBarRect, AfxGetMainWnd(), pDC, AfxGetMainWnd()) ; 

	::ReleaseDC(NULL, hdc);   
}

void CMyAppBar::AddFavoriteMenu(CMenu &menu)
{
	CString strMenuName;
	CMenu menuFavorites;
	menuFavorites.CreatePopupMenu();
	strMenuName.LoadString(IDS_MENU_FAVORITES);

	// Insert Favorites Name
	menu.InsertMenu(0, MF_BYPOSITION | MF_ENABLED | MF_POPUP,
		(UINT)(menuFavorites.GetSafeHmenu()), strMenuName);


	// create a tree
	CTreeCtrl tree;
	tree.Create(WS_TABSTOP | WS_CHILD | WS_BORDER
		| TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES
		| TVS_DISABLEDRAGDROP,
		CRect(10, 10, 300, 100), this, 0x1005);

	CFavorite fav;
	BROWSERTYPE iBrowserType;
	// 	fav.getBrowserType(iBrowserType);
	// 	if (iBrowserType == IEXPLORE)
	// 	{
	// 		thePrefs.SetDefaultBrowserName(_T("IE"));
	// 	}
	// 	else
	// 	if (iBrowserType == FIREFOX)
	// 	{
	// 		thePrefs.SetDefaultBrowserName(_T("FireFox"));
	// 	}

	if (thePrefs.IsShowIEFavorites())
	{
		// Insert IE Menu Name
		CMenu menuIE;
		menuIE.CreatePopupMenu();
		strMenuName.LoadString(IDS_MENU_FAVORITES_IE);
		menuFavorites.InsertMenu(0,MF_BYPOSITION | MF_ENABLED | MF_POPUP,
			(UINT)(menuIE.GetSafeHmenu()),strMenuName);

		fav.getFavoriteFromIE(&tree);
		// fav.GetFavoriterFromIEOrder(&tree);
		m_arrayFavUrlIE.RemoveAll();
		fav.getFavUrl(m_arrayFavUrlIE);

		HTREEITEM rootItem = tree.GetRootItem();   
		while(rootItem)
		{
			TravelTree(&tree, MENU_FAV_IE_BASE, rootItem, menuIE);  
			rootItem = tree.GetNextItem(rootItem, TVGN_NEXT);
		}
	}

	if (thePrefs.IsShowFFFavorites())
	{
		// Insert FireFox Menu Name
		CMenu menuFireFox;
		menuFireFox.CreatePopupMenu();
		strMenuName.LoadString(IDS_MENU_FAVORITES_FIREFOX);
		menuFavorites.InsertMenu(1,MF_BYPOSITION | MF_ENABLED | MF_POPUP,
			(UINT)(menuFireFox.GetSafeHmenu()),strMenuName);

		// new
		m_arrayFavUrlFF.RemoveAll();
		CNetscapteBookmark bookmark(&tree);
		bookmark.Parse();
		bookmark.GetFavUrl(m_arrayFavUrlFF);

		HTREEITEM rootItem = tree.GetRootItem();
		while(rootItem)
		{
			TravelTree(&tree, MENU_FAV_FF_BASE, rootItem, menuFireFox);
			rootItem = tree.GetNextItem(rootItem, TVGN_NEXT);
		}

		// Load FireFox Favorites
		/*
		if( fav.getFavoriteFromFireFox(&tree) )
		{
			fav.getFavUrl(m_arrayFavUrl);

			HTREEITEM rootItem = tree.GetRootItem();
			while(rootItem)
			{
				TravelTree(&tree, rootItem, menuFireFox);
				rootItem = tree.GetNextItem(rootItem, TVGN_NEXT);
			}
		}
		*/
	}


	/////// Agloco Favorites Menu /////
	CAglocoFavorites favAgloco;
	std::vector<CAglocoFavoritesItem> vecFav;
	favAgloco.GetFavorites(vecFav);
	CMenu menuAgloco;
	menuAgloco.CreatePopupMenu();
	for (int i=0; i<vecFav.size(); ++i)
	{
		CAglocoFavoritesItem item = vecFav[i];
		int iOffset = _ttoi(item.m_strId);
		menuAgloco.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
			MENU_FAV_AGLOCO_BASE + iOffset, item.m_strName);
	}
	menuFavorites.InsertMenu(0, MF_BYPOSITION | MF_ENABLED | MF_POPUP,
		(UINT)(menuAgloco.GetSafeHmenu()), _T("AGLOCO"));
	menuAgloco.Detach();

}

LRESULT CMyAppBar::OnNoticeAffiliate(WPARAM wParam, LPARAM lParam)
{
	lParam;
	
	EnterConnectCS();
	POSITION pos = (POSITION)wParam;	
	
	PConnect_HTTP pconHttp = GetNext(pos);
	g_log.OutputDebug(_T("OnNoticeAffiliate POS"),(int)pos);
	if( pconHttp == NULL )
	{
		LeaveConnectCS();
		return 0;
	}
	
	try
	{
	static TCHAR szHost[MAX_PATH];
	szHost[0] = _T('\0');
	int nHost = (int)strlen(pconHttp->host);
	int k = MultiByteToWideChar(CP_ACP,0,pconHttp->host, nHost, szHost,MAX_PATH);
	szHost[k] = _T('\0');
	//g_log.OutputDebug(szHost);
	

	AFFILIATELIST *affiliateList = CDataStore::GetInstance()->GetAffiliateList();
	pos = affiliateList->GetHeadPosition();
	while( pos )
	{
		PAFFILIATE pAff = affiliateList->GetNext(pos);
		if( pAff )
		{
			CAtlRegExp<> reUrl;
			if( REPARSE_ERROR_OK != reUrl.Parse(pAff->affiliate) )
				continue;
			CAtlREMatchContext<> mcUrl;
			if(reUrl.Match(szHost,&mcUrl))
			{
				MathAffiliate(pAff->id,pconHttp->CompUrl);
			}
		}
	}
	}
	catch(...)
	{
		g_log.OutputDebug(_T("OnNoticeAffiliate catch error"));
	}
	
	LeaveConnectCS();
	return 0;
}

void CMyAppBar::LoadAffiliateList()
{
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	SQLite->AffilatesExecTableQuery(TABLE_AFFILIATES,"select * "," ");

	AFFILIATELIST *affiliateList = CDataStore::GetInstance()->GetAffiliateList();
	CDataStore::GetInstance()->RemoveAllAffiliates();
	int nRows = SQLite->AffLiatesGetRowsNumber();
	for( int i = 0; i < nRows; i++ )
	{
		PAFFILIATE pAffiliate = new AFFILIATE;
		pAffiliate->id = SQLite->AffilatesGetIntValue(i,POS_AFFID_AFFILIATES);
		pAffiliate->affiliate = SQLite->AffilatesGetStringValue(i,POS_AFFNAME_AFFILIATES);
		affiliateList->AddTail(pAffiliate);
	}
	AddAffiliateList(this->GetSafeHwnd());
}

void CMyAppBar::MathAffiliate(int id,const char* url)
{
	CCppSQLiteManager* SQLite = CCppSQLiteManager::GetInstance();
	char condition[64];
	sprintf(condition,"where affiliateId=%d", id);
	//sprintf(condition,"where affiliateId=%d and status=1", id);
	SQLite->AffDetailsExecTableQuery(TABLE_AFFDETIALS,"select * ",condition);
	int nRows = SQLite->AffDetailsGetRowsNumber();
	for( int i = 0; i < nRows; i++ )
	{
		CString strPattern, strAgPattern;
		strPattern = SQLite->AffDetailsGetStringValue(i,POS_DETPATTERN_AFFILIATES);
		//strPattern.Format(_T("%s"), SQLite->AffDetailsGetStringValue(i,POS_DETPATTERN_AFFILIATES)); // pattern
		strAgPattern = SQLite->AffDetailsGetStringValue(i,POS_DETAGPATTERN_AFFILIATES);
		//strAgPattern.Format(_T("%s"), SQLite->AffDetailsGetStringValue(i,POS_DETAGPATTERN_AFFILIATES)); // agPattern
		CAtlRegExp<> reUrl;
		if( REPARSE_ERROR_OK != reUrl.Parse(strPattern) )
			continue;
		CAtlREMatchContext<> mcUrl;
		CString strUrl;
		strUrl = url;
		//strUrl.Format(_T("%s"),url);
		if(reUrl.Match(strUrl.GetBuffer(),&mcUrl))
		{
			CAtlRegExp<> reAgPattern;
			if( REPARSE_ERROR_OK != reAgPattern.Parse(strAgPattern) )
				continue;
			CAtlREMatchContext<> mcAg;
			if(reAgPattern.Match(strUrl.GetBuffer(),&mcAg))
			{
				NoticeUserAffiliate(strUrl,TRUE);
			}
			else
			{
				NoticeUserAffiliate(strUrl,FALSE);
			}
		}
	}
}

// show notice in appbar
void CMyAppBar::NoticeUserAffiliate(CString message, BOOL bOK/* = FALSE*/)
{
}

// handle WM_NOTICE_SURF message
// wParam boolean flag; TRUE: noinput -> action
//						FALSE: action -> noinput
LRESULT CMyAppBar::OnNoticeSurf(WPARAM wParam, LPARAM lParam)
{
	BOOL bAction = (BOOL)wParam;

	if (bAction)
	{
		CSkinManager::GetInstance()->SetIdle(FALSE);
	}
	else
	{
		CSkinManager::GetInstance()->SetIdle();
	}

	return 0;
}


void CMyAppBar::OnBnClickedBtnMinmize()
{
	OnBarHide();
}

void CMyAppBar::OnBnClickedBtnTools()
{
	OnMenuTools();
	//CSkinManager::GetInstance()->RedrawButton();
}


LRESULT CMyAppBar::OnMonitorKeyboardAndMOuse(WPARAM wParam, LPARAM lParam)
{
	((CDlgLogin*)m_pParent)->SetMonitorParam();
	return 0;
}

void CMyAppBar::OnStnClickedStaticEngineLogo()
{
	PopSearchEngineMenu();
}

void CMyAppBar::OnBnClickedBtnSearch()
{
	//m_cbxSearch.Search();
	m_editSearch.Search();
}

void CMyAppBar::PopSearchEngineMenu()
{
	CMenu menu;
	menu.CreatePopupMenu();

	std::list<std::pair<int, std::tstring> >list;
	std::list<std::pair<int, std::tstring> >::iterator it;
	CSearchManager::GetInstance()->GetSearchEngineList(list);

	int iSearchEngine = 0;
	for (it = list.begin(); it!=list.end(); ++it)
	{
		std::pair<int, std::tstring> item = *it;
		CString strItem;
		strItem.Format(_T("%s"), item.second.c_str());

		menu.AppendMenu(MF_STRING | MF_ENABLED | MF_POPUP,
			MENU_SEARCH_ENGINE_BASE + iSearchEngine, strItem);

		// check
		if (item.first == thePrefs.GetDefaultSearchEngine())
			menu.CheckMenuItem(MENU_SEARCH_ENGINE_BASE + iSearchEngine, MF_CHECKED);

		iSearchEngine++;
	}

	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_staticSearchLogo.GetClientRect(rect);
	m_staticSearchLogo.ClientToScreen(rect);

	if (GetViewBarPos() == ABE_BOTTOM)
	{
		point = rect.TopLeft();
	}
	else
		point = rect.BottomRight();

	menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

/*
void CMyAppBar::SetSearchEnglineLogo()
{
	int iId = thePrefs.GetDefaultSearchEngine();

	int iIcon = 0;
	switch(iId)
	{
	case 1:
		iIcon = IDI_ICON_GOOGLE;	break;
	case 2:
		iIcon = IDI_ICON_BAIDU;		break;
	case 3:
		iIcon = IDI_ICON_LIVE;		break;
	case 4:
		iIcon = IDI_ICON_YAHOO;		break;
	case 5:
		iIcon = IDI_ICON_EBAY;		break;
	case 6:
		iIcon = IDI_ICON_DICT;		break;
	case 7:
		iIcon = IDI_ICON_WIKI;		break;
	default:
		iIcon = IDI_ICON_DEFAULT;
		break;
	}
	m_staticSearchLogo.SetIcon(   
		(HICON)::LoadImage(AfxGetInstanceHandle(),   
		MAKEINTRESOURCE(iIcon), IMAGE_ICON, 16, 16, LR_SHARED));

	// arrow
	m_staticSearchArrow.SetIcon(   
		(HICON)::LoadImage(AfxGetInstanceHandle(),   
		MAKEINTRESOURCE(IDI_ICON_ARROW), IMAGE_ICON, 16, 16, LR_SHARED));
}
*/

void CMyAppBar::OnStnClickedStaticArrow()
{
	PopSearchEngineMenu();
}

// 
HBRUSH CMyAppBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_ENGINE_LOGO
		|| pWnd->GetDlgCtrlID() == IDC_STATIC_ARROW)
	{
		pDC->SetBkMode(TRANSPARENT);

		return CreateSolidBrush(RGB(255, 255, 255));
	}

	return hbr;
}

// open web page
void CMyAppBar::OnBarMyAccount()
{
	ShellExecute(NULL, _T("open"), _T("http://www.agloco.com/web/guest/myaccount"), NULL, NULL, SW_SHOWNORMAL);
}


BOOL CMyAppBar::OnEraseBkgnd(CDC* pDC)
{
	CBrush   brush(RGB(255, 255, 255));

	RECT   rect;   
	GetWindowRect(&rect);   
	ScreenToClient(&rect);   

	pDC->FillRect(&rect, &brush);   

	return FALSE;

	// return __super::OnEraseBkgnd(pDC);
}
BEGIN_EVENTSINK_MAP(CMyAppBar, CAppBarDlg)
	ON_EVENT(CMyAppBar, IDC_EXPLORER1, 250, CMyAppBar::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CMyAppBar, IDC_EXPLORER1, 259, CMyAppBar::DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

void CMyAppBar::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString strUrl = CString(URL->bstrVal);
	CString strTargetFrame = CString(TargetFrameName->bstrVal);

	// Default filter
	CStringArray arrayDefaultFilter;
	arrayDefaultFilter.Add(_T(".*agloco.com.*"));
	arrayDefaultFilter.Add(_T(".*.wikia.com.*"));

	CStringArray *pFilter = NULL;
	if (thePrefs.GetUrlFilter().GetCount() == 0)
	{
		pFilter = &arrayDefaultFilter;
	}
	else
	{
		pFilter = &thePrefs.GetUrlFilter();
	}
	
	if (strUrl.Left(4).CompareNoCase(_T("http")) == 0)
	{	
		BOOL bMatch = FALSE;
		for (int i=0; i<pFilter->GetSize(); ++i)
		{
			CString strRegExp = pFilter->GetAt(i);
			CAtlRegExp<> reg;
			CAtlREMatchContext<> mcCon;

			reg.Parse(strRegExp);

			if (reg.Match(strUrl, &mcCon))
			{
				bMatch = TRUE;
				break;
			}
		}

		// Open a new IE windows, when the ad is not from sponsor
		if (!bMatch)
		{
			ShellExecute(NULL, _T("open"), strUrl, NULL, NULL, SW_SHOW); //DOT
			*Cancel = TRUE;
		}

	}

	/*
	if (strUrl.Left(4).CompareNoCase(_T("http")) == 0
		&& strUrl.Left(25) != _T("http://viewbar.agloco.com")
		&& strUrl.Left(20) != _T("http://www.wikia.com")
		&& strTargetFrame != _T("google_ads_frame"))
	{
		ShellExecute(NULL, _T("open"), strUrl, NULL, NULL, SW_SHOW); 
		*Cancel = TRUE;
	}
	*/
}

void CMyAppBar::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	if (pDisp == NULL) return;
	if (URL->bstrVal == NULL) return;

	CString strUrl = (CString)(*URL);
	if (strUrl.Find(_T("http:")) < 0) return;

	// if the template don't need JavaScript, then just return
	if (CAdsManager::GetInstance()->GetJavaScriptName() == _T(""))
		return;

	/*
	BOOL bIsGoogleIFrame = FALSE;
	BOOL bIsWikiaIFrame = FALSE;

	int iPos = strUrl.Find(_T("pagead2.googlesyndication.com/"));
	if (iPos > 0)
	{
		bIsGoogleIFrame = TRUE;
	}

	iPos = strUrl.Find(_T("wikia.jsp"));
	if (iPos > 0)
	{
		bIsWikiaIFrame = TRUE;
	}

	*/

	try
	{
		HRESULT hr;

		// get IWebBrowser2
		IWebBrowser2 * pWebBrowser = NULL;
		hr = pDisp->QueryInterface(IID_IWebBrowser2, 
			(void **)&pWebBrowser);
		if (FAILED(hr))	return;
		if (pWebBrowser == NULL) return;
		pWebBrowser->put_Silent(TRUE);

		// get document
		CComPtr<IDispatch> spDispDoc;
		hr = pWebBrowser->get_Document(&spDispDoc);
		if (FAILED(hr))	return;
		if (spDispDoc == NULL) return;

		CComPtr<IHTMLDocument2> spHTMLDoc;
		hr = spDispDoc->QueryInterface(IID_IHTMLDocument2,(void**)&spHTMLDoc);
		if (FAILED(hr))	return;
		if (spHTMLDoc == NULL) return;

		IHTMLDocument2* document = NULL;
		document = spHTMLDoc;

		CComPtr<IHTMLElement> body;
		IHTMLElement *parent;

		// get the body object
		hr = document->get_body(&body); 
		if (FAILED(hr))	return;

		hr = body->get_parentElement(&parent);
		if (FAILED(hr))	return;
		if (parent == NULL) return;

		// get html
		BSTR bstr;
		CString str;
		hr = parent->get_outerHTML(&bstr);
		if (FAILED(hr)) return;
		// body->get_outerHTML(&bstr);
		str = bstr;

		SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		if (safe_array == NULL) return;

		VARIANT *variant = NULL;
		hr = SafeArrayAccessData(safe_array,(LPVOID *)&variant);
		if (FAILED(hr))
			return;
		variant->vt = VT_BSTR;


		/*
		if (bIsGoogleIFrame)
		{
			str += CAdsManager::GetInstance()->GetJavaScript();
			variant->bstrVal = CString(str).AllocSysString();
			SafeArrayUnaccessData(safe_array);

			// write SAFEARRAY to browser document
			document->write(safe_array);
			document->close();


			//BSTR pbBody = str.AllocSysString();
			//parent->put_innerHTML(pbBody);
		}
		
		if (bIsWikiaIFrame)
		{
			// write
			CString strWikiaFrame = 
			_T("<script language=\"javascript\">\r\n\
			<!--\r\n\
			if(window.onload) window.onload += _addon();\r\n\
			else window.onload = _addon();\r\n\
			\r\n\
			function _addon() {\r\n\
			var css = \"margin: 0px 0px 0px 0px;width: 900px; height: 25px\";\r\n\
			var f = document.getElementById(\"google_ads_frame\");\r\n\
			f.style.cssText = css;\r\n\
			document.body.style.cssText = css;\r\n\
			}\r\n\
			//-->\r\n\
			</script>\r\n\
			");

			//str += strWikiaFrame;
			//variant->bstrVal = CString(str).AllocSysString();
			//SafeArrayUnaccessData(safe_array);
			

			// write SAFEARRAY to browser document
			//document->write(safe_array);
			//document->close();

			//BSTR pbBody = str.AllocSysString();
			//parent->put_innerHTML(pbBody);

		}
		*/
		
		str += CAdsManager::GetInstance()->GetJavaScript();
		variant->bstrVal = CString(str).AllocSysString();

		// write SAFEARRAY to browser document
		document->write(safe_array);
		document->close();

		SafeArrayUnaccessData(safe_array);

		// release
		parent->Release();
		pWebBrowser->Release();
		document->Release();

	}catch(...)
	{
		// NOOP
	}

}

HWND CMyAppBar::GetParentWebBrowserWnd(HWND hMyWnd)
{
	HWND   hRetWBWnd = NULL;

	static TCHAR tszIEClass[] = _T("Internet Explorer_Server");
	TCHAR tszClass[sizeof(tszIEClass)+1];

	// Compare if our window is of type IE class
	::GetClassName( hMyWnd,tszClass,sizeof(tszClass));
	if(0 == _tcscmp(tszClass, tszIEClass))
	{
		// Then get the Shell which hosts the IE doc view window
		hRetWBWnd = ::GetParent(::GetParent(hMyWnd));
	}

	return hRetWBWnd;
}

BOOL CMyAppBar::IsThisOurBrowser(HWND hMyWnd)
{
	if (hMyWnd == NULL) return FALSE;

	HWND hWndOurWebBrowser = (HWND)m_ie.GetSafeHwnd();

	// Get the parent web browser window if any
	HWND hParentWBWnd = GetParentWebBrowserWnd(hMyWnd);

	while( (NULL != hParentWBWnd) &&
		(hParentWBWnd != hWndOurWebBrowser))
	{
		// Get the parent of the web browser window I have, recursively
		hParentWBWnd = GetParentWebBrowserWnd(::GetParent(hParentWBWnd));
	}

	return (NULL != hParentWBWnd);
}

// disable popup menu when we right click web browser
BOOL CMyAppBar::PreTranslateMessage(MSG* pMsg)
{
	// We have a right click    
	if ((WM_RBUTTONDOWN == pMsg->message) ||
		(WM_RBUTTONDBLCLK == pMsg->message))
	{
		// Now verify it is from our web browser
		if(IsThisOurBrowser(pMsg->hwnd))
		{
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}


void CMyAppBar::OnTimer(UINT_PTR nIDEvent)
{
	CAdsManager::GetInstance()->ShowAds();

	__super::OnTimer(nIDEvent);
} 
