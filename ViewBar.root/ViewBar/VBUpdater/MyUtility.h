// ***************************************************************
//  CMyUtility   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  A utility class
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

enum OsVersion {VBWIN32,VBWIN95,VBWIN98,VBWINME,VBWINNT,VBWIN2000,VBWINXP,VBWIN2003,VBWINVIST};

typedef struct _OSVerInfo{
	OsVersion _osver;	
}OSVerInfo, *POSVerInfo;

namespace viewbar
{
	class CMyUtility
	{
	public:
		CMyUtility(void);
		~CMyUtility(void);

		static CString	GetCurDir();
		static void		GetFileList(CStringArray &langs, const CString &subDir);
		static char*	GetAnsiString(const CString& unicodeString);
		static CStringA	GetCStringA(const CString &unicodeString);
		static CStringW	GetCStringW(const CStringA &ansiString);
		static wchar_t*	GetUnicodeString(const char *pAnsi, int len);

		static bool	getOSVersion(OSVerInfo &osver);
		static bool fileExist(TCHAR *pathname);
		static bool directoryExist(TCHAR *pathname);

		static BOOL DeleteDirectory(LPCTSTR DirName);
	private:
		
	};
}

