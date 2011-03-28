
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
#include "ReservationTableSync.h"
#include "ResaModule.h"
#include "ReplaceQuery.h"
#include "Service.h"

#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, ReservationTransactionTableSync>::FACTORY_KEY("31.2 Reservation Transaction Table Sync");
	}

	namespace resa
	{
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
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationTransactionTableSync>::TABLE(
			"t046_reservation_transactions", true
		);
		template<> const DBTableSync::Field DBTableSyncTemplate<ReservationTransactionTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTransactionTableSync::COL_LAST_RESERVATION_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTransactionTableSync::COL_SEATS, SQL_INTEGER),
			DBTableSync::Field(ReservationTransactionTableSync::COL_BOOKING_TIME, SQL_TIMESTAMP),
			DBTableSync::Field(ReservationTransactionTableSync::COL_CANCELLATION_TIME, SQL_TIMESTAMP),
			DBTableSync::Field(ReservationTransactionTableSync::COL_CUSTOMER_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTransactionTableSync::COL_CUSTOMER_NAME, SQL_TEXT),
			DBTableSync::Field(ReservationTransactionTableSync::COL_CUSTOMER_PHONE, SQL_TEXT),
			DBTableSync::Field(ReservationTransactionTableSync::COL_CUSTOMER_EMAIL, SQL_TEXT),
			DBTableSync::Field(ReservationTransactionTableSync::COL_BOOKING_USER_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTransactionTableSync::COL_CANCEL_USER_ID, SQL_INTEGER),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<ReservationTransactionTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>::Load(
			ReservationTransaction* object
			, const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setLastReservation(rows->getLongLong ( ReservationTransactionTableSync::COL_LAST_RESERVATION_ID));
			object->setSeats(rows->getInt( ReservationTransactionTableSync::COL_SEATS));
			object->setBookingTime(rows->getDateTime( ReservationTransactionTableSync::COL_BOOKING_TIME));
			object->setCancellationTime(rows->getDateTime( ReservationTransactionTableSync::COL_CANCELLATION_TIME));
			object->setCustomerUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CUSTOMER_ID));
			object->setCustomerName(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_NAME));
			object->setCustomerPhone(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_PHONE));
			object->setBookingUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_BOOKING_USER_ID));
			object->setCancelUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CANCEL_USER_ID));
			object->setCustomerEMail(rows->getText(ReservationTransactionTableSync::COL_CUSTOMER_EMAIL));
		}

		template<> void DBDirectTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>::Save(
			ReservationTransaction* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ReservationTransactionTableSync> query(*object);
			query.addField(object->getLastReservation());
			query.addField(object->getSeats());
			query.addField(object->getBookingTime());
			query.addField(object->getCancellationTime());
			query.addField(object->getCustomerUserId());
			query.addField(object->getCustomerName());
			query.addField(object->getCustomerPhone());
			query.addField(object->getCustomerEMail());
			query.addField(object->getBookingUserId());
			query.addField(object->getCancelUserId());
			query.execute(transaction);
		}

		template<> void  DBDirectTableSyncTemplate<ReservationTransactionTableSync,ReservationTransaction>::Unlink(
			ReservationTransaction* obj
		){

		}
	}

	namespace resa
	{
		ReservationTransactionTableSync::SearchResult ReservationTransactionTableSync::Search(
			Env& env,
			util::RegistryKeyType serviceId
			, const date& originDate
			, bool withCancelled
			, int first /*= 0*/
			, boost::optional<std::size_t> number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT " << TABLE.NAME << ".*"
				<< " FROM " << TABLE.NAME
				<< " INNER JOIN " << ReservationTableSync::TABLE.NAME << " AS r ON "
				<< " r." << ReservationTableSync::COL_TRANSACTION_ID << "=" << TABLE.NAME << "." << TABLE_COL_ID
				<< " WHERE " 
				<< " r." << ReservationTableSync::COL_SERVICE_ID << "=" << serviceId
				<< " AND r." << ReservationTableSync::COL_ORIGIN_DATE_TIME << ">='" << to_iso_extended_string(originDate) << " 00:00'"
				<< " AND r." << ReservationTableSync::COL_ORIGIN_DATE_TIME << "<='" << to_iso_extended_string(originDate) << " 23:59'";
			if (!withCancelled)
				query << " AND " << COL_CANCELLATION_TIME << " IS NULL";
			query << " GROUP BY " << TABLE.NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << ReservationTableSync::COL_DEPARTURE_TIME;
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}

		ReservationTransactionTableSync::SearchResult ReservationTransactionTableSync::Search(
			Env& env,
			boost::optional<RegistryKeyType> userId
			, const ptime& minDate
			, const ptime& maxDate
			, bool withCancelled
			, int first
			, boost::optional<std::size_t> number,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT " << TABLE.NAME << ".*"
				<< " FROM " << TABLE.NAME
				<< " INNER JOIN " << ReservationTableSync::TABLE.NAME << " AS r ON "
				<< " r." << ReservationTableSync::COL_TRANSACTION_ID << "=" << TABLE.NAME << "." << TABLE_COL_ID
				<< " WHERE " << COL_CANCEL_USER_ID << "=" << *userId;
			if (!minDate.is_not_a_date_time())
				query << " AND r." << ReservationTableSync::COL_DEPARTURE_TIME << ">='" << to_iso_extended_string(minDate.date()) << " " << to_simple_string(minDate.time_of_day()) << "'";
			if (!maxDate.is_not_a_date_time())
				query << " AND r." << ReservationTableSync::COL_DEPARTURE_TIME << "<='" << to_iso_extended_string(maxDate.date()) << " " << to_simple_string(maxDate.time_of_day()) << "'";
			if (!withCancelled)
				query << " AND " << COL_CANCELLATION_TIME << " IS NULL";
			query << " GROUP BY " << TABLE.NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << ReservationTableSync::COL_DEPARTURE_TIME << " DESC";
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
