#pragma once
#include "Biz.h"

namespace http
{

	class CXAdsUpdate : public CBiz
	{
	public:
		CXAdsUpdate(const CString &strKeyworkd);
	public:
		~CXAdsUpdate(void);

		void FillRequest();
		static void DownloadAds(const CString &strKeyword, CString &strResponse);

	private:
		CString m_strKeyword;
	};
}
