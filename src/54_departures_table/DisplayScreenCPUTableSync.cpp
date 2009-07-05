////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPUTableSync class implementation.
///	@file DisplayScreenCPUTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-26 14:04
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenCPU.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "Conversion.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departurestable;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,DisplayScreenCPUTableSync>::FACTORY_KEY("34.49 Display screen CPU");
	}

	namespace departurestable
	{
		const std::string DisplayScreenCPUTableSync::COL_NAME("name");
		const std::string DisplayScreenCPUTableSync::COL_PLACE_ID("place_id");
		const std::string DisplayScreenCPUTableSync::COL_MAC_ADDRESS("mac_address");
		const std::string DisplayScreenCPUTableSync::COL_MONITORING_DELAY("monitoring_delay");
		const std::string DisplayScreenCPUTableSync::COL_IS_ONLINE("is_online");
		const std::string DisplayScreenCPUTableSync::COL_MAINTENANCE_MESSAGE("maintenance_message");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<DisplayScreenCPUTableSync>::TABLE(
			"t058_display_screen_cpu"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<DisplayScreenCPUTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_TEXT, false),
			SQLiteTableSync::Field(DisplayScreenCPUTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenCPUTableSync::COL_PLACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenCPUTableSync::COL_MAC_ADDRESS, SQL_TEXT),
			SQLiteTableSync::Field(DisplayScreenCPUTableSync::COL_MONITORING_DELAY, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenCPUTableSync::COL_IS_ONLINE, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayScreenCPUTableSync::COL_MAINTENANCE_MESSAGE, SQL_TEXT),
			SQLiteTableSync::Field()
		};
		
		template<>  const SQLiteTableSync::Index SQLiteTableSyncTemplate<DisplayScreenCPUTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(DisplayScreenCPUTableSync::COL_PLACE_ID.c_str(), ""),
			SQLiteTableSync::Index(DisplayScreenCPUTableSync::COL_MAC_ADDRESS.c_str(), ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenCPUTableSync,DisplayScreenCPU>::Load(
			DisplayScreenCPU* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(DisplayScreenCPUTableSync::COL_NAME));
			object->setMacAddress(rows->getText(DisplayScreenCPUTableSync::COL_MAC_ADDRESS));
			object->setMaintenanceMessage(rows->getText(DisplayScreenCPUTableSync::COL_MAINTENANCE_MESSAGE));
			object->setMonitoringDelay(minutes(rows->getInt(DisplayScreenCPUTableSync::COL_MONITORING_DELAY)));
			object->setIsOnline(rows->getBool(DisplayScreenCPUTableSync::COL_IS_ONLINE));
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Links
				try
				{
					object->setPlace(ConnectionPlaceTableSync::Get(rows->getLongLong(DisplayScreenCPUTableSync::COL_PLACE_ID), env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayScreenCPU>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayScreenCPUTableSync::COL_PLACE_ID + e.getMessage());
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenCPUTableSync,DisplayScreenCPU>::Save(
			DisplayScreenCPU* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< object->getKey() << ","
				<< Conversion::ToSQLiteString(object->getName()) << ","
				<< ((object->getPlace() != NULL) ? object->getPlace()->getKey() : 0) << ","
				<< Conversion::ToSQLiteString(object->getMacAddress()) << ","
				<< object->getMonitoringDelay().minutes() << ","
				<< object->getIsOnline() << ","
				<< Conversion::ToSQLiteString(object->getMaintenanceMessage())
				<< ")";
			sqlite->execUpdate(query.str());
		}


		
		template<> void SQLiteDirectTableSyncTemplate<DisplayScreenCPUTableSync,DisplayScreenCPU>::Unlink(
			DisplayScreenCPU* object
		){
			object->setPlace(NULL);
		}
	}
	
	
	
	namespace departurestable
	{
		DisplayScreenCPUTableSync::SearchResult DisplayScreenCPUTableSync::Search(
			Env& env,
			boost::optional<RegistryKeyType> placeId,
			boost::optional<std::string> macAddress,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (placeId)
			{
				query << " AND " << COL_PLACE_ID << "=" << *placeId;
			}
			if(macAddress)
			{
				query << " AND " << COL_MAC_ADDRESS << "=" << Conversion::ToSQLiteString(*macAddress);
			}
			if (orderByName)
			{
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
			}
			if (first > 0)
			{
				query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
