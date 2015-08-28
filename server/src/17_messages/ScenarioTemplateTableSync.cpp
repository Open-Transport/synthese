
/** ScenarioTemplateTableSync class implementation.
	@file ScenarioTemplateTableSync.cpp

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

#include "ScenarioTemplateTableSync.h"
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
		template<> const string FactorableTemplate<DBTableSync,ScenarioTemplateTableSync>::FACTORY_KEY("17.00.01 Alarm scenarii templates");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScenarioTemplateTableSync>::TABLE(
			"t028_scenario_templates"
		);

		template<> const Field DBTableSyncTemplate<ScenarioTemplateTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ScenarioTemplateTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(Folder::FIELD.name.c_str(), ""));
			return r;
		}

		template<> bool DBTableSyncTemplate<ScenarioTemplateTableSync>::CanDelete(
			const server::Session* session,
			RegistryKeyType object_id
		){
			Env env;
			boost::shared_ptr<const ScenarioTemplate> scenario(ScenarioTemplateTableSync::Get(object_id, env));
			if(dynamic_cast<const ScenarioTemplate*>(scenario.get()))
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(DELETE_RIGHT);
			}
		}



		template<> void DBTableSyncTemplate<ScenarioTemplateTableSync>::BeforeDelete(
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



		template<> void DBTableSyncTemplate<ScenarioTemplateTableSync>::AfterDelete(
			RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScenarioTemplateTableSync>::LogRemoval(
			const server::Session* session,
			RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const ScenarioTemplate> scenario(ScenarioTemplateTableSync::Get(id, env));
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
		/// Performs a copy of each message from a scenario to a new one.
		/// @param sourceId the if of the scenario to copy
		/// @param dest the new scenario
		/// @param transaction the transaction
		void ScenarioTemplateTableSync::CopyMessages(
			RegistryKeyType sourceId,
			SentScenario& dest,
			optional<DBTransaction&> transaction
		){
			Env env;

			// Calendars
			typedef map<RegistryKeyType, boost::shared_ptr<ScenarioCalendar> > CalendarsMap;
			CalendarsMap calendarsMap;
			ScenarioCalendarTableSync::SearchResult calendars(ScenarioCalendarTableSync::Search(env, sourceId));
			BOOST_FOREACH(const boost::shared_ptr<ScenarioCalendar>& calendar, calendars)
			{
				// Calendar creation
				boost::shared_ptr<ScenarioCalendar> newCalendar(
					boost::dynamic_pointer_cast<ScenarioCalendar, ObjectBase>(
						calendar->copy()
				)	);
				newCalendar->setKey(0);

				// Link with the new scenario
				newCalendar->set<ScenarioPointer>(dest);

				// Save
				ScenarioCalendarTableSync::Save(newCalendar.get(), transaction);

				// Store the relation between the source calendar and the new one
				calendarsMap.insert(make_pair(calendar->getKey(), newCalendar));

				// Copy the periods
				MessageApplicationPeriodTableSync::CopyPeriods(
					calendar->getKey(),
					*newCalendar,
					transaction
				);

			}

			// The existing messages
			Alarms alarms;
			AlarmTableSync::Search(env, std::back_inserter(alarms), sourceId);

			// Copy of each message
			BOOST_FOREACH(const boost::shared_ptr<Alarm>& templateAlarm, alarms)
			{
				// Calendar
				boost::shared_ptr<ScenarioCalendar> calendar;
				if(templateAlarm->getCalendar())
				{
					calendar = calendarsMap[templateAlarm->getCalendar()->getKey()];
				}

				// Message creation
				boost::shared_ptr<Alarm> alarm(new Alarm(*templateAlarm));
				alarm->setScenario(static_cast<const SentScenario*>(&dest));
				alarm->setCalendar(calendar.get());
				
				AlarmTableSync::Save(alarm.get(), transaction);
				// Copy of the recipients of the message
				AlarmObjectLinkTableSync::CopyRecipients(
					templateAlarm->getKey(),
					*alarm,
					transaction
					);
				
				// Copy of the message alternatives
				MessageAlternativeTableSync::CopyAlternatives(
					templateAlarm->getKey(),
					*alarm,
					transaction
					);

			}
		}


		ScenarioTemplateTableSync::SearchResult ScenarioTemplateTableSync::Search(
			Env& env,
			optional<RegistryKeyType> folderId,
			const string name /*= string()*/,
			const ScenarioTemplate* scenarioToBeDifferentWith /*= NULL*/,
			int first /*= 0*/,
			optional<size_t> number /*= optional<size_t>()*/,
			bool orderByName /*= true*/,
			bool raisingOrder /*= false*/,
			LinkLevel linkLevel /*= UP_LINKS_LOAD_LEVEL */
		){
			ScenarioTemplateTableSync::SearchResult result;
			ScenarioTemplateTableSync::Search(env, std::back_inserter(result),
											  folderId, name, scenarioToBeDifferentWith, first,
											  number, orderByName, raisingOrder, linkLevel);
			return result;
		}



	}
}
