#pragma once

#include "Biz.h"

namespace http
{

	class CXAutoUPdate :
		public CBiz
	{
	public:
		CXAutoUPdate(void);
	public:
		~CXAutoUPdate(void);
		virtual void FillRequest();
		bool fetchRequest(CString servername, CString in, CString &out); 
	};
}
