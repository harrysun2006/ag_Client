#include "StdAfx.h"
#include "Preferences.h"
#include "Registry.h"
#include "utility/des/MyAes.h"

CPreferences thePrefs;

int		CPreferences::m_iDefaultSearchEngine;
CPreferences::REMEMBER_FLAG	CPreferences::m_iRememberFlag;
CPreferences::UPDATE_FLAG	CPreferences::m_iUpdateFlag = CPreferences::UPDATE_AUTO;
CString	CPreferences::m_strUsername;
CString CPreferences::m_strPassword;
int		CPreferences::m_iAppbarPosition;
CRect	CPreferences::m_rtLogo = CRect(0, 0, 69, 30);
CRect	CPreferences::m_rtAds = CRect(0, 0, 200, 30);
CRect	CPreferences::m_rtSearch = CRect(0, 0, 141, 30);
CRect	CPreferences::m_rtMisc = CRect(0, 0, 121, 30);
CString	CPreferences::m_strDefaultLanguage = _T("English");
bool	CPreferences::m_bIsDockTop = false;
CString	CPreferences::m_strDefaultSkin = _T("blue");
CString CPreferences::m_strProxyIp;		// proxy
CString CPreferences::m_strProxyPort;
CString CPreferences::m_strProxyUsername;
CString CPreferences::m_strProxyPassword;
bool	CPreferences::m_bIsProxyEnable = false;
CString	CPreferences::m_strWebServiceURL = _T("viewbar.agloco.com:443");	// ip:port
CString CPreferences::m_strWebServicePath = _T("servlet");		// /servlet
int		CPreferences::m_iMemberId = 0;
int		CPreferences::m_iServerTime = 0;
bool	CPreferences::m_bIsViewBarVisible = false;
bool	CPreferences::m_bIsUserLoggedIn = false;
CString	CPreferences::m_strVersion = _T("win32.1.05");
int		CPreferences::m_nLastTimeAff = 0;
bool	CPreferences::m_bShowIEFavorites = true;
bool	CPreferences::m_bShowFFFavorites = true;
CString	CPreferences::m_strDefaultBrowserName = _T("IE");
int		CPreferences::m_nNextSendTime = 120;
BOOL	CPreferences::m_bNetOK = TRUE;
bool	CPreferences::m_bWarningBeforeQuit = true;
bool	CPreferences::m_bForceUpdate = false;
CStringArray CPreferences::m_arrayUrlFilter;

#define ENCRYPTION_KEY _T("googlestop")


namespace viewbar
{

	CPreferences::CPreferences(void)
	{
	}

	CPreferences::~CPreferences(void)
	{
	}

	// general preferences
	void CPreferences::LoadConfigFromRegistry()
	{
		CRegistry reg;
		CString strLastRememberUser = reg.GetLastRememberUser();
		LoadConfigFromRegistry(strLastRememberUser);

		// load proxy settings
		m_strProxyIp		= reg.GetProxyIp();
		m_strProxyPort		= reg.GetProxyPort();
		m_strProxyUsername	= reg.GetProxyUser();
		m_strProxyPassword	= reg.GetProxyPassword();
		m_bIsProxyEnable	= reg.GetProxyEnableFlag();
	
		// webservice url
		CString ws;
		ws	= reg.GetWebserviceURL();
		if (ws != _T(""))
			m_strWebServiceURL = ws;

		// webservice path
		CString path = reg.GetWebservicePath();
		if (path != _T(""))
			m_strWebServicePath = path;
	}

	// preferences per user
	void CPreferences::LoadConfigFromRegistry(const CString &user)
	{
		CRegistry reg2(user);

		/// user name
		m_strUsername	= reg2.GetUsername();

		/// password
		// if password is empty, load password from registry
		// 'cause password in registry is encrypted, so it should
		// be decrypted first
		if (m_strPassword == _T(""))
		{
			m_strPassword	= reg2.GetPassword();
			utility::CMyCrypt des;
			CString out;
			des.Decrypt(ENCRYPTION_KEY, m_strPassword, out);
			m_strPassword = out;
		}
		

		/// remember flag
		m_iRememberFlag	= CPreferences::NOT_REMEMBER;
		int tmp = reg2.GetRememberFlag();
		if (tmp == 0)
			m_iRememberFlag = CPreferences::NOT_REMEMBER;
		else
		if (tmp == 1)
			m_iRememberFlag = CPreferences::REMEMBER_ME;
		else
		if (tmp == 2)
			m_iRememberFlag = CPreferences::REMEMBER_ALL;

		/// Auto update flag
		int iUpdateFlag = 0;
		iUpdateFlag = reg2.GetUpdateFlag();
		switch(iUpdateFlag)
		{
		case 0:
			m_iUpdateFlag = CPreferences::UPDATE_AUTO;
			break;
		case 1:
			m_iUpdateFlag = CPreferences::UPDATE_SEMIAUTO;
			break;
		case 2:
			m_iUpdateFlag = CPreferences::UPDATE_MANNUALLY;
			break;
		default:
			break;
		}


		/// default language
		CString strLang = reg2.GetDefaultLanguage();
		if (strLang != _T(""))
			m_strDefaultLanguage = strLang;

		/// default skin
		CString strDefaultSkin	= reg2.GetDefaultSkin();

		// if default skin is not set, load the default skin 'Green'
		if (strDefaultSkin != _T("")) m_strDefaultSkin = strDefaultSkin;

		/// dock
		m_bIsDockTop			= reg2.GetDockPosition();

		/// search engine
		m_iDefaultSearchEngine	= reg2.GetDefaultSearchEngine();

		m_nLastTimeAff			= reg2.GetLastTimeAff();

		/// favorites, IE, Firefox
		if(reg2.IsFavoritesIEExists())
		{
			m_bShowIEFavorites = reg2.GetFavoritesIEFlag();
		}

		if (reg2.IsFavoritesFFExists())
		{
			m_bShowFFFavorites = reg2.GetFavoritesFFFlag();
		}

		// flag for warning before quit
		if (reg2.IsWarningFlagExists())
		{
			m_bWarningBeforeQuit = reg2.GetWarningBeforeQuitFlag();
		}

	}

	void CPreferences::SaveConfigToRegistry()
	{
		CRegistry reg(m_strUsername);
		reg.SetUsername(m_strUsername);

		utility::CMyCrypt aes;
		CString encrypedPassword;
		aes.Encrypt(ENCRYPTION_KEY, m_strPassword, encrypedPassword);
		reg.SetPassword(encrypedPassword);
		reg.SetRememberFlag(m_iRememberFlag);
		reg.SetUpdateFlag(m_iUpdateFlag);		// update flag
		reg.SetDefaultSkin(m_strDefaultSkin);
		reg.SetDockPositionTop(m_bIsDockTop);
		reg.SetDefaultSearchEngine(m_iDefaultSearchEngine);
		reg.SetLastTimeAff(m_nLastTimeAff);	
		reg.SetFavoritesIEFlag(m_bShowIEFavorites);		// favorites for IE
		reg.SetFavoritesFFFlag(m_bShowFFFavorites);		// favorites for FireFox
		reg.SetWarningBeforeQuitFlag(m_bWarningBeforeQuit);

		{
			CRegistry reg;
			reg.SetLastRememberUser(m_strUsername);
			
			// proxy settings
			reg.SetProxyIp(m_strProxyIp);
			reg.SetProxyPort(m_strProxyPort);
			reg.SetProxyUser(m_strProxyUsername);
			reg.SetProxyPassword(m_strProxyPassword);
			reg.SetProxyEnableFlag(m_bIsProxyEnable);
		}
	}

	void CPreferences::SetUrlFilter(CStringArray &filter)
	{
		m_arrayUrlFilter.RemoveAll();

		for (int i=0; i<filter.GetSize(); ++i)
		{
			m_arrayUrlFilter.Add(filter.GetAt(i));
		}
	}
};