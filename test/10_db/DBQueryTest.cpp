
/** QueryTest class implementation.
	@file QueryTest.cpp
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
#include "ReplaceQuery.h"

void testQuery(const TestBackend& testBackend)
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
	BOOST_CHECK_EQUAL(registry.size(), 0);

	TestObject obj;
	obj.setNetworkId(10);
	obj.setName("sample name");
	obj.setShortName("some short name");

	///////////////////////////////////////////////////////////////////////////
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
		BOOST_REQUIRE_EQUAL(registry.size(), 1);
		util::Registry<TestObject>::value_type regValue(*registry.begin());
		boost::shared_ptr<TestObject> objFromReg = regValue.second;

		cout << " obj key " << objFromReg->getKey() << " name " << objFromReg->getName() << endl;
		// Object from registry is newly allocated.
		BOOST_CHECK_NE(&obj, objFromReg.get());
		BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
		BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), 12);
		BOOST_CHECK_EQUAL(objFromReg->getName(), "sample name");
		BOOST_CHECK_EQUAL(objFromReg->getShortName(), "some short name");
	}


	///////////////////////////////////////////////////////////////////////////
	{
		cout << "____ UpdateQuery in a transaction with multiple updates" << endl;

		DBTransaction transaction;

		UpdateQuery<TestTableSync> query;
		query.addUpdateField(
			TestTableSync::COL_NETWORK_ID,
			99
		);
		query.addWhereField(TestTableSync::COL_NAME, string("sample name"));
		query.execute(transaction);

		UpdateQuery<TestTableSync> query2;
		query2.addUpdateField(
			TestTableSync::COL_NETWORK_ID,
			15
		);
		query2.addWhereField(TestTableSync::COL_NAME, string("sample name"));
		query2.execute(transaction);

		transaction.run();

		CHECK_COUNTERS(1, 1, 0);
		BOOST_REQUIRE_EQUAL(registry.size(), 1);
		util::Registry<TestObject>::value_type regValue(*registry.begin());
		boost::shared_ptr<TestObject> objFromReg = regValue.second;

		cout << " obj key " << objFromReg->getKey() << " name " << objFromReg->getName() << endl;
		// Object from registry is newly allocated.
		BOOST_CHECK_NE(&obj, objFromReg.get());
		BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
		BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), 15);
		BOOST_CHECK_EQUAL(objFromReg->getName(), "sample name");
		BOOST_CHECK_EQUAL(objFromReg->getShortName(), "some short name");
	}

	///////////////////////////////////////////////////////////////////////////
	{
		cout << "____ DeleteQuery and UpdateQuery in a transaction" << endl;

		// Add another object
		TestObject obj2;
		obj2.setNetworkId(20);
		obj2.setName("sample name 2");
		obj2.setShortName("some short name 2");
		TestTableSync::Save(&obj2);
		CHECK_COUNTERS(1, 0, 1);

		DBTransaction transaction;

		// Delete obj2
		DeleteQuery<TestTableSync> deleteQuery;
		deleteQuery.addWhereField(TestTableSync::COL_NETWORK_ID, 15, ComposedExpression::OP_SUP);
		deleteQuery.execute(transaction);

		// Update all objects
		UpdateQuery<TestTableSync> updateQuery;
		updateQuery.addUpdateField(
			TestTableSync::COL_NETWORK_ID,
			16
		);
		updateQuery.addWhereField(TestTableSync::COL_NETWORK_ID, 100, ComposedExpression::OP_INF);
		updateQuery.execute(transaction);

		transaction.run();

		CHECK_COUNTERS(1, 2, 0);
		BOOST_REQUIRE_EQUAL(registry.size(), 1);

		util::Registry<TestObject>::value_type regValue(*registry.begin());
		boost::shared_ptr<TestObject> objFromReg = regValue.second;

		cout << " obj key " << objFromReg->getKey() << " name " << objFromReg->getName() << endl;
		// Object from registry is newly allocated.
		BOOST_CHECK_NE(&obj, objFromReg.get());
		BOOST_CHECK_EQUAL(objFromReg->getKey(), obj.getKey());
		BOOST_CHECK_EQUAL(objFromReg->getNetworkId(), 16);
		BOOST_CHECK_EQUAL(objFromReg->getName(), "sample name");
		BOOST_CHECK_EQUAL(objFromReg->getShortName(), "some short name");
	}

	// Replace query and transactions
	{
		DBTransaction transaction;

		TestObject obj;
		synthese::db::ReplaceQuery<TestTableSync> insertQuery(obj);
		insertQuery.addField(RegistryKeyType(12));
		insertQuery.addField(string("test"));
		insertQuery.addField(string("test"));
		insertQuery.execute(transaction);
		transaction.run();
	}
}

BOOST_AUTO_TEST_CASE(Query)
{
	runForEachBackends(testQuery);

	//testQuery(SQLiteTestBackend());
	//testQuery(MySQLTestBackend());
}
