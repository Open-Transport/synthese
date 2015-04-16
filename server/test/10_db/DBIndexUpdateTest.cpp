/** DBIndexUpdateTest class implementation.
	@file DBIndexUpdateTest.cpp
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

class TestIndexTableSync:
	public DBDummyTableSyncTemplate<TestIndexTableSync>
{
 public:

	static const std::string COL_COLUMN0;
	static const std::string COL_COLUMN1;
};

const string TestIndexTableSync::COL_COLUMN0("column0");
const string TestIndexTableSync::COL_COLUMN1("column1");

namespace synthese
{
	template<> const string util::FactorableTemplate<DBTableSync,TestIndexTableSync>::FACTORY_KEY(
		"102 test index"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestIndexTableSync>::TABLE(
			"t102_testindex", false, false
		);

		template<> const Field DBTableSyncTemplate<TestIndexTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestIndexTableSync::COL_COLUMN0, SQL_TEXT),
			Field(TestIndexTableSync::COL_COLUMN1, SQL_INTEGER),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestIndexTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					TestIndexTableSync::COL_COLUMN0.c_str(),
			"")	);
			return r;
		}


		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestIndexTableSync)
	}
}

class TestIndex2TableSync:
	public DBDummyTableSyncTemplate<TestIndex2TableSync>
{
public:
	static const std::string COL_COLUMN0;
	static const std::string COL_COLUMN1;
};

const string TestIndex2TableSync::COL_COLUMN0("column0");
const string TestIndex2TableSync::COL_COLUMN1("column1");

namespace synthese
{
	template<> const string util::FactorableTemplate<DBTableSync,TestIndex2TableSync>::FACTORY_KEY(
		"101 test index 2"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestIndex2TableSync>::TABLE(
			"t102_testindex", false, false
			);

		template<> const Field DBTableSyncTemplate<TestIndex2TableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestIndex2TableSync::COL_COLUMN0, SQL_TEXT),
			Field(TestIndex2TableSync::COL_COLUMN1, SQL_INTEGER),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestIndex2TableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					TestIndex2TableSync::COL_COLUMN0.c_str(),
			"")	);
			r.push_back(
					DBTableSync::Index(
					TestIndex2TableSync::COL_COLUMN0.c_str(),
					TestIndex2TableSync::COL_COLUMN1.c_str(),
			"")	);
			return r;
		}



		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestIndex2TableSync)
	}
}


void testIndexUpdate(const TestBackend& testBackend)
{
	testBackend.setUpDb();
	{
		cout << "____Creating table with index" << endl;
		ScopedFactory<TestIndexTableSync> scopedTestIndexTableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		BOOST_CHECK_EQUAL(true, DBTableSyncTemplate<TestIndexTableSync>::TABLE.CreatedTable);
		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestIndexTableSync>::TABLE.MigratedSchema);
		BOOST_CHECK_EQUAL(1, DBTableSyncTemplate<TestIndexTableSync>::TABLE.CreatedIndexes);

		DBTableSync::Indexes indexes(DBTableSyncTemplate<TestIndexTableSync>::GetIndexes());
		BOOST_CHECK(DBModule::GetDB()->doesIndexExist(
			"t102_testindex",
			indexes[0]
		));
	}
	{
		cout << "____No index update" << endl;
		ScopedFactory<TestIndexTableSync> scopedTestIndexTableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestIndexTableSync>::TABLE.CreatedTable);
		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestIndexTableSync>::TABLE.MigratedSchema);
		BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestIndexTableSync>::TABLE.CreatedIndexes);

		DBTableSync::Indexes indexes(DBTableSyncTemplate<TestIndexTableSync>::GetIndexes());
		BOOST_CHECK(DBModule::GetDB()->doesIndexExist(
			"t102_testindex",
			indexes[0]
		));
	}
	{
		cout << "____Index update" << endl;
		ScopedFactory<TestIndex2TableSync> scopedTestIndex2TableSync;

		DBModule::SetConnectionString(testBackend.getConnectionString());

		ScopedModule<DBModule> scopedDBModule;

		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestIndex2TableSync>::TABLE.CreatedTable);
		BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestIndex2TableSync>::TABLE.MigratedSchema);
		BOOST_CHECK_EQUAL(1, DBTableSyncTemplate<TestIndex2TableSync>::TABLE.CreatedIndexes);

		DBTableSync::Indexes indexes(DBTableSyncTemplate<TestIndex2TableSync>::GetIndexes());
		BOOST_CHECK(DBModule::GetDB()->doesIndexExist(
			"t102_testindex",
			indexes[0]
		));
		BOOST_CHECK(DBModule::GetDB()->doesIndexExist(
			"t102_testindex",
			indexes[1]
		));
	}
}



BOOST_AUTO_TEST_CASE(IndexUpdate)
{
	runForEachBackends(testIndexUpdate);
}
