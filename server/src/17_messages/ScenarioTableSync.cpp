
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
		template<> const string FactorableTemplate<DBTableSync,ScenarioTableSync>::FACTORY_KEY("17.00.01 Alarm scenarii");
	}

	namespace messages
	{
		const string ScenarioTableSync::COL_IS_TEMPLATE = "is_template";
		const string ScenarioTableSync::COL_ENABLED = "is_enabled";
		const string ScenarioTableSync::COL_NAME = "name";
		const string ScenarioTableSync::COL_PERIODSTART = "period_start";
		const string ScenarioTableSync::COL_PERIODEND = "period_end";
		const string ScenarioTableSync::COL_FOLDER_ID("folder_id");
		const string ScenarioTableSync::COL_VARIABLES("variables");
		const string ScenarioTableSync::COL_TEMPLATE("template_id");
		const string ScenarioTableSync::COL_DATASOURCE_LINKS("datasource_links");
		const string ScenarioTableSync::COL_SECTIONS = "messages_section_ids";
		const string ScenarioTableSync::COL_EVENT_START = "event_start";
		const string ScenarioTableSync::COL_EVENT_END = "event_end";
		const string ScenarioTableSync::COL_ARCHIVED = "archived";

		const string ScenarioTableSync::VARIABLES_SEPARATOR = "|";
		const string ScenarioTableSync::VARIABLES_OPERATOR = "&";
		const string ScenarioTableSync::SECTIONS_SEPARATOR = ",";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScenarioTableSync>::TABLE(
			"t039_scenarios"
		);

		template<> const Field DBTableSyncTemplate<ScenarioTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ScenarioTableSync::COL_IS_TEMPLATE, SQL_INTEGER),
			Field(ScenarioTableSync::COL_ENABLED, SQL_INTEGER),
			Field(ScenarioTableSync::COL_NAME, SQL_TEXT),
			Field(ScenarioTableSync::COL_PERIODSTART, SQL_DATETIME),
			Field(ScenarioTableSync::COL_PERIODEND, SQL_DATETIME),
			Field(ScenarioTableSync::COL_FOLDER_ID, SQL_INTEGER),
			Field(ScenarioTableSync::COL_VARIABLES, SQL_TEXT),
			Field(ScenarioTableSync::COL_TEMPLATE, SQL_INTEGER),
			Field(ScenarioTableSync::COL_DATASOURCE_LINKS, SQL_TEXT),
			Field(ScenarioTableSync::COL_SECTIONS, SQL_TEXT),
			Field(ScenarioTableSync::COL_EVENT_START, SQL_DATETIME),
			Field(ScenarioTableSync::COL_EVENT_END, SQL_DATETIME),
			Field(ScenarioTableSync::COL_ARCHIVED, SQL_BOOLEAN),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ScenarioTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					ScenarioTableSync::COL_IS_TEMPLATE.c_str(),
					ScenarioTableSync::COL_PERIODSTART.c_str(),
			"")	);
			r.push_back(DBTableSync::Index(ScenarioTableSync::COL_FOLDER_ID.c_str(), ""));
			return r;
		}



		template<>
		boost::shared_ptr<Scenario> InheritanceLoadSavePolicy<ScenarioTableSync, Scenario>::GetNewObject(
			const Record& row
		){
			return row.getDefault<bool>(ScenarioTableSync::COL_IS_TEMPLATE, false)
				? boost::shared_ptr<Scenario>(new ScenarioTemplate(row.getDefault<RegistryKeyType>(TABLE_COL_ID)))
				: boost::shared_ptr<Scenario>(new SentScenario(row.getDefault<RegistryKeyType>(TABLE_COL_ID)))
			;
		}



		template<>
		void InheritanceLoadSavePolicy<ScenarioTableSync, Scenario>::Load(
			Scenario* object,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			object->setName(rows->getText ( ScenarioTableSync::COL_NAME));

			// Sections
			Scenario::Sections sections;
			string sectionsStr(rows->getText(ScenarioTableSync::COL_SECTIONS));
			trim(sectionsStr);
			if(!sectionsStr.empty())
			{
				vector<string> tokens;
				split(tokens, sectionsStr, is_any_of(ScenarioTableSync::SECTIONS_SEPARATOR));
				BOOST_FOREACH(const string& token, tokens)
				{
					try
					{
						sections.insert(
							MessagesSectionTableSync::Get(
								lexical_cast<RegistryKeyType>(token),
								env
							).get()
						);
					}
					catch (bad_lexical_cast&)
					{						
					}
					catch(ObjectNotFoundException<MessagesSection>&)
					{
					}
				}
			}
			object->setSections(sections);

			// Data source links
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				if(&env == &Env::GetOfficialEnv())
				{
					object->setDataSourceLinksWithRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(ScenarioTableSync::COL_DATASOURCE_LINKS),
							env
					)	);
				}
				else
				{
					object->setDataSourceLinksWithoutRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(ScenarioTableSync::COL_DATASOURCE_LINKS),
							env
					)	);
				}
			}

			if(dynamic_cast<SentScenario*>(object))
			{
				SentScenario& sentScenario(static_cast<SentScenario&>(*object));

				sentScenario.setIsEnabled(rows->getBool ( ScenarioTableSync::COL_ENABLED));
				sentScenario.setPeriodStart(rows->getDateTime( ScenarioTableSync::COL_PERIODSTART));
				sentScenario.setPeriodEnd(rows->getDateTime( ScenarioTableSync::COL_PERIODEND));
				sentScenario.setEventStart(rows->getDateTime( ScenarioTableSync::COL_EVENT_START));
				sentScenario.setEventEnd(rows->getDateTime( ScenarioTableSync::COL_EVENT_END));

				// Archived
				sentScenario.setArchived(rows->getBool(ScenarioTableSync::COL_ARCHIVED));

				// Variables
				const string txtVariables(rows->getText(ScenarioTableSync::COL_VARIABLES));
				SentScenario::VariablesMap variables;
				vector<string> tokens;
				split(tokens, txtVariables, is_any_of(ScenarioTableSync::VARIABLES_SEPARATOR));
				BOOST_FOREACH(const string& token, tokens)
				{
					if(token.empty()) continue;

					typedef split_iterator<string::const_iterator> string_split_iterator;
					string_split_iterator it(
						make_split_iterator(
							token,
							first_finder(ScenarioTableSync::VARIABLES_OPERATOR, is_iequal())
					)	);
					string code = copy_range<string>(*it);
					++it;
					if (it == string_split_iterator())
					{
						Log::GetInstance().warn("Bad value for variable definition on scenario table");
						continue;
					}
					variables.insert(make_pair(code, copy_range<string>(*it)));
				}
				sentScenario.setVariables(variables);

				if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
				{
					RegistryKeyType id(rows->getLongLong(ScenarioTableSync::COL_TEMPLATE));
					if(id > 0)
					try
					{
						sentScenario.setTemplate(
							static_cast<ScenarioTemplate*>(
								ScenarioTableSync::GetEditable(id, env, linkLevel).get()
						)	);
					}
					catch(Exception&)
					{
					}
				}
			}
			else if(dynamic_cast<ScenarioTemplate*>(object))
			{
				ScenarioTemplate& scenarioTemplate(static_cast<ScenarioTemplate&>(*object));

				if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
				{
					scenarioTemplate.setVariablesMap(
						ScenarioTableSync::GetVariables(scenarioTemplate.getKey())
					);

					RegistryKeyType id(rows->getLongLong(ScenarioTableSync::COL_FOLDER_ID));
					if(id > 0)
					{
						scenarioTemplate.setFolder(ScenarioFolderTableSync::GetEditable(id, env, linkLevel).get());
					}
				}
			}
		}



		template<>
		void InheritanceLoadSavePolicy<ScenarioTableSync, Scenario>::Save(
			Scenario* object,
			optional<DBTransaction&> transaction
		){
			// Variables
			ReplaceQuery<ScenarioTableSync> query(*object);
			ScenarioTemplate* scenarioTemplate(dynamic_cast<ScenarioTemplate*>(object));
			SentScenario* sentScenario(static_cast<SentScenario*>(object));
			bool isTemplate(scenarioTemplate);

			// Is template field
			query.addField(isTemplate);

			// Is enabled field
			query.addField(!isTemplate && sentScenario->getIsEnabled());

			// Name field
			query.addField(object->getName());

			// Period start field
			query.addFrameworkField<PtimeField>(
				isTemplate ?
				ptime(not_a_date_time) :
				sentScenario->getPeriodStart()
			);

			// Period end field
			query.addFrameworkField<PtimeField>(
				isTemplate ?
				ptime(not_a_date_time) :
				sentScenario->getPeriodEnd()
			);

			// Folder id field
			query.addField(
				(isTemplate && scenarioTemplate->getFolder()) ?
				scenarioTemplate->getFolder()->getKey() :
				RegistryKeyType(0)
			);

			// Variables field
			stringstream vars;
			if(!isTemplate)
			{
				const SentScenario::VariablesMap& variables(sentScenario->getVariables());
				bool firstVar(true);
				BOOST_FOREACH(const SentScenario::VariablesMap::value_type& variable, variables)
				{
					if(!firstVar)
					{
						vars << ScenarioTableSync::VARIABLES_SEPARATOR;
					}
					else
					{
						firstVar = false;
					}
					vars << variable.first << ScenarioTableSync::VARIABLES_OPERATOR << variable.second;
				}
			}
			query.addField(vars.str());

			// Template id field
			query.addField(
				(!isTemplate && sentScenario->getTemplate()) ?
				sentScenario->getTemplate()->getKey() :
				RegistryKeyType(0)
			);

			// Data source links field
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);

			// Sections field
			bool first(true);
			stringstream sectionsStr;
			BOOST_FOREACH(const MessagesSection* section, object->getSections())
			{
				if(first)
				{
					first = false;
				}
				else
				{
					sectionsStr << ScenarioTableSync::SECTIONS_SEPARATOR;
				}
				sectionsStr << section->getKey();
			}
			query.addField(sectionsStr.str());

			// Event start field
			query.addFrameworkField<PtimeField>(
				isTemplate ?
				ptime(not_a_date_time) :
				sentScenario->getEventStart()
			);

			// Event end field
			query.addFrameworkField<PtimeField>(
				isTemplate ?
				ptime(not_a_date_time) :
				sentScenario->getEventEnd()
			);

			// Archived field
			query.addField(!isTemplate && sentScenario->getArchived());

			// Run the query
			query.execute(transaction);
		}



		template<>
		void InheritanceLoadSavePolicy<ScenarioTableSync, Scenario>::Unlink(
			Scenario* obj
		){
			if(Env::GetOfficialEnv().contains(*obj))
			{
				obj->cleanDataSourceLinks(true);
			}
		}



		template<> bool DBTableSyncTemplate<ScenarioTableSync>::CanDelete(
			const server::Session* session,
			RegistryKeyType object_id
		){
			Env env;
			boost::shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(object_id, env));
			if(dynamic_cast<const ScenarioTemplate*>(scenario.get()))
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(DELETE_RIGHT);
			}
		}



		template<> void DBTableSyncTemplate<ScenarioTableSync>::BeforeDelete(
			RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;

			// Messages
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, id)
			);
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



		template<> void DBTableSyncTemplate<ScenarioTableSync>::AfterDelete(
			RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ScenarioTableSync>::LogRemoval(
			const server::Session* session,
			RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(id, env));
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
		ScenarioTemplate::VariablesMap ScenarioTableSync::GetVariables( RegistryKeyType scenarioId )
		{
			Env env;
			ScenarioTemplate::VariablesMap result;
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, scenarioId, 0, optional<size_t>(), false, false, FIELDS_ONLY_LOAD_LEVEL)
			);
			BOOST_FOREACH(const boost::shared_ptr<const Alarm>& alarm, alarms)
			{
				ScenarioTemplate::GetVariablesInformations(alarm->getShortMessage(), result);
				ScenarioTemplate::GetVariablesInformations(alarm->getLongMessage(), result);
			}
			return result;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Performs a copy of each message from a scenario to a new one.
		/// @param sourceId the if of the scenario to copy
		/// @param dest the new scenario
		/// @param transaction the transaction
		void ScenarioTableSync::CopyMessages(
			RegistryKeyType sourceId,
			Scenario& dest,
			optional<DBTransaction&> transaction
		){
			// Variables
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
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, sourceId)
			);

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
				boost::shared_ptr<Alarm> alarm;
				if(dynamic_cast<const SentScenario*>(&dest))
				{
					alarm.reset(
						new SentAlarm(
							static_cast<const SentScenario&>(dest),
							*templateAlarm
					)	);
				}
				else
				{
					alarm.reset(
						new AlarmTemplate(
							static_cast<const ScenarioTemplate&>(dest),
							*templateAlarm
					)	);
				}
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



		void ScenarioTableSync::WriteVariablesIntoMessages( const SentScenario& scenario )
		{
			Env env;
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, scenario.getKey())
			);

			const SentScenario::VariablesMap& values(scenario.getVariables());

			BOOST_FOREACH(const boost::shared_ptr<Alarm>& alarm, alarms)
			{
				SentAlarm& sentAlarm(static_cast<SentAlarm&>(*alarm));

				if (!sentAlarm.getTemplate()) continue;

				// Retrieve the texts from both the alarm and its template :
				// If the alarm has non-empty texts, then they could have been modified since its instanciation
				// We want to preserve these modifications, so we apply the parameters to the alarm texts instead of the template texts
				const std::string& alarmShortMsg = alarm->getShortMessage();
				const std::string& alarmLongMsg  = alarm->getLongMessage();

				const std::string& templateShortMsg = sentAlarm.getTemplate()->getShortMessage();
				const std::string& templateLongMsg  = sentAlarm.getTemplate()->getLongMessage();

				std::string newShortMsg = "";
				std::string newLongMsg  = "";

				if(!alarmShortMsg.empty() || !alarmLongMsg.empty())
				{
					newShortMsg = ScenarioTemplate::WriteTextFromVariables(alarmShortMsg, values);
					newLongMsg  = ScenarioTemplate::WriteTextFromVariables(alarmLongMsg, values);
				}

				else
				{
					newShortMsg = ScenarioTemplate::WriteTextFromVariables(templateShortMsg, values);
					newLongMsg  = ScenarioTemplate::WriteTextFromVariables(templateLongMsg, values);
				}

				// Update the alarm texts and write it into database
				alarm->setShortMessage(newShortMsg);
				alarm->setLongMessage(newLongMsg);

				AlarmTableSync::Save(alarm.get());
			}

		}



		ScenarioTableSync::SearchResult ScenarioTableSync::SearchTemplates(
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
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=1";
			if(folderId)
			{
				query << " AND (" << COL_FOLDER_ID << "=" << *folderId;
				if (*folderId == 0)
					query << " OR " << COL_FOLDER_ID << " IS NULL";
				query << ")";
			}
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToDBString(name);
			if (scenarioToBeDifferentWith)
				query << " AND " << TABLE_COL_ID << "!=" << scenarioToBeDifferentWith->getKey();
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



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
		ScenarioTableSync::SearchResult ScenarioTableSync::SearchSentScenarios(
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
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< COL_IS_TEMPLATE << "=0";

			if (name)
			{
				query << " AND " << COL_NAME << " LIKE " << Conversion::ToDBString(*name);
			}

			// Archive filter
			if(inArchive)
			{
				ptime date(second_clock::local_time());
				if(*inArchive)
				{
					query << " AND " <<
						ScenarioTableSync::COL_ARCHIVED << " = 1 "
					;
				}
				else
				{
					query << " AND " <<
						ScenarioTableSync::COL_ARCHIVED << " = 0 "
					;
				}
			}

			// Active filter
			if(isActive)
			{
				query << " AND " << ScenarioTableSync::COL_ENABLED << "=" << *isActive;
			}
		
			if(scenarioId)
			{
				query << " AND " << ScenarioTableSync::COL_TEMPLATE << "=" << *scenarioId;
			}

			if(orderByDate)
			{
				query <<
					" ORDER BY " <<
					ScenarioTableSync::COL_PERIODSTART << " IS NULL " << (raisingOrder ? "DESC" : "ASC") << "," <<
					ScenarioTableSync::COL_PERIODSTART << (raisingOrder ? " ASC" : " DESC") << "," <<
					ScenarioTableSync::COL_PERIODEND << " IS NULL " << (raisingOrder ? "ASC" : "DESC") << "," <<
					ScenarioTableSync::COL_PERIODEND << (raisingOrder ? " ASC" : " DESC")
				;
			}

			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first)
					query << " OFFSET " << *first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		ScenarioTableSync::SearchResult ScenarioTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name /*= boost::optional<std::string>()*/,
			boost::optional<int> first /*= boost::optional<int>()*/,
			boost::optional<size_t> number /*= boost::optional<size_t>()*/,
			bool orderByName /*= false*/,
			bool raisingOrder /*= false*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1";
			if (name)
				query << " AND " << COL_NAME << "=" << Conversion::ToDBString(*name);
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
