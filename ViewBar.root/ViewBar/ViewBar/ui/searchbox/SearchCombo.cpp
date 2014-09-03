// SearchCombo.cpp : implementation file
//

#include "stdafx.h"
#include "SearchCombo.h"
#include "../../SearchManager.h"
#include "../../AdsManager.h"
#include "../../utility/http/URLEncode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchCombo

CString CSearchCombo::m_strHistory = _T("");

CSearchCombo::CSearchCombo() : m_nDefaultIndex(0)
{
}

CSearchCombo::~CSearchCombo()
{
}


BEGIN_MESSAGE_MAP(CSearchCombo, CComboBox)
	//{{AFX_MSG_MAP(CSearchCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_SELENDCANCEL, OnSelendcancel)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
//	ON_WM_CREATE()
ON_CONTROL_REFLECT(CBN_EDITCHANGE, &CSearchCombo::OnCbnEditchange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchCombo message handlers

void CSearchCombo::AddSearchEngine(int iEngineId, const CString &name)
{
	//m_strSearchName.Add(name);
	//m_strSearchUrl.Add(url);

	int iPos = GetCount();
	//InsertString(iPos, name);
	InsertString(iPos, _T(""));
	SetItemData(iPos, iEngineId);
}

void CSearchCombo::SetDefaultSearchEngine(int i)
{
	m_nDefaultIndex = i;
}


void CSearchCombo::OnDropdown() 
{
//	SetDefaultSearchEngine(GetCurSel());
//	SetCurSel(m_nDefaultIndex);
}

// change the default search engine
void CSearchCombo::OnSelchange() 
{
	// GetWindowText(m_strHistory);

	int nSel = GetCurSel();
	if (nSel >=0)
	{
		CSearchManager::GetInstance()->SetDefaultEngine(GetItemData(nSel));
		//SetWindowText(_T(""));
		//SetWindowText(m_strHistory);
		//UpdateData(FALSE);
	}
//		SetDefaultSearchEngine(nSel);
}

BOOL CSearchCombo::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		int nVirtKey = (int)(pMsg->wParam);
		
		if( nVirtKey == VK_RETURN )
		{
// 			CString tmp;
// 			if (m_nDefaultIndex >=0)
// 			{
// 				tmp = m_strSearchName.GetAt(m_nDefaultIndex);
// 				Search();
// 			}
			Search();
		}
	}
	else
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		CString tmp;

		// if Search Engine Name in CComboBox, clear it
		GetWindowText(tmp);
		CString name, dummy;
		CSearchManager::GetInstance()->GetDefaultEngine(name, dummy);
		if (tmp == name)
			Clear();
// 
// 		if (m_nDefaultIndex >=0 &&
// 			tmp == m_strSearchName.GetAt(m_nDefaultIndex))
// 			Clear();
	}
	
	if (pMsg->message == 0)
	{
		return TRUE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}


void CSearchCombo::OnSelendcancel() 
{
	// if setting default search engine being canceled
	// , then set the first one default

	//m_nDefaultIndex = 0;
} 

void CSearchCombo::Search()
{
	CString url, keyword;
	GetWindowText(keyword);
	CString originalKeyword = keyword;

 	CStringA encodedKeyword;
 	CURLEncode x;
 	USES_CONVERSION;
 	encodedKeyword = x.URLEncode(keyword);
 	keyword = A2W(encodedKeyword);
	CAdsManager::GetInstance()->GetAds(originalKeyword);		// call CAdsManager to show ads

	CString dummy;
	CSearchManager::GetInstance()->GetDefaultEngine(dummy, url);
	

	url.Replace(_T("{[%s]}"), keyword);
	ShellExecute(NULL, _T("open"), url, 
		NULL, _T(""), SW_SHOW); 
}

void CSearchCombo::OnSetfocus() 
{
	static int isFisrtTime = 1;
	if (isFisrtTime == 1)
	{
		Clear();
		isFisrtTime = 0;
	}
}


void CSearchCombo::Init()
{
	//SetCurSel(m_nDefaultIndex);
}

void CSearchCombo::PreSubclassWindow()
{
	::SendMessage(m_hWnd, CB_SETITEMHEIGHT, (WPARAM)-1, 100);


	CComboBox::PreSubclassWindow();
}

void CSearchCombo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Do not call CComboBox::OnPaint() for painting messages

	//ModifyStyleEx(0, ES_AUTOHSCROLL);

	return;




	ModifyStyleEx (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,
		0, SWP_FRAMECHANGED);

	Default();

	CRect rcItem;
	GetClientRect(&rcItem);
	CDC* pDC = GetDC();

	COLORREF m_clrBtnHilite  = ::GetSysColor(COLOR_BTNHILIGHT);
	COLORREF m_clrBtnShadow  = ::GetSysColor(COLOR_BTNSHADOW);
	COLORREF m_clrBtnFace    = ::GetSysColor(COLOR_BTNFACE);


	// Cover up dark 3D shadow.
	pDC->Draw3dRect(rcItem, m_clrBtnFace, m_clrBtnFace);
	rcItem.DeflateRect(1,1);

	if (!IsWindowEnabled()) {
		pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnHilite );
	}

	else {
		pDC->Draw3dRect( rcItem, m_clrBtnFace, m_clrBtnFace );
	}

	// Cover up dark 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	int m_nOffset		= ::GetSystemMetrics(SM_CXHTHUMB);
	rcItem.left = rcItem.right-m_nOffset;
	pDC->Draw3dRect( rcItem, m_clrBtnFace, m_clrBtnFace );

	// Cover up normal 3D shadow on drop arrow.
	rcItem.DeflateRect(1,1);
	pDC->Draw3dRect( rcItem, m_clrBtnFace, m_clrBtnFace );

	if (!IsWindowEnabled()) {
		return;
	}



	//rcItem.top -= 1;
	//rcItem.bottom += 1;
	//pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnHilite );
	//rcItem.left -= 1;
	//pDC->Draw3dRect( rcItem, m_clrBtnHilite, m_clrBtnHilite );

	ReleaseDC(pDC);

}

void CSearchCombo::OnCbnEditchange()
{
	GetWindowText(m_strHistory);
}
