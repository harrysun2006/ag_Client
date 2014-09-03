
#include "StdAfx.h"
//#include <Winbase.h>

#include "MyUtility.h"
#include <stdlib.h>

CMyUtility::CMyUtility(void)
{
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
	int iLen = unicodeString.GetLength() + 1;
	char *pAnsi = new char[iLen];
	memset(pAnsi, 0, iLen);
	WideCharToMultiByte(
		CP_ACP,
		0,
		unicodeString,
		-1,
		pAnsi,
		iLen,
		NULL,
		NULL);

	return pAnsi;
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


BOOL CMyUtility::DeleteDirectory(LPCTSTR DirName)
{
	CFileFind tempFind;
	TCHAR tempFileFind[MAX_PATH];
	_stprintf(tempFileFind, _T("%s\\*.*"), DirName);

	BOOL IsFinded = (BOOL)tempFind.FindFile(tempFileFind);
	while(IsFinded)
	{
		IsFinded = (BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			TCHAR foundFileName[MAX_PATH];
			_tcscpy(foundFileName,tempFind.GetFileName().GetBuffer(MAX_PATH));
			if(tempFind.IsDirectory())
			{
				TCHAR tempDir[MAX_PATH];
				_stprintf(tempDir, _T("%s\\%s"), DirName, foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				TCHAR tempFileName[MAX_PATH];
				_stprintf(tempFileName, _T("%s\\%s"), DirName, foundFileName);
				DeleteFile(tempFileName);
			}
		}
	}

	tempFind.Close();
	if(!RemoveDirectory(DirName))
	{
		return FALSE;
	}

	return TRUE;
}