#include "StdAfx.h"
#include "XSendSurfTime.h"
namespace http
{


CXSendSurfTime::CXSendSurfTime(void)
{
}

CXSendSurfTime::~CXSendSurfTime(void)
{
}

CXSendSurfTime::CXSendSurfTime(CString &xml)
{
	CBiz::FillRequest(xml);
}

void CXSendSurfTime::FillRequest()
{
}

}