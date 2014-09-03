#include "..\StdAfx.h"
#include "Registry.h"
#include "..\MyUtility.h"

namespace viewbar
{
	#define VB_REG_BASE				"SOFTWARE\\Agloco\\ViewBar\\"
	#define VB_REG_USER_BASE		"SOFTWARE\\Agloco\\ViewBar\\Users\\"

	#define VB_REG_SKIN				"\\Skin"
	#define VB_REG_DOCK				"\\Dock"
	#define VB_REG_USERNAME			"\\Username"
	#define VB_REG_PASSWORD			"\\Password"
	#define VB_REG_REMEMBER_FLAG	"\\RememberFlag"
	#define VB_REG_LANG				"\\Lang"
	#define VB_REG_SEARCH_ENGINE	"\\SearchEngine"
	#define VB_REG_LASTTIME_AFF		"\\AffLastTime"
	#define VB_REG_UPDATE			"\\Update"
	#define VB_REG_FAVORITES_IE		"\\FavIE"
	#define VB_REG_FAVORITES_FF		"\\FavFF"

	#define VB_REG_LASTREMEMBER_USER				"LastRememberUser"

	#define VB_REG_AUTO_START		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\ViewBar"
	#define VB_REG_PROXY_IP			"ProxyIp"
	#define VB_REG_PROXY_PORT		"ProxyPort"
	#define VB_REG_PROXY_USER		"ProxyUser"
	#define VB_REG_PROXY_PASS		"ProxyPassword"
	#define VB_REG_PROXY_ENABLE		"ProxyEnable"
	#define VB_REG_WEBSERVICE		"WS"
	#define VB_REG_WEBSERVICE_PATH	"WSPATH"

	CRegistry::CRegistry(const CString &username) : m_strUsername(username), m_pUsername(NULL)
	{
		m_pUsername = CMyUtility::GetAnsiString(m_strUsername);
	}

	CRegistry::CRegistry():m_pUsername(NULL)
	{

	}

	CRegistry::~CRegistry(void)
	{
		if (m_pUsername)
			delete [] m_pUsername;
	}

	void CRegistry::SetKeyValue(KEY_ROOT base, std::string key, const CString &in)
	{
		char *pIn = CMyUtility::GetAnsiString(in);

		if (base == HK_CURRENT_USER)
		{
			registry_string<std::string> rs(
				key, HKEY_CURRENT_USER);
			rs = pIn;
		}
		else
		if (base == HK_LOCAL_MACHINE)
		{
			registry_string<std::string> rs(
				key, HKEY_LOCAL_MACHINE);
			rs = pIn;
		}
		

		delete [] pIn;
	}

	void CRegistry::SetKeyValue(KEY_ROOT base, std::string key, int in)
	{
		if (base == HK_CURRENT_USER)
		{
			registry_int<int> rs(
				key, HKEY_CURRENT_USER);
			rs = in;
		}
		else
		if (base == HK_LOCAL_MACHINE)
		{
			registry_int<int> rs(
				key, HKEY_LOCAL_MACHINE);
			rs = in;
		}

	}


	CString CRegistry::GetWebserviceURL()
	{
		return GetString(VB_REG_BASE, VB_REG_WEBSERVICE);
	}

	CString CRegistry::GetWebservicePath()
	{
		return GetString(VB_REG_BASE, VB_REG_WEBSERVICE_PATH);
	}

	CString CRegistry::GetDefaultSkin()
	{
		return GetString(VB_REG_USER_BASE, VB_REG_SKIN, true);
	}

	CString CRegistry::GetUsername()
	{
		return GetString(VB_REG_USER_BASE, VB_REG_USERNAME, true);
	}

	CString CRegistry::GetPassword()
	{
		return GetString(VB_REG_USER_BASE, VB_REG_PASSWORD, true);
	}

	CString CRegistry::GetLastRememberUser()
	{
		return GetString(VB_REG_BASE, VB_REG_LASTREMEMBER_USER);
	}

	void CRegistry::SetLastRememberUser(const CString &username)
	{
		SetString(VB_REG_BASE, VB_REG_LASTREMEMBER_USER, username);
	}

	int CRegistry::GetRememberFlag()
	{
		return GetInt(VB_REG_USER_BASE, VB_REG_REMEMBER_FLAG, true);
	}

	// get update flag, e.g. automatially, semi-automatically, manually
	int CRegistry::GetUpdateFlag()
	{
		return GetInt(VB_REG_USER_BASE, VB_REG_UPDATE, true);
	}

	// set update flag, e.g. automatially, semi-automatically, manually
	void CRegistry::SetUpdateFlag(int flag)
	{
		return SetInt(VB_REG_USER_BASE, VB_REG_UPDATE, flag, true);
	}

	int CRegistry::GetLastTimeAff()
	{
		return GetInt(VB_REG_USER_BASE, VB_REG_LASTTIME_AFF, true);
	}

	void CRegistry::SetLastTimeAff(int nTime)
	{
		return SetInt(VB_REG_USER_BASE, VB_REG_LASTTIME_AFF, nTime, true);
	}

	// favorites section
	bool CRegistry::IsFavoritesIEExists()
	{
		return IsKeyExists(VB_REG_USER_BASE, VB_REG_FAVORITES_IE, true);
	}

	bool CRegistry::IsFavoritesFFExists()
	{
		return IsKeyExists(VB_REG_USER_BASE, VB_REG_FAVORITES_FF, true);
	}

	void CRegistry::SetFavoritesIEFlag(bool in)
	{
		SetInt(VB_REG_USER_BASE, VB_REG_FAVORITES_IE, in, true);
	}

	bool CRegistry::GetFavoritesIEFlag()
	{
		return GetBool(VB_REG_USER_BASE, VB_REG_FAVORITES_IE, true);
	}

	void CRegistry::SetFavoritesFFFlag(bool in)
	{
		SetInt(VB_REG_USER_BASE, VB_REG_FAVORITES_FF, in, true);
	}

	bool CRegistry::GetFavoritesFFFlag()
	{
		return GetBool(VB_REG_USER_BASE, VB_REG_FAVORITES_FF, true);
	}


	// dock
	bool CRegistry::GetDockPosition()
	{
		return GetBool(VB_REG_USER_BASE, VB_REG_DOCK, true);
	}

	bool CRegistry::GetProxyEnableFlag()
	{
		return GetBool(VB_REG_BASE, VB_REG_PROXY_ENABLE);
	}

	bool CRegistry::GetAutoStarWithWindowsFlag()
	{
		std::string path;
		path = VB_REG_AUTO_START;
		registry_int<bool> rs(
			path, HKEY_LOCAL_MACHINE);

		return rs.exists();
	}

	CString CRegistry::GetDefaultLanguage()
	{
		return GetString(VB_REG_USER_BASE, VB_REG_LANG, true);
	}

	void CRegistry::SetDefaultLanguage(const CString&lang)
	{
		SetString(VB_REG_USER_BASE, VB_REG_LANG, lang, true);
	}

	void CRegistry::SetDefaultSearchEngine(int in)
	{
		SetInt(VB_REG_USER_BASE, VB_REG_SEARCH_ENGINE, in, true);
	}

	int CRegistry::GetDefaultSearchEngine()
	{
		return GetInt(VB_REG_USER_BASE, VB_REG_SEARCH_ENGINE, true);
	}

	void CRegistry::SetUsername(const CString &in)
	{
		SetString(VB_REG_USER_BASE, VB_REG_USERNAME, in, true);
	}

	void CRegistry::SetDefaultSkin(const CString &skin)
	{
		SetString(VB_REG_USER_BASE, VB_REG_SKIN, skin, true);
	}

	void CRegistry::SetPassword(const CString &in)
	{
		SetString(VB_REG_USER_BASE, VB_REG_PASSWORD, in, true);
	}

	void CRegistry::SetRememberFlag(int flag)
	{
		SetInt(VB_REG_USER_BASE, VB_REG_REMEMBER_FLAG, flag, true);
	}

	void CRegistry::SetDockPositionTop(bool pos)
	{
		SetBool(VB_REG_USER_BASE, VB_REG_DOCK, pos, true);
	}

	// set vb to run with windows
	void CRegistry::SetAutoStarWithWindows(const CString &in)
	{
		SetString(VB_REG_AUTO_START, "", in);
	}


	// proxy
	CString CRegistry::GetProxyIp()
	{
		return GetString(VB_REG_BASE, VB_REG_PROXY_IP);
	}

	void CRegistry::SetProxyIp(const CString &in)
	{
		SetString(VB_REG_BASE, VB_REG_PROXY_IP, in);
	}

	void CRegistry::SetProxyEnableFlag(bool in)
	{
		SetBool(VB_REG_BASE, VB_REG_PROXY_ENABLE, in);
	}

	CString CRegistry::GetProxyPort()
	{
		return GetString(VB_REG_BASE, VB_REG_PROXY_PORT);
	}

	CString CRegistry::GetProxyUser()
	{
		return GetString(VB_REG_BASE, VB_REG_PROXY_USER);
	}

	CString CRegistry::GetProxyPassword()
	{
		return GetString(VB_REG_BASE, VB_REG_PROXY_PASS);
	}

	void CRegistry::SetProxyPort(const CString &in)
	{
		SetString(VB_REG_BASE, VB_REG_PROXY_PORT, in);
	}

	void CRegistry::SetProxyUser(const CString &in)
	{
		SetString(VB_REG_BASE, VB_REG_PROXY_USER, in);
	}

	void CRegistry::SetProxyPassword(const CString &in)
	{
		SetString(VB_REG_BASE, VB_REG_PROXY_PASS, in);
	}

	//
	int CRegistry::GetInt(std::string base, std::string key, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_int<int> rs(
			path, HKEY_LOCAL_MACHINE);

		return rs;
	}

	//
	bool CRegistry::GetBool(std::string base, std::string key, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_int<bool> rs(
			path, HKEY_LOCAL_MACHINE);

		return rs;
	}

	//
	void CRegistry::SetBool(std::string base, std::string key, bool in, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_int<bool> rs(
			path, HKEY_LOCAL_MACHINE);

		rs = in;
	}

	void CRegistry::SetInt(std::string base, std::string key, int in, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_int<int> rs(
			path, HKEY_LOCAL_MACHINE);

		rs = in;
	}

	//
	CString CRegistry::GetString(std::string base, std::string key, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_string<std::string> rs(
			path, HKEY_LOCAL_MACHINE);

		std::string ret = rs;

		CString xret(ret.c_str());
		return xret;
	}

	//
	void CRegistry::SetString(std::string base, std::string key, const CString &in, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_string<std::string> rs(
			path, HKEY_LOCAL_MACHINE);

		char *pIn = CMyUtility::GetAnsiString(in);
		rs = pIn;
		delete [] pIn;
	}

	bool CRegistry::IsKeyExists(std::string base, std::string key, bool bPerUser /* = false */)
	{
		std::string path;
		if (bPerUser)
		{
			path = base + m_pUsername + key;
		}
		else
		{
			path = base + key;
		}

		registry_string<std::string> rs(
			path, HKEY_LOCAL_MACHINE);

		return rs.exists();
	}
}
