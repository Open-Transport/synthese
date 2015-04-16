////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPUTableSync class implementation.
///	@file DisplayScreenCPUTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-26 14:04
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayScreenCPUTableSync.h"

#include "DisplayScreenCPU.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"
#include "Conversion.h"
#include "StopArea.hpp"
#include "Fetcher.h"
#include "ArrivalDepartureTableRight.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departure_boards;
	using namespace pt;
	using namespace geography;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DisplayScreenCPUTableSync>::FACTORY_KEY("54.49 Display screen CPU");
	}

	namespace departure_boards
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
		template<> const DBTableSync::Format DBTableSyncTemplate<DisplayScreenCPUTableSync>::TABLE(
			"t058_display_screen_cpu"
		);

		template<> const Field DBTableSyncTemplate<DisplayScreenCPUTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DisplayScreenCPUTableSync::COL_NAME, SQL_TEXT),
			Field(DisplayScreenCPUTableSync::COL_PLACE_ID, SQL_INTEGER),
			Field(DisplayScreenCPUTableSync::COL_MAC_ADDRESS, SQL_TEXT),
			Field(DisplayScreenCPUTableSync::COL_MONITORING_DELAY, SQL_INTEGER),
			Field(DisplayScreenCPUTableSync::COL_IS_ONLINE, SQL_INTEGER),
			Field(DisplayScreenCPUTableSync::COL_MAINTENANCE_MESSAGE, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DisplayScreenCPUTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(DisplayScreenCPUTableSync::COL_PLACE_ID.c_str(), "")
			);
			r.push_back(
				DBTableSync::Index(DisplayScreenCPUTableSync::COL_MAC_ADDRESS.c_str(), "")
			);
			return r;
		}


		template<> void OldLoadSavePolicy<DisplayScreenCPUTableSync,DisplayScreenCPU>::Load(
			DisplayScreenCPU* object,
			const db::DBResultSPtr& rows,
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
				object->setPlace(NULL);
				RegistryKeyType placeId(rows->getLongLong(DisplayScreenCPUTableSync::COL_PLACE_ID));
				if(placeId != 0) try
				{
					object->setPlace(Fetcher<NamedPlace>::Fetch(placeId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<DisplayScreenCPU>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayScreenCPUTableSync::TABLE.NAME + "/" + DisplayScreenCPUTableSync::COL_PLACE_ID + e.getMessage());
				}
			}
		}



		template<> void OldLoadSavePolicy<DisplayScreenCPUTableSync,DisplayScreenCPU>::Save(
			DisplayScreenCPU* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DisplayScreenCPUTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getPlace() ? object->getPlace()->getKey() : RegistryKeyType(0));
			query.addField(object->getMacAddress());
			query.addField(object->getMonitoringDelay().minutes());
			query.addField(object->getIsOnline());
			query.addField(object->getMaintenanceMessage());
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<DisplayScreenCPUTableSync,DisplayScreenCPU>::Unlink(
			DisplayScreenCPU* object
		){
			object->setPlace(NULL);
		}



		template<> bool DBTableSyncTemplate<DisplayScreenCPUTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				boost::shared_ptr<const DisplayScreenCPU> cpu(DisplayScreenCPUTableSync::Get(object_id, env));
				if (cpu->getPlace())
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(cpu->getPlace()->getKey()));
				}
				else
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
				}
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>&)
			{
				return false;
			}
		}



		template<> void DBTableSyncTemplate<DisplayScreenCPUTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			try
			{
				Env env;
				DisplayScreenTableSync::SearchResult screens(DisplayScreenTableSync::SearchFromCPU(env, id));
				BOOST_FOREACH(const DisplayScreenTableSync::SearchResult::value_type& screen, screens)
				{
					DisplayScreenTableSync::Remove(NULL, screen->getKey(), transaction, false);
				}
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>&)
			{
			}
		}



		template<> void DBTableSyncTemplate<DisplayScreenCPUTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayScreenCPUTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			// Log
			Env env;
			boost::shared_ptr<const DisplayScreenCPU> cpu(DisplayScreenCPUTableSync::Get(id, env));
			ArrivalDepartureTableLog::addRemoveEntry(*cpu, *session->getUser());
		}
	}



	namespace departure_boards
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
				query << " AND " << COL_MAC_ADDRESS << "=" << Conversion::ToDBString(*macAddress);
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
