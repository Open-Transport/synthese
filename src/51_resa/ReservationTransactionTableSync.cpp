
/** ReservationTransactionTableSync class implementation.
	@file ReservationTransactionTableSync.cpp

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

#include "ReservationTransactionTableSync.h"

#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "ResaModule.h"
#include "ReservationTableSync.h"
#include "Service.h"
#include "SQLSingleOperatorExpression.hpp"
#include "UserTableSync.h"

#include <sstream>
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
	using namespace security;


	namespace util
	{
		template<>
		const string FactorableTemplate<DBTableSync, ReservationTransactionTableSync>::FACTORY_KEY = "31.2 Reservation Transaction Table Sync";
	}

	namespace resa
	{
		const time_duration ReservationTransactionTableSync::BEFORE_RESERVATION_INDEXATION_DURATION = hours(1);
		const time_duration ReservationTransactionTableSync::AFTER_RESERVATION_INDEXATION_DURATION = hours(6);

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
		const string ReservationTransactionTableSync::COL_COMMENT = "comment";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationTransactionTableSync>::TABLE(
			"t046_reservation_transactions", true
		);

		template<> const Field DBTableSyncTemplate<ReservationTransactionTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ReservationTransactionTableSync::COL_LAST_RESERVATION_ID, SQL_INTEGER),
			Field(ReservationTransactionTableSync::COL_SEATS, SQL_INTEGER),
			Field(ReservationTransactionTableSync::COL_BOOKING_TIME, SQL_DATETIME),
			Field(ReservationTransactionTableSync::COL_CANCELLATION_TIME, SQL_DATETIME),
			Field(ReservationTransactionTableSync::COL_CUSTOMER_ID, SQL_INTEGER),
			Field(ReservationTransactionTableSync::COL_CUSTOMER_NAME, SQL_TEXT),
			Field(ReservationTransactionTableSync::COL_CUSTOMER_PHONE, SQL_TEXT),
			Field(ReservationTransactionTableSync::COL_CUSTOMER_EMAIL, SQL_TEXT),
			Field(ReservationTransactionTableSync::COL_BOOKING_USER_ID, SQL_INTEGER),
			Field(ReservationTransactionTableSync::COL_CANCEL_USER_ID, SQL_INTEGER),
			Field(ReservationTransactionTableSync::COL_COMMENT, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ReservationTransactionTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<>
		void OldLoadSavePolicy<ReservationTransactionTableSync,ReservationTransaction>::Load(
			ReservationTransaction* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setLastReservation(rows->getLongLong ( ReservationTransactionTableSync::COL_LAST_RESERVATION_ID));
			object->setSeats(rows->getInt( ReservationTransactionTableSync::COL_SEATS));
			object->setBookingTime(rows->getDateTime( ReservationTransactionTableSync::COL_BOOKING_TIME));
			object->setCancellationTime(rows->getDateTime( ReservationTransactionTableSync::COL_CANCELLATION_TIME));
			object->setComment(rows->getText(ReservationTransactionTableSync::COL_COMMENT));

			// Customer user
			object->setCustomerUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CUSTOMER_ID));
			object->setCustomer(NULL);
			if(	linkLevel > FIELDS_ONLY_LOAD_LEVEL &&
				object->getCustomerUserId()
			) try {
				object->setCustomer(
					UserTableSync::GetEditable(object->getCustomerUserId(), env, linkLevel).get()
				);
			}
			catch(ObjectNotFoundException<User>&)
			{
				// Not bad if the user does not exist anymore
			}

			object->setCustomerName(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_NAME));
			object->setCustomerPhone(rows->getText ( ReservationTransactionTableSync::COL_CUSTOMER_PHONE));
			object->setBookingUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_BOOKING_USER_ID));
			object->setCancelUserId(rows->getLongLong ( ReservationTransactionTableSync::COL_CANCEL_USER_ID));
			object->setCustomerEMail(rows->getText(ReservationTransactionTableSync::COL_CUSTOMER_EMAIL));

			// Indexation
			if( linkLevel == DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				ReservationTableSync::Search(env, object->getKey());
			}
		}



		template<>
		void OldLoadSavePolicy<ReservationTransactionTableSync,ReservationTransaction>::Save(
			ReservationTransaction* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ReservationTransactionTableSync> query(*object);
			query.addField(object->getLastReservation());
			query.addField(object->getSeats());
			query.addFrameworkField<PtimeField>(object->getBookingTime());
			query.addFrameworkField<PtimeField>(object->getCancellationTime());
			query.addField(object->getCustomerUserId());
			query.addField(object->getCustomerName());
			query.addField(object->getCustomerPhone());
			query.addField(object->getCustomerEMail());
			query.addField(object->getBookingUserId());
			query.addField(object->getCancelUserId());
			query.addField(object->getComment());
			query.execute(transaction);
		}



		template<>
		void OldLoadSavePolicy<ReservationTransactionTableSync,ReservationTransaction>::Unlink(
			ReservationTransaction* obj
		){
		}



		template<> bool DBTableSyncTemplate<ReservationTransactionTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return false;
		}



		template<> void DBTableSyncTemplate<ReservationTransactionTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ReservationTransactionTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ReservationTransactionTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}



		template<>
		const bool ConditionalSynchronizationPolicy<ReservationTransactionTableSync, ReservationTransaction>::NEEDS_AUTO_RELOAD = true;



		template<>
		bool ConditionalSynchronizationPolicy<ReservationTransactionTableSync, ReservationTransaction>::IsLoaded(
			const DBResultSPtr& row
		){
			// Getting current time
			ptime now(second_clock::local_time());

			// Loading the reservations in a temporary environment
			Env env;
			ReservationTableSync::SearchResult resas(
				ReservationTableSync::Search(
					env,
					row->get<RegistryKeyType>(TABLE_COL_ID)
			)	);

			// Empty transaction can not be cached
			if(resas.empty())
			{
				return false;
			}

			// Checking if all reservations are too old to be cached
			if((*resas.rbegin())->getArrivalTime() < (now - ReservationTransactionTableSync::BEFORE_RESERVATION_INDEXATION_DURATION))
			{
				return false;
			}

			// Checking if all reservations are too late to be cached
			return (*resas.begin())->getDepartureTime() < (now + ReservationTransactionTableSync::AFTER_RESERVATION_INDEXATION_DURATION);
		}



		template<>
		bool ConditionalSynchronizationPolicy<ReservationTransactionTableSync, ReservationTransaction>::IsLoaded(
			const ReservationTransaction& transaction
		){
			// Non empty transactions are kept
			return !transaction.getReservations().empty();
		}



		//////////////////////////////////////////////////////////////////////////
		/// Generates the SQL expression filtering the record to load.
		template<>
		boost::shared_ptr<SQLExpression> ConditionalSynchronizationPolicy<ReservationTransactionTableSync, ReservationTransaction>::GetWhereLoaded()
		{
			// No massive load (reservation transactions are loaded on demand by their reservations)
			return ValueExpression<int>::Get(0);
		}
	}

	namespace resa
	{
		ReservationTransactionTableSync::SearchResult ReservationTransactionTableSync::Search(
			Env& env,
			util::RegistryKeyType serviceId,
			const date& originDate,
			bool withCancelled,
			int first, /*= 0*/
			boost::optional<std::size_t> number, /*= 0*/
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



		ReservationTransactionTableSync::SearchResult ReservationTransactionTableSync::SearchByUser(
			Env& env,
			boost::optional<RegistryKeyType> userId
			, const ptime& minDate
			, const ptime& maxDate
			, bool withCancelled,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT " << TABLE.NAME << ".*"
				<< " FROM " << TABLE.NAME
				<< " INNER JOIN " << ReservationTableSync::TABLE.NAME << " AS r ON "
				<< " r." << ReservationTableSync::COL_TRANSACTION_ID << "=" << TABLE.NAME << "." << TABLE_COL_ID
				<< " WHERE " << COL_CUSTOMER_ID << "=" << *userId;
			if (!minDate.is_not_a_date_time())
				query << " AND " << TABLE.NAME << "." << COL_BOOKING_TIME << ">='" << to_iso_extended_string(minDate.date()) << " " << to_simple_string(minDate.time_of_day()) << "'";
			if (!maxDate.is_not_a_date_time())
				query << " AND " << TABLE.NAME << "." << COL_BOOKING_TIME << "<='" << to_iso_extended_string(maxDate.date()) << " " << to_simple_string(maxDate.time_of_day()) << "'";
			if (!withCancelled)
				query << " AND " << COL_CANCELLATION_TIME << " IS NULL";
			query << " GROUP BY " << TABLE.NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << ReservationTableSync::COL_DEPARTURE_TIME << " DESC";

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
