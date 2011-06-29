
#include "10_db/101_sqlite/SQLiteDB.h"

#include "SQLiteModule.inc.cpp"

void synthese::db::sqlite::moduleRegister()
{
	synthese::db::SQLiteDB::integrate();
}
