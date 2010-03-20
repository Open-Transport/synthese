
//////////////////////////////////////////////////////////////////////////
///	HikingTrailTableSync class implementation.
///	@file HikingTrailTableSync.cpp
///	@author Hugues
///	@date 2010
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

#include <sstream>

#include "HikingTrailTableSync.h"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "ConnectionPlaceTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace hiking;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,HikingTrailTableSync>::FACTORY_KEY("58 Hiking trails");
	}

	namespace hiking
	{
		const string HikingTrailTableSync::COL_NAME ("name");
		const string HikingTrailTableSync::COL_DURATION ("duration");
		const string HikingTrailTableSync::COL_MAP ("map");
		const string HikingTrailTableSync::COL_PROFILE ("profile");
		const string HikingTrailTableSync::COL_STOPS ("stops");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<HikingTrailTableSync>::TABLE(
			"t064_hiking_trails"
		);



		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<HikingTrailTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(HikingTrailTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(HikingTrailTableSync::COL_MAP, SQL_TEXT),
			SQLiteTableSync::Field(HikingTrailTableSync::COL_PROFILE, SQL_TEXT),
			SQLiteTableSync::Field(HikingTrailTableSync::COL_STOPS, SQL_TEXT),
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<HikingTrailTableSync>::_INDEXES[]=
		{
			// SQLiteTableSync::Index(
			//	HikingTrailTableSync::COL_NAME.c_str(),
			// ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<HikingTrailTableSync,HikingTrail>::Load(
			HikingTrail* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(HikingTrailTableSync::COL_NAME));
			object->setDuration(rows->getText(HikingTrailTableSync::COL_DURATION));
			object->setMap(rows->getText(HikingTrailTableSync::COL_MAP));
			object->setProfile(rows->getText(HikingTrailTableSync::COL_PROFILE));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				vector<string> stopids = Conversion::ToStringVector(rows->getText (HikingTrailTableSync::COL_STOPS));
				object->clearStops();
				size_t rank(0);
				BOOST_FOREACH(const string& stopid, stopids)
				{
					try
					{
						object->addStop(
							ConnectionPlaceTableSync::GetEditable(lexical_cast<RegistryKeyType>(stopid), env, linkLevel).get(),
							rank
						);
					}
					catch(ObjectNotFoundException<PublicTransportStopZoneConnectionPlace>& e)
					{
						Log::GetInstance().warn("No such stop "+ stopid +" in HikingTrail "+ lexical_cast<string>(object->getKey()));
					}
					catch(bad_lexical_cast)
					{
						Log::GetInstance().warn("No such stop "+ stopid +" in HikingTrail "+ lexical_cast<string>(object->getKey()));
					}
					++rank;
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<HikingTrailTableSync,HikingTrail>::Save(
			HikingTrail* object,
			optional<SQLiteTransaction&> transaction
		){
			stringstream stops;
			{
				bool first(true);
				BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace* stop, object->getStops())
				{
					if(!first) stops << ",";
					stops << stop->getKey();
					first = false;
				}
			}

			ReplaceQuery<HikingTrailTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getDuration());
			query.addField(object->getMap());
			query.addField(object->getProfile());
			query.addField(stops.str());
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<HikingTrailTableSync,HikingTrail>::Unlink(
			HikingTrail* obj
		){
		}
	}
	
	
	
	namespace hiking
	{
		HikingTrailTableSync::SearchResult HikingTrailTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
				;
			// if(parameterId)
			// {
			// 	query << " AND " << COL_PARENT_ID << "=" << *parentFolderId;
			// }
			// if(orderByName)
			// {
			// 	query << " ORDER BY " << COL_NAME << " " << (raisingOrder ? "ASC" : "DESC");
			// }
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
				{
					query << " OFFSET " << first;
			}	}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
