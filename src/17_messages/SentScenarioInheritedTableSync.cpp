////////////////////////////////////////////////////////////////////////////////
/// SentScenarioInheritedTableSync class implementation.
///	@file SentScenarioInheritedTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "SentScenarioInheritedTableSync.h"
#include "SentScenario.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "SentAlarm.h"
#include "ReplaceQuery.h"
#include "UpdateQuery.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;


	template<>
	const string util::FactorableTemplate<ScenarioTableSync,SentScenarioInheritedTableSync>::FACTORY_KEY("SentScenarioInheritedTableSync");

	namespace db
	{

		template<>
		void DBInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Load(
			SentScenario* obj,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			obj->setIsEnabled(rows->getBool ( ScenarioTableSync::COL_ENABLED));
			obj->setPeriodStart(rows->getDateTime( ScenarioTableSync::COL_PERIODSTART));
			obj->setPeriodEnd(rows->getDateTime( ScenarioTableSync::COL_PERIODEND));

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
			obj->setVariables(variables);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(ScenarioTableSync::COL_TEMPLATE));
				if(id > 0)
				try
				{
					obj->setTemplate(ScenarioTemplateInheritedTableSync::Get(id, env, linkLevel).get());
				}
				catch(Exception& e)
				{
				}
			}
		}

		template<>
		void DBInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Unlink(
			SentScenario* obj
		){

		}

		template<>
		void DBInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Save(
			SentScenario* obj,
			optional<DBTransaction&> transaction
		){
			// Preparation
			stringstream vars;
			const SentScenario::VariablesMap& variables(obj->getVariables());
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
			ReplaceQuery<ScenarioTableSync> query(*obj);
			query.addField(0);
			query.addField(obj->getIsEnabled());
			query.addField(obj->getName());
			query.addField(obj->getPeriodStart());
			query.addField(obj->getPeriodEnd());
			query.addField(UNKNOWN_VALUE);
			query.addField(vars.str());
			query.addField(obj->getTemplate() ? obj->getTemplate()->getKey() : RegistryKeyType(0));
			query.execute(transaction);

			// Alarms query
			UpdateQuery<AlarmTableSync> alarmsQuery;
			alarmsQuery.addUpdateField(
				AlarmTableSync::COL_PERIODSTART,
				to_iso_extended_string(obj->getPeriodStart().date()) + " " +
				to_simple_string(obj->getPeriodStart().time_of_day())
			);
			alarmsQuery.addUpdateField(
				AlarmTableSync::COL_PERIODEND,
				to_iso_extended_string(obj->getPeriodEnd().date()) + " " +
				to_simple_string(obj->getPeriodEnd().time_of_day())
			);
			alarmsQuery.addWhereField(AlarmTableSync::COL_SCENARIO_ID, obj->getKey());
			alarmsQuery.execute(transaction);
		}
	}

	namespace messages
	{
		SentScenarioInheritedTableSync::SearchResult SentScenarioInheritedTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name /*= boost::optional<std::string>()*/,
			/*AlarmConflict conflict, */
			/*AlarmLevel level, */
			boost::optional<StatusSearch> status /*= boost::optional<StatusSearch>()*/,
			boost::optional<ptime> date,
			boost::optional<util::RegistryKeyType> scenarioId /*= boost::optional<util::RegistryKeyType>()*/,
			boost::optional<int> first /*= boost::optional<int>()*/,
			boost::optional<std::size_t> number /*= boost::optional<int>()*/,
			bool orderByDate /*= true*/,
			bool orderByName /*= false*/,
			bool orderByStatus /*= false*/,
			//bool orderByConflict /*= false*/,
			bool raisingOrder /*= false*/,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
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



		void SentScenarioInheritedTableSync::CopyMessagesFromTemplate(
			util::RegistryKeyType sourceId,
			const SentScenario& dest
		){
			// The action on the alarms
			Env env;
			AlarmTemplateInheritedTableSync::SearchResult alarms(
				AlarmTemplateInheritedTableSync::Search(env, sourceId)
			);
			BOOST_FOREACH(shared_ptr<AlarmTemplate> templateAlarm, alarms)
			{
				SentAlarm alarm(dest, *templateAlarm);
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					*templateAlarm,
					alarm
				);
			}
		}



		void SentScenarioInheritedTableSync::CopyMessagesFromOther(
			util::RegistryKeyType sourceId,
			const SentScenario& dest
		){
			// The action on the alarms
			Env env;
			ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
				ScenarioSentAlarmInheritedTableSync::Search(env, sourceId)
			);
			BOOST_FOREACH(shared_ptr<SentAlarm> templateAlarm, alarms)
			{
				SentAlarm alarm(static_cast<SentAlarm&>(*templateAlarm));
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					*templateAlarm,
					alarm
				);
			}
		}



		void SentScenarioInheritedTableSync::WriteVariablesIntoMessages( const SentScenario& scenario )
		{
			Env env;
			ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
				ScenarioSentAlarmInheritedTableSync::Search(env, scenario.getKey())
			);

			const SentScenario::VariablesMap& values(scenario.getVariables());

			BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
			{
				if (!alarm->getTemplate()) continue;

				alarm->setShortMessage(
					ScenarioTemplate::WriteTextFromVariables(alarm->getTemplate()->getShortMessage(), values)
				);
				alarm->setLongMessage(
					ScenarioTemplate::WriteTextFromVariables(alarm->getTemplate()->getLongMessage(), values)
				);

				AlarmTableSync::Save(alarm.get());
			}
		}
	}
}
