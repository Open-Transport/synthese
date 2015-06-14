////////////////////////////////////////////////////////////////////////////////
/// DisplayTypeTableSync class implementation.
///	@file DisplayTypeTableSync.cpp
///	@author Hugues Romain
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

#include "DisplayTypeTableSync.h"

#include "ArrivalDepartureTableRight.h"
#include "ArrivalDepartureTableLog.h"
#include "Conversion.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "DisplayScreenTableSync.h"
#include "MessageTypeTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"
#include "WebPageTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace departure_boards;
	using namespace messages;
	using namespace util;
	using namespace cms;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DisplayTypeTableSync>::FACTORY_KEY("54.00 Display Types");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DisplayTypeTableSync>::TABLE(
			"t036_display_types"
		);

		template<> const Field DBTableSyncTemplate<DisplayTypeTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<> DBTableSync::Indexes DBTableSyncTemplate<DisplayTypeTableSync>::GetIndexes()
		{
			return DBTableSyncTemplate::Indexes();
		}




		template<> bool DBTableSyncTemplate<DisplayTypeTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			Env env;
			RightsOfSameClassMap rights;
			DisplayScreenTableSync::Search(
				env,
				rights,
				true,
				UNKNOWN_RIGHT_LEVEL,
				optional<RegistryKeyType>(),
				optional<RegistryKeyType>(),
				optional<RegistryKeyType>(),
				object_id,
				std::string(),
				std::string(),
				std::string(),
				optional<int>(),
				optional<int>(),
				0,
				1
			);
			if (!env.getRegistry<DisplayScreen>().empty())
			{
				return false;
			}

			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<DisplayTypeTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayTypeTableSync>::AfterDelete(
			util::RegistryKeyType id,
			DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayTypeTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const DisplayType> typ(DisplayTypeTableSync::Get(id, env));
			ArrivalDepartureTableLog::addDeleteTypeEntry(typ.get(), session->getUser().get());
		}
	}

	namespace departure_boards
	{
		DisplayTypeTableSync::SearchResult DisplayTypeTableSync::Search(
			Env& env,
			optional<string> likeName,
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
			bool orderByName,
			bool orderByRows,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT t.*"
				<< " FROM " << TABLE.NAME << " AS t";

			query << " WHERE 1";
			if (likeName)
			{
				query << " AND t." << Name::FIELD.name << " LIKE " << Conversion::ToDBString(*likeName);
			}

			if (orderByName)
			{
				query << " ORDER BY t." << Name::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			}
			else if(orderByRows)
			{
				query << " ORDER BY t." << RowsNumber::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			}

			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
