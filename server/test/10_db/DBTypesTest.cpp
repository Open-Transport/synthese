/** DBTypesTest class implementation.
	@file DBTypesTest.cpp
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

#include "DateField.hpp"
#include "DBTestUtils.hpp"
#include "DeleteQuery.hpp"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"

#include <boost/test/unit_test.hpp>
#include <boost/logic/tribool.hpp>
#include <geos/geom/Point.h>

using boost::posix_time::not_a_date_time;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::gregorian::date;
using geos::geom::Geometry;
using geos::geom::Point;

class DummyObject
{
	RegistryKeyType _key;
	static RegistryKeyType _LastKey;

public:
	DummyObject() : _key(0 /* _LastKey++*/) {}

	RegistryKeyType getKey() const
	{
		return _key;
	}

	void setKey(RegistryKeyType key)
	{
		_key = key;
		_LastKey++;
	}
};

RegistryKeyType DummyObject::_LastKey(0);

class TestTypesTableSync:
	public DBDummyTableSyncTemplate<TestTypesTableSync>
{
 public:
	typedef DummyObject ObjectType;

	static const std::string COL_INTEGER32;
	static const std::string COL_INTEGER32_2;
	static const std::string COL_INTEGER64;
	static const std::string COL_COLUMN1;
	static const std::string COL_DOUBLE;
	static const std::string COL_TEXT;
	static const std::string COL_BOOLEAN;
	static const std::string COL_TRIBOOLEAN;
	static const std::string COL_TIME;
	static const std::string COL_DATE;
	static const std::string COL_TIMESTAMP;
	// TODO: test blob type.
	static const std::string COL_GEOMETRY;
	// TODO: schema currently limited to only one geometry column called "geometry"
	/*
	static const std::string COL_POINT;
	static const std::string COL_LINESTRING;
	static const std::string COL_POLYGON;
	static const std::string COL_MULTIPOINT;
	static const std::string COL_MULTILINESTRING;
	static const std::string COL_MULTIPOLYGON;
	static const std::string COL_GEOMETRYCOLLECTION;
	*/

	static db::DBResultSPtr AddedRows;

	void rowsAdded(
		db::DB* db,
		const db::DBResultSPtr& rows
	) {
		std::cout << "____ database result:" << std::endl;
		std::cout << *rows << std::endl;
		if (AddedRows.get())
		{
			throw Exception("rows weren't reset");
		}
		AddedRows = rows;
	};
};

const string TestTypesTableSync::COL_INTEGER32("integer32");
const string TestTypesTableSync::COL_INTEGER32_2("integer32_2");
const string TestTypesTableSync::COL_INTEGER64("integer64");
const string TestTypesTableSync::COL_DOUBLE("double");
const string TestTypesTableSync::COL_TEXT("text");
const string TestTypesTableSync::COL_BOOLEAN("boolean");
const string TestTypesTableSync::COL_TRIBOOLEAN("triboolean");
const string TestTypesTableSync::COL_TIME("time");
const string TestTypesTableSync::COL_DATE("date");
const string TestTypesTableSync::COL_TIMESTAMP("timestamp");

// TODO: Maybe test with a point instead?
const string TestTypesTableSync::COL_GEOMETRY("geometry");
// TODO: other geometry types not tested yet, see above
/*
const string TestTypesTableSync::COL_POINT("point");
const string TestTypesTableSync::COL_LINESTRING("linestring");
const string TestTypesTableSync::COL_POLYGON("polygon");
const string TestTypesTableSync::COL_MULTIPOINT("multipoint");
const string TestTypesTableSync::COL_MULTILINESTRING("multilinestring");
const string TestTypesTableSync::COL_MULTIPOLYGON("multipolygon");
const string TestTypesTableSync::COL_GEOMETRYCOLLECTION("geometrycollection");
*/

db::DBResultSPtr TestTypesTableSync::AddedRows;

namespace synthese
{
	template<> const string util::FactorableTemplate<DBTableSync,TestTypesTableSync>::FACTORY_KEY(
		"100 test types"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestTypesTableSync>::TABLE(
			"t100_testtypes", false, false
		);

		template<> const Field DBTableSyncTemplate<TestTypesTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestTypesTableSync::COL_INTEGER32, SQL_INTEGER),
			Field(TestTypesTableSync::COL_INTEGER32_2, SQL_INTEGER),
			Field(TestTypesTableSync::COL_INTEGER64, SQL_INTEGER),
			Field(TestTypesTableSync::COL_DOUBLE, SQL_DOUBLE),

			Field(TestTypesTableSync::COL_TEXT, SQL_TEXT),
			Field(TestTypesTableSync::COL_BOOLEAN, SQL_BOOLEAN),
			Field(TestTypesTableSync::COL_TRIBOOLEAN, SQL_BOOLEAN),
			Field(TestTypesTableSync::COL_TIME, SQL_TIME),
			Field(TestTypesTableSync::COL_DATE, SQL_DATE),
			Field(TestTypesTableSync::COL_TIMESTAMP, SQL_DATETIME),

			Field(TestTypesTableSync::COL_GEOMETRY, SQL_GEOM_POINT),
			// TODO: other geometry types not tested yet, see above
			/*
			Field(TestTypesTableSync::COL_POINT, SQL_GEOM_POINT),
			Field(TestTypesTableSync::COL_LINESTRING, SQL_GEOM_LINESTRING),
			Field(TestTypesTableSync::COL_POLYGON, SQL_GEOM_POLYGON),
			Field(TestTypesTableSync::COL_MULTIPOINT, SQL_GEOM_MULTIPOINT),
			Field(TestTypesTableSync::COL_MULTILINESTRING, SQL_GEOM_MULTILINESTRING),
			Field(TestTypesTableSync::COL_MULTIPOLYGON, SQL_GEOM_MULTIPOLYGON),
			Field(TestTypesTableSync::COL_GEOMETRYCOLLECTION, SQL_GEOM_GEOMETRYCOLLECTION),
			*/
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestTypesTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestTypesTableSync)
	}
}



void testTypes(const TestBackend& testBackend)
{
	DummyObject object;

	testBackend.setUpDb();
	{
		ScopedFactory<TestTypesTableSync> scopedTestTypesTableSync;

		{
			cout << "____Creating table t100_testtypes" << endl;
			DBModule::SetConnectionString(testBackend.getConnectionString());

			ScopedModule<DBModule> scopedDBModule;

			BOOST_CHECK_EQUAL(true, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedTable);
			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.MigratedSchema);
			BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedIndexes);

			BOOST_REQUIRE(!TestTypesTableSync::AddedRows->next());
			TestTypesTableSync::AddedRows.reset();

			ReplaceQuery<TestTypesTableSync> query(object);
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();
			query.addFieldNull();

			query.execute(boost::optional<DBTransaction&>());
			TestTypesTableSync::AddedRows.reset();
		}
		{
			cout << "____Loading table t100_testtypes again" << endl;
			DBModule::SetConnectionString(testBackend.getConnectionString());

			ScopedModule<DBModule> scopedDBModule;

			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedTable);
			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.MigratedSchema);
			BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedIndexes);

			BOOST_REQUIRE(TestTypesTableSync::AddedRows->next());

			BOOST_CHECK_EQUAL(0, TestTypesTableSync::AddedRows->getInt("integer32"));
			BOOST_CHECK_EQUAL(0, TestTypesTableSync::AddedRows->getLongLong("integer32"));
			BOOST_CHECK_EQUAL(boost::optional<int>(), TestTypesTableSync::AddedRows->getOptionalInt("integer32"));
			BOOST_CHECK_EQUAL(boost::optional<std::size_t>(), TestTypesTableSync::AddedRows->getOptionalUnsignedInt("integer32"));
			// 0 is interpreted as a boolean false.
			BOOST_CHECK_EQUAL(false, TestTypesTableSync::AddedRows->getBool("integer32"));

			BOOST_CHECK_EQUAL(0, TestTypesTableSync::AddedRows->getInt("integer64"));
			BOOST_CHECK_EQUAL(0, TestTypesTableSync::AddedRows->getLongLong("integer64"));
			BOOST_CHECK_EQUAL(boost::optional<int>(), TestTypesTableSync::AddedRows->getOptionalInt("integer64"));
			BOOST_CHECK_EQUAL(boost::optional<std::size_t>(), TestTypesTableSync::AddedRows->getOptionalUnsignedInt("integer64"));
			// 0 is interpreted as a boolean false.
			BOOST_CHECK_EQUAL(false, TestTypesTableSync::AddedRows->getBool("integer64"));

			BOOST_CHECK_EQUAL(0.0, TestTypesTableSync::AddedRows->getDouble("double"));
			BOOST_CHECK_EQUAL("", TestTypesTableSync::AddedRows->getText("text"));

			BOOST_CHECK_EQUAL(false, TestTypesTableSync::AddedRows->getBool("boolean"));
			// Note: the only way to check for an indeterminate value is to use the indeterminate function.
			BOOST_CHECK(boost::logic::indeterminate(TestTypesTableSync::AddedRows->getTribool("triboolean")));

			BOOST_CHECK_EQUAL(time_duration(not_a_date_time), TestTypesTableSync::AddedRows->getHour("time"));
			BOOST_CHECK_EQUAL(date(not_a_date_time), TestTypesTableSync::AddedRows->getDate("date"));
			BOOST_CHECK_EQUAL(ptime(not_a_date_time), TestTypesTableSync::AddedRows->getDateTime("timestamp"));

			BOOST_CHECK(0 == TestTypesTableSync::AddedRows->getGeometryFromWKT("geometry").get());

			BOOST_REQUIRE(!TestTypesTableSync::AddedRows->next());

			TestTypesTableSync::AddedRows.reset();

			// Delete data
			DeleteQuery<TestTypesTableSync> deleteQuery;
			deleteQuery.addWhereField(TABLE_COL_ID, object.getKey());
			deleteQuery.execute(boost::optional<DBTransaction&>());

			// TODO: use a select query to check that the table is empty.
		}
		{
			cout << "____Creating table t100_testtypes" << endl;
			DBModule::SetConnectionString(testBackend.getConnectionString());

			ScopedModule<DBModule> scopedDBModule;
			CoordinatesSystem::SetDefaultCoordinatesSystems(4326); // WGS84

			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedTable);
			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.MigratedSchema);
			BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedIndexes);

			BOOST_REQUIRE(!TestTypesTableSync::AddedRows->next());
			TestTypesTableSync::AddedRows.reset();

			DummyObject object;

			ReplaceQuery<TestTypesTableSync> query(object);
			query.addField(static_cast<int>(-2147483648LL));
			query.addField(43);
			query.addField(static_cast<unsigned long long int>(9223372036854775807ULL));
			query.addField(12345.12345);
			query.addField(std::string("Foo bar blah '\\"));
			query.addField(true);
			query.addField(boost::tribool::false_value);
			// TODO: there is no ValueExpression<time_duration>
			query.addField(std::string("22:00")); // time_duration(22, 0, 0));
			query.addFrameworkField<DateField>(date(2011, 03, 30));
			query.addFrameworkField<PtimeField>(ptime(date(1995, 12, 07), time_duration(13, 25, 10)));

			boost::shared_ptr<Geometry> geom(CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(6, 10.3));
			query.addField(geom);

			query.execute(boost::optional<DBTransaction&>());
			TestTypesTableSync::AddedRows.reset();

			ModuleClassTemplate<DBModule>::End();

			cout << "____Loading table t100_testtypes again" << endl;
			DBModule::SetConnectionString(testBackend.getConnectionString());

			ModuleClassTemplate<DBModule>::PreInit();
			ModuleClassTemplate<DBModule>::Init();
			CoordinatesSystem::SetDefaultCoordinatesSystems(4326); // WGS84

			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedTable);
			BOOST_CHECK_EQUAL(false, DBTableSyncTemplate<TestTypesTableSync>::TABLE.MigratedSchema);
			BOOST_CHECK_EQUAL(0, DBTableSyncTemplate<TestTypesTableSync>::TABLE.CreatedIndexes);

			BOOST_REQUIRE(TestTypesTableSync::AddedRows->next());

			BOOST_CHECK_EQUAL(-2147483648LL, TestTypesTableSync::AddedRows->getInt("integer32"));
			BOOST_CHECK_EQUAL(-2147483648LL, TestTypesTableSync::AddedRows->getLongLong("integer32"));
			BOOST_CHECK_EQUAL(boost::optional<int>(-2147483648LL), TestTypesTableSync::AddedRows->getOptionalInt("integer32"));
			BOOST_CHECK_EQUAL(boost::optional<std::size_t>(-2147483648LL), TestTypesTableSync::AddedRows->getOptionalUnsignedInt("integer32"));
			// negative values are interpreted as a boolean false.
			BOOST_CHECK_EQUAL(false, TestTypesTableSync::AddedRows->getBool("integer32"));

			BOOST_CHECK_EQUAL(43, TestTypesTableSync::AddedRows->getInt("integer32_2"));
			BOOST_CHECK_EQUAL(43, TestTypesTableSync::AddedRows->getLongLong("integer32_2"));
			BOOST_CHECK_EQUAL(boost::optional<int>(43), TestTypesTableSync::AddedRows->getOptionalInt("integer32_2"));
			BOOST_CHECK_EQUAL(boost::optional<std::size_t>(43), TestTypesTableSync::AddedRows->getOptionalUnsignedInt("integer32_2"));
			// positive values are interpreted as a boolean true.
			BOOST_CHECK_EQUAL(true, TestTypesTableSync::AddedRows->getBool("integer32_2"));

			BOOST_CHECK_EQUAL(9223372036854775807LL, TestTypesTableSync::AddedRows->getLongLong("integer64"));
			// positive values are interpreted as a boolean true.
			BOOST_CHECK_EQUAL(true, TestTypesTableSync::AddedRows->getBool("integer64"));

			BOOST_CHECK_EQUAL(12345.12345, TestTypesTableSync::AddedRows->getDouble("double"));
			BOOST_CHECK_EQUAL(std::string("Foo bar blah '\\"), TestTypesTableSync::AddedRows->getText("text"));

			BOOST_CHECK_EQUAL(true, TestTypesTableSync::AddedRows->getBool("boolean"));
			BOOST_CHECK_EQUAL(boost::tribool::false_value, TestTypesTableSync::AddedRows->getTribool("triboolean"));

			BOOST_CHECK_EQUAL(time_duration(22, 0, 0), TestTypesTableSync::AddedRows->getHour("time"));
			BOOST_CHECK_EQUAL(date(2011, 03, 30), TestTypesTableSync::AddedRows->getDate("date"));
			BOOST_CHECK_EQUAL(ptime(date(1995, 12, 07), time_duration(13, 25, 10)), TestTypesTableSync::AddedRows->getDateTime("timestamp"));

			boost::shared_ptr<Point> point = boost::dynamic_pointer_cast<Point, Geometry>(
				TestTypesTableSync::AddedRows->getGeometryFromWKT("geometry")
			);
			BOOST_CHECK_CLOSE(6.0, point->getX(), 1.0);
			BOOST_CHECK_CLOSE(10.3, point->getY(), 1.0);

			BOOST_REQUIRE(!TestTypesTableSync::AddedRows->next());

			TestTypesTableSync::AddedRows.reset();
		}
	}
}


BOOST_AUTO_TEST_CASE(Types)
{
	runForEachBackends(testTypes);

	//testTypes(SQLiteTestBackend());
	//testTypes(MySQLTestBackend());
}
