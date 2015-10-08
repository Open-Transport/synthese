
/** ReservationTableSync class implementation.
	@file ReservationTableSync.cpp

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

#include "ReservationTableSync.h"

#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "JourneyPatternTableSync.hpp"
#include "Profile.h"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "UserTableSync.h"
#include "VehicleTableSync.hpp"
#include "VehiclePositionTableSync.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace pt;
	using namespace vehicle;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ReservationTableSync>::FACTORY_KEY("31.1 Reservation Table Sync");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationTableSync>::TABLE(
			"t044_reservations", true
		);

		template<> const Field DBTableSyncTemplate<ReservationTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ReservationTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					LineId::FIELD.name.c_str(),
					OriginDateTime::FIELD.name.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					ServiceId::FIELD.name.c_str(),
					OriginDateTime::FIELD.name.c_str(),
			"")	);
			r.push_back(
				DBTableSync::Index(
					Transaction::FIELD.name.c_str(),
					DepartureTime::FIELD.name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<ReservationTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return false;
		}



		template<> void DBTableSyncTemplate<ReservationTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ReservationTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ReservationTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}



		template<>
		const bool ConditionalSynchronizationPolicy<ReservationTableSync, Reservation>::NEEDS_AUTO_RELOAD = true;



		template<>
		bool ConditionalSynchronizationPolicy<ReservationTableSync, Reservation>::IsLoaded(
			const DBResultSPtr& row
		){
			// Getting current time
			ptime now(second_clock::local_time());

			// Loading the reservations in a temporary environment
			Env env;
			ReservationTableSync::SearchResult resas(
				ReservationTableSync::Search(
					env,
					row->get<RegistryKeyType>(Transaction::FIELD.name)
			)	);

			// Checking if all reservations are too old to be cached
			if((*resas.rbegin())->get<ArrivalTime>() < (now - ReservationTransactionTableSync::BEFORE_RESERVATION_INDEXATION_DURATION))
			{
				return false;
			}

			// Checking if all reservations are too late to be cached
			return (*resas.begin())->get<DepartureTime>() < (now + ReservationTransactionTableSync::AFTER_RESERVATION_INDEXATION_DURATION);
		}



		template<>
		bool ConditionalSynchronizationPolicy<ReservationTableSync, Reservation>::IsLoaded(
			const Reservation& reservation
		){
			// Getting current time
			ptime now(second_clock::local_time());

			// Checking if all reservations are too old to be cached
			if (!(reservation.get<Transaction>()))
			{
				return false;
			}
			if((*reservation.get<Transaction>()->getReservations().rbegin())->get<ArrivalTime>() < (now - ReservationTransactionTableSync::BEFORE_RESERVATION_INDEXATION_DURATION))
			{
				return false;
			}

			// Checking if all reservations are too late to be cached
			return (*reservation.get<Transaction>()->getReservations().begin())->get<DepartureTime>() < (now + ReservationTransactionTableSync::AFTER_RESERVATION_INDEXATION_DURATION);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Generates the SQL expression filtering the record to load.
		template<>
		boost::shared_ptr<SQLExpression> ConditionalSynchronizationPolicy<ReservationTableSync, Reservation>::GetWhereLoaded()
		{

			// Getting current time
			ptime now(second_clock::local_time());

			// Bounds
			ptime minBound(now - ReservationTransactionTableSync::BEFORE_RESERVATION_INDEXATION_DURATION);
			ptime maxBound(now + ReservationTransactionTableSync::AFTER_RESERVATION_INDEXATION_DURATION);

			// The expression
			return
				ComposedExpression::Get(
					ComposedExpression::Get(
						FieldExpression::Get(
							ReservationTableSync::TABLE.NAME,
							DepartureTime::FIELD.name
						),
						ComposedExpression::OP_SUPEQ,
						ValueExpression<ptime>::Get(minBound)
					),
					ComposedExpression::OP_AND,
					ComposedExpression::Get(
						FieldExpression::Get(
							ReservationTableSync::TABLE.NAME,
							DepartureTime::FIELD.name
						),
						ComposedExpression::OP_INFEQ,
						ValueExpression<ptime>::Get(maxBound)
				)	)
			;
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
				<< Transaction::FIELD.name << "=" << transactionId
			;
			query << " ORDER BY " << DepartureTime::FIELD.name;
			if (number)
				query << " LIMIT " << lexical_cast<string>(*number + 1);
			if (first > 0)
				query << " OFFSET " << lexical_cast<string>(first);

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		ReservationTableSync::SearchResult ReservationTableSync::Search(
			util::Env& env,
			util::RegistryKeyType commercialLineId,
			const date& minDate,
			const date& maxDate,
			optional<string> serviceNumber,
			logic::tribool cancellations,
			bool orderByService,
			bool raisingOrder,
			int first /*= 0 */,
			boost::optional<std::size_t> number  /*= 0*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */,
			optional<RegistryKeyType> serviceId
		){
			stringstream query;
			query <<
				" SELECT " << TABLE.NAME << ".*" <<
				" FROM " << TABLE.NAME
			;
			if(!indeterminate(cancellations))
			{
				query << " INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " t ON t." << TABLE_COL_ID << "=" << TABLE.NAME << "." << Transaction::FIELD.name;
			}

			query <<
				" WHERE " <<
				TABLE.NAME << "." << LineId::FIELD.name << "=" << commercialLineId << " AND " <<
				TABLE.NAME << "." << OriginDateTime::FIELD.name << ">='" << to_iso_extended_string(minDate) << " 03:00' ";

			query <<
				" AND " << TABLE.NAME << "." << OriginDateTime::FIELD.name << "<'" << to_iso_extended_string(maxDate) << " 03:00'"
			;
			if(serviceNumber)
			{
				query << " AND " << TABLE.NAME << "." << ServiceCode::FIELD.name << "=\"" << *serviceNumber << "\"";
			}
			if(serviceId)
			{
				query << " AND " << TABLE.NAME << "." << ServiceId::FIELD.name << "=" << *serviceId;
			}
			if(!indeterminate(cancellations))
			{
				query << " AND t." << CancellationTime::FIELD.name << " IS " << (cancellations == true ? "NOT " : "") << " NULL";
			}

			if(orderByService)
			{
				query <<
					" ORDER BY " << TABLE.NAME << "." << ServiceCode::FIELD.name << " " << (raisingOrder ? "ASC" : "DESC");
			}
			else
			{
				query <<
					" ORDER BY " << TABLE.NAME << "." << OriginDateTime::FIELD.name << " " << (raisingOrder ? "ASC" : "DESC");
			}

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		ReservationTableSync::SearchResult ReservationTableSync::Search(
			util::Env& env,
			boost::posix_time::ptime minArrivalDate,
			boost::posix_time::ptime maxDepartureDate,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query <<
				"SELECT r.*" <<
				" FROM " << TABLE.NAME << " r INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " t" << " ON r." << Transaction::FIELD.name << "=t." << TABLE_COL_ID <<
				" WHERE " <<
				"(SELECT MAX(r2." << ArrivalTime::FIELD.name << ") FROM " << TABLE.NAME << " r2 WHERE r2." << Transaction::FIELD.name << "=t." << TABLE_COL_ID << ") > '" << boost::gregorian::to_iso_extended_string(minArrivalDate.date()) << " " << boost::posix_time::to_simple_string(minArrivalDate.time_of_day()) << "' AND " <<
				"(SELECT MIN(r3." << DepartureTime::FIELD.name << ") FROM " << TABLE.NAME << " r3 WHERE r3." << Transaction::FIELD.name << "=t." << TABLE_COL_ID << ") < '" << boost::gregorian::to_iso_extended_string(maxDepartureDate.date()) << " " << boost::posix_time::to_simple_string(maxDepartureDate.time_of_day()) << "' AND " <<
				"t." << CancellationTime::FIELD.name << " IS NULL "
				" ORDER BY t." << TABLE_COL_ID;
			return LoadFromQuery(query.str(), env, linkLevel);
		}



		ReservationTableSync::SearchResult ReservationTableSync::SearchByService(
			util::Env& env,
			util::RegistryKeyType serviceId,
			boost::optional<boost::posix_time::ptime> departureTime,
			boost::optional<boost::posix_time::ptime> arrivalTime,
			util::LinkLevel linkLevel
		){
			stringstream query;
			query <<
				"SELECT * " <<
				"FROM " << TABLE.NAME <<
				" WHERE " << ServiceId::FIELD.name << " = " << serviceId;

			if (departureTime)
				query <<
					" AND " << DepartureTime::FIELD.name << " = '" << boost::gregorian::to_iso_extended_string((*departureTime).date()) << " " << boost::posix_time::to_simple_string((*departureTime).time_of_day()) << "'";
			
			if (arrivalTime)
				query <<
					" AND " << ArrivalTime::FIELD.name << " = '" << boost::gregorian::to_iso_extended_string((*arrivalTime).date()) << " " << boost::posix_time::to_simple_string((*arrivalTime).time_of_day()) << "'";

			return LoadFromQuery(query.str(), env, linkLevel);
		}
		
		std::string ReservationTableSync::whereClauseDefault() const
		{
			stringstream where;
			ptime now(second_clock::local_time());
			where << DepartureTime::FIELD.name << " >= '" << boost::gregorian::to_iso_extended_string(now.date()) << "'";
			return where.str();
		}

		bool ReservationTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::READ);
		}
}	}
