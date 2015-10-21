////////////////////////////////////////////////////////////////////////////////
///	RoadPlace Table Synchronizer class implementation.
///	@file RoadPlaceTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "RoadPlaceTableSync.h"

#include "CityTableSync.h"
#include "Conversion.h"
#include "GeographyModule.h"
#include "DataSourceLinksField.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ImportableTableSync.hpp"
#include "ReplaceQuery.h"
#include "RoadModule.h"
#include "RoadPlace.h"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace geography;
	using namespace impex;


	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,RoadPlaceTableSync>::FACTORY_KEY("34.01 RoadPlace");
	}


	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadPlaceTableSync>::TABLE(
			"t060_road_places"
		);

		template<> const Field DBTableSyncTemplate<RoadPlaceTableSync>::_FIELDS[]=
		{
			Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<RoadPlaceTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ComplexObjectFieldDefinition<NamedPlaceField>::FIELDS[1].name.c_str(), ""));
			return DBTableSync::Indexes();
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
			LinkLevel linkLevel,
			boost::logic::tribool mainRoad,
			bool isDifferentFromExactName
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (exactName)
				query << " AND " << geography::NamedPlaceField::FIELDS[0].name << (isDifferentFromExactName ? "!=" : "=") << Conversion::ToDBString(*exactName);
			if (likeName)
				query << " AND " << geography::NamedPlaceField::FIELDS[0].name << " LIKE " << Conversion::ToDBString(*likeName);
			if (cityId)
				query << " AND " << geography::NamedPlaceField::FIELDS[1].name << "=" <<*cityId;
			if (!logic::indeterminate(mainRoad))
				query << " AND " << IsCityMainRoad::FIELD.name << "=" << (mainRoad ? "1" : "0");
			if (orderByName)
				query << " ORDER BY " << geography::NamedPlaceField::FIELDS[0].name << (raisingOrder ? " ASC" : " DESC");
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
				return boost::shared_ptr<RoadPlace>();
			}
			return roadPlaces.front();
		}


		bool RoadPlaceTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

}	}
