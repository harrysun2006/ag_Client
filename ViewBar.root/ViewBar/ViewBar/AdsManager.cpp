#include "StdAfx.h"
#include "AdsManager.h"
#include "MyUtility.h"
#include "SkinManager.h"
#include "XAdsUpdate.h"
#include "utility/XmlNodeWrapper.h"
#include "CppSQLiteManager.h"
#include "utility/http/URLEncode.h"
#include "resource.h"
#include "Downloader.h"

CAdsManager * CAdsManager::m_pInstance = NULL;

using namespace http;
using namespace utility;
///////////////////////////////////////////////////
CAds::CAds(const CAds &src)
{
	m_strAdId		= src.m_strAdId;
	m_strAdCategory	= src.m_strAdCategory;
	m_strAdType		= src.m_strAdType;
	m_strAdContent	= src.m_strAdContent;
	m_strAdDescription = src.m_strAdDescription;
}

CAds::CAds()
{

}


///////////////////////////////////////////////////
CAdsManager::CPaserAds::CPaserAds(const CString &xml) :
	m_strXML(xml),
	m_bDownloadSuccess(FALSE)
{
	Parse();
}

BOOL CAdsManager::CPaserAds::IsDownloaded()
{
	return m_bDownloadSuccess;
}

void CAdsManager::CPaserAds::Parse()
{
	CXmlDocumentWrapper xml;
	xml.LoadXML(m_strXML);

	CXmlNodeWrapper theNode(xml.AsNode());
	CString strResult = theNode.GetValue(_T("result"));
	if (strResult != _VB_SUCCESS)
	{
		m_bDownloadSuccess = FALSE;
		return;
	}

	// parse ads
	CXmlNodeWrapper adsNode(theNode.GetNode(_T("advertises")));
	CXmlNodeListWrapper ads(adsNode.FindNodes(_T("advertise")));
	for (int i=0; i<ads.Count(); ++i)
	{
		CXmlNodeWrapper item = ads.Node(i);
		CAds *pAd = new CAds;
		pAd->m_strAdId			= item.GetValue(_T("adId"));
		pAd->m_strAdCategory	= item.GetValue(_T("adCategory"));
		pAd->m_strAdType		= item.GetValue(_T("adType"));
		pAd->m_strAdContent		= item.GetValue(_T("adContent"));
		pAd->m_strAdBanner		= item.GetValue(_T("adBanner"));
		pAd->m_strAdDescription = item.GetValue(_T("adDescription"));
		m_listAds.push_back(pAd);
	}
	if (ads.Count() > 0)
		m_bDownloadSuccess = TRUE;

	// parse keywords
	CXmlNodeWrapper keywordsNode(theNode.GetNode(_T("keywordsList")));
	CXmlNodeListWrapper keywords(keywordsNode.FindNodes(_T("keyword")));
	for (int i=0; i<keywords.Count(); ++i)
	{
		CXmlNodeWrapper item = keywords.Node(i);
		CAdsKeyword *pKeyword = new CAdsKeyword;
		pKeyword->m_strKeywordId	= item.GetValue(_T("adKeywordId"));
		pKeyword->m_strKeyword		= item.GetValue(_T("content"));
		m_listKeywords.push_back(pKeyword);
	}

	// parse matrix
	CXmlNodeWrapper matrixNode(theNode.GetNode(_T("matrixList")));
	CXmlNodeListWrapper matrix(matrixNode.FindNodes(_T("matrix")));
	for (int i=0; i<matrix.Count(); ++i)
	{
		CXmlNodeWrapper item = matrix.Node(i);
		CAdsMatrix *pMatrix = new CAdsMatrix;
		pMatrix->m_strAdId		= item.GetValue(_T("adId"));
		pMatrix->m_strKeywordId	= item.GetValue(_T("adKeywordId"));
		pMatrix->m_strWeight	= item.GetValue(_T("weight"));
		m_listAdMatrix.push_back(pMatrix);
	}
}

// release resources
CAdsManager::CPaserAds::~CPaserAds()
{
	// destroy ads list
	std::list<CAds*>::iterator itAd;
	for (itAd=m_listAds.begin(); itAd != m_listAds.end(); ++itAd)
	{
		CAds *pAd = static_cast<CAds*>(*itAd);
		if (pAd)
			delete pAd;
	}

	// destroy keywords list
	std::list<CAdsKeyword*>::iterator itKeyword;
	for (itKeyword=m_listKeywords.begin(); itKeyword != m_listKeywords.end(); ++itKeyword)
	{
		CAdsKeyword *pAd = static_cast<CAdsKeyword*>(*itKeyword);
		if (pAd)
			delete pAd;
	}

	// destroy matrix list
	std::list<CAdsMatrix*>::iterator itMatrix;
	for (itMatrix=m_listAdMatrix.begin(); itMatrix != m_listAdMatrix.end(); ++itMatrix)
	{
		CAdsMatrix *pAd = static_cast<CAdsMatrix*>(*itMatrix);
		if (pAd)
			delete pAd;
	}
}

void CAdsManager::CPaserAds::GetAdsList(std::list<CAds*> &list)
{
	std::list<CAds*>::iterator it;
	for (it=m_listAds.begin(); it != m_listAds.end(); ++it)
	{
		CAds *pAd = static_cast<CAds*>(*it);
		if (pAd)
			list.push_back(pAd);
	}
}

void CAdsManager::CPaserAds::GetKeywordsList(std::list<CAdsKeyword*> &list)
{
	std::list<CAdsKeyword*>::iterator it;
	for (it=m_listKeywords.begin(); it != m_listKeywords.end(); ++it)
	{
		CAdsKeyword *pAd = static_cast<CAdsKeyword*>(*it);
		if (pAd)
			list.push_back(pAd);
	}
}

void CAdsManager::CPaserAds::GetMatrixList(std::list<CAdsMatrix*> &list)
{
	std::list<CAdsMatrix*>::iterator it;
	for (it=m_listAdMatrix.begin(); it != m_listAdMatrix.end(); ++it)
	{
		CAdsMatrix *pAd = static_cast<CAdsMatrix*>(*it);
		if (pAd)
			list.push_back(pAd);
	}
}




///////////////////////////////////////////////////
/////////// Class CAdsManager  ////////////////////
///////////////////////////////////////////////////
CAdsManager::CAdsManager(void) : 
	m_pIE(NULL), m_pAdsThread(NULL), m_strWantedAds(_T(""))
{
	InitializeCriticalSection(&m_CriticalSection);
	// DownloadAdTemplate();
}

CAdsManager::~CAdsManager(void)
{
	DestroyCurrentAds();
	DeleteCriticalSection(&m_CriticalSection);
}

CAdsManager * CAdsManager::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CAdsManager;

	return m_pInstance;
}

void CAdsManager::FreeInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


// get ads from local db, if ads not available in local db
// then download ads from ad-server automatically
BOOL CAdsManager::GetAds(const CString &strAdKeyword)
{
	// replace ' with \'
	CString strParsedKeyword = strAdKeyword;

	strParsedKeyword.Replace(_T("'"), _T(""));
	strParsedKeyword.Replace(_T("\""), _T(""));

	// get ads from db
	// if exists show
	// else download
	if (FindAds(strParsedKeyword))
	{
		ShowAds();
	}
	else
	{
		DownloadAds(strAdKeyword);
	}
	
	return TRUE;
}

// download ads from ad-server
BOOL CAdsManager::DownloadAds(const CString &strKeywords)
{
	m_strWantedAds = strKeywords;

	CURLEncode x;
	USES_CONVERSION;
	CStringA strUTF8Keywords = x.URLEncode(strKeywords);
	m_strWantedAds = A2W(strUTF8Keywords);


	// start a threads
	m_pAdsThread = AfxBeginThread(ThreadDownloadAds, NULL);

	return TRUE;
}

// Send search keyword
void CAdsManager::SendSearchKeyword(const CString strKeyword)
{
	AfxBeginThread(ThreadSendSearchKeyword, (LPVOID)(LPCTSTR)strKeyword);
}

UINT CAdsManager::ThreadSendSearchKeyword(LPVOID pParam)
{
	CoInitialize(NULL);

	CString strKeyword = (LPCTSTR)pParam;

	CString response;
	CXAdsUpdate::DownloadAds(strKeyword, response);

	CoUninitialize();
	return 0;
}

void CAdsManager::AttachControl(CExplorer1 *pIE)
{
	m_pIE = pIE;
}

void CAdsManager::ShowAds(void)
{
	CString html;
	CString dir = CMyUtility::GetCurDir();
	html.Format(_T("%s\\ads\\ads.html"), dir);
	CString skin = CSkinManager::GetInstance()->GetCurrentSkin();
	GenerateAdFile(skin);
//	if (skin == _T("blue")) html.Format(_T("%s\\ads\\ads.html"), dir);
//	else html.Format(_T("%s\\ads\\ads1.html"), dir);
	m_pIE->Navigate(html, NULL, NULL, NULL, NULL);
}

BOOL CAdsManager::LoadDefaultAds(CString &ads)
{
	CString sql;

	sql.Format(_T("SELECT COUNT(*) AS TABLE_ROWS FROM %s WHERE AD_TYPE='def'"), _VB_TABLE_AD);
	int iRowNum = CCppSQLiteManager::GetInstance()->GetTableRowNumber(sql);

	if (iRowNum == 0)
	{
		return FALSE;
	}

	int iPerPage = 10;
	int iLimitBase = 0;
	if (iRowNum < iPerPage)
	{
		iLimitBase = 0;
	}
	else
	{
		int iRowNumBase = iRowNum - iPerPage;
		srand(time(NULL));
		iLimitBase = rand() % iRowNumBase;
	}

	sql.Format(_T("SELECT * FROM %s WHERE AD_TYPE='def' LIMIT %d, %d"), _VB_TABLE_AD, iLimitBase, iPerPage);

	DestroyCurrentAds();

	std::list<CAds*> listAds;
	CCppSQLiteManager::GetInstance()->GetAds(sql, listAds);
	if (listAds.size() > 0)
	{
		std::list<CAds*>::iterator it;
		for (it=listAds.begin(); it!=listAds.end(); ++it)
		{
			CAds *pAd = static_cast<CAds*> (*it);

			ads += _T("'");
			ads += pAd->m_strAdContent;
			ads += _T("',");
		}

		CString strAdScript = ads.Mid(1, ads.GetLength() - 3);
		ads = strAdScript;


		// free memory
		for (it=listAds.begin(); it!=listAds.end(); ++it)
		{
			CAds *pAd = static_cast<CAds*> (*it);
			if (pAd)
			{
				delete pAd;
			}
		}

		listAds.erase(listAds.begin(), listAds.end());


		return TRUE;
	}

	return FALSE;
}

BOOL CAdsManager::GenerateAdFile(const CString &skinName)
{
	EnterCriticalSection(&m_CriticalSection);

	CString dir;
	CString sql;
	dir = CMyUtility::GetCurDir();

	CString strAdTemplateFile	= dir + _T("\\ads\\ads-template.html");
	CString strAdFile			= dir + _T("\\ads\\ads.html");
	CString strBackground		= _T("..\\skins\\") + skinName + _T("\\ads.bmp");
	
	try
	{
		CString strAdsBody;
		strAdsBody.Empty();

		// read
		CStdioFile fileR(strAdTemplateFile, CFile::modeRead);

		CString line;
		while (fileR.ReadString(line))
		{
			strAdsBody = strAdsBody + line + _T("\r");
		}

		fileR.Close();

		// replace background placeholder
		strAdsBody.Replace(_T("{[_BACKGROUND_]}"), strBackground);

		//// generate body
		//std::list<CAds*>::iterator it;
		//CString ads;
		//for (it=m_listCurrentAds.begin(); it!=m_listCurrentAds.end(); ++it)
		//{
		//	CAds *pAd = static_cast<CAds*> (*it);
		//	ads = _T("<a href='");
		//	ads += pAd->m_strAdContent;
		//	ads += _T("' target='_blank'>");
		//	ads += pAd->m_strAdBanner;
		//	ads += _T("</a>");
		//}

		// generate body
		//sql.Format(_T("SELECT * FROM %s"), _VB_TABLE_AD);  // maybe there're something wrong
		//DestroyCurrentAds();     
		CCppSQLiteManager::GetInstance()->GetRandomAds(m_listCurrentAds); 
		//CCppSQLiteManager::GetInstance()->GetAds(sql,m_listCurrentAds);    //add with BB

		std::list<CAds*>::iterator it;
		CString ads;
		for (it=m_listCurrentAds.begin(); it!=m_listCurrentAds.end(); ++it)
		{
			
			
			CAds *pAd = static_cast<CAds*> (*it);
			if (pAd->m_strAdType == _T("def"))
			{
				ads += _T("'");
				ads += pAd->m_strAdContent;
				ads += _T("',");
			}
			else
			{
				ads = "";
				ads += _T("'    By AGLOCO     <a href=\"");
				ads += pAd->m_strAdContent;
				ads += _T("\" target=\"_blank\">");
				ads += pAd->m_strAdBanner;
				ads += _T("</a>");
				ads += pAd->m_strAdDescription;
				ads += pAd->m_strAdCategory;
				ads += _T("		win32.1.05");
				ads += _T("<br/>',");
			}
		}

		CString strAdScript = ads.Mid(1, ads.GetLength() - 3);
		ads = strAdScript;

		//if (ads.IsEmpty())
		//{
		//	ads =
		//		_T("'<iframe style=\"text-align: center\" align=\"center\" width=\"{[[_PAGE_WIDTH_]]}\" height=\"330\" frameborder=\"0\" scrolling=\"auto\" src=\"http://viewbar.agloco.com/ads/wikia.html\"></iframe>'");
		//}
		if (!ads.IsEmpty())
		{
			strAdsBody.Replace(_T("{[[_VIEWBAR_ADS_SEARCH_]]}"), ads);
		}
		
		// If there're some default ads in local database
		// then replace the placeholder with default ads
		CString strDefaultAds;
		if (LoadDefaultAds(strDefaultAds))
		{
			strAdsBody.Replace(_T("{[[_VIEWBAR_ADS_DEFAULT_]]}"), strDefaultAds);
		}

		CRect rtIE;
		m_pIE->GetClientRect(rtIE);
		CString strPageWidth;
		strPageWidth.Format(_T("%d"), rtIE.right);
		strAdsBody.Replace(_T("{[[_PAGE_WIDTH_]]}"), strPageWidth);



		CStringA strUFF8Body;
		strUFF8Body = CMyUtility::GetUTF8CStringA(strAdsBody);

		CFile fileW(strAdFile, CFile::modeCreate | CFile::modeWrite);
		fileW.Write(strUFF8Body, (strUFF8Body.GetLength()+1) * sizeof( char ));
		fileW.Close();

		// write
		//CStdioFile fileW(strAdFile, CFile::modeCreate | CFile::modeWrite);
		//fileW.WriteString(strUFF8Body);
		//fileW.Close();

	}
	catch (CException* e)
	{
		e->Delete();
	}


	LeaveCriticalSection(&m_CriticalSection);
	return TRUE;
}

BOOL CAdsManager::FindAds(const CString &strAdKeyword)
{
// 	select * from TableAd as ad
// 	where ad.AD_ID in
// 	(
// 		select x.AD_ID from TableAdKeyword as kw, TableMatrix as x
// 		where kw.KEYWORD_ID = x.KEYWORD_ID
// 		and kw.KEYWORD_CONTENT = 'bmw'
// 	)


	CString sql;
	sql.Format(_T("SELECT * FROM %s AS ad\
				  WHERE ad.AD_ID IN \
				  ( \
						SELECT x.AD_ID FROM %s as kw, %s as x\
						WHERE kw.KEYWORD_ID = x.KEYWORD_ID \
						AND kw.KEYWORD_CONTENT = \"%s\"\
				  ) \
				  "), _VB_TABLE_AD, _VB_TABLE_AD_KEYWORD, _VB_TABLE_MATRIX,
				  strAdKeyword);

	DestroyCurrentAds();
	CCppSQLiteManager::GetInstance()->GetAds(sql, m_listCurrentAds);
	if (m_listCurrentAds.size() > 0)
	{
		return TRUE;
	}

	return FALSE;
}

void CAdsManager::DownloadAdTemplate()
{
	AfxBeginThread(ThreadDownloadAdTemplate, (LPVOID)this);
}

UINT CAdsManager::ThreadDownloadAdTemplate(LPVOID pParam)
{
START:

	{
		CDownloader downloader;
		CString strResult;
		CString strURL;
		CStringArray arrayUrlFilter;

		// Download ads templates
		CString strLang = CAdsManager::GetInstance()->GetAdTemplateLang();
		// strURL.Format(_T("/ads/ads-template_%s.html"), strLang);
		strURL.Format(_T("/ads/%s_%s.html"), CAdsManager::GetInstance()->GetTemplateName(), strLang);

		CString strTemplate = _T("\\ads\\ads-template_%s.html");
		CString strTemplateFile;
		strTemplateFile.Format(strTemplate, strLang);

		BOOL bDownload = downloader.DownloadToFile(strURL, strTemplateFile, strResult, TRUE);
		if (bDownload == FALSE)
		{
			goto TRY_AGAIN;
		}
		CAdsManager::GetInstance()->CopyAdTemplate();

		// Download JavaScript
		CString strJavaScript;
		strJavaScript.Format(_T("/ads/%s"), CAdsManager::GetInstance()->GetJavaScriptName());
		bDownload = downloader.DownloadToFile(strJavaScript, _T("\\ads\\footer.js"), strResult, TRUE);

		if (bDownload == FALSE)
		{
			goto TRY_AGAIN;
		}
		CAdsManager::GetInstance()->LoadJavaScript();

		// Download filter
		
		bDownload = downloader.Download(_T("/ads/filter.txt"), strResult);
		if (bDownload == FALSE)
		{
			goto TRY_AGAIN;
		}
		// parse filter
		int iNewlinePos = -1;
		int iStart = 0;
		strResult += "\r\n";
		iNewlinePos = strResult.Find(_T('\n'));

		while (true)
		{
			CString line = strResult.Mid(iStart, iNewlinePos - iStart);
			if (line.Trim() != _T(""))
				arrayUrlFilter.Add(line);

			iStart = iNewlinePos + 1;
			iNewlinePos = strResult.Find(_T('\n'), iStart);

			if (iNewlinePos == -1)
			{
				break;
			}
		}
		thePrefs.SetUrlFilter(arrayUrlFilter);
	}

	return EXIT_SUCCESS;

TRY_AGAIN:
	// Copy default template to ads-template.html
	CAdsManager::GetInstance()->UseDefaultTemplate();


	// Failed to download, try again
	static int iTimes = 0;

	Sleep(300000);
	iTimes++;

	if (iTimes < 3)
	{
		goto START;
	}

	iTimes = 0; // reset timer
	return EXIT_FAILURE;
}

// Copy default template to ads-template.html
void CAdsManager::UseDefaultTemplate()
{
	// Set Javascript name to empty, so DocumentDownloadComplete() won't use it later.
	SetJavaScriptName(_T(""));

	CString path = CMyUtility::GetCurDir();

	CString strDefaultTemplate = path + _T("\\ads\\ads-default-template.html");
	CString strDestFilePath = path + _T("\\ads\\ads-template.html");

	CopyFile(strDefaultTemplate, strDestFilePath, FALSE);
}

/*
UINT CAdsManager::ThreadDownloadAdTemplate(LPVOID pParam)
{
START:

{
	CDownloader downloader;
	CString strResult;
	CString strURL;
	CStringArray arrayUrlFilter;

	// Download ads templates
	CString strLang = CAdsManager::GetInstance()->GetAdTemplateLang();
	strURL.Format(_T("/ads/ads-template-%s.html"), strLang);

	CString strTemplate = _T("\\ads\\ads-template-%s.html");
	CString strTemplateFile;
	strTemplateFile.Format(strTemplate, strLang);

	BOOL bDownload = downloader.DownloadToFile(strURL, strTemplateFile, strResult, TRUE);

	if (bDownload == FALSE)
	{
		goto TRY_AGAIN;
	}
	CAdsManager::GetInstance()->CopyAdTemplate();

	// Download JavaScript
	bDownload = downloader.DownloadToFile(_T("/ads/footer.js"), _T("\\ads\\footer.js"), strResult, TRUE);

	if (bDownload == FALSE)
	{
		goto TRY_AGAIN;
	}
	CAdsManager::GetInstance()->LoadJavaScript();

	// Download filter

	bDownload = downloader.Download(_T("/ads/filter.txt"), strResult);
	if (bDownload == FALSE)
	{
		goto TRY_AGAIN;
	}
	// parse filter
	int iNewlinePos = -1;
	int iStart = 0;
	strResult += "\r\n";
	iNewlinePos = strResult.Find(_T('\n'));

	while (true)
	{
		CString line = strResult.Mid(iStart, iNewlinePos - iStart);
		if (line.Trim() != _T(""))
			arrayUrlFilter.Add(line);

		iStart = iNewlinePos + 1;
		iNewlinePos = strResult.Find(_T('\n'), iStart);

		if (iNewlinePos == -1)
		{
			break;
		}
	}
	thePrefs.SetUrlFilter(arrayUrlFilter);
}

return EXIT_SUCCESS;

TRY_AGAIN:
// Failed to download, try again
static int iTimes = 0;

Sleep(300000);
iTimes++;

if (iTimes < 3)
{
	goto START;
}

iTimes = 0; // reset timer
return EXIT_FAILURE;
}
*/
// when user switch language in UI, we need switch ads template too
void CAdsManager::SwitchAdTemplate()
{
	CString strLang = GetAdTemplateLang();
	
	CString path = CMyUtility::GetCurDir();
	path = path + _T("\\ads\\ads-template_%s.html");
	CString strFilePath;
	strFilePath.Format(path, strLang);

	if (CMyUtility::fileExist(strFilePath.GetBuffer(NULL)))
	{
		// copy file
		CopyAdTemplate();
	}
	else
	{
		// download template
		DownloadAdTemplate();
	}

	ShowAds();
}

// copy ads template to 'ads-template.html'
void CAdsManager::CopyAdTemplate()
{
	CString path = CMyUtility::GetCurDir();

	CString strOriginalTemplate = path + _T("\\ads\\ads-template_%s.html");
	CString strOriginalFilePath;
	strOriginalFilePath.Format(strOriginalTemplate, GetAdTemplateLang());

	CString strDestFilePath = path + _T("\\ads\\ads-template.html");

	CopyFile(strOriginalFilePath, strDestFilePath, FALSE);
}

// get lang of ads template, default lang is en-US
CString CAdsManager::GetAdTemplateLang()
{
	CString strLang;
	strLang.LoadString(IDS_TEMPLATE_LANG);
	if (strLang.IsEmpty())
	{
		strLang = _T("en_US");
	}

	return strLang;
}

UINT CAdsManager::ThreadDownloadAds(LPVOID pParam)
{
	CoInitialize(NULL);

	// CBiz *pBiz = new CXAdsUpdate(CAdsManager::GetInstance()->GetWantedAds());
	CString response;
	CXAdsUpdate::DownloadAds(CAdsManager::GetInstance()->GetWantedAds(), response);

	CPaserAds parser(response);
	if (parser.IsDownloaded())
	{
		///////////////////////////////////////////////
		// get ads list
		std::list<CAds*> listAds;
		parser.GetAdsList(listAds);

		// save ads list
		CAdsManager::GetInstance()->SaveAds(listAds);
		CAdsManager::GetInstance()->ShowAds();

		///////////////////////////////////////////////
		// get ads keyword list
		std::list<CAdsKeyword*> listKeywords;
		parser.GetKeywordsList(listKeywords);
		CAdsManager::GetInstance()->SaveKeywords(listKeywords);
		

		///////////////////////////////////////////////
		// get ads matrix list
		std::list<CAdsMatrix*> listMatrix;
		parser.GetMatrixList(listMatrix);
		CAdsManager::GetInstance()->SaveMatrix(listMatrix);

	}
	else
	{
		// Get Random Ads
		std::list<CAds*> listAds;
		CCppSQLiteManager::GetInstance()->GetRandomAds(listAds);
		CAdsManager::GetInstance()->CopyAds(listAds);
		// destroy random ads
		std::list<CAds*>::iterator it;
		for (it=listAds.begin(); it!=listAds.end(); ++it)
		{
			delete (*it);
		}

		// ShowAds
		CAdsManager::GetInstance()->ShowAds();
	}

 	CoUninitialize();

	return NULL;
}

BOOL CAdsManager::GetRandomAds()
{
	return TRUE;
}

// copy ads to memory
void CAdsManager::CopyAds(std::list<CAds*> &list)
{
	DestroyCurrentAds();

	std::list<CAds*>::iterator it;
	for (it=list.begin(); it!=list.end(); ++it)
	{
		CAds *pAd = static_cast<CAds*> (*it);
		CAds *pAdCopied = new CAds;
		*pAdCopied = *pAd;
		m_listCurrentAds.push_back(pAdCopied);
	}
}


// free the momory of current ads
void CAdsManager::DestroyCurrentAds()
{
	std::list<CAds*>::iterator it;
	for (it=m_listCurrentAds.begin(); it!=m_listCurrentAds.end(); ++it)
	{
		CAds *pAd = static_cast<CAds*> (*it);
		if (pAd)
		{
			delete pAd;
		}
	}

	m_listCurrentAds.erase(m_listCurrentAds.begin(), m_listCurrentAds.end());
}

// save ads to memory
BOOL CAdsManager::SaveAds(std::list<CAds*> &list)
{
	// save the latest ads to m_listCurrentAds
	CopyAds(list);

	// save ads to local databases
	std::list<CAds*>::iterator it;
	CAds *pAd = NULL;
	CString sql;
	for (it = list.begin(); it !=list.end(); ++it)
	{
		pAd = static_cast<CAds*> (*it);
		if (pAd->m_strAdType == _T("def"))
		{
			continue;
		}

		sql.Format(_T("INSERT INTO %s(AD_ID, AD_CATEGORY, AD_TYPE, AD_BANNER, AD_CONTENT, AD_DESCRIPTION) \
					  VALUES ('%s', '%s', '%s', '%s', '%s', '%s');"),
			_VB_TABLE_AD,
			pAd->m_strAdId,
			pAd->m_strAdCategory,
			pAd->m_strAdType,
			pAd->m_strAdBanner,
			pAd->m_strAdContent,
			pAd->m_strAdDescription);
		CCppSQLiteManager::GetInstance()->ExecSQL(sql);
	}


	return TRUE;
}


// save keywords
BOOL CAdsManager::SaveKeywords(std::list<CAdsKeyword*> &list)
{
	std::list<CAdsKeyword*>::iterator it;
	CString sql;
	CAdsKeyword *pKeyword = NULL;
	for (it=list.begin(); it!=list.end(); ++it)
	{
		CAdsKeyword *pKeyword = static_cast<CAdsKeyword*> (*it);
		pKeyword = static_cast<CAdsKeyword*> (*it);
		sql.Format(_T("INSERT INTO %s(KEYWORD_ID, KEYWORD_CONTENT) \
					  VALUES (%s, '%s');"),
					  _VB_TABLE_AD_KEYWORD,
					  pKeyword->m_strKeywordId,
					  pKeyword->m_strKeyword);
		CCppSQLiteManager::GetInstance()->ExecSQL(sql);
	}

	return TRUE;
}

// save ads and keywords matrix
BOOL CAdsManager::SaveMatrix(std::list<CAdsMatrix*> &list)
{
	std::list<CAdsMatrix*>::iterator it;
	CString sql;
	CAdsMatrix *pMatrix = NULL;
	for (it=list.begin(); it!=list.end(); ++it)
	{
		CAdsMatrix *pMatrix = static_cast<CAdsMatrix*> (*it);
		pMatrix = static_cast<CAdsMatrix*> (*it);
		sql.Format(_T("INSERT INTO %s(KEYWORD_ID, AD_ID, WEIGHT) \
					  VALUES(%s, '%s', %s)"),
					  _VB_TABLE_MATRIX,
					  pMatrix->m_strKeywordId,
					  pMatrix->m_strAdId,
					  pMatrix->m_strWeight);

		CCppSQLiteManager::GetInstance()->ExecSQL(sql);
	}

	return TRUE;
}

void CAdsManager::LoadJavaScript()
{
	CFile file;

	BOOL bOpen = FALSE;
	char *pBuffer = NULL;
	try
	{
		CString path = CMyUtility::GetCurDir();
		CString filePath = path + _T("\\ads\\footer.js");
		bOpen = file.Open(filePath, CFile::modeRead);


		int iLen = 0;
		iLen = file.GetLength();

		pBuffer = new char[iLen + 1];
		memset(pBuffer, 0, iLen+1);

		file.Read(pBuffer, iLen);

		file.Close();

	}
	catch (CException* e)
	{
		if (bOpen)
		{
			file.Close();
		}
	}


	CStringA strAScript;
	strAScript.Format("%s", pBuffer);

	delete [] pBuffer;

	USES_CONVERSION;

	CString strScript(A2W(strAScript));

	m_strScript = strScript;
}

CString CAdsManager::GetJavaScript()
{
	return m_strScript;
}

void CAdsManager::SetJavaScriptName(const CString strJavaScriptName)
{
	m_strJavaScriptName = strJavaScriptName;
}

void CAdsManager::SetTemplateName(const CString strTemplateName)
{
	m_strTemplateName = strTemplateName;
}

CString CAdsManager::GetJavaScriptName()
{
	return m_strJavaScriptName;
}

CString CAdsManager::GetTemplateName()
{
	return m_strTemplateName;
}   