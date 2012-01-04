
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

#include <sstream>

#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"
#include "ReservationTransaction.h"
#include "ReservationTableSync.h"
#include "ReservationTransactionTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "VehicleTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "ResaModule.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
	using namespace pt_operation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,ReservationTableSync>::FACTORY_KEY("31.1 Reservation Table Sync");
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
		const string ReservationTableSync::COL_VEHICLE_ID("vehicle_id");
		const string ReservationTableSync::COL_SEAT_NUMBER("seat_number");
		const string ReservationTableSync::COL_VEHICLE_POSITION_ID_AT_DEPARTURE("vehicle_position_id_at_departure");
		const string ReservationTableSync::COL_VEHICLE_POSITION_ID_AT_ARRIVAL("vehicle_position_id_at_arrival");
		const string ReservationTableSync::COL_CANCELLED_BY_OPERATOR("cancelled_by_operator");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationTableSync>::TABLE(
			"t044_reservations", true
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<ReservationTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_TRANSACTION_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_LINE_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_LINE_CODE, SQL_TEXT),
			DBTableSync::Field(ReservationTableSync::COL_SERVICE_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_SERVICE_CODE, SQL_TEXT),
			DBTableSync::Field(ReservationTableSync::COL_DEPARTURE_PLACE_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_DEPARTURE_PLACE_NAME, SQL_TEXT),
			DBTableSync::Field(ReservationTableSync::COL_DEPARTURE_TIME, SQL_DATETIME),
			DBTableSync::Field(ReservationTableSync::COL_ARRIVAL_PLACE_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_ARRIVAL_PLACE_NAME, SQL_TEXT),
			DBTableSync::Field(ReservationTableSync::COL_ARRIVAL_TIME, SQL_DATETIME),
			DBTableSync::Field(ReservationTableSync::COL_RESERVATION_RULE_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_ORIGIN_DATE_TIME, SQL_DATETIME),
			DBTableSync::Field(ReservationTableSync::COL_RESERVATION_DEAD_LINE, SQL_DATETIME),
			DBTableSync::Field(ReservationTableSync::COL_VEHICLE_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_SEAT_NUMBER, SQL_TEXT),
			DBTableSync::Field(ReservationTableSync::COL_VEHICLE_POSITION_ID_AT_DEPARTURE, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_VEHICLE_POSITION_ID_AT_ARRIVAL, SQL_INTEGER),
			DBTableSync::Field(ReservationTableSync::COL_CANCELLED_BY_OPERATOR, SQL_BOOLEAN),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<ReservationTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				ReservationTableSync::COL_SERVICE_ID.c_str(),
				ReservationTableSync::COL_ORIGIN_DATE_TIME.c_str(),
			""),
			DBTableSync::Index(
				ReservationTableSync::COL_TRANSACTION_ID.c_str(),
				ReservationTableSync::COL_DEPARTURE_TIME.c_str(),
			""),
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<ReservationTableSync,Reservation>::Load(
			Reservation* object
			, const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setLineId(rows->getLongLong ( ReservationTableSync::COL_LINE_ID));
			object->setLineCode(rows->getText ( ReservationTableSync::COL_LINE_CODE));
			object->setServiceId(rows->getLongLong(ReservationTableSync::COL_SERVICE_ID));
			object->setServiceCode(rows->getText ( ReservationTableSync::COL_SERVICE_CODE));
			object->setDeparturePlaceId(rows->getLongLong ( ReservationTableSync::COL_DEPARTURE_PLACE_ID));
			object->setDeparturePlaceName(rows->getText ( ReservationTableSync::COL_DEPARTURE_PLACE_NAME));
			object->setDepartureTime(rows->getDateTime( ReservationTableSync::COL_DEPARTURE_TIME));
			object->setArrivalPlaceId(rows->getLongLong ( ReservationTableSync::COL_ARRIVAL_PLACE_ID));
			object->setArrivalPlaceName(rows->getText ( ReservationTableSync::COL_ARRIVAL_PLACE_NAME));
			object->setArrivalTime(rows->getDateTime( ReservationTableSync::COL_ARRIVAL_TIME));
			object->setReservationRuleId(rows->getLongLong ( ReservationTableSync::COL_RESERVATION_RULE_ID));
			object->setOriginDateTime(rows->getDateTime( ReservationTableSync::COL_ORIGIN_DATE_TIME));
			object->setReservationDeadLine(rows->getDateTime( ReservationTableSync::COL_RESERVATION_DEAD_LINE));
			object->setSeatNumber(rows->getText(ReservationTableSync::COL_SEAT_NUMBER));
			object->setCancelledByOperator(rows->getBool(ReservationTableSync::COL_CANCELLED_BY_OPERATOR));

			if(linkLevel == UP_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL)
			{
				// Vehicle
				object->setVehicle(NULL);
				RegistryKeyType vehicleId(rows->getLongLong(ReservationTableSync::COL_VEHICLE_ID));
				if(vehicleId > 0)
				{
					try
					{
						object->setVehicle(
							VehicleTableSync::Get(vehicleId, env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<Vehicle>&)
					{
						util::Log::GetInstance().warn("No such vehicle "+ lexical_cast<string>(vehicleId) +" in reservation "+ lexical_cast<string>(object->getKey()));
					}
				}

				// Vehicle position at departure
				object->setVehiclePositionAtDeparture(NULL);
				RegistryKeyType vpd(rows->getLongLong(ReservationTableSync::COL_VEHICLE_POSITION_ID_AT_DEPARTURE));
				if(vpd > 0)
				{
					try
					{
						object->setVehiclePositionAtDeparture(
							VehiclePositionTableSync::GetEditable(vpd, env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<VehiclePosition>&)
					{
						util::Log::GetInstance().warn("No such departure vehicle position "+ lexical_cast<string>(vpd) +" in reservation "+ lexical_cast<string>(object->getKey()));
					}
				}

				// Vehicle position at arrival
				object->setVehiclePositionAtArrival(NULL);
				RegistryKeyType vpa(rows->getLongLong(ReservationTableSync::COL_VEHICLE_POSITION_ID_AT_ARRIVAL));
				if(vpa > 0)
				{
					try
					{
						object->setVehiclePositionAtArrival(
							VehiclePositionTableSync::GetEditable(vpa, env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<VehiclePosition>&)
					{
						util::Log::GetInstance().warn("No such arrival vehicle position "+ lexical_cast<string>(vpa) +" in reservation "+ lexical_cast<string>(object->getKey()));
					}
				}

				// Keep this call at last position to be avoid the registration of the reservation in the transaction in case of
				// unhanded exception thrown by the previous links
				object->setTransaction(NULL);
				RegistryKeyType transactionId(rows->getLongLong(ReservationTableSync::COL_TRANSACTION_ID));
				try
				{
					object->setTransaction(
						ReservationTransactionTableSync::GetEditable(
							transactionId,
							env, linkLevel
						).get()
					);
				}
				catch(ObjectNotFoundException<ReservationTransaction>&)
				{
					util::Log::GetInstance().warn("No such transaction "+ lexical_cast<string>(transactionId) +" in reservation "+ lexical_cast<string>(object->getKey()));
				}
			}

			// Indexation
			if(linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				recursive_mutex::scoped_lock lock(ResaModule::GetReservationsByServiceMutex());
				if(object->getTransaction() && ResaModule::MustBeIndexed(*object->getTransaction()))
				{
					ResaModule::AddReservationByService(*object);
				}
			}
		}



		template<> void DBDirectTableSyncTemplate<ReservationTableSync,Reservation>::Unlink(Reservation* object)
		{
			// TODO : update transaction

			// Indexation
			ResaModule::RemoveReservationByService(*object);
		}



		template<> void DBDirectTableSyncTemplate<ReservationTableSync,Reservation>::Save(
			Reservation* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ReservationTableSync> query(*object);
			query.addField(object->getTransaction() ? object->getTransaction()->getKey() : RegistryKeyType(0));
			query.addField(object->getLineId());
			query.addField(object->getLineCode());
			query.addField(object->getServiceId());
			query.addField(object->getServiceCode());
			query.addField(object->getDeparturePlaceId());
			query.addField(object->getDeparturePlaceName());
			query.addField(object->getDepartureTime());
			query.addField(object->getArrivalPlaceId());
			query.addField(object->getArrivalPlaceName());
			query.addField(object->getArrivalTime());
			query.addField(object->getReservationRuleId());
			query.addField(object->getOriginDateTime());
			query.addField(object->getReservationDeadLine());
			query.addField(object->getVehicle() ? object->getVehicle()->getKey() : RegistryKeyType(0));
			query.addField(object->getSeatNumber());
			query.addField(object->getVehiclePositionAtDeparture() ? object->getVehiclePositionAtDeparture()->getKey() : RegistryKeyType(0));
			query.addField(object->getVehiclePositionAtArrival() ? object->getVehiclePositionAtArrival()->getKey() : RegistryKeyType(0));
			query.addField(object->getCancelledByOperator());
			query.execute(transaction);
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
			util::RegistryKeyType commercialLineId,
			const date& day,
			optional<string> serviceNumber,
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
				" INNER JOIN " << JourneyPatternTableSync::TABLE.NAME <<
					" l ON l." << TABLE_COL_ID << "=s." << ScheduledServiceTableSync::COL_PATHID
			;
			if(!indeterminate(cancellations))
			{
				query << " INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " t ON t." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_TRANSACTION_ID;
			}
			query <<
				" WHERE " <<
				"l." << JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID << "=" << commercialLineId << " AND " <<
				TABLE.NAME << "." << COL_ORIGIN_DATE_TIME << ">='" << to_iso_extended_string(day) << " 03:00' ";
			date dayp(day);
			dayp += days(1);
			query <<
				" AND " << TABLE.NAME << "." << COL_ORIGIN_DATE_TIME << "<'" << to_iso_extended_string(dayp) << " 03:00'"
			;
			if(serviceNumber)
			{
				query << " AND s." << ScheduledServiceTableSync::COL_SERVICENUMBER << "=\"" << *serviceNumber << "\"";
			}
			if(!indeterminate(cancellations))
			{
				query << " AND t." << ReservationTransactionTableSync::COL_CANCELLATION_TIME << " IS " << (cancellations == true ? "NOT " : "") << " NULL";
			}

			if(hideOldServices)
			{
				ptime now(second_clock::local_time());
				now -= hours(1);
				time_duration snow(now.time_of_day());
				if(snow <= time_duration(3,0,0))
				{
					snow += hours(24);
				}
				query <<
					" AND s." << ScheduledServiceTableSync::COL_SCHEDULES << ">='00:00:00#00:" <<
					to_simple_string(snow).substr(0, 5) << "'" ;
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



		ReservationTableSync::SearchResult ReservationTableSync::Search(
			util::Env& env,
			boost::posix_time::ptime minArrivalDate,
			boost::posix_time::ptime maxDepartureDate,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			stringstream query;
			query <<
				"SELECT r.*" <<
				" FROM " << TABLE.NAME << " r INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " t" << " ON r." << COL_TRANSACTION_ID << "=t." << TABLE_COL_ID <<
				" WHERE " <<
				"(SELECT MAX(r2." << COL_ARRIVAL_TIME << ") FROM " << TABLE.NAME << " r2 WHERE r2." << COL_TRANSACTION_ID << "=t." << TABLE_COL_ID << ") > '" << boost::gregorian::to_iso_extended_string(minArrivalDate.date()) << " " << boost::posix_time::to_simple_string(minArrivalDate.time_of_day()) << "' AND " <<
				"(SELECT MIN(r3." << COL_DEPARTURE_TIME << ") FROM " << TABLE.NAME << " r3 WHERE r3." << COL_TRANSACTION_ID << "=t." << TABLE_COL_ID << ") < '" << boost::gregorian::to_iso_extended_string(maxDepartureDate.date()) << " " << boost::posix_time::to_simple_string(maxDepartureDate.time_of_day()) << "' AND " <<
				"t." << ReservationTransactionTableSync::COL_CANCELLATION_TIME << " IS NULL "
				" ORDER BY t." << TABLE_COL_ID;
			return LoadFromQuery(query.str(), env, linkLevel);
		}
}	}
