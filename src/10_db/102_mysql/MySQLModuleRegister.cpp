#include "10_db/102_mysql/MySQLDB.hpp"
#include "10_db/102_mysql/MySQLDBModifiedAction.hpp"

#include "MySQLModule.inc.cpp"

void synthese::db::mysql::moduleRegister()
{
	synthese::db::MySQLDB::integrate();
	synthese::db::MySQLDBModifiedAction::integrate();

}
