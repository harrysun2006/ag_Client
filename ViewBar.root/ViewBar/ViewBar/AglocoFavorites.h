#pragma once
#include <vector>
#include "AglocoFavoritesItem.h"

class CAglocoFavorites
{
public:
	CAglocoFavorites(void);
	~CAglocoFavorites(void);

	BOOL SaveFavorites(const CString &xml);
	CString GetFavUrl(int iFavId);
	CString GetFavHashCode(void);
	void GetFavorites(std::vector<CAglocoFavoritesItem> &vec);
	

private:
	BOOL LoadFavorites(void);

	std::vector<CAglocoFavoritesItem> m_vecFavorites;
};
