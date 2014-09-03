#include "StdAfx.h"
#include "XLogout.h"

namespace http
{

	CXLogout::CXLogout(void)
	{
	}

	CXLogout::~CXLogout(void)
	{
	}


	void CXLogout::FillRequest()
	{
		CString xml;
		xml.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><logoutReq memberId=\"%d\"/>"),
			thePrefs.GetMemberId());

		CBiz::FillRequest(xml);
	}

	void CXLogout::Logout()
	{
		CString response;
		CBiz *pBiz = new CXLogout;
		pBiz->Request(_T("logoutService"), response);
		delete pBiz;
	}
}
