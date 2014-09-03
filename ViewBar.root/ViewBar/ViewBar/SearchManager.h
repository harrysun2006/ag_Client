#pragma once
// ***************************************************************
//  CSearchManager   version:  1.0  date: 04/03/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif // _DEBUG

#include <list>
#include <utility>
#include "ui/searchbox/SearchCombo.h"
#include "SearchBox.h"
#include "SearchEngine.h"

class CMyAppBar;

class CSearchManager
{

public:
	static CSearchManager * GetInstance();
	static void FreeInstance();
	BOOL GetSearchEngineList(CStringArray &list);
	BOOL GetSearchEngineList(std::list<std::pair<int, std::tstring> > &list);
	BOOL SetSearchEngineList(CString &listXMLs);
	BOOL AttachSearchBox(CSearchBox *pCombo);
	BOOL AttachAppBar(CMyAppBar *pAppBar);
	int GetDefaultEngine(CString &name, CString &url);
	void SetDefaultEngine(int iSearchEngine);
	void SetSearchEngineList(std::list<CSearchEngine*> &list);
	CString GetEngineHashCode(void);
	void NextEngine();
	void PrevEngine();
	void SetSearchEnglineLogo();

private:
	CSearchManager(void);
	~CSearchManager(void);
	BOOL LoadSearchEngine();
	void DestroySearchEngineList();
	void AddEngine();
	BOOL SaveSearchEngineList(void);		// save search engine list to local database
	


private:
	static CSearchManager *m_pInstance;
	std::list<CSearchEngine*> m_listSearchEngine;
	//CSearchCombo *m_pSearchBox;
	CSearchBox *m_pSearchBox;
	CMyAppBar *m_pAppBar;

};
