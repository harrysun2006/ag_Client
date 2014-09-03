// ***************************************************************
//  CPPgNetwork   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  a dialog for configurating network
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CPPgNetwork dialog

class CPPgNetwork : public CPropertyPage, viewbar::IUIObserver
{
	DECLARE_DYNAMIC(CPPgNetwork)

public:
	CPPgNetwork();   // standard constructor
	virtual ~CPPgNetwork();
	virtual BOOL OnApply();

// Dialog Data
	enum { IDD = IDD_PPG_NETWORK };

protected:
	void UpdateUI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iProxyPort;
	CString m_strProxyUser;
	CString m_strProxyPasswd;
	CIPAddressCtrl m_ProxyIp;
	CButton m_btnProxyEnable;


	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditProxyPort();
	afx_msg void OnEnChangeEditProxyUser();
	afx_msg void OnEnChangeEditProxyPasswd();
	afx_msg void OnBnClickedCheckProxy();
	afx_msg void OnDestroy();

private:
	void SetControlState();
};
