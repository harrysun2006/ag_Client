#include "StdAfx.h"
#include "SkinManager.h"
#include "AdsManager.h"
#include "MyAppBar.h"


CSkinManager* CSkinManager::m_pInstance = NULL;

// inner class
CSkinManager::CSkinManager()
{
	m_pParent = NULL;
}

// when user is idle, change the logo
void CSkinManager::SetIdle(BOOL bIsIdle /* = TRUE */)
{
	CString dir = CMyUtility::GetCurDir();

	CString skinDir;
	skinDir.Format(_T("%s\\skins\\%s"), dir, GetCurrentSkin());
	CString bmp;
	if (bIsIdle)
		bmp.Format(_T("%s\\logo-idle.bmp"), skinDir);
	else
		bmp.Format(_T("%s\\logo.bmp"), skinDir);

	SetBackgroundImage(&(m_pParent->m_staticLogo), bmp);
}

BOOL CSkinManager::LoadSkins(const CString &strSkinName)
{
	if (thePrefs.IsViewBarVisible() == false)
		return FALSE;

	ASSERT(m_pParent);

	CString dir = CMyUtility::GetCurDir();

	CString skinDir;
	skinDir.Format(_T("%s\\skins\\%s"), dir, strSkinName);

	CString bmp;
	bmp.Format(_T("%s\\logo.bmp"), skinDir);
	SetBackgroundImage(&(m_pParent->m_staticLogo), bmp);

	//bmp.Format(_T("%s\\ViewBarSkinTool.bmp"), skinDir);
	//SetBackgroundImage(&(m_pParent->m_staticMisc), bmp);

	// search box static
	bmp.Format(_T("%s\\search-right.bmp"), skinDir);
	SetBackgroundImage(&(m_pParent->m_staticSearchBoxRight), bmp);

	bmp.Format(_T("%s\\search-left.bmp"), skinDir);
	SetBackgroundImage(&(m_pParent->m_staticSearchLeft), bmp);

	//bmp.Format(_T("%s\\arrow.bmp"));
	//HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
	//	bmp, IMAGE_BITMAP, 0, 0, 
	//	LR_LOADFROMFILE);
	//m_pParent->m_staticEngineLogo.SetBitmap(hbmp);

//	{	
// 	CString bmp;
// 	CString path = CMyUtility::GetCurDir();
// 	int iId = thePrefs.GetDefaultSearchEngine();
// 	bmp.Format(_T("%s\\engine\\%d.bmp"), path, iId);
// 	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
// 		bmp, IMAGE_BITMAP, 0, 0, 
// 		LR_LOADFROMFILE);

	//m_pParent->m_staticEngineLogo.SetBitmap(hbmp);
	//m_pParent->m_staticEngineLogo.SetIcon(::LoadIcon(AfxGetResourceHandle(),
	//	MAKEINTRESOURCE(IDI_ICON_GOOGLE),IMAGE_ICON,16,16,LR_SHARED));

// 	m_pParent->m_staticSearchLogo.SetIcon(   
// 		(HICON)::LoadImage(AfxGetInstanceHandle(),   
// 		MAKEINTRESOURCE(IDI_ICON_GOOGLE),IMAGE_ICON,16,16,LR_SHARED));
// 
// 
// 	// arrow
// 	m_pParent->m_staticSearchArrow.SetIcon(   
// 		(HICON)::LoadImage(AfxGetInstanceHandle(),   
// 		MAKEINTRESOURCE(IDI_ICON_ARROW),IMAGE_ICON,16,16,LR_SHARED));

	//}

	bmp.Format(_T("%s\\search-top.bmp"), skinDir);
	SetBackgroundImage(&(m_pParent->m_staticSearchTop), bmp);

	bmp.Format(_T("%s\\search-bottom.bmp"), skinDir);
	SetBackgroundImage(&(m_pParent->m_staticSearchBottom), bmp);



	/// buttons
	m_pParent->m_btnMinimize.BringWindowToTop();
	bmp.Format(_T("%s\\arrow.bmp"), skinDir);
	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		bmp, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE);
	bmp.Format(_T("%s\\arrow-pressed.bmp"), skinDir);
	HBITMAP hbmp2 = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		bmp, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE);
	m_pParent->m_btnMinimize.SetBitmaps(hbmp, RGB(255, 0, 255), hbmp2, RGB(255, 0, 255));
	//m_pParent->m_btnMinimize.SetBitmaps(hbmp, RGB(255, 0, 255));

	m_pParent->m_btnTools.BringWindowToTop();
	bmp.Format(_T("%s\\tool.bmp"), skinDir);
	hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		bmp, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE);
	bmp.Format(_T("%s\\tool-pressed.bmp"), skinDir);
	hbmp2 = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		bmp, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE);
	m_pParent->m_btnTools.SetBitmaps(hbmp, RGB(255, 0, 255), hbmp2, RGB(255, 0, 255));
	//m_pParent->m_btnTools.SetBitmaps(hbmp, RGB(255, 0, 255));

	m_pParent->m_btnSearch.BringWindowToTop();
	bmp.Format(_T("%s\\search-button.bmp"), skinDir);
	hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		bmp, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE); 
	bmp.Format(_T("%s\\search-button-pressed.bmp"), skinDir);
	hbmp2 = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		bmp, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE);
	m_pParent->m_btnSearch.SetBitmaps(hbmp, RGB(255, 0, 255), hbmp2, RGB(255, 0, 255));
	//m_pParent->m_btnSearch.SetBitmaps(hbmp, RGB(255, 0, 255));

	// search engine menu button
	//m_pParent->m_staticEngineLogo.BringWindowToTop();

	//CString html;
	//html.Format(_T("%s\\ads\\ads.html"), dir);
	//GenerateAdFile(strSkinName);
	//m_pParent->m_ie.Navigate(html, NULL, NULL, NULL, NULL);

	m_strCurrentSkin = strSkinName;

	CAdsManager::GetInstance()->ShowAds();

	// save config
	thePrefs.SetDefaultSkin(strSkinName);
	CRegistry reg(thePrefs.GetUsername());
	reg.SetDefaultSkin(strSkinName);

	//RedrawButton();
	
	return TRUE;
}

void::CSkinManager::RedrawButton()
{
	//::PostMessage(m_pParent->m_btnTools.GetSafeHwnd(), WM_MOUSEMOVE, 0, 0);
	//::PostMessage(m_pParent->m_btnMinimize.GetSafeHwnd(), WM_MOUSEMOVE, 0, 0);

	//::PostMessage(m_pParent->m_staticLogo.GetSafeHwnd(), WM_LBUTTONDOWN, 1, 1);
	//::PostMessage(m_pParent->m_staticLogo.GetSafeHwnd(), WM_LBUTTONUP, 1, 1);

	//m_pParent->m_btnTools.Invalidate();
	//m_pParent->m_btnMinimize.Invalidate();
	//m_pParent->Invalidate();
}

CSkinManager * CSkinManager::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CSkinManager();

	return m_pInstance;
}

BOOL CSkinManager::FreeInstance()
{
	delete m_pInstance;
	m_pInstance = 0;

	return TRUE;
}

void CSkinManager::SetTarget(CMyAppBar *pAppBar)
{
	m_pParent = pAppBar;
}

BOOL CSkinManager::SetBackgroundImage(CStatic *pStatic, const CString &image)
{
	if (thePrefs.IsViewBarVisible() == false)
		return FALSE;

	HBITMAP hbmp= (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		image, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE); 

	pStatic->SetBitmap(hbmp);
	return TRUE;
}


void CSkinManager::GetSkinList(CStringArray &skins)
{
	CFileFind finder;
	CString dir = CMyUtility::GetCurDir();
	CString strSkins;
	strSkins.Format(_T("%s\\skins\\*.*"), dir);

	BOOL bFind = finder.FindFile(strSkins);
	if(!bFind)
		return;

	while(bFind)
	{
		bFind = finder.FindNextFile();
		if(finder.IsDots())
			continue;

		if(finder.IsDirectory())
		{
			skins.Add(finder.GetFileName());
			continue;
		}
	}

	finder.Close();

}