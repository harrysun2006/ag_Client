#pragma once

class CKeywordStore
{
public:
	static CKeywordStore* GetInstance();
	static BOOL FreeInstance();

	void Push(CString keyword);

	CStringList* GetStringList(){return &m_listKW;};
	~CKeywordStore(void);
private:
	CKeywordStore(void);
private:
	static CKeywordStore *m_pInstance;
	CStringList m_listKW;
};
