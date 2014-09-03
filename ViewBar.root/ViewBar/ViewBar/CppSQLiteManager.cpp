#include "StdAfx.h"
#include "CppSQLiteManager.h"
#include "MyUtility.h"
#include "SearchEngine.h"
#include "AdsManager.h"
#include "AdsManager.h"

#define sprintf sprintf_s

#define CCSECTION(fun,Rtn,OtherFun) \
	EnterCriticalSection(&m_ccsec); \
	fun; \
	LeaveCriticalSection(&m_ccsec); \
	OtherFun; \
	return Rtn;

CCppSQLiteManager* CCppSQLiteManager::m_pInstance = NULL;
CCppSQLiteManager::CCppSQLiteManager(void)
{
	InitializeCriticalSection(&m_ccsec);
	Initialize();
}

void CCppSQLiteManager::Initialize()
{
	TCHAR FileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, FileName,MAX_PATH);
	CString csFileName = FileName;
	int k = csFileName.ReverseFind(_T('\\'));
	int Len = csFileName.GetLength();
	CString csExeName = csFileName.Mid(k+1,Len-k-1-3);
	csFileName = csFileName.Left(k);
	csFileName += _T("\\data\\");
	csFileName += csExeName + _T("ldb");
#ifdef UNICODE
	char dbFile[MAX_PATH];
	int size = WideCharToMultiByte( CP_ACP, 0, csFileName.GetBuffer(), -1, dbFile, MAX_PATH, NULL, NULL);
	dbFile[size] = '\0';
#else
	char *dbFile = csFileName.GetBuffer();
#endif
	try
	{
		m_db.open(dbFile);
	}
	catch(CppSQLiteException e)
	{
		MessageBoxA(NULL, e.errorMessage(), "error", MB_ICONSTOP|IDOK);
		throw _T("failed to open database");
	}
	//CreateTable(TABLE_SURFTIME,ITEM_SURFTIME);
	//CreateTable(TABLE_URLTRACK,ITEM_URLTRACK);

	//DeleteItem(TABLE_SURFTIME,"where MemID='3'");
	//CreateTable(TABLE_SURFTIME,ITEM_SURFTIME);
	/*InsertItem(TABLE_SURFTIME,"0,'Hello SQLite0'");
	InsertItem(TABLE_SURFTIME,"1,'Hello SQLite1'");
	InsertItem(TABLE_SURFTIME,"2,'Hello SQLite2'");
	InsertItem(TABLE_SURFTIME,"3,'Hello SQLite3'");
	InsertItem(TABLE_SURFTIME,"3,'Hello SQLite4'");*/
/*
	char chCondition[128];
	sprintf(chCondition,"where MemID = %d",3);
	STExecTableQuery(TABLE_SURFTIME, "select * ",chCondition );
	int rows = STGetRowsNumber();
	int id = 0;
	char out[128];
	for( int i = 0; i < rows; i++ )
	{
		id = STGetIntValue(i,0);
		sprintf(out,"%s",STGetStringValue(i,1));
	}
	DeleteAllItems(TABLE_SURFTIME);
	rows = STGetRowsNumber();*/
}

CCppSQLiteManager::~CCppSQLiteManager(void)
{
	DeleteCriticalSection(&m_ccsec);
	m_db.close();
}

CCppSQLiteManager* CCppSQLiteManager::GetInstance()
{
	if( m_pInstance == NULL )
		m_pInstance = new CCppSQLiteManager();
	return m_pInstance;
}

void CCppSQLiteManager::FreeInstance()
{
	if( m_pInstance )
		delete m_pInstance;
}

int CCppSQLiteManager::UTInsertItem(const char* szContent)
{
	ASSERT(szContent);

	char szInsert[1024];
	sprintf(szInsert,"insert into TableURLTrack(MemID, Send, Param, Url, Title, Other) "
		"values(%s)",szContent);
	CCSECTION(int nRtn = m_db.execDML(szInsert),nRtn,;);
}

int CCppSQLiteManager::InsertItem(const char* szTable, const char* szContent)
{
	ASSERT(szTable);
	ASSERT(szContent);

	char szInsert[1024];
	sprintf(szInsert,"insert into %s values (%s);",szTable,szContent);
	CCSECTION(int nRtn = m_db.execDML(szInsert),nRtn,;);
}

int CCppSQLiteManager::InsertItem(const char* szTable, CString &szContent)
{
#ifdef UNICODE
	int len = szContent.GetLength();
	int chLen = len*2+10;
	char *szchContent = new char[chLen];
	int size = WideCharToMultiByte(CP_ACP,0,szContent.GetBuffer(),len,szchContent,chLen,NULL,NULL);
	szchContent[size] = '\0';
	CCSECTION(int nRtn = InsertItem(szTable,szchContent),nRtn,delete [] szchContent);
#else
	CCSECTION(int nRtn = InsertItem(szTable,szContent),nRtn,;);
#endif
}

int CCppSQLiteManager::UpdateItem(const char* szTable, const char* szContent)
{
	ASSERT(szTable);
	ASSERT(szContent);

	char szUpdate[1024*3];
	sprintf(szUpdate,"update %s %s;",szTable,szContent);
	CCSECTION(int nRtn = m_db.execDML(szUpdate),nRtn,;);
}

int CCppSQLiteManager::UpdateItem(const char* szTable, CString &szContent)
{
#ifdef UNICODE
	int len = szContent.GetLength();
	int chLen = len*2+10;
	char *szchContent = new char[chLen];
	int size = WideCharToMultiByte(CP_ACP,0,szContent.GetBuffer(),len,szchContent,chLen,NULL,NULL);
	szchContent[size] = '\0';
	CCSECTION(int nRtn = UpdateItem(szTable,szchContent),nRtn,delete [] szchContent);
#else
	return UpdateItem(szTable,szchContent);
#endif
}

int CCppSQLiteManager::CreateTable(const char* szTable, const char *szContent)
{
	ASSERT(szTable);
	ASSERT(szContent);
	char szCreate[1024];
	sprintf(szCreate,"create table %s(%s);",szTable,szContent);
	CCSECTION(int nRtn = m_db.execDML(szCreate),nRtn,;);
}

int CCppSQLiteManager::DeleteItem(const char* szTable, const char* szContent)
{
	ASSERT(szTable);
	ASSERT(szContent);

	char szDelete[1024];
	sprintf(szDelete,"delete from %s %s;",szTable,szContent);
	CCSECTION(int nRtn = m_db.execDML(szDelete),nRtn,;);
}

int CCppSQLiteManager::DeleteAllItems(const char* szTable)
{
	ASSERT(szTable);
	char szDelete[1024];
	sprintf(szDelete,"delete from %s;",szTable);
	CCSECTION(int nRtn = m_db.execDML(szDelete),nRtn,;);
}

int CCppSQLiteManager::DropTable(const char* szTable)
{
	ASSERT(szTable);
	char szDrop[1024];
	sprintf(szDrop,"drop table %s;",szTable);
	CCSECTION(int nRtn = m_db.execDML(szDrop),nRtn,;);
}

const char* CCppSQLiteManager::STGetStringValue(int row, int col)
{
	m_TableSurfTime.setRow(row);
	CCSECTION(const char * rtn = m_TableSurfTime.getStringField(col),rtn,;);
}

int CCppSQLiteManager::STGetIntValue(int row, int col)
{
	m_TableSurfTime.setRow(row);
	CCSECTION(int nRtn = m_TableSurfTime.getIntField(col),nRtn,;);
}

void CCppSQLiteManager::STExecTableQuery(const char* szTable, const char* szContent, const char* condition)
{
	ASSERT(szTable);
	ASSERT(szContent);
	ASSERT(condition);
	char szQuery[1024];
	sprintf(szQuery,"%s from %s %s;",szContent,szTable,condition);
	CCSECTION(m_TableSurfTime = m_db.getTable(szQuery),;,;);
}
/////////////////////////////////////////////////////////////////////////////
const char* CCppSQLiteManager::UTGetStringValue(int row, int col)
{
	m_TableUrlTrack.setRow(row);
	CCSECTION(const char *Rtn = m_TableUrlTrack.getStringField(col),Rtn,;);
}

int CCppSQLiteManager::UTGetIntValue(int row, int col)
{
	m_TableUrlTrack.setRow(row);
	CCSECTION(int nRtn = m_TableUrlTrack.getIntField(col),nRtn,;);
}

void CCppSQLiteManager::UTExecTableQuery(const char* szTable, const char* szContent, const char* condition)
{
	ASSERT(szTable);
	ASSERT(szContent);
	ASSERT(condition);
	char szQuery[1024];
	sprintf(szQuery,"%s from %s %s;",szContent,szTable,condition);
	CCSECTION(m_TableUrlTrack = m_db.getTable(szQuery),;,;);
}

// Execute a sql statement, such as update, delete, insert
// when exception occurs, the function will throw a string to caller
// Author: Charry Wong
int CCppSQLiteManager::ExecSQL(const CString &sql)
{
	char *pSql = CMyUtility::GetAnsiString(sql);
	try
	{
		CCSECTION(int iRet = m_db.execDML(pSql),iRet,delete [] pSql);
	}
	catch (CppSQLiteException e)
	{
		MessageBoxA(NULL, e.errorMessage(), "error", MB_OK);
		throw _T("Failed to call ExecSQL()");
	}	
}

// get random ads
void CCppSQLiteManager::GetRandomAds(std::list<CAds*> &list)
{
	CString sql;
	sql.Format(_T("SELECT * FROM %s"),_VB_TABLE_AD);
	GetAds(sql, list);
}

// Get number of rows
int CCppSQLiteManager::GetTableRowNumber(const CString sql)
{
	g_log.OutputDebug(sql);
	int iRow = 0;

	try
	{
		CppSQLiteTable table;
		CStringA sqlA = CMyUtility::GetCStringA(sql);
		table = m_db.getTable(sqlA.GetBuffer());
		
		for (int i=0; i<table.numRows(); ++i)
		{
			table.setRow(i);
			const char *pRow = NULL;
			pRow = table.getStringField("TABLE_ROWS");
			iRow = atoi(pRow);
			
			break;
		}

	}catch (CppSQLiteException e) 
	{
		// NOOP

		return 0;
	}

	return iRow;
}

// get ads from local databases
void CCppSQLiteManager::GetAds(const CString &sql, std::list<CAds*> &list)
{
	CppSQLiteTable table;
// 	char *pSQL = NULL;
// 
// 	pSQL = CMyUtility::GetAnsiString(sql);
// 	table = m_db.getTable(pSQL);
// 	delete []pSQL;
	CStringA mbSql = CMyUtility::GetCStringA(sql);
	table = m_db.getTable(mbSql.GetBuffer());

	int iLen = 0;
	TCHAR *pRowW = NULL;
	for (int i=0; i<table.numRows(); ++i)
	{
		CAds *pAd = new CAds;
		table.setRow(i);

		const char *pRow = NULL;
		pRow = table.getStringField("AD_ID");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pAd->m_strAdId.Format(_T("%s"), pRowW);
		delete [] pRowW;

		pRow = table.getStringField("AD_CATEGORY");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pAd->m_strAdCategory.Format(_T("%s"), pRowW);
		delete [] pRowW;

		pRow = table.getStringField("AD_TYPE");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pAd->m_strAdType.Format(_T("%s"), pRowW);
		delete [] pRowW;

		pRow = table.getStringField("AD_BANNER");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pAd->m_strAdBanner.Format(_T("%s"), pRowW);
		delete [] pRowW;

		pRow = table.getStringField("AD_CONTENT");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pAd->m_strAdContent.Format(_T("%s"), pRowW);
		delete [] pRowW;

		pRow = table.getStringField("AD_DESCRIPTION");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pAd->m_strAdDescription.Format(_T("%s"), pRowW);
		delete [] pRowW;

		list.push_back(pAd);
	}

}

void CCppSQLiteManager::GetFavorites(std::vector<CAglocoFavoritesItem> &vec)
{
	CppSQLiteTable table;
	CStringA sql;

	sql.Format("SELECT * FROM %s ORDER BY FAV_ID", _VB_TABLE_FAVORITES);
	table = m_db.getTable(sql.GetBuffer(NULL));
	int iLen = 0;
	TCHAR *pRowW = NULL;

	for (int i=0; i<table.numRows(); ++i)
	{
		CAglocoFavoritesItem item;
		table.setRow(i);

		CStringA strRow;
		int iFavId = table.getIntField("FAV_ID");	// Favorites ID
		strRow.Format("%d", iFavId);
		item.m_strId = CMyUtility::GetCStringW2(strRow);
		
		const char *pRow = NULL;
		pRow = table.getStringField("FAV_NAME");
		
		strRow.Format("%s", pRow);
		item.m_strName = CMyUtility::GetCStringW2(strRow);

		pRow = table.getStringField("FAV_URL");
		strRow.Format("%s", pRow);
		item.m_strUrl = CMyUtility::GetCStringW2(strRow);

		vec.push_back(item);
	}

	return;
}

void CCppSQLiteManager::GetSearchEngineList(std::list<CSearchEngine*> &vec)
{
	CppSQLiteTable table;
	
	CString sql;
	sql.Format(_T("SELECT * FROM %s ORDER BY ENGINE_ID"), _VB_TABLE_SEARCH_ENGINE);
	// char *pSQL = NULL;
	// pSQL = CMyUtility::GetAnsiString(sql);
	// table = m_db.getTable(pSQL);
	// delete [] pSQL;
	CStringA mbSql = CMyUtility::GetCStringA(sql);
	table = m_db.getTable(mbSql.GetBuffer());

	int iLen = 0;
	TCHAR *pRowW = NULL;

	for (int i=0; i<table.numRows(); ++i)
	{
		CSearchEngine *pEngine = new CSearchEngine;
		table.setRow(i);

		int iEngineId = table.getIntField("ENGINE_ID");	// Engine ID
		pEngine->m_iSearchEngineId = iEngineId;

		const char *pRow = NULL;
		pRow = table.getStringField("ENGINE_NAME");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pEngine->m_strSearchEngineName.Format(_T("%s"), pRowW);
		delete []pRowW;

		pRow = table.getStringField("ENGINE_URL");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pEngine->m_strSearchEngineURL.Format(_T("%s"), pRowW);
		delete []pRowW;

		pRow = table.getStringField("ENGINE_PATTERN");
		iLen = strlen(pRow) + 1;
		pRowW = CMyUtility::GetUnicodeString(pRow, iLen);
		pEngine->m_strSearchEnginePattern.Format(_T("%s"), pRowW);
		delete []pRowW;

		vec.push_back(pEngine);
	}
}

/*
CppSQLiteTable CCppSQLiteManager::GetTable(const char* szSQL)
{
	return m_db.getTable(szSQL);
}
//---------------------------------------------------------------------------
void CCppSQLiteManager::SetTransaction(PROC pfnTransaction)
{
	m_pfnTransaction = pfnTransaction;
}

void CCppSQLiteManager::Transaction()
{
	ASSERT(m_pfnTransaction);
	m_db.execDML("begin transaction;");
	m_pfnTransaction();
	m_db.execDML("commit transaction;");
}

//---------------------------------------------------------------------------
void CCppSQLiteManager::ExecQuery(const char* szTable, const char* szContent, const char* condition)
{
	ASSERT(szTable);
	ASSERT(szContent);
	ASSERT(condition);
	char szQuery[1024];
	sprintf(szQuery,"%s from %s %s",szContent,szTable,condition);
	m_Query = m_db.execQuery(szQuery);
}
//---------------------------------------------------------------------------

const char* CCppSQLiteManager::GetUnitValue(int row, int col)
{
	m_Table.setRow(row);
	if( m_Table.fieldIsNull(col) )
		return "NULL";
	return m_Table.fieldValue(col);
}

int CCppSQLiteManager::GetIntValue(int row, int col)
{
	m_Table.setRow(row);
	return m_Table.getIntField(col);
}

*/


void CCppSQLiteManager::AffilatesExecTableQuery(const char* szTable, const char* szContent, const char* condition)
{
	ASSERT(szTable);
	ASSERT(szContent);
	ASSERT(condition);
	char szQuery[1024];
	sprintf(szQuery,"%s from %s %s;",szContent,szTable,condition);
	CCSECTION(m_TableAffiliates = m_db.getTable(szQuery),;,;);
}

const char* CCppSQLiteManager::AffilatesGetStringValue(int row, int col)
{
	m_TableAffiliates.setRow(row);
	CCSECTION(const char* rtn = m_TableAffiliates.getStringField(col), rtn,;);
}

int CCppSQLiteManager::AffilatesGetIntValue(int row, int col)
{
	m_TableAffiliates.setRow(row);
	CCSECTION(int nRtn = m_TableAffiliates.getIntField(col),nRtn,;);
}


void CCppSQLiteManager::AffDetailsExecTableQuery(const char* szTable, const char* szContent, const char* condition)
{
	ASSERT(szTable);
	ASSERT(szContent);
	ASSERT(condition);
	char szQuery[1024];
	sprintf(szQuery,"%s from %s %s;",szContent,szTable,condition);
	CCSECTION(m_TableAffDetails = m_db.getTable(szQuery),;,;);
}

const char* CCppSQLiteManager::AffDetailsGetStringValue(int row, int col)
{
	m_TableAffDetails.setRow(row);
	CCSECTION(const char* rtn = m_TableAffDetails.getStringField(col),rtn,;);
}

int CCppSQLiteManager::AffDetailsGetIntValue(int row, int col)
{
	m_TableAffDetails.setRow(row);
	CCSECTION(int nRtn = m_TableAffDetails.getIntField(col), nRtn,;);
}

int CCppSQLiteManager::execQuery(const char* szTable,const char* szSQL)
{
	char szValue[128];
	sprintf(szValue,"%s from %s;", szSQL, szTable);
	CppSQLiteQuery q = m_db.execQuery(szValue);
	if( q.eof() || q.numFields() < 1 )
		return 0;
	return q.getIntField("max(createDate)");
	return atoi(q.fieldValue(0));
};