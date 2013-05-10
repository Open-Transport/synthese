
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
#include "MessagesLibraryRight.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "MessagesRight.h"
#include "MessagesSectionTableSync.hpp"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SentScenario.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTemplate.h"
#include "Session.h"
#include "User.h"

#include <sstream>

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
		shared_ptr<Scenario> InheritanceLoadSavePolicy<ScenarioTableSync, Scenario>::GetNewObject(
			const DBResultSPtr& row
		){
			return row->getBool(ScenarioTableSync::COL_IS_TEMPLATE)
				? shared_ptr<Scenario>(new ScenarioTemplate(row->getKey()))
				: shared_ptr<Scenario>(new SentScenario(row->getKey()))
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
				split(tokens, sectionsStr, is_any_of(","));
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

				const string txtVariables(rows->getText(ScenarioTableSync::COL_VARIABLES));
				SentScenario::VariablesMap variables;
				vector<string> tokens;
				split(tokens, txtVariables, is_any_of("|"));
				BOOST_FOREACH(const string& token, tokens)
				{
					if(token.empty()) continue;

					typedef split_iterator<string::const_iterator> string_split_iterator;
					string_split_iterator it = make_split_iterator(token, first_finder("$", is_iequal()));
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
			ReplaceQuery<ScenarioTableSync> query(*object);

			if(dynamic_cast<ScenarioTemplate*>(object))
			{
				ScenarioTemplate& scenarioTemplate(static_cast<ScenarioTemplate&>(*object));
				query.addField(1);
				query.addField(0);
				query.addField(object->getName());
				query.addFieldNull();
				query.addFieldNull();
				query.addField(scenarioTemplate.getFolder() ? scenarioTemplate.getFolder()->getKey() : RegistryKeyType(0));
				query.addField(string());
				query.addField(0);
			}
			else if(dynamic_cast<SentScenario*>(object))
			{
				SentScenario& sentScenario(static_cast<SentScenario&>(*object));

				// Preparation
				stringstream vars;
				const SentScenario::VariablesMap& variables(sentScenario.getVariables());
				bool firstVar(true);
				BOOST_FOREACH(const SentScenario::VariablesMap::value_type& variable, variables)
				{
					if(!firstVar)
					{
						vars << "|";
					}
					else
					{
						firstVar = false;
					}
					vars << variable.first << "$" << variable.second;
				}

				// Main replace query
				query.addField(0);
				query.addField(sentScenario.getIsEnabled());
				query.addField(object->getName());
				query.addFrameworkField<PtimeField>(sentScenario.getPeriodStart());
				query.addFrameworkField<PtimeField>(sentScenario.getPeriodEnd());
				query.addField(RegistryKeyType(0));
				query.addField(vars.str());
				query.addField(sentScenario.getTemplate() ? sentScenario.getTemplate()->getKey() : RegistryKeyType(0));
			}

			// Data source links
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);

			// Sections
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
					sectionsStr << ",";
				}
				sectionsStr << section->getKey();
			}
			query.addField(sectionsStr.str());

			if(dynamic_cast<ScenarioTemplate*>(object))
			{
				query.addFieldNull();
				query.addFieldNull();
			}
			else
			{
				SentScenario& sentScenario(static_cast<SentScenario&>(*object));
				query.addFrameworkField<PtimeField>(sentScenario.getEventStart());
				query.addFrameworkField<PtimeField>(sentScenario.getEventEnd());
			}

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
			shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(object_id, env));
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
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, id)
			);
			BOOST_FOREACH(const shared_ptr<Alarm>& alarm, alarms)
			{
				AlarmTableSync::Remove(NULL, alarm->getKey(), transaction, false);
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
			shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(id, env));
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
			BOOST_FOREACH(const shared_ptr<const Alarm>& alarm, alarms)
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
			const Scenario& dest,
			optional<DBTransaction&> transaction
		){
			// The existing messages
			Env env;
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, sourceId)
			);

			// Copy of each message
			BOOST_FOREACH(const shared_ptr<Alarm>& templateAlarm, alarms)
			{
				// Message creation
				shared_ptr<Alarm> alarm;
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

			BOOST_FOREACH(const shared_ptr<Alarm>& alarm, alarms)
			{
				SentAlarm& sentAlarm(static_cast<SentAlarm&>(*alarm));

				if (!sentAlarm.getTemplate()) continue;

				alarm->setShortMessage(
					ScenarioTemplate::WriteTextFromVariables(sentAlarm.getTemplate()->getShortMessage(), values)
				);
				alarm->setLongMessage(
					ScenarioTemplate::WriteTextFromVariables(sentAlarm.getTemplate()->getLongMessage(), values)
				);

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
						ScenarioTableSync::COL_PERIODEND << " IS NOT NULL AND " <<
						ScenarioTableSync::COL_PERIODEND << "!='' AND " <<
						ScenarioTableSync::COL_PERIODEND << "<='" << to_iso_extended_string(date.date()) << " " << to_simple_string(date.time_of_day()) << "'"
					;
				}
				else
				{
					query << " AND (" <<
						ScenarioTableSync::COL_PERIODEND << " IS NULL OR " <<
						ScenarioTableSync::COL_PERIODEND << "='' OR " <<
						ScenarioTableSync::COL_PERIODEND << ">'" << to_iso_extended_string(date.date()) << " " << to_simple_string(date.time_of_day()) << "'" <<
						")"
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
