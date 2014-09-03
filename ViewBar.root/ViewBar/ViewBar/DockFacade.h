// ***************************************************************
//  CDockFacade   version:  1.0  date: 03/19/2007
//  -------------------------------------------------------------
//  Facade for docking management
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

class CMyAppBar;
class CDockFacade
{
public:
	static CDockFacade *GetInstance();
	static BOOL FreeInstance();

	void SetTarget(CMyAppBar *pAppBar);
	void Dock(int pos);
	void GetDockInfo();

private:
	CDockFacade(void);
	~CDockFacade(void);

	static CDockFacade *m_pInstance;
	CMyAppBar *m_pAppBar;
};
