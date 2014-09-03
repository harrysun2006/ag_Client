#include "StdAfx.h"
#include "DockFacade.h"
#include "MyAppBar.h"

CDockFacade* CDockFacade::m_pInstance = NULL;

CDockFacade::CDockFacade(void):m_pAppBar(NULL)
{
}

CDockFacade::~CDockFacade(void)
{
}

void CDockFacade::Dock(int pos)
{
	if (thePrefs.IsViewBarVisible() == false)
		return;

	ASSERT(m_pAppBar);
	m_pAppBar->OnDock(pos);
}

void CDockFacade::SetTarget(CMyAppBar *pAppBar)
{
	m_pAppBar = pAppBar;
}

void CDockFacade::GetDockInfo()
{
	//const int ABE_TOP = 1;
	//const int ABE_BOTTOM = 3;
	if (thePrefs.IsViewBarVisible() == false)
		return;

	int x = m_pAppBar->GetViewBarPos();
	if (x == ABE_TOP)
		thePrefs.SetDockTop(true);
	else
		thePrefs.SetDockTop(false);
}

CDockFacade * CDockFacade::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CDockFacade();

	return m_pInstance;
}

BOOL CDockFacade::FreeInstance()
{
	delete m_pInstance;
	m_pInstance = 0;

	return TRUE;
}

