#include "StdAfx.h"
#include "SearchBox.h"

#include "SearchManager.h"
#include "AdsManager.h"
#include "utility/http/URLEncode.h"


CSearchBox::CSearchBox(void)
{
}

CSearchBox::~CSearchBox(void)
{
}


BEGIN_MESSAGE_MAP(CSearchBox, CEdit)
	//{{AFX_MSG_MAP(CSearchBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


void CSearchBox::Search()
{
	CString url, keyword;
	GetWindowText(keyword);
	CString originalKeyword = keyword;

	CStringA encodedKeyword;
	CURLEncode x;
	USES_CONVERSION;
	encodedKeyword = x.URLEncode(keyword);
	keyword = A2W(encodedKeyword);

	// CAdsManager::GetInstance()->GetAds(originalKeyword);		// call CAdsManager to show ads
	CAdsManager::GetInstance()->SendSearchKeyword(keyword);

	CString dummy;
	CSearchManager::GetInstance()->GetDefaultEngine(dummy, url);


	url.Replace(_T("{[%s]}"), keyword);
	if (thePrefs.GetDefaultBrowserName() == _T("IE"))
	{
		ShellExecute(NULL, _T("open"), _T("IEXPLORE"), url, 
			NULL, SW_SHOW); 
	}
	else
	{
		ShellExecute(NULL, _T("open"), url, 
			NULL, _T(""), SW_SHOW); 
	}
}

BOOL CSearchBox::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		int nVirtKey = (int)(pMsg->wParam);

		switch(nVirtKey)
		{
		case VK_RETURN:
			Search();
			break;

		case VK_DOWN:
			CSearchManager::GetInstance()->NextEngine();
			return TRUE;
			break;	// dummy

		case VK_UP:
			CSearchManager::GetInstance()->PrevEngine();
			return TRUE;
			break;	// dummy

		default:
			break;
		}
	}

	if (pMsg->message == 0)
	{
		return TRUE;
	}


	return CEdit::PreTranslateMessage(pMsg);
}
