// CAutoUpdate header file AutoUpdate.h

#pragma once

#define PROCESSOR	0	// server processor time percentage.
#define MEMORY		1	// server memory percentage.
#define ONLINEUSERS 2	// the number of the online users
#define DOWNLOAD	3	// the number of user who downloading viewbar or update.
#define FILELISTNAME _T("\\filelist.dat");

#define WM_DOWNLOADFINISHED	WM_USER+2501
#define WM_INSTALLFINISHED	WM_USER+2502

// LPARAM TRUE: start; FALSE: failed
#define	WM_CHECKSTATUS		WM_USER+2503
#define WM_CHECKPERFORMANCE WM_USER+2504

// LPARAM the number of files download
#define WM_BEGINDOWNLOAD	WM_USER+2505
#define WM_DOWNLOADONE		WM_USER+2506
#define WM_BEGININSTALL		WM_USER+2507
#define WM_INSTALLONE		WM_USER+2508

#define WM_AUTODOWNLOAD		WM_USER+2509
#define WM_AUTOINSTALL		WM_USER+2510


class CAutoUpdate
{
public:
	CAutoUpdate(void);
	CAutoUpdate(CString desPathname,HWND hwnd);
	~CAutoUpdate(void);
private:
	void SendNotice(UINT notice, LPARAM lParam);

public:
	BOOL AutoDownLoad();
	BOOL AutoInstall();

	BOOL InstallEx();		// by Charry

	// 1. check whether current verson expired and get latest version.
	BOOL CheckStatus(CString &latestVer);

	// 2. get the parameter of server performance and check the status.
	BOOL CheckPerformance();

	// 3. get the filelist and save in path <des>,named <FILELISTNAME>.
	BOOL GetFileList( const CString latestVer,CString &des, CString &outFilelistBuf, int &filecout);

	// 4. send the beginning and end of download.
	bool sendDownloadStatus(bool bStartOrEnd, const CString ver); // true is start, else end.

	BOOL GetUpdateVersion(CString &out);

	// check server performance.
	bool canDownload(TCHAR *buf, TCHAR *fileListAddr);

	// download should be long transaction. 
	BOOL downloadAndLogging(TCHAR *flBuf,	// filelist buffer.
							TCHAR *despath,
							TCHAR *logpath,
							int &downloadcount);

	// install also be long transaction.	
	bool installingAndLogging(TCHAR *srcPath,
							  TCHAR *desPath,
							  TCHAR *logPath);
	BOOL installingAndLogging(	CString srcPath,
								CString desPath,
								CString logPath);

	// check all file status before installing.
	bool checkOk(TCHAR *path);

	// compare two version.
	bool compareVer(TCHAR *cver, TCHAR *ver);

	// 
	bool compareHashId(TCHAR *pathfile, TCHAR *hashid);

	// get hashid regarding file <filepath>.
	BOOL getMD5(CString filepath, CString hashKey);

	// 
	BOOL isOpened(CString filepathname);

	BOOL IsAutoDownload(); // user answer "yes" if true, "no" else.

	BOOL IsAutoInstall(); // user answer "yes" if true, "no" else.
	void SetParameters(const CString &desPathname,HWND hwnd);


	void RemoveUpdateDir();

private:
	CString m_despathname;
	HWND m_hWnd;

private:
	BOOL AopB(CString A, CString op, CString B);
	void AuCreateDirectory(const CString &des,const CString &nameFile);
	void AuCopyFile(const CString &srcFullPath, const CString &desPath, const CString &desFile);
	int m_nDownCount;
	void AddLog(CString infor,int);
	void AddLog(CString);
	void RemoveSelf(const CString &srcFullPath, const CString &desPath, const CString &desFile);
	void SaveErrorFile(const CString &strFileName);
};