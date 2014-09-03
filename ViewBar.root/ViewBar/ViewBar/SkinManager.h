// ***************************************************************
//  CSkinManager   version:  1.0  date: 03/19/2007
//  -------------------------------------------------------------
//  Facade for skin management
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************

#pragma once

//#include "MyAppBar.h"
#include "MyUtility.h"
#include "Registry.h"

class CMyAppBar;
class CSkinManager
{
public:
	static CSkinManager *GetInstance();
	static BOOL FreeInstance();

private:
	CSkinManager();
public:
	void SetTarget(CMyAppBar *pAppBar);
	BOOL LoadSkins(const CString &strSkinName = _T("blue"));	// load skins
	BOOL SetBackgroundImage(CStatic *pStatic, const CString &image);
	void GetSkinList(CStringArray &skins);
	CString GetCurrentSkin() {return m_strCurrentSkin;}
	void RedrawButton();	// bug fix :(
	//BOOL GenerateAdFile(const CString &skinName);
	void SetIdle(BOOL bIsIdle = TRUE);
private:
	CMyAppBar *m_pParent;
	CString m_strCurrentSkin;

	static CSkinManager *m_pInstance;
};
