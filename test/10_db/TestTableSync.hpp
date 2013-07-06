/** A table sync implementation to be used by the tests.
	@file TestTableSync.hpp
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

#ifndef SYNTHESE_db_TestTableSync_hpp__
#define SYNTHESE_db_TestTableSync_hpp__

#include "10_db/DeleteQuery.hpp"
#include "10_db/ReplaceQuery.h"
#include "OldLoadSavePolicy.hpp"

#include <boost/optional.hpp>

using boost::optional;

class TestObject:
	public virtual util::Registrable
{
private:
	RegistryKeyType		_networkId;
	std::string			_name;
	std::string			_shortName;

public:
	typedef util::Registry<TestObject>	Registry;

	TestObject(util::RegistryKeyType key = 0) :
		Registrable(key)
	{};

	const RegistryKeyType getNetworkId() const { return _networkId; }
	virtual std::string getName() const { return _name; }
	const std::string& getShortName() const { return _shortName; }

	void setNetworkId(const RegistryKeyType networkId) { _networkId = networkId; }
	void setName(const std::string& name) { _name = name; }
	void setShortName(const std::string& shortName) { _shortName = shortName; }
};



class TestTableSync:
	public db::DBDirectTableSyncTemplate<TestTableSync,TestObject,db::FullSynchronizationPolicy,db::OldLoadSavePolicy>
{
public:
	static const std::string COL_NETWORK_ID;
	static const std::string COL_NAME;
	static const std::string COL_SHORT_NAME;

	static int LoadCount;
	static int UnlinkCount;
	static int SaveCount;

	static SearchResult Search(
		util::Env& env,
		boost::optional<util::RegistryKeyType> networkId = boost::optional<util::RegistryKeyType>(),
		boost::optional<std::string> name = boost::optional<std::string>(),
		boost::optional<std::string> shortName = boost::optional<std::string>(),
		util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
	);

	static void RemoveByName(
		const std::string& name
	);
};


const string TestTableSync::COL_NETWORK_ID("network_id");
const string TestTableSync::COL_NAME("name");
const string TestTableSync::COL_SHORT_NAME("short_name");

int TestTableSync::LoadCount(0);
int TestTableSync::UnlinkCount(0);
int TestTableSync::SaveCount(0);

// object
namespace synthese
{
	namespace util
	{
		template<> const std::string Registry<TestObject>::KEY("TestObject");
	}
}

// table sync
namespace synthese
{
	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TestTableSync>::FACTORY_KEY("10 Test Objects");
	}



	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TestTableSync>::TABLE(
			"t020_test"
		);



		template<> const Field DBTableSyncTemplate<TestTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TestTableSync::COL_NETWORK_ID, SQL_INTEGER),
			Field(TestTableSync::COL_NAME, SQL_TEXT),
			Field(TestTableSync::COL_SHORT_NAME, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TestTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					TestTableSync::COL_NETWORK_ID.c_str(),
			"")	);
			return r;
		}



		template<> void OldLoadSavePolicy<TestTableSync,TestObject>::Load(
			TestObject* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			cout << "_____________Load" << endl;
			TestTableSync::LoadCount++;

			object->setNetworkId(rows->getLongLong(TestTableSync::COL_NETWORK_ID));
			object->setName(rows->getText(TestTableSync::COL_NAME));
			object->setShortName(rows->getText(TestTableSync::COL_SHORT_NAME));
		}



		template<> void OldLoadSavePolicy<TestTableSync,TestObject>::Unlink(
			TestObject* obj
		){
			cout << "_____________Unlink" << endl;
			TestTableSync::UnlinkCount++;
		}



		template<> void OldLoadSavePolicy<TestTableSync,TestObject>::Save(
			TestObject* object,
			optional<DBTransaction&> transaction
		){
			cout << "_____________Save" << endl;
			TestTableSync::SaveCount++;

			ReplaceQuery<TestTableSync> query(*object);
			query.addField(object->getNetworkId());
			query.addField(object->getName());
			query.addField(object->getShortName());
			query.execute(transaction);
		}



		DEFINE_EMPTY_TABLESYNC_DELETE_METHODS(TestTableSync)
	}
}

TestTableSync::SearchResult TestTableSync::Search(
	util::Env& env,
	boost::optional<util::RegistryKeyType> networkId,
	boost::optional<std::string> name,
	boost::optional<std::string> shortName,
	util::LinkLevel linkLevel
	)
{
	SelectQuery<TestTableSync> query;

	if (networkId)
	{
		query.addWhereField(COL_NETWORK_ID, *networkId);
	}

	if (name)
	{
		query.addWhereField(COL_NAME, *name);
	}

	if (shortName)
	{
		query.addWhereField(COL_SHORT_NAME, *shortName);
	}

	return LoadFromQuery(query, env, linkLevel);
}

void TestTableSync::RemoveByName(const std::string& name)
{
	DeleteQuery<TestTableSync> query;
	query.addWhereField(COL_NAME, name);
	query.execute();
}

#define RESET_COUNTERS \
	TestTableSync::LoadCount = TestTableSync::UnlinkCount = TestTableSync::SaveCount = 0

// This is a macro to have the line numbers from the call site in case of failure.
#define CHECK_COUNTERS(loadCount, unlinkCount, saveCount) do \
{ \
	BOOST_CHECK_EQUAL(TestTableSync::LoadCount, loadCount); \
	TestTableSync::LoadCount = 0; \
	BOOST_CHECK_EQUAL(TestTableSync::UnlinkCount, unlinkCount); \
	TestTableSync::UnlinkCount = 0; \
	BOOST_CHECK_EQUAL(TestTableSync::SaveCount, saveCount); \
	TestTableSync::SaveCount = 0; \
} while(0);


#endif // SYNTHESE_db_TestTableSync_hpp__
