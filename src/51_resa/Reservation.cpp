
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
		const string Reservation::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
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
		const string Reservation::DATA_CANCELLATION_ACKNOWLEDGE_TIME = "cancellation_acknowledge_time";
		const string Reservation::DATA_CANCELLATION_ACKNOWLEDGE_USER = "cancellation_acknowledge_user";



		Reservation::Reservation(
			RegistryKeyType key
		):	Registrable(key),
			_transaction(NULL),
			_lineId(0),
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
					return AT_WORK;
				}
				return DONE;
			}
			else
			{
				if (cancellationTime < _reservationDeadLine)
				{
					return CANCELLED;
				}
				if (cancellationTime < _departureTime)
				{
					return CANCELLED_AFTER_DELAY;
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

			case CANCELLED: return statusText + " le " + to_simple_string(getTransaction()->getCancellationTime());
			case CANCELLED_AFTER_DELAY: return statusText + " le " + to_simple_string(getTransaction()->getCancellationTime());
			case NO_SHOW: return statusText + " constatée le " + to_simple_string(getTransaction()->getCancellationTime());
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

			// Places
			pm.insert(DATA_DEPARTURE_PLACE_NAME, getDeparturePlaceName());
			pm.insert(DATA_ARRIVAL_PLACE_NAME, getArrivalPlaceName());
			pm.insert(DATA_DEPARTURE_PLACE_ID, getDeparturePlaceId());
			pm.insert(DATA_ARRIVAL_PLACE_ID, getArrivalPlaceId());

			// Service
			pm.insert(DATA_SERVICE_NUMBER, getServiceCode());
			pm.insert(DATA_SERVICE_ID, getServiceId());

			// Time
			pm.insert(DATA_DEPARTURE_TIME, getDepartureTime());
			pm.insert(DATA_ARRIVAL_TIME, getArrivalTime());

			// Driver acknowledge
			if(!_acknowledgeTime.is_not_a_date_time())
			{
				pm.insert(DATA_ACKNOWLEDGE_TIME, getAcknowledgeTime());
			}
			if(_acknowledgeUser)
			{
				shared_ptr<ParametersMap> userPM(new ParametersMap);
				_acknowledgeUser->toParametersMap(*userPM);
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
				shared_ptr<ParametersMap> userPM(new ParametersMap);
				_cancellationAcknowledgeUser->toParametersMap(*userPM);
				pm.insert(DATA_CANCELLATION_ACKNOWLEDGE_USER, userPM);
			}

			// Vehicle
			if(getVehicle())
			{
				pm.insert(DATA_VEHICLE_ID, getVehicle()->getKey());
			}
			pm.insert(DATA_SEAT, getSeatNumber());

			// Language
			const User* user(getTransaction()->getCustomer());
			if(language && user && user->getLanguage())
			{
				pm.insert(DATA_LANGUAGE, user->getLanguage()->getName(*language));
			}
		}
}	}
