#include "StdAfx.h"
#include "MyCounter.h"

int CMyCounter::m_iCounter = -1;

CMyCounter::CMyCounter(void)
{
	m_iCounter++;
}

CMyCounter::~CMyCounter(void)
{
	m_iCounter--;
}

BOOL CMyCounter::IsEmpty()
{
	return m_iCounter == 0 ? TRUE : FALSE;
}
