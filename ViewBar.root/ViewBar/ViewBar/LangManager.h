// ***************************************************************
//  CLangManager   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  A helper class for i18n
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "ui/lang/MultiLanguage.h"
#include "UISubject.h"

namespace viewbar
{

	class CLangManager
	{
	private:
		CLangManager(void);
		~CLangManager(void);

	public:
		static CLangManager *GetInstance();
		static BOOL FreeInstance();

		void SelectLang(const CString &lang);
		void GetLangList(CStringArray &langs);
		CString GetCurrentLang();

		void GetDefaultLanguage();

	private:
		CString m_strCurrentLang;

		static CLangManager *m_pInstance;
	};
}