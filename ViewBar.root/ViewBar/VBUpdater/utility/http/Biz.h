// ***************************************************************
//  CBiz   version:  1.0  date: 04/03/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "..\VBSession.h"

namespace http
{

	class CBiz
	{
	public:
		CBiz(void);
		virtual	~CBiz(void);

		BOOL Request(CString strServiceName, CString &response);
		BOOL RequestURL(CString strServiceName, CString &response);
	protected:
		virtual void FillRequest() = 0;		// fill the body
		void FillRequest(CString &xml);
		void FillRequest(std::string &xml);

	private:
		CString m_strRequest;
		std::string m_Request;
	};
}
