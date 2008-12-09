
/** ReservationTransactionTableSync class implementation.
	@file ReservationTransactionTableSync.cpp

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

#include "ReservationTransactionTableSync.h"
#include "ReservationTransaction.h"
#include "31_resa/ReservationTableSync.h"
#include "31_resa/ResaModule.h"

#include "15_env/Service.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include <boost/foreach.hpp>

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
		template<> const string FactorableTemplate<SQLiteTableSync, ReservationTransactionTableSync>::FACTORY_KEY("31.2 Reservation Transaction Table Sync");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<ReservationTransactionTableSync>::TABLE_NAME("t046_reservation_transactions");
		template<> const int SQLiteTableSyncTemplate<ReservationTransactionTableSync>::TABLE_ID(46);
		template<> const bool SQLiteTableSyncTemplate<ReservationTransactionTableSync>::HAS_AUTO_INCREMENT(true);

		template<> void SQLiteDirectTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>::Load(
			ReservationTransaction* object
			, const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			object->setLastReservation(rows->getLongLong ( ReservationTransactionTableSync::COL_LAST_RESERVATION_ID));
			object->setSeats(rows->getInt( ReservationTransactionTableSync::COL_SEATS));
			object->setBookingTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTransactionTableSync::COL_BOOKING_TIME)));
			object->setCancellationTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTransactionTableSync::COL_CANCELLATION_TIME)));
			object->setCustomerUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CUSTOMER_ID));
			object->setCustomerName(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_NAME));
			object->setCustomerPhone(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_PHONE));
			object->setBookingUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_BOOKING_USER_ID));
			object->setCancelUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CANCEL_USER_ID));
			object->setCustomerEMail(rows->getText(ReservationTransactionTableSync::COL_CUSTOMER_EMAIL));

			if (linkLevel == DOWN_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL)
			{
				Env senv;
				ReservationTableSync::Search(senv, object);
				BOOST_FOREACH(shared_ptr<Reservation> reser, senv.template getRegistry<Reservation>())
				{
					object->addReservation(reser);
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>::Save(ReservationTransaction* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());

			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToString(object->getLastReservation())
				<< "," << Conversion::ToString(object->getSeats())
				<< "," << object->getBookingTime().toSQLString()
				<< "," << object->getCancellationTime().toSQLString()
				<< "," << Conversion::ToString(object->getCustomerUserId())
				<< "," << Conversion::ToSQLiteString(object->getCustomerName())
				<< "," << Conversion::ToSQLiteString(object->getCustomerPhone())
				<< "," << Conversion::ToSQLiteString(object->getCustomerEMail())
				<< "," << Conversion::ToString(object->getBookingUserId())
				<< "," << Conversion::ToString(object->getCancelUserId())
				<< ")";
			sqlite->execUpdate(query.str());
		}

		template<> void  SQLiteDirectTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>::Unlink(
			ReservationTransaction* obj,
			Env* env
		){

		}
	}

	namespace resa
	{
		// const std::string ReservationTransactionTableSync::COL_xx("xx");
		const string ReservationTransactionTableSync::COL_LAST_RESERVATION_ID = "last_reservation_id";
		const string ReservationTransactionTableSync::COL_SEATS = "seats";
		const string ReservationTransactionTableSync::COL_BOOKING_TIME = "booking_time";
		const string ReservationTransactionTableSync::COL_CANCELLATION_TIME = "cancellation_time";
		const string ReservationTransactionTableSync::COL_CUSTOMER_ID = "customer_id";
		const string ReservationTransactionTableSync::COL_CUSTOMER_NAME = "customer_name";
		const string ReservationTransactionTableSync::COL_CUSTOMER_PHONE = "customer_phone";
		const string ReservationTransactionTableSync::COL_CUSTOMER_EMAIL = "customer_email";
		const string ReservationTransactionTableSync::COL_BOOKING_USER_ID = "booking_user_id";
		const string ReservationTransactionTableSync::COL_CANCEL_USER_ID = "cancel_user_id";

		ReservationTransactionTableSync::ReservationTransactionTableSync()
			: SQLiteNoSyncTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_LAST_RESERVATION_ID, "INTEGER");
			addTableColumn(COL_SEATS, "INTEGER");
			addTableColumn(COL_BOOKING_TIME, "TIMESTAMP");
			addTableColumn(COL_CANCELLATION_TIME, "TIMESTAMP");
			addTableColumn(COL_CUSTOMER_ID, "INTEGER");
			addTableColumn(COL_CUSTOMER_NAME, "TEXT");
			addTableColumn(COL_CUSTOMER_PHONE, "TEXT");
			addTableColumn(COL_CUSTOMER_EMAIL, "TEXT");
			addTableColumn(COL_BOOKING_USER_ID, "INTEGER");
			addTableColumn(COL_CANCEL_USER_ID, "INTEGER");
		}

		void ReservationTransactionTableSync::Search(
			Env& env,
			const env::Service* service
			, const time::Date& originDate
			, bool withCancelled
			, int first /*= 0*/
			, int number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT " << TABLE_NAME << ".*"
				<< " FROM " << TABLE_NAME
				<< " INNER JOIN " << ReservationTableSync::TABLE_NAME << " AS r ON "
				<< " r." << ReservationTableSync::COL_TRANSACTION_ID << "=" << TABLE_NAME << "." << TABLE_COL_ID
				<< " WHERE " 
				<< " r." << ReservationTableSync::COL_SERVICE_ID << "=" << Conversion::ToString(service->getKey())
				<< " AND r." << ReservationTableSync::COL_ORIGIN_DATE_TIME << ">='" << originDate.toSQLString(false) << " 00:00'"
				<< " AND r." << ReservationTableSync::COL_ORIGIN_DATE_TIME << "<='" << originDate.toSQLString(false) << " 23:59'";
			if (!withCancelled)
				query << " AND " << COL_CANCELLATION_TIME << " IS NULL";
			query << " GROUP BY " << TABLE_NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << ReservationTableSync::COL_DEPARTURE_TIME;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}

		void ReservationTransactionTableSync::Search(
			Env& env,
			uid userId
			, const time::DateTime& minDate
			, const time::DateTime& maxDate
			, bool withCancelled
			, int first
			, int number,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT " << TABLE_NAME << ".*"
				<< " FROM " << TABLE_NAME
				<< " INNER JOIN " << ReservationTableSync::TABLE_NAME << " AS r ON "
				<< " r." << ReservationTableSync::COL_TRANSACTION_ID << "=" << TABLE_NAME << "." << TABLE_COL_ID
				<< " WHERE " << COL_CANCEL_USER_ID << "=" << userId;
			if (!minDate.isUnknown())
				query << " AND r." << ReservationTableSync::COL_DEPARTURE_TIME << ">=" << minDate.toSQLString();
			if (!maxDate.isUnknown())
				query << " AND r." << ReservationTableSync::COL_DEPARTURE_TIME << "<=" << maxDate.toSQLString();
			if (!withCancelled)
				query << " AND " << COL_CANCELLATION_TIME << " IS NULL";
			query << " GROUP BY " << TABLE_NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << ReservationTableSync::COL_DEPARTURE_TIME << " DESC";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}

		ReservationTransactionTableSync::~ReservationTransactionTableSync()
		{

		}
	}
}
