// ***************************************************************
//  CRegistry   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  A helper class for registry operation
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "registry_value.h"
namespace viewbar
{

	class CRegistry
	{
	public:
		enum KEY_ROOT{HK_CURRENT_USER, HK_LOCAL_MACHINE};
	public:
		// config from users' key
		CRegistry(const CString &username);

		// config from general key
		CRegistry();
	public:
		~CRegistry(void);

		CString GetUsername();
		CString GetPassword();
		int		GetRememberFlag();
		int		GetUpdateFlag();
		CString	GetDefaultLanguage();
		CString GetLastRememberUser();
		CString	GetDefaultSkin();
		bool	GetDockPosition();
		bool	GetAutoStarWithWindowsFlag();
		CString	GetProxyIp();		// proxy
		CString	GetProxyPort();
		CString	GetProxyUser();
		CString	GetProxyPassword();
		int		GetDefaultSearchEngine();
		bool	GetProxyEnableFlag();
		CString	GetWebserviceURL();
		CString GetWebservicePath();
		int		GetLastTimeAff();
		bool	GetFavoritesIEFlag();
		bool	GetFavoritesFFFlag();
		bool	IsFavoritesIEExists();	//
		bool	IsFavoritesFFExists();


		void	SetUsername(const CString &username);
		void	SetPassword(const CString &password);
		void	SetRememberFlag(int flag);
		void	SetUpdateFlag(int flag);
		void	SetDefaultLanguage(const CString&lang);
		void	SetLastRememberUser(const CString &username);
		void	SetDefaultSkin(const CString &skin);
		void	SetDockPositionTop(bool pos);
		void	SetAutoStarWithWindows(const CString &path);
		void	SetProxyIp(const CString &in);	// proxy
		void	SetProxyPort(const CString &in);
		void	SetProxyUser(const CString &in);
		void	SetProxyPassword(const CString &in);
		void	SetDefaultSearchEngine(int in);
		void	SetProxyEnableFlag(bool in);
		void	SetLastTimeAff(int nLastTimeAff);
		void	SetFavoritesIEFlag(bool in);
		void	SetFavoritesFFFlag(bool in);

		void	SetKeyValue(KEY_ROOT base, std::string key, const CString &in);
		void	SetKeyValue(KEY_ROOT base, std::string key, int in);

	private:
		int		GetInt(std::string base, std::string key, bool bPerUser = false);
		bool	GetBool(std::string base, std::string key, bool bPerUser = false);
		CString	GetString(std::string base, std::string key, bool bPerUser = false);

		void	SetInt(std::string base, std::string key, int in, bool bPerUser = false);
		void	SetBool(std::string base, std::string key, bool in, bool bPerUser = false);
		void	SetString(std::string base, std::string key, const CString &in, bool bPerUser = false);

		bool	IsKeyExists(std::string base, std::string key, bool bPerUser = false);

		CString m_strUsername;
		char *m_pUsername;
		//CString m_strLastRememberUser;

	};
}
