#include "StdAfx.h"
#include "Downloader.h"
#include "MyUtility.h"

namespace utility
{

	CDownloader::CDownloader(void)
	{
		Init();
	}

	CDownloader::~CDownloader(void)
	{
	}

	void CDownloader::Init()
	{
		CString strServer = _T("viewbar.agloco.com");
		m_http.SetHttpServer(strServer, 80, FALSE);
	}

	BOOL CDownloader::DownloadToFile(CString strURL, CString strPath, CString &strResult, BOOL bEncodeByUTF8 /* = FALSE */)
	{
		BOOL bDownload = Download(strURL, strResult);
		if (bDownload == FALSE)
		{
			return FALSE;
		}

		BOOL bOpen = FALSE;
		CString strFilePath;
		CFile file;
		try
		{
			CString path = CMyUtility::GetCurDir();
			path = path + strPath;
			bOpen = file.Open(path, CFile::modeCreate | CFile::modeWrite);
			if (bOpen)
			{
				if (bEncodeByUTF8)
				{
					CStringA strUFF8Body;
					strUFF8Body = CMyUtility::GetUTF8CStringA(strResult);

					file.Write(strUFF8Body, (strUFF8Body.GetLength()+1) * sizeof( char ));
				}
				else
				{
					file.Write(strResult, (strResult.GetLength()+1) * sizeof( TCHAR ));
				}
				
			}
		}
		catch (CException* e)
		{
			e->Delete();
			if (bOpen)
			{
				file.Close();
			}

			return FALSE;
		}

		if (bOpen)
		{
			file.Close();
			return TRUE;
		}

		return FALSE;
	}

	BOOL CDownloader::Download(CString strURL, CString &strResult)
	{
		return m_http.Get(strURL, strResult);
	}
}
