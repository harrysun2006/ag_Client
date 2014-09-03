#include "StdAfx.h"
#include "XAutoUPdate.h"

namespace http
{

	CXAutoUPdate::CXAutoUPdate(void)
	{
	}

	CXAutoUPdate::~CXAutoUPdate(void)
	{
	}

	void CXAutoUPdate::FillRequest()
	{

	}

	/*void CXAutoUPdate::test(CString &out)
	{
		CString tmp = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>   <viewbarReq viewbarId=\"w98.1.1.0.1\" /> ");
		CBiz::FillRequest(tmp);
		this->Request(_T("viewbarCheckService"), out);
	}
	*/
	bool CXAutoUPdate::fetchRequest(CString servername, CString in, CString &out)
	{
		bool bRet=true;
		
		try
		{
			CBiz::FillRequest(in );

			Request(servername, out);
		}
		catch(CException *e)
		{
			bRet = false;
		}

		return bRet;
	}

}