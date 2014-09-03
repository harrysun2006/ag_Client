#include "StdAfx.h"
#include "KeywordStore.h"

CKeywordStore* CKeywordStore::m_pInstance = NULL;
CKeywordStore::CKeywordStore(void)
{
}

CKeywordStore::~CKeywordStore(void)
{
	if( !m_listKW.IsEmpty() )
		m_listKW.RemoveAll();
}

CKeywordStore* CKeywordStore::GetInstance()
{
	if( NULL == m_pInstance )
		m_pInstance = new CKeywordStore();
	return m_pInstance;
}

BOOL CKeywordStore::FreeInstance()
{
	if( NULL != m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
	return TRUE;
}

void CKeywordStore::Push(CString keyword)
{
	POSITION pos = m_listKW.Find(keyword);
	if( NULL == pos )
		m_listKW.AddTail(keyword);
}