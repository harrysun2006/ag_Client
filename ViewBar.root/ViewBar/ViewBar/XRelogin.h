#pragma once
#include "Biz.h"

namespace http
{
	class CXRelogin : public CBiz
	{

	public:
		CXRelogin(void);
		CXRelogin(const CString &xml);
		void FillRequest();
		static BOOL Login();
	public:
		~CXRelogin(void);
	private:
		CString m_strXML;
	};
}
