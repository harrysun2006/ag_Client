//  how to confiura the env.
//  1. link the sqlite.lib static library.
//  2. append CPSQLite.h/cpp into project.
//  3. include CppSQLite.h head file.

#include "CppSQLite.H"

CppSQLiteDB db

// open database.
db.open(filename);

// create table, exec command, run DDL 
db.execDML("DDL")

// start transaction, commit transaction.
db.execDML("begin transaction;")
db.execDML("commit transaction;")

//query data.
CppSQLiteQuery q1  =db.execQuery("sql statement");

q1.numFields()		//the number of fields?
q1.fieldName(index)	//indexed-base field name?
q1.fieldType(index)		//indexed-base field type?
q1.fieldValue(index)		//indexed-base field value?
q1.nextRow()		// next row?
q1.eof			// judge whether reaching eof of table?


