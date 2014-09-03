#include "..\StdAfx.h"
#include "VBFile.h"

#define BUFSIZE_READFILE (1024*8)
CVBFile::CVBFile(void)
{
	m_buffer = new char[BUFSIZE_READFILE];
}

CVBFile::~CVBFile(void)
{
	if( m_buffer )
		delete [] m_buffer;
}

BOOL CVBFile::GetFile( CString strPath, CString strFile, CString strHttp )
{
	CInternetSession sess;
	CHttpFile *phttpFile;
	DWORD dwFlag = INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_DONT_CACHE |
		INTERNET_FLAG_RELOAD;
	char *bufFile = NULL;

	try
	{
		phttpFile = (CHttpFile*)sess.OpenURL(strHttp, 1, dwFlag); 
		if( NULL == phttpFile )
			return FALSE;

		CString Status;
		phttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE, Status);
		if(Status != _T("200"))
		{
			phttpFile->Close();
			delete phttpFile;
			return FALSE;
		}
		phttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, Status); // file's length
		int nLenFile = _ttol(Status);
		int nLen = nLenFile;

		CFile file;
		if(file.Open(strPath+strFile,CFile::modeCreate | CFile::modeWrite))
		{
			while(1)
			{
				int n = phttpFile->Read(m_buffer,(nLenFile < BUFSIZE_READFILE) ? nLenFile : BUFSIZE_READFILE);
				if( n <= 0 )
					break;
				file.Write(m_buffer,n);
				nLenFile -= n;
			}
			file.Close();
			phttpFile->Close();
			delete phttpFile;
		}
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

/*bool CVBFile::getFile(TCHAR *url, TCHAR *location)
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

		if( !lfile.Open(location, CFile::modeCreate | CFile::modeWrite) )
		{
			DWORD error = GetLastError();
		}
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
}*/