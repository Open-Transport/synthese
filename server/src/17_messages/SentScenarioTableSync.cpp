
/** ScenarioTableSync class implementation.
	@file ScenarioTableSync.cpp

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

#include "SentScenarioTableSync.h"
#include "ScenarioTableSync.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "DataSourceLinksField.hpp"
#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "ImportableTableSync.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessagesLibraryRight.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "MessagesRight.h"
#include "MessagesSectionTableSync.hpp"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SentScenario.h"
#include "ScenarioCalendarTableSync.hpp"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTemplate.h"
#include "Session.h"
#include "User.h"

#include <sstream>
#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;
	using namespace messages;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,SentScenarioTableSync>::FACTORY_KEY("17.00.01 Alarm scenarii");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<SentScenarioTableSync>::TABLE(
			"t039_scenarios"
		);

		template<> const Field DBTableSyncTemplate<SentScenarioTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<SentScenarioTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					PeriodStart::FIELD.name.c_str(),
					"")	);
			return r;
		}


		template<> bool DBTableSyncTemplate<SentScenarioTableSync>::CanDelete(
			const server::Session* session,
			RegistryKeyType object_id
		){
			Env env;
			boost::shared_ptr<const Scenario> scenario(SentScenarioTableSync::Get(object_id, env));
			if(dynamic_cast<const ScenarioTemplate*>(scenario.get()))
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(DELETE_RIGHT);
			}
		}



		template<> void DBTableSyncTemplate<SentScenarioTableSync>::BeforeDelete(
			RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;

			// Messages
			Alarms alarms;
			AlarmTableSync::Search(env, std::back_inserter(alarms), id);
			BOOST_FOREACH(const boost::shared_ptr<Alarm>& alarm, alarms)
			{
				AlarmTableSync::Remove(NULL, alarm->getKey(), transaction, false);
			}

			// Calendars
			ScenarioCalendarTableSync::SearchResult calendars(
				ScenarioCalendarTableSync::Search(env, id)
			);
			BOOST_FOREACH(const boost::shared_ptr<ScenarioCalendar>& calendar, calendars)
			{
				ScenarioCalendarTableSync::Remove(NULL, calendar->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<SentScenarioTableSync>::AfterDelete(
			RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<SentScenarioTableSync>::LogRemoval(
			const server::Session* session,
			RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const Scenario> scenario(SentScenarioTableSync::Get(id, env));
			if(dynamic_cast<const ScenarioTemplate*>(scenario.get()))
			{
				MessagesLibraryLog::addDeleteEntry(dynamic_cast<const ScenarioTemplate*>(scenario.get()), session->getUser().get());
			}
			else
			{
				MessagesLog::AddDeleteEntry(*dynamic_cast<const SentScenario*>(scenario.get()), *session->getUser());
			}
		}
	}
	
	namespace messages
	{

		//////////////////////////////////////////////////////////////////////////
		/// Sent scenario search.
		/// @param env the environment to populate
		///	@param name Name of the scenario
		/// @param inArchive filters the scenario with end date in the past
		/// @param isActive filters the scenario with the activation status set to on
		/// @param scenarioId filters the scenarion on its template
		///	@param first First Scenario object to answer
		///	@param number Number of Scenario objects to answer (0 = all) The size of the vector is less or equal to number, 
		/// then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then 
		/// there is others accounts to show. Test it to know if the situation needs a "click for more" button.
		///	@author Hugues Romain
		///	@date 2006-2013
		SentScenarioTableSync::SearchResult SentScenarioTableSync::Search(
			Env& env,
			optional<string> name /*= optional<string>()*/,
			boost::optional<bool> inArchive,
			boost::optional<bool> isActive,
			optional<RegistryKeyType> scenarioId /*= optional<RegistryKeyType>()*/,
			optional<int> first /*= optional<int>()*/,
			optional<size_t> number /*= optional<size_t>()*/,
			bool orderByDate /*= true*/,
			bool orderByName /*= false*/,
			bool raisingOrder /*= false*/,
			LinkLevel linkLevel /*= UP_LINKS_LOAD_LEVEL */
		){
			SentScenarioTableSync::SearchResult result;
			SentScenarioTableSync::Search(env, std::back_inserter(result),
										  name, inArchive, isActive, scenarioId, first,
										  number, orderByDate, orderByName, raisingOrder, linkLevel);
			return result;
		}


	}
}
