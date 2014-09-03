// PPgDisplay.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "PPgDisplay.h"


// CPPgDisplay dialog

IMPLEMENT_DYNAMIC(CPPgDisplay, CPropertyPage)

CPPgDisplay::CPPgDisplay()
	: CPropertyPage(CPPgDisplay::IDD)
{

}

CPPgDisplay::~CPPgDisplay()
{
}

void CPPgDisplay::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_LANG, m_cmbLanguage);
	DDX_Control(pDX, IDC_CMB_SKIN, m_cmbSkin);
	DDX_Control(pDX, IDC_CMB_DOCK, m_cmbDock);
	DDX_Control(pDX, IDC_CHECK_IE, m_checkFavIE);
	DDX_Control(pDX, IDC_CHECK_FIREFOX, m_checkFavFF);
	DDX_Control(pDX, IDC_CHK_WARNING, m_btnWarning);
}


BEGIN_MESSAGE_MAP(CPPgDisplay, CPropertyPage)
	ON_BN_CLICKED(IDCANCEL, &CPPgDisplay::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CPPgDisplay::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CMB_LANG, &CPPgDisplay::OnCbnSelchangeCmbLang)
	ON_CBN_SELCHANGE(IDC_CMB_SKIN, &CPPgDisplay::OnCbnSelchangeCmbSkin)
	ON_CBN_SELCHANGE(IDC_CMB_DOCK, &CPPgDisplay::OnCbnSelchangeCmbDock)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_FIREFOX, &CPPgDisplay::OnBnClickedCheckFirefox)
	ON_BN_CLICKED(IDC_CHK_WARNING, &CPPgDisplay::OnBnClickedChkWarning)
END_MESSAGE_MAP()


// CPPgDisplay message handlers

void CPPgDisplay::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
}

void CPPgDisplay::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}

BOOL CPPgDisplay::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	CUISubject::GetInstance()->Attatch(this);

	InitLangList();
	InitDockList();
	InitSkinList();
	InitFavorites();
	InitWarningFlag();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPPgDisplay::InitWarningFlag()
{
	m_btnWarning.SetCheck(thePrefs.IsWarningBeforeQuitAllowed());

	return TRUE;
}

BOOL CPPgDisplay::InitFavorites()
{
	if (thePrefs.IsShowIEFavorites())
	{
		m_checkFavIE.SetCheck(TRUE);
	}

	if (thePrefs.IsShowFFFavorites())
	{
		m_checkFavFF.SetCheck(TRUE);
	}

	return TRUE;
}

BOOL CPPgDisplay::InitLangList()
{
	CLangManager *pLangManager = CLangManager::GetInstance();
	pLangManager->GetLangList(m_listLang);

	int nSelectedLang = 0;
	for (int i=0; i<m_listLang.GetCount(); ++i)
	{
		m_cmbLanguage.InsertString(i, m_listLang.GetAt(i));
		if (m_listLang.GetAt(i) == pLangManager->GetCurrentLang())
			nSelectedLang = i;
	}

	m_cmbLanguage.SetCurSel(nSelectedLang);
	
	return TRUE;
}


BOOL CPPgDisplay::InitSkinList()
{
	CSkinManager *pSkinManager = CSkinManager::GetInstance();
	pSkinManager->GetSkinList(m_listSkin);

	int nSelectedSkin = 0;
	for (int i=0; i<m_listSkin.GetCount(); ++i)
	{
		m_cmbSkin.InsertString(i, m_listSkin.GetAt(i));
		if (m_listSkin.GetAt(i) == pSkinManager->GetCurrentSkin())
			nSelectedSkin = i;
	}

	m_cmbSkin.SetCurSel(nSelectedSkin);

	return TRUE;
}

BOOL CPPgDisplay::InitDockList()
{
	m_cmbDock.ResetContent();

	CString tmp;
	tmp.LoadString(IDS_PPG_DISPLAY_TOP);
	m_cmbDock.InsertString(0, tmp);
	tmp.LoadString(IDS_PPG_DISPLAY_BOTTOM);
	m_cmbDock.InsertString(1, tmp);

	if (thePrefs.IsDockTop())
		m_cmbDock.SetCurSel(0);
	else
		m_cmbDock.SetCurSel(1);

	return TRUE;
}

void CPPgDisplay::OnCbnSelchangeCmbLang()
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

void CPPgDisplay::UpdateUI()
{
	if (m_hWnd)
	{

		SetWindowText(GetResString(IDS_PPG_DISPLAY));

		GetDlgItem(IDC_STATIC_LANG)->SetWindowText(GetResString(IDS_PPG_DISPLAY_LANG));
		GetDlgItem(IDC_STATIC_SKIN)->SetWindowText(GetResString(IDS_PPG_DISPLAY_SKIN));
		GetDlgItem(IDC_STATIC_DOCK)->SetWindowText(GetResString(IDS_PPG_DISPLAY_DOCK));

		GetDlgItem(IDC_STATIC_FAV)->SetWindowText(GetResString(IDS_PPG_DISPLAY_FAV_SETTING));
		GetDlgItem(IDC_CHK_WARNING)->SetWindowText(GetResString(IDS_PPG_DISPLAY_PROMPT));
		//GetDlgItem(IDC_PPG_GENERAL_SEARCH_ENGINE_SETTING)->SetWindowText(GetResString(IDS_PPG_GENERAL_DEFAULT_SEARCH_ENGINE));
		//GetDlgItem(IDC_BTN_CHECKUPDATES)->SetWindowText(GetResString(IDS_PPG_GENERAL_CHK_BUTTON));

		InitDockList();
	}
}



BOOL CPPgDisplay::OnApply()
{
	SetModified(FALSE);

	// save dock config
	bool bOnTop = false;
	if (m_cmbDock.GetCurSel() == 0)
		bOnTop = true;

	if (bOnTop != thePrefs.IsDockTop())
	{
		if (bOnTop)
			CDockFacade::GetInstance()->Dock(ID_DOCK_TOP);
		else
			CDockFacade::GetInstance()->Dock(ID_DOCK_BOTTOM);

		CDockFacade::GetInstance()->GetDockInfo();
	}

	// save lang config
	CString strLang;
	int iCurSel = m_cmbLanguage.GetCurSel();
	if (iCurSel >=0)
	{
		m_cmbLanguage.GetLBText(iCurSel, strLang);
		
		if (thePrefs.GetDefaultLanguage() != strLang)
		{
			thePrefs.SetDefaultLanguage(strLang);
			CLangManager::GetInstance()->SelectLang(strLang);
		}
	}	
	
	// save skin config
	CString strSkin;
	m_cmbSkin.GetLBText(m_cmbSkin.GetCurSel(), strSkin);

	if (thePrefs.GetDefaultSkin() != strSkin)
	{
		thePrefs.SetDefaultSkin(strSkin);
		CSkinManager::GetInstance()->LoadSkins(strSkin);
	}
	



	// favorites config
	thePrefs.SetFavoritesIE(m_checkFavIE.GetCheck());
	thePrefs.SetFavoritesFF(m_checkFavFF.GetCheck());

	// warning
	thePrefs.SetWarningBeforeQuitFlag(m_btnWarning.GetCheck());

	// save to registry
	thePrefs.SaveConfigToRegistry();

	return TRUE;
}
void CPPgDisplay::OnCbnSelchangeCmbSkin()
{
	// TODO: Add your control notification handler code here
	SetModified();
}

void CPPgDisplay::OnCbnSelchangeCmbDock()
{
	SetModified();
}

void CPPgDisplay::OnDestroy()
{
	__super::OnDestroy();

	CUISubject::GetInstance()->Detatch(this);
}

void CPPgDisplay::OnBnClickedCheckFirefox()
{
	SetModified();
}

void CPPgDisplay::OnBnClickedChkWarning()
{
	SetModified();
}
