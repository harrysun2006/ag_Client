#pragma once

typedef struct tagAFFILIATE
{
	int id;
	CString affiliate;
} AFFILIATE, *PAFFILIATE;
typedef CList<PAFFILIATE> AFFILIATELIST;

class CDataStore
{
private:
	CDataStore(void);
	~CDataStore(void);
public:
	static CDataStore* GetInstance();
	static void FreeInstance();
public:
	AFFILIATELIST* GetAffiliateList(){return &m_listAffiliate;};
	void RemoveAllAffiliates();
private:
	static CDataStore* m_pInstance;
private:
	AFFILIATELIST m_listAffiliate;
};
