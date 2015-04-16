
/** MySQLTriggerTest class implementation.
	@file MySQLTriggerTest.cpp
	@author Sylvain Pasche
	@date 2011

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

// At first to avoid a macro double definition
#include "ServerModule.h"

#include "DBTestUtils.hpp"
#include "TestTableSync.hpp"

#include "01_util/threads/Thread.h"

#include <my_global.h>
#include <mysql.h>

using synthese::server::ServerModule;

typedef synthese::db::DB::ConnectionInfo ConnectionInfo;

// TODO: make it configurable?
const string PORT = "4080";

class MySQLWithHTTPServerTestBackend : public MySQLTestBackend
{
	// XXX plain object?
	boost::shared_ptr<ScopedModule<ServerModule> > _scopedServerModule;

public:
	MySQLWithHTTPServerTestBackend()
	{
		// Set this manually in case the HTTP server is not used. That parameter will be
		// used by the MySQL backend for the trigger URL.
		ModuleClass::SetParameter(ServerModule::MODULE_PARAM_PORT, PORT);

		ModuleClass::Parameters defaultParams;
		defaultParams[ServerModule::MODULE_PARAM_LOG_LEVEL] = "-1";
		defaultParams[ServerModule::MODULE_PARAM_PORT] = PORT;
		ModuleClass::SetDefaultParameters(defaultParams);
		_scopedServerModule.reset(new ScopedModule<ServerModule>());
		ServerModule::RunHTTPServer();
	}
};

BOOST_AUTO_TEST_CASE(MySQLTrigger)
{
	MySQLWithHTTPServerTestBackend testBackend;

	ConnectionInfo connInfo(testBackend.getConnectionString());

	MYSQL* connection;
	connection = mysql_init(NULL);
	BOOST_REQUIRE(connection != NULL);

	BOOST_REQUIRE(
		mysql_real_connect(
			connection, connInfo.host.c_str(), connInfo.user.c_str(),
			connInfo.passwd.c_str(), connInfo.db.c_str(),
			connInfo.port, NULL, CLIENT_MULTI_STATEMENTS
		) != NULL
	);


	ScopedRegistrable<TestObject> scopedTestObject;
	ScopedFactory<TestTableSync> scopedTestTableSync;
	testBackend.setUpDb();

	RESET_COUNTERS;

	DBModule::SetConnectionString(testBackend.getConnectionString());

	ScopedModule<DBModule> scopedDBModule;

	BOOST_CHECK_EQUAL(true, DBTableSyncTemplate<TestTableSync>::TABLE.CreatedTable);
	BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTableSync>::TABLE.MigratedSchema);
	BOOST_CHECK_EQUAL(1, DBTableSyncTemplate<TestTableSync>::TABLE.CreatedIndexes);

	// Start with a clean registry.
	util::Env::GetOfficialEnv().clear();

	util::Env& env(util::Env::GetOfficialEnv());
	util::Registry<TestObject>& registry(env.getEditableRegistry<TestObject>());
	BOOST_CHECK_EQUAL(registry.size(), 0);

	TestObject obj;
	obj.setNetworkId(22);
	obj.setName("sample name");
	obj.setShortName("some short name");

	// object creation

	TestTableSync::Save(&obj);

	CHECK_COUNTERS(1, 0, 1);
	// Check that the object is now in the registry.
	BOOST_REQUIRE_EQUAL(registry.size(), 1);
	util::Registry<TestObject>::value_type regValue(*registry.begin());
	boost::shared_ptr<TestObject> objFromReg = regValue.second;

	cout << " obj key " << objFromReg->getKey() << " name " << objFromReg->getName() << endl;
	// Object from registry is newly allocated.
	BOOST_CHECK_NE(&obj, objFromReg.get());
	BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
	BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), obj.getNetworkId());
	BOOST_CHECK_EQUAL(objFromReg->getName(), obj.getName());
	BOOST_CHECK_EQUAL(objFromReg->getShortName(), obj.getShortName());

	string sql("UPDATE t020_test SET name='new name' WHERE name='sample name';");

	int res = mysql_query(connection, sql.c_str());
	if (res)
	{
		cout << "mysql_query error: " << mysql_error(connection) << endl;
	}
	BOOST_REQUIRE(!res);

	cout << "Waiting for trigger callback" << endl;
	util::Thread::Sleep(400);

	// Check that the object was updated through the trigger.
	CHECK_COUNTERS(1, 1, 0);
	BOOST_REQUIRE_EQUAL(registry.size(), 1);
	objFromReg = registry.begin()->second;
	BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
	BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), obj.getNetworkId());
	BOOST_CHECK_EQUAL(objFromReg->getName(), "new name");
	BOOST_CHECK_EQUAL(objFromReg->getShortName(), obj.getShortName());


	// TODO: test deleting rows, insert rows, updating several rows at the same time.
}
