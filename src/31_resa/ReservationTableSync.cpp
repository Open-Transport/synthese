
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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTableSync.h"

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

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<ReservationTableSync>::TABLE_NAME = "t044_reservations";
		template<> const int SQLiteTableSyncTemplate<ReservationTableSync>::TABLE_ID = 44;
		template<> const bool SQLiteTableSyncTemplate<ReservationTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<ReservationTableSync,Reservation>::Load(
			Reservation* object
			, const db::SQLiteResultSPtr& rows,
			Env* env,
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

		template<> void SQLiteDirectTableSyncTemplate<ReservationTableSync,Reservation>::Unlink(Reservation* object, Env * env)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<ReservationTableSync,Reservation>::Save(Reservation* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == UNKNOWN_VALUE)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
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

		ReservationTableSync::ReservationTableSync()
			: SQLiteNoSyncTableSyncTemplate<ReservationTableSync,Reservation>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_TRANSACTION_ID, "INTEGER");
			addTableColumn(COL_LINE_ID, "INTEGER");
			addTableColumn(COL_LINE_CODE, "TEXT");
			addTableColumn(COL_SERVICE_ID, "INTEGER");
			addTableColumn(COL_SERVICE_CODE, "TEXT");
			addTableColumn(COL_DEPARTURE_PLACE_ID, "INTEGER");
			addTableColumn(COL_DEPARTURE_PLACE_NAME, "TEXT");
			addTableColumn(COL_DEPARTURE_TIME, "TIMESTAMP");
			addTableColumn(COL_ARRIVAL_PLACE_ID, "INTEGER");
			addTableColumn(COL_ARRIVAL_PLACE_NAME, "TEXT");
			addTableColumn(COL_ARRIVAL_TIME, "TIMESTAMP");
			addTableColumn(COL_RESERVATION_RULE_ID, "INTEGER");
			addTableColumn(COL_ORIGIN_DATE_TIME, "TIMESTAMP");
			addTableColumn(COL_RESERVATION_DEAD_LINE, "TIMESTAMP");
			
			addTableIndex(COL_LINE_ID);
			addTableIndex(COL_DEPARTURE_PLACE_ID);
			addTableIndex(COL_ARRIVAL_PLACE_ID);
		}

		void ReservationTableSync::Search(
			Env& env,
			ReservationTransaction* transaction
			, int first /*= 0*/
			, int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				<< COL_TRANSACTION_ID << "=" << Conversion::ToString(transaction->getKey());
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
