
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
#include "ResaModule.h"
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
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string Registry<resa::Reservation>::KEY("Reservation");
	}

	namespace resa
	{
		const string Reservation::DATA_COMMENT = "comment";
        const string Reservation::DATA_ARRIVAL_CITY_NAME("arrival_city_name");
        const string Reservation::DATA_ARRIVAL_PLACE_NAME_NO_CITY("arrival_place_name_no_city");
		const string Reservation::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
        const string Reservation::DATA_DEPARTURE_CITY_NAME("departure_city_name");
        const string Reservation::DATA_DEPARTURE_PLACE_NAME_NO_CITY("departure_place_name_no_city");
		const string Reservation::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string Reservation::DATA_ARRIVAL_PLACE_ID("arrival_place_id");
		const string Reservation::DATA_DEPARTURE_PLACE_ID("departure_place_id");
		const string Reservation::DATA_LANGUAGE("language");
		const string Reservation::DATA_NAME("name");
		const string Reservation::DATA_PHONE = "phone";
		const string Reservation::DATA_TRANSACTION_ID("transaction_id");
		const string Reservation::DATA_SEATS_NUMBER("seats_number");
		const string Reservation::DATA_VEHICLE_ID("vehicle_id");
		const string Reservation::DATA_RESERVATION_ID("reservation_id");
		const string Reservation::DATA_SEAT("seat");
		const string Reservation::DATA_SERVICE_NUMBER("service_number");
		const string Reservation::DATA_SERVICE_ID("service_id");
		const string Reservation::DATA_DEPARTURE_TIME("departure_time");
		const string Reservation::DATA_ARRIVAL_TIME("arrival_time");
		const string Reservation::DATA_CANCELLATION_TIME("cancellation_time");
		const string Reservation::DATA_ACKNOWLEDGE_TIME = "acknowledge_time";
		const string Reservation::DATA_ACKNOWLEDGE_USER = "acknowledge_user";
		const string Reservation::DATA_BOOKING_USER_ID = "booking_user_id";
		const string Reservation::DATA_CANCELLATION_ACKNOWLEDGE_TIME = "cancellation_acknowledge_time";
		const string Reservation::DATA_CANCELLATION_ACKNOWLEDGE_USER = "cancellation_acknowledge_user";
		const string Reservation::DATA_STATUS = "status";
		const string Reservation::DATA_FULL_TEXT = "full_text";
		const string Reservation::DATA_RESERVATION_DEAD_LINE = "reservation_dead_line";
		const string Reservation::DATA_MINUTES_TO_DEAD_LINE = "minutes_to_dead_line";
		const string Reservation::DATA_MINUTES_TO_DEPARTURE = "minutes_to_departure";
		const string Reservation::DATA_MINUTES_DEAD_LINE_TO_DEPARTURE = "minutes_dead_line_to_departure";



		Reservation::Reservation(
			RegistryKeyType key
		):	Registrable(key),
			_transaction(NULL),
			_lineId(0),
            _isReservationPossible(false),
			_serviceId(0),
			_departurePlaceId(0),
			_arrivalPlaceId(0),
			_reservationRuleId(0),
			_departureTime(not_a_date_time),
			_arrivalTime(not_a_date_time),
			_originDateTime(not_a_date_time),
			_reservationDeadLine(not_a_date_time),
			_vehicle(NULL),
			_vehiclePositionAtDeparture(NULL),
			_vehiclePositionAtArrival(NULL),
			_cancelledByOperator(false),
			_acknowledgeTime(not_a_date_time),
			_acknowledgeUser(NULL),
			_cancellationAcknowledgeTime(not_a_date_time),
			_cancellationAcknowledgeUser(NULL)
		{}



		void Reservation::setTransaction(ReservationTransaction* transaction )
		{
			if(	_transaction)
			{
				_transaction->removeReservation(*this);
			}
			_transaction = transaction;
			if(transaction)
			{
				transaction->addReservation(*this);
			}
		}



		ReservationStatus Reservation::getStatus() const
		{
			if (_reservationRuleId == 0)
				return NO_RESERVATION;

			const ptime& cancellationTime(getTransaction()->getCancellationTime());
			const ptime now(second_clock::local_time());

			if (cancellationTime.is_not_a_date_time())
			{
				if (now < _reservationDeadLine)
				{
					if(_acknowledgeTime.is_not_a_date_time())
					{
						return OPTION;
					}
					else
					{
						return ACKNOWLEDGED_OPTION;
					}
				}
				if (now < _departureTime)
				{
					if(_acknowledgeTime.is_not_a_date_time())
					{
						return TO_BE_DONE;
					}
					else
					{
						return ACKNOWLEDGED;
					}
				}
				if (now < _arrivalTime)
				{
					if(_vehiclePositionAtDeparture)
					{
						return AT_WORK;
					}
					else
					{
						return SHOULD_BE_AT_WORK;
					}
				}
				if(_vehiclePositionAtArrival)
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
				if (cancellationTime < _reservationDeadLine)
				{
					if(!_acknowledgeTime.is_not_a_date_time() && _cancellationAcknowledgeTime.is_not_a_date_time())
					{
						return CANCELLATION_TO_ACK;
					}
					else
					{
						return CANCELLED;
					}
				}
				if (cancellationTime < _departureTime)
				{
					if(_cancellationAcknowledgeTime.is_not_a_date_time())
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
				statusText += " le " + to_simple_string(_acknowledgeTime);
				if(_acknowledgeUser)
				{
					statusText += " par " + _acknowledgeUser->getFullName();
				}

			case OPTION:
				statusText += " pouvant être annulée avant le " + to_simple_string(_reservationDeadLine);
				break;

			case ACKNOWLEDGED:
				statusText += " le " + to_simple_string(_acknowledgeTime);
				if(_acknowledgeUser)
				{
					statusText += " par " + _acknowledgeUser->getFullName();
				}
				break;

			case CANCELLATION_TO_ACK:
			case CANCELLED_AFTER_DELAY:
				statusText += " le " + to_simple_string(getTransaction()->getCancellationTime());
				break;

			case CANCELLED:
			case ACKNOWLEDGED_CANCELLED_AFTER_DELAY:
				statusText += " le " + to_simple_string(getTransaction()->getCancellationTime());
				if(!_cancellationAcknowledgeTime.is_not_a_date_time())
				{
					statusText + " confirmé le "
						+ to_simple_string(_cancellationAcknowledgeTime);
					if(_cancellationAcknowledgeUser)
					{
						statusText += " par " + _cancellationAcknowledgeUser->getFullName();
					}
				}
				break;

			case AT_WORK:
				statusText += " depuis le " + to_simple_string(_vehiclePositionAtDeparture->getTime());
				break;

			case DONE:
				statusText += " le " + to_simple_string(_vehiclePositionAtArrival->getTime());

			case NO_SHOW:
				statusText += " constatée le " + to_simple_string(getTransaction()->getCancellationTime());
				break;
				
			default:
				break;
			}

			return statusText;
		}



		void Reservation::toParametersMap(
			util::ParametersMap& pm,
			boost::optional<Language> language,
			std::string prefix /*= std::string() */
		) const	{

			// ID
			pm.insert(DATA_RESERVATION_ID, getKey());

			// Customer
			pm.insert(DATA_COMMENT, getTransaction()->getComment());
			pm.insert(DATA_NAME, getTransaction()->getCustomerName());
			pm.insert(DATA_PHONE, getTransaction()->getCustomerPhone());

			// Transaction
			pm.insert(DATA_TRANSACTION_ID, getTransaction()->getKey());
			pm.insert(DATA_SEATS_NUMBER, getTransaction()->getSeats());
			pm.insert(DATA_BOOKING_USER_ID, getTransaction()->getBookingUserId());

			// Places
            pm.insert(DATA_DEPARTURE_CITY_NAME, getDepartureCityName());
            pm.insert(DATA_DEPARTURE_PLACE_NAME_NO_CITY, getDeparturePlaceNameNoCity());
			pm.insert(DATA_DEPARTURE_PLACE_NAME, getDeparturePlaceName());
            pm.insert(DATA_ARRIVAL_CITY_NAME, getArrivalCityName());
            pm.insert(DATA_ARRIVAL_PLACE_NAME_NO_CITY, getArrivalPlaceNameNoCity());
			pm.insert(DATA_ARRIVAL_PLACE_NAME, getArrivalPlaceName());
			pm.insert(DATA_DEPARTURE_PLACE_ID, getDeparturePlaceId());
			pm.insert(DATA_ARRIVAL_PLACE_ID, getArrivalPlaceId());

			// Service
			pm.insert(DATA_SERVICE_NUMBER, getServiceCode());
			pm.insert(DATA_SERVICE_ID, getServiceId());

			// Time
			ptime now(second_clock::local_time());
			pm.insert(DATA_DEPARTURE_TIME, getDepartureTime());
			pm.insert(DATA_ARRIVAL_TIME, getArrivalTime());
			pm.insert(DATA_RESERVATION_DEAD_LINE, _reservationDeadLine);
			pm.insert(DATA_MINUTES_DEAD_LINE_TO_DEPARTURE, (_departureTime - _reservationDeadLine).total_seconds() / 60);
			pm.insert(DATA_MINUTES_TO_DEAD_LINE, (_reservationDeadLine - now).total_seconds() / 60);
			pm.insert(DATA_MINUTES_TO_DEPARTURE, (_departureTime - now).total_seconds() / 60);

			// Driver acknowledge
			if(!_acknowledgeTime.is_not_a_date_time())
			{
				pm.insert(DATA_ACKNOWLEDGE_TIME, getAcknowledgeTime());
			}
			if(_acknowledgeUser)
			{
				boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
				_acknowledgeUser->toParametersMap(*userPM, true);
				pm.insert(DATA_ACKNOWLEDGE_USER, userPM);
			}

			// Cancellation time
			if(!getTransaction()->getCancellationTime().is_not_a_date_time())
			{
				pm.insert(DATA_CANCELLATION_TIME, getTransaction()->getCancellationTime());
			}

			// Cancellation acknowledge
			if(!_cancellationAcknowledgeTime.is_not_a_date_time())
			{
				pm.insert(DATA_CANCELLATION_ACKNOWLEDGE_TIME, getCancellationAcknowledgeTime());
			}
			if(_cancellationAcknowledgeUser)
			{
				boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
				_cancellationAcknowledgeUser->toParametersMap(*userPM, true);
				pm.insert(DATA_CANCELLATION_ACKNOWLEDGE_USER, userPM);
			}

			// Vehicle
			if(getVehicle())
			{
				pm.insert(DATA_VEHICLE_ID, getVehicle()->getKey());
			}
			pm.insert(DATA_SEAT, getSeatNumber());

			// Status
			boost::shared_ptr<ParametersMap> statusPM(new ParametersMap);
			statusPM->insert(DATA_STATUS, static_cast<int>(getStatus()));
			statusPM->insert(DATA_NAME, ResaModule::GetStatusText(getStatus()));
			statusPM->insert(DATA_FULL_TEXT, getFullStatusText());
			pm.insert(DATA_STATUS, statusPM);

			// Language
			const User* user(getTransaction()->getCustomer());
			if(language && user && user->getLanguage())
			{
				pm.insert(DATA_LANGUAGE, user->getLanguage()->getName(*language));
			}
		}



		void Reservation::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			toParametersMap(pm, optional<Language>(), prefix);
		}
}	}
