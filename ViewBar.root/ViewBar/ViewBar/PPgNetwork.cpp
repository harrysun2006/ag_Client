// PPgNetwork.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "PPgNetwork.h"


// CPPgNetwork dialog

IMPLEMENT_DYNAMIC(CPPgNetwork, CPropertyPage)

CPPgNetwork::CPPgNetwork()
	: CPropertyPage(CPPgNetwork::IDD)
	, m_iProxyPort(0)
	, m_strProxyUser(_T(""))
	, m_strProxyPasswd(_T(""))
{

}

CPPgNetwork::~CPPgNetwork()
{
}

void CPPgNetwork::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROXY_PORT, m_iProxyPort);
	DDX_Text(pDX, IDC_EDIT_PROXY_USER, m_strProxyUser);
	DDX_Text(pDX, IDC_EDIT_PROXY_PASSWD, m_strProxyPasswd);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ProxyIp);
	DDX_Control(pDX, IDC_CHECK_PROXY, m_btnProxyEnable);
}


BEGIN_MESSAGE_MAP(CPPgNetwork, CPropertyPage)
	ON_BN_CLICKED(IDOK, &CPPgNetwork::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPPgNetwork::OnBnClickedCancel)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CPPgNetwork::OnIpnFieldchangedIpaddress1)
	ON_EN_CHANGE(IDC_EDIT_PROXY_PORT, &CPPgNetwork::OnEnChangeEditProxyPort)
	ON_EN_CHANGE(IDC_EDIT_PROXY_USER, &CPPgNetwork::OnEnChangeEditProxyUser)
	ON_EN_CHANGE(IDC_EDIT_PROXY_PASSWD, &CPPgNetwork::OnEnChangeEditProxyPasswd)
	ON_BN_CLICKED(IDC_CHECK_PROXY, &CPPgNetwork::OnBnClickedCheckProxy)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPPgNetwork message handlers

BOOL CPPgNetwork::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CUISubject::GetInstance()->Attatch(this);

	m_ProxyIp.SetWindowText(thePrefs.GetProxyIp());

	// set proxy port
	CString port = thePrefs.GetProxyPort();

#ifdef _UNICODE
	m_iProxyPort = _wtoi(port);
#else
	m_iProxyPort = atoi(port);
#endif

	// set proxy user
	m_strProxyUser = thePrefs.GetProxyUser();

	// set proxy password
	m_strProxyPasswd = thePrefs.GetProxyPassword();

	if (thePrefs.IsProxyEnable())
	{
		m_btnProxyEnable.SetCheck(TRUE);
	}
	else
	{
		m_btnProxyEnable.SetCheck(FALSE);
	}

	SetControlState();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPPgNetwork::UpdateUI()
{
	if (m_hWnd)
	{
		SetWindowText(GetResString(IDS_PPG_NETWORK));

		GetDlgItem(IDC_STATIC_PROXY_IP)->SetWindowText(GetResString(IDS_PPG_NETWORK_PROXYIP));
		GetDlgItem(IDC_STATIC_PROXY_PORT)->SetWindowText(GetResString(IDS_PPG_NETWORK_PROXYPORT));
		GetDlgItem(IDC_STATIC_PROXY_USER)->SetWindowText(GetResString(IDS_PPG_NETWORK_USERNAME));
		GetDlgItem(IDC_STATIC_PROXY_PASSWD)->SetWindowText(GetResString(IDS_PPG_NETWORK_PASSWD));
		GetDlgItem(IDC_CHECK_PROXY)->SetWindowText(GetResString(IDS_PPG_NETWORK_ENABLEPROXY));
	}
}

void CPPgNetwork::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	// OnOK();
}

void CPPgNetwork::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	// OnCancel();
}


BOOL CPPgNetwork::OnApply()
{
	SetModified(FALSE);

	DWORD dwIpAddress;

	m_ProxyIp.GetAddress(dwIpAddress);   
	CString ip;
	ip.Format(_T("%d.%d.%d.%d"),
		HIBYTE(HIWORD(dwIpAddress)),   
		LOBYTE(HIWORD(dwIpAddress)),   
		HIBYTE(LOWORD(dwIpAddress)),   
		LOBYTE(LOWORD(dwIpAddress))   
		);   

	thePrefs.SetProxyIp(ip);

	thePrefs.SetProxyUser(m_strProxyUser);
	thePrefs.SetProxyPassword(m_strProxyPasswd);
	CString port;
	port.Format(_T("%d"), m_iProxyPort);
	thePrefs.SetProxyPort(port);

	if (m_btnProxyEnable.GetCheck())
		thePrefs.SetProxyEnable(true);
	else
		thePrefs.SetProxyEnable(false);

	// save to registry
	thePrefs.SaveConfigToRegistry();

	return TRUE;
}
void CPPgNetwork::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: Add your control notification handler code here
	SetModified();
	*pResult = 0;
}

void CPPgNetwork::OnEnChangeEditProxyPort()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SetModified();
}

void CPPgNetwork::OnEnChangeEditProxyUser()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SetModified();
}

void CPPgNetwork::OnEnChangeEditProxyPasswd()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	SetModified();
}

void CPPgNetwork::OnBnClickedCheckProxy()
{
	// TODO: Add your control notification handler code here
	SetModified();
	SetControlState();
}

void CPPgNetwork::OnDestroy()
{
	__super::OnDestroy();

	CUISubject::GetInstance()->Detatch(this);
}

void CPPgNetwork::SetControlState()
{
	BOOL bEnale = m_btnProxyEnable.GetCheck();

	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(bEnale);
	GetDlgItem(IDC_EDIT_PROXY_PORT)->EnableWindow(bEnale);
	GetDlgItem(IDC_EDIT_PROXY_USER)->EnableWindow(bEnale);
	GetDlgItem(IDC_EDIT_PROXY_PASSWD)->EnableWindow(bEnale);
}