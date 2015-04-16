/** DBSchemaUpdateTest class implementation.
	@file DBSchemaUpdateTest.cpp
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

#include <boost/test/unit_test.hpp>

class TestSchemaTableSync:
	public DBDummyTableSyncTemplate<TestSchemaTableSync>
{
public:
	static const std::string COL_COLUMN0;
	static const std::string COL_COLUMN1;
};

const string TestSchemaTableSync::COL_COLUMN0("column0");
const string TestSchemaTableSync::COL_COLUMN1("column1");

namespace synthese
{
	template<> const string util::FactorableTemplate<DBTableSync,TestSchemaTableSync>::FACTORY_KEY(
		"100 test schema"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestSchemaTableSync>::TABLE(
			"t100_testschema", false, false
		);

		template<> const Field DBTableSyncTemplate<TestSchemaTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestSchemaTableSync::COL_COLUMN0, SQL_TEXT),
			Field(TestSchemaTableSync::COL_COLUMN1, SQL_INTEGER),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestSchemaTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestSchemaTableSync)
	}
}



// Table sync with a new column

class TestSchema2TableSync:
	public DBDummyTableSyncTemplate<TestSchema2TableSync>
{
 public:
	static const std::string COL_COLUMN0;
	static const std::string COL_COLUMN1;
	static const std::string COL_COLUMN2;
};

const string TestSchema2TableSync::COL_COLUMN0("column0");
const string TestSchema2TableSync::COL_COLUMN1("column1");
const string TestSchema2TableSync::COL_COLUMN2("column2");

namespace synthese
{
	template<> const string util::FactorableTemplate<DBTableSync,TestSchema2TableSync>::FACTORY_KEY(
		"100 test schema 2"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestSchema2TableSync>::TABLE(
			"t100_testschema", false, false
		);

		template<> const Field DBTableSyncTemplate<TestSchema2TableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestSchema2TableSync::COL_COLUMN0, SQL_TEXT),
			Field(TestSchema2TableSync::COL_COLUMN1, SQL_INTEGER),
			Field(TestSchema2TableSync::COL_COLUMN2, SQL_GEOM_POINT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestSchema2TableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestSchema2TableSync)
	}
}


// This table uses another table name than TestSchemaTableSync and TestSchema2TableSync

class TestSchema3TableSync:
	public DBDummyTableSyncTemplate<TestSchema3TableSync>
{
 public:
	static const std::string COL_COLUMN0;
	static const std::string COL_COLUMN1;
	static const std::string COL_COLUMN2;
};

const string TestSchema3TableSync::COL_COLUMN0("column0");
const string TestSchema3TableSync::COL_COLUMN1("column1");
const string TestSchema3TableSync::COL_COLUMN2("column2");


namespace synthese
{
	template<> const string util::FactorableTemplate<DBTableSync,TestSchema3TableSync>::FACTORY_KEY(
		"101 test schema 3"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestSchema3TableSync>::TABLE(
			"t101_testschema3", false, false
		);

		template<> const Field DBTableSyncTemplate<TestSchema3TableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestSchema3TableSync::COL_COLUMN0, SQL_TEXT),
			Field(TestSchema3TableSync::COL_COLUMN1, SQL_DOUBLE),
			Field(TestSchema3TableSync::COL_COLUMN2, SQL_BOOLEAN),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestSchema3TableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestSchema3TableSync)
	}
}


void testSchemaUpdate(const TestBackend& testBackend)
{
	testBackend.setUpDb();
	{
		cout << "____Creating table t100_testschema" << endl;
		ScopedFactory<TestSchemaTableSync> scopedTestSchemaTableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		BOOST_CHECK_EQUAL(true, DBTableSyncTemplate<TestSchemaTableSync>::TABLE.CreatedTable);
		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestSchemaTableSync>::TABLE.MigratedSchema);
		BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestSchemaTableSync>::TABLE.CreatedIndexes);

		std::stringstream query;
		query << "INSERT INTO t100_testschema VALUES (" <<
			DBTableSyncTemplate<TestSchemaTableSync>::getId() <<
			", 'foo', 12)";
		DBModule::GetDB()->execUpdate(query.str());
		query.str("");
		query << "INSERT INTO t100_testschema VALUES (" <<
			DBTableSyncTemplate<TestSchemaTableSync>::getId() <<
			", 'bar', 48)";
		DBModule::GetDB()->execUpdate(query.str());

		DBResultSPtr rows = DBModule::GetDB()->execQuery("SELECT * FROM t100_testschema");
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("foo", rows->getText("column0"));
		BOOST_CHECK_EQUAL(12, rows->getInt("column1"));
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("bar", rows->getText("column0"));
		BOOST_CHECK_EQUAL(48, rows->getInt("column1"));
		BOOST_CHECK(!rows->next());
		// Manual reset to release the SQLite handle.
		rows.reset();
	}
	{
		cout << "____No schema update" << endl;
		ScopedFactory<TestSchemaTableSync> scopedTestSchemaTableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestSchemaTableSync>::TABLE.CreatedTable);
		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestSchemaTableSync>::TABLE.MigratedSchema);
		BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestSchemaTableSync>::TABLE.CreatedIndexes);

		DBResultSPtr rows = DBModule::GetDB()->execQuery("SELECT * FROM t100_testschema");
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("foo", rows->getText("column0"));
		BOOST_CHECK_EQUAL(12, rows->getInt("column1"));
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("bar", rows->getText("column0"));
		BOOST_CHECK_EQUAL(48, rows->getInt("column1"));
		BOOST_CHECK(!rows->next());
		// Manual reset to release the SQLite handle.
		rows.reset();
	}
	{
		cout << "____Schema update because of new column" << endl;
		ScopedFactory<TestSchema2TableSync> scopedTestSchema2TableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestSchema2TableSync>::TABLE.CreatedTable);
		BOOST_CHECK_EQUAL(true, DBTableSyncTemplate<TestSchema2TableSync>::TABLE.MigratedSchema);
		BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestSchema2TableSync>::TABLE.CreatedIndexes);

		DBResultSPtr rows = DBModule::GetDB()->execQuery("SELECT * FROM t100_testschema");
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("foo", rows->getText("column0"));
		BOOST_CHECK_EQUAL(12, rows->getInt("column1"));
		BOOST_CHECK(!rows->getGeometryFromWKT("column2").get());

		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("bar", rows->getText("column0"));
		BOOST_CHECK_EQUAL(48, rows->getInt("column1"));
		BOOST_CHECK(!rows->getGeometryFromWKT("column2").get());

		BOOST_CHECK(!rows->next());
		// Manual reset to release the SQLite handle.
		rows.reset();
	}
	{
		//cout << "____Schema update because of column type change" << endl;
		// TODO: Test that schema is updated when a column type changes.
	}
	{
		cout << "____Creating table t101_testschema3" << endl;
		ScopedFactory<TestSchema3TableSync> scopedTestSchema3TableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		std::stringstream query;
		query << "INSERT INTO t101_testschema3 VALUES (" <<
			DBTableSyncTemplate<TestSchema3TableSync>::getId() <<
			" ,'foo''\\', -400.5, 1)";
		DBModule::GetDB()->execUpdate(query.str());
		query.str("");
		query << "INSERT INTO t101_testschema3 VALUES (" <<
			DBTableSyncTemplate<TestSchema3TableSync>::getId() <<
			", 'bar', 12345.78, 0)";
		DBModule::GetDB()->execUpdate(query.str());

		DBResultSPtr rows = DBModule::GetDB()->execQuery("SELECT * FROM t101_testschema3");
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("foo'\\", rows->getText("column0"));
		BOOST_CHECK_EQUAL(-400.5, rows->getDouble("column1"));
		BOOST_CHECK_EQUAL(true, rows->getBool("column2"));
		BOOST_REQUIRE(rows->next());
		BOOST_CHECK_EQUAL("bar", rows->getText("column0"));
		BOOST_CHECK_EQUAL(12345.78, rows->getDouble("column1"));
		BOOST_CHECK_EQUAL(false, rows->getBool("column2"));
		BOOST_CHECK(!rows->next());
		// Manual reset to release the SQLite handle.
		rows.reset();
	}
}



BOOST_AUTO_TEST_CASE(SchemaUpdate)
{
	runForEachBackends(testSchemaUpdate);
}
