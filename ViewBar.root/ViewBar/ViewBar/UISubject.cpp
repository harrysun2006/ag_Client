#include "StdAfx.h"
#include "UISubject.h"


CUISubject * CUISubject::m_pInstance = NULL;

CUISubject * CUISubject::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CUISubject();
	return m_pInstance;
}

void CUISubject::FreeInstance()
{
	delete m_pInstance;
	m_pInstance = NULL;
}

CUISubject::CUISubject()
{

}

CUISubject::~CUISubject()
{

}

void CUISubject::Notify()
{
	std::list<IUIObserver*>::iterator it;

	for (it=m_listAllUI.begin(); it!=m_listAllUI.end(); ++it)
	{
		IUIObserver *p = static_cast<IUIObserver*>(*it);
		if (p)
		{
			p->UpdateUI();
		}
	}
}

void CUISubject::Attatch(IUIObserver* obs)
{
	m_listAllUI.push_back(obs);
}

void CUISubject::Detatch(IUIObserver* obs)
{
	m_listAllUI.remove(obs);
}