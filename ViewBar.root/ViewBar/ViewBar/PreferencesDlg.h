// ***************************************************************
//  CPreferencesDlg   version:  1.0  date: 03/19/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once


// CPreferencesDlg
#include "ui/proppage/TreePropSheet.h"
#include "PPgGeneral.h"
#include "PPgDisplay.h"
#include "PPgNetwork.h"

class CPreferencesDlg : public TreePropSheet::CTreePropSheet, public viewbar::IUIObserver
{
	DECLARE_DYNAMIC(CPreferencesDlg)

public:
	CPreferencesDlg();
	virtual ~CPreferencesDlg();

	static BOOL bShow;

	void SetStartPage(UINT uStartPageID);
	void UpdateUI();
	void SetUnsigninMode();		// when user doesn't sign in, disable related controls

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	void DisableWin(CWnd *pWnd);

private:
	CPPgGeneral		m_wndGeneral;
	CPPgDisplay		m_wndDisplay;
	CPPgNetwork		m_wndNetwork;

	LPCTSTR m_pPshStartPage;
	afx_msg void OnDestroy();
};

