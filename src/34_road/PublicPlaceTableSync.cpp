/** PublicPlaceTableSync class implementation.
	@file PublicPlaceTableSync.cpp

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

#include "PublicPlaceTableSync.h"

#include "CityTableSync.h"
#include "GeographyModule.h"
#include "ImportableTableSync.hpp"
#include "PublicPlace.h"
#include "RoadModule.h"
#include "SelectQuery.hpp"

#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace geography;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, PublicPlaceTableSync>::FACTORY_KEY("34.40.03 Public places");
		template<> const string FactorableTemplate<Fetcher<NamedPlace>, PublicPlaceTableSync>::FACTORY_KEY("13");
	}

	namespace road
	{
		const string PublicPlaceTableSync::COL_NAME ("name");
		const string PublicPlaceTableSync::COL_CITYID ("city_id");
		const string PublicPlaceTableSync::COL_DATASOURCE_LINKS = "datasource_links";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PublicPlaceTableSync>::TABLE(
			"t013_public_places"
		);

		template<> const Field DBTableSyncTemplate<PublicPlaceTableSync>::_FIELDS[]=
		{	Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PublicPlaceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ComplexObjectFieldDefinition<NamedPlaceField>::FIELDS[1].name.c_str(), ""));
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<PublicPlaceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<PublicPlaceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PublicPlaceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PublicPlaceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace road
	{
		PublicPlaceTableSync::SearchResult PublicPlaceTableSync::Search(
			Env& env,
			optional<RegistryKeyType> cityId,
			optional<string> name,
			int first /*= 0*/,
			optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<PublicPlaceTableSync> query;
			if(cityId)
			{
				query.addWhereField(COL_CITYID, *cityId);
			}
			if(name)
			{
				query.addWhereField(COL_NAME, *name, ComposedExpression::OP_LIKE);
			}

			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
