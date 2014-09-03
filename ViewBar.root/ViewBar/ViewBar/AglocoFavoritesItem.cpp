#include "StdAfx.h"
#include "AglocoFavoritesItem.h"

CAglocoFavoritesItem::CAglocoFavoritesItem()
{

}

CAglocoFavoritesItem::CAglocoFavoritesItem(const CAglocoFavoritesItem &src)
{
	this->m_strId		= src.m_strId;
	this->m_strName		= src.m_strName;
	this->m_strUrl		= src.m_strUrl;
}
