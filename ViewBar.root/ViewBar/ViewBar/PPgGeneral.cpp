// PPgGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "PPgGeneral.h"
#include "Registry.h"
#include "MyUtility.h"
#include "SearchManager.h"
#include "Updater.h"
// CPPgGeneral dialog

IMPLEMENT_DYNAMIC(CPPgGeneral, CPropertyPage)

CPPgGeneral::CPPgGeneral() 
	: CPropertyPage(CPPgGeneral::IDD)
{

}

CPPgGeneral::~CPPgGeneral()
{
}

void CPPgGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_AUTOSTART, m_btnAutoStart);
	DDX_Control(pDX, IDC_COMBO_SEARCH, m_cbxSearchEngine);
}


BEGIN_MESSAGE_MAP(CPPgGeneral, CPropertyPage)
	ON_BN_CLICKED(IDCANCEL, &CPPgGeneral::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CPPgGeneral::OnBnClickedOk)
//	ON_BN_CLICKED(IDC_AUTOSTART, &CPPgGeneral::OnBnClickedAutostart)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH, &CPPgGeneral::OnCbnSelchangeComboSearch)
	ON_BN_CLICKED(IDC_RADIO_UPDATE_AUTO, &CPPgGeneral::OnBnClickedRadioUpdateAuto)
	ON_BN_CLICKED(IDC_RADIO_UPDATE_SEMIAUTO, &CPPgGeneral::OnBnClickedRadioUpdateSemiauto)
	ON_BN_CLICKED(IDC_RADIO_UDPATE_MANNUALLY, &CPPgGeneral::OnBnClickedRadioUdpateMannually)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CHECKUPDATES, &CPPgGeneral::OnBnClickedBtnCheckupdates)
	ON_BN_CLICKED(IDC_AUTOSTART, &CPPgGeneral::OnBnClickedAutostart)
END_MESSAGE_MAP()


// CPPgGeneral message handlers
void CPPgGeneral::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
}

void CPPgGeneral::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}

BOOL CPPgGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	InitCheckBox();
	InitSearchEngine();
	InitUpdateFlag();

	CUISubject::GetInstance()->Attatch(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPPgGeneral::InitUpdateFlag()
{
	CButton *pAuto		= static_cast<CButton*>(GetDlgItem(IDC_RADIO_UPDATE_AUTO));
	CButton *pSemiAuto	= static_cast<CButton*>(GetDlgItem(IDC_RADIO_UPDATE_SEMIAUTO));
	CButton *pManual	= static_cast<CButton*>(GetDlgItem(IDC_RADIO_UDPATE_MANNUALLY));

	// load config
	if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_AUTO)
		pAuto->SetCheck(TRUE);
	else
	if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_SEMIAUTO)
		pSemiAuto->SetCheck(TRUE);
	else
	if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_MANNUALLY)
		pManual->SetCheck(TRUE);
	else
		pAuto->SetCheck(TRUE);	// default
}

void CPPgGeneral::UpdateUI()
{
	if (m_hWnd)
	{

		SetWindowText(GetResString(IDS_PPG_GENERAL));

		GetDlgItem(IDC_AUTOSTART)->SetWindowText(GetResString(IDS_PPG_GENERAL_AUTOSTART));
		GetDlgItem(IDC_PPG_GENERAL_SEARCH_ENGINE_SETTING)->SetWindowText(GetResString(IDS_PPG_GENERAL_SEARCH_ENGINE_SETTING));
		GetDlgItem(IDC_PPG_GENERAL_DEFAULT_SEARCH_ENGINE)->SetWindowText(GetResString(IDS_PPG_GENERAL_DEFAULT_SEARCH_ENGINE));
		GetDlgItem(IDC_PPG_GENERAL_UPDATE_SETTING)->SetWindowText(GetResString(IDS_PPG_GENERAL_UPDATE_SETTING));
		GetDlgItem(IDC_RADIO_UPDATE_AUTO)->SetWindowText(GetResString(IDS_PPG_GENERAL_UPDATE_AUTO));
		GetDlgItem(IDC_RADIO_UPDATE_SEMIAUTO)->SetWindowText(GetResString(IDS_PPG_GENERAL_UPDATE_SEMIAUTO));
		GetDlgItem(IDC_RADIO_UDPATE_MANNUALLY)->SetWindowText(GetResString(IDS_PPG_GENERAL_UPDATE_MANNUALLY));
		GetDlgItem(IDC_BTN_CHECKUPDATES)->SetWindowText(GetResString(IDS_PPG_GENERAL_CHK_BUTTON));
	}
}

void CPPgGeneral::InitSearchEngine()
{
 	std::list<std::pair<int, std::tstring> > list;
 	CSearchManager::GetInstance()->GetSearchEngineList(list);
 
 	std::list<std::pair<int, std::tstring> >::iterator it;
	int i = 0;
	int iDefaultEngine = 0;
 	for (it=list.begin(); it!=list.end(); ++it)
 	{
		std::pair<int, std::tstring> item = *it;
		m_cbxSearchEngine.InsertString(i, item.second.c_str());
		m_cbxSearchEngine.SetItemData(i, item.first);
		if (item.first == thePrefs.GetDefaultSearchEngine())
			iDefaultEngine = i;
		++i;
 	}

	// select the default engine
	m_cbxSearchEngine.SetCurSel(iDefaultEngine);
}

void CPPgGeneral::InitCheckBox()
{
	CRegistry reg;

	if (reg.GetAutoStarWithWindowsFlag())
	{
		m_btnAutoStart.SetCheck(TRUE);
	}
	else
		m_btnAutoStart.SetCheck(FALSE);
}

// automatically start ViewBar with Windows
void CPPgGeneral::OnBnClickedAutostart()
{
	// TODO: Add your control notification handler code here
	CRegistry reg;
	
	CString strVB = CMyUtility::GetCurDir();
	strVB += _T("\\ViewBar.exe");
	if (m_btnAutoStart.GetCheck())
		reg.SetAutoStarWithWindows(strVB);
	else
		reg.SetAutoStarWithWindows(_T(""));

	SetModified();
}

void CPPgGeneral::OnCbnSelchangeComboSearch()
{
	SetModified(TRUE);
}

BOOL CPPgGeneral::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//int iSel = m_cbxSearchEngine.GetCurSel();
	//int iEngineId = m_cbxSearchEngine.GetItemData(iSel);

	//if (iEngineId>0)
	//{
	//	CSearchManager::GetInstance()->SetDefaultEngine(iEngineId);
	//}
	
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

BOOL CPPgGeneral::OnApply()
{
	CButton *pAuto		= static_cast<CButton*>(GetDlgItem(IDC_RADIO_UPDATE_AUTO));
	CButton *pSemiAuto	= static_cast<CButton*>(GetDlgItem(IDC_RADIO_UPDATE_SEMIAUTO));
	CButton *pManual	= static_cast<CButton*>(GetDlgItem(IDC_RADIO_UDPATE_MANNUALLY));

	// save config
	if (pAuto->GetCheck())
		thePrefs.SetUpdateFlag(CPreferences::UPDATE_AUTO);
	else
	if (pSemiAuto->GetCheck())
		thePrefs.SetUpdateFlag(CPreferences::UPDATE_SEMIAUTO);
	else
	if (pManual->GetCheck())
		thePrefs.SetUpdateFlag(CPreferences::UPDATE_MANNUALLY);

	// search engine
	int iSel = m_cbxSearchEngine.GetCurSel();
	int iEngineId = m_cbxSearchEngine.GetItemData(iSel);

	if (iEngineId>0)
	{
		CSearchManager::GetInstance()->SetDefaultEngine(iEngineId);
	}

	// save to registry
	thePrefs.SaveConfigToRegistry();

	return CPropertyPage::OnApply();
}

void CPPgGeneral::OnBnClickedRadioUpdateAuto()
{
	SetModified(TRUE);
}

void CPPgGeneral::OnBnClickedRadioUpdateSemiauto()
{
	SetModified(TRUE);
}

void CPPgGeneral::OnBnClickedRadioUdpateMannually()
{
	SetModified(TRUE);
}

void CPPgGeneral::OnDestroy()
{
	__super::OnDestroy();

	CUISubject::GetInstance()->Detatch(this);
}

void CPPgGeneral::OnBnClickedBtnCheckupdates()
{
	CUpdater updater;
	CString ver;
	updater.CheckStatus(ver);
	if (ver == thePrefs.GetVersion())
	{
		CString msg;
		msg.LoadString(IDS_PPG_GENERAL_CHK_UPDATES);
		MessageBox(msg);
		return;
	}

	//CString filename = CMyUtility::GetCurDir() + _T("\\Updater.exe");
	//ShellExecute(GetSafeHwnd(), _T("open"), filename, _T("0.0"), NULL, SW_SHOW);

	updater.CallUpdater(FALSE, FALSE);

}

