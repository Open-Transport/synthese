
/** Reservation class implementation.
	@file 51_resa/Reservation.cpp

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

#include "Reservation.h"

#include "Language.hpp"
#include "Profile.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ReservationTransaction.h"
#include "UserTableSync.h"
#include "Vehicle.hpp"
#include "VehiclePosition.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace resa;
	using namespace security;
	using namespace util;

	CLASS_DEFINITION(Reservation, "t044_reservations", 44)
	FIELD_DEFINITION_OF_OBJECT(Reservation, "reservation_id", "reservation_ids")

	FIELD_DEFINITION_OF_TYPE(Transaction, "transaction_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LineId, "line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LineCode, "line_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(IsReservationPossible, "is_reservation_possible", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ServiceId, "service_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ServiceCode, "service_code", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DeparturePlaceId, "departure_place_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DepartureCityName, "departure_city_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DeparturePlaceNameNoCity, "departure_place_name_no_city", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DeparturePlaceName, "departure_place_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DepartureTime, "departure_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(ArrivalPlaceId, "arrival_place_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ArrivalCityName, "arrival_city_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ArrivalPlaceNameNoCity, "arrival_place_name_no_city", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ArrivalPlaceName, "arrival_place_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ArrivalTime, "arrival_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(ReservationRuleId, "reservation_rule_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(OriginDateTime, "origin_date_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(ReservationDeadLine, "reservation_dead_line", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(Vehicle, "vehicle_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(SeatNumber, "seat_number", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(VehiclePositionAtDeparture, "vehicle_position_id_at_departure", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(VehiclePositionAtArrival, "vehicle_position_id_at_arrival", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CancelledByOperator, "cancelled_by_operator", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(AcknowledgeTime, "acknowledge_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(AcknowledgeUser, "acknowledge_user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CancellationAcknowledgeTime, "cancellation_acknowledge_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(CancellationAcknowledgeUser, "cancellation_acknowledge_user_id", SQL_INTEGER)

	namespace resa
	{
		const string Reservation::DATA_COMMENT = "comment";
		const string Reservation::DATA_CLIENT_ID = "client_id";
		const string Reservation::DATA_LANGUAGE("language");
		const string Reservation::DATA_NAME("name");
		const string Reservation::DATA_PHONE = "phone";
		const string Reservation::DATA_EMAIL = "email";
		const string Reservation::DATA_ADDRESS = "address";
		const string Reservation::DATA_POSTCODE = "postcode";
		const string Reservation::DATA_CITYTEXT = "cityText";
		const string Reservation::DATA_COUNTRY = "country";
		const string Reservation::DATA_SEATS_NUMBER("seats_number");
		const string Reservation::DATA_RESERVATION_ID("reservation_id");
		const string Reservation::DATA_SEAT("seat");
		const string Reservation::DATA_SERVICE_NUMBER("service_number");
		const string Reservation::DATA_CANCELLATION_TIME("cancellation_time");
		const string Reservation::DATA_ACKNOWLEDGE_USER = "acknowledge_user";
		const string Reservation::DATA_BOOKING_USER_ID = "booking_user_id";
		const string Reservation::DATA_CANCELLATION_ACKNOWLEDGE_USER = "cancellation_acknowledge_user";
		const string Reservation::DATA_STATUS = "status";
		const string Reservation::DATA_FULL_TEXT = "full_text";
		const string Reservation::DATA_MINUTES_TO_DEAD_LINE = "minutes_to_dead_line";
		const string Reservation::DATA_MINUTES_TO_DEPARTURE = "minutes_to_departure";
		const string Reservation::DATA_MINUTES_DEAD_LINE_TO_DEPARTURE = "minutes_dead_line_to_departure";



		Reservation::Reservation(
			RegistryKeyType key
		):	Registrable(key),
			Object<Reservation, ReservationSchema> (
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Transaction),
					FIELD_VALUE_CONSTRUCTOR(LineId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(LineCode),
					FIELD_VALUE_CONSTRUCTOR(IsReservationPossible, false),
					FIELD_VALUE_CONSTRUCTOR(ServiceId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceCode),
					FIELD_VALUE_CONSTRUCTOR(DeparturePlaceId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(DepartureCityName),
					FIELD_DEFAULT_CONSTRUCTOR(DeparturePlaceNameNoCity),
					FIELD_DEFAULT_CONSTRUCTOR(DeparturePlaceName),
					FIELD_VALUE_CONSTRUCTOR(DepartureTime, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(ArrivalPlaceId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(ArrivalCityName),
					FIELD_DEFAULT_CONSTRUCTOR(ArrivalPlaceNameNoCity),
					FIELD_DEFAULT_CONSTRUCTOR(ArrivalPlaceName),
					FIELD_VALUE_CONSTRUCTOR(ArrivalTime, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(ReservationRuleId,0),
					FIELD_VALUE_CONSTRUCTOR(OriginDateTime, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(ReservationDeadLine, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(Vehicle),
					FIELD_DEFAULT_CONSTRUCTOR(SeatNumber),
					FIELD_DEFAULT_CONSTRUCTOR(VehiclePositionAtDeparture),
					FIELD_DEFAULT_CONSTRUCTOR(VehiclePositionAtArrival),
					FIELD_VALUE_CONSTRUCTOR(CancelledByOperator, false),
					FIELD_VALUE_CONSTRUCTOR(AcknowledgeTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(AcknowledgeUser),
					FIELD_VALUE_CONSTRUCTOR(CancellationAcknowledgeTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(CancellationAcknowledgeUser)
		)	)
		{ }

		Reservation::~Reservation()
		{
			unlink();
		}



		void Reservation::setTransaction(ReservationTransaction* transaction )
		{
			if(	get<Transaction>())
			{
				get<Transaction>()->removeReservation(*this);
			}
			set<Transaction>(transaction
				? boost::optional<ReservationTransaction&>(*transaction)
				: boost::none);
			if(transaction)
			{
				transaction->addReservation(*this);
			}
		}



		ReservationStatus Reservation::getStatus() const
		{
			if (get<ReservationRuleId>() == 0)
				return NO_RESERVATION;

			const ptime& cancellationTime(get<Transaction>()->get<CancellationTime>());
			const ptime now(second_clock::local_time());

			if (cancellationTime.is_not_a_date_time())
			{
				if (now < get<ReservationDeadLine>())
				{
					if(get<AcknowledgeTime>().is_not_a_date_time())
					{
						return OPTION;
					}
					else
					{
						return ACKNOWLEDGED_OPTION;
					}
				}
				if (now < get<DepartureTime>())
				{
					if(get<AcknowledgeTime>().is_not_a_date_time())
					{
						return TO_BE_DONE;
					}
					else
					{
						return ACKNOWLEDGED;
					}
				}
				if (now < get<ArrivalTime>())
				{
					if(get<VehiclePositionAtDeparture>())
					{
						return AT_WORK;
					}
					else
					{
						return SHOULD_BE_AT_WORK;
					}
				}
				if(get<VehiclePositionAtArrival>())
				{
					return DONE;
				}
				else
				{
					return SHOULD_BE_DONE;
				}
			}
			else
			{
				if (cancellationTime < get<ReservationDeadLine>())
				{
					if(!get<AcknowledgeTime>().is_not_a_date_time() && get<CancellationAcknowledgeTime>().is_not_a_date_time())
					{
						return CANCELLATION_TO_ACK;
					}
					else
					{
						return CANCELLED;
					}
				}
				if (cancellationTime < get<DepartureTime>())
				{
					if(get<CancellationAcknowledgeTime>().is_not_a_date_time())
					{
						return CANCELLED_AFTER_DELAY;
					}
					else
					{
						return ACKNOWLEDGED_CANCELLED_AFTER_DELAY;
					}
				}
				return NO_SHOW;
			}
		}



		std::string Reservation::getFullStatusText() const
		{
			ReservationStatus status(getStatus());
			string statusText(ResaModule::GetStatusText(status));

			switch(status)
			{
			case ACKNOWLEDGED_OPTION:
				statusText += " le " + to_simple_string(get<AcknowledgeTime>());
				if(get<AcknowledgeUser>())
				{
					statusText += " par " + get<AcknowledgeUser>()->getFullName();
				}

			case OPTION:
				statusText += " pouvant être annulée avant le " + to_simple_string(get<ReservationDeadLine>());
				break;

			case ACKNOWLEDGED:
				statusText += " le " + to_simple_string(get<AcknowledgeTime>());
				if(get<AcknowledgeUser>())
				{
					statusText += " par " + get<AcknowledgeUser>()->getFullName();
				}
				break;

			case CANCELLATION_TO_ACK:
			case CANCELLED_AFTER_DELAY:
				statusText += " le " + to_simple_string(get<Transaction>()->get<CancellationTime>());
				break;

			case CANCELLED:
			case ACKNOWLEDGED_CANCELLED_AFTER_DELAY:
				statusText += " le " + to_simple_string(get<Transaction>()->get<CancellationTime>());
				if(!get<CancellationAcknowledgeTime>().is_not_a_date_time())
				{
					statusText + " confirmé le "
						+ to_simple_string(get<CancellationAcknowledgeTime>());
					if(get<CancellationAcknowledgeUser>())
					{
						statusText += " par " + get<CancellationAcknowledgeUser>()->getFullName();
					}
				}
				break;

			case AT_WORK:
				statusText += " depuis le " + to_simple_string(get<VehiclePositionAtDeparture>()->getTime());
				break;

			case DONE:
				statusText += " le " + to_simple_string(get<VehiclePositionAtArrival>()->getTime());

			case NO_SHOW:
				statusText += " constatée le " + to_simple_string(get<Transaction>()->get<CancellationTime>());
				break;
				
			default:
				break;
			}

			return statusText;
		}



		void Reservation::toParametersMap(
			util::ParametersMap& pm,
			boost::optional<synthese::Language> language,
			std::string prefix /*= std::string() */
		) const	{

			// ID
			pm.insert(DATA_RESERVATION_ID, getKey());

			// Customer
			pm.insert(DATA_COMMENT, get<Transaction>()->get<Comment>());
			pm.insert(DATA_NAME, get<Transaction>()->get<CustomerName>());
			pm.insert(DATA_PHONE, get<Transaction>()->get<CustomerPhone>());
			try {
				pm.insert(DATA_CLIENT_ID, get<Transaction>()->get<Customer>()->getKey());
				pm.insert(DATA_EMAIL, get<Transaction>()->get<Customer>()->getEMail());
				pm.insert(DATA_ADDRESS, get<Transaction>()->get<Customer>()->getAddress());
				pm.insert(DATA_POSTCODE, get<Transaction>()->get<Customer>()->getPostCode());
				pm.insert(DATA_CITYTEXT, get<Transaction>()->get<Customer>()->getCityText());
				pm.insert(DATA_COUNTRY, get<Transaction>()->get<Customer>()->getCountry());
			}
			catch(ObjectNotFoundException<User>&)
			{
			}

			// Transaction
			pm.insert(Transaction::FIELD.name, get<Transaction>()->getKey());
			pm.insert(DATA_SEATS_NUMBER, get<Transaction>()->get<Seats>());
			pm.insert(DATA_BOOKING_USER_ID, get<Transaction>()->get<BookingUserId>());

			// Places
            pm.insert(DepartureCityName::FIELD.name, get<DepartureCityName>());
			pm.insert(DeparturePlaceNameNoCity::FIELD.name, get<DeparturePlaceNameNoCity>());
			pm.insert(DeparturePlaceName::FIELD.name, get<DeparturePlaceName>());
			pm.insert(ArrivalCityName::FIELD.name, get<ArrivalCityName>());
			pm.insert(ArrivalPlaceNameNoCity::FIELD.name, get<ArrivalPlaceNameNoCity>());
			pm.insert(ArrivalPlaceName::FIELD.name, get<ArrivalPlaceName>());
			pm.insert(DeparturePlaceId::FIELD.name, get<DeparturePlaceId>());
			pm.insert(ArrivalPlaceId::FIELD.name, get<ArrivalPlaceId>());

			// Service
			pm.insert(DATA_SERVICE_NUMBER, get<ServiceCode>());
			pm.insert(ServiceId::FIELD.name, get<ServiceId>());

			// Time
			ptime now(second_clock::local_time());
			pm.insert(DepartureTime::FIELD.name, get<DepartureTime>());
			pm.insert(ArrivalTime::FIELD.name, get<ArrivalTime>());
			pm.insert(ReservationDeadLine::FIELD.name, get<ReservationDeadLine>());
			pm.insert(DATA_MINUTES_DEAD_LINE_TO_DEPARTURE, (get<DepartureTime>() - get<ReservationDeadLine>()).total_seconds() / 60);
			pm.insert(DATA_MINUTES_TO_DEAD_LINE, (get<ReservationDeadLine>() - now).total_seconds() / 60);
			pm.insert(DATA_MINUTES_TO_DEPARTURE, (get<DepartureTime>() - now).total_seconds() / 60);

			// Driver acknowledge
			if(!get<AcknowledgeTime>().is_not_a_date_time())
			{
				pm.insert(AcknowledgeTime::FIELD.name, get<AcknowledgeTime>());
			}
			if(get<AcknowledgeUser>())
			{
				boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
				get<AcknowledgeUser>()->toParametersMap(*userPM, true);
				pm.insert(DATA_ACKNOWLEDGE_USER, userPM);
			}

			// Cancellation time
			if(!get<Transaction>()->get<CancellationTime>().is_not_a_date_time())
			{
				pm.insert(DATA_CANCELLATION_TIME, get<Transaction>()->get<CancellationTime>());
			}

			// Cancellation acknowledge
			if(!get<CancellationAcknowledgeTime>().is_not_a_date_time())
			{
				pm.insert(CancellationAcknowledgeTime::FIELD.name, get<CancellationAcknowledgeTime>());
			}
			if(get<CancellationAcknowledgeUser>())
			{
				boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
				get<CancellationAcknowledgeUser>()->toParametersMap(*userPM, true);
				pm.insert(DATA_CANCELLATION_ACKNOWLEDGE_USER, userPM);
			}

			// Vehicle
			if(get<Vehicle>())
			{
				pm.insert(Vehicle::FIELD.name, get<Vehicle>()->getKey());
			}
			pm.insert(DATA_SEAT, get<SeatNumber>());

			// Status
			boost::shared_ptr<ParametersMap> statusPM(new ParametersMap);
			statusPM->insert(DATA_STATUS, static_cast<int>(getStatus()));
			statusPM->insert(DATA_NAME, ResaModule::GetStatusText(getStatus()));
			statusPM->insert(DATA_FULL_TEXT, getFullStatusText());
			pm.insert(DATA_STATUS, statusPM);

			// Language
			if(language && get<Transaction>()->get<Customer>() && get<Transaction>()->get<Customer>()->getLanguage())
			{
				pm.insert(DATA_LANGUAGE, get<Transaction>()->get<Customer>()->getLanguage()->getName(*language));
			}
		}



		void Reservation::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			toParametersMap(pm, optional<synthese::Language>(), prefix);
		}

		void Reservation::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			if(	get<Transaction>())
			{
				get<Transaction>()->addReservation(*this);
			}
			// Indexation
			if(&env == &Env::GetOfficialEnv())
			{
				recursive_mutex::scoped_lock lock(ResaModule::GetReservationsByServiceMutex());
				if(get<Transaction>())
				{
					ResaModule::AddReservationByService(*this);
				}
			}
		}

		void Reservation::unlink()
		{
			// Indexation (at first to avoid use of incomplete reservations in other threads)
			ResaModule::RemoveReservationByService(*this);

			// Clean up transaction
			if(get<Transaction>())
			{
				get<Transaction>()->removeReservation(*this);
			}
		}

		bool Reservation::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::WRITE);
		}

		bool Reservation::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::WRITE);
		}

		bool Reservation::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::DELETE_RIGHT);
		}
}	}
