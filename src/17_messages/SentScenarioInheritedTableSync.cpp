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
#include "DateTime.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "AlarmObjectLinkTableSync.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;
	using namespace time;
	

	template<>
	const string util::FactorableTemplate<ScenarioTableSync,SentScenarioInheritedTableSync>::FACTORY_KEY("SentScenarioInheritedTableSync");

	namespace db
	{
		
		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Load(
			SentScenario* obj,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			obj->setIsEnabled(rows->getBool ( ScenarioTableSync::COL_ENABLED));
			obj->setPeriodStart(DateTime::FromSQLTimestamp (rows->getText ( ScenarioTableSync::COL_PERIODSTART)));
			obj->setPeriodEnd(DateTime::FromSQLTimestamp (rows->getText ( ScenarioTableSync::COL_PERIODEND)));

			const string txtVariables(rows->getText(ScenarioTableSync::COL_VARIABLES));
			SentScenario::VariablesMap variables;
			vector<string> tokens;
			split(tokens, txtVariables, is_any_of("|"));
			BOOST_FOREACH(const string& token, tokens)
			{
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
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Unlink(
			SentScenario* obj
		){

		}

		template<>
		void SQLiteInheritedTableSyncTemplate<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>::Save(
			SentScenario* obj
		){
			SQLite* sqlite = DBModule::GetSQLite();
			if (obj->getKey() == UNKNOWN_VALUE)
				obj->setKey(getId());

			stringstream query;
			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(obj->getKey())
				<< ",0"
				<< "," << Conversion::ToString(obj->getIsEnabled())
				<< "," << Conversion::ToSQLiteString(obj->getName())
				<< "," << obj->getPeriodStart().toSQLString()
				<< "," << obj->getPeriodEnd().toSQLString()
				<< "," << UNKNOWN_VALUE
				<< ",\"";
			const SentScenario::VariablesMap& variables(obj->getVariables());
			bool firstVar(true);
			BOOST_FOREACH(const SentScenario::VariablesMap::value_type& variable, variables)
			{
				if (firstVar)
				{
					query << "|";
					firstVar = false;
				}
				query << variable.first << "$" << variable.second;
			}
			query << "\""
				<< "," << (obj->getTemplate() ? Conversion::ToString(obj->getTemplate()->getKey()) : "0")
				<< ")"
			;
			sqlite->execUpdate(query.str());

			stringstream alarmquery;
			alarmquery
				<< "UPDATE "
				<< AlarmTableSync::TABLE.NAME
				<< " SET "
				<< AlarmTableSync::COL_PERIODSTART << "=" << obj->getPeriodStart().toSQLString()
				<< "," << AlarmTableSync::COL_PERIODEND << "=" << obj->getPeriodEnd().toSQLString()
				<< " WHERE " 
				<< AlarmTableSync::COL_SCENARIO_ID << "=" << obj->getKey();
			sqlite->execUpdate(alarmquery.str());
		}
	}

	namespace messages
	{

		SentScenarioInheritedTableSync::SentScenarioInheritedTableSync()
			: SQLiteInheritedRegistryTableSync<ScenarioTableSync,SentScenarioInheritedTableSync,SentScenario>()
		{

		}

		void SentScenarioInheritedTableSync::Search(
			Env& env,
			time::DateTime startDate,
			time::DateTime endDate,
			const std::string name,
			int first /*= 0*/,
			int number /*= 0*/,
			bool orderByDate,
			bool orderByName,
			bool orderByStatus,
			bool orderByConflict,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE " 
				<< COL_IS_TEMPLATE << "=0";
			if (!startDate.isUnknown())
				query << " AND " << ScenarioTableSync::COL_PERIODEND << "<=" << startDate.toSQLString();
			if (!endDate.isUnknown())
				query << " AND " << ScenarioTableSync::COL_PERIODSTART << "<=" << endDate.toSQLString();
			if (!name.empty())
				query << " AND " << COL_NAME << "=" << Conversion::ToSQLiteString(name);
			if (orderByDate)
				query << " ORDER BY " << COL_PERIODSTART << (raisingOrder ? " ASC" : " DESC") << "," << COL_PERIODEND  << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
		
		
		
		void SentScenarioInheritedTableSync::CopyMessagesFromTemplate(
			util::RegistryKeyType sourceId,
			const SentScenario& dest
		){
			// The action on the alarms
			Env env;
			AlarmTemplateInheritedTableSync::Search(env, sourceId);
			BOOST_FOREACH(shared_ptr<AlarmTemplate> templateAlarm, env.getRegistry<AlarmTemplate>())
			{
				ScenarioSentAlarm alarm(dest, *templateAlarm);
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					templateAlarm->getKey(),
					alarm.getKey()
				);
			}
		}
		
		
		
		void SentScenarioInheritedTableSync::CopyMessagesFromOther(
			util::RegistryKeyType sourceId,
			const SentScenario& dest
		){
			// The action on the alarms
			Env env;
			ScenarioSentAlarmInheritedTableSync::Search(env, sourceId);
			BOOST_FOREACH(shared_ptr<ScenarioSentAlarm> templateAlarm, env.getRegistry<ScenarioSentAlarm>())
			{
				ScenarioSentAlarm alarm(dest, *templateAlarm);
				AlarmTableSync::Save(&alarm);

				AlarmObjectLinkTableSync::CopyRecipients(
					templateAlarm->getKey(),
					alarm.getKey()
				);
			}
		}
	}
}
