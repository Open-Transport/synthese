
/** PlaceAliasTableSync class implementation.
	@file PlaceAliasTableSync.cpp

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

#include <sstream>

#include "PlaceAliasTableSync.h"

#include "CityTableSync.h"
#include "Fetcher.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "GeographyModule.h"
#include "PTModule.h"
#include "ReplaceQuery.h"
#include "StopArea.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, PlaceAliasTableSync>::FACTORY_KEY("32.10.01 Places Alias");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PlaceAliasTableSync>::TABLE(
			"t011_place_aliases"
			);

		template<> const Field DBTableSyncTemplate<PlaceAliasTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PlaceAliasTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<PlaceAliasTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<PlaceAliasTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PlaceAliasTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PlaceAliasTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace geography
	{
		PlaceAliasTableSync::SearchResult PlaceAliasTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> aliasedPlaceId,
			int first /*= 0*/,
			optional<size_t> number,
			LinkLevel linkLevel
		){
			SelectQuery<PlaceAliasTableSync> query;
			if(aliasedPlaceId)
			{
				query.addWhereField(AliasedPlaceId::FIELD.name, *aliasedPlaceId);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}
			return LoadFromQuery(query.toString(), env, linkLevel);
		}
	}
}
