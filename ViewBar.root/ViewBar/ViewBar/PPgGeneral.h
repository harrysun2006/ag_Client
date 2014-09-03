// ***************************************************************
//  CPPgGeneral   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  a dialog for configurating general settings
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "Resource.h"
#include "afxwin.h"

// CPPgGeneral dialog

class CPPgGeneral : public CPropertyPage, viewbar::IUIObserver
{
	DECLARE_DYNAMIC(CPPgGeneral)

public:
	CPPgGeneral();   // standard constructor
	virtual ~CPPgGeneral();

// Dialog Data
	enum { IDD = IDD_PPG_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void InitCheckBox();
	void InitSearchEngine();
	void InitUpdateFlag();
	void UpdateUI(/* = 0 */);

	CButton m_btnAutoStart;
	CComboBox m_cbxSearchEngine;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
//	afx_msg void OnBnClickedAutostart();
	afx_msg void OnCbnSelchangeComboSearch();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	virtual BOOL OnApply();
public:
	afx_msg void OnBnClickedRadioUpdateAuto();
public:
	afx_msg void OnBnClickedRadioUpdateSemiauto();
public:
	afx_msg void OnBnClickedRadioUdpateMannually();
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnBnClickedBtnCheckupdates();
public:
	afx_msg void OnBnClickedAutostart();
};
