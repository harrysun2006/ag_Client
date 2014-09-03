#if !defined(AFX_SEARCHCOMBO_H__10CA1C64_6776_4B30_AD6D_83EFED5B4580__INCLUDED_)
#define AFX_SEARCHCOMBO_H__10CA1C64_6776_4B30_AD6D_83EFED5B4580__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchCombo.h : header file
//
// ***************************************************************
//  CSearchCombo   version:  1.0  date: 03/19/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************


/////////////////////////////////////////////////////////////////////////////
// CSearchCombo window
class CSearchCombo : public CComboBox
{
// Construction
public:
	CSearchCombo();
static CString m_strHistory;
// Attributes
public:

// Operations
public:

	void AddSearchEngine(int iEngineId, const CString &name);
	void SetDefaultSearchEngine(int i);

	void Init();
	void Search();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSearchCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSearchCombo)
	afx_msg void OnDropdown();
	afx_msg void OnSelchange();
	afx_msg void OnSelendcancel();
	afx_msg void OnSetfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CStringArray m_strSearchUrl;
	CStringArray m_strSearchName;
	int m_nDefaultIndex;
	virtual void PreSubclassWindow();
public:
	afx_msg void OnPaint();

private:
	
	afx_msg void OnCbnEditchange();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHCOMBO_H__10CA1C64_6776_4B30_AD6D_83EFED5B4580__INCLUDED_)
