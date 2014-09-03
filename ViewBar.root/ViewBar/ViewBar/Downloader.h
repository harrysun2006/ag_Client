#pragma once

#include "utility/http/MyHttps.h"

namespace utility
{

	class CDownloader
	{
	public:
		CDownloader(void);
		~CDownloader(void);
	
	BOOL DownloadToFile(CString strURL, CString strPath, CString &strResult, BOOL bEncodeByUTF8 = FALSE);
	BOOL Download(CString strURL, CString &strResult);


	private:
		void Init();

	private:
		CMyHttps m_http;
	};
}