
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
#include "FareTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace db;

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
		const string PTUseRuleTableSync::COL_DEFAULT_FARE("default_fare_id");
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
			SQLiteTableSync::Field(PTUseRuleTableSync::COL_DEFAULT_FARE, SQL_INTEGER),
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

			time_duration minDelayMinutes = minutes(rows->getInt (PTUseRuleTableSync::COL_MINDELAYMINUTES));
			date_duration minDelayDays = days(rows->getInt (PTUseRuleTableSync::COL_MINDELAYDAYS));
			date_duration maxDelayDays = days(rows->getInt (PTUseRuleTableSync::COL_MAXDELAYDAYS));


			time_duration hourDeadline(rows->getHour(PTUseRuleTableSync::COL_HOURDEADLINE));

			PTUseRule::ReservationRuleType ruleType(static_cast<PTUseRule::ReservationRuleType>(rows->getInt(PTUseRuleTableSync::COL_RESERVATION_TYPE)));

			rr->setReservationType(ruleType);
 		    rr->setOriginIsReference (originIsReference);
 		    rr->setMinDelayMinutes (minDelayMinutes);
 		    rr->setMinDelayDays (minDelayDays);
			rr->setMaxDelayDays(maxDelayDays.days() > 0 ? maxDelayDays : optional<date_duration>());
 		    rr->setHourDeadLine (hourDeadline);
			rr->setName(rows->getText(PTUseRuleTableSync::COL_NAME));
			rr->setAccessCapacity(rows->getOptionalUnsignedInt(PTUseRuleTableSync::COL_CAPACITY));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(PTUseRuleTableSync::COL_DEFAULT_FARE));
				if(id > 0)
				{
					try
					{
						rr->setDefaultFare(FareTableSync::Get(id, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<Fare> e)
					{
						Log::GetInstance().warn("Fare "+ lexical_cast<string>(id) +" not found in PT Use Rule "+ lexical_cast<string>(rr->getKey()));
					}
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Save(
			PTUseRule* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<PTUseRuleTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getAccessCapacity() ? lexical_cast<string>(*object->getAccessCapacity()) : string());
			query.addField(static_cast<int>(object->getReservationType()));
			query.addField(object->getMinDelayMinutes().total_seconds() / 60);
			query.addField(static_cast<int>(object->getMinDelayDays().days()));
			query.addField(object->getMaxDelayDays() ? lexical_cast<string>(object->getMaxDelayDays()->days()) : string());
			query.addField(object->getHourDeadLine().is_not_a_date_time() ? string() : to_simple_string(object->getHourDeadLine()));
			query.addField(object->getDefaultFare() ? object->getDefaultFare()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<PTUseRuleTableSync,PTUseRule>::Unlink(
			PTUseRule* obj
		){

		}
	}

	namespace pt
	{
		PTUseRuleTableSync::SearchResult PTUseRuleTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name /*= boost::optional<std::string>()*/,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName /*= true*/,
			bool raisingOrder /*= true*/,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			SelectQuery<PTUseRuleTableSync> query;
			if(name)
			{
				query.addWhereField(COL_NAME, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
				if(first > 0)
				{
					query.setFirst(first);
				}
			}
			return LoadFromQuery(query, env, linkLevel);

		}

	}
}