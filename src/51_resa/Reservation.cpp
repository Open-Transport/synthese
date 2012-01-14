
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
#include "ReservationTransaction.h"
#include "ResaModule.h"
#include "Registry.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<resa::Reservation>::KEY("Reservation");
	}

	namespace resa
	{
		Reservation::Reservation(
			RegistryKeyType key
		):	Registrable(key),
			_transaction(NULL),
			_departureTime(not_a_date_time),
			_arrivalTime(not_a_date_time),
			_originDateTime(not_a_date_time),
			_reservationDeadLine(not_a_date_time),
			_reservationRuleId(0),
			_vehicle(NULL),
			_vehiclePositionAtDeparture(NULL),
			_vehiclePositionAtArrival(NULL),
			_cancelledByOperator(false)
		{}



		void Reservation::setTransaction(ReservationTransaction* transaction )
		{
			_transaction = transaction;
			if(transaction)
			{
				transaction->addReservation(this);
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
}	}
