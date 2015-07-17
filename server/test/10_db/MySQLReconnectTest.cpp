
/** MySQLReconnectTest class implementation.
	@file MySQLReconnectTest.cpp
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

#include "DBTestUtils.hpp"
#include "TestTableSync.hpp"

#include "10_db/UpdateQuery.hpp"

BOOST_AUTO_TEST_CASE(MySQLReconnect)
{
	MySQLTestBackend testBackend;

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
	BOOST_CHECK_EQUAL(registry.size(), 0ULL);

	TestObject obj;
	obj.setNetworkId(10);
	obj.setName("sample name");
	obj.setShortName("some short name");

	///////////////////////////////////////////////////////////////////////////
	// object creation

	TestTableSync::Save(&obj);

	CHECK_COUNTERS(1, 0, 1);
	// Check that the object is now in the registry.
	BOOST_REQUIRE_EQUAL(registry.size(), 1ULL);
	util::Registry<TestObject>::value_type regValue(*registry.begin());
	boost::shared_ptr<TestObject> objFromReg = regValue.second;

	cout << " obj key " << objFromReg->getKey() << " name " << objFromReg->getName() << endl;
	// Object from registry is newly allocated.
	BOOST_CHECK_NE(&obj, objFromReg.get());
	BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
	BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), obj.getNetworkId());
	BOOST_CHECK_EQUAL(objFromReg->getName(), obj.getName());
	BOOST_CHECK_EQUAL(objFromReg->getShortName(), obj.getShortName());

	// Check that the trigger_metadata contains our connection identifier.
	DBResultSPtr rows = DBModule::GetDB()->execQuery(
		"SELECT synthese_conn_id = CONNECTION_ID() as is_conn_id FROM trigger_metadata;"
	);
	BOOST_REQUIRE(rows->next());
	BOOST_CHECK_EQUAL(true, rows->getBool("is_conn_id"));

	///////////////////////////////////////////////////////////////////////////
	// server restart.

	/*
		Example sudo configuration:

		unix-user-running-tests ALL=NOPASSWD: /etc/init.d/mysql restart

		Then, set:
		export SYNTHESE_MYSQL_RESTART_COMMAND='sudo /etc/init.d/mysql restart'
	*/
	if (getenv("SYNTHESE_MYSQL_RESTART_COMMAND"))
	{
		::system(getenv("SYNTHESE_MYSQL_RESTART_COMMAND"));
	}
	else if (getenv("SYNTHESE_MYSQL_RESTART_MANUAL"))
	{
		cout << "Restart the MySQL server and press enter to continue" << endl;
		cin.get();
	}
	else
	{
		cout <<
			"WARNING: Neither SYNTHESE_MYSQL_RESTART_COMMAND or SYNTHESE_MYSQL_RESTART_MANUAL "
			"environment variable defined. Can't run test" << endl;
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	{
		cout << "____ UpdateQuery" << endl;

		UpdateQuery<TestTableSync> query;
		query.addUpdateField(
			TestTableSync::COL_NETWORK_ID,
			12
		);
		query.addWhereField(TestTableSync::COL_NAME, string("sample name"));
		query.execute();

		CHECK_COUNTERS(1, 1, 0);
		BOOST_REQUIRE_EQUAL(registry.size(), 1ULL);
		util::Registry<TestObject>::value_type regValue(*registry.begin());
		boost::shared_ptr<TestObject> objFromReg = regValue.second;

		cout << " obj key " << objFromReg->getKey() << " name " << objFromReg->getName() << endl;
		// Object from registry is newly allocated.
		BOOST_CHECK_NE(&obj, objFromReg.get());
		BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
		BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), 12ULL);
		BOOST_CHECK_EQUAL(objFromReg->getName(), "sample name");
		BOOST_CHECK_EQUAL(objFromReg->getShortName(), "some short name");

		// Check that the trigger_metadata contains our connection identifier.
		DBResultSPtr rows = DBModule::GetDB()->execQuery(
			"SELECT synthese_conn_id = CONNECTION_ID() as is_conn_id FROM trigger_metadata;"
			);
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL(true, rows->getBool("is_conn_id"));
	}
}
