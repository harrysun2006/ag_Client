// ***************************************************************
//  CXLogout   version:  1.0  date: 04/03/2007
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

	class CXLogout : public CBiz
	{
	public:
		CXLogout(void);
		~CXLogout(void);
		static void Logout();


	void FillRequest();
		
	};
}
