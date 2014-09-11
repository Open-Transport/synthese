
/** StopPointAccessibilityCalendarTableSync class implementation.
	@file StopPointAccessibilityCalendarTableSync.cpp

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

#include "StopPointInaccessibilityCalendarTableSync.hpp"

#include "SelectQuery.hpp"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string util::FactorableTemplate<DBTableSync, StopPointInaccessibilityCalendarTableSync>::FACTORY_KEY(
			"35.56.01 StopPointInaccessibilityCalendar"
		);
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::TABLE(
			"t501_stop_point_inaccessibility_calendars"
		);

		template<> const Field DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::_FIELDS[] = { Field() }; // Defined by the record



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopPointInaccessibilityCalendarTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace pt
	{
		StopPointInaccessibilityCalendarTableSync::SearchResult StopPointInaccessibilityCalendarTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> stopPointId,
			boost::optional<util::RegistryKeyType> calendarId,
			graph::UserClassCode userClassCode,
			int first,
			boost::optional<std::size_t> number,
			util::LinkLevel linkLevel
		){
			SelectQuery<StopPointInaccessibilityCalendarTableSync> query;

			if(number)
			{
				query.setNumber(*number + 1);
			}
			if(first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
