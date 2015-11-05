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

#include "ArrivalDepartureTableLog.h"
#include "ArrivalDepartureTableRight.h"
#include "Conversion.h"
#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DisplayMaintenanceRight.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenTableSync.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "StopArea.hpp"
#include "User.h"

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

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DisplayScreenCPUTableSync>::TABLE(
			"t058_display_screen_cpu"
		);

		template<> const Field DBTableSyncTemplate<DisplayScreenCPUTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DisplayScreenCPUTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(PlaceId::FIELD.name.c_str(), "")
			);
			r.push_back(
				DBTableSync::Index(MacAddress::FIELD.name.c_str(), "")
			);
			return r;
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
				query << " AND " << PlaceId::FIELD.name << "=" << *placeId;
			}
			if(macAddress)
			{
				query << " AND " << MacAddress::FIELD.name << "=" << Conversion::ToDBString(*macAddress);
			}
			if (orderByName)
			{
				query << " ORDER BY " << SimpleObjectFieldDefinition<Name>::FIELD.name << (raisingOrder ? " ASC" : " DESC");
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

		bool DisplayScreenCPUTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::READ);
		}
	}
}
