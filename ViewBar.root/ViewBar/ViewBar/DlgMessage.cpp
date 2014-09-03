// DlgMessage.cpp : implementation file
//

#include "stdafx.h"
#include "ViewBar.h"
#include "DlgMessage.h"


// CDlgMessage dialog

IMPLEMENT_DYNAMIC(CDlgMessage, CDialog)

CDlgMessage::CDlgMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMessage::IDD, pParent)
{

}

CDlgMessage::CDlgMessage(const CString &title, const CString &msg, LPTSTR lpszIcon)
	: CDialog(CDlgMessage::IDD)
{
	m_strTitle = title;
	m_strMsg = msg;
	m_lpszIcon = lpszIcon;
}

CDlgMessage::~CDlgMessage()
{
}

void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MSG_ICON, m_iconMsg);
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDlgMessage::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgMessage::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgMessage message handlers

void CDlgMessage::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgMessage::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgMessage::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}

BOOL CDlgMessage::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strTitle);
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText(m_strMsg);
	UpdateData(FALSE);
	
	HICON hIcon = ::LoadIcon(NULL, m_lpszIcon);
	m_iconMsg.SetIcon(hIcon);


	return TRUE;  // rSetIconeturn TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
