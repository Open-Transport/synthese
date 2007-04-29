
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
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTableSync.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Reservation>::TABLE_NAME = "t044_reservations";
		template<> const int SQLiteTableSyncTemplate<Reservation>::TABLE_ID = 44;
		template<> const bool SQLiteTableSyncTemplate<Reservation>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Reservation>::load(Reservation* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setLineId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_LINE_ID)));
			object->setLineCode(rows.getColumn(rowId, ReservationTableSync::COL_LINE_CODE));
			object->setServiceCode(rows.getColumn(rowId, ReservationTableSync::COL_SERVICE_CODE));
			object->setDeparturePlaceId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_DEPARTURE_PLACE_ID)));
			object->setDeparturePlaceName(rows.getColumn(rowId, ReservationTableSync::COL_DEPARTURE_PLACE_NAME));
			object->setDepartureAddress(rows.getColumn(rowId, ReservationTableSync::COL_DEPARTURE_ADDRESS));
			object->setDepartureTime(DateTime::FromSQLTimestamp(rows.getColumn(rowId, ReservationTableSync::COL_DEPARTURE_TIME)));
			object->setArrivalPlaceId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_ARRIVAL_PLACE_ID)));
			object->setArrivalPlaceName(rows.getColumn(rowId, ReservationTableSync::COL_ARRIVAL_PLACE_NAME));
			object->setArrivalAddress(rows.getColumn(rowId, ReservationTableSync::COL_ARRIVAL_ADDRESS));
			object->setArrivalTime(DateTime::FromSQLTimestamp(rows.getColumn(rowId, ReservationTableSync::COL_ARRIVAL_TIME)));
			object->setReservationRuleId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_RESERVATION_RULE_ID)));
			object->setLastReservationId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_LAST_RESERVATION_ID)));
			object->setSeats(Conversion::ToInt(rows.getColumn(rowId, ReservationTableSync::COL_SEATS)));
			object->setBookingTime(DateTime::FromSQLTimeStamp(rows.getColumn(rowId, ReservationTableSync::COL_BOOKING_TIME)));
			object->setCancellationTime(DateTime::FromSQLTimeStamp(rows.getColumn(rowId, ReservationTableSync::COL_CANCELLATION_TIME)));
			object->setCustomerUserId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_CUSTOMER_ID)));
			object->setCustomerName(rows.getColumn(rowId, ReservationTableSync::COL_CUSTOMER_NAME));
			object->setCustomerPhone(rows.getColumn(rowId, ReservationTableSync::COL_CUSTOMER_PHONE));
			object->setBookingUserId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_BOOKING_USER_ID)));
			object->setCancelUserId(Conversion::ToLongLong(rows.getColumn(rowId, ReservationTableSync::COL_CANCEL_USER_ID)));
		}

		template<> void SQLiteTableSyncTemplate<Reservation>::save(Reservation* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId(1,1));	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToString(object->getLineId())
				<< "," << Conversion::ToSQLiteString(object->getLineCode())
				<< "," << Conversion::ToSQLiteString(object->getServiceCode())
				<< "," << Conversion::ToString(object->getDeparturePlaceId())
				<< "," << Conversion::ToSQLiteString(object->getDeparturePlaceName())
				<< "," << Conversion::ToSQLiteString(object->getDepartureAddress())
				<< "," << object->getDepartureTime().toSQLString()
				<< "," << Conversion::ToString(object->getArrivalPlaceId())
				<< "," << Conversion::ToSQLiteString(object->getArrivalPlaceName())
				<< "," << Conversion::ToSQLiteString(object->getArrivalAddress())
				<< "," << object->getArrivalTime().toSQLString()
				<< "," << Conversion::ToString(object->getReservationRuleId())
				<< "," << Conversion::ToString(object->getLastReservationId())
				<< "," << Conversion::ToString(object->getSeats())
				<< "," << object->getBookingTime().toSQLString()
				<< "," << object->getCancellationTime().toSQLString()
				<< "," << Conversion::ToString(object->getCustomerId())
				<< "," << Conversion::ToSQLiteString(object->getCustomerName())
				<< "," << Conversion::ToSQLiteString(object->getCustomerPhone())
				<< "," << Conversion::ToString(object->getBookingUserId())
				<< "," << Conversion::ToString(object->getCancelUserId())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		const string ReservationTableSync::COL_LINE_ID = "line_id";
		const string ReservationTableSync::COL_LINE_CODE = "line_code";
		const string ReservationTableSync::COL_SERVICE_CODE = "service_code";
		const string ReservationTableSync::COL_DEPARTURE_PLACE_ID = "departure_place_id";
		const string ReservationTableSync::COL_DEPARTURE_PLACE_NAME = "departure_place_name";
		const string ReservationTableSync::COL_DEPARTURE_ADDRESS = "departure_address";
		const string ReservationTableSync::COL_DEPARTURE_TIME = "departure_time";
		const string ReservationTableSync::COL_ARRIVAL_PLACE_ID = "arrival_place_id";
		const string ReservationTableSync::COL_ARRIVAL_PLACE_NAME = "arrival_place_name";
		const string ReservationTableSync::COL_ARRIVAL_ADDRESS = "arrival_address";
		const string ReservationTableSync::COL_ARRIVAL_TIME = "arrival_time";
		const string ReservationTableSync::COL_RESERVATION_RULE_ID = "reservation_rule_id";
		const string ReservationTableSync::COL_LAST_RESERVATION_ID = "last_reservation_id";
		const string ReservationTableSync::COL_SEATS = "seats";
		const string ReservationTableSync::COL_BOOKING_TIME = "booking_time";
		const string ReservationTableSync::COL_CANCELLATION_TIME = "cancellation_time";
		const string ReservationTableSync::COL_CUSTOMER_ID = "customer_id";
		const string ReservationTableSync::COL_CUSTOMER_NAME = "customer_name";
		const string ReservationTableSync::COL_CUSTOMER_PHONE = "customer_phone";
		const string ReservationTableSync::COL_BOOKING_USER_ID = "booking_user_id";
		const string ReservationTableSync::COL_CANCEL_USER_ID = "cancel_user_id";

		ReservationTableSync::ReservationTableSync()
			: SQLiteTableSyncTemplate<Reservation>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_LINE_ID, "INTEGER");
			addTableColumn(COL_LINE_CODE, "TEXT");
			addTableColumn(COL_SERVICE_CODE, "TEXT");
			addTableColumn(COL_DEPARTURE_PLACE_ID, "INTEGER");
			addTableColumn(COL_DEPARTURE_PLACE_NAME, "TEXT");
			addTableColumn(COL_DEPARTURE_ADDRESS, "TEXT");
			addTableColumn(COL_DEPARTURE_TIME, "TIMESTAMP");
			addTableColumn(COL_ARRIVAL_PLACE_ID, "INTEGER");
			addTableColumn(COL_ARRIVAL_PLACE_NAME, "TEXT");
			addTableColumn(COL_ARRIVAL_ADDRESS, "TEXT");
			addTableColumn(COL_ARRIVAL_TIME, "TIMESTAMP");
			addTableColumn(COL_RESERVATION_RULE_ID, "INTEGER");
			addTableColumn(COL_LAST_RESERVATION_ID, "INTEGER");
			addTableColumn(COL_SEATS, "INTEGER");
			addTableColumn(COL_BOOKING_TIME, "TIMESTAMP");
			addTableColumn(COL_CANCELLATION_TIME, "TIMESTAMP");
			addTableColumn(COL_CUSTOMER_ID, "INTEGER");
			addTableColumn(COL_CUSTOMER_NAME, "TEXT");
			addTableColumn(COL_CUSTOMER_PHONE, "TEXT");
			addTableColumn(COL_BOOKING_USER_ID, "INTEGER");
			addTableColumn(COL_CANCEL_USER_ID, "INTEGER");

			addTableIndex(COL_LINE_ID);
			addTableIndex(COL_CUSTOMER_ID);
			addTableIndex(COL_DEPARTURE_PLACE_ID);
			addTableIndex(COL_ARRIVAL_PLACE_ID);
		}

		void ReservationTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
		}
		
		void ReservationTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
		}

		void ReservationTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
		}

		std::vector<Reservation*> ReservationTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<Reservation*> objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					Reservation* object = new Reservation();
					load(object, result, i);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
