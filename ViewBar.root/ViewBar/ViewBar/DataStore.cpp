#include "StdAfx.h"
#include "DataStore.h"

CDataStore* CDataStore::m_pInstance = NULL;
CDataStore::CDataStore(void)
{
}

CDataStore::~CDataStore(void)
{
	RemoveAllAffiliates();
}

CDataStore* CDataStore::GetInstance()
{
	if( NULL == m_pInstance )
		m_pInstance = new CDataStore();
	return m_pInstance;
}

void CDataStore::FreeInstance()
{
	if( m_pInstance )
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CDataStore::RemoveAllAffiliates()
{
	POSITION pos = m_listAffiliate.GetHeadPosition();
	while( pos )
	{
		PAFFILIATE pAff = m_listAffiliate.GetNext(pos);
		delete pAff;
	}
	m_listAffiliate.RemoveAll();
}