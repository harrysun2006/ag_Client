// ***************************************************************
//  CPPgDisplay   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  a dialog for configurating UI
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once
#include "afxwin.h"

#include "Resource.h"
#include "LangManager.h"
#include "SkinManager.h"
#include "DockFacade.h"

// CPPgDisplay dialog

class CPPgDisplay : public CPropertyPage,viewbar::IUIObserver
{
	DECLARE_DYNAMIC(CPPgDisplay)

public:
	CPPgDisplay();   // standard constructor
	virtual ~CPPgDisplay();

// Dialog Data
	enum { IDD = IDD_PPG_DISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnApply();

	DECLARE_MESSAGE_MAP()

	BOOL InitLangList();
	BOOL InitSkinList();
	BOOL InitDockList();
	BOOL InitFavorites();
	BOOL InitWarningFlag();

	void UpdateUI();

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCmbLang();

	virtual BOOL OnInitDialog();
private:
	CComboBox m_cmbLanguage;
	CComboBox m_cmbSkin;
	CComboBox m_cmbDock;

	CStringArray m_listLang;
	CStringArray m_listSkin;
	afx_msg void OnCbnSelchangeCmbSkin();
	afx_msg void OnCbnSelchangeCmbDock();
	CButton m_checkFavIE;
	CButton m_checkFavFF;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckFirefox();
	afx_msg void OnBnClickedChkWarning();
	CButton m_btnWarning;
};
