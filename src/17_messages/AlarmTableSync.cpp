/** AlarmTableSync class implementation.
	@file AlarmTableSync.cpp

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

#include "AlarmTableSync.h"

#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "AlarmTemplate.h"
#include "SentScenario.h"
#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "MessagesTypes.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "MessagesLibraryLog.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesLog.h"

#include "Conversion.h"

#include "DBResult.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
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
		const string AlarmTableSync::COL_ENABLED = "is_enabled";
		const string AlarmTableSync::COL_LEVEL = "level";
		const string AlarmTableSync::COL_SHORT_MESSAGE = "short_message";
		const string AlarmTableSync::COL_LONG_MESSAGE = "long_message";
		const string AlarmTableSync::COL_PERIODSTART = "period_start";
		const string AlarmTableSync::COL_PERIODEND = "period_end";
		const string AlarmTableSync::COL_SCENARIO_ID = "scenario_id";
		const string AlarmTableSync::COL_TEMPLATE_ID("template_id");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AlarmTableSync>::TABLE(
			"t003_alarms"
			);

		template<> const DBTableSync::Field DBTableSyncTemplate<AlarmTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(AlarmTableSync::COL_IS_TEMPLATE, SQL_INTEGER),
			DBTableSync::Field(AlarmTableSync::COL_ENABLED, SQL_INTEGER),
			DBTableSync::Field(AlarmTableSync::COL_LEVEL, SQL_INTEGER),
			DBTableSync::Field(AlarmTableSync::COL_SHORT_MESSAGE, SQL_TEXT),
			DBTableSync::Field(AlarmTableSync::COL_LONG_MESSAGE, SQL_TEXT),
			DBTableSync::Field(AlarmTableSync::COL_PERIODSTART, SQL_DATETIME),
			DBTableSync::Field(AlarmTableSync::COL_PERIODEND, SQL_DATETIME),
			DBTableSync::Field(AlarmTableSync::COL_SCENARIO_ID, SQL_INTEGER),
			DBTableSync::Field(AlarmTableSync::COL_TEMPLATE_ID, SQL_INTEGER),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<AlarmTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(AlarmTableSync::COL_SCENARIO_ID.c_str(),	AlarmTableSync::COL_PERIODSTART.c_str(), ""),
			DBTableSync::Index()
		};

		template<>
		string DBInheritanceTableSyncTemplate<AlarmTableSync,Alarm>::_GetSubClassKey(const DBResultSPtr& row)
		{
			return row->getBool(AlarmTableSync::COL_IS_TEMPLATE)
				? AlarmTemplateInheritedTableSync::FACTORY_KEY
				: ScenarioSentAlarmInheritedTableSync::FACTORY_KEY
			;
		}


		template<>
		string DBInheritanceTableSyncTemplate<AlarmTableSync,Alarm>::_GetSubClassKey(const Alarm* obj)
		{
			return	(dynamic_cast<const SentAlarm*>(obj) != NULL)
				?	ScenarioSentAlarmInheritedTableSync::FACTORY_KEY
				:	AlarmTemplateInheritedTableSync::FACTORY_KEY
			;
		}



		template<> void DBInheritanceTableSyncTemplate<AlarmTableSync,Alarm>::_CommonLoad(
			Alarm* alarm
			, const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			alarm->setLevel (static_cast<AlarmLevel>(rows->getInt ( AlarmTableSync::COL_LEVEL)));
			alarm->setShortMessage (rows->getText (AlarmTableSync::COL_SHORT_MESSAGE));
			alarm->setLongMessage (rows->getText (AlarmTableSync::COL_LONG_MESSAGE));
		}



		template<> bool DBTableSyncTemplate<AlarmTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				shared_ptr<const Alarm> alarm(AlarmTableSync::Get(object_id, env));
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
			AlarmObjectLinkTableSync::Remove(id);
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
			shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id, env));
			if (dynamic_cast<const SentAlarm*>(alarm.get()))
			{
				MessagesLog::AddDeleteEntry(static_cast<const SentAlarm*>(alarm.get()), session->getUser().get());
			}
			else
			{
				MessagesLibraryLog::AddDeleteEntry(static_cast<const AlarmTemplate*>(alarm.get()), session->getUser().get());
			}
		}
	}


	namespace messages
	{
		AlarmTableSync::AlarmTableSync ()
			: DBInheritanceTableSyncTemplate<AlarmTableSync,Alarm>()
		{
		}


		AlarmTableSync::~AlarmTableSync ()
		{
		}
	}
}
