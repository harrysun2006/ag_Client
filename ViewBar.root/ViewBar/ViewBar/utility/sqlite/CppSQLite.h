////////////////////////////////////////////////////////////////////////////////
// CppSQLite - A C++ wrapper header for SQLite embedded database library.
////////////////////////////////////////////////////////////////////////////////
#ifndef _CppSQLite_H_
#define _CppSQLite_H_

#include "sqlite.h"
#include <cstdio>
#include <cstring>
#include "..\\..\\VBObject.h"

using namespace viewbar;

#define CPPSQLITE_ERROR 1000

class CppSQLiteException:public CVBObject
{
public:

    CppSQLiteException(const int nErrCode,
                    char* szErrMess,
                    bool bDeleteMsg=true);

    CppSQLiteException(const CppSQLiteException&  e);

    virtual ~CppSQLiteException();

    const int errorCode() { return mnErrCode; }

    const char* errorMessage() { return mpszErrMess; }

    static const char* errorCodeAsString(int nErrCode);

private:

    int mnErrCode;
    char* mpszErrMess;
};


class CppSQLiteBuffer:public CVBObject
{
public:

    CppSQLiteBuffer();

    ~CppSQLiteBuffer();

    const char* format(const char* szFormat, ...);

    operator const char*() { return mpBuf; }

    void clear();

private:

    char* mpBuf;
};

class CppSQLiteBinary:public CVBObject
{
public:

    CppSQLiteBinary();

    ~CppSQLiteBinary();

    void setBinary(const unsigned char* pBuf, int nLen);
    void setEncoded(const unsigned char* pBuf);

    const unsigned char* getEncoded();
    const unsigned char* getBinary();

    int getBinaryLength();

    unsigned char* allocBuffer(int nLen);

    void clear();

private:

    unsigned char* mpBuf;
    int mnBinaryLen;
    int mnBufferLen;
    int mnEncodedLen;
    bool mbEncoded;
};


class CppSQLiteQuery:public CVBObject
{
public:

    CppSQLiteQuery();

    CppSQLiteQuery(const CppSQLiteQuery& rQuery);

    CppSQLiteQuery(sqlite_vm* pVM,
                bool bEof,
                int nCols,
                const char** paszValues,
                const char** paszColNames,
                bool bOwnVM=true);

    CppSQLiteQuery& operator=(const CppSQLiteQuery& rQuery);

    virtual ~CppSQLiteQuery();

    int numFields();

    const char* fieldName(int nCol);

    const char* fieldType(int nCol);

    const char* fieldValue(int nField);
    const char* fieldValue(const char* szField);

    int getIntField(int nField, int nNullValue=0);
    int getIntField(const char* szField, int nNullValue=0);

    double getFloatField(int nField, double fNullValue=0.0);
    double getFloatField(const char* szField, double fNullValue=0.0);

    const char* getStringField(int nField, const char* szNullValue="");
    const char* getStringField(const char* szField, const char* szNullValue="");

    bool fieldIsNull(int nField);
    bool fieldIsNull(const char* szField);

    bool eof();

    void nextRow();

    void finalize();

private:

    void checkVM();

    sqlite_vm* mpVM;
    bool mbEof;
    int mnCols;
    const char** mpaszValues;
    const char** mpaszColNames;
    bool mbOwnVM;
};


class CppSQLiteTable:public CVBObject
{
public:

    CppSQLiteTable();

    CppSQLiteTable(const CppSQLiteTable& rTable);

    CppSQLiteTable(char** paszResults, int nRows, int nCols);

    virtual ~CppSQLiteTable();

    CppSQLiteTable& operator=(const CppSQLiteTable& rTable);

    int numFields();

    int numRows();

    const char* fieldName(int nCol);

    const char* fieldValue(int nField);
    const char* fieldValue(const char* szField);

    int getIntField(int nField, int nNullValue=0);
    int getIntField(const char* szField, int nNullValue=0);

    double getFloatField(int nField, double fNullValue=0.0);
    double getFloatField(const char* szField, double fNullValue=0.0);

    const char* getStringField(int nField, const char* szNullValue="");
    const char* getStringField(const char* szField, const char* szNullValue="");

    bool fieldIsNull(int nField);
    bool fieldIsNull(const char* szField);

    void setRow(int nRow);

    void finalize();

private:

    void checkResults();

    int mnCols;
    int mnRows;
    int mnCurrentRow;
    char** mpaszResults;
};


class CppSQLiteStatement:public CVBObject
{
public:

    CppSQLiteStatement();

    CppSQLiteStatement(const CppSQLiteStatement& rStatement);

    CppSQLiteStatement(sqlite* pDB, sqlite_vm* pVM);

    virtual ~CppSQLiteStatement();

    CppSQLiteStatement& operator=(const CppSQLiteStatement& rStatement);

    int execDML();

    CppSQLiteQuery execQuery();

    void bind(int nParam, const char* szValue);
    void bind(int nParam, const int nValue);
    void bind(int nParam, const double dwValue);
    void bindNull(int nParam);

    void reset();

    void finalize();

private:

    void checkDB();
    void checkVM();

    sqlite* mpDB;
    sqlite_vm* mpVM;
};


class CppSQLiteDB:public CVBObject
{
public:

    CppSQLiteDB();

    virtual ~CppSQLiteDB();

    void open(const char* szFile);

    void close();

    int execDML(const char* szSQL);

    CppSQLiteQuery execQuery(const char* szSQL);

    int execScalar(const char* szSQL);

    CppSQLiteTable getTable(const char* szSQL);

	// added by victor. 4/5-2007
	BOOL IsExistTable(const char* szTable);

    CppSQLiteStatement compileStatement(const char* szSQL);

    int lastRowId();

    void interrupt() { sqlite_interrupt(mpDB); }

    void setBusyTimeout(int nMillisecs);

    static const char* SQLiteVersion() { return SQLITE_VERSION; }

private:

    CppSQLiteDB(const CppSQLiteDB& db);
    CppSQLiteDB& operator=(const CppSQLiteDB& db);

    sqlite_vm* compile(const char* szSQL);

    void checkDB();

    sqlite* mpDB;
    int mnBusyTimeoutMs;
};

#endif
