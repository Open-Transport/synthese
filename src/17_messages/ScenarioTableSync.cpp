
/** ScenarioTableSync class implementation.
	@file ScenarioTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "SentScenario.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "AlarmTableSync.h"
#include "Conversion.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "AlarmTableSync.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
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
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ScenarioTableSync>::TABLE(
			"t039_scenarios"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<ScenarioTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ScenarioTableSync::COL_IS_TEMPLATE, SQL_INTEGER),
			DBTableSync::Field(ScenarioTableSync::COL_ENABLED, SQL_INTEGER),
			DBTableSync::Field(ScenarioTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(ScenarioTableSync::COL_PERIODSTART, SQL_DATETIME),
			DBTableSync::Field(ScenarioTableSync::COL_PERIODEND, SQL_DATETIME),
			DBTableSync::Field(ScenarioTableSync::COL_FOLDER_ID, SQL_INTEGER),
			DBTableSync::Field(ScenarioTableSync::COL_VARIABLES, SQL_TEXT),
			DBTableSync::Field(ScenarioTableSync::COL_TEMPLATE, SQL_INTEGER),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<ScenarioTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				ScenarioTableSync::COL_IS_TEMPLATE.c_str(),
				ScenarioTableSync::COL_PERIODSTART.c_str(),
			""),
			DBTableSync::Index(ScenarioTableSync::COL_FOLDER_ID.c_str(), ""),
			DBTableSync::Index()
		};


		template<>
		string DBInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_GetSubClassKey(const DBResultSPtr& row)
		{
			return row->getBool(ScenarioTableSync::COL_IS_TEMPLATE)
				? ScenarioTemplateInheritedTableSync::FACTORY_KEY
				: SentScenarioInheritedTableSync::FACTORY_KEY
			;
		}



		template<>
		string DBInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_GetSubClassKey(const Scenario* obj)
		{
			return (dynamic_cast<const SentScenario*>(obj) != NULL)
				?	SentScenarioInheritedTableSync::FACTORY_KEY
				:	ScenarioTemplateInheritedTableSync::FACTORY_KEY
			;
		}



		template<> void DBInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>::_CommonLoad(
			Scenario* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( ScenarioTableSync::COL_NAME));
		}



		template<> bool DBTableSyncTemplate<ScenarioTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
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
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			shared_ptr<const Scenario> scenario(ScenarioTableSync::Get(id, env));
			if(dynamic_cast<const ScenarioTemplate*>(scenario.get()))
			{
				AlarmTemplateInheritedTableSync::SearchResult alarms(
					AlarmTemplateInheritedTableSync::Search(env, id)
				);
				BOOST_FOREACH(shared_ptr<AlarmTemplate> alarm, alarms)
				{
					AlarmTableSync::Remove(NULL, alarm->getKey(), transaction, false);
				}
			}
			else
			{
				ScenarioSentAlarmInheritedTableSync::SearchResult alarms(
					ScenarioSentAlarmInheritedTableSync::Search(env, id)
				);
				BOOST_FOREACH(shared_ptr<SentAlarm> alarm, alarms)
				{
					AlarmTableSync::Remove(NULL, alarm->getKey(), transaction, false);
				}
			}
		}



		template<> void DBTableSyncTemplate<ScenarioTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		void DBTableSyncTemplate<ScenarioTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
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
		ScenarioTableSync::ScenarioTableSync()
			: DBInheritanceTableSyncTemplate<ScenarioTableSync,Scenario>()
		{
		}

		ScenarioTableSync::~ScenarioTableSync ()
		{
		}
	}
}
