
/** PTUseRulesTableSync class implementation.
	@file PTUseRulesTableSync.cpp

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

#include "PTUseRuleTableSync.h"
#include "PTUseRule.h"

using namespace std;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace time;

	template<> const string util::FactorableTemplate<SQLiteTableSync,PTUseRuleTableSync>::FACTORY_KEY("35.10.06 Public transportation use rules");

	namespace pt
	{
		const string PTUseRuleTableSync::COL_RESERVATION_TYPE("reservation_type");
		const string PTUseRuleTableSync::COL_CAPACITY("capacity");
		const string PTUseRuleTableSync::COL_ORIGINISREFERENCE ("origin_is_reference");
		const string PTUseRuleTableSync::COL_MINDELAYMINUTES ("min_delay_minutes");
		const string PTUseRuleTableSync::COL_MINDELAYDAYS ("min_delay_days");
		const string PTUseRuleTableSync::COL_MAXDELAYDAYS ("max_delay_days");
		const string PTUseRuleTableSync::COL_HOURDEADLINE ("hour_deadline");
		const string PTUseRuleTableSync::COL_NAME("name");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<PTUseRuleTableSync>::TABLE(
			"t061_pt_use_rules"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<PTUseRuleTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_CAPACITY, SQL_TEXT),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_RESERVATION_TYPE, SQL_INTEGER),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_ORIGINISREFERENCE, SQL_BOOLEAN),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_MINDELAYMINUTES, SQL_INTEGER),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_MINDELAYDAYS, SQL_INTEGER),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_MAXDELAYDAYS, SQL_INTEGER),
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_HOURDEADLINE, SQL_TIME),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<PTUseRuleTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Load(
			PTUseRule* rr,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			bool originIsReference (rows->getBool (PTUseRuleTableSync::COL_ORIGINISREFERENCE));

			int minDelayMinutes = rows->getInt (PTUseRuleTableSync::COL_MINDELAYMINUTES);
			int minDelayDays = rows->getInt (PTUseRuleTableSync::COL_MINDELAYDAYS);
			int maxDelayDays = rows->getInt (PTUseRuleTableSync::COL_MAXDELAYDAYS);


			Hour hourDeadline = 
			Hour::FromSQLTime (rows->getText (PTUseRuleTableSync::COL_HOURDEADLINE));

			PTUseRule::ReservationRuleType ruleType(static_cast<PTUseRule::ReservationRuleType>(rows->getInt(PTUseRuleTableSync::COL_RESERVATION_TYPE)));

			rr->setReservationType(ruleType);
 		    rr->setOriginIsReference (originIsReference);
 		    rr->setMinDelayMinutes (minDelayMinutes);
 		    rr->setMinDelayDays (minDelayDays);
 		    rr->setMaxDelayDays (maxDelayDays);
 		    rr->setHourDeadLine (hourDeadline);
			rr->setName(rows->getText(PTUseRuleTableSync::COL_NAME));
			rr->setAccessCapacity(rows->getOptionalUnsignedInt(PTUseRuleTableSync::COL_CAPACITY));
		}

		template<> void SQLiteDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Save(
			PTUseRule* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == UNKNOWN_VALUE)
				object->setKey(getId());
			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES(" <<
				Conversion::ToSQLiteString(object->getName()) << "," <<
				object->getKey() << "," <<
				(object->getAccessCapacity() ? Conversion::ToString(*object->getAccessCapacity()) : "''") << "," <<
				static_cast<int>(object->getReservationType()) << "," <<
				object->getMinDelayMinutes() << "," <<
				object->getMinDelayDays() << "," <<
				object->getMaxDelayDays() << "," <<
				object->getHourDeadLine().toSQLString() << "," <<
			")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Unlink(
			PTUseRule* obj
		){

		}
	}

		namespace pt
		{
			PTUseRuleTableSync::PTUseRuleTableSync()
				: SQLiteRegistryTableSyncTemplate<PTUseRuleTableSync,PTUseRule>()
			{
			}

	}
}