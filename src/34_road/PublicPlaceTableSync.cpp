

/** PublicPlaceTableSync class implementation.
	@file PublicPlaceTableSync.cpp

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

#include <sstream>

#include "PublicPlaceTableSync.h"
#include "PublicPlace.h"
#include "CityTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync, PublicPlaceTableSync>::FACTORY_KEY("34.40.03 Public places");
		template<> const string FactorableTemplate<Fetcher<NamedPlace>, PublicPlaceTableSync>::FACTORY_KEY("13");
	}

	namespace road
	{
		const string PublicPlaceTableSync::COL_NAME ("name");
		const string PublicPlaceTableSync::COL_CITYID ("city_id");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<PublicPlaceTableSync>::TABLE(
			"t013_public_places"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<PublicPlaceTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PublicPlaceTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(PublicPlaceTableSync::COL_CITYID, SQL_INTEGER, false),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<PublicPlaceTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Load(
			PublicPlace* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			string name (rows->getText (PublicPlaceTableSync::COL_NAME));
			object->setName(name);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				City* city(CityTableSync::GetEditable(rows->getLongLong (PublicPlaceTableSync::COL_CITYID), env, linkLevel).get());
				object->setCity(city);

				city->addPlaceToMatcher<PublicPlace>(env.getEditableSPtr(object));
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Unlink(
			PublicPlace* obj
		){
			City* city(const_cast<City*>(obj->getCity()));
			if (city != NULL)
			{
//				city->removePlaceFromMatcher<PublicPlace>(obj);
				obj->setCity(NULL);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<PublicPlaceTableSync,PublicPlace>::Save(
			PublicPlace* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<PublicPlaceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}
	}

	namespace road
	{
		PublicPlaceTableSync::SearchResult PublicPlaceTableSync::Search(
			Env& env,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<PublicPlaceTableSync> query;
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
