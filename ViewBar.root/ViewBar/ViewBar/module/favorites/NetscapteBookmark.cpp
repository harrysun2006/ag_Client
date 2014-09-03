#include "StdAfx.h"
#include "NetscapteBookmark.h"

CNetscapteBookmark::CNetscapteBookmark(CTreeCtrl *pTree):
	m_iCurrentPosition(0),
	m_pTree(pTree)
{

}

CNetscapteBookmark::CNetscapteBookmark(void):
	m_iCurrentPosition(0)
{

}

CNetscapteBookmark::~CNetscapteBookmark(void)
{

}

void CNetscapteBookmark::AttachTree(CTreeCtrl *pTree)
{
	m_pTree = pTree;
}

BOOL CNetscapteBookmark::LoadBookmark()
{
	CFile file;

	try
	{
		//
		// Open bookmarks.html and read html to buffer
		//

		CString strProfile = GetBookmarkPath();
		BOOL bOpen = file.Open(strProfile, CFile::modeRead);
		size_t iFileLen = file.GetLength();
		char *pBuf = NULL;
		pBuf = new char[iFileLen + 1];
		memset(pBuf, 0, iFileLen + 1);
		file.Read(pBuf, iFileLen);
		
		//
		// Convert to UNICODE string
		//
		size_t iWideStringLen = 0;
		iWideStringLen = ::MultiByteToWideChar(CP_UTF8, 0, pBuf, iFileLen, 0, 0);
		if (iWideStringLen > 0)
		{
			BSTR unicodeSting;
			unicodeSting = ::SysAllocStringLen(0, iWideStringLen);
			::MultiByteToWideChar(CP_UTF8, 0, pBuf, iFileLen, unicodeSting, iWideStringLen);

			m_strHtml = unicodeSting;
			::SysFreeString(unicodeSting);
		}
		else
		{
			// Release
			delete [] pBuf;
			file.Close();

			return FALSE;
		}

		// Release
		delete [] pBuf;
		file.Close();
	}
	catch (CException* e)
	{
		// NOOP

		return FALSE;
	}
	
	return TRUE;
}

BOOL CNetscapteBookmark::HasNextElement()
{
	int iPosDL = m_strHtml.Find(_T("<DL>"), m_iCurrentPosition);
	int iPosDT = m_strHtml.Find(_T("<DT>"), m_iCurrentPosition);
	int iPosLD = m_strHtml.Find(_T("</DL>"), m_iCurrentPosition);

	// If no tags found, return FALSE
	if (iPosDL == -1 && iPosDT == -1 && iPosLD == -1) return FALSE;

	//
	// Find the nearest tag, if the position of one tag equals -1
	// We should ignore it.
	// 
	int iMin = -1;
	if (iPosDL != -1)
		iMin = iPosDL;
	else
	if (iPosDT != -1)
		iMin = iPosDT;
	else
		iMin = iPosLD;

	if (iPosDL != -1 && iMin > iPosDL)
	{
		iMin = iPosDL;
	}

	if (iPosDT != -1 && iMin > iPosDT)
	{
		iMin = iPosDT;
	}

	if (iPosLD != -1 && iMin > iPosLD)
	{
		iMin = iPosLD;
	}

	Node node;

	// <DL> found
	if (iMin == iPosDL)
	{
		m_nextItemType = tagDL;
		m_iCurrentPosition = iPosDL + 1;

		node.type = tagDL;
	}
	else
	// <DT> found
	if (iMin == iPosDT)
	{
		m_nextItemType = tagDT;
		m_iCurrentPosition = iPosDT + 1;
		
		// Example:
		// <DT><A HREF="http://www.microsoft.com/">CNN Videoselect</A>

		// Find name of item
		int iStart = m_strHtml.Find(_T(">"), iPosDT + lstrlen(_T("<DT>")) + 1 );
		int iEnd = m_strHtml.Find(_T("<"), iPosDT + lstrlen(_T("<DT>")) + 1 );
		CString strName = m_strHtml.Mid(iStart + 1, iEnd - iStart - 1);

		// Find URL
		int iUrlStart = m_strHtml.Find(_T("HREF=\""), iPosDT + lstrlen(_T("HREF=\"")) + 1);
		CString strUrl;
		if (iUrlStart != -1)
		{
			int iUrlEnd = m_strHtml.Find(_T("\""), iUrlStart + lstrlen(_T("HREF=\"")) + 1);
			if (iUrlEnd < iEnd)
			{
				strUrl = m_strHtml.Mid(iUrlStart + lstrlen(_T("HREF=\"")), iUrlEnd - iUrlStart - lstrlen(_T("HREF=\"")));
			}
		}
		
		node.name = strName;
		node.url = strUrl;
		node.type = tagDT;
	}
	else
	if (iMin == iPosLD)
	{
		m_nextItemType = tagLD;
		m_iCurrentPosition = iPosLD + 1;

		node.type = tagLD;
	}

	node.index = m_stack.size();
	m_stack.push_back(node);

	return TRUE;
}

CNetscapteBookmark::ITEM_TYPE CNetscapteBookmark::NextElement()
{
	return m_nextItemType;
}

void CNetscapteBookmark::DumpToTree(CTreeCtrl* pTreeCtrl)
{
	pTreeCtrl->DeleteAllItems();

	HTREEITEM hCurPar  = TVI_ROOT;
	HTREEITEM hPreNode = TVI_ROOT;

	for (int i=0; i< m_stack.size(); ++i)
	{
		Node node;
		node = m_stack[i];

		HTREEITEM hItem = NULL;
		switch(node.type)
		{
		case tagDL:
			hCurPar = hPreNode;
			break;

		case tagLD:
			if (hCurPar != TVI_ROOT)
				hCurPar = pTreeCtrl->GetParentItem(hCurPar);	//parent of hCurPar;
			break;

		case tagDT:
			hItem = pTreeCtrl->InsertItem(node.name, hCurPar);
			pTreeCtrl->SetItemData(hItem, node.index);
			hPreNode = hItem;
			break;

		default:
			break;
		}
	}
}

CString CNetscapteBookmark::LookUp(size_t index)
{
	Node node = m_stack[index];
	return node.url;
}

void CNetscapteBookmark::Parse()
{
	LoadBookmark();

	while (HasNextElement())
	{
		// NOOP
	}

	DumpToTree(m_pTree);

}

BOOL CNetscapteBookmark::GetFFSectionCount(TCHAR *pIniFile, TCHAR *pEndSection, int endLen, TCHAR *defaultSection, int defLen, int &sCount)
{
	BOOL bRet = TRUE;

	TCHAR tmp[MAX_PATH] = {0};
	int sectionCount = GetPrivateProfileSectionNames(tmp, MAX_PATH, pIniFile);

	int j = sectionCount - 2;
	for( ; j>0; j--)
	{
		if ( tmp[j]==0 )
			break;
	}
	_tcscpy_s(pEndSection, endLen, tmp+j+1);

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
			GetPrivateProfileString(tmp+j+1, key, def, defValues, 10, pIniFile);
			if (_tcscmp(defValues, _T("1"))==0)
			{
				_tcscpy_s(defaultSection, defLen, tmp+j+1);
			}
			else if ( (_tcscmp(defValues, _T("0"))==0) &&  
				(_tcscmp(tmp+j+1, _T("Profile0"))==0) &&
				(_tcscmp(defaultSection, _T(""))==0) )
			{
				_tcscpy_s(defaultSection, defLen, _T("Profile0"));
			}
		}
	}

	return bRet;
}

CString CNetscapteBookmark::GetBookmarkPath()
{
	TCHAR szProfilePath[MAX_PATH] = {0};
	ExpandEnvironmentStrings(_T("%appdata%\\mozilla\\Firefox\\"), szProfilePath, MAX_PATH);

	TCHAR szProfiles[MAX_PATH]  = {0};
	_tcscat_s(szProfiles, MAX_PATH, szProfilePath);
	_tcscat_s(szProfiles, MAX_PATH, _T("profiles.ini"));

	TCHAR szEndSection[128] = {0};
	TCHAR szDefaultSection[128] = {0};
	int sCount=0;
	GetFFSectionCount(szProfiles, szEndSection, 128, szDefaultSection, 128, sCount);

	TCHAR path[MAX_PATH] = {0};
	GetPrivateProfileString(szDefaultSection, _T("Path"), _T(""), path, MAX_PATH, szProfiles);
	TCHAR *slashPos = _tcschr(path, _T('/'));
	if (slashPos)
	{
		slashPos[0]='\\';
	}

	_tcscat_s(szProfilePath, MAX_PATH, path);
	_tcscat_s(szProfilePath, MAX_PATH, _T("\\bookmarks.html"));

	return CString(szProfilePath);
}


void CNetscapteBookmark::GetFavUrl(CStringArray &arrayUrl)
{
	int size = arrayUrl.GetSize();

	for (int i=0; i<m_stack.size(); ++i)
	{
		Node node = m_stack[i];
		arrayUrl.InsertAt(size+i, node.url);
	}
}