/** AlarmTableSync class implementation.
	@file AlarmTableSync.cpp

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

#include "AlarmTableSync.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTemplate.h"
#include "DataSource.h"
#include "DataSourceLinksField.hpp"
#include "DBResult.hpp"
#include "ImportableTableSync.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessagesLibraryLog.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesSectionTableSync.hpp"
#include "MessagesLog.h"
#include "MessagesTypes.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "ScenarioCalendarTableSync.hpp"
#include "ScenarioTemplate.h"
#include "ScenarioTableSync.h"
#include "SentAlarm.h"
#include "SentScenario.h"
#include "Session.h"
#include "User.h"

#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace impex;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,AlarmTableSync>::FACTORY_KEY("17.10.01 Alarms");
	}

	namespace messages
	{
		const string AlarmTableSync::_COL_CONFLICT_LEVEL = "conflict_level";
		const string AlarmTableSync::_COL_RECIPIENTS_NUMBER = "recipients";

		const string AlarmTableSync::COL_IS_TEMPLATE = "is_template";
		const string AlarmTableSync::COL_LEVEL = "level";
		const string AlarmTableSync::COL_SHORT_MESSAGE = "short_message";
		const string AlarmTableSync::COL_LONG_MESSAGE = "long_message";
		const string AlarmTableSync::COL_SCENARIO_ID = "scenario_id";
		const string AlarmTableSync::COL_TEMPLATE_ID("template_id");
		const string AlarmTableSync::COL_RAW_EDITOR = "raw_editor";
		const string AlarmTableSync::COL_DONE = "done";
		const string AlarmTableSync::COL_MESSAGES_SECTION_ID = "messages_section_id";
		const string AlarmTableSync::COL_CALENDAR_ID = "calendar_id";
		const string AlarmTableSync::COL_DATASOURCE_LINKS = "datasource_links";
		const string AlarmTableSync::COL_DISPLAY_DURATION = "display_duration";
		const string AlarmTableSync::COL_DIGITIZED_VERSION = "digitized_version";
		const string AlarmTableSync::COL_TAGS = "tags";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AlarmTableSync>::TABLE(
			"t003_alarms"
		);

		template<> const Field DBTableSyncTemplate<AlarmTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(AlarmTableSync::COL_IS_TEMPLATE, SQL_INTEGER),
			Field(AlarmTableSync::COL_LEVEL, SQL_INTEGER),
			Field(AlarmTableSync::COL_SHORT_MESSAGE, SQL_TEXT),
			Field(AlarmTableSync::COL_LONG_MESSAGE, SQL_TEXT),
			Field(AlarmTableSync::COL_SCENARIO_ID, SQL_INTEGER),
			Field(AlarmTableSync::COL_TEMPLATE_ID, SQL_INTEGER),
			Field(AlarmTableSync::COL_RAW_EDITOR, SQL_BOOLEAN),
			Field(AlarmTableSync::COL_DONE, SQL_BOOLEAN),
			Field(AlarmTableSync::COL_MESSAGES_SECTION_ID, SQL_INTEGER),
			Field(AlarmTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			Field(AlarmTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field(AlarmTableSync::COL_DISPLAY_DURATION, SQL_INTEGER),
			Field(AlarmTableSync::COL_DIGITIZED_VERSION, SQL_TEXT),
			Field(AlarmTableSync::COL_TAGS, SQL_TEXT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<AlarmTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(AlarmTableSync::COL_SCENARIO_ID.c_str(), ""));
			return r;
		};

		template<>
		boost::shared_ptr<Alarm> InheritanceLoadSavePolicy<AlarmTableSync, Alarm>::GetNewObject(
			const Record& row
		){
			if(row.getDefault<bool>(AlarmTableSync::COL_IS_TEMPLATE, false))
			{
				return boost::shared_ptr<Alarm>(new AlarmTemplate(row.getDefault<RegistryKeyType>(TABLE_COL_ID)));
			}
			else
			{
				return boost::shared_ptr<Alarm>(new SentAlarm(row.getDefault<RegistryKeyType>(TABLE_COL_ID)));
			}
		}



		template<>
		void InheritanceLoadSavePolicy<AlarmTableSync,Alarm>::Load(
			Alarm* alarm
			, const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			alarm->setLevel (static_cast<AlarmLevel>(rows->getInt ( AlarmTableSync::COL_LEVEL)));
			alarm->setShortMessage (rows->getText (AlarmTableSync::COL_SHORT_MESSAGE));
			alarm->setLongMessage (rows->getText (AlarmTableSync::COL_LONG_MESSAGE));
			alarm->setRawEditor(rows->getBool(AlarmTableSync::COL_RAW_EDITOR));
			alarm->setDone(rows->getBool(AlarmTableSync::COL_DONE));
			alarm->setDisplayDuration(rows->getInt (AlarmTableSync::COL_DISPLAY_DURATION));
			alarm->setDigitizedVersion(rows->getText(AlarmTableSync::COL_DIGITIZED_VERSION));

			std::string tagsString(rows->getText(AlarmTableSync::COL_TAGS));
			std::set<string> tags;
			boost::algorithm::split(tags, tagsString, is_any_of(","), token_compress_on );
			alarm->setTags(tags);

			// Section
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				alarm->setSection(NULL);
				RegistryKeyType id(rows->getDefault<RegistryKeyType>(AlarmTableSync::COL_MESSAGES_SECTION_ID));
				if(id) try
				{
					alarm->setSection(MessagesSectionTableSync::Get(id, env).get());
				}
				catch (ObjectNotFoundException<MessagesSection>& e)
				{
					Log::GetInstance().warn("Invalid section", e);
				}

				if(&env == &Env::GetOfficialEnv())
				{
					alarm->setDataSourceLinksWithRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(AlarmTableSync::COL_DATASOURCE_LINKS),
							env
					)	);
				}
				else
				{
					alarm->setDataSourceLinksWithoutRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(AlarmTableSync::COL_DATASOURCE_LINKS),
							env
					)	);
				}
			}

			if(dynamic_cast<AlarmTemplate*>(alarm))
			{
				AlarmTemplate& alarmTemplate(static_cast<AlarmTemplate&>(*alarm));
				if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
				{
					alarmTemplate.setScenario(
						ScenarioTableSync::GetCast<ScenarioTemplate>(
							rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID),
							env,
							linkLevel
						).get()
					);
					if(alarmTemplate.getScenario())
					{
						alarmTemplate.getScenario()->addMessage(alarmTemplate);
					}
				}
			}

			if(dynamic_cast<SentAlarm*>(alarm))
			{
				SentAlarm& sentAlarm(static_cast<SentAlarm&>(*alarm));
				if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
				{
					// Scenario
					sentAlarm.setScenario(
						ScenarioTableSync::GetCast<SentScenario>(
							rows->getLongLong(AlarmTableSync::COL_SCENARIO_ID),
							env,
							linkLevel
						).get()
					);
					if(sentAlarm.getScenario())
					{
						sentAlarm.getScenario()->addMessage(sentAlarm);
					}

					// Template
					RegistryKeyType id(rows->getLongLong(AlarmTableSync::COL_TEMPLATE_ID));
					if(id > 0)
					{
						sentAlarm.setTemplate(
							AlarmTableSync::GetCast<AlarmTemplate>(
								id,
								env,
								linkLevel
							).get()
						);
					}

					if(&env == &Env::GetOfficialEnv())
					{
						MessagesModule::UpdateActivatedMessages();
					}
				}
				sentAlarm.clearBroadcastPointsCache();
			}

			// Calendar
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType calendarId(
					rows->getDefault<RegistryKeyType>(
						AlarmTableSync::COL_CALENDAR_ID,
						RegistryKeyType(0)
				)	);
				alarm->setCalendar(NULL);
				if(calendarId)
				{
					try
					{
						alarm->setCalendar(
							ScenarioCalendarTableSync::Get(
								calendarId,
								env
							).get()
						);
					}
					catch(ObjectNotFoundException<ScenarioCalendar>&)
					{

					}
				}
			}
		}



		template<>
		void InheritanceLoadSavePolicy<AlarmTableSync,Alarm>::Save(
			Alarm* object,
			optional<DBTransaction&> transaction
		){
			bool isTemplate(dynamic_cast<AlarmTemplate*>(object) != NULL);
			ReplaceQuery<AlarmTableSync> query(*object);
			query.addField(isTemplate);
			query.addField(object->getLevel());
			query.addField(object->getShortMessage());
			query.addField(object->getLongMessage());
			query.addField(
				object->getScenario() ?
				object->getScenario()->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				(!isTemplate && static_cast<SentAlarm*>(object)->getTemplate()) ?
				static_cast<SentAlarm*>(object)->getTemplate()->getKey() :
				RegistryKeyType(0)
			);
			query.addField(object->getRawEditor());
			query.addField(object->getDone());
			query.addField(
						object->getSection() ?
						object->getSection()->getKey() :
						RegistryKeyType(0)
			);
			query.addField(
				object->getCalendar() ?
				object->getCalendar()->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(object->getDisplayDuration());
			query.addField(object->getDigitizedVersion());

			string tagsString = boost::algorithm::join(object->getTags(), ",");
			query.addField(tagsString);

			query.execute(transaction);
		}



		template<>
		void InheritanceLoadSavePolicy<AlarmTableSync, Alarm>::Unlink(
			Alarm* obj
		){
			if(obj->getScenario())
			{
				if(dynamic_cast<AlarmTemplate*>(obj))
				{
					AlarmTemplate& alarmTemplate(static_cast<AlarmTemplate&>(*obj));
					alarmTemplate.getScenario()->removeMessage(alarmTemplate);
				}
				if(dynamic_cast<SentAlarm*>(obj))
				{
					SentAlarm& sentAlarm(static_cast<SentAlarm&>(*obj));
					sentAlarm.getScenario()->removeMessage(sentAlarm);
				}
			}

			if(dynamic_cast<SentAlarm*>(obj))
			{
				// Prevent the message to stay active
				obj->setScenario(NULL);

				MessagesModule::UpdateActivatedMessages();
			}
		}


		template<> bool DBTableSyncTemplate<AlarmTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				boost::shared_ptr<const Alarm> alarm(AlarmTableSync::Get(object_id, env));
				if (dynamic_cast<const SentAlarm*>(alarm.get()))
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(DELETE_RIGHT);
				}
				else
				{
					return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
				}
			}
			catch (ObjectNotFoundException<AlarmObjectLink>&)
			{
				return false;
			}
		}



		template<> void DBTableSyncTemplate<AlarmTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;

			// Links
			AlarmObjectLinkTableSync::RemoveByMessage(id);

			// Message alternatives
			MessageAlternativeTableSync::SearchResult alternatives(
				MessageAlternativeTableSync::Search(env, id)
			);
			BOOST_FOREACH(const boost::shared_ptr<MessageAlternative>& alternative, alternatives)
			{
				MessageAlternativeTableSync::Remove(NULL, alternative->getKey(), transaction, false);
			}

		}



		template<> void DBTableSyncTemplate<AlarmTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlarmTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id, env));
			if (dynamic_cast<const SentAlarm*>(alarm.get()))
			{
				MessagesLog::AddDeleteEntry(static_cast<const SentAlarm*>(alarm.get()), (session ? session->getUser().get() : NULL));
			}
			else
			{
				MessagesLibraryLog::AddDeleteEntry(static_cast<const AlarmTemplate*>(alarm.get()), (session ? session->getUser().get() : NULL));
			}
		}
	}


	namespace messages
	{
		AlarmTableSync::SearchResult AlarmTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> scenarioId /*= boost::optional<util::RegistryKeyType>() */,
			int first /*= 0 */,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>() */,
			bool orderByLevel /*= false */,
			bool raisingOrder /*= false*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE.NAME << " AS a"
				<< " WHERE 1";
			if(scenarioId)
			{
				query << " AND " << COL_SCENARIO_ID << "=" << *scenarioId;
			}
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
}	}
