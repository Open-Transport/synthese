
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

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "DateTime.h"

#include "ReservationTransaction.h"
#include "ReservationTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace time;

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
				"t044_reservations"
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
			SQLiteTableSync::Index(ReservationTableSync::COL_LINE_ID.c_str(), ""),
			SQLiteTableSync::Index(ReservationTableSync::COL_DEPARTURE_PLACE_ID.c_str(), ""),
			SQLiteTableSync::Index(ReservationTableSync::COL_ARRIVAL_PLACE_ID.c_str(), ""),
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
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToString(object->getTransaction()->getKey())
				<< "," << Conversion::ToString(object->getLineId())
				<< "," << Conversion::ToSQLiteString(object->getLineCode())
				<< "," << Conversion::ToString(object->getServiceId())
				<< "," << Conversion::ToSQLiteString(object->getServiceCode())
				<< "," << Conversion::ToString(object->getDeparturePlaceId())
				<< "," << Conversion::ToSQLiteString(object->getDeparturePlaceName())
				<< "," << object->getDepartureTime().toSQLString()
				<< "," << Conversion::ToString(object->getArrivalPlaceId())
				<< "," << Conversion::ToSQLiteString(object->getArrivalPlaceName())
				<< "," << object->getArrivalTime().toSQLString()
				<< "," << Conversion::ToString(object->getReservationRuleId())
				<< "," << object->getOriginDateTime().toSQLString()
				<< "," << object->getReservationDeadLine().toSQLString()
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		ReservationTableSync::ReservationTableSync()
			: SQLiteNoSyncTableSyncTemplate<ReservationTableSync,Reservation>()
		{
		}

		void ReservationTableSync::Search(
			Env& env,
			RegistryKeyType transactionId
			, int first /*= 0*/
			, int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (transactionId != UNKNOWN_VALUE)
			{
				query << COL_TRANSACTION_ID << "=" << transactionId;
			}
			query << " ORDER BY " << COL_DEPARTURE_TIME;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}

		ReservationTableSync::~ReservationTableSync()
		{

		}
	}
}
