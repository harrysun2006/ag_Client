#include "StdAfx.h"
#include "LangManager.h"
#include "MyUtility.h"
#include "Registry.h"
#include "AdsManager.h"

namespace viewbar
{

	CLangManager * CLangManager::m_pInstance = NULL;
	CLangManager::CLangManager(void)
	{
	}

	CLangManager::~CLangManager(void)
	{
	}

	CLangManager * CLangManager::GetInstance()
	{
		if (m_pInstance == NULL)
			m_pInstance = new CLangManager();

		return m_pInstance;
	}

	BOOL CLangManager::FreeInstance()
	{
		delete m_pInstance;
		m_pInstance = 0;

		return TRUE;
	}

	void CLangManager::SelectLang(const CString &lang)
	{
		CString dir = CMyUtility::GetCurDir();
		CString file = dir +  _T("\\langs\\") + lang + _T(".dll");

		HINSTANCE hDll = NULL;
		hDll = CMultiLanguage::LoadResourceDLL(file);

		if (hDll)
		{
			AfxSetResourceHandle(hDll);
		}

		viewbar::CUISubject::GetInstance()->Notify();

		// save config
		thePrefs.SetDefaultLanguage(lang);
		CRegistry reg(thePrefs.GetUsername());
		reg.SetDefaultLanguage(lang);

		m_strCurrentLang = lang;

		// Switch ads template
		CAdsManager::GetInstance()->SwitchAdTemplate();
	}

	CString CLangManager::GetCurrentLang()
	{
		return m_strCurrentLang;
	}

	void CLangManager::GetLangList(CStringArray &langs)
	{

		CMyUtility::GetFileList(langs, _T("langs"));

		for (int i=0; i<langs.GetCount(); ++i)
		{
			CString temp = langs.GetAt(i);
			temp = temp.Left(temp.GetLength() - 4);
			langs.SetAt(i, temp);
		}

	}


	void CLangManager::GetDefaultLanguage()
	{
		CString lang = thePrefs.GetDefaultLanguage();
		CString dir = CMyUtility::GetCurDir();
		CString file = dir +  _T("\\langs\\") + lang + _T(".dll");

		HINSTANCE hDll = NULL;
		hDll = CMultiLanguage::LoadResourceDLL(file);

		if (hDll)
		{
			AfxSetResourceHandle(hDll);
		}

		m_strCurrentLang = lang;
	}
}
