#include "StdAfx.h"
#include "AutoUpdate.h"
#include ".\utility\XmlNodeWrapper.h"
#include ".\utility\VBFile.h"
#include ".\utility\FileVersion.h"
#include "utility/md5.h"
#include "MyUtility.h"
#include "XAutoUPdate.h"
#include ".\utility\Preferences.h"

#include "AuDlg.h"

#include "VBUpdater.h"
//#include "LogMsg.h"

//#include "../ViewBar/VBdefine.h"

// service name
#define   CHECKSERVICE		"viewbarCheckService"
#define   PERSERVICE		"performanceService"
#define   UPDATESERVICE		"autoUpdateService"
#define   DOWNLOADSERVICE	"downloadCountService"

using namespace utility;
using namespace viewbar;

//CLogMessage g_Log;

CAutoUpdate::CAutoUpdate(void) : m_despathname(_T("")), m_hWnd(NULL)
{
	m_nDownCount = 0;
	//g_Log.Open(_T("Updater.log"));
}

CAutoUpdate::CAutoUpdate(CString desPathname,HWND hwnd) : m_despathname(desPathname), m_hWnd(hwnd)
{
	m_nDownCount = 0;
}

CAutoUpdate::~CAutoUpdate(void)
{
}

BOOL CAutoUpdate::AutoInstall()
{
	//if manual.
// 	if(!IsAutoInstall())
// 	{
// 		SendNotice(WM_AUTOINSTALL,(LPARAM)FALSE);
// 		return FALSE;
// 	}
	CString desstr = CMyUtility::GetCurDir();
	SendNotice(WM_BEGININSTALL,(LPARAM)m_nDownCount);

	//AddLog(_T("Start install..."));
	installingAndLogging(m_despathname, desstr, desstr);

	//AddLog(_T("End install..."));
	//SendNotice(WM_INSTALLFINISHED,(LPARAM)m_nDownCount);
	return TRUE;
}

BOOL CAutoUpdate::AutoDownLoad()
{
	int filecount=0;

	SendNotice(WM_CHECKSTATUS,(LPARAM)TRUE);

	CString strLatestVer;
	if(CheckStatus(strLatestVer))	// start to download.
	{
		CString flbuf;

		SendNotice(WM_CHECKPERFORMANCE,(LPARAM)TRUE);
		if (!CheckPerformance())
		{
			SendNotice(WM_CHECKPERFORMANCE,(LPARAM)FALSE);
			return FALSE;
		}

		if( !GetFileList(strLatestVer,m_despathname, flbuf, filecount) )
			return FALSE;

		//begin download.
		//download,install

		//if manual.
// 		if(!IsAutoDownload())
// 		{
// 			SendNotice(WM_AUTODOWNLOAD,(LPARAM)FALSE);
// 			return TRUE;
// 		}

		SendNotice(WM_BEGINDOWNLOAD,(LPARAM)filecount);
		sendDownloadStatus(true,strLatestVer);

 		BOOL bDownloadOkay = downloadAndLogging(flbuf.GetBuffer(0), 
 						m_despathname.GetBuffer(0), 
 						m_despathname.GetBuffer(0),
 						m_nDownCount );

		if (!bDownloadOkay)
		{
			// notify Viewbar.exe to quit
			::PostMessage(HWND_BROADCAST, g_pMainDlg->m_ProcessMsg, GetCurrentProcessId(), _VB_MSG_DOWNLOAD_FAILURE);

			// remove update dir
			RemoveUpdateDir();

			// quit Updater.exe
			::PostMessage(g_pMainDlg->GetSafeHwnd(), WM_QUIT, 0, 0);

			return FALSE;
		}

		//end download.
		sendDownloadStatus(false,strLatestVer);

		// if parameter == 0.0 then send WM_DOWNLOADFINISHED
		// 这里实际上是下载完毕
		if (theApp.m_bAutoDown == 0)
		{
			SendNotice(WM_INSTALLFINISHED,(LPARAM)0);
		}
		
/*
		//if manual.
		BOOL install;
		if(!IsAutoInstall(install))
		{
			// if user answer "no", goto lExit;
			if(!install)
				goto lExit;
		}
		CString desstr = CMyUtility::GetCurDir();
		int downloadcount = 0;
		SendNotice(WM_BEGININSTALL,(LPARAM)downloadcount);

		installingAndLogging(m_despathname, desstr, desstr);

		SendNotice(WM_INSTALLFINISHED,(LPARAM)downloadcount);*/
	}
	else
	{
		SendNotice(WM_CHECKSTATUS,(LPARAM)FALSE);
	}

	return TRUE;
}

BOOL CAutoUpdate::IsAutoDownload()
{
	AddLog(_T("AutoDown: "),theApp.m_bAutoDown);
	return theApp.m_bAutoDown;
}

BOOL CAutoUpdate::IsAutoInstall()
{
	AddLog(_T("AutoInstall: "),theApp.m_bAutoDown);
	return theApp.m_bAutoInstall;
}

BOOL CAutoUpdate::CheckStatus(CString &latestVer)	//check whether current verson expired and get latest version.
{
	CString strUpdateVersion;
	GetUpdateVersion(strUpdateVersion);
	AddLog(_T("Version:")+strUpdateVersion);

	CString xmlin, xmlout;
	xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>   <viewbarReq viewbarId=\"") + strUpdateVersion + _T("\" />");
	CXAutoUPdate xauUpdate;
	xauUpdate.fetchRequest(_T(CHECKSERVICE), xmlin, xmlout); //CHECKSERVICE,

	/* 
	*  <?xml version="1.0" encoding="UTF-8" ?> 
	*  <latestViewbar result="7836485" latestViewbarId="w98.1.1.0.1" forceUpdate="false" /> 
	*/

	CXmlDocumentWrapper docXml;
	docXml.LoadXML(xmlout);

	CXmlNodeWrapper node;
	node = docXml.AsNode();

	CString result = node.GetValue(_T("result"));
	latestVer = node.GetValue(_T("latestViewbarId"));
	CString forceUpdate = node.GetValue(_T("forceUpdate"));

	if (result != _VB_SUCCESS)
		return FALSE;
	if ( (forceUpdate.CompareNoCase(_T("false"))==0) && strUpdateVersion==latestVer)
		return FALSE;
	return TRUE;
}
/*
*01: more than
*02: less than
*03: not more than
*04: not less than
*05: equal
*/
BOOL CAutoUpdate::AopB(CString A, CString op, CString B)
{
	BOOL bRet = TRUE;
	long a,b,iop;

	a  = _ttol(A.GetBuffer(0));
	b  = _ttol(B.GetBuffer(0));
	iop = _ttol(op.GetBuffer(0));
	
	switch(iop)
	{
	case 1:
		if (a<=b)
			bRet = FALSE;break;
	case 2:
		if (a>=b)
			bRet = FALSE;break;
	case 3:
		if (a>b)
			bRet = FALSE;break;
	case 4:
		if (a<b)
			bRet = FALSE;break;
	case 5:
		if (a!=b)
			bRet = FALSE;break;
	}

	return bRet;
}

/*
*<array-list>
*	<VBPerformanceRes name="cpu" value="wwww" currentValue="aa"
*		operator="sss" />
*	<VBPerformanceRes name="mem" value="wwww" currentValue="aa"
*		operator="sss" />
*	<VBPerformanceRes name="EE" value="wwww" currentValue="aa"
*		operator="sss" />
*</array-list>

*/

BOOL CAutoUpdate::CheckPerformance()
{
	CString xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");

	CString xmlout;
	CXAutoUPdate xauUpdate;
	xauUpdate.fetchRequest(_T(PERSERVICE), xmlin, xmlout);
	AddLog(_T("Chk Performance: ")+xmlout);
/*
<?xml version="1.0" encoding="UTF-8"?>
<vbPerformanceListRes result="4444" nextTime="38838383">
	<performance name="EE" value="wwww" currentValue="aa" operator="sss" />
</vbPerformanceListRes>
*/
	CXmlDocumentWrapper doc;
	doc.LoadXML(xmlout);

	CXmlNodeWrapper root, node;
	root = doc.AsNode();
	CString result = root.GetValue(_T("result"));  //success: 100000.

	if ( result.CompareNoCase(_VB_SUCCESS) != 0 )
		return FALSE;

	CString nexttime = root.GetValue(_T("nextTime"));

	node = root.GetNode(0);
	CString	aval,cval,oper;
	aval = node.GetValue(_T("value"));
	cval = node.GetValue(_T("currentValue"));
	oper = node.GetValue(_T("operator"));

	if(!AopB(cval, oper, aval))
		return FALSE;

	int i = 1;
	while(root.GetNode(i++))
	{
		node = root.GetNode(i);

		aval = node.GetValue(_T("value"));
		cval = node.GetValue(_T("currentValue"));
		oper = node.GetValue(_T("operator"));

		if(!AopB(cval, oper, aval))
			return FALSE;
	}

	AddLog(_T("Check Performance"));
	return TRUE;
}

BOOL CAutoUpdate::GetUpdateVersion(CString &out)
{
	AddLog(_T("GetVersion: ")+thePrefs.GetVersion());
	out = thePrefs.GetVersion();
	return TRUE;
}


/*
this function get the following xml data

math operator:

01: more than
02: less than
03: not more than
04: not less than
05: equal

<processor operation="01">
	<c>60%</c>	<-- current value -->
	<v>70%</v>	<-- compare value-->
</processor>

*/
bool CAutoUpdate::canDownload(TCHAR *buf, TCHAR *filelistAddr)
{
	bool bRet = true;
	CXmlDocumentWrapper		 xmldoc;
	CXmlNodeWrapper			 nodeWP;
	CXmlNodeWrapper			 nodeWPT;

	long listLength=0;
	CString name,c,v,oper;

	xmldoc.LoadXML(buf);
	nodeWP = xmldoc.AsNode();

	oper = nodeWP.GetValue(_T("operation"));
	c=v=_T("");
	nodeWPT = nodeWP.GetNode(_T("c"));
	c = nodeWPT.GetText();
	nodeWPT = nodeWP.GetNode(_T("v"));
	v = nodeWPT.GetText();

	long ic,iv;
	long ioper;

	ioper = _ttol(oper);
	ic	  = _ttol(c);
	iv    = _ttol(v);
	switch( ioper )
	{
		case 1:	//c>v
			if (!(ic>iv))  { bRet = false; goto lExit; }
			break;
		case 2: //c<v
			if (!(ic<iv))  { bRet = false; goto lExit; }
			break;
		case 3: //c<=v
			if (!(ic<=iv)) { bRet = false; goto lExit; }
			break;
		case 4: //c>=v
			if (!(ic>=iv)) { bRet = false; goto lExit; }
			break;
		case 5: //c==v
			if (!(ic==iv)) { bRet = false; goto lExit; }
			break;
		default:
			break;
	}
	while (nodeWP.GetNextSibling())
	{
		nodeWP = nodeWP.GetNextSibling();
		oper=nodeWP.GetValue(_T("operation"));
		c=v=_T("");
		nodeWPT = nodeWP.GetNode(_T("c"));
		c = nodeWPT.GetText();
		nodeWPT = nodeWP.GetNode(_T("v"));
		v = nodeWPT.GetText();

		ioper = _ttol(oper);
		ic = _ttol(c);
		iv = _ttol(v);
		switch( ioper )
		{
		case 1:	//c>v
			if (!(ic>iv) ) { bRet = false; goto lExit; }
			break;
		case 2: //c<v
			if (!(ic<iv))  { bRet = false; goto lExit; }
			break;
		case 3: //c<=v
			if (!(ic<=iv)) { bRet = false; goto lExit; }
			break;
		case 4: //c>=v
			if (!(ic>=iv)) { bRet = false; goto lExit; }
			break;
		case 5: //c==v
			if (!(ic==iv)) { bRet = false; goto lExit; }
			break;
		default:
			break;
		}
	}

lExit:
	return bRet;
}

BOOL CAutoUpdate::GetFileList(const CString latestVer,CString &des, CString &outFilelistBuf, int &filecout)
{
	AddLog(_T("GetFileList() LatestVer:")+latestVer);
	AddLog(_T("GetFileList() Des Path:")+des);
	CString		xmlin=_T(""), xmlout, result;

	CString despathname = des + FILELISTNAME;

	// check the des path
	DWORD ret = ::GetFileAttributes(des);

	//not found the dir <des>, to create it.
	if ( (ret & INVALID_FILE_ATTRIBUTES) || 
		!(ret & FILE_ATTRIBUTE_DIRECTORY) )
	{
		::CreateDirectory(des, NULL);
	}

	// get filelist from service and write to <despathname>.
	xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?> <viewbarReq viewbarId=\"")+latestVer+ _T("\" /> ");
	AddLog(_T("GetFileList Request"));
	CXAutoUPdate au;
	au.fetchRequest(_T(UPDATESERVICE), xmlin, xmlout);
	AddLog(_T("GetFileList Request return: ")+xmlout);

/*
<?xml version="1.0" encoding="UTF-8"?>
<VBUpdateFilesRes viewbarId="w98.1.1.0.1" result="4456666"	rootPath="http://www.agloco.com">
	<file name="/viewbar/skin/green/test1.jpg"	path="/download/win98/skin/test1.jpg" md5="ddd" version="" />
</VBUpdateFilesRes>
*/
	CXmlDocumentWrapper		 xmldoc;
	CXmlNodeWrapper			 root;
	outFilelistBuf = xmlout;
	xmldoc.LoadXML(xmlout);
	
	root = xmldoc.AsNode();

	result = root.GetValue(_T("result"));
	
	if ( result.CompareNoCase(_VB_SUCCESS)!=0 )
	{
		return FALSE;
	}

	filecout = root.NumNodes();
	xmldoc.Save(despathname);
	CXmlNodeWrapper node;
	for( int i = 0; i < filecout; i++ )
	{
		node = root.GetNode(i);
		CString name = node.GetValue(_T("name"));
		AuCreateDirectory(des,name);
	}
	return TRUE;
}

bool CAutoUpdate::sendDownloadStatus(bool bStartOrEnd, const CString ver)
{
	/*  <?xml version="1.0" encoding="UTF-8" ?> 
	**  <downloadReq viewbarId="w98.1.1.0.1" operate="autoUpdateStart" /> 
	*/
	CString xmlin,xmlout;
	CXAutoUPdate		au;

	if (bStartOrEnd)
		xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?> <downloadReq viewbarId=\"")+ ver +_T("\" operate=\"autoUpdateStart\" />");
	else
		xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?> <downloadReq viewbarId=\"")+ ver +_T("\" operate=\"autoUpdateEnd\" />");

	au.fetchRequest(_T(DOWNLOADSERVICE), xmlin, xmlout);

	return true;

}

#define VBVER		_T("viewbarVer")
#define ROOTPATH	_T("rootPath")
#define FILELIST	_T("fileList")
#define FILE		_T("file")
#define FILENAME	_T("fileName")
#define FILEURL		_T("fileUrl")
#define FILEVERSION _T("fileVersion")
#define MD5			_T("md5")

/*
* this function must be a long transaction.
*/
BOOL CAutoUpdate::downloadAndLogging(TCHAR *flBuf,	
							TCHAR *despath,
							TCHAR *logpath,
							int &downloadcount)
{
	CXmlDocumentWrapper		 xmldoc;
	CXmlNodeWrapper			 root;
	CXmlNodeWrapper			 node;
	CString pver,roothttp;	// rootpath: http root path.

	AddLog(_T("downloadAndLogging().."));

	xmldoc.LoadXML(flBuf);
	root = xmldoc.AsNode();
	
	roothttp = root.GetValue(_T("rootPath"));

	char filehash[50]={""};
	WCHAR wfilehash[50]={L""};
/*	filelist:
*	---------
*	<VBUpdateFilesRes  viewbarId="win32.1.02" result="100000" rootPath="http://119.119.115.35:8080">
*		<file name="/msxml4.dll" path="/win32.1.02/msxml4.dll" md5="44e45bd9327abc0540593e809b32f3ca"/>
*	</VBUpdateFilesRes >
*/
	int i = 0;
	downloadcount = 0;
	for(;root.GetNode(i); )
	{
		CString name, url, md5;

		node = root.GetNode(i++);
		name = node.GetValue(_T("name"));
		url = node.GetValue(_T("path"));
		md5  = node.GetValue(_T("md5"));

		//check file current version whether is latest.
		name.Replace('/','\\');
		CString pathName = CMyUtility::GetCurDir()+name;
		
		if( !getMD5(pathName, md5) )
			continue;
		
		CString httpFile = roothttp + url;
		CString strDesPath = despath + name;

		downloadcount++;
		SendNotice(WM_DOWNLOADONE,downloadcount);
		if ( isOpened(strDesPath) )
			continue;
		CVBFile vbFile;
		BOOL bDownloadOkay = vbFile.GetFile(despath,name,httpFile);

		if (!bDownloadOkay)
		{
			// write file name to "update.log"
			SaveErrorFile(name);

			return FALSE;
		}
	}
	AddLog(_T("downloadAndLogging() OK"));

	return TRUE;
}

void CAutoUpdate::SaveErrorFile(const CString &strFileName)
{
	CFile file;
	BOOL bOpen = FALSE;

	try
	{
		bOpen = file.Open(_T("./update.log"), CFile::modeCreate | CFile::modeWrite);
		file.Write(strFileName, (strFileName.GetLength() + 1) * sizeof( TCHAR ));
	}
	catch (CException* e)
	{
		if (bOpen)
		{
			file.Close();
		}

		e->Delete();
	}

	if (bOpen)
	{
		file.Close();
	}
}

/*
*	check the status of files in filelist.
*	1. files must exist in <path>, which is in filelist and 
*		it's version is higher than current version.
*/
bool CAutoUpdate::checkOk(TCHAR *path)
{
	bool bRet = true;
	CXmlDocumentWrapper xmldoc;
	CXmlNodeWrapper		node;
	CXmlNodeWrapper		nodeT;
	CFile file;
	WCHAR *wbuf=NULL;
	char *mbbuf=NULL;
	CString filelist;
	int i=0;
	viewbar::CMyUtility myut;
	char hashKey[40]={""};
	CString filename,fileurl,fileversion,md5,binpath,filepath;

	// check if file filelist exists in <path>
	filelist += path ;
	filelist += FILELISTNAME ;
	DWORD ret = ::GetFileAttributes(filelist.GetBuffer());
	if( (ret & INVALID_FILE_ATTRIBUTES) ||
		(ret & FILE_ATTRIBUTE_DIRECTORY) )
	{
		bRet = false;
		goto lExit;
	}

	file.Open(filelist.GetBuffer(), CFile::modeRead);
	int fl=file.GetLength();
	mbbuf= new char[fl+1];
	::ZeroMemory(mbbuf, fl+1);
	file.Read(mbbuf, (unsigned int)fl);
	file.Close();

#ifndef _UNICODE 
	xmldoc.LoadXML(mbbuf);
#else
	::ZeroMemory(wbuf, sizeof(WCHAR)*fl+1);
	wbuf = new WCHAR[fl+1];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbbuf, (int)fl, wbuf, (int)fl);
	xmldoc.LoadXML(wbuf);
#endif
	/*
	*	<vbver>xx.mm.dd.yyyy</vbver>  <-- xx.mm.dd.yyyy -->
	*	<rootpath>http://www.agloco.com/windows/</rootpath>
	*	<filelist>
	*		<file filename="" fileurl="" fileverson="" md5="" ></file>
	*	</fileList>
	*/

	node = xmldoc.AsNode();
	node = node.GetNextSibling();
	node = node.GetNextSibling(); // here, get to <filelist> node

	i=0;
	binpath = myut.GetCurDir();
	for(;node.GetNode(i);i++)
	{
		nodeT = node.GetNode(i);
		filename	= nodeT.GetValue(_T("filename"));		// type.
		fileurl		= nodeT.GetValue(_T("fileurl"));		// destination path.
		fileversion	= nodeT.GetValue(_T("fileversion"));		// version.
		md5			= nodeT.GetValue(_T("md5"));		// url.
		
		// check if file <filepath> exists, and if <its hasdid>==<md5>
		filepath = path + filename;
		memset(hashKey, 0, 40);
		getMD5(filepath, md5);

#ifdef _UNICODE
		// hashKey to unicode.
		WCHAR whashKey[40];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, hashKey, 40, whashKey, 40);
		if (_tcscmp(whashKey, md5) !=0 )
		{
			bRet = false;
			break;
		}
#else
		if(strcmp(md5.GetBuffer(), hashKey)!=0)	// current file isn't available.
		{
			bRet = false;
			break;
		}
#endif
	}
lExit:
	delete mbbuf;
	mbbuf=NULL;
	delete wbuf;
	wbuf=NULL;

	return bRet;
}

/*
*	here, you should have called "checkOK" to check the all file status.
*   and every file be available 
*/

BOOL CAutoUpdate::installingAndLogging( CString srcPath, CString desPath, CString logPath)
{
	AddLog(_T("installingAndLogging()...."));
	CString fileList = srcPath + FILELISTNAME;

	AddLog(_T("installingAndLogging ") + fileList);
	CFile fileXml;
	CString xmlContent;
	if( fileXml.Open(fileList,CFile::modeRead) )
	{
		int nLenFileXml = fileXml.GetLength();
		char* buf = new char[nLenFileXml+1];
		fileXml.Read(buf,nLenFileXml);
		buf[nLenFileXml] = '\0';
		fileXml.Close();
		xmlContent = buf;
		delete [] buf;
	}
	else
	{
		AddLog(_T("Open file FAILED ")+fileList);
		return FALSE;
	}

	CXmlDocumentWrapper xmlDoc;
	if( !xmlDoc.LoadXML(xmlContent) )
	{
		AddLog(_T("FAILED LoadXML ") + xmlContent);
		return FALSE;
	}

	CXmlNodeWrapper node = xmlDoc.AsNode();
	
	CString nameFile,pathFile,md5; // read from xml
	CXmlNodeWrapper nodeT;
	int installcount = 0;
	for( int i = 0; node.GetNode(i); i++)
	{
		nodeT = node.GetNode(i);
		
		nameFile	= nodeT.GetValue(_T("name"));
		pathFile	= nodeT.GetValue(_T("path"));
		md5			= nodeT.GetValue(_T("md5"));

		nameFile.Replace('/','\\');
		CString srcFile = srcPath + nameFile;

		AddLog(_T("SourceFile ")+srcFile);
		if( -1 == srcFile.Find(_T("Updater.exe")) )
		//if(!CMyUtility::fileExist(srcFile.GetBuffer()))
		{
			// begin to install.
			if( !isOpened(srcFile.GetBuffer()) )
			//{}//here, wait until user to close the program.
				AuCopyFile(srcPath+nameFile, desPath, nameFile);
		}
		else
		{
			RemoveSelf(srcPath+nameFile,desPath,nameFile);
		}
		installcount++;
		SendNotice(WM_INSTALLONE,installcount);
	}

	AddLog(_T("installingAndLogging() END"));

	// remove update dir
	RemoveUpdateDir();

	return TRUE;
}
bool CAutoUpdate::installingAndLogging(TCHAR *srcPath,
									   TCHAR *desPath,
									   TCHAR *logPath)  // install also be long transaction.	
{
	bool bRet = true;
	CString flpath;
	CString srcfilepath;
	CString desfilepath;
	CFile file;
	CXmlDocumentWrapper xmldoc;
	CXmlNodeWrapper		node;
	CXmlNodeWrapper		nodeT;
	char  *buf=NULL;
	WCHAR *wbuf=NULL;
	int fLen,i=0;
	int installcount=0;

	try
	{
		flpath += srcPath;
		flpath += FILELISTNAME;

		file.Open(flpath, CFile::modeRead);
		fLen = file.GetLength();
		buf = new char[fLen+1];
		ZeroMemory(buf, fLen+1);
		file.Read(buf, fLen);
		file.Close();
#ifdef _UNICODE
		wbuf = new WCHAR[fLen+1];
		ZeroMemory(wbuf, fLen+1);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf, fLen+1, wbuf, fLen+1);
		xmldoc.LoadXML(wbuf);
#else
		xmldoc.LoadXML(buf);
#endif
		/*	filelist content:
		<?xml version="1.0" encoding="UTF-8"?>
		<VBUpdateFilesRes viewbarId="win32.1.02" result="100000" rootPath="http://119.119.115.35:8080/download">
		<file name="msxml4.dll" path="\win32.1.02\msxml4.dll" md5="44e45bd9327abc0540593e809b32f3ca"/>
		</VBUpdateFilesRes>
		*/	

		node = xmldoc.AsNode();

		i=0;
		CString name,path,md5;

		// still need to do many check.
		for(;node.GetNode(i);)
		{
			nodeT = node.GetNode(i++);

			name	= nodeT.GetValue(_T("name"));
			path	= nodeT.GetValue(_T("path"));
			md5		= nodeT.GetValue(_T("md5"));

			TCHAR * filename = (TCHAR *)_tcsrchr(path, _T('/'));
			srcfilepath = srcPath;
			srcfilepath += filename;
			desfilepath = desPath;
			desfilepath += name;

			if(CMyUtility::fileExist(srcfilepath.GetBuffer()))
			{
				// begin to install.
				if( isOpened(desfilepath.GetBuffer()) )
				{}//here, wait until user to close the program.

				CopyFile(srcfilepath.GetBuffer(), desfilepath.GetBuffer(), false);
			}
		installcount++;
		SendNotice(WM_INSTALLONE,installcount);
		}

		delete buf;
		buf  = NULL;
		delete wbuf;
		wbuf = NULL;
	}
	catch(...)
	{
		delete buf;
		buf  = NULL;
		delete wbuf;
		wbuf = NULL;
	}
	return bRet;
}
/*
* false 0: ver >cver;
* true  1: ver<=cver;
*/
bool CAutoUpdate::compareVer(TCHAR *cver, TCHAR *ver)
{
	bool bRet = true;

	int cv,cd,cm,cy;	// current vresion.
	int v,d,m,y;		// lastest version.

	_stscanf(cver, _T("%d"), &cv);
	_stscanf(cver, _T("%d"), &cd);
	_stscanf(cver, _T("%d"), &cm);
	_stscanf(cver, _T("%d"), &cy);

	_stscanf(ver, _T("%d"), &v);
	_stscanf(ver, _T("%d"), &d);
	_stscanf(ver, _T("%d"), &m);
	_stscanf(ver, _T("%d"), &y);

	if (cv>v)
	{
		goto lExit;
	}
	else if (cv == v)
		if (cd>d)
		{
			goto lExit;
		}
		else if(cd == d)
			if (cm>m)
			{
				goto lExit;
			}
			else if(cm==m)
				if(cy>=y)
				{
					goto lExit;
				}

	bRet = false;

lExit:
	return bRet;
}

BOOL CAutoUpdate::getMD5(CString filepath, CString hashKey)
{
	CFile file;
	if( !file.Open(filepath,CFile::modeRead) )
	{
		return TRUE;
	}
	int nLen = file.GetLength();
	char *buf = NULL;
	buf = new char[nLen+1];
	memset(buf,0,nLen+1);
	file.Read(buf,nLen);
	file.Close();

	CStringA filePathMB = CMyUtility::GetCStringA(filepath);
	CStringA strHash = MD5Hash(filePathMB);
	
//	CMD5 md5;
//	md5.setPlainText(buf);
	delete [] buf;
	CString newHash;

	newHash = CMyUtility::GetCStringW(strHash);

//	newHash.Format(_T("%s"), md5.getMD5Digest());
	return (0 != newHash.CompareNoCase(hashKey));
}

BOOL CAutoUpdate::isOpened(CString filepathname)
{
	if( !CMyUtility::fileExist(filepathname.GetBuffer()) )
	{
		return FALSE;
	}

	OFSTRUCT ofStruct;
	memset(&ofStruct, 0, sizeof(ofStruct));
	ofStruct.cBytes = sizeof(LPOFSTRUCT);

#ifdef UNICODE
	char chFile[MAX_PATH];
	int nPos = WideCharToMultiByte(CP_ACP,0,filepathname.GetBuffer(),filepathname.GetLength(),chFile,MAX_PATH,NULL,NULL);
	chFile[nPos] = '\0';
#else
	char *chFile = filepathname.GetBuffer();
#endif

	HFILE fret = ::OpenFile(chFile,&ofStruct,OF_SHARE_EXCLUSIVE);
	if(HFILE_ERROR != fret)
	{
		::CloseHandle((HANDLE)fret);
		return FALSE;
	}
	return TRUE;
}


void CAutoUpdate::SendNotice(UINT notice, LPARAM lParam)
{
	if( IsWindow(m_hWnd) )
		::PostMessage(m_hWnd,notice,(WPARAM)this, lParam);
}

void CAutoUpdate::AuCreateDirectory(const CString &des,const CString &nameFile)
{
	CString strDesPath = des;
	CString name = nameFile;
	int nPos = des.ReverseFind('\\');
	if( nPos == des.GetLength()-1 )
		strDesPath = des.Left(nPos);

	name.Replace('/','\\');
	int nNumber = 0;
	nPos = 1;
	while(-1 != (nPos = name.Find('\\',nPos)))
	{
		nPos++;
		nNumber++;
	}

	for( int i = 0; i < nNumber; i++ )
	{
		CString strDir;
		nPos = 1;
		for( int j = 0; j <= i; j++)
		{
			nPos = name.Find('\\',nPos);
			nPos++;
		}
		strDir = strDesPath + name.Left(nPos-1);
		GetFileAttributes(strDir);
		if( GetLastError() == ERROR_FILE_NOT_FOUND )
		{
			if( !CreateDirectory(strDir,NULL) )
			{
				TRACE("%d",GetLastError());
			}
		}
		AddLog(_T("CreateDir ")+strDir);
		TRACE("%s\n",strDir);
	}
}

void CAutoUpdate::AuCopyFile(const CString &srcFullPath, const CString &desPath, const CString &desFile)
{
	AuCreateDirectory(desPath,desFile);

	AddLog(_T("copyfile to ") + desPath+desFile);
	CString desFull = desPath+desFile;
	if( !CMyUtility::fileExist(desFull.GetBuffer()))
	{
		CopyFile(srcFullPath, desPath+desFile, FALSE);
		return;
	}
	DWORD Attribute = GetFileAttributes(desPath+desFile);

	if( Attribute & FILE_ATTRIBUTE_READONLY )
		SetFileAttributes(desPath+desFile,FILE_ATTRIBUTE_NORMAL);

	CopyFile(srcFullPath, desPath+desFile, FALSE);
	SetFileAttributes(desPath+desFile,Attribute);
}

void CAutoUpdate::SetParameters(const CString &desPathname,HWND hwnd)
{
	AddLog(_T("SetParameters(DesPath) ")+desPathname);
	m_hWnd = hwnd;
	m_despathname = desPathname;
}

void CAutoUpdate::AddLog(CString infor,int i)
{/*
	g_Log.WriteLog(infor+_T(": "));
	g_Log.WriteLog(i);
	g_Log.WriteLog(_T(".........."));
	g_Log.WriteLog(GetTickCount());
	g_Log.NewLine();*/
}
void CAutoUpdate::AddLog(CString str)
{/*
	g_Log.WriteLog(str);
	g_Log.WriteLog(_T(".........."));
	g_Log.WriteLog(GetTickCount());
	g_Log.NewLine();*/
}

BOOL CAutoUpdate::InstallEx()
{
	CString desstr = CMyUtility::GetCurDir();
	SendNotice(WM_BEGININSTALL, (LPARAM)m_nDownCount);

	AddLog(_T("Start installing..."));
	installingAndLogging(m_despathname, desstr, desstr);

	AddLog(_T("Installing Done!"));
	SendNotice(WM_INSTALLFINISHED,(LPARAM)m_nDownCount);

	return TRUE;
}

void CAutoUpdate::RemoveSelf(const CString &srcFullPath,
							 const CString &desPath,
							 const CString &desFile)
{
	CString strSrcFullPath;
	CString strDesPath;
	CString strDesFile;

	strSrcFullPath = srcFullPath;
	strDesPath = desPath;
	strDesFile = desFile;

	// look like: the temppath is not correct as input parameter in MoveFileEx();
// 	TCHAR strTempPath[MAX_PATH];
// 	GetTempPath(MAX_PATH,strTempPath);
// 	_tcscat(strTempPath,_T("vbupdate.tmp"));
	CFileFind find;
	if( !find.FindFile(strSrcFullPath) )
	{
		find.Close();
		return;
	}
	find.Close();

	CString strDesFullPath = desPath + desFile;
	strDesFullPath = strDesFullPath.Left(strDesFullPath.ReverseFind('\\'));
	strDesFullPath += "\\temp.tmp";

	DeleteFile(strDesFullPath);

	CFile::Rename(desPath+desFile,strDesFullPath);
	CopyFile(strSrcFullPath,desPath+desFile,FALSE);
	MoveFileEx(strDesFullPath,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);

	
}

void CAutoUpdate::RemoveUpdateDir()
{
	CString path = CMyUtility::GetCurDir();

	CString strUpdate = path + _T("\\update");

	CMyUtility::DeleteDirectory(strUpdate);
}