#pragma once

class CMyCounter
{
public:
	CMyCounter(void);
	~CMyCounter(void);

	BOOL IsEmpty();

private:
	static int m_iCounter;

};
