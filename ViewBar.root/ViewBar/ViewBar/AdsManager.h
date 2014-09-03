// ***************************************************************
//  CAdsManager   version:  1.0  date: 04/04/2007
//  -------------------------------------------------------------
//  Facade for docking management
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "explorer1.h"
#include <list>

// class for ads
class CAds
{
public:
	CString m_strAdId;
	CString m_strAdCategory;
	CString m_strAdBanner;
	CString m_strAdType;
	CString m_strAdContent;
	CString m_strAdDescription;

	CAds(const CAds&src);
	CAds();
};

class CAdsManager
{
	
public:

private:

	// class for keyword
	class CAdsKeyword
	{
	public:
		CString m_strKeywordId;
		CString m_strKeyword;
	};

	// class for relationship between ads and keywords
	class CAdsMatrix
	{
	public:
		CString m_strKeywordId;
		CString m_strAdId;
		CString m_strWeight;
	};

	// ads parser class
	class CPaserAds
	{
	public:
		CPaserAds(const CString &xml);
		~CPaserAds();

		BOOL IsDownloaded();

		void GetAdsList(std::list<CAds*> &list);
		void GetKeywordsList(std::list<CAdsKeyword*> &list);
		void GetMatrixList(std::list<CAdsMatrix*> &list);

		
	private:
		void Parse();

	private:
		CString m_strXML;
		BOOL m_bDownloadSuccess;

		std::list<CAds*>		m_listAds;
		std::list<CAdsKeyword*> m_listKeywords;
		std::list<CAdsMatrix*>	m_listAdMatrix;
	};

	CAdsManager(void);
	~CAdsManager(void);

public:
	static CAdsManager * GetInstance();
	static void FreeInstance();

	void AttachControl(CExplorer1 *pIE);		//
	BOOL GetAds(const CString &strAdKeyword);	// get ads
	BOOL FindAds(const CString &strAdKeyword);	// query ads from local database
	void ShowAds(void);
	CString GetWantedAds() {return m_strWantedAds;}
	void DownloadAdTemplate(void);				// start thread
	void SendSearchKeyword(const CString strKeyword);
	void SwitchAdTemplate(void);

	void LoadJavaScript();
	CString GetJavaScript();

	void SetTemplateName(const CString strTemplateName);
	CString GetTemplateName(void);

	void SetJavaScriptName(const CString strJavaScriptName);
	CString GetJavaScriptName(void);

private:
	BOOL DownloadAds(const CString &strKeywords);

	// ads
	BOOL SaveAds(std::list<CAds*> &list);
	void CopyAds(std::list<CAds*> &list);
	void DestroyCurrentAds();

	// ads keywords
	BOOL SaveKeywords(std::list<CAdsKeyword*> &list);

	// ads matrix
	BOOL SaveMatrix(std::list<CAdsMatrix*> &list);
      
	// generate ads files
	BOOL GenerateAdFile(const CString &skinName);

	// Get random ads
	BOOL GetRandomAds();
	
	// thread for downloading ads
	static UINT ThreadDownloadAds(LPVOID pParam);
        
	// thread for downloading ad template
	static UINT ThreadDownloadAdTemplate(LPVOID pParam);

	// thread for sending search keyword
	static UINT ThreadSendSearchKeyword(LPVOID pParam);

	CString GetAdTemplateLang(void);
	void CopyAdTemplate(void);
	void UseDefaultTemplate(void);

	// load default ads
	BOOL LoadDefaultAds(CString &ads);

private:
	static CAdsManager *m_pInstance;
	CExplorer1 *m_pIE;
	CWinThread *m_pAdsThread;
	CString m_strWantedAds;			// if user searches 'auto', then all auto-related ads are 'wanted-ads'
	
	std::list<CAds*>			m_listCurrentAds;
	std::list<CAdsKeyword*>		m_listCurrentKeywords;
	std::list<CAdsMatrix*>		m_listCurrentMatrix;

	CString m_strTemplateName;
	CString m_strJavaScriptName;

	CString m_strScript;

	CRITICAL_SECTION m_CriticalSection;
};
