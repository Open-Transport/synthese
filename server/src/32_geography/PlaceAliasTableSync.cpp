
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

	namespace geography
	{
		const string PlaceAliasTableSync::COL_NAME ("name");
		const string PlaceAliasTableSync::COL_ALIASEDPLACEID ("aliased_place_id");
		const string PlaceAliasTableSync::COL_CITYID ("city_id");
		const string PlaceAliasTableSync::COL_ISCITYMAINCONNECTION ("is_city_main_connection");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PlaceAliasTableSync>::TABLE(
			"t011_place_aliases"
			);

		template<> const Field DBTableSyncTemplate<PlaceAliasTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(PlaceAliasTableSync::COL_NAME, SQL_TEXT),
			Field(PlaceAliasTableSync::COL_ALIASEDPLACEID, SQL_INTEGER),
			Field(PlaceAliasTableSync::COL_CITYID, SQL_INTEGER),
			Field(PlaceAliasTableSync::COL_ISCITYMAINCONNECTION, SQL_BOOLEAN),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PlaceAliasTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<>
		void OldLoadSavePolicy<PlaceAliasTableSync,PlaceAlias>::Load(
			PlaceAlias* obj,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			obj->setName (rows->getText (PlaceAliasTableSync::COL_NAME));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Because the fetch place has no equivalent in database read mode
//				assert(temporary == GET_REGISTRY);

				RegistryKeyType aliasedPlaceId (
					rows->getLongLong (PlaceAliasTableSync::COL_ALIASEDPLACEID));
				RegistryKeyType cityId (
					rows->getLongLong (PlaceAliasTableSync::COL_CITYID));
				City* city(CityTableSync::GetEditable(cityId, env, linkLevel).get());

				obj->setCity(city);
				obj->setAliasedPlace(Fetcher<NamedPlace>::Fetch(aliasedPlaceId, env, linkLevel).get());


				bool isCityMainConnection (rows->getBool ( PlaceAliasTableSync::COL_ISCITYMAINCONNECTION));
				if (isCityMainConnection)
				{
					city->addIncludedPlace(*obj);
				}
				city->addPlaceToMatcher(env.getEditableSPtr(obj));

				// Registration to all places matcher
				if(	&env == &Env::GetOfficialEnv() &&
					obj->getCity() &&
					linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL
				){
					GeographyModule::GetGeneralAllPlacesMatcher().add(
						obj->getFullName(),
						env.getEditableSPtr(obj)
					);
				}

				if(	&env == &Env::GetOfficialEnv() &&
					obj->getCity() &&
					linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL &&
					dynamic_cast<const pt::StopArea*>(obj->getAliasedPlace())
				){
					const pt::StopArea* stopArea = static_cast<const pt::StopArea*>(obj->getAliasedPlace());
					pt::PTModule::GetGeneralStopsMatcher().add(
						obj->getFullName(),
						env.getEditableSPtr(const_cast<pt::StopArea*>(stopArea))
					);
				}
			}
		}

		template<>
		void OldLoadSavePolicy<PlaceAliasTableSync,PlaceAlias>::Unlink(
			PlaceAlias* obj
		){
			City* city(const_cast<City*>(obj->getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher(*obj);
				obj->setCity(NULL);
				city->removeIncludedPlace(*obj);
			}
		}

		template<>
		void OldLoadSavePolicy<PlaceAliasTableSync,PlaceAlias>::Save(
			PlaceAlias* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<PlaceAliasTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getAliasedPlace() ? object->getAliasedPlace()->getKey() : RegistryKeyType(0));
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.addField(object->getCity() ? object->getCity()->includes(*object) : false);
			query.execute(transaction);
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
				query.addWhereField(COL_ALIASEDPLACEID, *aliasedPlaceId);
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
