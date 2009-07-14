
/** ReservationTableSync class implementation.
	@file ReservationTableSync.cpp

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

#include <sstream>

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "DateTime.h"

#include "ReservationTransaction.h"
#include "ReservationTableSync.h"
#include "ReservationTransactionTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "LineTableSync.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace time;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,ReservationTableSync>::FACTORY_KEY("31.1 Reservation Table Sync");
	}

	namespace resa
	{
		const string ReservationTableSync::COL_TRANSACTION_ID("transaction_id");
		const string ReservationTableSync::COL_LINE_ID = "line_id";
		const string ReservationTableSync::COL_LINE_CODE = "line_code";
		const string ReservationTableSync::COL_SERVICE_ID = "service_id";
		const string ReservationTableSync::COL_SERVICE_CODE = "service_code";
		const string ReservationTableSync::COL_DEPARTURE_PLACE_ID = "departure_place_id";
		const string ReservationTableSync::COL_DEPARTURE_PLACE_NAME = "departure_place_name";
		const string ReservationTableSync::COL_DEPARTURE_TIME = "departure_time";
		const string ReservationTableSync::COL_ARRIVAL_PLACE_ID = "arrival_place_id";
		const string ReservationTableSync::COL_ARRIVAL_PLACE_NAME = "arrival_place_name";
		const string ReservationTableSync::COL_ARRIVAL_TIME = "arrival_time";
		const string ReservationTableSync::COL_RESERVATION_RULE_ID = "reservation_rule_id";
		const string ReservationTableSync::COL_ORIGIN_DATE_TIME = "origin_date_time";
		const string ReservationTableSync::COL_RESERVATION_DEAD_LINE("reservation_dead_line");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ReservationTableSync>::TABLE(
			"t044_reservations", true
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ReservationTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ReservationTableSync::COL_TRANSACTION_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ReservationTableSync::COL_LINE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ReservationTableSync::COL_LINE_CODE, SQL_TEXT),
			SQLiteTableSync::Field(ReservationTableSync::COL_SERVICE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ReservationTableSync::COL_SERVICE_CODE, SQL_TEXT),
			SQLiteTableSync::Field(ReservationTableSync::COL_DEPARTURE_PLACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ReservationTableSync::COL_DEPARTURE_PLACE_NAME, SQL_TEXT),
			SQLiteTableSync::Field(ReservationTableSync::COL_DEPARTURE_TIME, SQL_TIMESTAMP),
			SQLiteTableSync::Field(ReservationTableSync::COL_ARRIVAL_PLACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ReservationTableSync::COL_ARRIVAL_PLACE_NAME, SQL_TEXT),
			SQLiteTableSync::Field(ReservationTableSync::COL_ARRIVAL_TIME, SQL_TIMESTAMP),
			SQLiteTableSync::Field(ReservationTableSync::COL_RESERVATION_RULE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(ReservationTableSync::COL_ORIGIN_DATE_TIME, SQL_TIMESTAMP),
			SQLiteTableSync::Field(ReservationTableSync::COL_RESERVATION_DEAD_LINE, SQL_TIMESTAMP),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ReservationTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				ReservationTableSync::COL_SERVICE_ID.c_str(),
				ReservationTableSync::COL_ORIGIN_DATE_TIME.c_str(),
			""),
			SQLiteTableSync::Index(
				ReservationTableSync::COL_TRANSACTION_ID.c_str(),
				ReservationTableSync::COL_DEPARTURE_TIME.c_str(),
			""),
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<ReservationTableSync,Reservation>::Load(
			Reservation* object
			, const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setLineId(rows->getLongLong ( ReservationTableSync::COL_LINE_ID));
			object->setLineCode(rows->getText ( ReservationTableSync::COL_LINE_CODE));
			object->setServiceId(rows->getLongLong(ReservationTableSync::COL_SERVICE_ID));
			object->setServiceCode(rows->getText ( ReservationTableSync::COL_SERVICE_CODE));
			object->setDeparturePlaceId(rows->getLongLong ( ReservationTableSync::COL_DEPARTURE_PLACE_ID));
			object->setDeparturePlaceName(rows->getText ( ReservationTableSync::COL_DEPARTURE_PLACE_NAME));
			object->setDepartureTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_DEPARTURE_TIME)));
			object->setArrivalPlaceId(rows->getLongLong ( ReservationTableSync::COL_ARRIVAL_PLACE_ID));
			object->setArrivalPlaceName(rows->getText ( ReservationTableSync::COL_ARRIVAL_PLACE_NAME));
			object->setArrivalTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_ARRIVAL_TIME)));
			object->setReservationRuleId(rows->getLongLong ( ReservationTableSync::COL_RESERVATION_RULE_ID));
			object->setOriginDateTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_ORIGIN_DATE_TIME)));
			object->setReservationDeadLine(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_RESERVATION_DEAD_LINE)));

			if(linkLevel == UP_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL)
			{
				object->setTransaction(
					ReservationTransactionTableSync::GetEditable(
						rows->getLongLong(ReservationTableSync::COL_TRANSACTION_ID),
						env, linkLevel
					).get()
				);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<ReservationTableSync,Reservation>::Unlink(Reservation* object)
		{
		}

		template<> void SQLiteDirectTableSyncTemplate<ReservationTableSync,Reservation>::Save(Reservation* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == UNKNOWN_VALUE)
				object->setKey(getId());
               
			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< lexical_cast<string>(object->getKey())
				<< "," << (object->getTransaction() ? lexical_cast<string>(object->getTransaction()->getKey()) : "0")
				<< "," << lexical_cast<string>(object->getLineId())
				<< "," << Conversion::ToSQLiteString(object->getLineCode())
				<< "," << lexical_cast<string>(object->getServiceId())
				<< "," << Conversion::ToSQLiteString(object->getServiceCode())
				<< "," << lexical_cast<string>(object->getDeparturePlaceId())
				<< "," << Conversion::ToSQLiteString(object->getDeparturePlaceName())
				<< "," << object->getDepartureTime().toSQLString()
				<< "," << lexical_cast<string>(object->getArrivalPlaceId())
				<< "," << Conversion::ToSQLiteString(object->getArrivalPlaceName())
				<< "," << object->getArrivalTime().toSQLString()
				<< "," << lexical_cast<string>(object->getReservationRuleId())
				<< "," << object->getOriginDateTime().toSQLString()
				<< "," << object->getReservationDeadLine().toSQLString()
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		ReservationTableSync::SearchResult ReservationTableSync::Search(
			Env& env,
			RegistryKeyType transactionId
			, int first /*= 0*/
			, boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				<< COL_TRANSACTION_ID << "=" << transactionId
			;
			query << " ORDER BY " << COL_DEPARTURE_TIME;
			if (number)
				query << " LIMIT " << lexical_cast<string>(*number + 1);
			if (first > 0)
				query << " OFFSET " << lexical_cast<string>(first);

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		ReservationTableSync::SearchResult ReservationTableSync::Search(
			util::Env& env,
			const util::RegistryKeyType commercialLineId,
			const Date& day,
			const optional<RegistryKeyType> serviceId,
			bool hideOldServices,
			logic::tribool cancellations,
			bool orderByService,
			bool raisingOrder,
			int first /*= 0 */,
			boost::optional<std::size_t> number  /*= 0*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query <<
				" SELECT " << TABLE.NAME << ".*" <<
				" FROM " << TABLE.NAME <<
				" INNER JOIN " << ScheduledServiceTableSync::TABLE.NAME <<
					" s ON s." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_SERVICE_ID <<
				" INNER JOIN " << LineTableSync::TABLE.NAME <<
					" l ON l." << TABLE_COL_ID << "=s." << ScheduledServiceTableSync::COL_PATHID
			;
			if(!indeterminate(cancellations))
			{
				query << " INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " t ON t." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_TRANSACTION_ID;
			}
			query <<
				" WHERE " <<
				"l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << commercialLineId << " AND " <<
				TABLE.NAME << "." << COL_ORIGIN_DATE_TIME << ">=" << day.toSQLString();
			Date dayp(day);
			dayp += 1;
			query <<
				" AND " << TABLE.NAME << "." << COL_ORIGIN_DATE_TIME << "<" << dayp.toSQLString()
			;
			if(serviceId)
			{
				query << " AND s." << TABLE_COL_ID << "=" << *serviceId;
			}
			if(!indeterminate(cancellations))
			{
				query << " AND t." << ReservationTransactionTableSync::COL_CANCELLATION_TIME << " IS " << (cancellations == true ? "NOT " : "") << " NULL";
			}

			if(hideOldServices)
			{
				Hour now(TIME_CURRENT);
				Schedule snow(now, now <= Hour(3,0));
				query <<
					" AND s." << ScheduledServiceTableSync::COL_SCHEDULES << ">='00:00:00#" <<
					snow.toSQLString(false) << "'" ;
			}
			
			if(orderByService)
			{
				query <<
					" ORDER BY substr(s." << ScheduledServiceTableSync::COL_SCHEDULES << ",0,17) " <<
					(raisingOrder ? "ASC" : "DESC");
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
