// ***************************************************************
//  CXLogin   version:  1.0  date: 04/03/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "Biz.h"

namespace http
{
	class CXLogin : public CBiz
	{
	public:
		CXLogin(const CString &xml);
		virtual ~CXLogin(void);

		void FillRequest();

		static BOOL Login(const CString &username, const CString &password, CString &response);
	private:
		CString m_strXML;
	};
}