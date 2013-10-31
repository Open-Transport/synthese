
/** PTUseRuleTableSync class implementation.
	@file PTUseRuleTableSync.cpp

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

#include "PTUseRuleTableSync.h"

#include "DaysField.hpp"
#include "MinutesField.hpp"
#include "Profile.h"
#include "PtimeField.hpp"
#include "PTUseRule.h"
#include "Session.h"
#include "TimeField.hpp"
#include "User.h"
#include "FareTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "TransportNetworkRight.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::algorithm;

namespace synthese
{
	using namespace fare;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace security;
	using namespace graph;

	template<> const string util::FactorableTemplate<DBTableSync,PTUseRuleTableSync>::FACTORY_KEY("35.10.06 Public transportation use rules");

	namespace pt
	{
		const string PTUseRuleTableSync::COL_RESERVATION_TYPE("reservation_type");
		const string PTUseRuleTableSync::COL_CAPACITY("capacity");
		const string PTUseRuleTableSync::COL_ORIGINISREFERENCE ("origin_is_reference");
		const string PTUseRuleTableSync::COL_MINDELAYMINUTES ("min_delay_minutes");
		const string PTUseRuleTableSync::COL_MINDELAYDAYS ("min_delay_days");
		const string PTUseRuleTableSync::COL_MAXDELAYDAYS ("max_delay_days");
		const string PTUseRuleTableSync::COL_HOURDEADLINE ("hour_deadline");
		const string PTUseRuleTableSync::COL_RESERVATION_MIN_DEPARTURE_TIME = "reservation_min_departure_time";
		const string PTUseRuleTableSync::COL_RESERVATION_FORBIDDEN_DAYS = "reservation_forbidden_days";
		const string PTUseRuleTableSync::COL_NAME("name");
		const string PTUseRuleTableSync::COL_DEFAULT_FARE("default_fare_id");
		const string PTUseRuleTableSync::COL_FORBIDDEN_IN_DEPARTURE_BOARDS("forbidden_in_departure_boards");
		const string PTUseRuleTableSync::COL_FORBIDDEN_IN_TIMETABLES("forbidden_in_timetables");
		const string PTUseRuleTableSync::COL_FORBIDDEN_IN_JOURNEY_PLANNING("forbidden_in_journey_planning");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PTUseRuleTableSync>::TABLE(
			"t061_pt_use_rules"
		);

		template<> const Field DBTableSyncTemplate<PTUseRuleTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(PTUseRuleTableSync::COL_NAME, SQL_TEXT),
			Field(PTUseRuleTableSync::COL_CAPACITY, SQL_TEXT),
			Field(PTUseRuleTableSync::COL_RESERVATION_TYPE, SQL_INTEGER),
			Field(PTUseRuleTableSync::COL_ORIGINISREFERENCE, SQL_BOOLEAN),
			Field(PTUseRuleTableSync::COL_MINDELAYMINUTES, SQL_INTEGER),
			Field(PTUseRuleTableSync::COL_MINDELAYDAYS, SQL_INTEGER),
			Field(PTUseRuleTableSync::COL_MAXDELAYDAYS, SQL_INTEGER),
			Field(PTUseRuleTableSync::COL_HOURDEADLINE, SQL_TIME),
			Field(PTUseRuleTableSync::COL_RESERVATION_MIN_DEPARTURE_TIME, SQL_TIME),
			Field(PTUseRuleTableSync::COL_RESERVATION_FORBIDDEN_DAYS, SQL_TEXT),
			Field(PTUseRuleTableSync::COL_DEFAULT_FARE, SQL_INTEGER),
			Field(PTUseRuleTableSync::COL_FORBIDDEN_IN_DEPARTURE_BOARDS, SQL_BOOLEAN),
			Field(PTUseRuleTableSync::COL_FORBIDDEN_IN_TIMETABLES, SQL_BOOLEAN),
			Field(PTUseRuleTableSync::COL_FORBIDDEN_IN_JOURNEY_PLANNING, SQL_BOOLEAN),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PTUseRuleTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<PTUseRuleTableSync,PTUseRule>::Load(
			PTUseRule* rr,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(rr->getLinkedObjectsIds(*rows), env, linkLevel);
			rr->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				rr->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void OldLoadSavePolicy<PTUseRuleTableSync,PTUseRule>::Save(
			PTUseRule* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<PTUseRuleTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getAccessCapacity() ? lexical_cast<string>(*object->getAccessCapacity()) : string());
			query.addField(static_cast<int>(object->getReservationType()));
			query.addField(object->getOriginIsReference());
			query.addFrameworkField<MinutesField>(object->getMinDelayMinutes());
			query.addFrameworkField<DaysField>(object->getMinDelayDays());
			query.addField(object->getMaxDelayDays() ? static_cast<int>(object->getMaxDelayDays()->days()) : int(0));
			query.addFrameworkField<TimeField>(object->getHourDeadLine());
			query.addFrameworkField<TimeField>(object->getReservationMinDepartureTime());
			query.addField(PTUseRuleTableSync::SerializeForbiddenDays(object->getReservationForbiddenDays()));
			query.addField(object->getDefaultFare() ? object->getDefaultFare()->getKey() : RegistryKeyType(0));
			query.addField(object->getForbiddenInDepartureBoards());
			query.addField(object->getForbiddenInTimetables());
			query.addField(object->getForbiddenInJourneyPlanning());
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<PTUseRuleTableSync,PTUseRule>::Unlink(
			PTUseRule* obj
		){

		}



		template<> bool DBTableSyncTemplate<PTUseRuleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<PTUseRuleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PTUseRuleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PTUseRuleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
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
			util::LinkLevel linkLevel
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



		std::string PTUseRuleTableSync::SerializeForbiddenDays( const PTUseRule::ReservationForbiddenDays& value )
		{
			bool first(true);
			stringstream forbiddenDays;
			BOOST_FOREACH(const date::day_of_week_type& day, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					forbiddenDays << ",";
				}
				forbiddenDays << int(day);
			}
			return forbiddenDays.str();
		}



		PTUseRule::ReservationForbiddenDays PTUseRuleTableSync::UnserializeForbiddenDays( const std::string& value )
		{
			if(value.empty())
			{
				return PTUseRule::ReservationForbiddenDays();
			}
			else
			{
				PTUseRule::ReservationForbiddenDays days;
				vector<string> daysVec;
				split(daysVec, value, is_any_of(","));
				BOOST_FOREACH(const string& dayStr, daysVec)
				{
					try
					{
						days.insert(static_cast<date::day_of_week_type>(lexical_cast<int>(dayStr)));
					}
					catch(bad_lexical_cast&)
					{
					}
				}
				return days;
			}
		}



		graph::RuleUser::Rules PTUseRuleTableSync::UnserializeUseRules(
			const std::string& value,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			RuleUser::Rules result(USER_CLASSES_VECTOR_SIZE, NULL);

			if(!value.empty())
			{
				vector<string> rules;
				split(rules, value, is_any_of(","));
				BOOST_FOREACH(const string& rule, rules)
				{
					vector<string> parts;
					split(parts, rule, is_any_of(":"));

					try
					{
						size_t i(lexical_cast<size_t>(parts[0]) - USER_CLASS_CODE_OFFSET);
						RegistryKeyType ruleId(lexical_cast<RegistryKeyType>(rule[1]));
						result[i] = GetEditable(ruleId, env, linkLevel).get();
					}
					catch(ObjectNotFoundException<PTUseRule>&)
					{
					}
				}
			}

			return result;
		}



		std::string PTUseRuleTableSync::SerializeUseRules(
			const graph::RuleUser::Rules& value
		){
			stringstream stream;
			bool first(true);
			for(size_t index(0); index<value.size(); ++index)
			{
				if(value[index] && dynamic_cast<const PTUseRule*>(value[index]))
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << ",";
					}
					stream << (USER_CLASS_CODE_OFFSET + index) << ":" << static_cast<const PTUseRule*>(value[index])->getKey();
				}
			}
			return stream.str();
		}
}	}
