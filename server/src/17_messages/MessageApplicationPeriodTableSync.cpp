
/** MessageApplicationPeriodTableSync class implementation.
	@file MessageApplicationPeriodTableSync.cpp

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

#include "MessageApplicationPeriodTableSync.hpp"

#include "Alarm.h"
#include "SentScenario.h"
#include "DBResult.hpp"
#include "SelectQuery.hpp"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,MessageApplicationPeriodTableSync>::FACTORY_KEY("17.40 Message application period");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<MessageApplicationPeriodTableSync>::TABLE(
			"t104_message_application_periods"
		);

		template<> const Field DBTableSyncTemplate<MessageApplicationPeriodTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<MessageApplicationPeriodTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<MessageApplicationPeriodTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<MessageApplicationPeriodTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<MessageApplicationPeriodTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<MessageApplicationPeriodTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace messages
	{
		MessageApplicationPeriodTableSync::SearchResult MessageApplicationPeriodTableSync::Search(
			Env& env,
			optional<RegistryKeyType> calendarId,
			int first /*= 0*/,
			boost::optional<std::size_t> number,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<MessageApplicationPeriodTableSync> query;
			if (calendarId)
			{
				query.addWhereField(ScenarioCalendar::FIELD.name, *calendarId, ComposedExpression::OP_EQ);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		void MessageApplicationPeriodTableSync::CopyPeriods(
			util::RegistryKeyType sourceId,
			ScenarioCalendar& calendar,
			boost::optional<db::DBTransaction&> transaction
		){
			Env env;
			SearchResult periods(
				Search(env, sourceId)
			);
			BOOST_FOREACH(const boost::shared_ptr<MessageApplicationPeriod>& period, periods)
			{
				// Raw copy
				boost::shared_ptr<MessageApplicationPeriod> newPeriod(
					boost::dynamic_pointer_cast<MessageApplicationPeriod, ObjectBase>(
						period->copy()
				)	);
				newPeriod->setKey(0);

				// Link to the new calendar
				period->set<ScenarioCalendar>(calendar);

				// Save
				Save(period.get(), transaction);
			}
		}
}	}
