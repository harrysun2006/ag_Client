#pragma once
#include "afxwin.h"


// CDlgMessage dialog

class CDlgMessage : public CDialog
{
	DECLARE_DYNAMIC(CDlgMessage)

public:
	CDlgMessage(CWnd* pParent = NULL);   // standard constructor
	CDlgMessage(const CString &title, const CString &msg, LPTSTR lpszIcon);
	virtual ~CDlgMessage();

// Dialog Data
	enum { IDD = IDD_DLG_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CString m_strTitle;
	CString m_strMsg;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	CStatic m_iconMsg;
	LPTSTR m_lpszIcon;
};
