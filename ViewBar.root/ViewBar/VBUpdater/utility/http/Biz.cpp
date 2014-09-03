#include "..\..\StdAfx.h"
#include "Biz.h"
#include "..\VBSession.h"

namespace http
{

	CBiz::CBiz(void)
	{
	}

	CBiz::~CBiz(void)
	{
	}

	BOOL CBiz::Request(CString strServiceName, CString &response)
	{
		FillRequest();

		CVBSession::GetInstance()->Request(strServiceName, m_strRequest, response);
		return TRUE;
	}

	BOOL CBiz::RequestURL(CString strServiceName, CString &response)
	{
		FillRequest();

		CVBSession::GetInstance()->Request(strServiceName, m_Request, response);
		return TRUE;
	}

	void CBiz::FillRequest(CString &xml)
	{
		m_strRequest = xml;
	}

	void CBiz::FillRequest(std::string &xml)
	{
		m_Request = xml;
	}

}
