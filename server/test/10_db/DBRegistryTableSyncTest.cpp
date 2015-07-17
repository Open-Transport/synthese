/** DBTableSyncTest class implementation.
	@file ConnectionInfoTest.cpp
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

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sstream>

using namespace std;

using namespace synthese;
using namespace synthese::db;
using namespace synthese::util;

using boost::shared_ptr;

void testRegistryTableSync(const TestBackend& testBackend)
{
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
	obj.setNetworkId(22);
	obj.setName("sample name");
	obj.setShortName("some short name");

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

	// save the object again, it will call Unlink and Load.
	TestTableSync::Save(&obj);

	CHECK_COUNTERS(1, 1, 1);
	BOOST_REQUIRE_EQUAL(registry.size(), 1ULL);

	// querying

	TestTableSync::SearchResult results = TestTableSync::Search(util::Env::GetOfficialEnv(), 999);
	BOOST_CHECK_EQUAL(results.size(), 0);

	results = TestTableSync::Search(util::Env::GetOfficialEnv(), 22, std::string("sample name"));
	BOOST_CHECK_EQUAL(results.size(), 1);
	boost::shared_ptr<TestObject> objFromResult = results[0];
	BOOST_CHECK_EQUAL(objFromResult->getNetworkId(), obj.getNetworkId());
	BOOST_CHECK_EQUAL(objFromResult->getName(), obj.getName());
	BOOST_CHECK_EQUAL(objFromResult->getShortName(), obj.getShortName());

	// removal

	TestTableSync::RemoveByName("non existing name");

	CHECK_COUNTERS(0, 0, 0);
	// Nothing should be removed from registry.
	BOOST_CHECK_EQUAL(registry.size(), 1);

	TestTableSync::RemoveByName("sample name");

	CHECK_COUNTERS(0, 1, 0);
	// Removed from registry
	BOOST_CHECK_EQUAL(registry.size(), 0);

	// test removal with DBTableSyncTemplate::Remove

	TestObject obj2;
	obj2.setNetworkId(900);
	obj2.setName("Foo");
	obj2.setShortName("Bar");
	TestTableSync::Save(&obj2);

	CHECK_COUNTERS(1, 0, 1);
	BOOST_REQUIRE_EQUAL(registry.size(), 1ULL);

	db::DBModule::GetDB()->deleteStmt(obj2.getKey(), optional<db::DBTransaction&>());

	CHECK_COUNTERS(0, 1, 0);
	BOOST_CHECK_EQUAL(registry.size(), 0);

	// test transactions
	{
		DBTransaction transaction;

		TestObject obj1;
		obj1.setNetworkId(100);
		obj1.setName("transaction obj1");
		obj1.setShortName("t1");

		TestObject obj2;
		obj2.setNetworkId(200);
		obj2.setName("transaction obj2");
		obj2.setShortName("t2");

		TestTableSync::Save(&obj1, transaction);

		CHECK_COUNTERS(0, 0, 1);
		// Object is not yet in the registry
		BOOST_REQUIRE_EQUAL(registry.size(), 0ULL);

		TestTableSync::Save(&obj2, transaction);

		CHECK_COUNTERS(0, 0, 1);
		// Object is not yet in the registry
		BOOST_REQUIRE_EQUAL(registry.size(), 0ULL);

		db::DBModule::GetDB()->deleteStmt(obj2.getKey(), transaction);

		CHECK_COUNTERS(0, 0, 0);

		transaction.run();

		CHECK_COUNTERS(1, 0, 0);
		BOOST_REQUIRE_EQUAL(registry.size(), 1ULL);
	}
}

BOOST_AUTO_TEST_CASE(RegistryTableSync)
{
	runForEachBackends(testRegistryTableSync);

	//testRegistryTableSync(SQLiteTestBackend());
	//testRegistryTableSync(MySQLTestBackend());
}
