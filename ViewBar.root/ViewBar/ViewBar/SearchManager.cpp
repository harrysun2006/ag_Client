// ***************************************************************
//  CSearchManager   version:  1.0  date: 04/03/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#include "StdAfx.h"
#include "SearchManager.h"
#include "utility/XmlNodeWrapper.h"
#include "MyUtility.h"
#include "CppSQLiteManager.h"
#include <vector>
#include "utility/md5/md5class.h"
#include "MyAppBar.h"

using namespace utility;

#define DIM(x) ( sizeof((x)) / sizeof((x)[0]) )

CSearchManager * CSearchManager::m_pInstance = NULL;

CSearchManager::CSearchManager(void) : m_pSearchBox(NULL), m_pAppBar(NULL)
{
	LoadSearchEngine();
}

// load search engine from local database
BOOL CSearchManager::LoadSearchEngine()
{
	CString sql;

	DestroySearchEngineList();
	CCppSQLiteManager::GetInstance()->GetSearchEngineList(m_listSearchEngine);

	SetDefaultEngine(thePrefs.GetDefaultSearchEngine());

	return TRUE;
}

// if VB get new search engine list from server, fill in with m_listSearchEngine
void CSearchManager::SetSearchEngineList(std::list<CSearchEngine*> &list)
{
	DestroySearchEngineList();

	// save search engine list to memory
	std::list<CSearchEngine*>::iterator it;
	for (it = list.begin(); it !=list.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		m_listSearchEngine.push_back(pEngine);
	}

	// save search engine list to local database
	SaveSearchEngineList();
}

// destroy
CSearchManager::~CSearchManager(void)
{
	DestroySearchEngineList();
}

void CSearchManager::DestroySearchEngineList()
{
	std::list<CSearchEngine*>::iterator it;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		if (pEngine)
			delete pEngine;
	}
	m_listSearchEngine.erase(m_listSearchEngine.begin(), m_listSearchEngine.end());
}

CSearchManager * CSearchManager::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CSearchManager;

	return m_pInstance;
}

void CSearchManager::FreeInstance()
{
	if (m_pInstance)
		delete m_pInstance;
}

BOOL CSearchManager::AttachSearchBox(CSearchBox *pComb)
{
	m_pSearchBox = pComb;
 	AddEngine();

	return TRUE;
}

// return name and index of default search engine
int CSearchManager::GetDefaultEngine(CString &name, CString &url)
{
	int iDefaultSearchEngine = thePrefs.GetDefaultSearchEngine();

	std::list<CSearchEngine*>::iterator it;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		if (pEngine && pEngine->m_iSearchEngineId == iDefaultSearchEngine)
		{
			name = pEngine->m_strSearchEngineName;
			url = pEngine->m_strSearchEngineURL + _T("/") + pEngine->m_strSearchEnginePattern;
			break;
		}
	}

	return iDefaultSearchEngine;
}

void CSearchManager::SetDefaultEngine(int iSearchEngine)
{
	thePrefs.SetDefaultSearchEngine(iSearchEngine);

	if (!thePrefs.IsViewBarVisible())
		return;

	/*
	if (m_pSearchBox)
	{
		for (int i=0; i<m_pSearchBox->GetCount(); ++i)
		{
			if (iSearchEngine == m_pSearchBox->GetItemData(i))
			{
				m_pSearchBox->SetCurSel(i);
				m_pSearchBox->SetWindowText(CSearchCombo::m_strHistory);
				break;
			}
		}
	}
	*/
	SetSearchEnglineLogo();
}

void CSearchManager::NextEngine()
{
	std::list<CSearchEngine*>::iterator it;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		if (pEngine && pEngine->m_iSearchEngineId == thePrefs.GetDefaultSearchEngine())
		{
			++it;
			if (it != m_listSearchEngine.end())
			{
				pEngine = static_cast<CSearchEngine*> (*it);
				SetDefaultEngine(pEngine->m_iSearchEngineId);
			}
			break;
		}
	}
}

void CSearchManager::PrevEngine()
{
	std::list<CSearchEngine*>::iterator it;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		if (pEngine && pEngine->m_iSearchEngineId == thePrefs.GetDefaultSearchEngine())
		{
			if (it != m_listSearchEngine.begin())
			{
				--it;
				pEngine = static_cast<CSearchEngine*> (*it);
				SetDefaultEngine(pEngine->m_iSearchEngineId);
			}
			break;
		}
	}
}

BOOL CSearchManager::AttachAppBar(CMyAppBar *pAppBar)
{
	m_pAppBar = pAppBar;

	return TRUE;
}

BOOL CSearchManager::SaveSearchEngineList()
{
	std::list<CSearchEngine*>::iterator it;

	// remove all data from local database
	CString sql = _T("DELETE FROM ")_VB_TABLE_SEARCH_ENGINE;
	CCppSQLiteManager::GetInstance()->ExecSQL(sql);

	CSearchEngine *pEngine = NULL;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		pEngine = static_cast<CSearchEngine*> (*it);
		sql.Format(_T("INSERT INTO %s(ENGINE_ID, ENGINE_NAME, ENGINE_URL, ENGINE_PATTERN) VALUES (%d, '%s', '%s', '%s');"),
			_VB_TABLE_SEARCH_ENGINE,
			pEngine->m_iSearchEngineId,
			pEngine->m_strSearchEngineName,
			pEngine->m_strSearchEngineURL,
			pEngine->m_strSearchEnginePattern);
		CCppSQLiteManager::GetInstance()->ExecSQL(sql);
	}

	return TRUE;
}

// add search engines to CComboBox
void CSearchManager::AddEngine()
{
	ASSERT(m_pSearchBox);

	std::list<CSearchEngine*>::iterator it;

	CSearchEngine *pEngine = NULL;
	CSearchEngine *pFirstEngine = NULL;
	BOOL bFound = FALSE;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		pEngine = static_cast<CSearchEngine*> (*it);
		if (pEngine)
		{
			// m_pSearchBox->AddSearchEngine(pEngine->m_iSearchEngineId, pEngine->m_strSearchEngineName);
			if (bFound == FALSE)
			{
				pFirstEngine = pEngine;
				bFound = TRUE;
			}
		}
	}

	// select default search engine
	int iDefaultSearchEngineId = thePrefs.GetDefaultSearchEngine();
	bool bFoundDefaultEngine = false;

	/*
	for (int i=0; i<m_pSearchBox->GetCount(); ++i)
	{
		if (m_pSearchBox->GetItemData(i) == iDefaultSearchEngineId)
		{
			m_pSearchBox->SetCurSel(i);
			bFoundDefaultEngine = true;
			break;
		}
	}
	*/

	// if default engine not set, select the last one
	if (bFoundDefaultEngine == false && pEngine != NULL)
	{
		//thePrefs.SetDefaultSearchEngine(pEngine->m_iSearchEngineId);
		//m_pSearchBox->SetCurSel(m_pSearchBox->GetCount() - 1);

		thePrefs.SetDefaultSearchEngine(pFirstEngine->m_iSearchEngineId);
		//m_pSearchBox->SetCurSel(0);
	}
}

// get search engine id list
BOOL CSearchManager::GetSearchEngineList(CStringArray &list)
{
	std::list<CSearchEngine*>::iterator it;
	list.RemoveAll();

	CString id;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		id.Format(_T("%d"), pEngine->m_iSearchEngineId);
		list.Add(id);
	}

	return TRUE;
}

BOOL CSearchManager::GetSearchEngineList(std::list<std::pair<int,std::tstring> > &xlist)
{
	std::list<CSearchEngine*>::iterator it;

	CString id;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		// char *pName = CMyUtility::GetAnsiString(pEngine->m_strSearchEngineName);
		if (pEngine == NULL) continue;
		std::pair<int, std::tstring> item(pEngine->m_iSearchEngineId, pEngine->m_strSearchEngineName.GetBuffer());
		xlist.push_back(item);
		// delete [] pName;
	}

	return TRUE;
}

// return hash code for all search engines
CString CSearchManager::GetEngineHashCode()
{
	std::list<CSearchEngine*>::iterator it;

	CString all;
	for (it = m_listSearchEngine.begin(); it !=m_listSearchEngine.end(); ++it)
	{
		CSearchEngine *pEngine = static_cast<CSearchEngine*> (*it);
		CString id;
		id.Format(_T("%d"), pEngine->m_iSearchEngineId);
		all += id;
		all += pEngine->m_strSearchEngineName;
		all += pEngine->m_strSearchEngineURL;
		all += pEngine->m_strSearchEnginePattern;
	}

	CString allMd5;
	utility::CMD5::GetMD5(all, allMd5);
	
	return allMd5;
}

// set search engine list
BOOL CSearchManager::SetSearchEngineList(CString &listXML)
{


	return TRUE;
}


void CSearchManager::SetSearchEnglineLogo()
{
	int iId = thePrefs.GetDefaultSearchEngine();

	int iIcon = 0;
	switch(iId)
	{
	case 1:
		iIcon = IDI_ICON_ASK;		break;               //add with BB
	case 2:
		iIcon = IDI_ICON_GOOGLE;	break;
	case 3:
		iIcon = IDI_ICON_BAIDU;		break;
	case 4:
		iIcon = IDI_ICON_LIVE;		break;
	case 5:
		iIcon = IDI_ICON_YAHOO;		break;
	case 6:
		iIcon = IDI_ICON_EBAY;		break;
	case 7:
		iIcon = IDI_ICON_DICT;		break;
	case 8:
		iIcon = IDI_ICON_WIKI;		break;
	case 9:
		iIcon = IDI_ICON_REDIFF;	break;
	case 10:
		iIcon = IDI_ICON_AMAZON;	break;
	default:
		iIcon = IDI_ICON_DEFAULT;
		break;
	}
	m_pAppBar->m_staticSearchLogo.SetIcon(   
		(HICON)::LoadImage(AfxGetInstanceHandle(),   
		MAKEINTRESOURCE(iIcon), IMAGE_ICON, 16, 16, LR_SHARED));

	// arrow
	m_pAppBar->m_staticSearchArrow.SetIcon(   
		(HICON)::LoadImage(AfxGetInstanceHandle(),   
		MAKEINTRESOURCE(IDI_ICON_ARROW), IMAGE_ICON, 16, 16, LR_SHARED));
}