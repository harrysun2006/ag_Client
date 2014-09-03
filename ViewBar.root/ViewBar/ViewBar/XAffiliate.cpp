#include "StdAfx.h"
#include "XAffiliate.h"
namespace http
{

CXAffiliate::CXAffiliate(CString &xml)
{
	CBiz::FillRequest(xml);
}

CXAffiliate::CXAffiliate(DWORD dwLastTime,DWORD lastTimeDetail)
{
	CString xml;
	xml.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?><affiliateReq afTime=\"%d\" afdTime=\"%d\" />"),
		dwLastTime,lastTimeDetail);
	CBiz::FillRequest(xml);
}

CXAffiliate::~CXAffiliate(void)
{
}


}