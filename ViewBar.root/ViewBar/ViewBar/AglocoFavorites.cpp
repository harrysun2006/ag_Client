#include "StdAfx.h"
#include "AglocoFavorites.h"
#include "AglocoFavoritesItem.h"
#include "CppSQLiteManager.h"
#include "utility/md5/md5class.h"
#include "utility/XmlNodeWrapper.h"
#include "SearchManager.h"
#include "MyUtility.h"

using namespace utility;
using namespace viewbar;

CAglocoFavorites::CAglocoFavorites(void)
{
	LoadFavorites();
}

CAglocoFavorites::~CAglocoFavorites(void)
{
}

BOOL CAglocoFavorites::LoadFavorites()
{
	CCppSQLiteManager::GetInstance()->GetFavorites(m_vecFavorites);
	return TRUE;
}

// calculate hash code for favorites stored in local database
CString CAglocoFavorites::GetFavHashCode()
{
	CString strFav;
	for (int i=0; i<m_vecFavorites.size(); ++i)
	{
		CAglocoFavoritesItem item = m_vecFavorites[i];
		strFav += item.m_strId;
		strFav += item.m_strName;
		strFav += item.m_strUrl;
	}

	utility::CMD5 x;
	CString strFavMd5;
	x.GetMD5(strFav, strFavMd5);
	return strFavMd5;
}

void CAglocoFavorites::GetFavorites(std::vector<CAglocoFavoritesItem> &vec)
{
	for (int i=0; i<m_vecFavorites.size(); ++i)
	{
		CAglocoFavoritesItem item = m_vecFavorites[i];
		vec.push_back(item);
	}
}

// save favorites to local database
BOOL CAglocoFavorites::SaveFavorites(const CString &xml)
{
	if (xml.IsEmpty())
		return FALSE;

	CXmlDocumentWrapper xmlDoc;
	xmlDoc.LoadXML(xml);
	CXmlNodeWrapper nodeRoot(xmlDoc.AsNode());
	CXmlNodeListWrapper nodes(nodeRoot.FindNodes(_T("favorite")));

	CString sql;
	sql.Format(_T("DELETE FROM %s"), _T(_VB_TABLE_FAVORITES));
	CCppSQLiteManager::GetInstance()->ExecSQL(sql);
	for (int i=0; i<nodes.Count(); ++i)
	{
		CXmlNodeWrapper fav(nodes.Node(i));
		sql.Format(_T("INSERT INTO %s(FAV_ID, FAV_NAME, FAV_URL)\
			VALUES(%s, '%s', '%s');"), _T(_VB_TABLE_FAVORITES),
			fav.GetValue(_T("favoriteId")),
			fav.GetValue(_T("name")),
			fav.GetValue(_T("url")));

		CCppSQLiteManager::GetInstance()->ExecSQL(sql);
	}

	return TRUE;
}

CString CAglocoFavorites::GetFavUrl(int iFavId)
{
	CString strId;
	strId.Format(_T("%d"), iFavId);

	for (int i=0; i<m_vecFavorites.size(); ++i)
	{
		CAglocoFavoritesItem item = m_vecFavorites[i];
		if (item.m_strId == strId)
		{
			return item.m_strUrl;
		}
	}

	return _T("");
}
