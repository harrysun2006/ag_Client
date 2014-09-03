#pragma once

class CAglocoFavoritesItem
{
public:
	CString m_strId;
	CString m_strName;
	CString m_strUrl;

	CAglocoFavoritesItem();
	CAglocoFavoritesItem(const CAglocoFavoritesItem &src);
};
