#if !defined(AFX_SEARCHCOMBO_s__10CA1C64_6776_4B30_AD6D_83EFED5B4580__INCLUDED_)
#define AFX_SEARCHCOMBO_s__10CA1C64_6776_4B30_AD6D_83EFED5B4580__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CSearchBox

class CSearchBox : public CEdit
{
	//DECLARE_DYNAMIC(CSearchBox)

public:
	CSearchBox();
	virtual ~CSearchBox();

	void Search();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchBox)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSearchCombo)
	//}}AFX_MSG

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHCOMBO_s__10CA1C64_6776_4B30_AD6D_83EFED5B4580__INCLUDED_)
