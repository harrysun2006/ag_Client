#pragma once
#include "Biz.h"
#define SERVICE_AFFILIATE "affiliateService"
namespace http
{

	class CXAffiliate : public CBiz
	{
	public:
		CXAffiliate(DWORD dwLastTime,DWORD lastTimeDetail);
		CXAffiliate(CString &xml);
		~CXAffiliate(void);

		void FillRequest(){};
	};
}