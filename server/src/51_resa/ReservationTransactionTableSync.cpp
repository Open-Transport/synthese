
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
#include "Profile.h"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "ResaModule.h"
#include "ResaRight.h"
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
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationTransactionTableSync>::TABLE(
			"t046_reservation_transactions", true
		);

		template<> const Field DBTableSyncTemplate<ReservationTransactionTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ReservationTransactionTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
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
			if((*resas.rbegin())->get<ArrivalTime>() < (now - ReservationTransactionTableSync::BEFORE_RESERVATION_INDEXATION_DURATION))
			{
				return false;
			}

			// Checking if all reservations are too late to be cached
			return (*resas.begin())->get<DepartureTime>() < (now + ReservationTransactionTableSync::AFTER_RESERVATION_INDEXATION_DURATION);
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
				<< " r." << Transaction::FIELD.name << "=" << TABLE.NAME << "." << TABLE_COL_ID
				<< " WHERE "
				<< " r." << ServiceId::FIELD.name << "=" << serviceId
				<< " AND r." << OriginDateTime::FIELD.name << ">='" << to_iso_extended_string(originDate) << " 00:00'"
				<< " AND r." << OriginDateTime::FIELD.name << "<='" << to_iso_extended_string(originDate) << " 23:59'";
			if (!withCancelled)
				query << " AND " << CancellationTime::FIELD.name << " IS NULL";
			query << " GROUP BY " << TABLE.NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << DepartureTime::FIELD.name;
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
				<< " r." << Transaction::FIELD.name << "=" << TABLE.NAME << "." << TABLE_COL_ID
				<< " WHERE " << CancelUserId::FIELD.name << "=" << *userId;
			if (!minDate.is_not_a_date_time())
				query << " AND r." << DepartureTime::FIELD.name << ">='" << to_iso_extended_string(minDate.date()) << " " << to_simple_string(minDate.time_of_day()) << "'";
			if (!maxDate.is_not_a_date_time())
				query << " AND r." << DepartureTime::FIELD.name << "<='" << to_iso_extended_string(maxDate.date()) << " " << to_simple_string(maxDate.time_of_day()) << "'";
			if (!withCancelled)
				query << " AND " << CancellationTime::FIELD.name << " IS NULL";
			query << " GROUP BY " << TABLE.NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << DepartureTime::FIELD.name << " DESC";
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
				<< " r." << Transaction::FIELD.name << "=" << TABLE.NAME << "." << TABLE_COL_ID
				<< " WHERE " << Customer::FIELD.name << "=" << *userId;
			if (!minDate.is_not_a_date_time())
				query << " AND " << TABLE.NAME << "." << BookingTime::FIELD.name << ">='" << to_iso_extended_string(minDate.date()) << " " << to_simple_string(minDate.time_of_day()) << "'";
			if (!maxDate.is_not_a_date_time())
				query << " AND " << TABLE.NAME << "." << BookingTime::FIELD.name << "<='" << to_iso_extended_string(maxDate.date()) << " " << to_simple_string(maxDate.time_of_day()) << "'";
			if (!withCancelled)
				query << " AND " << CancellationTime::FIELD.name << " IS NULL";
			query << " GROUP BY " << TABLE.NAME << "." << TABLE_COL_ID;
			query << " ORDER BY " << DepartureTime::FIELD.name << " DESC";

			return LoadFromQuery(query.str(), env, linkLevel);
		}
		
		std::string ReservationTransactionTableSync::whereClauseDefault() const
		{
			stringstream where;
			ptime now(second_clock::local_time());
			where << TABLE_COL_ID << " in (SELECT " << Transaction::FIELD.name <<
				" FROM " << ReservationTableSync::TABLE.NAME <<
				" WHERE " << DepartureTime::FIELD.name << " >= '" << boost::gregorian::to_iso_extended_string(now.date()) <<
				"'')";
			return where.str();
		}

		bool ReservationTransactionTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::READ);
		}
	}
}
