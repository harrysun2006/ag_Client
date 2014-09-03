#pragma once
#include ".\utility\sqlite\CppSQLite.h"
#include <list>
#include <vector>
#include "SearchEngine.h"
#include "AglocoFavoritesItem.h"

class CAds;

#define TABLE_SURFTIME		"TableSurfTime"
#define TABLE_URLTRACK		"TableURLTrack"
#define TABLE_AFFILIATES	"TableAffiliates"
#define TABLE_AFFDETIALS	"TableAffDetials"

#define ITEM_SURFTIME	"MemID int, DataItem char(256)"
#define ITEM_URLTRACK	"MemID int, Send int, Param int, Url char(255), Title char(255), Other char(255), ID int"
#define ITEM_AFFILIATES	"affiliateId int, status int,MemId int, name char(255), createDate int, category char(255)"
#define ITEM_AFFDETIALS	"afDetailId int, MemId int, affiliateId int, createDate int, pattern char(255), agPattern char(255), category char(255), rebate float, status int, info char(255), description char(255)"

#define POS_AFFID_AFFILIATES		0
#define POS_AFFNAME_AFFILIATES		3

#define POS_DETPATTERN_AFFILIATES	4
#define POS_DETAGPATTERN_AFFILIATES	5

class CCppSQLiteManager
{
private:
	CCppSQLiteManager(void);
	~CCppSQLiteManager(void);

	void Initialize();

	static CCppSQLiteManager* m_pInstance;
	CppSQLiteDB m_db;
public:
	
	static CCppSQLiteManager* GetInstance();
	static void FreeInstance();

	int InsertItem(const char* szTable, const char* szContent);
	int InsertItem(const char* szTable, CString &szContent);
	int UpdateItem(const char* szTable, const char* szContent);
	int UpdateItem(const char* szTable, CString &szContent);
	int CreateTable(const char* szTable, const char *szContent);
	int DeleteItem(const char* szTable, const char* szContent);
	int DeleteAllItems(const char* szTable);
	int DropTable(const char* szTable);
	BOOL IsExistTable(const char* szTable);

	int execQuery(const char* szTable,const char* szSQL);
	
	// ex, added by Charry
	// execute a sql statement
	int ExecSQL(const CString &sql);
	int GetTableRowNumber(const CString sql);
	// get search engine list from database
	void GetSearchEngineList(std::list<CSearchEngine *> &vec);
	void GetAds(const CString &sql, std::list<CAds*> &list);
	void GetRandomAds(std::list<CAds*> &list);
	void GetFavorites(std::vector<CAglocoFavoritesItem> &vec);

public:
	// Surf Time
	// {{
	//-----------------------------------------------------------------------
	int STGetRowsNumber()
	{
		return m_TableSurfTime.numRows();
	};
	void STExecTableQuery(const char* szTable, const char* szContent, const char* condition);
	const char* STGetStringValue(int row, int col);
	int STGetIntValue(int row, int col);
private:
	CppSQLiteTable m_TableSurfTime;
	// }}
	//-----------------------------------------------------------------------

public:
	// URL Track
	// {{
	//-----------------------------------------------------------------------
	int UTInsertItem(const char* szContent);
	int UTGetRowsNumber()
	{
		return m_TableUrlTrack.numRows();
	};
	void UTExecTableQuery(const char* szTable, const char* szContent, const char* condition);
	const char* UTGetStringValue(int row, int col);
	int UTGetIntValue(int row, int col);
private:
	CppSQLiteTable m_TableUrlTrack;
	//-----------------------------------------------------------------------
	// }}

	// Affiliates
	//-----------------------------------------------------------------------
public:
	int AffLiatesGetRowsNumber()
	{
		return m_TableAffiliates.numRows();
	};
	void AffilatesExecTableQuery(const char* szTable, const char* szContent, const char* condition);
	const char* AffilatesGetStringValue(int row, int col);
	int AffilatesGetIntValue(int row, int col);
private:
	CppSQLiteTable m_TableAffiliates;
	//-----------------------------------------------------------------------
	// AffDetails
	//-----------------------------------------------------------------------
public:
	int AffDetailsGetRowsNumber()
	{
		return m_TableAffDetails.numRows();
	};
	void AffDetailsExecTableQuery(const char* szTable, const char* szContent, const char* condition);
	const char* AffDetailsGetStringValue(int row, int col);
	int AffDetailsGetIntValue(int row, int col);
private:
	CppSQLiteTable m_TableAffDetails;
	CRITICAL_SECTION m_ccsec;
	//-----------------------------------------------------------------------
};

/////////////////////////////////////////////////////////////////////////////


	/*
	CppSQLiteTable GetTable(const char* szSQL);
	//--------------------------------------------------------------------------
	void SetTransaction(PROC pfnTransaction);
	void Transaction();

	//--------------------------------------------------------------------------
	void ExecQuery(const char* szTable, const char* szContent, const char* condition);
	int GetFieldsNumber(){return m_Query.numFields();};
	const char* GetFieldName(int i){return m_Query.fieldName(i);};
	const char* GetFieldType(int i){return m_Query.fieldType(i);};
	BOOL Eof(){return m_Query.eof();};
	const char* GetFieldValue(int i) {return m_Query.fieldValue(i);};
	void NextRow(){m_Query.nextRow();};

	// table;
	//--------------------------------------------------------------------------
	void ExecTableQuery(const char* szTable, const char* szContent, const char* condition);
	int GetFieldsNumberTable(){return m_Table.numFields();};
	const char* GetFieldNameTable(int i){return m_Table.fieldName(i);};
	const char* GetUnitValue(int row, int col);
	int GetIntValue(int row, int col);
private:
	static CCppSQLiteManager* m_pInstance;
	CppSQLiteDB m_db;
	PROC m_pfnTransaction;
	CppSQLiteQuery m_Query;
	CppSQLiteTable m_Table;
};*/
