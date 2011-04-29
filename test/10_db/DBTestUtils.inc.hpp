/** Utilities for database tests.
	@file DBTestUtils.inc.hpp
	@author Sylvain Pasche
	@date 2011

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_db_DbTestUtils_inc_hpp__
#define SYNTHESE_db_DbTestUtils_inc_hpp__

#include "10_db/DBModule.h"
#include "10_db/DBTableSync.hpp"
#include "10_db/DBRegistryTableSyncTemplate.hpp"
#include "10_db/101_sqlite/SQLiteDB.h"

#ifdef WITH_MYSQL
#include "10_db/102_mysql/MySQLDB.hpp"
#include "10_db/102_mysql/MySQLDBModifiedAction.hpp"
#include "10_db/102_mysql/MySQLException.hpp"
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using boost::shared_ptr;
using namespace synthese;
using namespace synthese::db;
using synthese::server::ModuleClass;
using synthese::server::ModuleClassTemplate;
using namespace synthese::util;
using namespace std;

struct GlobalFixture
{
	GlobalFixture()
	{
#ifdef _MSC_VER

	// Disable leak logs on Windows (they are set by Boost.Test).
	// TODO: fix the leaks and then remove this line
	_CrtSetDbgFlag(~_CRTDBG_LEAK_CHECK_DF & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));

	// Useful for attaching debugger to the test at startup.
	if (::getenv("SYNTHESE_PAUSE"))
		::system("pause");
#endif
		Log::GetInstance().setLevel(Log::LEVEL_TRACE);
	}

	~GlobalFixture()
	{
#ifdef _MSC_VER
		// For debugging, to keep the cmd window open.
		if (::getenv("SYNTHESE_PAUSE"))
			system("pause");
#endif
	}
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);


/// Register a registrable in the environment on construction and unregister it on destruction.
template <class Registrable>
struct ScopedRegistrable
{
	ScopedRegistrable()
	{
		synthese::util::Env::Integrate<Registrable>();
	}
	~ScopedRegistrable()
	{
		synthese::util::Env::Unregister<Registrable>();
	}
};

/// Register a factory on construction and unregister it on destruction.
template <class FactoryTemplate>
struct ScopedFactory
{
	ScopedFactory()
	{
		FactoryTemplate::integrate();
	}
	~ScopedFactory()
	{
		FactoryTemplate::unregister();
	}
};

/// Calls the module PreInit() and Init() methods on construction and End() on destruction.
template <class Module>
struct ScopedModule
{
	ScopedModule()
	{
		ModuleClassTemplate<Module>::PreInit();
		ModuleClassTemplate<Module>::Init();
	}
	~ScopedModule()
	{
		ModuleClassTemplate<Module>::End();
	}
};


/// Base class for testing a database backend.
class TestBackend
{
protected:
	std::string _connectionString;
public:
	TestBackend() {}
	virtual ~TestBackend() {}

	virtual const std::string getConnectionString() const
	{
		return _connectionString;
	}
	virtual std::string getName() const = 0;
	virtual void setUpDb() const = 0;
};

class SQLiteTestBackend : public TestBackend
{
	boost::filesystem::path _dbPath;
	ScopedFactory<SQLiteDB> _scopedSqliteDb;

public:
	SQLiteTestBackend()
	{
		// TODO: maybe use a temporary file instead.
		_dbPath = boost::filesystem::complete("test_db.db", boost::filesystem::initial_path());

		_connectionString = "sqlite://debug=1,path=" + _dbPath.string();
	}

	~SQLiteTestBackend()
	{
		if (getenv("SYNTHESE_SQLITE_KEEPDB"))
			return;
		boost::filesystem::remove(_dbPath);
	}

	virtual std::string getName() const
	{
		return "sqlite";
	}

	virtual void setUpDb() const
	{
		boost::filesystem::remove(_dbPath);
	}
};

#ifdef WITH_MYSQL
class MySQLTestBackend : public TestBackend
{
	ScopedFactory<MySQLDB> _scopedMysqlDb;
	ScopedFactory<MySQLDBModifiedAction> _scopedMysqlDbModifiedAction;
	std::string _connectionStringWithoutDb;
	string _dbName;

public:

	MySQLTestBackend()
	{
		std::string params = "host=localhost,user=synthese,passwd=synthese";
		if (getenv("SYNTHESE_MYSQL_PARAMS"))
			params = getenv("SYNTHESE_MYSQL_PARAMS");

		if (getenv("SYNTHESE_MYSQL_DB"))
			_dbName = getenv("SYNTHESE_MYSQL_DB");
		else
			_dbName = "synthese_test";

		_connectionStringWithoutDb = "mysql://debug=1," + params;

		Log::GetInstance().info("conn string: " + _connectionStringWithoutDb + " db: " + _dbName);

		// Try to connect and create the synthese db if it is missing.
		DBModule::SetConnectionString(getConnectionString());
		try
		{
			ModuleClassTemplate<DBModule>::PreInit();
		}
		catch (MySQLException& e)
		{
			const unsigned int UNKNOWN_DB = 1049;
			if (e.getErrno() != UNKNOWN_DB)
				throw;

			cout << "____________ creating db ____________" << endl;
			DBModule::SetConnectionString(_connectionStringWithoutDb);
			try
			{
				ModuleClassTemplate<DBModule>::PreInit();
				ModuleClassTemplate<DBModule>::Init();
			}
			catch(...) { }
			DBModule::GetDB()->execUpdate("CREATE DATABASE " + _dbName);
			ModuleClassTemplate<DBModule>::End();

			DBModule::SetConnectionString(getConnectionString());
			ModuleClassTemplate<DBModule>::PreInit();
		}
		ModuleClassTemplate<DBModule>::Init();
		ModuleClassTemplate<DBModule>::End();
	}


	~MySQLTestBackend()
	{
		if (getenv("SYNTHESE_MYSQL_KEEPDB"))
			return;

		cout << "____MySQLTestBackend:~MySQLTestBackend Dropping database" << endl;
		DBModule::SetConnectionString(getConnectionString());

		ScopedModule<DBModule> scopedDBModule;
		DBModule::GetDB()->execUpdate("DROP DATABASE IF EXISTS " + _dbName);
	}

	virtual const std::string getConnectionString() const
	{
		return _connectionStringWithoutDb + ",db=" + _dbName;
	}

	virtual std::string getName() const
	{
		return "mysql";
	}

	virtual void setUpDb() const
	{
		cout << "____MySQLTestBackend:setUpDb: Dropping existing database" << endl;
		DBModule::SetConnectionString(getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		DBModule::GetDB()->execUpdate("DROP DATABASE IF EXISTS " + _dbName);
		DBModule::GetDB()->execUpdate("CREATE DATABASE " + _dbName);
	}
};

#endif

void runForEachBackends(void (*callback)(const TestBackend& testBackend))
{
	std::cout << "=====================   Running SQLite backend tests   =====================" << std::endl;
	callback(SQLiteTestBackend());
#ifdef WITH_MYSQL
	std::cout << "=====================   Running MySQL backend tests   =====================" << std::endl;
	callback(MySQLTestBackend());
#endif
}


/// Extends DBTableSyncTemplate with empty rows{Added,Updated,Removed} methods.
template <class K>
class DBDummyTableSyncTemplate : public db::DBTableSyncTemplate<K>
{
public:
	void rowsAdded(
		db::DB* db,
		const db::DBResultSPtr& rows
	) {};

	void rowsUpdated(
		db::DB* db,
		const db::DBResultSPtr& rows
	) {};

	void rowsRemoved(
		db::DB* db,
		const db::RowIdList& rowIds
	) {};
};

#define DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TABLE) \
template<> void DBTableSyncTemplate<TABLE>::BeforeDelete( \
	util::RegistryKeyType id, \
	db::DBTransaction& transaction \
){ \
} \
template<> void DBTableSyncTemplate<TABLE>::AfterDelete( \
	util::RegistryKeyType id, \
	db::DBTransaction& transaction \
){ \
} \
template<> void DBTableSyncTemplate<TABLE>::LogRemoval( \
	const server::Session* session, \
	util::RegistryKeyType id \
){ \
} \
template<> bool DBTableSyncTemplate<TABLE>::CanDelete( \
	const server::Session* session, \
	util::RegistryKeyType object_id \
){ \
	return true; \
}

#endif // SYNTHESE_db_DbTestUtils_inc_hpp__
