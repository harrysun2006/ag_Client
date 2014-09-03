// RegDLL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RegDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		TCHAR FileName[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(), FileName, MAX_PATH);
		CString path;
		path = FileName;
		path = path.Left(path.ReverseFind('\\'));

		CString newPath = _T("/s \"") + path;

		ShellExecute(NULL, _T("open"), _T("regsvr32.exe"), newPath + _T("\\mssoap30.dll\""), NULL, SW_HIDE);
		ShellExecute(NULL, _T("open"), _T("regsvr32.exe"), newPath + _T("\\mssoapR3.dll\""), NULL, SW_HIDE);
		ShellExecute(NULL, _T("open"), _T("regsvr32.exe"), newPath + _T("\\wisc30.dll\""), NULL, SW_HIDE);
		ShellExecute(NULL, _T("open"), _T("regsvr32.exe"), newPath + _T("\\msxml4r.dll\""), NULL, SW_HIDE);
		ShellExecute(NULL, _T("open"), _T("regsvr32.exe"), newPath + _T("\\msxml4.dll\""), NULL, SW_HIDE);
	}

	return nRetCode;
}
