#include "StdAfx.h"
#include "VBFile.h"

CVBFile::CVBFile(void)
{
}

CVBFile::~CVBFile(void)
{

	
}
bool CVBFile::getFile(TCHAR *url, TCHAR *location)
{
	bool bRet = true;
	
	CInternetSession   sess ;
	CHttpFile   *phttpFile;
	CString Status;
	CFile lfile;
	char *buf=NULL;
	char *pbuf=NULL;

	DWORD dwFlag = INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE |
					INTERNET_FLAG_RELOAD ;

	try
	{
		//here, should think of proxy SetOption.
		//
		//INTERNET_OPTION_PROXY 
		//INTERNET_OPTION_PROXY_PASSWORD 
		//INTERNET_OPTION_PROXY_USERNAME 


		phttpFile = (CHttpFile*)sess.OpenURL(url, 1, dwFlag); 

		VERIFY(phttpFile);

		phttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE, Status) ;
		if (Status != _T("200"))
		{
			bRet=false;
			phttpFile->Close();
			delete phttpFile ;
			goto lExit;
		}

		phttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, Status) ; // file's length
		long filelen = _ttol(Status);
		long totalLen = filelen;
		buf = new char[filelen+1];
		pbuf = buf;

		::ZeroMemory(buf, filelen+1);

		while(true)
		{
			int   n = phttpFile->Read (buf, (filelen < 8192) ? filelen : 8192) ;
			if (n <= 0)
				break ;
			buf += n ; filelen -= n ;
		}
		phttpFile->Close();
		delete phttpFile;

		lfile.Open(location, CFile::modeCreate | CFile::modeWrite);
		lfile.Write((void*)pbuf, totalLen);
		lfile.Close();
	}
	catch(...)
	{
		bRet = false;
		goto lExit;
	}

lExit:
	delete pbuf;
	return bRet;
}