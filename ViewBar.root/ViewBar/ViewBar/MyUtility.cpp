
#include "StdAfx.h"
//#include <Winbase.h>

#include "MyUtility.h"
#include <stdlib.h>

// for GetAdapterInfo()
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

#include <WinSock2.h>
#pragma comment(lib,"Ws2_32.lib")

UINT CMyUtility::m_uProcessMsg = 0;

CMyUtility::CMyUtility(void)
{
	m_uProcessMsg = 0;
}

CMyUtility::~CMyUtility(void)
{
}


CString CMyUtility::GetCurDir()
{
	// get the current work dir
	CString dir;
	TCHAR FileName[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(), FileName, MAX_PATH);
	dir = FileName;
	dir = dir.Left(dir.ReverseFind(_T('\\')));

	return dir;
}


void CMyUtility::GetFileList(CStringArray &langs, const CString& subDir)
{
	CFileFind finder;
	CString dir = CMyUtility::GetCurDir();
	CString strLangs;
	strLangs.Format(_T("%s\\%s\\*.*"), dir, subDir);

	BOOL bFind = finder.FindFile(strLangs);
	if(!bFind)
		return;

	while(bFind)
	{
		bFind = finder.FindNextFile();
		if(finder.IsDots())
			continue;

		if(finder.IsDirectory() == FALSE)
		{
			langs.Add(finder.GetFileName());
			continue;
		}
	}

	finder.Close();
}

// !!!!!!!!!!!!!!----Warning----!!!!!!!!!!!!!!
// you must delete the memory after operation
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
char *CMyUtility::GetAnsiString(const CString& unicodeString)
{
	//int iLen = unicodeString.GetLength() + 1;
	//char *pAnsi = new char[iLen];
	//memset(pAnsi, 0, iLen);
	//WideCharToMultiByte(
	//	CP_ACP,
	//	0,
	//	unicodeString,
	//	-1,
	//	pAnsi,
	//	iLen,
	//	NULL,
	//	NULL);

	CString wString = unicodeString;
	CStringA mbString;

	int cbLen = unicodeString.GetLength() * 3;

	char *pStr = new char[cbLen + 1];
	memset(pStr, 0, cbLen + 1);

	WideCharToMultiByte(CP_ACP, 0, wString, -1, pStr, cbLen, NULL, NULL);

	return pStr;
}

CStringA CMyUtility::GetCStringA(const CString &unicodeString)
{
	USES_CONVERSION;
	return (W2A(unicodeString));
}

CStringW CMyUtility::GetCStringW(const CStringA &ansiString)
{
	USES_CONVERSION;
	return (A2W(ansiString));
}

CStringW CMyUtility::GetCStringW2(const CStringA &ansiString)
{
	int iLen = ansiString.GetLength() + 1;
	//char *pAnsi = new char[iLen+1];
	//memset(pAnsi, 0, iLen+1);
	//strncpy(pAnsi, ansiString, iLen);
	wchar_t *pUnicode = new wchar_t[iLen];
	MultiByteToWideChar(CP_ACP,	0, ansiString, -1, pUnicode, iLen);
	CStringW strResult;
	strResult.Format(_T("%s"), pUnicode);
	delete []pUnicode;

	return strResult;
}

// the following code should be refactored, if new algorithm is available
CStringA CMyUtility::GetUTF8CStringA(const CString &unicodeString)
{
	CString wString = unicodeString;
	CStringA mbString;

	//USES_CONVERSION;
	//mbString = W2A(wString);
	//int cbLen = strlen(mbString);
	int cbLen = unicodeString.GetLength() * 3;

	char *pStr = new char[cbLen + 1];
	memset(pStr, 0, cbLen + 1);
	
	WideCharToMultiByte(CP_UTF8, 0, wString, -1, pStr, cbLen, NULL, NULL);
	CStringA ret;
	ret.Format("%s", pStr);

	delete [] pStr;
	return ret;
}

wchar_t *CMyUtility::GetUnicodeString(const char *pAnsi, int len) 
{
	int iLen = len + 1;
	wchar_t *pUnicode = new wchar_t[iLen];
	MultiByteToWideChar(CP_ACP,	0, pAnsi, -1, pUnicode, iLen);

	return pUnicode;
}
/*
* get os version.
*/
bool CMyUtility::getOSVersion(OSVerInfo &osver)
{
   bool bRet = true;
   OSVERSIONINFOEX osvi;

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bRet = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
	  {
		bRet = false;
		goto lExit;
	  }
   }

   switch (osvi.dwPlatformId)
   {
      // Test for the Windows NT product family.
      case VER_PLATFORM_WIN32_NT:
		  if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			  osver._osver = VBWIN2003;
		  if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			  osver._osver = VBWINXP;
		  if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			  osver._osver = VBWIN2000;
		  if ( osvi.dwMajorVersion <= 4 )
			  osver._osver = VBWINNT;
		break;

      // Test for the Windows 95 product family.
      case VER_PLATFORM_WIN32_WINDOWS:
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
			 osver._osver = VBWIN95;
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			 osver._osver = VBWIN98;
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			 osver._osver = VBWINME;
         break;
      case VER_PLATFORM_WIN32s:
		 osver._osver = VBWINME;
		 break;			
   }

lExit:
   return bRet; 

}
bool CMyUtility::fileExist(TCHAR *pathname)
{
	bool bRet = true;

	DWORD fa=0;

	fa =::GetFileAttributes(pathname);
	
	if ( (fa == INVALID_FILE_ATTRIBUTES) || 
		 (fa & FILE_ATTRIBUTE_DIRECTORY) )
	{
		bRet =false;
		goto lExit;
	}

lExit:
   return bRet;
}
bool CMyUtility::directoryExist(TCHAR *pathname)
{
	bool bRet = true;

	DWORD fa=0;

	fa =::GetFileAttributes(pathname);
	if ( (fa == INVALID_FILE_ATTRIBUTES) ||  
		 !(fa & FILE_ATTRIBUTE_DIRECTORY) )
	{
		bRet =false;
		goto lExit;
	}

lExit:
   return bRet;
}

CString CMyUtility::GetMAC()
{
	//return _T("mac");
	IP_ADAPTER_INFO ai;
	DWORD dwBufLen = sizeof(ai);
	if( ERROR_SUCCESS != GetAdaptersInfo(&ai,&dwBufLen) )
		return _T("MAC");
	CString mac;
	mac.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), ai.Address[0],ai.Address[1],ai.Address[2],ai.Address[3],ai.Address[4],ai.Address[5]);
	return mac;
}

CString CMyUtility::GetIP()
{
	char name[MAX_PATH] = {0};
	if( SOCKET_ERROR == gethostname(name,MAX_PATH) )
		return _T("IP");
	hostent *host = gethostbyname(name);
	if( NULL == host )
		return _T("IP");

	strcpy(name,inet_ntoa(*(IN_ADDR*)host->h_addr_list[0]) );
	
	return GetCStringW(name);

/*	IP_ADAPTER_INFO ai;
	DWORD dwBufLen = sizeof(ai);
	GetAdaptersInfo(&ai,&dwBufLen);
	CString IP;
	IP = ai.IpAddressList.IpAddress.String;
	return IP;*/
}

CString CMyUtility::GetMachineName()
{
	char name[MAX_PATH];
	if( SOCKET_ERROR == gethostname(name,MAX_PATH) )
		return _T("machinename");

	return GetCStringW(name);
/*
	return _T("machinename");
	TCHAR name[MAX_COMPUTERNAME_LENGTH+1];
	DWORD lenName = MAX_COMPUTERNAME_LENGTH+1;
	GetComputerName(name,&lenName);
	CString hostName;
	hostName = name;*/
	/*FIXED_INFO *pFi = (FIXED_INFO*)new BYTE[sizeof(FIXED_INFO)];
	DWORD len = sizeof(FIXED_INFO);
	if( ERROR_BUFFER_OVERFLOW == GetNetworkParams(pFi,&len) )
	{
		delete [] pFi;
		pFi = (FIXED_INFO*)new BYTE[len];
		GetNetworkParams(pFi,&len);
	}
	CString hostName;
	hostName = pFi->HostName;
	delete [] pFi;*/
//	return hostName;
}

CString CMyUtility::GetDomainName()
{
	char name[MAX_PATH] = {0};
	if( SOCKET_ERROR == gethostname(name,MAX_PATH) )
		return _T("IP");
	hostent *host = gethostbyname(name);
	if( NULL == host )
		return _T("IP");

	strcpy(name,host->h_name );

	return GetCStringW(name);
	/*
	return _T("domainname");
	CString domanname;
	TCHAR name[MAX_PATH];
	DWORD lenName = MAX_PATH;
	GetComputerNameEx(ComputerNameDnsDomain,name,&lenName);*/
// 	GetComputerNameEx(ComputerNameDnsDomain,name,&lenName);
// 	GetComputerNameEx(ComputerNameDnsFullyQualified,name,&lenName);
// 	GetComputerNameEx(ComputerNamePhysicalNetBIOS,name,&lenName);
// 	GetComputerNameEx(ComputerNamePhysicalDnsHostname,name,&lenName);
// 	GetComputerNameEx(ComputerNamePhysicalDnsDomain,name,&lenName);
// 	GetComputerNameEx(ComputerNamePhysicalDnsFullyQualified,name,&lenName);
// 	GetComputerNameEx(ComputerNameMax,name,&lenName);
// 	domanname = name;
// 	return domanname;
}

CString CMyUtility::GetOSStringVersion()
{
	OSVerInfo osver;
	getOSVersion(osver);

	CString name;
	switch(osver._osver)
	{
	case VBWIN32:
		name = _T("VBWIN32");
		break;
	case VBWIN95:
		name = _T("VBWIN95");
		break;
	case VBWIN98:
		name = _T("VBWIN98");
		break;
	case VBWINME:
		name = _T("VBWINME");
		break;
	case VBWINNT:
		name = _T("VBWINNT");
		break;
	case VBWIN2000:
		name = _T("VBWIN2000");
		break;
	case VBWINXP:
		name = _T("VBWINXP");
		break;
	case VBWIN2003:
		name = _T("VBWIN2003");
		break;
	case VBWINVIST:
		name = _T("VBWINVIST");
		break;
	default:
		name = _T("Unknown");
		break;
	}
	return name;
}