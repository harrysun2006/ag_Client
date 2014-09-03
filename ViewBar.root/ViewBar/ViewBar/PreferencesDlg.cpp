// PreferencesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "PreferencesDlg.h"

BOOL CPreferencesDlg::bShow = FALSE;
// CPreferencesDlg

CString GetResString(UINT uStringID)
{
	CString resString;
		resString.LoadString(uStringID);

	if (resString.IsEmpty())
		resString.LoadString(GetModuleHandle(NULL), uStringID);
	return resString;
}

IMPLEMENT_DYNAMIC(CPreferencesDlg, CTreePropSheet)

CPreferencesDlg::CPreferencesDlg()
{
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_wndGeneral.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndDisplay.m_psp.dwFlags &= ~PSH_HASHELP;

	TreePropSheet::CTreePropSheet::SetPageIcon(&m_wndGeneral, IDI_ICON_GENERAL);
	TreePropSheet::CTreePropSheet::SetPageIcon(&m_wndDisplay, IDI_ICON_DISPLAY);
	TreePropSheet::CTreePropSheet::SetPageIcon(&m_wndNetwork, IDI_ICON_NETWORK);

	AddPage(&m_wndGeneral);
	AddPage(&m_wndDisplay);
	AddPage(&m_wndNetwork);

	//SetTitle(_T("Prefecenes"));
	CString title;
	title.LoadString(IDS_PPG_PREFERENCES);
	SetTitle(title);

	SetTreeViewMode(TRUE, TRUE, TRUE);
	SetTreeWidth(130);
}

CPreferencesDlg::~CPreferencesDlg()
{
}


BEGIN_MESSAGE_MAP(CPreferencesDlg, CTreePropSheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CPreferencesDlg message handlers


BOOL CPreferencesDlg::OnInitDialog() 
{
	// remove 'HIDE' button
	GetDlgItem(IDHELP)->ShowWindow(SW_HIDE); 

	CPreferencesDlg::bShow = TRUE;

	//BOOL bResult = CPropertySheet::OnInitDialog();
	BOOL bResult = CTreePropSheet::OnInitDialog();

	// TODO: Add your specialized code here

	for (int i = 0; i < m_pages.GetSize(); i++)
	{
		SetActivePage(i);
	}
	SetActivePage(0);

	for (int i = 0; i < m_pages.GetSize(); i++)
	{
		if (GetPage(i)->m_psp.pszTemplate == m_pPshStartPage)
		{
			SetActivePage(i);
			break;
		}
	}

	CenterWindow();

	// attach self to observer list
	CUISubject *pUI = CUISubject::GetInstance();
	pUI->Attatch(this);

	if (thePrefs.IsUserLoggedIn() == false)
	{
		SetUnsigninMode();

		// active network page
		for (int i = 0; i < m_pages.GetSize(); i++)
		{
			if (GetPage(i)->m_psp.pszTemplate == MAKEINTRESOURCE(IDD_PPG_NETWORK))
			{
				SetActivePage(i);
				break;
			}
		}

	}

	return bResult;
}

void CPreferencesDlg::DisableWin(CWnd *pWnd)
{
	CWnd *pWndCtrl = NULL;

	pWndCtrl = pWnd->GetWindow(GW_CHILD);
	while (pWndCtrl)
	{
		if (IsWindow(pWndCtrl->GetSafeHwnd()) && pWndCtrl->GetDlgCtrlID() != IDC_BTN_CHECKUPDATES)
		{
			pWndCtrl->EnableWindow(FALSE);
		}

		pWndCtrl   =   pWndCtrl->GetWindow(GW_HWNDNEXT);
	}
}

void CPreferencesDlg::SetUnsigninMode()
{
	DisableWin(&m_wndDisplay);
	DisableWin(&m_wndGeneral);
}

void CPreferencesDlg::SetStartPage(UINT uStartPageID)
{
	m_pPshStartPage = MAKEINTRESOURCE(uStartPageID);
}

void CPreferencesDlg::UpdateUI()
{
	CString title;
	title.LoadString(IDS_PPG_PREFERENCES);
	SetTitle(title);

	int c = 0;

	CTreeCtrl* pTree = GetPageTreeControl();
	if (pTree)
	{
		pTree->SetItemText(GetPageTreeItem(c++), GetResString(IDS_PPG_GENERAL));
		pTree->SetItemText(GetPageTreeItem(c++), GetResString(IDS_PPG_DISPLAY));
		pTree->SetItemText(GetPageTreeItem(c++), GetResString(IDS_PPG_NETWORK));
	}

	UpdateCaption();
}

void CPreferencesDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO: Add your message handler code here

	CUISubject *pUI = CUISubject::GetInstance();
	pUI->Detatch(this);
}
