/*
* CFavorite implementation file.
* 
* Author: Bruce Shao, bruce_shao@amaxgs.com.
*
* Copyright: AMAX GLOBAL SERVICE.
*/

#include "stdafx.h"

#include <string.h>

#include "shellapi.h"
#include "FavoriteMenu.h"
#include "..\\MyUtility.h"

#define IEFAVORITESPATH	_T("%USERPROFILE%\\Favorites")
#define IEFAVORITESREG	_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Favorites")
#define IsOvenNumber(n) if(((n) % 2) != 0)(n)++

using namespace viewbar;

#define  _CRT_SECURE_NO_WARNINGS 
#if _UNICODE
#define	vbStrCpy	wcscpy
#else
#define vbStrCpy	strcpy
#endif

/*
* total process: 
*	1. get the default browser
*	2. get the position of storing favorites or bookmark.
*		
*	3. get the favorites.
*/

CFavorite::CFavorite(void)
{
	reset();
	//memset( &_bmStack, 0, sizeof(_bmStack) ); 
	_bmStack.pos = -1;
	_bmStack.bot = 0;
}
CFavorite::CFavorite(CTreeCtrl* iTreeCtrl)
{
	reset();
	//memset( &_bmStack, 0, sizeof(_bmStack) ); 
	_bmStack.pos = -1;
	_bmStack.bot = 0;
}

CFavorite::~CFavorite(void)
{
	m_favString.RemoveAll();
}
bool CFavorite::reset()
{
	_bmStack.pos=-1;
	_bmStack.count = 0;
	_bmStack.bot = 0;
	for(int i=0;i<MAXNODE;i++)
	{
		_bmStack.arrNode[i].childNode = NULL;
		_bmStack.arrNode[i].nextNode  = NULL;
		_bmStack.arrNode[i].nt = NONT;
		_bmStack.arrNode[i].title.Append(_T(""));
		_bmStack.arrNode[i].title = "";
		_bmStack.arrNode[i].url.Append(_T(""));
		_bmStack.arrNode[i].url = "";
	}

	return true;
}

bool CFavorite::reversePop(NODE &node)
{
	bool bRet = true;

	// No element.
	if (this->_bmStack.count<=0)
	{
		bRet = false;
		goto lExit;
	}

	node.nt = _bmStack.arrNode[_bmStack.bot].nt;
	if (node.nt == DT)
	{
		node.childNode = _bmStack.arrNode[_bmStack.bot].childNode;
		node.nextNode  = _bmStack.arrNode[_bmStack.bot].nextNode;
		node.title.Append(_T(""));
		node.title	= _bmStack.arrNode[_bmStack.bot].title;
		node.url.Append(_T(""));
		node.url	= _bmStack.arrNode[_bmStack.bot].url;
	}

	this->_bmStack.count--;
	this->_bmStack.bot++;

	if (this->_bmStack.count<0)
	{
		_bmStack.count = 0;
		_bmStack.bot = 0;
	}

lExit:
	return bRet;
}

bool CFavorite::push(NODE &node)
{
	bool bRet = true;

	if (_bmStack.count>=MAXNODE)
	{
		bRet = false;
		goto lExit;
	}

	_bmStack.count++;
	_bmStack.pos++;

	_bmStack.arrNode[_bmStack.pos].nt = node.nt;
	if ( node.nt == DT )
	{
		_bmStack.arrNode[_bmStack.pos].childNode = node.childNode;
		_bmStack.arrNode[_bmStack.pos].nextNode = node.nextNode;
		_bmStack.arrNode[_bmStack.pos].title.Append(_T(""));
		_bmStack.arrNode[_bmStack.pos].title = node.title;
		_bmStack.arrNode[_bmStack.pos].url.Append(_T(""));
		_bmStack.arrNode[_bmStack.pos].url = node.url;
	}

lExit:
	return bRet;
}

bool CFavorite::communicate()
{
	// add your code here.

	return true;
}
/*
*{Foxfire}HKEY_CLASSES_ROOT\http\shell\open\ddeexec\Application ==Firefox
*{IE}HKEY_CLASSES_ROOT\http\shell\open\ddeexec\Application == IExplore
*(Netscape)HKEY_CLASSES_ROOT\http\shell\open\ddeexec\Application == nsshell 
*/
/*
* get current default browser.
*/
 bool CFavorite::getBrowserType(BROWSERTYPE &bType)
{
	bool bRet=true;
	HKEY hKey;
	TCHAR sz[1024]={_T("")};
	
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("http\\shell\\open\\ddeexec\\Application"),
					0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS )
	{
		bRet = false;
		goto lExit;
	}
	DWORD dwSize = 1024;
	if( RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)sz, &dwSize)
				!=ERROR_SUCCESS )
	{
		bRet = false;
		goto lExit;
	}
	RegCloseKey(hKey);
	if (_tcsicmp(sz, _T("IExplore"))==0)
	{
		bType = IEXPLORE; 
	}
	else if (_tcsicmp(sz, _T("Firefox"))==0)
	{
		bType = FIREFOX;
	}
	else if (_tcsicmp(sz, _T("nsshell"))==0)
	{
		bType = NETSCAPE;
	}

lExit:
	return bRet;
}
 /*
 * Get FireFox current prfoile,
 *
 * input:
 *		iniFile:  firefox profile ini file.	
 *	
 * output:
 *		endSection:		the lastest profile section.
 *		defaultSection: current active section.
 *		sCount:			the number of the section in <iniFile> file.
 */
 bool CFavorite::getFFSectionCount(TCHAR *iniFile, TCHAR *endSection, int endLen, TCHAR *defaultSection, int defLen, int &sCount)
 {
	 bool bRet=true;
	 TCHAR tmp[1024]={_T("")};

	 int sectionCount = GetPrivateProfileSectionNames(tmp,1024, iniFile);
	 int j= sectionCount - 2;
	 for(;j>0;j--)
	 {
	     if ( tmp[j]==0 )
		     break;
	 }
	 //_tcscpy(endSection, tmp+j+1);
	 wcscpy_s(endSection, endLen, tmp+j+1);

	 //calculating the section count.
	 sCount = 1;
	 j = sectionCount - 2;
	 for(; j>=0; j--)
	 {
		 TCHAR defValues[10]={_T("")};
		 if ( tmp[j] == 0 )
		 {
			 sCount++;
			 TCHAR key[256] = {_T("Default")};
			 TCHAR def[256] = {_T("0")};
			 GetPrivateProfileString(tmp+j+1, key, def, defValues, 10, iniFile);
			 if (_tcscmp(defValues, _T("1"))==0)
			 {
				 
				 wcscpy_s(defaultSection, defLen, tmp+j+1);
				 //_tcscpy(defaultSection, defValues);

			 }
			 /*not found default section yet, and reached [Profile0] section*/
			 else if ( (_tcscmp(defValues, _T("0"))==0) &&  
						(_tcscmp(tmp+j+1, _T("Profile0"))==0) &&
						(_tcscmp(defaultSection, _T(""))==0) )
			 {
				 wcscpy_s(defaultSection, defLen, _T("Profile0"));
				 //_tcscpy(defaultSection, "Profile0");
			 }
		 }
	 }

	 return bRet;
 }
/*
 * %appdata%\mozilla\Firefox\profiles.ini
 * %USERPROFILE%\Application Data\Mozilla\Firefox\profiles.ini
 */
 bool CFavorite::getFavoriteFromDefaultBrowser(CTreeCtrl* iTreeCtrl)
 {
	 TCHAR ofPath[1024]={_T("")};
	 bool bRet = true;
	 BROWSERTYPE bType;
	 TCHAR sz[1024]={_T("")};

	 if (!this->getBrowserType(bType))
	 {
		 bRet = false;
		 goto lExit;
	 }
	 iTreeCtrl->DeleteAllItems();

	 if (bType == IEXPLORE)
	 {
		 //_tcscpy(sz, _T("%USERPROFILE%\\Favorites"));
		 wcscpy_s(sz, 1024, IEFAVORITESPATH);
		 ExpandEnvironmentStrings(sz, ofPath, 1024);
		 createFavoriteTree(TVI_ROOT, ofPath,iTreeCtrl);
	 }
	 else if (bType == FIREFOX )
	 {
		 TCHAR Profiles[1024]  = {_T("")};
		 TCHAR endSection[128] = {_T("")};
		 TCHAR defaultSection[128] = {_T("")};
		 int sCount=0;

		 ExpandEnvironmentStrings(_T("%appdata%\\mozilla\\Firefox\\"), sz, 1024);
		 wcscat_s(Profiles, 1024, sz);
		 wcscat_s(ofPath, 1024, sz);
		 wcscat_s(Profiles, 1024, _T("profiles.ini"));
		 getFFSectionCount(Profiles, endSection, 128, defaultSection, 128, sCount);
		 /* get the default section profile, that is, defaul=1
		 *
		 * [Profile<n>]
		 * Name=default
		 * IsRelative=1
		 * Path=Profiles/tx61c89i.default
		 * Default=1
		 */
		 TCHAR path[1024]={_T("")};
		 GetPrivateProfileString(defaultSection, _T("Path"), _T(""), path, 1024, Profiles);
		 TCHAR * slashPos=_tcschr(path, _T('/'));
		 if (slashPos)
		 {
			 slashPos[0]='\\';
		 }
		 wcscat_s(ofPath, 1024, path);
		 wcscat_s(ofPath, 1024, _T("\\bookmarks.html"));
		 createFireFoxTree(TVI_ROOT, ofPath,iTreeCtrl);
	 }
	 else
	 {
		 bRet = false;
		 goto lExit;
	 }

lExit:
	 return bRet;
 }

bool CFavorite::getFavoritePath(TCHAR ofPath[1024])
{
	HKEY hKey;
	TCHAR sz[1024]={_T("")};

	if(RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders"), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
	{
		TRACE0("Favorites folder not found\n");
		return 0;
	}

	ofPath[1024-1] = '\0';
	DWORD dwSize = 1024;
	long lRet = RegQueryValueEx(hKey, _T("Favorites"), NULL, NULL, (LPBYTE)sz, &dwSize);
	RegCloseKey(hKey);
	if (lRet != ERROR_SUCCESS || sz[1024-1] != '\0')
	{
		TRACE0("Favorites folder not found\n");
		return 0;
	}
	ExpandEnvironmentStrings(sz, ofPath, 1024);

	return true;
}

/*
* create FireFox bookmark tree.
*	input:
*		hParent:  current node.
*		parPath:  current node path.
*	output:
*		return true on successfully, else false.
*/
bool CFavorite::createFireFoxTree(HTREEITEM hParent, TCHAR *filename,CTreeCtrl* iTreeCtrl)
{
	bool bRet=true;

	if ( hParent!=TVI_ROOT)
	{
		bRet = false;
		goto lExit;
	}

	// root node.
	if (hParent==TVI_ROOT)
	{
		CFile file;
		unsigned long long len;

		try
		{
			BOOL bSuccess = file.Open((LPCTSTR)filename, CFile::modeRead);
			if (!bSuccess)
				return false;

			len = file.GetLength();
		}
		catch(CFileException *e)
		{
			TCHAR buf[128];
			e->GetErrorMessage( (LPTSTR)buf, 128);
			AfxMessageBox((LPCTSTR)buf);

			return false;
		}

		TCHAR * buf = NULL;
		char  * mbbuf = NULL;

		buf= new TCHAR[len+1];	memset(buf, 0, len + 1);
		mbbuf= new char[len+1];	memset(mbbuf, 0, len + 1);
		
		//tiny up the bookmark content.
		file.Read(mbbuf, (unsigned int)len);
		file.Close();
		// MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbbuf, (int)len, buf, (int)len);
		// Bug fixed by Charry 2007-04-14
		MultiByteToWideChar(CP_UTF8, 0, mbbuf, (int)len, buf, (int)len);

		TCHAR* beginBuf = _tcsstr(buf, _T("<DL>") );
		wmemmove(buf, beginBuf, _tcslen(beginBuf)+1);
		deleteTag(buf, _T("<p>") );
		deleteTag(buf, _T("<HR>") );
		deleteAtrribute(buf, _T("ICON=\"") );
		deleteAtrribute(buf, _T("ID=\"") );
		deleteAtrribute(buf, _T("LAST_MODIFIED=\"") );
		deleteAtrribute(buf, _T("PERSONAL_TOOLBAR_FOLDER=\"") );
		deleteAtrribute(buf, _T("FEEDURL=\"") );

		//here, we get pure list, begin to push it into stack.
		/*
		<DL>
			<DT><A HREF="" >Get Bookmark Add-ons</A>
			<DT><A >New Bookmark</A>
			<DT><H3 >Bookmarks Toolbar Folder</H3>
			<DD>Add bookmarks to this folder to see them displayed on the Bookmarks Toolbar
			<DL>
				<DT><H3 >New Folder</H3>
				<DT><A HREF="http://en-US.www.mozilla.com/en-US/firefox/central/" >Getting Started</A>
				<DT><A HREF="http://en-US.fxfeeds.mozilla.com/en-US/firefox/livebookmarks/" >Latest Headlines</A>
				<DT><A HREF="http://www.amaxgs.com/" >Problem loading page</A>
			</DL>
			<DT><H3 ID="rdf:#$ZvPhC3">Mozilla Firefox</H3>
			<DL>
				<DT><A HREF="http://en-US.www.mozilla.com/en-US/firefox/help/" >Help and Tutorials</A>
				<DT><A HREF="http://en-US.www.mozilla.com/en-US/firefox/customize/" >Customize Firefox</A>
				<DT><A HREF="http://en-US.www.mozilla.com/en-US/firefox/community/" >Get Involved</A>
				<DT><A HREF="http://en-US.www.mozilla.com/en-US/firefox/about/" >About Us</A>
			</DL>
			<DT><A HREF="http:" >Problem loading page</A>
		</DL>
		*/
		//let me chaw the buf, get a <DL> or <DT> once.
		TCHAR * cBuf = buf;
		chawDTDL(cBuf);
		
		//put it into tree, finished chawing.
		ejectToTree(iTreeCtrl);

		delete buf;
		delete mbbuf;

		buf = NULL;
		mbbuf = NULL;
		cBuf = NULL;
		beginBuf = NULL;
	}

lExit:
	return bRet;
}
bool CFavorite::ejectToTree(CTreeCtrl* iTreeCtrl)
{
	bool bRet = true;

	//clear the bowl.
	iTreeCtrl->DeleteAllItems();
	
	// climb through the stack.
	// get a dl to dive, and get a ld to rise.
	HTREEITEM hCurPar  = TVI_ROOT;
	HTREEITEM hPreNode = TVI_ROOT;

	int dl=-1;
	int totalCount = _bmStack.count;
	for (int i=0; i< totalCount; i++)
	{
		NODE nd;

		this->reversePop(nd);
		if (nd.nt == DL )
		{
			dl++; //dive.
			hCurPar = hPreNode;		
		}
		else if (nd.nt == LD )
		{
			dl--; //rise.
			if (hCurPar!=TVI_ROOT)
				hCurPar = iTreeCtrl->GetParentItem(hCurPar);//parent of hCurPar;
		}
		else if (nd.nt == DT )
		{
			HTREEITEM nn= iTreeCtrl->InsertItem(nd.title, hCurPar);
			iTreeCtrl->SetItemData(nn, m_favString.Add(nd.url));
			hPreNode = nn;
		}
	}
 
	//here: dl==0,else error.
	if (dl!=0)
	{
		bRet = false;
		goto lExit;
	}

lExit:
	return bRet;
}

bool CFavorite::chawDTDL(TCHAR *cBuf)
{
	/*
	*  if eat a "<", then eat 4 to 5 chars continually.
	*  and if chaw a <DL> or <DL>, then spit it into stack.
	*/
	bool bRet = true;

	TCHAR *tmp = cBuf;
	TCHAR tag[8]={_T("")};
	TCHAR ut[1024]={_T("")};
	NODE tnode;

	tnode.childNode = NULL;
	tnode.nextNode  = NULL;
	while (tmp[0] != _T('<'))tmp++;
	tnode.childNode = NULL;
	tnode.nextNode  = NULL;
	tnode.title.Append(_T(""));
	tnode.title.Append(_T(""));

	if (tmp[0]==_T('<'))
	{
		memset(tag, 0, _tcslen(tag)*2);
		if (tmp[1]==_T('/'))
			wcsncpy_s(tag, 8, tmp, 5); //<DL> or <DT>, and delete it if <DD>.
		else
			wcsncpy_s(tag, 8, tmp, 4); //<DL> or <DT>, and delete it if <DD>.
		if (  (_tcsicmp(tag, _T("<DT>"))==0 ) )
		{
			//We current are at <DT> or <DL> find next <D.
			TCHAR *tmpdlt = _tcsstr(tmp+4, _T("<D"));  // tmp<DT><A > </A> | <DL><H3> </H3>  tmpdlt
			TCHAR *tmpldt = _tcsstr(tmp+4, _T("</D"));
			if (!tmpdlt)
				tmpdlt = tmpldt;
			else if (tmpldt< tmpdlt)
				tmpdlt=tmpldt;

			tnode.nt = DT;

			//get url;
			int i=0;
			TCHAR * httpstart = _tcsstr(tmp, _T("http:"));
			if (!httpstart || (httpstart>tmpdlt) )
				tnode.url=_T("");
			else if (httpstart)
			{
				while(httpstart[i] != _T('"'))
					i++;
				wcsncpy_s(ut, 1024, httpstart, i);
				ut[i]= NULL;
				tnode.url = ut;
			}

			//get title
			TCHAR *titlestart = NULL;
			if (!httpstart || (httpstart>tmpdlt))
				titlestart = _tcsstr(tmp+6, _T(">"));
			else if(httpstart)
				titlestart = _tcsstr(httpstart+i, _T(">"));

			titlestart++;
			if (titlestart)
			{
				i=0;
				while(titlestart[i] != _T('<'))
					i++;
				wcsncpy_s(ut, 1024, titlestart, i);
				ut[i] = NULL;
				tnode.title = ut;
			}

			tmp = tmpdlt; // get to next line.
			this->push(tnode);
		}
		else if ( (_tcsicmp(tag, _T("<DL>"))==0 ) ||
				  (_tcsicmp(tag, _T("</DL>"))==0 ) )
		{
//			memset(&tnode, 0, sizeof(tnode));
			if (_tcsicmp(tag, _T("<DL>"))==0 )
			{
				tnode.nt = DL;
				this->push(tnode);
				tmp=tmp+4;
			}
			else
			{
				tnode.nt = LD;
				this->push(tnode);
				tmp=tmp+5;
			}
		}
		else if ( _tcsicmp(tag, _T("<DD>"))==0 )
		{
			//delete the stuff.
			// TCHAR *tmpdd = _tcsstr(tmp+_tcslen(tag), _T("<D"));
			TCHAR *tmpdd = _tcsstr(tmp+_tcslen(tag), _T("<"));
			if (!tmpdd)
				tmpdd = _tcsstr(tmp+_tcslen(tag), _T("</D"));

			if (tmpdd)
			{
#ifdef _UNICODE
				memmove(tmp, tmpdd, _tcslen(tmpdd)*2+1);
#else
				memmove(tmp, tmpdd, _tcslen(tmpdd)+1);
#endif
			}
		}
		else // sorrry, i don't know the tag.
		{
			bRet = false;
			goto lExit;
		}
	}

	// continue to chaw next line.
	if( (_tcsstr(tmp, _T("<D"))) || (_tcsstr(tmp, _T("</D"))))
		chawDTDL(tmp);

lExit:
	return bRet;
}
bool CFavorite::createFavoriteTree(HTREEITEM hParent, TCHAR parPath[1024],CTreeCtrl* iTreeCtrl)
{
	//get favorite folder into szPath, and backup into strPath and strPath2 ended with '\'
	TCHAR szPath[1024]={_T("")};
	TCHAR sz[1024]={_T("")};
	HKEY  hKey=0;

	wcscpy_s(szPath, 1024, parPath);

	CString strPath=_T("");
	CString strPath2=_T("");

	strPath = szPath;
	if(strPath[strPath.GetLength() - 1] != _T('\\'))
		strPath += _T('\\');
	strPath2 = strPath;
	strPath += "*.*";

	TCHAR		str[256]  = { _T("") };
	TCHAR		buf[1024] = { _T("") };
	WIN32_FIND_DATA wfd;
	HANDLE      h;

	// scan the folder and subfoler. 
	h = FindFirstFile(strPath, &wfd);
	if (h != INVALID_HANDLE_VALUE)
	{
		if ( (_tcscmp(wfd.cFileName, _T("."))  !=0 ) &&
			(_tcscmp(wfd.cFileName, _T("..")) !=0 ) ) //((wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==0)
		{
#if _UNICODE
			wcscpy_s(str, 1024, wfd.cFileName);
#else
			strcpy(str, wfd.cFileName);
#endif
			if( _tcsstr(str, _T(".url")) )
			{
				DWORD dwSize;
				dwSize = ::GetPrivateProfileString(_T("InternetShortcut"), _T("URL"),
					_T(""), buf, 1024,
					strPath2 + str);
				if (dwSize < 1024 &&
					buf[dwSize] == '\0')
				{
					//str = str.Left(str.GetLength() - 4);
					//str[_tcslen(str)-4]= ;
					wcsncpy_s(str, 256, str, _tcslen(str)-4);

					HTREEITEM htmp = iTreeCtrl->InsertItem(str, hParent);
					iTreeCtrl->SetItemData(htmp, m_favString.Add(buf));
				}
			}
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{	
				TCHAR curPath[1024]={_T("")};
				HTREEITEM htmp = iTreeCtrl->InsertItem(wfd.cFileName, hParent);
				iTreeCtrl->SetItemData(htmp, -1);
				wcscpy_s(curPath, 1024, szPath);
				wcscat_s(curPath, 1024, wfd.cFileName);
				wcscat_s(curPath, 1024, _T("\\"));
				createFavoriteTree(htmp, curPath,iTreeCtrl);
			}
		}

		while( FindNextFile(h, &wfd) )
		{
			if ( (_tcscmp(wfd.cFileName, _T("."))==0) ||  
				(_tcscmp(wfd.cFileName, _T(".."))==0) )
				continue;
			if (true)//((wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==0)
			{
				wcscpy_s(str, 256, wfd.cFileName);
				//if(str.Right(4) == _T(".url"))
				if ( _tcsstr(str, _T(".url")) )
				{
					DWORD dwSize;
					dwSize = ::GetPrivateProfileString(_T("InternetShortcut"), _T("URL"),
						_T(""), buf, 1024, strPath2 + str);
					if (dwSize < 1024 &&
						buf[dwSize] == '\0')
					{
						wcsncpy_s(str, 256, str, _tcslen(str)-4);
						HTREEITEM htmp = iTreeCtrl->InsertItem(str, hParent);	
						iTreeCtrl->SetItemData(htmp, m_favString.Add(buf));
					}
				}
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				{
					TCHAR curPath[1024]={_T("")};
					HTREEITEM htmp = iTreeCtrl->InsertItem(wfd.cFileName, hParent);
					iTreeCtrl->SetItemData(htmp, -1);
					wcscpy_s(curPath, 1024, szPath);
					wcscat_s(curPath, 1024, _T("\\"));
					wcscat_s(curPath, 1024, wfd.cFileName);
					createFavoriteTree(htmp, curPath,iTreeCtrl);
				}
			}
		}
		FindClose(h);
	}
	return true;
}

// not finished, Plan to extend.
bool CFavorite::createFavoriteMenu(PFAVORITETREELIST pCurTreeList, PFAVORITETREELIST pParentTreeList,CString parPath)
{
	pCurTreeList->m_pParent = pParentTreeList;

	//get favorite folder into szPath, and backup into strPath and strPath2 ended with '\'
	TCHAR szPath[1024]={_T("")};
	TCHAR sz[1024]={_T("")};
	HKEY  hKey=0;

	if (pParentTreeList==NULL)
	{
		getFavoritePath(szPath);
	}
	else
	{
		CString tmp = parPath+pParentTreeList->m_strFavoTitle;
		wcscpy_s(szPath, 1024, tmp.GetBuffer(0));
	}
	CString         strPath=_T("");
	CString         strPath2=_T("");

	strPath = szPath;
	if(strPath[strPath.GetLength() - 1] != _T('\\'))
		strPath += _T('\\');
	strPath2 = strPath;
	strPath += "*.*";

	TCHAR		str[256]  = { _T("") };
	TCHAR		buf[1024] = { _T("") };
	WIN32_FIND_DATA wfd;
	HANDLE      h;

	// scan the folder and subfoler. 
	h = FindFirstFile(strPath, &wfd);
	if (h != INVALID_HANDLE_VALUE)
	{
		if ( (_tcscmp(wfd.cFileName, _T("."))  !=0 ) &&
			(_tcscmp(wfd.cFileName, _T("..")) !=0 ) ) //((wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==0)
		{
#if _UNICODE
			wcscpy_s(str, 1024, wfd.cFileName);
#else
			strcpy(str, wfd.cFileName);
#endif
			/*	str = wfd.cFileName;*/

			//if(str.Right(4) == _T(".url"))
			if( _tcsstr(str, _T(".url")) )
			{
				DWORD dwSize;
				dwSize = ::GetPrivateProfileString(_T("InternetShortcut"), _T("URL"),
					_T(""), buf, 1024,
					strPath2 + str);
				if (dwSize < 1024 &&
					buf[dwSize] == '\0')
				{
					//str = str.Left(str.GetLength() - 4);
					//str[_tcslen(str)-4]= ;
					wcsncpy_s(str, 256, str, _tcslen(str)-4);

					wcscpy_s( pCurTreeList->m_strFavoTitle, 256, str);
					wcscpy_s( pCurTreeList->m_strFavoURL, 1024, buf);
				}
			}
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				pCurTreeList->m_FAVOType = FT_FOLDER;
				pParentTreeList->m_pChild = new FAVORITETREELIST; 
				pParentTreeList->m_pChild->m_pParent = pParentTreeList;
				memset((void*)(pParentTreeList->m_pChild), 0, sizeof(FAVORITETREELIST));
				createFavoriteMenu(pParentTreeList->m_pChild, pCurTreeList,strPath2);
			}
			else if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				pCurTreeList->m_FAVOType = FT_FILE;
		}

		PFAVORITETREELIST pActiveTree=NULL;
		PFAVORITETREELIST pPrePoint=NULL;
		pActiveTree = pCurTreeList;
		while( FindNextFile(h, &wfd) )
		{
			if ( (_tcscmp(wfd.cFileName, _T("."))==0) ||  
				(_tcscmp(wfd.cFileName, _T(".."))==0) )
				continue;
			pPrePoint=NULL;
			if (pActiveTree->m_FAVOType == FT_FOLDER)
			{
				pPrePoint = pActiveTree;
				pActiveTree = new FAVORITETREELIST;
			}
			pActiveTree->m_pPrev = pPrePoint;
			if (pPrePoint)
				pPrePoint->m_pNext = pActiveTree;
			memset((void*)pActiveTree,0, sizeof(FAVORITETREELIST));
			if (true)//((wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==0)
			{
				wcscpy_s(str, 256, wfd.cFileName);
				//if(str.Right(4) == _T(".url"))
				if ( _tcsstr(str, _T(".url")) )
				{
					DWORD dwSize;
					dwSize = ::GetPrivateProfileString(_T("InternetShortcut"), _T("URL"),
						_T(""), buf, 1024, strPath2 + str);
					if (dwSize < 1024 &&
						buf[dwSize] == '\0')
					{
						wcsncpy_s(str, 256, str, _tcslen(str)-4);
						//str = str.Left(str.GetLength() - 4);
						wcscpy_s(pActiveTree->m_strFavoTitle, 256, str);
						wcscpy_s(pActiveTree->m_strFavoURL, 1024, buf);
					}
				}
				else // this is a folder.
				{
					wcscpy_s(pActiveTree->m_strFavoTitle, 256, str);
					//pActiveTree->m_strFavoTitle = str;
				}

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
				{
					pActiveTree->m_FAVOType = FT_FOLDER;
					pActiveTree->m_pChild = new FAVORITETREELIST;
					memset((void*)(pActiveTree->m_pChild), 0, sizeof(FAVORITETREELIST));
					pActiveTree->m_pChild->m_FAVOType = FT_NONE; 
					pActiveTree->m_pChild->m_pParent = pActiveTree;
					createFavoriteMenu(pActiveTree->m_pChild, pActiveTree,strPath2);
				}
				else if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					pCurTreeList->m_FAVOType = FT_FILE;
			}
		}
		FindClose(h);
	}
	return true;
}
/*
*	delete string like 'ICON="*"';
*	tag : such as "ICON=\""
*/
bool CFavorite::deleteAtrribute(TCHAR *srcdes, TCHAR *attribute)
{
	bool bRet =true;
	int i=0;
	int atrlen= _tcslen(attribute);

	TCHAR *tmp=_tcsstr(srcdes, attribute);
	if(tmp)
	{	
		while((tmp+atrlen+i)[0]!=_T('"'))
			i++;
#ifdef _UNICODE 
	wmemmove(tmp, tmp+atrlen+i+1, _tcslen(tmp+atrlen+i+1)+1);
#else
	memmove(tmp, tmp+atrlen+i+1, _tcslen(tmp+atrlen+i+1)+1);
#endif
	}

	if (_tcsstr(srcdes, attribute))
		deleteAtrribute(srcdes, attribute);

lExit:
	return bRet;
}
bool CFavorite::deleteTag(TCHAR *srcdes, TCHAR *tag)
{
	bool bRet =true;
	unsigned int taglen=_tcslen(tag);  

	try
	{
		TCHAR * tmp = _tcsstr(srcdes, tag);
		if(tmp)
		{
#ifdef _UNICODE
			memmove(tmp, tmp+taglen, _tcslen(tmp+taglen)*2+1);
#else
			memmove(tmp, tmp+taglen, _tcslen(tmp+taglen+1)+1);
#endif
		}
		if (_tcsstr(srcdes, tag))
			deleteTag(srcdes, tag);
	}
	catch(...)
	{
		bRet = false;
		goto lExit;
	}
lExit:
	return bRet;
}
bool CFavorite::deleteDDContent(TCHAR *srcdes)
{
	bool bRet = true;
	try
	{
		TCHAR *tmp = _tcsstr(srcdes, _T("<DD>"));
		if(tmp)
		{
			TCHAR *tmp2 = _tcsstr(tmp+4, _T("<D"));
#ifdef _UNICODE
			memmove(tmp, tmp2, _tcslen(tmp2)*2+1);
#else
			memmove(tmp, tmp2, _tcslen(tmp2)+1);
#endif

		}
	}
	catch(...)
	{
		bRet=false;
		goto lExit;
	}

lExit:
	return bRet;
}
bool CFavorite::replaceString(TCHAR *source, TCHAR *oldStr, TCHAR *newStr)
{
	bool bRet = true;

	return bRet;
}

void CFavorite::getFavUrl(CStringArray &arrayUrl)
{
	int size = arrayUrl.GetSize();
	for (int i=0; i<m_favString.GetSize(); ++i)
	{
		arrayUrl.InsertAt(size+i, m_favString.GetAt(i));
	}
}

void CFavorite::getFavoriteFromIE(CTreeCtrl* iTreeCtrl)
{
	TCHAR ofPath[1024]={_T("")};
	TCHAR sz[1024]={_T("")};
	
	
	iTreeCtrl->DeleteAllItems();
	
	wcscpy_s(sz, 1024, IEFAVORITESPATH);
	ExpandEnvironmentStrings(sz, ofPath, 1024);
	createFavoriteTree(TVI_ROOT, ofPath,iTreeCtrl);
}


BOOL CFavorite::getFavoriteFromFireFox(CTreeCtrl* iTreeCtrl)
{
	TCHAR ofPath[1024]={_T("")};
	TCHAR sz[1024]={_T("")};
	
	iTreeCtrl->DeleteAllItems();


	TCHAR Profiles[1024]  = {_T("")};
	TCHAR endSection[128] = {_T("")};
	TCHAR defaultSection[128] = {_T("")};
	int sCount=0;
	
	if( 0 == ExpandEnvironmentStrings(_T("%appdata%\\mozilla\\Firefox\\"), sz, 1024) )
		return FALSE;
	wcscat_s(Profiles, 1024, sz);
	wcscat_s(ofPath, 1024, sz);
	wcscat_s(Profiles, 1024, _T("profiles.ini"));
	if( false == getFFSectionCount(Profiles, endSection, 128, defaultSection, 128, sCount) )
		return FALSE;
	
	TCHAR path[1024]={_T("")};
	GetPrivateProfileString(defaultSection, _T("Path"), _T(""), path, 1024, Profiles);
	TCHAR * slashPos=_tcschr(path, _T('/'));
	if (slashPos)
	{
		slashPos[0]='\\';
	}
	wcscat_s(ofPath, 1024, path);
	wcscat_s(ofPath, 1024, _T("\\bookmarks.html"));
	return createFireFoxTree(TVI_ROOT, ofPath,iTreeCtrl);
}

struct HeaderFavorites
{
	int one;	// 8
	int two;	// 2
	int TotalSize;	// totalsize of buffer not include one and two fields.
	int Flag1;	// 1
	int Number; // num of items include folders and files
};

struct ItemFavorite
{
	int SizeItem; // the size of total bytes in one item
	int noFB;     // always FFFFFFFB in win2k, index in winXp
	WORD code1; // the two fields might be used as code page??? just used in win2k
	WORD code2;
	int FileSize; // the size of file, if folder, 0; bytes
	int noc; // low 36B8
	WORD Type; // 0x10 folder (FILE_ATTRIBUTE_DIRECTORY); 0x20 url (FILE_ATTRIBUTE_ARCHIVE) to see WIN32_FIND_DATA.dwFileAttributes
};

struct ItemParamXP
{
	int XPone;	// high 0003
	int XPtwo;	// BEEF0004
	int XPthree;	// equal ItemFavorite.noc
	int XPfour;		// most time equal three
	int XPfive;	// 00000014
};

void CFavorite::GetFavoriterFromIEOrder(CTreeCtrl* iTreeCtrl)
{
	HKEY hkeyRead;
	if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,
		IEFAVORITESREG,0,KEY_QUERY_VALUE, &hkeyRead) )
	{
		return getFavoriteFromIE(iTreeCtrl);
	}

	RegCloseKey(hkeyRead);
	
	iTreeCtrl->DeleteAllItems();

	CString strFavFilePath = GetFavoriteFilePath();
	CreateFavoriteTreeOrder(TVI_ROOT,IEFAVORITESREG,strFavFilePath,iTreeCtrl);
	return;
}


BOOL CFavorite::CreateFavoriteTreeOrder(HTREEITEM hParent,
										TCHAR *regKey,
										CString &strFavFilePath,
										CTreeCtrl* iTreeCtrl)
{
	HKEY hkeyRead;
	if( ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,regKey,0,KEY_QUERY_VALUE, &hkeyRead) )
		return FALSE;

	DWORD cbData = 0;
	DWORD type = REG_BINARY;
	RegQueryValueEx(hkeyRead,_T("Order"),NULL,&type,NULL,&cbData);
	LPBYTE lpData = new BYTE[cbData];
	if( ERROR_SUCCESS != RegQueryValueEx(hkeyRead,_T("Order"),NULL,&type,lpData,&cbData) )
	{
		RegCloseKey(hkeyRead);
		delete [] lpData;
		return FALSE;
	}
	RegCloseKey(hkeyRead);

	HeaderFavorites* hf = (HeaderFavorites*)lpData;
	LPBYTE lpBuf = lpData + sizeof(HeaderFavorites);
	for( int i = 0; i < hf->Number; i++ )
	{
		ItemFavorite *itFav = (ItemFavorite*)lpBuf;
		CString strName = GetFavoriteName(itFav->code2,lpBuf);
		CString strPath = strFavFilePath + _T("\\") + strName;

		if( itFav->Type == FILE_ATTRIBUTE_DIRECTORY ) // folder
		{
			HTREEITEM htmp = iTreeCtrl->InsertItem(strName, hParent);
			iTreeCtrl->SetItemData(htmp, -1);			
			
			CString strRegkey;
			strRegkey = regKey;
			strRegkey += _T("\\") + strName;
			CreateFavoriteTreeOrder(htmp,strRegkey.GetBuffer(),strPath,iTreeCtrl);
		}
		else // files
		{
			TCHAR strURL[1024];
			DWORD dwSize = GetPrivateProfileString(_T("InternetShortcut"),_T("URL"),_T(""),
				strURL,1024,strPath);
			if( dwSize < 1024 )
			{
				strName = strName.Left(strName.ReverseFind(_T('.')));
				HTREEITEM htmp = iTreeCtrl->InsertItem(strName,hParent);
				iTreeCtrl->SetItemData(htmp,m_favString.Add(strURL));
			}
		}
		lpBuf = lpBuf + itFav->SizeItem;
	}

	delete [] lpData;
	return TRUE;
}

CString CFavorite::GetFavoriteName(int codepage, LPBYTE lpData)
{
	// lpData + sizeof(ItemFavorite)
	char* lpBuf = (char*)(lpData+22);
	CString strName;
	OSVerInfo osv;
	CMyUtility::getOSVersion(osv);
	if( osv._osver == VBWINXP ) // XP
	{
		ItemParamXP *ipxp;
		if( codepage > 0x33 )
		{
			int lencode = lstrlen((wchar_t*)lpBuf);
			if( lencode <= 12 )
				ipxp = (ItemParamXP*)(lpBuf+lencode*2+2);
			else
			{
				// if the len of shortfilename below 12 charactor,
				// the space is 20 bytes
				// or, space is 24 bytes.
				ipxp = (ItemParamXP*)(lpBuf+20);
				if( ipxp->XPtwo != 0xBEEF0004 )
					ipxp = (ItemParamXP*)(lpBuf+24);
			}
		}
		else
		{
			int lencode = strlen(lpBuf);			
			if( lencode == 8 || lencode == 12 )
			{
				if( 0 != strchr(lpBuf,'~') )
					lencode++;
			}
			lencode++;
			IsOvenNumber(lencode);
			ipxp = (ItemParamXP*)(lpBuf+lencode);			
		}
		// (LPBYTE)ipxp+sizeof(ItemParamXP)
		strName = (wchar_t*)(((LPBYTE)ipxp)+20);	
	}
	else
	{
		if( codepage > 0x33 )
		{
			strName = (wchar_t*)lpBuf;
		}
		else
		{
			strName = (char*)lpBuf;
		}
	}
	return strName;
}

CString CFavorite::GetFavoriteFilePath()
{
	CString strName;
	TCHAR ofPath[1024]={_T("")};
	TCHAR sz[128]={_T("")};

	wcscpy_s(sz, 128, IEFAVORITESPATH);
	ExpandEnvironmentStrings(sz, ofPath, 1024);
	strName = ofPath;
	return strName;
}
