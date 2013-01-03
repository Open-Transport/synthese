
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
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SentScenario.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTemplate.h"
#include "AlarmTableSync.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "AlarmTableSync.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "Session.h"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;

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
		const string ScenarioTableSync::COL_SECTIONS = "sections";
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
						sections.insert(lexical_cast<int>(token));
					}
					catch (bad_lexical_cast&)
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
			BOOST_FOREACH(int section, object->getSections())
			{
				if(first)
				{
					first = false;
				}
				else
				{
					sectionsStr << ",";
				}
				sectionsStr << section;
			}
			query.addField(sectionsStr.str());

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



		void ScenarioTableSync::CopyMessagesFromOther( RegistryKeyType sourceId, const ScenarioTemplate& dest )
		{
			// The action on the alarms
			Env env;
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, sourceId)
			);
			BOOST_FOREACH(const shared_ptr<Alarm>& templateAlarm, alarms)
			{
				AlarmTemplate alarm(dest, static_cast<AlarmTemplate&>(*templateAlarm));
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					*templateAlarm,
					alarm
				);
			}
		}



		void ScenarioTableSync::CopyMessagesFromOther( RegistryKeyType sourceId, const SentScenario& dest )
		{
			// The action on the alarms
			Env env;
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, sourceId)
			);
			BOOST_FOREACH(const shared_ptr<Alarm>& templateAlarm, alarms)
			{
				SentAlarm alarm(static_cast<SentAlarm&>(*templateAlarm));
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					*templateAlarm,
					alarm
				);
			}
		}



		void ScenarioTableSync::CopyMessagesFromTemplate(
			RegistryKeyType sourceId,
			const SentScenario& dest
		){
			// The action on the alarms
			Env env;
			AlarmTableSync::SearchResult alarms(
				AlarmTableSync::Search(env, sourceId)
			);
			BOOST_FOREACH(const shared_ptr<Alarm>& templateAlarm, alarms)
			{
				SentAlarm alarm(dest, static_cast<AlarmTemplate&>(*templateAlarm));
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					*templateAlarm,
					alarm
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



		ScenarioTableSync::SearchResult ScenarioTableSync::SearchSentScenarios(
			Env& env,
			optional<string> name /*= optional<string>()*/,
			/*AlarmConflict conflict, */
			/*AlarmLevel level, */
			optional<StatusSearch> status /*= optional<StatusSearch>()*/,
			optional<posix_time::ptime> date /*= optional<posix_time::ptime>()*/,
			optional<RegistryKeyType> scenarioId /*= optional<RegistryKeyType>()*/,
			optional<int> first /*= optional<int>()*/,
			optional<size_t> number /*= optional<size_t>()*/,
			bool orderByDate /*= true*/,
			bool orderByName /*= false*/,
			bool orderByStatus /*= false*/,
			/*bool orderByConflict = false, */
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

			if(status && date)
			{
				switch(*status)
				{
				case BROADCAST_OVER:
					query << " AND " <<
						ScenarioTableSync::COL_PERIODEND << " IS NOT NULL AND " <<
						ScenarioTableSync::COL_PERIODEND << "!='' AND " <<
						ScenarioTableSync::COL_PERIODEND << "<'" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						;
					break;

				case BROADCAST_RUNNING:
					query << " AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL OR " << ScenarioTableSync::COL_PERIODEND << "=''"
						<< " OR " << ScenarioTableSync::COL_PERIODEND << ">'" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						<< ") AND " << ScenarioTableSync::COL_ENABLED
						;
					break;

				case BROADCAST_RUNNING_WITH_END:
					query << " AND (" << ScenarioTableSync::COL_PERIODSTART << " IS NULL OR " << ScenarioTableSync::COL_PERIODSTART << "!=''"
						<< " OR " << ScenarioTableSync::COL_PERIODSTART << "<='" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						<< ") AND " << ScenarioTableSync::COL_ENABLED
						<<  " AND " << ScenarioTableSync::COL_PERIODEND << " IS NOT NULL AND " << ScenarioTableSync::COL_PERIODEND << "!=''"
						<< " AND " << ScenarioTableSync::COL_PERIODEND << ">'" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						;
					break;

				case BROADCAST_RUNNING_WITHOUT_END:
					query << " AND (" << ScenarioTableSync::COL_PERIODSTART << " IS NULL OR " << ScenarioTableSync::COL_PERIODSTART << "=''"
						<< " OR " << ScenarioTableSync::COL_PERIODSTART << "<='" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						<< ") AND " << ScenarioTableSync::COL_ENABLED
						<<  " AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL OR " << ScenarioTableSync::COL_PERIODEND << "='')"
						;
					break;

				case FUTURE_BROADCAST:
					query << " AND " << ScenarioTableSync::COL_PERIODSTART << " IS NOT NULL " << " AND " << ScenarioTableSync::COL_PERIODSTART << "!=''"
						<< " AND " << ScenarioTableSync::COL_PERIODSTART << ">'" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						<< " AND " << ScenarioTableSync::COL_ENABLED
						;
					break;

				case BROADCAST_DRAFT:
					query << " AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL "
						<< " OR " << ScenarioTableSync::COL_PERIODEND << "=''"
						<< " OR " << ScenarioTableSync::COL_PERIODEND << ">'" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'"
						<< ") AND NOT " << ScenarioTableSync::COL_ENABLED
						;
					break;
				}
			}

			if(!status && date)
			{
				query << " AND (" << ScenarioTableSync::COL_PERIODSTART << " IS NULL OR " << ScenarioTableSync::COL_PERIODSTART << "='' OR " <<
					ScenarioTableSync::COL_PERIODSTART << " <='" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'" <<
					") AND (" << ScenarioTableSync::COL_PERIODEND << " IS NULL OR " << ScenarioTableSync::COL_PERIODEND << "='' OR " <<
					ScenarioTableSync::COL_PERIODEND << " >='" << to_iso_extended_string(date->date()) << " " << to_simple_string(date->time_of_day()) << "'" <<
					")"
				;
			}

			if(scenarioId)
			{
				query << " AND " << ScenarioTableSync::COL_TEMPLATE << "=" << *scenarioId;
			}


			/*				<< ",(SELECT COUNT(" << AlarmObjectLinkTableSync::COL_OBJECT_ID << ") FROM " <<
			AlarmObjectLinkTableSync::TABLE.NAME << " AS aol3 WHERE aol3." <<
			AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID << ") AS " <<
			_COL_RECIPIENTS_NUMBER*/

			/*				<< ",(SELECT MAX(al2."  << COL_LEVEL << ") FROM " << AlarmObjectLinkTableSync::TABLE.NAME <<
			" AS aol1 INNER JOIN " << AlarmObjectLinkTableSync::TABLE.NAME << " AS aol2 ON aol1." <<
			AlarmObjectLinkTableSync::COL_OBJECT_ID << "=aol2." <<
			AlarmObjectLinkTableSync::COL_OBJECT_ID << " AND aol1." <<
			AlarmObjectLinkTableSync::COL_ALARM_ID << " != aol2." <<
			AlarmObjectLinkTableSync::COL_ALARM_ID << " INNER JOIN " << TABLE.NAME << " AS al2 ON al2." <<
			TABLE_COL_ID << " = aol2." << AlarmObjectLinkTableSync::COL_ALARM_ID << " WHERE "
			<< " aol1." << AlarmObjectLinkTableSync::COL_ALARM_ID << "=a." << TABLE_COL_ID
			<< " AND al2." << COL_IS_TEMPLATE << "=0 "
			<< " AND (al2." << COL_PERIODSTART << " IS NULL OR a." << COL_PERIODEND << " IS NULL OR al2." <<
			COL_PERIODSTART << " <= a." << COL_PERIODEND << ")"
			<< " AND (al2." << COL_PERIODEND << " IS NULL OR a." << COL_PERIODSTART << " IS NULL OR al2." <<
			COL_PERIODEND <<" >= a." << COL_PERIODSTART << ")"
			<< ") AS " << _COL_CONFLICT_LEVEL*/

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
