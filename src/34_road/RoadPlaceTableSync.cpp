////////////////////////////////////////////////////////////////////////////////
///	RoadPlace Table Synchronizer class implementation.
///	@file RoadPlaceTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "RoadPlaceTableSync.h"
#include "RoadPlace.h"
#include "CityTableSync.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"
#include "Conversion.h"

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
		template<> const string FactorableTemplate<DBTableSync,RoadPlaceTableSync>::FACTORY_KEY("34.01 RoadPlace");
	}

	namespace road
	{
		const string RoadPlaceTableSync::COL_NAME("name");
		const string RoadPlaceTableSync::COL_CITYID("city_id");
	}


	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadPlaceTableSync>::TABLE(
			"t060_road_places"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<RoadPlaceTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(RoadPlaceTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(RoadPlaceTableSync::COL_CITYID, SQL_INTEGER),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<RoadPlaceTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				RoadPlaceTableSync::COL_CITYID.c_str()
				, ""),
			DBTableSync::Index()
		};


		template<> void DBDirectTableSyncTemplate<RoadPlaceTableSync,RoadPlace>::Load(
			RoadPlace* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Properties
			object->setKey(rows->getLongLong(TABLE_COL_ID));
			object->setName(rows->getText(RoadPlaceTableSync::COL_NAME));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType cityId(rows->getLongLong(RoadPlaceTableSync::COL_CITYID));

				// City
				object->setCity(CityTableSync::Get(cityId, env, linkLevel).get());
				City* city(CityTableSync::GetEditable(cityId, env, linkLevel).get());
				city->addPlaceToMatcher<RoadPlace>(env.getEditableSPtr(object));
			}
		}



		template<> void DBDirectTableSyncTemplate<RoadPlaceTableSync,RoadPlace>::Save(
			RoadPlace* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<RoadPlaceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<RoadPlaceTableSync,RoadPlace>::Unlink(
			RoadPlace* obj
		){
			City* city = const_cast<City*>(obj->getCity ());
			if (city != NULL)
			{
//				city->removePlaceFromMatcher<RoadPlace>(obj);
				obj->setCity(NULL);
			}
		}



		template<> bool DBTableSyncTemplate<RoadPlaceTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<RoadPlaceTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadPlaceTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadPlaceTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace road
	{
		RoadPlaceTableSync::SearchResult RoadPlaceTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> cityId,
			boost::optional<std::string> exactName,
			boost::optional<std::string> likeName,
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (exactName)
			 	query << " AND " << COL_NAME << "=" << Conversion::ToDBString(*exactName);
			if (likeName)
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToDBString(*likeName);
			if (cityId)
				query << " AND " << COL_CITYID << "=" <<*cityId;
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}

		boost::shared_ptr<RoadPlace> RoadPlaceTableSync::GetEditableFromCityAndName(
			util::RegistryKeyType cityId,
			const std::string& name,
			util::Env& environment,
			util::LinkLevel linkLevel
		){
			SearchResult roadPlaces(
				Search(environment, cityId, optional<string>(), name, 0, 1, false, false)
			);
			if(roadPlaces.empty())
			{
				return shared_ptr<RoadPlace>();
			}
			return roadPlaces.front();
		}
	}
}


