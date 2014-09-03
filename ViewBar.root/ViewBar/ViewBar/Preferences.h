// ***************************************************************
//  CPreferences   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  This class aims to record all preferences of user
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "Resource.h"
namespace viewbar
{
	class CPreferences
	{
	public:
		enum REMEMBER_FLAG{NOT_REMEMBER, REMEMBER_ME, REMEMBER_ALL};
		enum UPDATE_FLAG{UPDATE_AUTO, UPDATE_SEMIAUTO, UPDATE_MANNUALLY};

		CPreferences();
		~CPreferences();

		// getter
		static int		GetAppbarPostion()				{return m_iAppbarPosition;}
		static int		GetDefaultSearchEngine()		{return m_iDefaultSearchEngine;}
		static REMEMBER_FLAG	GetRememberFlag()		{return m_iRememberFlag;}
		static UPDATE_FLAG		GetUpdateFlag()			{return m_iUpdateFlag;}
		static CString	GetUsername()					{return m_strUsername;}
		static CString	GetPassword()					{return m_strPassword;}
		static CRect	GetLogoRect()					{return m_rtLogo;}
		static CRect	GetAdsRect()					{return m_rtAds;}
		static CRect	GetSearchRect()					{return m_rtSearch;}
		static CRect	GetMiscRect()					{return m_rtMisc;}
		static CString	GetDefaultLanguage()			{return m_strDefaultLanguage;}
		static bool		IsDockTop()						{return m_bIsDockTop;}
		static CString	GetDefaultSkin()				{return m_strDefaultSkin;}
		static CString	GetProxyIp()					{return m_strProxyIp;}				// proxy settings
		static CString	GetProxyPort()					{return m_strProxyPort;}
		static CString	GetProxyUser()					{return m_strProxyUsername;}
		static CString	GetProxyPassword()				{return m_strProxyPassword;}
		static bool		IsProxyEnable()					{return m_bIsProxyEnable;}
		static CString	GetWebServiceURL()				{return m_strWebServiceURL;}
		static CString	GetWebServicePath()				{return m_strWebServicePath;}
		static int		GetMemberId()					{return m_iMemberId;}
		static int		GetServerTime()					{return m_iServerTime;}
		static bool		IsShowIEFavorites()				{return m_bShowIEFavorites;}
		static bool		IsShowFFFavorites()				{return m_bShowFFFavorites;}

		static bool		IsViewBarVisible()				{return m_bIsViewBarVisible;}
		static bool		IsUserLoggedIn()				{return m_bIsUserLoggedIn;}
		static CString	GetVersion()					{return m_strVersion;}
		static int		GetLastTimeAff()				{return m_nLastTimeAff;}
		static CString	GetDefaultBrowserName()			{return m_strDefaultBrowserName;}
		static int		GetNextSendTime()				{return m_nNextSendTime;};
		static BOOL		GetNetState()					{return m_bNetOK;};
		static bool		IsWarningBeforeQuitAllowed()	{return m_bWarningBeforeQuit;}
		static bool		IsForceUpdate()					{return m_bForceUpdate;}
		static CStringArray& GetUrlFilter()				{return m_arrayUrlFilter;}
		


		// setter
		static void	SetAppbarPostion(int in)		{m_iAppbarPosition = in;}
		static void	SetDefaultSearchEngine(int in)	{m_iDefaultSearchEngine = in;}
		static void SetRememberFlag(REMEMBER_FLAG in)			{m_iRememberFlag = in;}
		static void SetUpdateFlag(UPDATE_FLAG in)				{m_iUpdateFlag = in;}
		static void SetUsername(CString in)			{m_strUsername = in;}
		static void SetPassword(CString in)			{m_strPassword = in;}
		static void SetLogoRect(CRect in)			{m_rtLogo = in;}
		static void SetAdsRect(CRect in)			{m_rtAds = in;}
		static void SetSearchRect(CRect in)			{m_rtSearch = in;}
		static void SetMiscRect(CRect in)			{m_rtMisc = in;}
		static void SetDefaultLanguage(CString in)	{m_strDefaultLanguage = in;}
		static void SetDockTop(bool in)				{m_bIsDockTop = in;}
		static void SetDefaultSkin(CString in)		{m_strDefaultSkin = in;}	
		static void SetProxyIp(CString in)			{m_strProxyIp = in;}	// proxy settings
		static void SetProxyPort(CString in)		{m_strProxyPort = in;}
		static void SetProxyUser(CString in)		{m_strProxyUsername = in;}
		static void SetProxyPassword(CString in)	{m_strProxyPassword = in;}
		static void SetProxyEnable(bool in)			{m_bIsProxyEnable = in;}
		static void SetMemberId(int in)				{m_iMemberId = in;}
		static void SetServerTime(int in)			{m_iServerTime = in;}
		static void SetFavoritesIE(bool in)			{m_bShowIEFavorites = in;}
		static void SetFavoritesFF(bool in)			{m_bShowFFFavorites = in;}
		static void SetDefaultBrowserName(CString in)	{m_strDefaultBrowserName = in;}
		static void SetWarningBeforeQuitFlag(bool in)	{m_bWarningBeforeQuit = in;}
		static void SetForceUpdate(bool in)			{m_bForceUpdate = in;}

		static void ShowViewBar(bool in)			{m_bIsViewBarVisible = in;}
		static void UserLogin()						{m_bIsUserLoggedIn = true;}
		static void UserLogout()					{m_bIsUserLoggedIn = false;}
		static void SetLastTimeAff(int nLastTimeAff){m_nLastTimeAff = nLastTimeAff;};
		static void SetNextSendTime(int nNextSendTime) {m_nNextSendTime = nNextSendTime;};
		static void SetUrlFilter(CStringArray &filter);

		static void SetNetState(BOOL bOK)
		{
#ifdef MY_DEBUG_STRING_OUTPUT
			CString strOut;
			strOut.Format(_T("Network State %d"),bOK);
			OutputDebugString(strOut);
#endif
			InterlockedExchange((LONG*)&m_bNetOK,bOK);};

		// load config from registry
		// this method should and must be called at the startup of this application
		static void LoadConfigFromRegistry();
		static void LoadConfigFromRegistry(const CString &user);

		// save config to registry
		static void SaveConfigToRegistry();



	private:
		// field
		static int		m_iAppbarPosition;			// 0 means bottom and 1 means top
		static int		m_iDefaultSearchEngine;		// default search engine id
		static REMEMBER_FLAG	m_iRememberFlag;
		static UPDATE_FLAG		m_iUpdateFlag;		// auto update flag
		static CString	m_strUsername;
		static CString	m_strPassword;
		static CRect	m_rtLogo;
		static CRect	m_rtAds;
		static CRect	m_rtSearch;
		static CRect	m_rtMisc;
		static CString	m_strDefaultLanguage;
		static bool		m_bIsDockTop;
		static CString	m_strDefaultSkin;
		static CString	m_strProxyIp;				// proxy settings
		static CString	m_strProxyPort;
		static CString	m_strProxyUsername;
		static CString	m_strProxyPassword;
		static bool		m_bIsProxyEnable;
		static CString	m_strWebServiceURL;
		static CString	m_strWebServicePath;
		static int		m_iMemberId;
		static int		m_iServerTime;
		static bool		m_bShowIEFavorites;			// show IE favorites
		static bool		m_bShowFFFavorites;			// show FireFox favorites

		static bool		m_bIsViewBarVisible;			// if the desktop bar is visible
		static bool		m_bIsUserLoggedIn;
		static CString	m_strVersion;
		static int		m_nLastTimeAff;
		static CString	m_strDefaultBrowserName;
		static int		m_nNextSendTime;
		static BOOL		m_bNetOK;
		static bool		m_bWarningBeforeQuit;
		static bool		m_bForceUpdate;
		static CStringArray m_arrayUrlFilter;
	};

	extern CPreferences thePrefs;
};